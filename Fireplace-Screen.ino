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
IRsend irsend(kIrLed);
RCSwitch rf = RCSwitch();

void setup() { 
  Serial.begin(115200);
  irsend.begin();

  rf.enableTransmit(radio);
  rf.setProtocol(1);

  if (setupNetwork()) {
    // Setup fauxmo
     Serial.println("Adding Devices");
     fauxmo.setPort(80);  
     fauxmo.enable(true);
     // Add devices here
     fauxmo.addDevice("Hearth");  
     fauxmo.addDevice("Screen");   
    
    // Set a callback function for when a device is found
     fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);       
        if (strcmp(device_name, "Hearth") == 0) {
          if (state) {
            Serial.println("Turning on Hearth");
            fireplaceOnOff();
          }
          else {
            Serial.println("Turning off Hearth");
            fireplaceOnOff();
          }
       }
       if (strcmp(device_name, "Screen") == 0) {
        if (state) {
          Serial.println("Lowering Screen");
          screenDown();
        }
        else {
          Serial.println("Raising Screen");
          screenUp();
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
    


  server.on("/fireplaceOn", HTTP_GET, fireplaceOnOff);
  server.on("/fireplaceHeat", HTTP_GET, fireplaceHeat);
  server.on("/fireplaceTimer", HTTP_GET, fireplaceTimer);
  server.on("/fireplaceDimmer", HTTP_GET, fireplaceDimmer);
  server.on("/fireplaceSpeed", HTTP_GET, fireplaceSpeed);
  server.on("/fireplaceA", HTTP_GET, fireplaceA);
  server.on("/fireplaceB", HTTP_GET, fireplaceB);

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


//Fireplace Commands
void fireplaceOnOff() {
  Serial.println("Fireplace On/off");
  server.send(200, "text/plain", "On/off");
  irsend.sendNEC(0x10EF8877, 32, 3);
  delay(500);
}

void fireplaceHeat() {
  Serial.println("Heat");
  server.send(200, "text/plain", "Heat");
  irsend.sendNEC(0x10EF28D7, 32, 3);
  delay(500);
}

void fireplaceTimer() {
  Serial.println("Timer");
  server.send(200, "text/plain", "Timer");
  irsend.sendNEC(0x10EFC837, 32, 3);
  delay(500);
}

void fireplaceDimmer() {
  Serial.println("Dimmer");
  server.send(200, "text/plain", "Dimmer");
  irsend.sendNEC(0x10EF48B7, 32, 3);
  delay(500);
}

void fireplaceSpeed() {
  Serial.println("Speed");
  server.send(200, "text/plain", "Speed");
  irsend.sendNEC(0x10EFA857, 32, 3);
  delay(500);
}

void fireplaceA() {
  Serial.println("Color A");
  server.send(200, "text/plain", "A");
  irsend.sendNEC(0x10EFB04F, 32, 3);
  delay(500);
}

void fireplaceB() {
  Serial.println("Color B");
  server.send(200, "text/plain", "B");
  irsend.sendNEC(0x10EF708F, 32, 3);
  delay(500);
}

//Screen Commands
void screenUp() {
  server.send(200, "text/plain", "Screen Up");
  Serial.println("Up a doo");
  rf.send(6582737, 24);  
  delay(300);
  rf.send(6582737, 24);  
}

void screenDown() {
  server.send(200, "text/plain", "Screen Down");
  Serial.println("Down a doo");
  rf.send(6582740, 24); 
  delay(300);
  rf.send(6582740, 24);
}

void screenStop() {
  server.send(200, "text/plain", "Screen Stop");
  Serial.println("Stop a doo");
  rf.send(6582738, 24);  
}