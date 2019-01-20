#include "TH_private.h"

#define SECONDS (1000)
#define MINUTES (60 * SECONDS)
#define HOURS (60 * MINUTES)

#define MAIN_RELAY_PIN 16 //D0
#define PUMP_PIN 5 //D1
#define FAN_PIN 4 //D2
#define VALVE_PIN 0 //D3

#define ONE_WIRE_BUS 2  // DS18B20 on D4

#define SERVER "api.thingspeak.com"

#define TH_STATE_NONE 0
#define TH_STATE_START 1
#define TH_STATE_HEAT 2
#define TH_STATE_PAUSE 3
#define TH_STATE_DEFROST 4
#define TH_STATE_DEFROST_PAUSE 5
#define TH_STATE_DEFROST_COOL 6
#define TH_STATE_ERROR 7
