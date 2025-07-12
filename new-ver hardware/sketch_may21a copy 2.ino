#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include "HeThongVuon.h"
#include "HienThi.h"
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// === CẤU HÌNH PHẦN CỨNG ===
#define MCP_ADDRESS 0x20
#define MCP_SDA_PIN 21
#define MCP_SCL_PIN 22

const int SO_HANG = 2;
const int SO_COT = 2;
const int TONG_SO_LUONG = SO_HANG * SO_COT;

// Cấu hình chân GPIO
const int chanDHT[SO_HANG][SO_COT] = {{32, 33}, {25, 26}};
const int chanDatAm[SO_HANG][SO_COT] = {{34, 35}, {36, 39}};
const int chanAnhSang[SO_HANG][SO_COT] = {{27, 14}, {12, 13}};
const int chanBomNuoc[SO_HANG][SO_COT] = {{0, 1}, {2, 3}};
const int chanDen[SO_HANG][SO_COT] = {{4, 5}, {6, 7}};
const int chanQuat[SO_HANG][SO_COT] = {{8, 9}, {10, 11}};

// === CẤU HÌNH WIFI & FIREBASE ===
#define WIFI_SSID "Nga Liem"
#define WIFI_PASSWORD "tinuanoi"
#define API_KEY "AIzaSyCa-0haXGQ1j620ppeOm9HV5Ltdw-JeF6g"
#define DATABASE_URL "https://smart-garden-64cd4-default-rtdb.firebaseio.com/"
#define USER_EMAIL "minhhieupham981@gmail.com"
#define USER_PASSWORD "21092109"

// === CẤU HÌNH TIMING ===
const unsigned long THOI_GIAN_DOC_CAM_BIEN = 2000;     // 2s
const unsigned long THOI_GIAN_CAP_NHAT_GUI = 5000;     // 5s
const unsigned long THOI_GIAN_CAP_NHAT_MAN_HINH = 500; // 0.5s
const unsigned long THOI_GIAN_KIEM_TRA_MODE = 3000;    // 3s
const unsigned long THOI_GIAN_DIEU_KHIEN = 1000;       // 1s

// === BIẾN TOÀN CỤC ===
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool firebaseReady = false;

Adafruit_MCP23X17 mcp;
HienThi manHinh(SO_HANG, SO_COT);

// State management
typedef enum
{
  TRANG_THAI_KHOI_DONG,
  TRANG_THAI_HOAT_DONG,
  TRANG_THAI_LOI
} TrangThaiHeThong;

TrangThaiHeThong trangThaiHienTai = TRANG_THAI_KHOI_DONG;
bool cheDoTuDong = true;

// Timers
unsigned long thoiGianDocCamBien = 0;
unsigned long thoiGianCapNhatGui = 0;
unsigned long thoiGianCapNhatManHinh = 0;
unsigned long thoiGianKiemTraMode = 0;
unsigned long thoiGianDieuKhien = 0;

// Data caching để giảm Firebase calls
struct DuLieuCamBien
{
  float nhietDo;
  float doAm;
  float anhSang;
  bool bomBat;
  bool denBat;
  bool quatBat;
  bool coCapNhat;
};

DuLieuCamBien duLieuCu[SO_HANG][SO_COT];
DuLieuCamBien duLieuMoi[SO_HANG][SO_COT];

// === FUNCTIONS DECLARATION ===
void khoiTaoHeThong();
void khoiTaoWiFi();
void khoiTaoFirebase();
void docCamBien();
void dieuKhienThietBi();
void capNhatFirebase();
void capNhatManHinh();
void kiemTraMode();
void xuLyLoi();
String layGardenId(int hang, int cot);
bool coThayDoiDuLieu(int hang, int cot);

// === MAIN FUNCTIONS ===
void setup()
{
  Serial.begin(115200);
  Serial.println("=== KHOI DONG HE THONG ===");

  khoiTaoHeThong();
  trangThaiHienTai = TRANG_THAI_HOAT_DONG;
  Serial.println("He thong san sang");
}

