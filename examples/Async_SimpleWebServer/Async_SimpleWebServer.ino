/****************************************************************************************************************************
  Async_SimpleWebServer.ino
  
  For ESP8266 using W5x00/ENC8266 Ethernet
   
  AsyncWebServer_Ethernet is a library for the Ethernet with lwIP_5100, lwIP_5500 or lwIP_enc28j60 library
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_Ethernet
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

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

  Serial.print("\nStart Async_AdvancedWebServer on "); Serial.print(BOARD_NAME);
  Serial.print(" with "); Serial.println(SHIELD_TYPE);
  Serial.println(ASYNC_WEBSERVER_ETHERNET_VERSION);

  initEthernet();
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    request->send(200, "text/plain", "Hello, world from " SHIELD_TYPE);
  });

  // Send a GET request to <IP>/get?message=<message>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) 
  {
    String message;
    
    if (request->hasParam(PARAM_MESSAGE)) 
    {
      message = request->getParam(PARAM_MESSAGE)->value();
    } 
    else 
    {
      message = "No message sent";
    }
    
    request->send(200, "text/plain", "Hello, GET: " + message);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest * request) 
  {
    String message;
    
    if (request->hasParam(PARAM_MESSAGE, true)) 
    {
      message = request->getParam(PARAM_MESSAGE, true)->value();
    } 
    else 
    {
      message = "No message sent";
    }
    
    request->send(200, "text/plain", "Hello, POST: " + message);
  });

  server.onNotFound(notFound);

  server.begin();

  Serial.print("Server started @ ");
  Serial.println(eth.localIP());
}

void loop()
{
}
