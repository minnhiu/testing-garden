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
#include "esp_task_wdt.h"

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

// Firebase objects
FirebaseData fbdo;
FirebaseData fbdoStream;
FirebaseAuth auth;
FirebaseConfig config;
bool firebaseReady = false;

// Hardware objects
Adafruit_MCP23X17 mcp;
HienThi manHinh(SO_HANG, SO_COT);

// Timer
unsigned long lanCapNhatCamBienCuoi = 0;
unsigned long lanCapNhatManHinhCuoi = 0;
unsigned long lanKiemTraKetNoiCuoi = 0;
const unsigned long THOI_GIAN_CAP_NHAT_CAM_BIEN = 3000;
const unsigned long THOI_GIAN_CAP_NHAT_MAN_HINH = 1000;
const unsigned long THOI_GIAN_KIEM_TRA_KET_NOI = 30000;

// Mode control
bool isAutoMode = true;
bool modeChanged = false;

// Previous values for change detection
static float nhietDoCu[SO_HANG][SO_COT] = {{-1000.0, -1000.0}, {-1000.0, -1000.0}};
static float doAmDatCu[SO_HANG][SO_COT] = {{-1.0, -1.0}, {-1.0, -1.0}};
static float anhSangCu[SO_HANG][SO_COT] = {{-1.0, -1.0}, {-1.0, -1.0}};
static int bomCu[SO_HANG][SO_COT] = {{-1, -1}, {-1, -1}};
static int denCu[SO_HANG][SO_COT] = {{-1, -1}, {-1, -1}};
static int quatCu[SO_HANG][SO_COT] = {{-1, -1}, {-1, -1}};

// Error handling
struct ErrorState
{
  bool wifiConnected;
  bool firebaseConnected;
  bool mcpConnected;
  unsigned long lastErrorTime;
  String lastError;
};

ErrorState errorState = {false, false, false, 0, ""};

// Firebase stream callback
void streamCallback(FirebaseStream data)
{
  String path = data.dataPath();
  Serial.printf("[Stream] Path: %s, Data: %s\n", path.c_str(), data.stringData().c_str());

  // Xu ly thay doi che do
  if (path == "/currentMode")
  {
    bool newMode = (data.stringData() == "auto");
    if (isAutoMode != newMode)
    {
      isAutoMode = newMode;
      modeChanged = true;
      Serial.printf("Mode changed to: %s\n", newMode ? "AUTO" : "MANUAL");
    }
  }

  // Xu ly dieu khien thiet bi trong che do manual
  else if (!isAutoMode)
  {
    for (int i = 0; i < SO_HANG; i++)
    {
      for (int j = 0; j < SO_COT; j++)
      {
        String gardenId = "/gardenId" + String(i * SO_COT + j + 1);
        HeThongVuon *cay = manHinh.layHeThongVuon(i, j);

        if (cay && path.startsWith(gardenId))
        {
          if (path == gardenId + "/mayBom")
          {
            bool newState = (data.intData() == 1);
            cay->datTrangThaiBom(newState);
            Serial.printf("Manual control: Pump [%d][%d] = %s\n", i, j, newState ? "ON" : "OFF");
          }
          else if (path == gardenId + "/den")
          {
            bool newState = (data.intData() == 1);
            cay->datTrangThaiDen(newState);
            Serial.printf("Manual control: Light [%d][%d] = %s\n", i, j, newState ? "ON" : "OFF");
          }
          else if (path == gardenId + "/quat")
          {
            bool newState = (data.intData() == 1);
            cay->datTrangThaiQuat(newState);
            Serial.printf("Manual control: Fan [%d][%d] = %s\n", i, j, newState ? "ON" : "OFF");
          }
          break;
        }
      }
    }
  }
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println("Firebase stream timeout - attempting reconnect");
    errorState.firebaseConnected = false;
    errorState.lastError = "Stream timeout";
    errorState.lastErrorTime = millis();
  }
}

// WiFi connection with retry
bool connectToWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000)
  {
    delay(500);
    Serial.print(".");
    esp_task_wdt_reset();
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected successfully");
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
    errorState.wifiConnected = true;
    return true;
  }
  else
  {
    Serial.println("\nWiFi connection failed");
    errorState.wifiConnected = false;
    errorState.lastError = "WiFi connection timeout";
    errorState.lastErrorTime = millis();
    return false;
  }
}

