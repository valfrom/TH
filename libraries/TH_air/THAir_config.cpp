#include "THAir_config.h"

THAirConfig::THAirConfig() {
    SetMaxBoilerTemp(42);
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
