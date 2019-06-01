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
    long GetFanOffTime();
    long GetTotalPumpOnTime();
    void Update(long deltaTime);
    void SetFanLockTime(long time);
private:
    bool fanOn;
    bool valveHeatOn;
    bool pumpOn;
    bool mainRelayOn;
    long pumpOnTime;
    long fanOnTime;
    long pumpTotalOnTime;
    long previousTimePumpOn;

    long fanLockTime;
};

#endif //TH_AIR_HARDWARE_H