/****************************************************************************************************************************
  Async_HttpBasicAuth.ino
  
  For ESP8266 using W5x00/ENC8266 Ethernet
   
  AsyncWebServer_Ethernet is a library for the Ethernet with lwIP_5100, lwIP_5500 or lwIP_enc28j60 library
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_Ethernet
  Licensed under GPLv3 license
 *****************************************************************************************************************************/
 
#include "defines.h"

#include <AsyncWebServer_Ethernet.h>

AsyncWebServer    server(80);

const char* www_username = "admin";
const char* www_password = "ethernet";

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

  Serial.print("\nStart Async_HttpBasicAuth on "); Serial.print(BOARD_NAME);
  Serial.print(" with "); Serial.println(SHIELD_TYPE);
  Serial.println(ASYNC_WEBSERVER_ETHERNET_VERSION);

  initEthernet();
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    if (!request->authenticate(www_username, www_password))
    {
      return request->requestAuthentication();
    }
    
    request->send(200, "text/plain", "Login OK");
  });
  
  server.begin();

  Serial.print(F("Async_HttpBasicAuth started @ IP : "));
  Serial.println(eth.localIP());

  Serial.print(F("Open http://"));
  Serial.print(eth.localIP());
  Serial.println(F("/ in your browser to see it working"));

  Serial.print(F("Login using username = "));
  Serial.print(www_username);
  Serial.print(F(" and password = "));
  Serial.println(www_password);
}

void loop()
{
}
