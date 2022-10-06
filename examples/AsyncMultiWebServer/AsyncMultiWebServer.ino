/****************************************************************************************************************************
  AsyncMultiWebServer.h
  
  For ESP8266 using W5x00/ENC8266 Ethernet
   
  AsyncWebServer_Ethernet is a library for the Ethernet with lwIP_5100, lwIP_5500 or lwIP_enc28j60 library
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_Ethernet
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

#include "defines.h"

#include <AsyncWebServer_Ethernet.h>

unsigned int    analogReadPin []  = { 12, 13, 14 };

#define BUFFER_SIZE       512

#define HTTP_PORT1        8080
#define HTTP_PORT2        8081
#define HTTP_PORT3        8082

AsyncWebServer* server1;
AsyncWebServer* server2;
AsyncWebServer* server3;

AsyncWebServer*  multiServer  []  = { server1, server2, server3 };
uint16_t        http_port     []  = { HTTP_PORT1, HTTP_PORT2, HTTP_PORT3 };

#define NUM_SERVERS     ( sizeof(multiServer) / sizeof(AsyncWebServer*) )

unsigned int serverIndex;

String createBuffer()
{
  char temp[BUFFER_SIZE];

  memset(temp, 0, sizeof(temp));

  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int day = hr / 24;

  snprintf(temp, BUFFER_SIZE - 1,
           "<html>\
<head>\
<meta http-equiv='refresh' content='5'/>\
<title>%s</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h1>Hello from %s</h1>\
<h2>running AsyncWebServer_Ethernet</h2>\
<h3>on %s</h3>\
<h3>Uptime: %d d %02d:%02d:%02d</h3>\
</body>\
</html>", BOARD_NAME, BOARD_NAME, SHIELD_TYPE, day, hr, min % 60, sec % 60);

  return temp;
}


void handleRoot(AsyncWebServerRequest * request)
{
  String message = createBuffer();
  request->send(200, F("text/html"), message);
}

String createNotFoundBuffer(AsyncWebServerRequest * request)
{
  String message;

  message.reserve(500);

  message = F("File Not Found\n\n");

  message += F("URI: ");
  message += request->url();
  message += F("\nMethod: ");
  message += (request->method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += request->args();
  message += F("\n");

  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }

  return message;
}

void handleNotFound(AsyncWebServerRequest * request)
{
  String message = createNotFoundBuffer(request);
  request->send(404, F("text/plain"), message);
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

  Serial.print("\nStart AsyncMultiWebServer on "); Serial.print(BOARD_NAME);
  Serial.print(" with "); Serial.println(SHIELD_TYPE);
  Serial.println(ASYNC_WEBSERVER_ETHERNET_VERSION);

  initEthernet();
  
  for (serverIndex = 0; serverIndex < NUM_SERVERS; serverIndex++)
  {
    multiServer[serverIndex] = new AsyncWebServer(http_port[serverIndex]);  

    if (multiServer[serverIndex])
    {
      Serial.printf("Initialize multiServer OK, serverIndex = %d, port = %d\n", serverIndex, http_port[serverIndex]);
    }
    else
    {
      Serial.printf("Error initialize multiServer, serverIndex = %d\n", serverIndex);

      while(1);
    }

    multiServer[serverIndex]->on("/", HTTP_GET, [](AsyncWebServerRequest * request)
    {
      handleRoot(request);
    });

    multiServer[serverIndex]->on("/hello", HTTP_GET, [](AsyncWebServerRequest * request) 
    {     
      String message = F("Hello from AsyncWebServer using built-in LAN8742A Ethernet, running on ");
      message       += BOARD_NAME;
      
      request->send(200, "text/plain", message);
    });
      
    multiServer[serverIndex]->onNotFound([](AsyncWebServerRequest * request) 
    {
      handleNotFound(request);
    });

    multiServer[serverIndex]->begin();

    Serial.printf("HTTP server started at ports %d\n", http_port[serverIndex]);
  }
}

void loop()
{
}
