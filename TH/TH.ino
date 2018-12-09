#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <TH_temp.h>
#include <TH_ground.h>

#include <TH_conf.h> //Or uncomment code below
/*#define MAIN_RELAY_PIN 16 //D0
#define PUMP_PIN 5 //D1
#define FAN_PIN 4 //D2
#define VALVE_PIN 0 //D3
#define ONE_WIRE_BUS 2  // DS18B20 on D4
const char* ssid = "Wifiname";
const char* password = "wifipass";
const char* server = "api.thingspeak.com";
String apiKey ="API_KEY";*/

THDevice* device;

void setup() {
    device = new THDevice();

    Serial.begin(115200);
    Serial.println("Booting TH-wifi");
    delay(5000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    ArduinoOTA.setHostname("TH-wifi");

    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());    
}

void loop() {
    ArduinoOTA.handle();
    device->Update(1000 * 45 / 30);
    delay(1000);
}
