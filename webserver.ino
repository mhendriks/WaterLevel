#include <WiFi.h>
#include <ESPmDNS.h>
#include "ESPAsyncWebServer.h"
#include <LittleFS.h>

const char* ssid     = "MobileHome";
const char* password = "martijn1971";

String htmlpage = "<!DOCTYPE html><html><body><h1>My First Heading</h1><p>My first paragraph.</p></body></html>";

AsyncWebServer httpServer(80);

void SetupWebServer() {
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);
  delay(500);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  if (MDNS.begin("mobile-home")) {
    Serial.println("MDNS responder started");
  }
    delay(100);
    httpServer.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    httpServer.on("/levels", [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "{\"min\":"+String(Vmin)+",\"max\":"+String(Vmax)+",\"actual\":"+String(Vgem)+",\"perc\":"+String(perc)+",\"temp\":" + String(temp.temperature) +"}");
  });
    httpServer.on("/h2olevel", [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(perc) );
    });
    
    httpServer.on("/templevel", [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(temp.temperature) );
    });


    httpServer.onNotFound([](AsyncWebServerRequest *request){
//      String filename = request->url();
//      Serial.print("onNotFound handle filename: ");Serial.println(filename);
      request->send(404, "text/plain", F("FileNotFound\r\n"));
    });
  httpServer.begin();
  Serial.println("HTTP server started");
}
