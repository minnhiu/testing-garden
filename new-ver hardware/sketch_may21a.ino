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

// ========== Cấu hình WiFi và Firebase ==========
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
bool isAutoMode = true;

// ========== Cấu hình phần cứng ==========
#define MCP_ADDRESS 0x20
#define MCP_SDA_PIN 21
#define MCP_SCL_PIN 22

const int SO_HANG = 2;
const int SO_COT = 2;

const int chanAnhSang[SO_HANG][SO_COT] = {{32, 33}, {25, 26}};
const int chanDatAm[SO_HANG][SO_COT] = {{34, 35}, {36, 39}};
const int chanDHT[SO_HANG][SO_COT] = {{27, 14}, {12, 13}};
const int chanBomNuoc[SO_HANG][SO_COT] = {{0, 1}, {2, 3}};
const int chanDen[SO_HANG][SO_COT] = {{4, 5}, {6, 7}};
const int chanQuat[SO_HANG][SO_COT] = {{8, 9}, {10, 11}};

Adafruit_MCP23X17 mcp;
HienThi manHinh(SO_HANG, SO_COT);

// ========== Biến thời gian ==========
unsigned long lastSensorUpdate = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastTouchCheck = 0;
unsigned long lastModeCheck = 0;
const unsigned long SENSOR_INTERVAL = 3000;
const unsigned long DISPLAY_INTERVAL = 1000;
const unsigned long TOUCH_INTERVAL = 30;
const unsigned long MODE_CHECK_INTERVAL = 2000;

// ========== Trạng thái cũ ==========
float nhietDoCu[SO_HANG][SO_COT];
float doAmCu[SO_HANG][SO_COT];
float anhSangCu[SO_HANG][SO_COT];
int bomCu[SO_HANG][SO_COT];
int denCu[SO_HANG][SO_COT];
int quatCu[SO_HANG][SO_COT];

// ========== WiFi ==========
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

// ========== Firebase ==========
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
  Serial.println(firebaseReady ? "\nFirebase da san sang" : "\nFirebase loi ket noi");
}

// ========== SETUP ==========
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
    while (1)
      ;
  }

  manHinh.ganThongTinPhanCung(&mcp, chanDHT, chanDatAm, chanAnhSang,
                              chanBomNuoc, chanDen, chanQuat);

  for (int i = 0; i < SO_HANG; i++)
  {
    for (int j = 0; j < SO_COT; j++)
    {
      manHinh.thietLapLuongCay(i, j, nullptr);
      nhietDoCu[i][j] = -999;
      doAmCu[i][j] = -1;
      anhSangCu[i][j] = -1;
      bomCu[i][j] = -1;
      denCu[i][j] = -1;
      quatCu[i][j] = -1;
    }
  }

  manHinh.veGiaoDien();
  Serial.println("He thong san sang");
}

// ========== LOOP ==========
void loop()
{
  unsigned long now = millis();

  // 1. Kiểm tra chế độ hoạt động từ Firebase
  if (firebaseReady && now - lastModeCheck >= MODE_CHECK_INTERVAL)
  {
    lastModeCheck = now;
    if (Firebase.RTDB.getString(&fbdo, "currentMode"))
    {
      String mode = fbdo.stringData();
      bool newMode = (mode == "auto");
      if (newMode != isAutoMode)
      {
        isAutoMode = newMode;
        manHinh.datCheDoHienTai(isAutoMode ? CHE_DO_AUTO : CHE_DO_MANUAL);
        Serial.printf("Che do moi: %s\n", isAutoMode ? "AUTO" : "MANUAL");
      }
    }
  }

  // 2. Cập nhật dữ liệu cảm biến và trạng thái thiết bị
  if (now - lastSensorUpdate >= SENSOR_INTERVAL)
  {
    lastSensorUpdate = now;

    for (int i = 0; i < SO_HANG; i++)
    {
      for (int j = 0; j < SO_COT; j++)
      {
        HeThongVuon *cay = manHinh.layHeThongVuon(i, j);
        if (!cay)
          continue;

        cay->docDuLieuCamBien();
        float nhietDo = cay->layNhietDo();
        float doAm = cay->layDoAmDat();
        float anhSang = cay->layAnhSang();
        String gardenId = "gardenId" + String(i * SO_COT + j + 1);

        if (isAutoMode)
        {
          cay->capNhat();
        }
        else
        {
          // Lấy trạng thái từ Firebase khi ở chế độ thủ công
          if (Firebase.RTDB.getInt(&fbdo, gardenId + "/mayBom"))
            cay->datTrangThaiBom(fbdo.intData() == 1);
          if (Firebase.RTDB.getInt(&fbdo, gardenId + "/den"))
            cay->datTrangThaiDen(fbdo.intData() == 1);
          if (Firebase.RTDB.getInt(&fbdo, gardenId + "/quat"))
            cay->datTrangThaiQuat(fbdo.intData() == 1);
        }

        // Gửi dữ liệu cảm biến
        if (abs(nhietDo - nhietDoCu[i][j]) > 0.3)
        {
          Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtNhietDo", nhietDo);
          nhietDoCu[i][j] = nhietDo;
        }

        if (abs(doAm - doAmCu[i][j]) > 0.6)
        {
          Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtDoAm", doAm);
          doAmCu[i][j] = doAm;
        }

        if (abs(anhSang - anhSangCu[i][j]) > 50)
        {
          Firebase.RTDB.setFloat(&fbdo, gardenId + "/anhSang", anhSang);
          anhSangCu[i][j] = anhSang;
        }

        // Gửi trạng thái thiết bị chỉ khi ở chế độ tự động
        if (isAutoMode)
        {
          int bom = cay->layTrangThaiBom() ? 1 : 0;
          int den = cay->layTrangThaiDen() ? 1 : 0;
          int quat = cay->layTrangThaiQuat() ? 1 : 0;

          if (bom != bomCu[i][j])
          {
            Firebase.RTDB.setInt(&fbdo, gardenId + "/mayBom", bom);
            bomCu[i][j] = bom;
          }

          if (den != denCu[i][j])
          {
            Firebase.RTDB.setInt(&fbdo, gardenId + "/den", den);
            denCu[i][j] = den;
          }

          if (quat != quatCu[i][j])
          {
            Firebase.RTDB.setInt(&fbdo, gardenId + "/quat", quat);
            quatCu[i][j] = quat;
          }
        }

        Serial.printf("Luong [%d][%d] - Nhiet do: %.1f - Do am: %.1f - Anh sang: %.1f\n",
                      i, j, nhietDo, doAm, anhSang);
      }
    }
  }

  // 3. Hiển thị và cảm ứng
  if (now - lastDisplayUpdate >= DISPLAY_INTERVAL)
  {
    lastDisplayUpdate = now;
    manHinh.capNhatHienThi();
  }

  if (now - lastTouchCheck >= TOUCH_INTERVAL)
  {
    lastTouchCheck = now;
    manHinh.capNhatTrangThaiNut();
  }
}
