#include <TH_conf.h>
#include "THAir_temp.h"

#ifndef TH_AIR_TEMP_H
#define TH_AIR_TEMP_H

class THHardwareState {
public:
    THHardwareState();
    void SetMainRelayOn(bool relayOn);
    bool IsRelayOn();
    void SetFanOn(bool fanOn);
    bool IsFanOn();
    void SetValveHeatOn(bool valveOn);
    bool IsValveHeatOn();
    void SetPumpOn(bool pumpOn);
    bool IsPumpOn();
private:
    bool fanOn;
    bool valveHeatOn;
    bool pumpOn;
    bool mainRelayOn;
};

class THDevice {
public:
    THDevice();
    void SendCurrentState(bool force);
    void SetState(int newState);
    void Update(long deltaTime);
private:
    bool IsError();
    void Error();
    void UpdateDefrost();
    void DefrostPause();
    void DefrostCool();
    void Defrost();
    void Heat();
    void Start();

    int currentState;
    int counter;
    int nextState;

    long stateTime;

    THHardwareState hardwareState;
    THSensorServiceAir tempService;
};

#endif //TH_AIR_TEMP_H
