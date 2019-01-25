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

void swap(float *xp, float *yp)
{
    float temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// An optimized version of Bubble Sort
void bubbleSort(float arr[], int n)
{
   int i, j;
   bool swapped;
   for (i = 0; i < n-1; i++)
   {
     swapped = false;
     for (j = 0; j < n-i-1; j++)
     {
        if (arr[j] > arr[j+1])
        {
           swap(&arr[j], &arr[j+1]);
           swapped = true;
        }
     }

     // IF no two elements were swapped by inner loop, then break
     if (swapped == false)
        break;
   }
}

float GetMedianTemperature(DeviceAddress& addr) {
    float t[5];
    for(int i=0;i<5;i++) {
        DS18B20.requestTemperaturesByAddress(addr);
        t[i] = DS18B20.getTempC(addr);
        Serial.println(t[i]);
        delay(2000);
    }
    bubbleSort(t, 5);
    return t[2];
}

bool THSensorService::MapAddresses() {
    sensorsMapped = count > 0;
    for(int i=0;i<count;i++) {
        if(!DS18B20.getAddress(addrs[i], i)) {
            sensorsMapped = false;
            break;
        }
        Serial.print("Mapped: ");
        for(int j=0;j<8;j++) {
            Serial.print(addrs[i][j]);
        }
        Serial.println();
    }
}

void THSensorService::RequestSensors() {
    if(!sensorsMapped) {
        MapAddresses();
        if(!sensorsMapped) {
            readingErrors++;
            return;
        }
    }
    if(filters.size() != count) {
        filters.resize(count);
        for(int i=0;i<count;i++) {
            filters[i] = new SimpleKalmanFilter(0.5, 0.5, 0.1);
        }
    }
    bool wasError = false;
    float temps[count];
    for(int i=0;i<count;i++) {
        float t = GetMedianTemperature(addrs[i]);
        if(t < -126 || (t < 85.000001 && t > 84.999999)) {
            wasError = true;
            Serial.print("Sensor error: ");
            Serial.print(i);
            Serial.print(" temp: ");
            Serial.println(t);
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
