//This example only has an example of an IR-controlled device

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <fauxmoESP.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define ledPin 2
const uint16_t kIrLed = 4;  //Pin D2 for IR led



fauxmoESP fauxmo;
ESP8266WebServer server(81);
IRsend irsend(kIrLed);

void setup() { 
  Serial.begin(115200);
  irsend.begin();

  if (setupNetwork()) {
    // Setup fauxmo and set to port 80
     Serial.println("Adding Devices");
     fauxmo.setPort(80);  
     fauxmo.enable(true);
     //Add each device here
     fauxmo.addDevice("Device");
     

    // Set a callback function for when a device is found
     fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);       
        if (strcmp(device_name, "Device") == 0) {
          if (state) {
            Serial.println("Turning on Device");
            deviceOn();
          }
          else {
            Serial.println("Turning off Device");
            devicOff();
          }
       }
     });

  }
}

//start fauxmo and our webserver
void loop() {
  fauxmo.handle();
  server.handleClient();
}

//Connect to wifi and start the webserver
boolean setupNetwork() {
  // Let us connect to WiFi
  WiFi.mode(WIFI_STA);
  String ssid = "<Wifi-Network-Name>";
  String password = "<Wifi-Network-Password>";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".......");
  Serial.println("WiFi Connected....IP Address:");

  Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());

  routeServer();
  server.enableCORS(true);
  server.begin();
  Serial.println("HTTP server live");
  
  return true;
}

//Set the endpoints for the webserver
void routeServer () {
  server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("ESP8266 Controller Living Room"));
    });
    
  server.on("/deviceOn", HTTP_GET, deviceOn);
  server.on("/devceOff", HTTP_GET, deviceOff);

  server.onNotFound(handleNotFound);
}

void handleNotFound()
{
    if (server.method() == HTTP_OPTIONS)
    {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Max-Age", "10000");
        server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "*");
        server.send(204);
    }
    else
    {
        server.send(404, "text/plain", "");
    }
}

//Functions for each Device command (on/off)
void deviceOn() {
  Serial.println("Turning on Device");
  server.send(200, "text/plain", "On");
  irsend.sendNEC(0xC1AA09F6, 32, 2);
  delay(500);
}

void deviceOff() {
  Serial.println("Turning off Device");
  server.send(200, "text/plain", "Off");
  irsend.sendNEC(0xC1AA8976, 32, 2);
  delay(500);
}
