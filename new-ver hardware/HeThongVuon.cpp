#include "HeThongVuon.h"
#include <Firebase_ESP_Client.h>

private:
FirebaseData *fbdoPtr = nullptr; // con trỏ tới FirebaseData

public:
void ganFirebase(FirebaseData *fbdo); // khai báo hàm gán fbdo
// Constructor
HeThongVuon::HeThongVuon(const CamBien &camNhiet,
                         const CamBien &camDoAm,
                         const CamBien &camAnhSang,
                         const ThietBi &bom,
                         const ThietBi &den,
                         const ThietBi &quat)
    : _camBienDoAm(camDoAm),
      _camBienAnhSang(camAnhSang),
      _camBienNhietDo(camNhiet),
      _bom(bom),
      _den(den),
      _quat(quat),
      _dht(camNhiet.layChan(), DHT11),
      _cheDoTuDong(true),
      _fbdoPtr(nullptr),
      _gardenPath("gardenId1") // mặc định
{
}

void HeThongVuon::ganFirebase(FirebaseData *fbdo, const String &gardenIdPath)
{
    _fbdoPtr = fbdo;
    _gardenPath = gardenIdPath;
}

void HeThongVuon::batDau()
{
    _dht.begin();
    _bom.khoiTao();
    _den.khoiTao();
    _quat.khoiTao();
    tatBom();
    tatDen();
    tatQuat();
}

void HeThongVuon::capNhat()
{
    if (!_cheDoTuDong)
        return;
    xuLyLogicChamSoc();
}

void HeThongVuon::xuLyLogicChamSoc()
{
    float doAmDat = layDoAmDat();
    float anhSang = layAnhSang();
    float nhietDo = layNhietDo();

    if (doAmDat < _cauHinhCay.layNguongDoAmDat())
        batBom();
    else
        tatBom();
    if (anhSang < _cauHinhCay.layNguongAnhSang())
        batDen();
    else
        tatDen();
    if (nhietDo > _cauHinhCay.layNguongNhietDo())
        batQuat();
    else
        tatQuat();
}

// --- Cảm biến ---
float HeThongVuon::layDoAmDat()
{
    return _camBienDoAm.layGiaTri(_dht);
}

float HeThongVuon::layAnhSang()
{
    return _camBienAnhSang.layGiaTri(_dht);
}

float HeThongVuon::layNhietDo()
{
    return _camBienNhietDo.layGiaTri(_dht);
}

// --- Thiết bị ---
void HeThongVuon::batBom()
{
    _bom.bat();
    capNhatFirebase("mayBom", 1);
}
void HeThongVuon::tatBom()
{
    _bom.tat();
    capNhatFirebase("mayBom", 0);
}

void HeThongVuon::batDen()
{
    _den.bat();
    capNhatFirebase("den", 1);
}
void HeThongVuon::tatDen()
{
    _den.tat();
    capNhatFirebase("den", 0);
}

void HeThongVuon::batQuat()
{
    _quat.bat();
    capNhatFirebase("quat", 1);
}
void HeThongVuon::tatQuat()
{
    _quat.tat();
    capNhatFirebase("quat", 0);
}

bool HeThongVuon::layTrangThaiBom() { return _bom.layTrangThai(); }
bool HeThongVuon::layTrangThaiDen() { return _den.layTrangThai(); }
bool HeThongVuon::layTrangThaiQuat() { return _quat.layTrangThai(); }

// --- Cấu hình ---
void HeThongVuon::thietLapCauHinhCay(CauHinhCay cauHinh)
{
    _cauHinhCay = cauHinh;
}
CauHinhCay HeThongVuon::layCauHinhCay()
{
    return _cauHinhCay;
}

// --- Chế độ ---
void HeThongVuon::chuyenCheDo(bool tuDong)
{
    _cheDoTuDong = tuDong;
}
bool HeThongVuon::laCheDoTuDong()
{
    return _cheDoTuDong;
}

void HeThongVuon::datTrangThaiBom(bool trangThai)
{
    if (trangThai)
        batBom();
    else
        tatBom();
}
void HeThongVuon::datTrangThaiQuat(bool trangThai)
{
    if (trangThai)
        batQuat();
    else
        tatQuat();
}
void HeThongVuon::datTrangThaiDen(bool trangThai)
{
    if (trangThai)
        batDen();
    else
        tatDen();
}

// --- Cập nhật cảm biến theo mô phỏng (nếu có) ---
void HeThongVuon::capNhatDuLieuCamBien(float doAmDat, float nhietDo, float anhSang)
{
}

// --- Nội bộ ---
void HeThongVuon::capNhatFirebase(const String &tenThietBi, int trangThai)
{
    if (_fbdoPtr != nullptr && Firebase.ready())
    {
        Firebase.RTDB.setInt(_fbdoPtr, _gardenPath + "/" + tenThietBi, trangThai);
    }
}