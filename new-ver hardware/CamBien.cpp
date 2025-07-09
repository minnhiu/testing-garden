#include "CamBien.h"

// Constructor
CamBien::CamBien(int chan, int loai, int giaTriMin, int giaTriMax, int giaTriADCMin, int giaTriADCMax) 
    : _chan(chan), _loai(loai), _giaTriMin(giaTriMin), _giaTriMax(giaTriMax), 
      _giaTriADCMin(giaTriADCMin), _giaTriADCMax(giaTriADCMax)
{
}
    
    
void CamBien::khoiTao() {
    if(_loai != 2) { 
        pinMode(_chan, INPUT);
    }
    
}

// Doc gia tri cam bien
float CamBien::layGiaTri(DHT& dht) {
    switch (_loai) {
        case 0: // Cam bien digital
            return digitalRead(_chan);
        case 1: {  // Cam bien analog thong thuong
            int giaTriADC = analogRead(_chan);
            return (float)map(giaTriADC, _giaTriADCMin, _giaTriADCMax, _giaTriMin, _giaTriMax);
        }
        case 2: {  // Cam bien DHT
            float nhietDo = dht.readTemperature();
            return isnan(nhietDo) ? -100.0f : nhietDo; // Xu  ly loi NaN
        }
        default:
            return -1.0e3f;  // Gia tri loi
    }
}

void CamBien::datPhamVi(int giaTriMin, int giaTriMax) {
    _giaTriMin = giaTriMin;
    _giaTriMax = giaTriMax;
}

void CamBien::datDoPhanGiai(int giaTriADCMin, int giaTriADCMax) {
    _giaTriADCMin = giaTriADCMin;
    _giaTriADCMax = giaTriADCMax;
}
