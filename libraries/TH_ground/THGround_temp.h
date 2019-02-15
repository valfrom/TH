#include <TH_temp.h>

#ifndef TH_GROUND_TEMP_H
#define TH_GROUND_TEMP_H

class THSensorServiceGround : public THSensorService {
public:
    THSensorServiceGround();
    float GetTeLowTemp();
    float GetTeHighTemp();
    float GetInTemp();
    float GetOutTemp();
private:
    short teLowTempIndex;
    short outTempIndex;
    short inTempIndex;
    short teHighTempIndex;
};

#endif //TH_GROUND_TEMP_H
