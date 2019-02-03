#include <TH_conf.h>
#include "THAir_temp.h"
#include "THAir_hardware.h"

#ifndef TH_AIR_H
#define TH_AIR_H

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

#endif //TH_AIR_H