void loop()
{
  unsigned long thoiGian = millis();

  switch (trangThaiHienTai)
  {
  case TRANG_THAI_HOAT_DONG:
    // 1. Kiểm tra chế độ hoạt động
    if (thoiGian - thoiGianKiemTraMode >= THOI_GIAN_KIEM_TRA_MODE)
    {
      thoiGianKiemTraMode = thoiGian;
      kiemTraMode();
    }

    // 2. Đọc dữ liệu cảm biến
    if (thoiGian - thoiGianDocCamBien >= THOI_GIAN_DOC_CAM_BIEN)
    {
      thoiGianDocCamBien = thoiGian;
      docCamBien();
    }

    // 3. Điều khiển thiết bị
    if (thoiGian - thoiGianDieuKhien >= THOI_GIAN_DIEU_KHIEN)
    {
      thoiGianDieuKhien = thoiGian;
      dieuKhienThietBi();
    }

    // 4. Cập nhật Firebase
    if (thoiGian - thoiGianCapNhatGui >= THOI_GIAN_CAP_NHAT_GUI)
    {
      thoiGianCapNhatGui = thoiGian;
      capNhatFirebase();
    }

    // 5. Cập nhật màn hình
    if (thoiGian - thoiGianCapNhatManHinh >= THOI_GIAN_CAP_NHAT_MAN_HINH)
    {
      thoiGianCapNhatManHinh = thoiGian;
      capNhatManHinh();
    }
    break;

  case TRANG_THAI_LOI:
    xuLyLoi();
    break;

  default:
    trangThaiHienTai = TRANG_THAI_LOI;
    break;
  }
}

// === IMPLEMENTATION ===
void khoiTaoHeThong()
{
  // Khởi tạo kết nối
  khoiTaoWiFi();
  khoiTaoFirebase();

  // Khởi tạo I2C và MCP23017
  Wire.begin(MCP_SDA_PIN, MCP_SCL_PIN);
  if (!mcp.begin_I2C(MCP_ADDRESS))
  {
    Serial.println("Loi khoi tao MCP23017");
    trangThaiHienTai = TRANG_THAI_LOI;
    return;
  }
  Serial.println("MCP23017 khoi tao thanh cong");

  // Khởi tạo màn hình và hệ thống
  manHinh.ganThongTinPhanCung(&mcp, chanDHT, chanDatAm, chanAnhSang, chanBomNuoc, chanDen, chanQuat);

  // Khởi tạo các luống cây
  for (int i = 0; i < SO_HANG; i++)
  {
    for (int j = 0; j < SO_COT; j++)
    {
      manHinh.thietLapLuongCay(i, j, nullptr);

      // Khởi tạo dữ liệu cache
      duLieuCu[i][j] = {-999, -999, -999, false, false, false, false};
      duLieuMoi[i][j] = {-999, -999, -999, false, false, false, false};
    }
  }

  manHinh.veGiaoDien();
  manHinh.datCheDoHienTai(CHE_DO_AUTO);
}

void khoiTaoWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Đang kết nối WiFi");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\n✅ Ket noi WiFi thanh cong");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nKet noi WiFi that bai");
  }
}

void khoiTaoFirebase()
{
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.reconnectWiFi(true);
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);

  Serial.print("Dang ket noi Firebase");
  unsigned long startTime = millis();
  while (!Firebase.ready() && millis() - startTime < 15000)
  {
    delay(500);
    Serial.print(".");
  }

  firebaseReady = Firebase.ready();
  if (firebaseReady)
  {
    Serial.println("\n✅ Firebase kết nối thành công");
  }
  else
  {
    Serial.println("\n⚠️ Firebase kết nối thất bại - tiếp tục offline");
  }
}

void kiemTraMode()
{
  if (!firebaseReady)
    return;

  static bool cheDoTruoc = true;
  bool cheDoHienTai = true;

  if (Firebase.RTDB.getString(&fbdo, "currentMode"))
  {
    String mode = fbdo.stringData();
    cheDoHienTai = (mode == "auto");

    if (cheDoHienTai != cheDoTruoc)
    {
      cheDoTuDong = cheDoHienTai;
      manHinh.datCheDoHienTai(cheDoTuDong ? CHE_DO_AUTO : CHE_DO_MANUAL);
      Serial.printf("Chuyen che do: %s\n", cheDoTuDong ? "AUTO" : "MANUAL");
      cheDoTruoc = cheDoHienTai;
    }
  }
}

void docCamBien()
{
  static int luongHienTai = 0;

  // Đọc từng luống một cách tuần tự để giảm tải
  int hang = luongHienTai / SO_COT;
  int cot = luongHienTai % SO_COT;

  HeThongVuon *cay = manHinh.layHeThongVuon(hang, cot);
  if (cay != nullptr)
  {
    cay->docDuLieuCamBien();

    // Cập nhật dữ liệu mới
    duLieuMoi[hang][cot].nhietDo = cay->layNhietDo();
    duLieuMoi[hang][cot].doAm = cay->layDoAmDat();
    duLieuMoi[hang][cot].anhSang = cay->layAnhSang();
    duLieuMoi[hang][cot].bomBat = cay->layTrangThaiBom();
    duLieuMoi[hang][cot].denBat = cay->layTrangThaiDen();
    duLieuMoi[hang][cot].quatBat = cay->layTrangThaiQuat();
    duLieuMoi[hang][cot].coCapNhat = true;
  }

  // Chuyển sang luống tiếp theo
  luongHienTai = (luongHienTai + 1) % TONG_SO_LUONG;
}

