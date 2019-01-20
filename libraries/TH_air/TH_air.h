#include <TH_conf.h>
#include "THAir_temp.h"

#ifndef TH_AIR_TEMP_H
#define TH_AIR_TEMP_H

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

class THHardwareState {
public:
    THHardwareState() {
        SetPumpOn(false);
        SetFanOn(false);
        SetValveHeatOn(false);
    }
    void SetMainRelayOn(bool relayOn) {
        mainRelayOn = relayOn;
        pinMode(MAIN_RELAY_PIN, relayOn?OUTPUT:INPUT_PULLUP);
    }
    bool IsRelayOn() {
        return mainRelayOn;
    }
    void SetFanOn(bool fanOn) {
        pinMode(FAN_PIN, fanOn?OUTPUT:INPUT_PULLUP);
        this->fanOn = fanOn;
    }
    bool IsFanOn() {
        return fanOn;
    }
    void SetValveHeatOn(bool valveOn) {
        pinMode(VALVE_PIN, valveOn?OUTPUT:INPUT_PULLUP);
        this->valveHeatOn = valveOn;
    }
    bool IsValveHeatOn() {
        return valveHeatOn;
    }
    void SetPumpOn(bool pumpOn) {
        pinMode(PUMP_PIN, pumpOn?OUTPUT:INPUT_PULLUP);
        digitalWrite(LED_BUILTIN, pumpOn?HIGH:LOW);
        this->pumpOn = pumpOn;
    }
    bool IsPumpOn() {
        return pumpOn;
    }
private:
    bool fanOn;
    bool valveHeatOn;
    bool pumpOn;
    bool mainRelayOn;
};

class THDevice {
public:
    THDevice() {
        counter = 0;
        SetState(TH_STATE_START);
    }
    void SendCurrentState(bool force) {
        if(counter % 60 == 0 || force) {
            counter = 0;
            float temperatures[8];
            temperatures[0] = tempService.GetOutsideTemp();
            temperatures[1] = tempService.GetPumpTemp();
            temperatures[2] = tempService.GetTeTemp();
            temperatures[3] = 0;
            temperatures[4] = 0;
            temperatures[5] = float(stateTime);
            temperatures[6] = float(currentState);
            temperatures[7] = float(nextState);
            sendTeperatureTS(temperatures, 8);
        }

        counter++;
    }
    void SetState(int newState) {
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
    void Update(long deltaTime) {
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
            UpdateDefrost();
        }
        Serial.println(stateTime);
    }
private:
    bool IsError() {
        Serial.print("Pump temp: ");
        Serial.println(tempService.GetPumpTemp());

        Serial.print("Out temp: ");
        Serial.println(tempService.GetOutsideTemp());

        Serial.print("Te temp: ");
        Serial.println(tempService.GetTeTemp());

        if(tempService.GetPumpTemp() > 84 || tempService.GetPumpTemp() < -7) {
            return true;
        }
        return false;
    }
    void Error() {
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

        nextState = TH_STATE_HEAT;
        stateTime = 2 * MINUTES;
    }
    void UpdateDefrost() {
        if(tempService.GetTeTemp() > 1.0) {
            SetState(TH_STATE_DEFROST_PAUSE);
        }
    }
    void DefrostPause() {
        hardwareState.SetPumpOn(false);
        nextState = TH_STATE_HEAT;
        stateTime = 2 * MINUTES;
    }
    void DefrostCool() {
        hardwareState.SetValveHeatOn(false);
        delay(1000);
        hardwareState.SetPumpOn(true);
        nextState = TH_STATE_DEFROST_PAUSE;
        stateTime = 10 * MINUTES;
    }
    void Defrost() {
        if(tempService.GetOutsideTemp() > 10) {
            stateTime = 5 * SECONDS;
            nextState = TH_STATE_HEAT;
            return ;
        }
        hardwareState.SetPumpOn(false);
        hardwareState.SetFanOn(false);
        stateTime = 1 * MINUTES;
        nextState = TH_STATE_DEFROST_COOL;
    }
    void Heat() {
        hardwareState.SetValveHeatOn(true);
        delay(1000);
        hardwareState.SetFanOn(true);
        delay(1000);
        hardwareState.SetPumpOn(true);

        nextState = TH_STATE_DEFROST;
        stateTime = 50 * MINUTES;
    }
    void Start() {
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

    int currentState;
    int counter;
    int nextState;

    long stateTime;

    THHardwareState hardwareState;
    THSensorServiceAir tempService;
};

#endif //TH_AIR_TEMP_H
