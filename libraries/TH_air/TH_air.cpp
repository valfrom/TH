#include <ESP8266WiFi.h>
#include "TH_air.h"

void sendTeperatureTS(float* temps, int count){
    WiFiClient client;

    if (client.connect(SERVER, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
        Serial.println("WiFi Client connected ");

        String postStr = API_KEY;
        for(int i=0;i<count;i++) {
            postStr += "&field" + String(i+1)+"=";
            postStr += String(temps[i]);
        }

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + String(API_KEY) + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
        delay(1000);

   }//end if
   client.stop();
}

THHardwareState::THHardwareState() {
    SetPumpOn(false);
    SetFanOn(false);
    SetValveHeatOn(false);
}

void THHardwareState::SetMainRelayOn(bool relayOn) {
    mainRelayOn = relayOn;
    pinMode(MAIN_RELAY_PIN, relayOn?OUTPUT:INPUT_PULLUP);
}

bool THHardwareState::IsRelayOn() {
    return mainRelayOn;
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

void THHardwareState::SetPumpOn(bool pumpOn) {
    pinMode(PUMP_PIN, pumpOn?OUTPUT:INPUT_PULLUP);
    digitalWrite(LED_BUILTIN, pumpOn?HIGH:LOW);
    this->pumpOn = pumpOn;
}

bool THHardwareState::IsPumpOn() {
    return pumpOn;
}

THDevice::THDevice() {
    counter = 0;
    deviceTime = 0;
    oldSendTime = 0;
    SetState(TH_STATE_START);
}

void THDevice::SendCurrentState(bool force) {
    long delta = deviceTime - oldSendTime;
    if(delta > 120000 || force) {
        counter = 0;
        float temperatures[8];
        temperatures[0] = tempService.GetOutsideTemp();
        temperatures[1] = tempService.GetPumpTemp();
        temperatures[2] = tempService.GetTeTemp();
        temperatures[3] = tempService.GetBoilerTemp();
        temperatures[4] = 0;
        temperatures[5] = float(stateTime);
        temperatures[6] = float(currentState);
        temperatures[7] = float(nextState);
        sendTeperatureTS(temperatures, 8);
    }

    counter++;
}

void THDevice::SetState(int newState) {
    bool needToSend = currentState != newState;
    currentState = newState;

    Serial.print("Current state: ");
    Serial.println(currentState);

    if(needToSend) {
        SendCurrentState(true);
    }

    switch(newState) {
        case TH_STATE_START:
            Start();
            break;
        case TH_STATE_HEAT:
            Heat();
            break;
        case TH_STATE_PAUSE:
            Pause();
            break;
        case TH_STATE_DEFROST:
            Defrost();
            break;
        case TH_STATE_DEFROST_COOL:
            DefrostCool();
            break;
        case TH_STATE_DEFROST_PAUSE:
            DefrostPause();
            break;
        case TH_STATE_ERROR:
            Error();
            break;
    }
}

void THDevice::Update(long deltaTime) {
    deviceTime += deltaTime;
    tempService.RequestSensors();
    SendCurrentState(false);
    if(IsError() || tempService.IsError()) {
        SetState(TH_STATE_ERROR);
        return;
    }
    if(stateTime > 0) {
        stateTime -= deltaTime;
        if(stateTime <= 0) {
            SetState(nextState);
        }
    }
    if(currentState == TH_STATE_DEFROST_COOL) {
        UpdateDefrostCool();
    }
    Serial.println(stateTime);
}

bool THDevice::IsError() {
    Serial.print("Pump temp: ");
    Serial.println(tempService.GetPumpTemp());

    Serial.print("Out temp: ");
    Serial.println(tempService.GetOutsideTemp());

    Serial.print("Te temp: ");
    Serial.println(tempService.GetTeTemp());

    Serial.print("Boiler temp: ");
    Serial.println(tempService.GetBoilerTemp());

    if(tempService.GetPumpTemp() > 80 || tempService.GetPumpTemp() < -7) {
        return true;
    }
    return false;
}

void THDevice::Error() {
    //Turn off all
    hardwareState.SetPumpOn(false);
    hardwareState.SetFanOn(false);
    hardwareState.SetValveHeatOn(false);

    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

    hardwareState.SetMainRelayOn(false);

    nextState = TH_STATE_START;
    stateTime = 2 * MINUTES;
}

void THDevice::UpdateDefrostCool() {
    if(tempService.GetTeTemp() > 1.0) {
        SetState(TH_STATE_DEFROST_PAUSE);
    }
}

void THDevice::DefrostPause() {
    hardwareState.SetPumpOn(false);
    nextState = TH_STATE_HEAT;
    stateTime = 2 * MINUTES;
}

void THDevice::DefrostCool() {
    hardwareState.SetValveHeatOn(false);
    delay(1000);
    hardwareState.SetPumpOn(true);
    nextState = TH_STATE_DEFROST_PAUSE;
    stateTime = 3 * MINUTES;
}

void THDevice::Defrost() {
    if(tempService.GetOutsideTemp() > 15) {
        stateTime = 5 * SECONDS;
        nextState = TH_STATE_HEAT;
        return ;
    }
    if(tempService.GetTeTemp() > 3.0 && tempService.GetOutsideTemp() > 3.0) {
        stateTime = 5 * MINUTES;
        hardwareState.SetPumpOn(false);
        hardwareState.SetFanOn(true);
        nextState = TH_STATE_HEAT;
        return;
    }
    hardwareState.SetPumpOn(false);
    hardwareState.SetFanOn(false);
    stateTime = 1 * MINUTES;
    nextState = TH_STATE_DEFROST_COOL;
}

void THDevice::Pause() {
    hardwareState.SetValveHeatOn(false);
    hardwareState.SetFanOn(false);
    hardwareState.SetPumpOn(false);

    nextState = TH_STATE_HEAT;
    stateTime = 5 * MINUTES;
}

void THDevice::Heat() {
    if(tempService.GetBoilerTemp() > 40) {
        SetState(TH_STATE_PAUSE);
        return;
    }
    hardwareState.SetValveHeatOn(true);
    delay(1000);
    hardwareState.SetFanOn(true);
    delay(1000);
    hardwareState.SetPumpOn(true);

    nextState = TH_STATE_DEFROST;
    stateTime = 50 * MINUTES;
}

void THDevice::Start() {
    hardwareState.SetPumpOn(false);
    hardwareState.SetFanOn(false);
    hardwareState.SetValveHeatOn(false);
    delay(1000);
    hardwareState.SetMainRelayOn(true);
    if(tempService.GetOutsideTemp() > 0) {
        nextState = TH_STATE_HEAT;
        stateTime = 2 * MINUTES;
    } else {
        nextState = TH_STATE_DEFROST;
        stateTime = 2 * MINUTES;
    }
}
