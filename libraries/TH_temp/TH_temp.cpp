#include <TH_conf.h>
#include <SimpleKalmanFilter.h>
#include "TH_temp.h"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

THSensorService::THSensorService() {
    readingErrors = 0;
    count = 0;
    RequestSensors();
    delay(1000);
    RequestSensors();
}

void THSensorService::RequestSensors() {
    DS18B20.requestTemperatures();
    if(filters.size() != count) {
        filters.resize(count);
        for(int i=0;i<count;i++) {
            filters[i] = new SimpleKalmanFilter(10, 10, 0.1);
        }
    }
    bool wasError = false;
    float temps[count];
    for(int i=0;i<count;i++) {
        float t = DS18B20.getTempCByIndex(i);
        if(t < -126 || (t < 85.000001 && t > 84.999999)) {
            wasError = true;
        } else {
            temps[i] = filters[i]->updateEstimate(t);
        }
    }
    if(wasError) {
        readingErrors ++;
    } else {
        for(int i=0;i<count;i++) {
            temperatures[i] = temps[i];
        }
        readingErrors = 0;
    }
}
bool THSensorService::IsError() {
    return readingErrors > 10;
}
