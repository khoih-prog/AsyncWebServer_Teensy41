/****************************************************************************************************************************
  AsyncMultiWebServer.h - Dead simple AsyncWebServer for Teensy41 QNEthernet

  For Teensy41 with QNEthernet

  AsyncWebServer_Teensy41 is a library for the Teensy41 with QNEthernet

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_Teensy41
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

#if !( defined(CORE_TEENSY) && defined(__IMXRT1062__) && defined(ARDUINO_TEENSY41) )
  #error Only Teensy 4.1 supported
#endif

// Debug Level from 0 to 4
#define _TEENSY41_ASYNC_TCP_LOGLEVEL_       1
#define _AWS_TEENSY41_LOGLEVEL_             1

#define SHIELD_TYPE     "Teensy4.1 QNEthernet"

#if (_AWS_TEENSY41_LOGLEVEL_ > 3)
  #warning Using QNEthernet lib for Teensy 4.1. Must also use Teensy Packages Patch or error
#endif

#define USING_DHCP            true
//#define USING_DHCP            false

#if !USING_DHCP
  // Set the static IP address to use if the DHCP fails to assign
  IPAddress myIP(192, 168, 2, 222);
  IPAddress myNetmask(255, 255, 255, 0);
  IPAddress myGW(192, 168, 2, 1);
  //IPAddress mydnsServer(192, 168, 2, 1);
  IPAddress mydnsServer(8, 8, 8, 8);
#endif

#include "QNEthernet.h"       // https://github.com/ssilverman/QNEthernet
using namespace qindesign::network;

#include <AsyncWebServer_Teensy41.h>

unsigned int    analogReadPin []  = { 12, 13, 14 };

#define BUFFER_SIZE       500

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
<h2>running AsyncWebServer_Teensy41</h2>\
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

void setup()
{
  Serial.begin(115200);

  while (!Serial && millis() < 5000);

  delay(200);

  Serial.print("\nStart AsyncMultiWebServer on ");
  Serial.print(BOARD_NAME);
  Serial.print(" with ");
  Serial.println(SHIELD_TYPE);
  Serial.println(ASYNC_WEBSERVER_TEENSY41_VERSION);

  delay(500);

#if USING_DHCP
  // Start the Ethernet connection, using DHCP
  Serial.print("Initialize Ethernet using DHCP => ");
  Ethernet.begin();
#else
  // Start the Ethernet connection, using static IP
  Serial.print("Initialize Ethernet using static IP => ");
  Ethernet.begin(myIP, myNetmask, myGW);
  Ethernet.setDNSServerIP(mydnsServer);
#endif

  if (!Ethernet.waitForLocalIP(5000))
  {
    Serial.println(F("Failed to configure Ethernet"));

    if (!Ethernet.linkStatus())
    {
      Serial.println(F("Ethernet cable is not connected."));
    }

    // Stay here forever
    while (true)
    {
      delay(1);
    }
  }
  else
  {
    Serial.print(F("Connected! IP address:"));
    Serial.println(Ethernet.localIP());
  }

#if USING_DHCP
  delay(1000);
#else
  delay(2000);
#endif

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

      while (1);
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
