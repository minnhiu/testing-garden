#include "CauHinhCay.h"

CauHinhCay::CauHinhCay(
    LoaiCay loai,
    const String &ten,
    float doAmDat,
    float anhSang,
    float nhietDo) : _loai(loai),
                     _ten(ten),
                     _nguongDoAmDat(doAmDat),
                     _nguongAnhSang(anhSang),
                     _nguongNhietDo(nhietDo)
{
}

CauHinhCay CauHinhCay::layCauHinhMacDinh(LoaiCay loai)
{
    switch (loai)
    {
    case CA_CHUA:
        return CauHinhCay(CA_CHUA, "Ca chua", 40.0, 600.0, 27.0);

    case RAU_XA_LACH:
        return CauHinhCay(RAU_XA_LACH, "Xa lach", 50.0, 450.0, 18.0);

    case HUNG_QUE:
        return CauHinhCay(HUNG_QUE, "Hung que", 35.0, 550.0, 25.0);

    case BAC_HA:
        return CauHinhCay(BAC_HA, "Bac ha", 45.0, 400.0, 25.0);

    case DUA_CHUOT:
        return CauHinhCay(DUA_CHUOT, "Dua chuot", 55.0, 650.0, 25.0);

    case DAU:
        return CauHinhCay(DAU, "Dau", 45.0, 550.0, 24.0);

    case TUY_CHINH:
    default:
        return CauHinhCay(TUY_CHINH, "Tuy chinh", 30.0, 500.0, 28.0);
    }
}