// Firebase connection with retry
bool connectToFirebase()
{
  if (!errorState.wifiConnected)
  {
    return false;
  }

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.reconnectWiFi(true);
  config.token_status_callback = tokenStatusCallback;
  config.timeout.serverResponse = 10 * 1000;

  Firebase.begin(&config, &auth);

  Serial.println("Connecting to Firebase...");
  unsigned long startTime = millis();
  while (!Firebase.ready() && millis() - startTime < 15000)
  {
    delay(500);
    Serial.print(".");
    esp_task_wdt_reset();
  }

  firebaseReady = Firebase.ready();

  if (firebaseReady)
  {
    Serial.println("\nFirebase connected successfully");

    // Setup stream
    if (!Firebase.RTDB.beginStream(&fbdoStream, "/"))
    {
      Serial.printf("Stream setup failed: %s\n", fbdoStream.errorReason().c_str());
      errorState.lastError = "Stream setup failed";
      errorState.lastErrorTime = millis();
      return false;
    }

    Firebase.RTDB.setStreamCallback(&fbdoStream, streamCallback, streamTimeoutCallback);

    // Initialize mode from Firebase
    String currentMode;
    if (Firebase.RTDB.getString(&fbdo, "/currentMode"))
    {
      currentMode = fbdo.stringData();
      isAutoMode = (currentMode == "auto");
      Serial.printf("Initial mode: %s\n", isAutoMode ? "AUTO" : "MANUAL");
    }

    errorState.firebaseConnected = true;
    return true;
  }
  else
  {
    Serial.println("\nFirebase connection failed");
    errorState.firebaseConnected = false;
    errorState.lastError = "Firebase connection timeout";
    errorState.lastErrorTime = millis();
    return false;
  }
}

// System health check
void checkSystemHealth()
{
  unsigned long currentTime = millis();

  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED)
  {
    errorState.wifiConnected = false;
    if (currentTime - lanKiemTraKetNoiCuoi > THOI_GIAN_KIEM_TRA_KET_NOI)
    {
      Serial.println("WiFi disconnected - attempting reconnection");
      connectToWiFi();
      lanKiemTraKetNoiCuoi = currentTime;
    }
  }

  // Check Firebase connection
  if (!Firebase.ready() && errorState.wifiConnected)
  {
    errorState.firebaseConnected = false;
    if (currentTime - lanKiemTraKetNoiCuoi > THOI_GIAN_KIEM_TRA_KET_NOI)
    {
      Serial.println("Firebase disconnected - attempting reconnection");
      connectToFirebase();
      lanKiemTraKetNoiCuoi = currentTime;
    }
  }

  // Print error status if any
  if (currentTime - errorState.lastErrorTime < 5000 && errorState.lastError.length() > 0)
  {
    Serial.printf("System Error: %s\n", errorState.lastError.c_str());
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("=== Smart Garden System Starting ===");

  // Initialize watchdog
  esp_task_wdt_init(30, true);
  esp_task_wdt_add(NULL);

  // Connect to WiFi
  if (!connectToWiFi())
  {
    Serial.println("WARNING: Starting in offline mode");
  }

  // Connect to Firebase
  if (errorState.wifiConnected)
  {
    connectToFirebase();
  }

  // Initialize MCP23017
  Wire.begin(MCP_SDA_PIN, MCP_SCL_PIN);
  if (!mcp.begin_I2C(MCP_ADDRESS))
  {
    Serial.println("FATAL: MCP23017 initialization failed");
    errorState.mcpConnected = false;
    while (true)
    {
      delay(1000);
      esp_task_wdt_reset();
    }
  }

  Serial.println("MCP23017 initialized successfully");
  errorState.mcpConnected = true;

  // Initialize display
  manHinh.ganThongTinPhanCung(&mcp, chanDHT, chanDatAm, chanAnhSang, chanBomNuoc, chanDen, chanQuat);

  // Initialize HeThongVuon objects
  for (int i = 0; i < SO_HANG; i++)
  {
    for (int j = 0; j < SO_COT; j++)
    {
      HeThongVuon *cay = new HeThongVuon(i, j, chanDHT[i][j], chanDatAm[i][j], chanAnhSang[i][j],
                                         mcp, chanBomNuoc[i][j], chanDen[i][j], chanQuat[i][j]);
      if (cay)
      {
        manHinh.thietLapLuongCay(i, j, cay);
        Serial.printf("Garden [%d][%d] initialized successfully\n", i, j);
      }
      else
      {
        Serial.printf("ERROR: Failed to create garden [%d][%d]\n", i, j);
      }
    }
  }

  // Set initial mode
  manHinh.datCheDoHienTai(isAutoMode ? CHE_DO_AUTO : CHE_DO_MANUAL);
  manHinh.veGiaoDien();

  Serial.println("=== System Ready ===");
}

