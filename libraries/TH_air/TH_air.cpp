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
        values[7] = (hardwareState.IsPumpOn()?1:0) + (hardwareState.IsFanOn()?10:0) + (hardwareState.IsValveHeatOn()?100:0);
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
    hardwareState.SetValveHeatOn(false);

    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

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
        case TH_STATE_HEAT_A:
            HeatA();
            break;
        case TH_STATE_HEAT_B:
            HeatB();
            break;
        case TH_STATE_HEAT_C:
            HeatC();
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
    if(tempService.GetBoilerTemp() < 40.0) {
        SetState(TH_STATE_START);
        return;
    }
}

void THDevice::UpdateHeat() {
    if(tempService.GetBoilerTemp() > 42) {
        SetState(TH_STATE_PAUSE);
        return;
    }
    float t1 = 50;
    float t2 = 46;
    float t3 = 56;
    float t4 = 49;

    if(tempService.GetBoilerTemp() > 38.0) {
        t1 = 53;
        t2 = 49;
        t3 = 59;
        t4 = 52;
    }

    if(tempService.GetTeTemp() > t1 || !hardwareState.IsPumpOn()) {
        hardwareState.SetFanOn(false);
    } else if(tempService.GetTeTemp() < t2 && hardwareState.IsPumpOn() && hardwareState.GetPumpOnTime() > 5 * MINUTES) {
        hardwareState.SetFanOn(true);
    }
    if(tempService.GetTeTemp() > t3) {
        hardwareState.SetPumpOn(false);
    } else if(tempService.GetTeTemp() < t4) {
        hardwareState.SetPumpOn(true);
    }
    switch(currentState) {
        case TH_STATE_HEAT:
            if(hardwareState.GetTotalPumpOnTime() > 45 * MINUTES) {
                SetState(TH_STATE_HEAT_B);
            }
            break;
        case TH_STATE_HEAT_B:
            if(hardwareState.GetTotalPumpOnTime() > 70 * MINUTES) {
                SetState(TH_STATE_HEAT_C);
            }
            break;
        case TH_STATE_HEAT_C:
            if(hardwareState.GetTotalPumpOnTime() > 120 * MINUTES) {
                SetState(TH_STATE_HEAT_A);
            }
            break;
        case TH_STATE_HEAT_A:
            if(tempService.GetBoilerTemp() > 38.0 && hardwareState.GetPumpOnTime() > 40 * MINUTES && tempService.GetOutsideTemp() > 0.0 && tempService.GetTeTemp() < 50) {
                SetState(TH_STATE_DEFROST_PAUSE);
                stateTime = 30 * MINUTES;
            }
            break;
    }
}

void THDevice::DefrostPause() {
    hardwareState.SetPumpOn(false);
    nextState = TH_STATE_HEAT;
    stateTime = 2 * MINUTES;
}

void THDevice::DefrostCool() {
    hardwareState.SetFanOn(false);
    hardwareState.SetValveHeatOn(false);
    delay(1000);
    hardwareState.SetPumpOn(true);
    nextState = TH_STATE_DEFROST_PAUSE;
    stateTime = 4 * MINUTES;
}

void THDevice::DefrostCoolHigh() {
    hardwareState.SetFanOn(false);
    hardwareState.SetValveHeatOn(false);
    delay(1000);
    hardwareState.SetPumpOn(true);
    nextState = TH_STATE_DEFROST_PAUSE;
    stateTime = 6 * MINUTES;
}

void THDevice::Defrost() {
    hardwareState.SetPumpOn(false);
    if(tempService.GetOutsideTemp() > 15.0) {
        stateTime = 15 * MINUTES;
        nextState = TH_STATE_HEAT;
        return;
    } else if(tempService.GetOutsideTemp() > 0.7) {
        hardwareState.SetFanOn(true);
    } else {
        hardwareState.SetFanOn(false);
    }
    stateTime = 5 * MINUTES;
    switch(previousState) {
        case TH_STATE_HEAT_C:
            nextState = TH_STATE_DEFROST_COOL_HIGH;
            break;
        default:
            nextState = TH_STATE_DEFROST_COOL;
            break;
    }
}

void THDevice::Pause() {
    hardwareState.SetValveHeatOn(false);
    hardwareState.SetFanOn(false);
    hardwareState.SetPumpOn(false);

    nextState = TH_STATE_PAUSE;
    stateTime = 10 * MINUTES;
}

void THDevice::Heat() {
    if(tempService.GetBoilerTemp() > 42) {
        SetState(TH_STATE_PAUSE);
        return;
    }
    previousTemp = tempService.GetTeTemp();
    hardwareState.SetValveHeatOn(true);

    nextState = TH_STATE_HEAT;
    stateTime = 4 * MINUTES;
}

void THDevice::HeatA() {
    nextState = TH_STATE_HEAT_A;
    stateTime = 16 * MINUTES;
    float currentTemp = tempService.GetTeTemp();
    float delta2 = currentTemp - tempService.GetBoilerTemp();
    if(hardwareState.GetPumpOnTime() > 16 * MINUTES && delta2 < 8.0 || currentTemp < 40.0) {
        SetState(TH_STATE_DEFROST);
    }
}

void THDevice::HeatB() {
    nextState = TH_STATE_HEAT_B;
    stateTime = 16 * MINUTES;
    float currentTemp = tempService.GetTeTemp();
    float delta = fabs(currentTemp - previousTemp);
    float delta2 = currentTemp - tempService.GetBoilerTemp();
    previousTemp = currentTemp;

    if(hardwareState.GetPumpOnTime() > 5 * MINUTES && delta > 1.0 && (delta2 < 3.0 || currentTemp < 40.0)) {
        SetState(TH_STATE_DEFROST);
    }
}

void THDevice::HeatC() {
    nextState = TH_STATE_HEAT_C;
    stateTime = 16 * MINUTES;
    float currentTemp = tempService.GetTeTemp();
    float delta2 = currentTemp - tempService.GetBoilerTemp();
    if(hardwareState.GetPumpOnTime() > 5 * MINUTES && delta2 < 3.0 || currentTemp < 40.0) {
        SetState(TH_STATE_DEFROST);
    }
}

void THDevice::Start() {
    hardwareState.SetPumpOn(false);
    hardwareState.SetFanOn(false);
    hardwareState.SetValveHeatOn(false);
    nextState = TH_STATE_HEAT;
    stateTime = 2 * MINUTES;
}
