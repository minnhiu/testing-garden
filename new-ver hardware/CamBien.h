#ifndef CAM_BIEN_H
#define CAM_BIEN_H

#include <Arduino.h>
#include <DHT.h>

#define LOAI_DHT DHT11

class CamBien {
public:
    CamBien(int chan = 0, int loai = 0, int giaTriMin = 0, int giaTriMax = 1, int giaTriADCMin = 0, int giaTriADCMax = 4095);
    void khoiTao();
    float layGiaTri(DHT& dht);
    void datPhamVi(int giaTriMin, int giaTriMax);
    void datDoPhanGiai(int giaTriADCMin, int giaTriADCMax);

private:
    int _chan;
    int _loai;
    int _giaTriMin, _giaTriMax;
    int _giaTriADCMin, _giaTriADCMax;
};

#endif
