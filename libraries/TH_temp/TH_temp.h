#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef TH_TEMP_H
#define TH_TEMP_H

class SimpleKalmanFilter;

class THSensorService {
public:
    THSensorService();
    void RequestSensors();
    bool IsError();
    bool MapAddresses();
protected:
    short count;
    float temperatures[8];
private:
    int readingErrors;
    std::vector<SimpleKalmanFilter*> filters;
    std::vector<DeviceAddress> addrs;
};

#endif //TH_TEMP_H
