#ifndef CAU_HINH_CAY_H
#define CAU_HINH_CAY_H

#include <Arduino.h>

// Nhan dien tung loai cay
enum LoaiCay {
    CA_CHUA = 0,
    RAU_XA_LACH = 1,
    HUNG_QUE = 2, 
    BAC_HA = 3,
    DUA_CHUOT = 4,
    DAU = 5,
    TUY_CHINH = 99
};

class CauHinhCay {
private:
    LoaiCay _loai;
    String _ten;
    float _nguongDoAmDat;
    float _nguongAnhSang;
    float _nguongNhietDo;
    
public:
    // Constructor voi tham so mac dinh
    CauHinhCay(
        LoaiCay loai = CA_CHUA,
        const String& ten = "Ca chua",
        float doAmDat = 30.0,
        float anhSang = 500.0,
        float nhietDo = 28.0
    );
    
    // Getters
    LoaiCay layLoai() const { return _loai; }
    String layTen() const { return _ten; }
    float layNguongDoAmDat() const { return _nguongDoAmDat; }
    float layNguongAnhSang() const { return _nguongAnhSang; }
    float layNguongNhietDo() const { return _nguongNhietDo; }
    
    // Setters
    void datNguongDoAmDat(float giaTri) { _nguongDoAmDat = giaTri; }
    void datNguongAnhSang(float giaTri) { _nguongAnhSang = giaTri; }
    void datNguongNhietDo(float giaTri) { _nguongNhietDo = giaTri; }
    void datTen(const String& ten) { _ten = ten; }
    void datLoai(LoaiCay loai) { _loai = loai; }
    
    // Tao cau hinh mac dinh dua tren loai cay
    static CauHinhCay layCauHinhMacDinh(LoaiCay loai);
};

#endif
