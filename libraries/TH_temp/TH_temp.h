#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef TH_TEMP_H
#define TH_TEMP_H

class THSensorService {
public:
    THSensorService();
    void RequestSensors();
    bool IsError();
    float GetPumpTemp();
    float GetKegDownTemp();
    float GetKegUpTemp();
    float GetInTemp();
    float GetOutTemp();
private:
    float temperatures[8];
    short pumpTempIndex;
    short outTempIndex;
    short inTempIndex;
    short kegDownIndex;
    short kegUpIndex;
    int readingErrors;
};

#endif //TH_TEMP_H