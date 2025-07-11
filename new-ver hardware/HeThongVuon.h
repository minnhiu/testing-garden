#ifndef HETHONGVUON_H
#define HETHONGVUON_H

#include <Adafruit_MCP23X17.h>
#include <DHT.h>
#include "ThietBi.h"
#include "CamBien.h"
#include "CauHinhCay.h"

class HeThongVuon
{
public:
    HeThongVuon(Adafruit_MCP23X17 &mcp,
                const CamBien &camNhiet,
                const CamBien &camDoAm,
                const CamBien &camAnhSang,
                int chanBom, int chanDen, int chanQuat);

    void batDau();
    void capNhat();
    void xuLyLogicChamSoc();

    float layDoAmDat();
    float layNhietDo();
    float layAnhSang();

    bool layTrangThaiBom();
    bool layTrangThaiDen();
    bool layTrangThaiQuat();

    void batBom();
    void tatBom();
    void batDen();
    void tatDen();
    void batQuat();
    void tatQuat();

    void chuyenCheDo(bool tuDong);
    bool laCheDoTuDong();

    void thietLapCauHinhCay(CauHinhCay cauHinh);
    CauHinhCay layCauHinhCay();

    void datTrangThaiBom(bool trangThai);
    void datTrangThaiDen(bool trangThai);
    void datTrangThaiQuat(bool trangThai);

    void capNhatDuLieuCamBien(float doAmDat, float nhietDo, float anhSang);
    void docDuLieuCamBien(); // Thêm phương thức này để tách riêng việc đọc cảm biến

private:
    CamBien _camBienDoAm, _camBienNhietDo, _camBienAnhSang;
    ThietBi _bom, _den, _quat;
    DHT _dht;
    CauHinhCay _cauHinhCay;

    bool _cheDoTuDong;
    float _doAmDat = 0;
    float _nhietDo = 0;
    float _anhSang = 0;
};

#endif