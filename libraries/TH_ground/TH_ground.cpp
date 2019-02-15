#include <ESP8266WiFi.h>
#include <TH_conf.h>
#include "TH_ground.h"

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
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);

    SetPumpOn(false);
}

void THHardwareState::SetPumpOn(bool pumpOn) {
    digitalWrite(PUMP_PIN, pumpOn?HIGH:LOW);
    this->pumpOn = pumpOn;
}

bool THHardwareState::IsPumpOn() {
    return pumpOn;
}

THDevice::THDevice() {
    currentState = TH_STATE_NONE;
    counter = 0;
    SetState(TH_STATE_START);
}

void THDevice::SetState(int newState) {
    Serial.print("Set state: ");
    Serial.println(newState);

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
        case TH_STATE_ERROR:
            Error();
            break;
        case TH_STATE_PAUSE:
            Pause();
            break;
        case TH_STATE_DEFROST:
            Defrost();
            break;
    }
}

void THDevice::SendCurrentState(bool force) {
    if(counter % 2 == 0 || force) {
        counter = 0;
        float temperatures[8];
        temperatures[0] = tempService.GetOutTemp();
        temperatures[1] = tempService.GetTeLowTemp();
        temperatures[2] = tempService.GetTeHighTemp();
        temperatures[3] = 0;
        temperatures[4] = tempService.GetInTemp();
        temperatures[5] = float(stateTime);
        temperatures[6] = float(currentState);
        temperatures[7] = float(nextState);
        sendTeperatureTS(temperatures, 8);
    }

    counter++;
}

void THDevice::Update(long deltaTime) {
    tempService.RequestSensors();
    SendCurrentState(false);

    if(stateTime > 0) {
        stateTime -= deltaTime;
        if(stateTime <= 0) {
            SetState(nextState);
        }
    }
    Serial.print("State: ");
    Serial.print(currentState);
    Serial.print(" time: ");
    Serial.println(stateTime);

    Serial.print("Out temp: ");
    Serial.println(tempService.GetOutTemp());

    Serial.print("In temp: ");
    Serial.println(tempService.GetInTemp());

    Serial.print("Te Low temp: ");
    Serial.println(tempService.GetTeLowTemp());

    Serial.print("Te High temp: ");
    Serial.println(tempService.GetTeHighTemp());

    if(IsError() || tempService.IsError()) {
        SetState(TH_STATE_ERROR);
        return;
    }
    switch(currentState) {
        case TH_STATE_HEAT:
            UpdateHeat();
            break;
        default:
            break;
    }
}

void THDevice::UpdateHeat() {
    if(tempService.GetTeLowTemp() < -8) {
        SetState(TH_STATE_DEFROST);
    }
}

bool THDevice::IsError() {
    return false;
}

void THDevice::Error() {
    hardwareState.SetPumpOn(false);

    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

    nextState = TH_STATE_START;
    stateTime = 5 * MINUTES;
}

void THDevice::Heat() {
    hardwareState.SetPumpOn(true);

    nextState = TH_STATE_DEFROST;
    stateTime = 50 * MINUTES;
    return;
}

void THDevice::Defrost() {
    if(tempService.GetTeLowTemp() > -2.0) {
        SetState(TH_STATE_HEAT);
        return;
    }
    hardwareState.SetPumpOn(false);
    nextState = TH_STATE_HEAT;

    if(tempService.GetTeLowTemp() < -8.0) {
        stateTime = 30 * MINUTES;
    } else if(tempService.GetTeLowTemp() < -6.0) {
        stateTime = 20 * MINUTES;
    } else {
        stateTime = 10 * MINUTES;
    }
}

void THDevice::Pause() {
    hardwareState.SetPumpOn(false);
    nextState = TH_STATE_HEAT;
    stateTime = 5 * MINUTES;
}

void THDevice::Start() {
    hardwareState.SetPumpOn(false);
    if(tempService.GetTeLowTemp() < 0.0) {
        nextState = TH_STATE_DEFROST;
    } else {
        nextState = TH_STATE_HEAT;
    }
    stateTime = 5 * MINUTES;
}
