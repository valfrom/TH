#include <TH_conf.h>

#ifndef TH_AIR_CONFIG_H
#define TH_AIR_CONFIG_H

class THAirConfig {
public:
    THAirConfig();
    void SetMaxBoilerTemp(float temp);
    float GetMaxBoilerTemp();
    float GetMinBoilerTemp();
    float GetMaxPumpHeatTemp();
    float GetMaxPumpTeDiff();
private:
    float maxBoilerTemp;
};


#endif //TH_AIR_CONFIG_H