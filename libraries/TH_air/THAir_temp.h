#include <TH_temp.h>

#ifndef TH_GROUND_TEMP_H
#define TH_GROUND_TEMP_H

class THSensorServiceAir : public THSensorService {
public:
    THSensorServiceAir();
    float GetPumpTemp();
    float GetOutsideTemp();
    float GetTeTemp();
private:
    short pumpTempIndex;
    short outsideTempIndex;
    short teTempIndex;
};

#endif //TH_GROUND_TEMP_H
