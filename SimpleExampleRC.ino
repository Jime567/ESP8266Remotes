#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <fauxmoESP.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <RCSwitch.h>

#define ledPin 2
const uint16_t kIrLed = 5;  //Pin D1 for IR led
const uint16_t radio = 4; //Pin D2 for radio transmit



fauxmoESP fauxmo;
ESP8266WebServer server(81);
RCSwitch rf = RCSwitch();

void setup() { 
  Serial.begin(115200);

  rf.enableTransmit(radio);
  rf.setProtocol(1);

  if (setupNetwork()) {
    // Setup fauxmo
     Serial.println("Adding Devices");
     fauxmo.setPort(80);  
     fauxmo.enable(true);
     // Add devices here
     fauxmo.addDevice("RCDevice");    
    
    // Set a callback function for when a device is found
     fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);       
       if (strcmp(device_name, "RCDevice") == 0) {
        if (state) {
          Serial.println("RCDevice On");
          RCDeviceOn();
        }
        else {
          Serial.println("RCDevice Off");
          RCDeviceOff();
        }
       }
     });

  }
}

void loop() {
  fauxmo.handle();
  server.handleClient();
}

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

//Server Routing
void routeServer () {
  server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("ESP8266 Controller Basement"));
    });
    
  server.on("/rcDeviceOn", HTTP_GET, RCDeviceOn);
  server.on("/rcDeviceOff", HTTP_GET, RCDeviceOff);
  server.on("/rcDeviceStop", HTTP_GET, RCDeviceStop);

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

//RCDevice Commands
void RCDeviceOn() {
  server.send(200, "text/plain", "RCDevice On");
  rf.send(6582737, 24);  
  delay(300);
  rf.send(6582737, 24);  
}

void RCDeviceOff() {
  server.send(200, "text/plain", "RCDevice Off");
  rf.send(6582740, 24); 
  delay(300);
  rf.send(6582740, 24);
}

void RCDeviceStop() {
  server.send(200, "text/plain", "RCDevice Stop");
  rf.send(6582738, 24);  
}
