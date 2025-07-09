#ifndef HIENTHI_H
#define HIENTHI_H

#include <TFT_eSPI.h>
#include <vector>
#include <Arduino.h>
#include <Adafruit_MCP23X17.h>
#include "HeThongVuon.h"
#include "CauHinhCay.h"
#include "CamBien.h"

#define MAU_NEN TFT_BLACK
#define SO_LOAI_CAY 6

enum CheDoBoSung {
    CHE_DO_AUTO = 0,
    CHE_DO_MANUAL = 1
};

struct ThongTinNut {
    int x, y;
    int chieuRong, chieuCao;
    String nhan;
    bool dangDuocBam;
};

class HienThi {
private:
    TFT_eSPI tft;
    int soHang;
    int soCot;
    std::vector<std::vector<HeThongVuon*>> luongCay;
    int luongDangChon[2];
    int trangThaiHienThi;
    CheDoBoSung cheDoHienTai;
    bool daCham;
    int diemChamX, diemChamY;

    // Các nút
    ThongTinNut nutMenu, nutChonCay, nutQuayLai, nutThoatMenu;
    std::vector<ThongTinNut> nutChonLoaiCay;
    ThongTinNut nutAutoManual, nutBom, nutQuat, nutDen;
    ThongTinNut nutCaiDat, nutThongKe, nutTrongCay;

    // Cấu hình phần cứng
    Adafruit_MCP23X17* mcpPtr;
    const int (*chanDHT)[2];
    const int (*chanDatAm)[2];
    const int (*chanAnhSang)[2];
    const int (*chanBomNuoc)[2];
    const int (*chanDen)[2];
    const int (*chanQuat)[2];

    // Hàm riêng
    void veNut(ThongTinNut nut);
    bool kiemTraNut(ThongTinNut nut, int x, int y);
    void veHeader(const String& tieuDe, uint16_t mauChinh);
    void veCardThongTin(int x, int y, int w, int h);
    void veGiaoDienChinh();
    void veGiaoDienMenu();
    void veGiaoDienChonLoaiCay();
    void veManHinhDieuKhien();
    void veThanhTienTrinh(int x, int y, int width, int height, float giaTriHienTai, float giaTriToiDa, uint16_t mauThanh);
    void veBieuTuongDoAm(int x, int y);
    void veBieuTuongAnhSang(int x, int y);
    void veBieuTuongNhietDo(int x, int y);
    void xuLyDieuKhienThucong(int x, int y);

public:
    HienThi(int soHang, int soCot);
    void veGiaoDien();
    void capNhatHienThi();
    void capNhatTrangThaiNut();

    void ganThongTinPhanCung(Adafruit_MCP23X17* mcp,
        const int chanDHT_[2][2],
        const int chanDatAm_[2][2],
        const int chanAnhSang_[2][2],
        const int chanBomNuoc_[2][2],
        const int chanDen_[2][2],
        const int chanQuat_[2][2]);

    void batTat();
    void trongCayTaiViTri(int hang, int cot, int loaiCay);
    void capNhatDuLieuCamBien(int hang, int cot, float doAmDat, float nhietDo, float anhSang);
    void xuLyLogicTuDong();

    void chuyenDoiCheDoAutoManual();
    CheDoBoSung layCheDoHienTai() const;
    void datCheDoHienTai(CheDoBoSung cheDo);
    void layLuongDangChon(int& hang, int& cot) const;
    void datLuongDangChon(int hang, int cot);

    void thongBaoCanhBao(const String& thongDiep);
    void thietLapLuongCay(int hang, int cot, HeThongVuon* heThong);
    HeThongVuon* layHeThongVuon(int hang, int cot);
    void xoaCayTaiViTri(int hang, int cot);
};

#endif
