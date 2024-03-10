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
     fauxmo.addDevice("Projector");
     fauxmo.addDevice("Onkyo");
     

    // Set a callback function for when a device is found
     fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);       
        if (strcmp(device_name, "Projector") == 0) {
          if (state) {
            Serial.println("Turning on Projector");
            projectorOn();
          }
          else {
            Serial.println("Turning off Projector");
            projectorOff();
          }
       }
       else if (strcmp(device_name, "Onkyo") == 0) {
          if (state) {
            Serial.println("Turning on Onkyo");
            onkyoOnOff();
          }
          else {
            Serial.println("Turning off Onkyo");
            onkyoOnOff();
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
            F("ESP8266 Controller Basement"));
    });
    
  server.on("/projectorOn", HTTP_GET, projectorOn);
  server.on("/projectorOff", HTTP_GET, projectorOff);
  
  server.on("/onkyoOn", HTTP_GET, onkyoOnOff);
  server.on("/onkyoOff", HTTP_GET, onkyoOnOff);
  server.on("/onkyoUp", HTTP_GET, onkyoUP);
  server.on("/onkyoDown", HTTP_GET, onkyoDOWN);
  server.on("/onkyoBlue", HTTP_GET, onkyoBlue);
  server.on("/onkyoPC", HTTP_GET, onkyoPC);
  server.on("/onkyoSTRMBOX", HTTP_GET, onkyoSTRMBOX);
  server.on("/onkyoCD", HTTP_GET, onkyoCD);
  server.on("/onkyoGame", HTTP_GET, onkyoGame);
  server.on("/onkyoAux", HTTP_GET, onkyoAux);
  server.on("/onkyoDVD", HTTP_GET, onkyoDVD);
  server.on("/onkyoCBL", HTTP_GET, onkyoCBL);
  server.on("/onkyoNet", HTTP_GET, onkyoNet);
  server.on("/onkyoQuick", HTTP_GET, onkyoQuick);
  server.on("/onkyoUpArrow", HTTP_GET, onkyoUpArrow);
  server.on("/onkyoInfo", HTTP_GET, onkyoInfo);
  server.on("/onkyoLeft", HTTP_GET, onkyoLeft);
  server.on("/onkyoEnter", HTTP_GET, onkyoEnter);
  server.on("/onkyoRight", HTTP_GET, onkyoRight);
  server.on("/onkyoSettings", HTTP_GET, onkyoSettings);
  server.on("/onkyoDownArrow", HTTP_GET, onkyoDownArrow);
  server.on("/onkyoBack", HTTP_GET, onkyoBack);
  server.on("/onkyoMute", HTTP_GET, onkyoMute);

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

//Functions for each Projector command (on/off)
void projectorOn() {
  Serial.println("Turning on Projector");
  server.send(200, "text/plain", "On");
  irsend.sendEpson(0xC1AA09F6, 32, 2);
  delay(500);
}

void projectorOff() {
  Serial.println("Turning off Projector");
  server.send(200, "text/plain", "Off");
  irsend.sendEpson(0xC1AA8976, 32, 2);
  delay(500);
}

//Functions for each Onkyo command
void onkyoOnOff() {
  Serial.println("Turning on Onkyo");
  server.send(200, "text/plain", "On/Off");
  irsend.sendNEC(0x4B36D32C, 32, 2);
  delay(500);
}

void onkyoSTRMBOX() {
  Serial.println("STRMBOX");
  server.send(200, "text/plain", "STRMBOX");
  irsend.sendNEC(0x4BB630CF, 32, 3);
  delay(500);
}

void onkyoUP() {
  Serial.println("Volume UP");
  server.send(200, "text/plain", "Volume UP");
  irsend.sendNEC(0x4BB640BF, 32, 3);
}

void onkyoDOWN() {
  server.send(200, "text/plain", "Volume Down");
  irsend.sendNEC(0x4BB6C03F, 32, 3);
}

void onkyoBlue() {
  server.send(200, "text/plain", "Bluetooth");
  irsend.sendNEC(0x4B780A70, 32, 3);
  delay(500);
}

void onkyoDVD() {
  server.send(200, "text/plain", "DVD");
  irsend.sendNEC(0x4B3631CE, 32, 3);
  delay(500);
}

void onkyoPC() {
  Serial.println("PC");
  server.send(200, "text/plain", "PC");
  irsend.sendNEC(0x4BB639C6, 32, 3);
  delay(500);
}

void onkyoCD() {
  Serial.println("PC");
  server.send(200, "text/plain", "CD");
  irsend.sendNEC(0x4BB6906F, 32, 3);
  delay(500);
}

void onkyoGame() {
  Serial.println("PC");
  server.send(200, "text/plain", "Game");
  irsend.sendNEC(0x4BB6B04F, 32, 3);
  delay(500);
}

void onkyoAux() {
  Serial.println("PC");
  server.send(200, "text/plain", "Aux");
  irsend.sendNEC(0x4BB6F906, 32, 3);
  delay(500);
}

void onkyoCBL() {
  Serial.println("PC");
  server.send(200, "text/plain", "CBL/SAT");
  irsend.sendNEC(0x4BB6708F, 32, 3);
  delay(500);
}

void onkyoNet() {
  Serial.println("PC");
  server.send(200, "text/plain", "Net");
  irsend.sendNEC(0x4B40E11E, 32, 3);
  delay(500);
}

void onkyoQuick() {
  Serial.println("PC");
  server.send(200, "text/plain", "Quick Menu");
  irsend.sendNEC(0x4B36CA35, 32, 3);
  delay(500);
}

void onkyoUpArrow() {
  Serial.println("PC");
  server.send(200, "text/plain", "Up Arr");
  irsend.sendEpson(0x4BB641BE, 32, 2);
  delay(500);
}

void onkyoInfo() {
  Serial.println("PC");
  server.send(200, "text/plain", "Info");
  irsend.sendNEC(0x4B36AA55, 32, 3);
  delay(500);
}

void onkyoLeft() {
  Serial.println("PC");
  server.send(200, "text/plain", "Left");
  irsend.sendEpson(0x4BB621DE, 32, 2);
  delay(500);
}

void onkyoEnter() {
  Serial.println("PC");
  server.send(200, "text/plain", "Enter");
  irsend.sendNEC(0x4BB6E916, 32, 3);
  delay(500);
}

void onkyoRight() {
  Serial.println("PC");
  server.send(200, "text/plain", "Right");
  irsend.sendEpson(0x4BB6A15E, 32, 2);
  delay(500);
}

void onkyoSettings() {
  Serial.println("PC");
  server.send(200, "text/plain", "Settings");
  irsend.sendNEC(0x4B36DB24, 32, 3);
  delay(500);
}

void onkyoDownArrow() {
  Serial.println("PC");
  server.send(200, "text/plain", "Down");
  irsend.sendEpson(0x4BB6C13E, 32, 2);
  delay(500);
}

void onkyoBack() {
  Serial.println("PC");
  server.send(200, "text/plain", "Back");
  irsend.sendNEC(0x4B362AD5, 32, 3);
  delay(500);
}

void onkyoMute() {
  Serial.println("PC");
  server.send(200, "text/plain", "Mute");
  irsend.sendNEC(0x4BB6A05F, 32, 3);
  delay(500);
}


//Fireplace
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