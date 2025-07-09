
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include "HeThongVuon.h"
#include "HienThi.h"

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

Adafruit_MCP23X17 mcp;
HienThi manHinh(SO_HANG, SO_COT);

unsigned long lanCapNhatCamBienCuoi = 0;
unsigned long lanCapNhatManHinhCuoi = 0;
const unsigned long THOI_GIAN_CAP_NHAT_CAM_BIEN = 5000;
const unsigned long THOI_GIAN_CAP_NHAT_MAN_HINH = 1000;

void setup() {
  Serial.begin(115200);
  Serial.println("Khoi dong he thong...");

  Wire.begin(MCP_SDA_PIN, MCP_SCL_PIN);
  if (!mcp.begin_I2C(MCP_ADDRESS)) {
    Serial.println("Loi MCP23017");
    while (true);
  }

  Serial.println("MCP23017 OK");

  manHinh.ganThongTinPhanCung(&mcp, chanDHT, chanDatAm, chanAnhSang, chanBomNuoc, chanDen, chanQuat);

  for (int i = 0; i < SO_HANG; i++) {
    for (int j = 0; j < SO_COT; j++) {
      manHinh.thietLapLuongCay(i, j, nullptr);  // chưa có cây ban đầu
      Serial.printf("Khoi tao luong [%d][%d]\n", i, j);
    }
  }

  manHinh.veGiaoDien();
  Serial.println("San sang.");
}

void loop() {
  unsigned long thoiGian = millis();

  if (thoiGian - lanCapNhatCamBienCuoi > THOI_GIAN_CAP_NHAT_CAM_BIEN) {
    lanCapNhatCamBienCuoi = thoiGian;

    for (int i = 0; i < SO_HANG; i++) {
      for (int j = 0; j < SO_COT; j++) {
        HeThongVuon* cay = manHinh.layHeThongVuon(i, j);
        if (cay != nullptr && manHinh.layCheDoHienTai() == CHE_DO_AUTO) {
          cay->capNhat();
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
  }

  if (thoiGian - lanCapNhatManHinhCuoi > THOI_GIAN_CAP_NHAT_MAN_HINH) {
    lanCapNhatManHinhCuoi = thoiGian;
    manHinh.capNhatHienThi();
  }

  manHinh.capNhatTrangThaiNut();
  delay(10);
}
