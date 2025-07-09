#ifndef THIET_BI_H
#define THIET_BI_H
#include <Adafruit_MCP23X17.h>

class ThietBi {
private:
    Adafruit_MCP23X17 _mcp; 
    int _chan;
    bool _trangThai;
public:
    ThietBi(Adafruit_MCP23X17 mcp, int chan); 
    void khoiTao();
    void bat();
    void tat();
    void chuyenTrangThai();
    bool layTrangThai();
    int layChan(); 
};

#endif