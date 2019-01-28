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
protected:
    short count;
    float temperatures[8];
private:
    bool MapAddresses();
    bool sensorsMapped;
    int readingErrors;
    SimpleKalmanFilter* filters[8];
    DeviceAddress addrs[8];
};

#endif //TH_TEMP_H
