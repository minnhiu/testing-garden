#include "HeThongVuon.h"

HeThongVuon::HeThongVuon(Adafruit_MCP23X17 &mcp,
                         const CamBien &camNhiet,
                         const CamBien &camDoAm,
                         const CamBien &camAnhSang,
                         int chanBom, int chanDen, int chanQuat)
    : _camBienDoAm(camDoAm),
      _camBienAnhSang(camAnhSang),
      _camBienNhietDo(camNhiet),
      _bom(ThietBi(mcp, chanBom)),
      _den(ThietBi(mcp, chanDen)),
      _quat(ThietBi(mcp, chanQuat)),
      _dht(camNhiet.layChan(), DHT11),
      _cheDoTuDong(true),
      _fbdoPtr(nullptr)
{
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
    // CHỈ xử lý khi ở chế độ tự động
    if (!_cheDoTuDong)
    {
        return;
    }

    // Đọc dữ liệu cảm biến
    docDuLieuCamBien();

    // Xử lý logic chăm sóc tự động
    xuLyLogicChamSoc();
}

void HeThongVuon::docDuLieuCamBien()
{
    // Đọc dữ liệu từ cảm biến (có thể gọi bất kể chế độ nào)
    _nhietDo = _camBienNhietDo.layGiaTri(_dht);
    _doAmDat = _camBienDoAm.layGiaTri(_dht);
    _anhSang = _camBienAnhSang.layGiaTri(_dht);
}

void HeThongVuon::xuLyLogicChamSoc()
{
    // CHỈ xử lý logic khi ở chế độ tự động
    if (!_cheDoTuDong)
    {
        return;
    }

    if (_doAmDat < _cauHinhCay.layNguongDoAmDat())
        batBom();
    else
        tatBom();

    if (_anhSang < _cauHinhCay.layNguongAnhSang())
        batDen();
    else
        tatDen();

    if (_nhietDo > _cauHinhCay.layNguongNhietDo())
        batQuat();
    else
        tatQuat();
}

float HeThongVuon::layDoAmDat() { return _doAmDat; }
float HeThongVuon::layAnhSang() { return _anhSang; }
float HeThongVuon::layNhietDo() { return _nhietDo; }

void HeThongVuon::batBom()
{
    _bom.bat();
}
void HeThongVuon::tatBom()
{
    _bom.tat();
}
void HeThongVuon::batDen()
{
    _den.bat();
}
void HeThongVuon::tatDen()
{
    _den.tat();
}
void HeThongVuon::batQuat()
{
    _quat.bat();
}
void HeThongVuon::tatQuat()
{
    _quat.tat();
}

bool HeThongVuon::layTrangThaiBom() { return _bom.layTrangThai(); }
bool HeThongVuon::layTrangThaiDen() { return _den.layTrangThai(); }
bool HeThongVuon::layTrangThaiQuat() { return _quat.layTrangThai(); }

void HeThongVuon::thietLapCauHinhCay(CauHinhCay cauHinh)
{
    _cauHinhCay = cauHinh;
}
CauHinhCay HeThongVuon::layCauHinhCay()
{
    return _cauHinhCay;
}

void HeThongVuon::chuyenCheDo(bool tuDong)
{
    if (!tuDong && _cheDoTuDong)
    {
        // Khi chuyển từ tự động sang thủ công thì tắt hết thiết bị
        tatBom();
        tatDen();
        tatQuat();
        Serial.println("Chuyển sang chế độ thủ công - Tắt tất cả thiết bị");
    }
    _cheDoTuDong = tuDong;
    Serial.printf("Chế độ hiện tại: %s\n", _cheDoTuDong ? "TỰ ĐỘNG" : "THỦ CÔNG");
}

bool HeThongVuon::laCheDoTuDong()
{
    return _cheDoTuDong;
}

void HeThongVuon::datTrangThaiBom(bool trangThai)
{
    // CHỈ cho phép điều khiển thủ công khi ở chế độ thủ công
    if (!_cheDoTuDong)
    {
        if (trangThai)
        {
            _bom.bat();
        }
        else
        {
            _bom.tat();
        }
        Serial.printf("Điều khiển thủ công - Bơm: %s\n", trangThai ? "BẬT" : "TẮT");
    }
    else
    {
        Serial.println("Không thể điều khiển thủ công trong chế độ tự động!");
    }
}

void HeThongVuon::datTrangThaiQuat(bool trangThai)
{
    // CHỈ cho phép điều khiển thủ công khi ở chế độ thủ công
    if (!_cheDoTuDong)
    {
        if (trangThai)
        {
            _quat.bat();
        }
        else
        {
            _quat.tat();
        }
        Serial.printf("Điều khiển thủ công - Quạt: %s\n", trangThai ? "BẬT" : "TẮT");
    }
    else
    {
        Serial.println("Không thể điều khiển thủ công trong chế độ tự động!");
    }
}

void HeThongVuon::datTrangThaiDen(bool trangThai)
{
    // CHỈ cho phép điều khiển thủ công khi ở chế độ thủ công
    if (!_cheDoTuDong)
    {
        if (trangThai)
        {
            _den.bat();
        }
        else
        {
            _den.tat();
        }
        Serial.printf("Điều khiển thủ công - Đèn: %s\n", trangThai ? "BẬT" : "TẮT");
    }
    else
    {
        Serial.println("Không thể điều khiển thủ công trong chế độ tự động!");
    }
}

void HeThongVuon::capNhatDuLieuCamBien(float doAmDat, float nhietDo, float anhSang)
{
    _doAmDat = doAmDat;
    _nhietDo = nhietDo;
    _anhSang = anhSang;
}
