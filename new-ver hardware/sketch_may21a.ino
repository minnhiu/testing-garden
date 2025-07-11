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

// Cấu hình MCP23017
#define MCP_ADDRESS 0x20
#define MCP_SDA_PIN 21
#define MCP_SCL_PIN 22

const int SO_HANG = 2;
const int SO_COT = 2;

const int chanDHT[SO_HANG][SO_COT] = {{32, 33}, {25, 26}};
const int chanDatAm[SO_HANG][SO_COT] = {{34, 35}, {36, 39}};
const int chanAnhSang[SO_HANG][SO_COT] = {{27, 14}, {12, 13}};
const int chanBomNuoc[SO_HANG][SO_COT] = {{0, 1}, {2, 3}};
const int chanDen[SO_HANG][SO_COT] = {{4, 5}, {6, 7}};
const int chanQuat[SO_HANG][SO_COT] = {{8, 9}, {10, 11}};

// Firebase cấu hình
#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"
#define API_KEY "AIzaSyCa-0haXGQ1j620ppeOm9HV5Ltdw-JeF6g"
#define DATABASE_URL "https://smart-garden-64cd4-default-rtdb.firebaseio.com/"
#define USER_EMAIL "minhhieupham981@gmail.com"
#define USER_PASSWORD "21092109"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool firebaseReady = false;

// Màn hình & MCP
Adafruit_MCP23X17 mcp;
HienThi manHinh(SO_HANG, SO_COT);

// Timer
unsigned long lanCapNhatCamBienCuoi = 0;
unsigned long lanCapNhatManHinhCuoi = 0;
const unsigned long THOI_GIAN_CAP_NHAT_CAM_BIEN = 1000;
const unsigned long THOI_GIAN_CAP_NHAT_MAN_HINH = 1000;

// Kết nối WiFi
void connectToWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

// Kết nối Firebase
void connectToFirebase()
{
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  firebaseReady = Firebase.ready();

  Serial.println(firebaseReady ? "Firebase ready" : "Firebase failed");
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Khoi dong he thong...");

  connectToWiFi();
  connectToFirebase();

  Wire.begin(MCP_SDA_PIN, MCP_SCL_PIN);
  if (!mcp.begin_I2C(MCP_ADDRESS))
  {
    Serial.println("Loi MCP23017");
    while (true)
      ;
  }

  Serial.println("MCP23017 OK");

  manHinh.ganThongTinPhanCung(&mcp, chanDHT, chanDatAm, chanAnhSang, chanBomNuoc, chanDen, chanQuat);

  for (int i = 0; i < SO_HANG; i++)
  {
    for (int j = 0; j < SO_COT; j++)
    {
      manHinh.thietLapLuongCay(i, j, nullptr); // chưa có cây ban đầu
      Serial.printf("Khoi tao luong [%d][%d]\n", i, j);
    }
  }

  manHinh.veGiaoDien();
  Serial.println("San sang.");
}

void loop()
{
  unsigned long thoiGian = millis();

  // 1. Đọc chế độ hoạt động từ Firebase
  static bool cheDoTruocDo = true;
  bool isAutoMode = true;
  if (firebaseReady && Firebase.RTDB.getString(&fbdo, "currentMode"))
  {
    String mode = fbdo.stringData();
    isAutoMode = (mode == "auto");
  }

  // Nếu chế độ thay đổi, cập nhật cho toàn bộ hệ thống
  if (isAutoMode != cheDoTruocDo)
  {
    Serial.printf("Chuyển chế độ: %s → %s\n", cheDoTruocDo ? "AUTO" : "MANUAL", isAutoMode ? "AUTO" : "MANUAL");
    cheDoTruocDo = isAutoMode;
    for (int i = 0; i < SO_HANG; i++)
    {
      for (int j = 0; j < SO_COT; j++)
      {
        HeThongVuon *cay = manHinh.layHeThongVuon(i, j);
        if (cay)
          cay->chuyenCheDo(isAutoMode);
      }
    }
  }

  // 2. Cập nhật và gửi dữ liệu cảm biến
  if (thoiGian - lanCapNhatCamBienCuoi > THOI_GIAN_CAP_NHAT_CAM_BIEN)
  {
    lanCapNhatCamBienCuoi = thoiGian;

    for (int i = 0; i < SO_HANG; i++)
    {
      for (int j = 0; j < SO_COT; j++)
      {
        HeThongVuon *cay = manHinh.layHeThongVuon(i, j);
        if (!cay)
          continue;

        cay->docDuLieuCamBien();

        float nhietDo = cay->layNhietDo();
        float doAmDat = cay->layDoAmDat();
        float anhSang = cay->layAnhSang();
        String gardenId = "gardenId" + String(i * SO_COT + j + 1);

        if (isAutoMode)
        {
          cay->capNhat(); // Auto control
        }
        else
        {
          // Manual mode – lấy lệnh điều khiển từ Firebase
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

        // Gửi dữ liệu cảm biến lên Firebase
        Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtNhietDo", nhietDo);
        Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtDoAm", doAmDat);
        Firebase.RTDB.setFloat(&fbdo, gardenId + "/anhSang", anhSang);

        // Gửi trạng thái thiết bị
        Firebase.RTDB.setInt(&fbdo, gardenId + "/mayBom", cay->layTrangThaiBom() ? 1 : 0);
        Firebase.RTDB.setInt(&fbdo, gardenId + "/den", cay->layTrangThaiDen() ? 1 : 0);
        Firebase.RTDB.setInt(&fbdo, gardenId + "/quat", cay->layTrangThaiQuat() ? 1 : 0);

        // Ghi log
        Serial.printf("Luong [%d][%d]\n", i, j);
        Serial.printf("  Do am dat: %.1f\n", cay->layDoAmDat());
        Serial.printf("  Anh sang: %.1f\n", cay->layAnhSang());
        Serial.printf("  Nhiet do: %.1f\n", cay->layNhietDo());
        Serial.printf("  Bom: %s | Den: %s | Quat: %s\n",
                      cay->layTrangThaiBom() ? "BAT" : "TAT",
                      cay->layTrangThaiDen() ? "BAT" : "TAT",
                      cay->layTrangThaiQuat() ? "BAT" : "TAT");
      }
    }
  }

  // 3. Cập nhật hiển thị màn hình
  if (thoiGian - lanCapNhatManHinhCuoi > THOI_GIAN_CAP_NHAT_MAN_HINH)
  {
    lanCapNhatManHinhCuoi = thoiGian;
    manHinh.capNhatHienThi();
  }

  manHinh.capNhatTrangThaiNut();
  delay(10);
}
