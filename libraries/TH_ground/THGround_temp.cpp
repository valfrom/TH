#include "THGround_temp.h"

THSensorServiceGround::THSensorServiceGround() : THSensorService() {
    pumpTempIndex = 2;
    outTempIndex = 4;
    inTempIndex = 0;
    kegDownIndex = 1;
    kegUpIndex = 3;
}

float THSensorServiceGround::GetPumpTemp() {
    return temperatures[pumpTempIndex];
}
float THSensorServiceGround::GetKegDownTemp() {
    return temperatures[kegDownIndex];
}
float THSensorServiceGround::GetKegUpTemp() {
    return temperatures[kegUpIndex];
}
float THSensorServiceGround::GetInTemp() {
    return temperatures[inTempIndex];
}
float THSensorServiceGround::GetOutTemp() {
    return temperatures[outTempIndex];
}
