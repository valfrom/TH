#include "THAir_temp.h"

THSensorServiceAir::THSensorServiceAir() {
    pumpTempIndex = 0;
    outsideTempIndex = 1;
    teTempIndex = 2;
    count = 3;
}

float THSensorServiceAir::GetPumpTemp() {
    return temperatures[pumpTempIndex];
}
float THSensorServiceAir::GetOutsideTemp() {
    return temperatures[outsideTempIndex];
}
float THSensorServiceAir::GetTeTemp() {
    return temperatures[teTempIndex];
}
