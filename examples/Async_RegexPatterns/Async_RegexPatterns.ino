/****************************************************************************************************************************
  Async_RegexPatterns.ino
  
  For ESP8266 using W5x00/ENC8266 Ethernet
   
  AsyncWebServer_Ethernet is a library for the Ethernet with lwIP_5100, lwIP_5500 or lwIP_enc28j60 library
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_Ethernet
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

//
// A simple server implementation with regex routes:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//
// Add buildflag ASYNCWEBSERVER_REGEX to enable the regex support
//
// For platformio: platformio.ini:
//  build_flags =
//      -DASYNCWEBSERVER_REGEX
//
// For arduino IDE: create/update platform.local.txt
// Windows: C:\Users\(username)\AppData\Local\Arduino15\packages\espxxxx\hardware\espxxxx\{version}\platform.local.txt
// Linux: ~/.arduino15/packages/espxxxx/hardware/espxxxx/{version}/platform.local.txt
//
// compiler.cpp.extra_flags=-DASYNCWEBSERVER_REGEX=1

#define ASYNCWEBSERVER_REGEX      true

#include "defines.h"

#include <AsyncWebServer_Ethernet.h>

AsyncWebServer    server(80);

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void initEthernet()
{
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);

#if !USING_DHCP
  eth.config(localIP, gateway, netMask, gateway);
#endif
  
  eth.setDefault();
  
  if (!eth.begin()) 
  {
    Serial.println("No Ethernet hardware ... Stop here");
    
    while (true) 
    {
      delay(1000);
    }
  } 
  else 
  {
    Serial.print("Connecting to network : ");
    
    while (!eth.connected()) 
    {
      Serial.print(".");
      delay(1000);
    }
  }
 
  Serial.println();

#if USING_DHCP  
  Serial.print("Ethernet DHCP IP address: ");
#else
  Serial.print("Ethernet Static IP address: ");
#endif
  
  Serial.println(eth.localIP());
}

void setup() 
{
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  delay(200);

  Serial.print("\nStart Async_RegexPatterns on "); Serial.print(BOARD_NAME);
  Serial.print(" with "); Serial.println(SHIELD_TYPE);
  Serial.println(ASYNC_WEBSERVER_ETHERNET_VERSION);

  initEthernet();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    request->send(200, "text/plain", "Hello, world from Async_RegexPatterns on " + String(BOARD_NAME));
  });

  // Send a GET request to <IP>/sensor/<number>
  server.on("^\\/sensor\\/([0-9]+)$", HTTP_GET, [] (AsyncWebServerRequest * request) 
  {
    String sensorNumber = request->pathArg(0);
    request->send(200, "text/plain", "Hello, sensor: " + sensorNumber);
  });

  // Send a GET request to <IP>/sensor/<number>/action/<action>
  server.on("^\\/sensor\\/([0-9]+)\\/action\\/([a-zA-Z0-9]+)$", HTTP_GET, [] (AsyncWebServerRequest * request) 
  {
    String sensorNumber = request->pathArg(0);
    String action = request->pathArg(1);
    request->send(200, "text/plain", "Hello, sensor: " + sensorNumber + ", with action: " + action);
  });

  server.onNotFound(notFound);

  server.begin();

  Serial.print("Server started @ ");
  Serial.println(eth.localIP());
}

void loop() 
{
}