void loop()
{
  esp_task_wdt_reset();

  unsigned long currentTime = millis();

  // Check system health
  checkSystemHealth();

  // Handle mode changes
  if (modeChanged)
  {
    manHinh.datCheDoHienTai(isAutoMode ? CHE_DO_AUTO : CHE_DO_MANUAL);
    modeChanged = false;
    Serial.printf("Mode updated to: %s\n", isAutoMode ? "AUTO" : "MANUAL");
  }

  // Update sensor data and control devices
  if (currentTime - lanCapNhatCamBienCuoi > THOI_GIAN_CAP_NHAT_CAM_BIEN)
  {
    lanCapNhatCamBienCuoi = currentTime;

    for (int i = 0; i < SO_HANG; i++)
    {
      for (int j = 0; j < SO_COT; j++)
      {
        HeThongVuon *cay = manHinh.layHeThongVuon(i, j);
        if (!cay)
          continue;

        // Read sensor data
        cay->docDuLieuCamBien();

        // Auto control if in auto mode
        if (isAutoMode)
        {
          cay->capNhat();
        }

        // Get current values
        float nhietDo = cay->layNhietDo();
        float doAmDat = cay->layDoAmDat();
        float anhSang = cay->layAnhSang();
        bool bomMoi = cay->layTrangThaiBom();
        bool denMoi = cay->layTrangThaiDen();
        bool quatMoi = cay->layTrangThaiQuat();

        String gardenId = "gardenId" + String(i * SO_COT + j + 1);

        // Send sensor data to Firebase if changed significantly
        if (firebaseReady && abs(nhietDo - nhietDoCu[i][j]) > 0.3)
        {
          Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtNhietDo", nhietDo);
          nhietDoCu[i][j] = nhietDo;
        }

        if (firebaseReady && abs(doAmDat - doAmDatCu[i][j]) > 0.6)
        {
          Firebase.RTDB.setFloat(&fbdo, gardenId + "/dhtDoAm", doAmDat);
          doAmDatCu[i][j] = doAmDat;
        }

        if (firebaseReady && abs(anhSang - anhSangCu[i][j]) > 100)
        {
          Firebase.RTDB.setFloat(&fbdo, gardenId + "/anhSang", anhSang);
          anhSangCu[i][j] = anhSang;
        }

        // Send device status to Firebase only in AUTO mode and if changed
        if (firebaseReady && isAutoMode)
        {
          if (bomMoi != (bomCu[i][j] == 1))
          {
            Firebase.RTDB.setInt(&fbdo, gardenId + "/mayBom", bomMoi ? 1 : 0);
            bomCu[i][j] = bomMoi ? 1 : 0;
          }

          if (denMoi != (denCu[i][j] == 1))
          {
            Firebase.RTDB.setInt(&fbdo, gardenId + "/den", denMoi ? 1 : 0);
            denCu[i][j] = denMoi ? 1 : 0;
          }

          if (quatMoi != (quatCu[i][j] == 1))
          {
            Firebase.RTDB.setInt(&fbdo, gardenId + "/quat", quatMoi ? 1 : 0);
            quatCu[i][j] = quatMoi ? 1 : 0;
          }
        }

        // Debug output
        Serial.printf("Garden [%d][%d]: T=%.1f°C, H=%.1f%%, L=%.0f lux | P:%s L:%s F:%s [%s]\n",
                      i, j, nhietDo, doAmDat, anhSang,
                      bomMoi ? "ON" : "OFF",
                      denMoi ? "ON" : "OFF",
                      quatMoi ? "ON" : "OFF",
                      isAutoMode ? "AUTO" : "MANUAL");
      }
    }
  }

  // Update display
  if (currentTime - lanCapNhatManHinhCuoi > THOI_GIAN_CAP_NHAT_MAN_HINH)
  {
    lanCapNhatManHinhCuoi = currentTime;
    manHinh.capNhatHienThi();
  }

  // Handle button input
  manHinh.capNhatTrangThaiNut();

  // Small delay to prevent task hogging
  delay(10);
}