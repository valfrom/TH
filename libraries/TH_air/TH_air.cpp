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

THDevice::THDevice() {
    counter = 0;
    deviceTime = 0;
    oldSendTime = 0;
    previousState = TH_STATE_NONE;
    SetState(TH_STATE_START);
}

void THDevice::SendCurrentState(bool force) {
    long delta = deviceTime - oldSendTime;
    if(delta > 120000 || force) {
        counter = 0;
        float values[8];
        values[0] = tempService.GetOutsideTemp();
        values[1] = tempService.GetPumpTemp();
        values[2] = tempService.GetTeTemp();
        values[3] = tempService.GetBoilerTemp();
        values[4] = hardwareState.GetPumpOnTime();
        values[5] = float(stateTime);
        values[6] = float(currentState);
        values[7] = (hardwareState.IsPumpOn()?1:0) + (hardwareState.IsFanOn()?10:0) + (hardwareState.IsValveHeatOn()?100:0) + 
                TH_VERSION * 1000;
        sendTeperatureTS(values, 8);
    }

    counter++;
}

bool THDevice::IsError() {
    if(tempService.GetPumpTemp() > 70 || tempService.GetPumpTemp() < -7) {
        return true;
    }
    return false;
}

void THDevice::Error() {
    //Turn off all
    hardwareState.SetPumpOn(false);
    hardwareState.SetFanOn(false);

    delay(2000);
    hardwareState.SetValveHeatOn(false);

    nextState = TH_STATE_START;
    stateTime = 2 * MINUTES;
}

void THDevice::SetState(int newState) {
    bool needToSend = currentState != newState;
    previousState = currentState;
    currentState = newState;

    Serial.print("Pump temp: ");
    Serial.println(tempService.GetPumpTemp());

    Serial.print("Out temp: ");
    Serial.println(tempService.GetOutsideTemp());

    Serial.print("Te temp: ");
    Serial.println(tempService.GetTeTemp());

    Serial.print("Boiler temp: ");
    Serial.println(tempService.GetBoilerTemp());

    Serial.print("Current state: ");
    Serial.println(currentState);

    Serial.print("Pump time on: ");
    Serial.println(hardwareState.GetPumpOnTime());

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
        case TH_STATE_DEFROST_COOL_HIGH:
            DefrostCoolHigh();
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
    hardwareState.Update(deltaTime);
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

    switch(currentState) {
        case TH_STATE_DEFROST_COOL:
        case TH_STATE_DEFROST_COOL_HIGH:
            UpdateDefrostCool();
            break;
        case TH_STATE_HEAT:
        case TH_STATE_HEAT_A:
        case TH_STATE_HEAT_B:
        case TH_STATE_HEAT_C:
            UpdateHeat();
            break;
        case TH_STATE_PAUSE:
            UpdatePause();
            break;
    }
    Serial.println(stateTime);
}

void THDevice::UpdateDefrostCool() {
    if(tempService.GetPumpTemp() > 60) {
        hardwareState.SetPumpOn(false);
    }
}

void THDevice::UpdatePause() {
    if(tempService.GetBoilerTemp() < conf.GetMinBoilerTemp()) {
        SetState(TH_STATE_START);
        return;
    }
}

void THDevice::UpdateHeat() {
    if(tempService.GetBoilerTemp() > conf.GetMaxBoilerTemp()) {
        SetState(TH_STATE_PAUSE);
        return;
    }

    if(hardwareState.GetPumpOnTime() > 30 * MINUTES && tempService.GetTeTemp() < 40) {
        Defrost();
    }
}

void THDevice::DefrostPause() {
    hardwareState.SetFanOn(true);
    hardwareState.SetPumpOn(false);
    nextState = TH_STATE_HEAT;
    stateTime = 6 * MINUTES;
}

void THDevice::DefrostCool() {
    hardwareState.SetFanOn(false);
    hardwareState.SetValveHeatOn(false);
    delay(1000);
    hardwareState.SetPumpOn(true);
    nextState = TH_STATE_DEFROST_PAUSE;
    stateTime = 2 * MINUTES;
}

void THDevice::DefrostCoolHigh() {
    hardwareState.SetFanOn(false);
    hardwareState.SetValveHeatOn(false);
    delay(2000);
    hardwareState.SetPumpOn(true);
    nextState = TH_STATE_DEFROST_PAUSE;
    stateTime = 4 * MINUTES;
}

void THDevice::Defrost() {
    hardwareState.SetPumpOn(false);
    hardwareState.SetFanOn(false);

    if(tempService.GetOutsideTemp() > 1.0) {
        DefrostPause();
        return;
    }

    if(tempService.GetOutsideTemp() > -2.0) {
        DefrostCool();
        return;
    }
    DefrostCoolHigh();
}

void THDevice::Pause() {
    hardwareState.SetFanOn(false);
    hardwareState.SetPumpOn(false);
    delay(2000);
    hardwareState.SetValveHeatOn(false);

    nextState = TH_STATE_PAUSE;
    stateTime = 10 * MINUTES;
}

void THDevice::Heat() {
    if(tempService.GetBoilerTemp() > conf.GetMaxBoilerTemp()) {
        SetState(TH_STATE_PAUSE);
        return;
    }
    previousTemp = tempService.GetTeTemp();
    hardwareState.SetValveHeatOn(true);
    hardwareState.SetFanOn(true);
    delay(5000);

    nextState = TH_STATE_HEAT;
    stateTime = 30 * MINUTES;
}

void THDevice::Start() {
    hardwareState.SetPumpOn(false);
    hardwareState.SetFanOn(false);
    hardwareState.SetValveHeatOn(false);
    nextState = TH_STATE_HEAT;
    stateTime = 4 * MINUTES;
}
