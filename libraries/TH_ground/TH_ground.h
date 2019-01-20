#include "THGround_temp.h"

#ifndef TH_GROUND_H
#define TH_GROUND_H

class THHardwareState {
public:
    THHardwareState();
    void SetPumpOn(bool pumpOn);
    bool IsPumpOn();
private:
    bool pumpOn;
};

class THDevice {
public:
    THDevice();
    void SetState(int newState);
    void SendCurrentState(bool force);
    void Update(long deltaTime);
private:
    bool IsError();
    void Error();
    void Heat();
    void Defrost();
    void Pause();
    void Start();

    void UpdateHeat();

    int currentState;
    int nextState;

    int counter;

    long stateTime;

    THHardwareState hardwareState;
    THSensorServiceGround tempService;
};


#endif //TH_GROUND_H
