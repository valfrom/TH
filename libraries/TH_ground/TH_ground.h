#include <TH_temp.h>

#define TH_STATE_NONE 0
#define TH_STATE_START 1
#define TH_STATE_HEAT 2
#define TH_STATE_PAUSE 3
#define TH_STATE_DEFROST 4
#define TH_STATE_ERROR 7

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

    long state;
    long nextState;

    int counter;
    
    long stateTime;

    THHardwareState hardwareState;
    THSensorService tempService;
};


#endif //TH_GROUND_H