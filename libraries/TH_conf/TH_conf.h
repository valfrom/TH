#include "TH_private.h"

#define SECONDS (1000)
#define MINUTES (60 * SECONDS)
#define HOURS (60 * MINUTES)

#define PUMP_PIN D1 //D1
#define FAN_PIN D2 //D2
#define VALVE_PIN D5 //D5

#define ONE_WIRE_BUS D4  // DS18B20 on D4

#define SERVER "api.thingspeak.com"

#define TH_STATE_NONE 0
#define TH_STATE_START 1
#define TH_STATE_HEAT 2
#define TH_STATE_PAUSE 3
#define TH_STATE_DEFROST 4
#define TH_STATE_DEFROST_PAUSE 5
#define TH_STATE_DEFROST_COOL 6
#define TH_STATE_ERROR 7
#define TH_STATE_HEAT_A 8
#define TH_STATE_HEAT_B 9
#define TH_STATE_HEAT_C 10
#define TH_STATE_DEFROST_COOL_HIGH 11
