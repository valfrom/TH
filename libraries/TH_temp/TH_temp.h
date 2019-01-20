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
    int readingErrors;
    std::vector<SimpleKalmanFilter*> filters;
};

#endif //TH_TEMP_H
