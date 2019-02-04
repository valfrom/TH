#include <TH_conf.h>

#ifndef TH_AIR_HARDWARE_H
#define TH_AIR_HARDWARE_H

class THHardwareState {
public:
    THHardwareState();
    void SetFanOn(bool fanOn);
    bool IsFanOn();
    void SetValveHeatOn(bool valveOn);
    bool IsValveHeatOn();
    void SetPumpOn(bool pumpOn);
    bool IsPumpOn();
    long GetPumpOnTime();
    void Update(long deltaTime);
private:
    bool fanOn;
    bool valveHeatOn;
    bool pumpOn;
    bool mainRelayOn;
    long pumpOnTime;
    long previousTimePumpOn;
};

#endif //TH_AIR_HARDWARE_H