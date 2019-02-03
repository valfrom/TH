#include "THAir_hardware.h"
#include <ESP8266WiFi.h>

THHardwareState::THHardwareState() {
    SetPumpOn(false);
    SetFanOn(false);
    SetValveHeatOn(false);
}

void THHardwareState::SetFanOn(bool fanOn) {
    pinMode(FAN_PIN, fanOn?OUTPUT:INPUT_PULLUP);
    this->fanOn = fanOn;
}

bool THHardwareState::IsFanOn() {
    return fanOn;
}

void THHardwareState::SetValveHeatOn(bool valveOn) {
    pinMode(VALVE_PIN, valveOn?OUTPUT:INPUT_PULLUP);
    this->valveHeatOn = valveOn;
}

bool THHardwareState::IsValveHeatOn() {
    return valveHeatOn;
}

void THHardwareState::Update(long deltaTime) {
    if(pumpOn) {
        pumpOnTime += deltaTime;
    }
}

long THHardwareState::GetPumpOnTime() {
    return pumpOnTime;
}

void THHardwareState::SetPumpOn(bool pumpOn) {
    pinMode(PUMP_PIN, pumpOn?OUTPUT:INPUT_PULLUP);
    digitalWrite(LED_BUILTIN, pumpOn?HIGH:LOW);
    this->pumpOn = pumpOn;
    if(!pumpOn) {
        pumpOnTime = 0;
    }
}

bool THHardwareState::IsPumpOn() {
    return pumpOn;
}