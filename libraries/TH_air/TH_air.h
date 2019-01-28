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
    long GetPumpOnTime();
    void Update(long deltaTime);
private:
    bool fanOn;
    bool valveHeatOn;
    bool pumpOn;
    bool mainRelayOn;
    long pumpOnTime;
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
    void UpdateDefrostCool();
    void UpdatePause();
    void UpdateHeat();
    void DefrostPause();
    void DefrostCool();
    void DefrostCoolHigh();
    void Defrost();
    void Heat();
    void HeatA();
    void HeatB();
    void HeatC();
    void Start();
    void Pause();

    int currentState;
    int counter;
    int nextState;
    int previousState;

    long stateTime;
    long deviceTime;
    long oldSendTime;

    float previousTemp;

    THHardwareState hardwareState;
    THSensorServiceAir tempService;
};

#endif //TH_AIR_TEMP_H
