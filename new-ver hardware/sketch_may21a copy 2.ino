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

// ==== WiFi & Firebase Configuration ====
#define WIFI_SSID "Ngà Liêm"
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

// ==== Hardware Setup ====
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

unsigned long lastTouchCheck = 0;
unsigned long lastFirebaseCheck = 0;
unsigned long lastSensorCheck = 0;
unsigned long lastDisplayUpdate = 0;

// === Sửa tại đây: giảm khoảng thời gian kiểm tra cảm ứng ===
const unsigned long TOUCH_INTERVAL = 30;
const unsigned long FIREBASE_INTERVAL = 3000;
const unsigned long SENSOR_INTERVAL = 3000;
const unsigned long DISPLAY_INTERVAL = 1000;

bool cheDoTruocDo = true;

// =============================
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
    }
  }
  manHinh.veGiaoDien();
  Serial.println("He thong san sang");
}

// =============================
void capNhatCheDoTuFirebase(unsigned long now)
{
  static unsigned long lastModeCheck = 0;
  if (!firebaseReady || now - lastModeCheck < 2000)
    return;
  lastModeCheck = now;

  if (Firebase.RTDB.getString(&fbdo, "currentMode"))
  {
    String mode = fbdo.stringData();
    isAutoMode = (mode == "auto");

    if (isAutoMode != cheDoTruocDo)
    {
      Serial.printf("Che do thay doi: %s\n", isAutoMode ? "AUTO" : "MANUAL");
      manHinh.datCheDoHienTai(isAutoMode ? CHE_DO_AUTO : CHE_DO_MANUAL);
      cheDoTruocDo = isAutoMode;

      // === Sửa tại đây: cập nhật ngay lập tức sau khi đổi chế độ ===
      lastSensorCheck = 0;
    }
  }
}

void capNhatCamBienVaGuiFirebase(unsigned long now)
{
  if (!firebaseReady || now - lastSensorCheck < SENSOR_INTERVAL)
    return;
  lastSensorCheck = now;

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
        Firebase.RTDB.getInt(&fbdo, gardenId + "/mayBom");
        cay->datTrangThaiBom(fbdo.intData() == 1);
        Firebase.RTDB.getInt(&fbdo, gardenId + "/den");
        cay->datTrangThaiDen(fbdo.intData() == 1);
        Firebase.RTDB.getInt(&fbdo, gardenId + "/quat");
        cay->datTrangThaiQuat(fbdo.intData() == 1);
      }

      Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtNhietDo", nhietDo);
      Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtDoAm", doAm);
      Firebase.RTDB.setFloat(&fbdo, gardenId + "/anhSang", anhSang);
      Firebase.RTDB.setInt(&fbdo, gardenId + "/mayBom", cay->layTrangThaiBom());
      Firebase.RTDB.setInt(&fbdo, gardenId + "/den", cay->layTrangThaiDen());
      Firebase.RTDB.setInt(&fbdo, gardenId + "/quat", cay->layTrangThaiQuat());
    }
  }
}

void capNhatHienThiManHinh(unsigned long now)
{
  if (now - lastDisplayUpdate >= DISPLAY_INTERVAL)
  {
    lastDisplayUpdate = now;
    manHinh.capNhatHienThi();
  }
}

void capNhatTouch()
{
  if (millis() - lastTouchCheck > TOUCH_INTERVAL)
  {
    lastTouchCheck = millis();
    manHinh.capNhatTrangThaiNut();
  }
}

// =============================
void loop()
{
  unsigned long now = millis();
  capNhatCheDoTuFirebase(now);
  capNhatCamBienVaGuiFirebase(now);
  capNhatHienThiManHinh(now);
  capNhatTouch();
}