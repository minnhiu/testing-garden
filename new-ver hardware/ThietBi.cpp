#include "ThietBi.h"

// Constructor
ThietBi::ThietBi(Adafruit_MCP23X17 mcp, int chan) : _mcp(mcp), _chan(chan), _trangThai(false) {
}

// Khởi tạo thiết bị
void ThietBi::khoiTao() {
    _mcp.pinMode(_chan, OUTPUT); 
    tat(); // Mặc định tắt tất cả thiết bị khi khởi tạo
}

// Bật thiết bị
void ThietBi::bat() {
    _mcp.digitalWrite(_chan, LOW); // Relay hoạt động mức thấp
    _trangThai = true;
}

// Tắt thiết bị
void ThietBi::tat() {
    _mcp.digitalWrite(_chan, HIGH); 
    _trangThai = false;
}

// Đổi trạng thái thiết bị
void ThietBi::chuyenTrangThai() {
    if (_trangThai) {
        tat();
    } else {
        bat();
    }
}

// Lấy trạng thái hiện tại
bool ThietBi::layTrangThai() {
    return _trangThai;
}

// Lấy số chân
int ThietBi::layChan() {
    return _chan;
}