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
