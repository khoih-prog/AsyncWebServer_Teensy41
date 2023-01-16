/****************************************************************************************************************************
  AsyncWebSynchronization_Teensy41.h - Dead simple AsyncWebServer for Teensy41 QNEthernet

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

#ifndef ASYNCWEBSYNCHRONIZATION_TEENSY41_H_
#define ASYNCWEBSYNCHRONIZATION_TEENSY41_H_

// Synchronisation is only available on ESP32, as the ESP8266 isn't using FreeRTOS by default

#include <AsyncWebServer_Teensy41.hpp>

/////////////////////////////////////////////////

// This is the Teensy41 version of the Sync Lock which is currently unimplemented
class AsyncWebLock
{

  public:
    AsyncWebLock()  {}

    ~AsyncWebLock() {}

    /////////////////////////////////////////////////

    inline bool lock() const
    {
      return false;
    }

    /////////////////////////////////////////////////

    inline void unlock() const {}
};

class AsyncWebLockGuard
{
  private:
    const AsyncWebLock *_lock;

  public:

    /////////////////////////////////////////////////

    AsyncWebLockGuard(const AsyncWebLock &l)
    {
      if (l.lock())
      {
        _lock = &l;
      }
      else
      {
        _lock = NULL;
      }
    }

    /////////////////////////////////////////////////

    ~AsyncWebLockGuard()
    {
      if (_lock)
      {
        _lock->unlock();
      }
    }
};

#endif // ASYNCWEBSYNCHRONIZATION_TEENSY41_H_
