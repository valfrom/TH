#include <TH_conf.h>
#include "TH_temp.h"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

THSensorService::THSensorService() {
    readingErrors = 0;
    pumpTempIndex = 2;
    outTempIndex = 4;
    inTempIndex = 0;
    kegDownIndex = 1;
    kegUpIndex = 3;
    RequestSensors();
    delay(1000);
    RequestSensors();
}

void THSensorService::RequestSensors() {
    DS18B20.requestTemperatures(); 
    bool wasError = false;
    float temps[5];
    for(int i=0;i<5;i++) {
        float t = DS18B20.getTempCByIndex(i);
        if(t < -126 || (t < 85.000001 && t > 84.999999)) {
            wasError = true;
        } else {
            temps[i] = t;
        }
    }
    if(wasError) {
        readingErrors ++;
    } else {
        for(int i=0;i<5;i++) {
            temperatures[i] = temps[i];
        }
        readingErrors = 0;
    }
}
bool THSensorService::IsError() {
    return readingErrors > 10;
}
float THSensorService::GetPumpTemp() {
    return temperatures[pumpTempIndex];
}
float THSensorService::GetKegDownTemp() {
    return temperatures[kegDownIndex];
}
float THSensorService::GetKegUpTemp() {
    return temperatures[kegUpIndex];
}
float THSensorService::GetInTemp() {
    return temperatures[inTempIndex];
}
float THSensorService::GetOutTemp() {
    return temperatures[outTempIndex];
}