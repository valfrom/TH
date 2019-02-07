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
    long GetFanOnTime();
    long GetTotalPumpOnTime();
    void Update(long deltaTime);
private:
    bool fanOn;
    bool valveHeatOn;
    bool pumpOn;
    bool mainRelayOn;
    long pumpOnTime;
    long fanOnTime;
    long pumpTotalOnTime;
    long previousTimePumpOn;
};

#endif //TH_AIR_HARDWARE_H