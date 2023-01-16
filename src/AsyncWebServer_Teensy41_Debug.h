/****************************************************************************************************************************
  AsyncWebServer_Teensy41_Debug.h - Dead simple AsyncWebServer for Teensy41 QNEthernet

  For Teensy41 with QNEthernet

  AsyncWebServer_Teensy41 is a library for the Teensy41 with QNEthernet

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_Teensy41

  Copyright (c) 2016 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
  as published bythe Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <https://www.gnu.org/licenses/>.

  Version: 1.6.2

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.4.1   K Hoang      18/03/2022 Initial coding for Teensy 4.1 using built-in QNEthernet.
                                  Bump up version to v1.4.1 to sync with AsyncWebServer_STM32 v1.4.1
  1.5.0   K Hoang      01/10/2022 Fix issue with slow browsers or network. Add function and example to support favicon.ico
  1.6.0   K Hoang      06/10/2022 Option to use non-destroyed cString instead of String to save Heap
  1.6.1   K Hoang      10/11/2022 Add examples to demo how to use beginChunkedResponse() to send in chunks
  1.6.2   K Hoang      16/01/2023 Add examples Async_WebSocketsServer
 *****************************************************************************************************************************/

#pragma once

#ifndef _ASYNCWEBSERVER_TEENSY41_DEBUG_H_
#define _ASYNCWEBSERVER_TEENSY41_DEBUG_H_

#ifdef ASYNCWEBSERVER_TEENSY41_DEBUG_PORT
  #define DBG_PORT_AWS      ASYNCWEBSERVER_TEENSY41_DEBUG_PORT
#else
  #define DBG_PORT_AWS      Serial
#endif

// Change _AWS_TEENSY41_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _AWS_TEENSY41_LOGLEVEL_
  #define _AWS_TEENSY41_LOGLEVEL_       1
#endif

/////////////////////////////////////////////////////////

#define AWS_PRINT_MARK      AWS_PRINT("[AWS] ")
#define AWS_PRINT_SP        DBG_PORT_AWS.print(" ")

#define AWS_PRINT           DBG_PORT_AWS.print
#define AWS_PRINTLN         DBG_PORT_AWS.println

/////////////////////////////////////////////////////////

#define LOGERROR(x)         if(_AWS_TEENSY41_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define LOGERROR0(x)        if(_AWS_TEENSY41_LOGLEVEL_>0) { AWS_PRINT(x); }
#define LOGERROR1(x,y)      if(_AWS_TEENSY41_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define LOGERROR2(x,y,z)    if(_AWS_TEENSY41_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define LOGERROR3(x,y,z,w)  if(_AWS_TEENSY41_LOGLEVEL_>0) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define LOGWARN(x)          if(_AWS_TEENSY41_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define LOGWARN0(x)         if(_AWS_TEENSY41_LOGLEVEL_>1) { AWS_PRINT(x); }
#define LOGWARN1(x,y)       if(_AWS_TEENSY41_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define LOGWARN2(x,y,z)     if(_AWS_TEENSY41_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define LOGWARN3(x,y,z,w)   if(_AWS_TEENSY41_LOGLEVEL_>1) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define LOGINFO(x)          if(_AWS_TEENSY41_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define LOGINFO0(x)         if(_AWS_TEENSY41_LOGLEVEL_>2) { AWS_PRINT(x); }
#define LOGINFO1(x,y)       if(_AWS_TEENSY41_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define LOGINFO2(x,y,z)     if(_AWS_TEENSY41_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define LOGINFO3(x,y,z,w)   if(_AWS_TEENSY41_LOGLEVEL_>2) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#define LOGDEBUG(x)         if(_AWS_TEENSY41_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINTLN(x); }
#define LOGDEBUG0(x)        if(_AWS_TEENSY41_LOGLEVEL_>3) { AWS_PRINT(x); }
#define LOGDEBUG1(x,y)      if(_AWS_TEENSY41_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINTLN(y); }
#define LOGDEBUG2(x,y,z)    if(_AWS_TEENSY41_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINTLN(z); }
#define LOGDEBUG3(x,y,z,w)  if(_AWS_TEENSY41_LOGLEVEL_>3) { AWS_PRINT_MARK; AWS_PRINT(x); AWS_PRINT_SP; AWS_PRINT(y); AWS_PRINT_SP; AWS_PRINT(z); AWS_PRINT_SP; AWS_PRINTLN(w); }

/////////////////////////////////////////////////////////

#endif    //_ASYNCWEBSERVER_TEENSY41_DEBUG_H_
