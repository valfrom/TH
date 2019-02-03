#include <TH_temp.h>

#ifndef TH_AIR_TEMP_H
#define TH_AIR_TEMP_H

class THSensorServiceAir : public THSensorService {
public:
    THSensorServiceAir();
    float GetPumpTemp();
    float GetOutsideTemp();
    float GetTeTemp();
    float GetBoilerTemp();
private:
    short pumpTempIndex;
    short outsideTempIndex;
    short teTempIndex;
    short boilerTempIndex;
};

#endif //TH_AIR_TEMP_H
