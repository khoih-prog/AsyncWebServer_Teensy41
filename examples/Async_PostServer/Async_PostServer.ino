/****************************************************************************************************************************
  Async_PostServer.ino
  
  For ESP8266 using W5x00/ENC8266 Ethernet
   
  AsyncWebServer_Ethernet is a library for the Ethernet with lwIP_5100, lwIP_5500 or lwIP_enc28j60 library
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_Ethernet
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

#include "defines.h"

#include <AsyncWebServer_Ethernet.h>

AsyncWebServer    server(80);

const String postForms =
  "<html>\
<head>\
<title>AsyncWebServer_Ethernet POST handling</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h1>POST plain text to /postplain/</h1><br>\
<form method=\"post\" enctype=\"text/plain\" action=\"/postplain/\">\
<input type=\"text\" name=\'{\"hello\": \"world\", \"trash\": \"\' value=\'\"}\'><br>\
<input type=\"submit\" value=\"Submit\">\
</form>\
<h1>POST form data to /postform/</h1><br>\
<form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
<input type=\"text\" name=\"hello\" value=\"world\"><br>\
<input type=\"submit\" value=\"Submit\">\
</form>\
</body>\
</html>";

void handleRoot(AsyncWebServerRequest *request)
{
  digitalWrite(LED_BUILTIN, LED_ON);
  request->send(200, "text/html", postForms);
  digitalWrite(LED_BUILTIN, LED_OFF);
}

void handlePlain(AsyncWebServerRequest *request)
{
  if (request->method() != HTTP_POST)
  {
    digitalWrite(LED_BUILTIN, LED_ON);
    request->send(405, "text/plain", "Method Not Allowed");
    digitalWrite(LED_BUILTIN, LED_OFF);
  } 
  else
  {
    digitalWrite(LED_BUILTIN, LED_ON);
    request->send(200, "text/plain", "POST body was:\n" + request->arg("plain"));
    digitalWrite(LED_BUILTIN, LED_OFF);
  }
}

void handleForm(AsyncWebServerRequest *request)
{
  if (request->method() != HTTP_POST)
  {
    digitalWrite(LED_BUILTIN, LED_ON);
    request->send(405, "text/plain", "Method Not Allowed");
    digitalWrite(LED_BUILTIN, LED_OFF);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LED_ON);
    String message = "POST form was:\n";
    for (uint8_t i = 0; i < request->args(); i++)
    {
      message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
    }
    request->send(200, "text/plain", message);
    digitalWrite(LED_BUILTIN, LED_OFF);
  }
}

void handleNotFound(AsyncWebServerRequest *request)
{
  digitalWrite(LED_BUILTIN, LED_ON);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";
  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }
  request->send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, LED_OFF);
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
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_OFF);

  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  delay(200);

  Serial.print("\nStart Async_PostServer on "); Serial.print(BOARD_NAME);
  Serial.print(" with "); Serial.println(SHIELD_TYPE);
  Serial.println(ASYNC_WEBSERVER_ETHERNET_VERSION);

  initEthernet();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    handleRoot(request);
  });

  server.on("/postplain/", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    handlePlain(request);
  });

  server.on("/postform/", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    handleForm(request);
  });

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.print(F("HTTP EthernetWebServer started @ IP : "));
  Serial.println(eth.localIP());
}

void loop()
{
}
