#include "THGround_temp.h"

THSensorServiceGround::THSensorServiceGround() : THSensorService() {
    teLowTempIndex = 3;
    outTempIndex = 1;
    inTempIndex = 0;
    teHighTempIndex = 2;
    count = 4;
}

float THSensorServiceGround::GetTeLowTemp() {
    return temperatures[teLowTempIndex];
}
float THSensorServiceGround::GetTeHighTemp() {
    return temperatures[teHighTempIndex];
}
float THSensorServiceGround::GetInTemp() {
    return temperatures[inTempIndex];
}
float THSensorServiceGround::GetOutTemp() {
    return temperatures[outTempIndex];
}
