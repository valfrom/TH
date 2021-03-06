#include <TH_conf.h>
#include "THAir_temp.h"
#include "THAir_hardware.h"
#include "THAir_config.h"

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
    THAirConfig conf;
};

#endif //TH_AIR_H