void dieuKhienThietBi()
{
  if (cheDoTuDong)
  {
    // Chế độ tự động
    for (int i = 0; i < SO_HANG; i++)
    {
      for (int j = 0; j < SO_COT; j++)
      {
        HeThongVuon *cay = manHinh.layHeThongVuon(i, j);
        if (cay != nullptr)
        {
          cay->capNhat();
        }
      }
    }
  }
  else
  {
    // Chế độ thủ công - đọc từ Firebase
    if (!firebaseReady)
      return;

    static int luongDieuKhien = 0;
    int hang = luongDieuKhien / SO_COT;
    int cot = luongDieuKhien % SO_COT;

    HeThongVuon *cay = manHinh.layHeThongVuon(hang, cot);
    if (cay != nullptr)
    {
      String gardenId = layGardenId(hang, cot);

      // Đọc trạng thái từ Firebase
      if (Firebase.RTDB.getInt(&fbdo, gardenId + "/mayBom"))
      {
        cay->datTrangThaiBom(fbdo.intData() == 1);
      }
      if (Firebase.RTDB.getInt(&fbdo, gardenId + "/den"))
      {
        cay->datTrangThaiDen(fbdo.intData() == 1);
      }
      if (Firebase.RTDB.getInt(&fbdo, gardenId + "/quat"))
      {
        cay->datTrangThaiQuat(fbdo.intData() == 1);
      }
    }

    luongDieuKhien = (luongDieuKhien + 1) % TONG_SO_LUONG;
  }
}

void capNhatFirebase()
{
  if (!firebaseReady)
    return;

  for (int i = 0; i < SO_HANG; i++)
  {
    for (int j = 0; j < SO_COT; j++)
    {
      if (!duLieuMoi[i][j].coCapNhat)
        continue;

      if (coThayDoiDuLieu(i, j))
      {
        String gardenId = layGardenId(i, j);

        // Gửi dữ liệu cảm biến
        Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtNhietDo", duLieuMoi[i][j].nhietDo);
        Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtDoAm", duLieuMoi[i][j].doAm);
        Firebase.RTDB.setFloat(&fbdo, gardenId + "/anhSang", duLieuMoi[i][j].anhSang);

        // Gửi trạng thái thiết bị
        Firebase.RTDB.setInt(&fbdo, gardenId + "/mayBom", duLieuMoi[i][j].bomBat ? 1 : 0);
        Firebase.RTDB.setInt(&fbdo, gardenId + "/den", duLieuMoi[i][j].denBat ? 1 : 0);
        Firebase.RTDB.setInt(&fbdo, gardenId + "/quat", duLieuMoi[i][j].quatBat ? 1 : 0);

        // Cập nhật cache
        duLieuCu[i][j] = duLieuMoi[i][j];

        Serial.printf("Gui du lieu luong [%d][%d]: T=%.1f°C, H=%.1f%%, L=%.0f\n",
                      i, j, duLieuMoi[i][j].nhietDo, duLieuMoi[i][j].doAm, duLieuMoi[i][j].anhSang);
      }

      duLieuMoi[i][j].coCapNhat = false;
    }
  }
}

void capNhatManHinh()
{
  manHinh.capNhatHienThi();
  manHinh.capNhatTrangThaiNut();

  // Cập nhật luống được chọn
  int hang, cot;
  manHinh.layLuongDangChon(hang, cot);
  HeThongVuon *cay = manHinh.layHeThongVuon(hang, cot);

  if (cay != nullptr && cheDoTuDong)
  {
    cay->capNhat();
  }
}

void xuLyLoi()
{
  Serial.println("He thong gap loi, khoi dong lai...");
  delay(5000);
  ESP.restart();
}

// === HELPER FUNCTIONS ===
String layGardenId(int hang, int cot)
{
  return "gardenId" + String(hang * SO_COT + cot + 1);
}

bool coThayDoiDuLieu(int hang, int cot)
{
  const float NGUONG_NHIET_DO = 0.5;
  const float NGUONG_DO_AM = 1.0;
  const float NGUONG_ANH_SANG = 50.0;

  DuLieuCamBien &cu = duLieuCu[hang][cot];
  DuLieuCamBien &moi = duLieuMoi[hang][cot];

  return (abs(moi.nhietDo - cu.nhietDo) > NGUONG_NHIET_DO) ||
         (abs(moi.doAm - cu.doAm) > NGUONG_DO_AM) ||
         (abs(moi.anhSang - cu.anhSang) > NGUONG_ANH_SANG) ||
         (moi.bomBat != cu.bomBat) ||
         (moi.denBat != cu.denBat) ||
         (moi.quatBat != cu.quatBat);
}