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

// Cau hinh MCP23017
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

// Cau hinh Firebase
#define WIFI_SSID "Nga Liem"
#define WIFI_PASSWORD "tinuanoi"
#define API_KEY "AIzaSyCa-0haXGQ1j620ppeOm9HV5Ltdw-JeF6g"
#define DATABASE_URL "https://smart-garden-64cd4-default-rtdb.firebaseio.com/"
#define USER_EMAIL "minhhieupham981@gmail.com"
#define USER_PASSWORD "21092109"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool firebaseReady = false;

// Man hinh & MCP
Adafruit_MCP23X17 mcp;
HienThi manHinh(SO_HANG, SO_COT);

// Timer
unsigned long lanCapNhatCamBienCuoi = 0;
unsigned long lanCapNhatManHinhCuoi = 0;
const unsigned long THOI_GIAN_CAP_NHAT_CAM_BIEN = 3000;
const unsigned long THOI_GIAN_CAP_NHAT_MAN_HINH = 1000;
unsigned long lanKiemTraModeCuoi = 0;
const unsigned long THOI_GIAN_KIEM_TRA_MODE = 2950;

bool cheDoTruocDo = true;

// WiFi
void connectToWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Dang ket noi WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi da ket noi");
}

// Firebase
void connectToFirebase()
{
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.reconnectWiFi(true);
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);

  Serial.println("Dang ket noi Firebase...");
  unsigned long startTime = millis();
  while (!Firebase.ready() && millis() - startTime < 10000)
  {
    delay(300);
    Serial.print(".");
  }

  firebaseReady = Firebase.ready();

  if (firebaseReady)
  {
    Serial.println("\nFirebase da san sang");
  }
  else
  {
    Serial.println("\nFirebase loi ket noi");
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Khoi dong he thong");

  connectToWiFi();
  connectToFirebase();

  Wire.begin(MCP_SDA_PIN, MCP_SCL_PIN);
  if (!mcp.begin_I2C(MCP_ADDRESS))
  {
    Serial.println("Loi khoi tao MCP23017");
    while (true)
      ;
  }

  Serial.println("MCP23017 da san sang");

  manHinh.ganThongTinPhanCung(&mcp, chanDHT, chanDatAm, chanAnhSang, chanBomNuoc, chanDen, chanQuat);

  for (int i = 0; i < SO_HANG; i++)
  {
    for (int j = 0; j < SO_COT; j++)
    {
      manHinh.thietLapLuongCay(i, j, nullptr);
      Serial.printf("Da khoi tao luong [%d][%d]\n", i, j);
    }
  }

  manHinh.veGiaoDien();
  Serial.println("He thong san sang");
}

void loop()
{
  unsigned long thoiGian = millis();

  // ===== 1. KIEM TRA CHE DO HOAT DONG =====
  if (firebaseReady && thoiGian - lanKiemTraModeCuoi >= THOI_GIAN_KIEM_TRA_MODE)
  {
    lanKiemTraModeCuoi = thoiGian;

    bool isAutoMode = true;
    if (Firebase.RTDB.getString(&fbdo, "currentMode"))
    {
      String mode = fbdo.stringData();
      isAutoMode = (mode == "auto");
    }

    if (isAutoMode != cheDoTruocDo)
    {
      Serial.printf("Chuyen che do: %s -> %s\n",
                    cheDoTruocDo ? "AUTO" : "MANUAL",
                    isAutoMode ? "AUTO" : "MANUAL");

      cheDoTruocDo = isAutoMode;
      manHinh.datCheDoHienTai(isAutoMode ? CHE_DO_AUTO : CHE_DO_MANUAL);
    }
  }
  // 2. Doc cam bien va gui len Firebase
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
        int bomMoi = cay->layTrangThaiBom() ? 1 : 0;
        int denMoi = cay->layTrangThaiDen() ? 1 : 0;
        int quatMoi = cay->layTrangThaiQuat() ? 1 : 0;
        String gardenId = "gardenId" + String(i * SO_COT + j + 1);

        if (isAutoMode)
        {
          cay->capNhat(); // cap nhat tu dong
        }
        else
        {
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

        // Gui chi khi thay doi du lieu
        static float nhietDoCu[2][2] = {{-1000, -1000}, {-1000, -1000}};
        static float doAmCu[2][2] = {{-1, -1}, {-1, -1}};
        static float anhSangCu[2][2] = {{-1, -1}, {-1, -1}};
        static int bomCu[2][2] = {{-1, -1}, {-1, -1}};
        static int denCu[2][2] = {{-1, -1}, {-1, -1}};
        static int quatCu[2][2] = {{-1, -1}, {-1, -1}};

        if (abs(nhietDo - nhietDoCu[i][j]) > 0.3)
        {
          Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtNhietDo", nhietDo);
          nhietDoCu[i][j] = nhietDo;
        }

        if (abs(doAmDat - doAmCu[i][j]) > 0.6)
        {
          Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtDoAm", doAmDat);
          doAmCu[i][j] = doAmDat;
        }

        if (abs(anhSang - anhSangCu[i][j]) > 100)
        {
          Firebase.RTDB.setFloat(&fbdo, gardenId + "/anhSang", anhSang);
          anhSangCu[i][j] = anhSang;
        }

        if (bomMoi != bomCu[i][j])
        {
          Firebase.RTDB.setInt(&fbdo, gardenId + "/mayBom", bomMoi);
          bomCu[i][j] = bomMoi;
        }
        if (denMoi != denCu[i][j])
        {
          Firebase.RTDB.setInt(&fbdo, gardenId + "/den", denMoi);
          denCu[i][j] = denMoi;
        }
        if (quatMoi != quatCu[i][j])
        {
          Firebase.RTDB.setInt(&fbdo, gardenId + "/quat", quatMoi);
          quatCu[i][j] = quatMoi;
        }

        Serial.printf("Luong [%d][%d] - Mode: %s\n", i, j, isAutoMode ? "AUTO" : "MANUAL");
        Serial.printf("  Nhiet do: %.1f | Do am: %.1f | Anh sang: %.1f\n", nhietDo, doAmDat, anhSang);
        Serial.printf("  Bom: %s | Den: %s | Quat: %s\n\n",
                      cay->layTrangThaiBom() ? "BAT" : "TAT",
                      cay->layTrangThaiDen() ? "BAT" : "TAT",
                      cay->layTrangThaiQuat() ? "BAT" : "TAT");
      }
    }
  }

  // 3. Cap nhat hien thi man hinh
  if (thoiGian - lanCapNhatManHinhCuoi > THOI_GIAN_CAP_NHAT_MAN_HINH)
  {
    lanCapNhatManHinhCuoi = thoiGian;
    manHinh.capNhatHienThi();
  }

  manHinh.capNhatTrangThaiNut();

  int hang, cot;
  manHinh.layLuongDangChon(hang, cot);
  HeThongVuon *cay = manHinh.layHeThongVuon(hang, cot);
  if (cay != nullptr && cay->laCheDoTuDong())
  {
    cay->capNhat(); // cap nhat rieng cho luong dang chon
  }
}
