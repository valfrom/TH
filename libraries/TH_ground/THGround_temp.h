#include <TH_temp.h>

#ifndef TH_GROUND_TEMP_H
#define TH_GROUND_TEMP_H

class THSensorServiceGround : public THSensorService {
public:
    THSensorServiceGround();
    float GetPumpTemp();
    float GetKegDownTemp();
    float GetKegUpTemp();
    float GetInTemp();
    float GetOutTemp();
private:
    short pumpTempIndex;
    short outTempIndex;
    short inTempIndex;
    short kegDownIndex;
    short kegUpIndex;
};

#endif //TH_GROUND_TEMP_H
