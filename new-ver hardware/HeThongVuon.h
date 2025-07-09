#ifndef HETHONGVUON_H
#define HETHONGVUON_H

#include <DHT.h>
#include "CamBien.h"
#include "CauHinhCay.h"
#include "ThietBi.h"

class HeThongVuon {
private:
    DHT _dht;

    // Cảm biến
    CamBien _camBienDoAm;
    CamBien _camBienAnhSang;
    CamBien _camBienNhietDo;

    // Thiết bị
    ThietBi _bom;
    ThietBi _den;
    ThietBi _quat;

    // Cấu hình
    CauHinhCay _cauHinhCay;
    bool _cheDoTuDong;

public:
    HeThongVuon(const CamBien& camNhiet,
                const CamBien& camDoAm,
                const CamBien& camAnhSang,
                const ThietBi& bom,
                const ThietBi& den,
                const ThietBi& quat);

    void batDau();
    void capNhat();

    float layDoAmDat();
    float layAnhSang();
    float layNhietDo();

    void batBom();
    void tatBom();
    void batDen();
    void tatDen();
    void batQuat();
    void tatQuat();

    bool layTrangThaiBom();
    bool layTrangThaiDen();
    bool layTrangThaiQuat();

    void thietLapCauHinhCay(CauHinhCay cauHinh);
    CauHinhCay layCauHinhCay();

    void chuyenCheDo(bool tuDong);
    bool laCheDoTuDong();

    void datTrangThaiBom(bool trangThai);
    void datTrangThaiQuat(bool trangThai);
    void datTrangThaiDen(bool trangThai);

    void capNhatDuLieuCamBien(float doAmDat, float nhietDo, float anhSang);
    void xuLyLogicChamSoc();
};

#endif
