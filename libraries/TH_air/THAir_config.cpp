#include "THAir_config.h"

THAirConfig::THAirConfig() {
    SetMaxBoilerTemp(44);
}

void THAirConfig::SetMaxBoilerTemp(float temp) {
    maxBoilerTemp = temp;
}

float THAirConfig::GetMaxBoilerTemp() {
    return maxBoilerTemp;
}

float THAirConfig::GetMinBoilerTemp() {
    return GetMaxBoilerTemp() - 1;
}

float THAirConfig::GetMaxPumpHeatTemp() {
    return 60;
}

float THAirConfig::GetMaxPumpTeDiff() {
    return 3.0;
}
