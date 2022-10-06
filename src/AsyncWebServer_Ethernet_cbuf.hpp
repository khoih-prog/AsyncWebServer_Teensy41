/****************************************************************************************************************************
  AsyncWebServer_Ethernet_cbuf.hpp
  
  cbuf.hpp - Circular buffer implementation
  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
  
  For ESP8266 using W5x00/ENC8266 Ethernet
   
  AsyncWebServer_Ethernet is a library for the Ethernet with lwIP_5100, lwIP_5500 or lwIP_enc28j60 library
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_Ethernet
  
  Copyright (c) 2016 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
  as published bythe Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <https://www.gnu.org/licenses/>.  
 
  Version: 1.5.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.4.1   K Hoang      18/03/2022 Initial coding for ESP8266 using W5x00/ENC8266 Ethernet.
                                  Bump up version to v1.4.1 to sync with AsyncWebServer_STM32 v1.4.1
  1.5.0   K Hoang      05/10/2022 Option to use non-destroyed cString instead of String to save Heap
 *****************************************************************************************************************************/

#pragma once

#ifndef _ASYNC_ETHERNET_CBUF_HPP_
#define _ASYNC_ETHERNET_CBUF_HPP_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/////////////////////////////////////////////////

class cbuf 
{
  public:
    cbuf(size_t size);
    ~cbuf();

    size_t resizeAdd(size_t addSize);
    size_t resize(size_t newSize);
    size_t available() const;
    size_t size();

    size_t room() const;
    
    /////////////////////////////////////////////////

    inline bool empty() const 
    {
      return _begin == _end;
    }
    
    /////////////////////////////////////////////////

    inline bool full() const 
    {
      return wrap_if_bufend(_end + 1) == _begin;
    }
    
    /////////////////////////////////////////////////

    int peek();
    size_t peek(char *dst, size_t size);

    int read();
    size_t read(char* dst, size_t size);

    size_t write(char c);
    size_t write(const char* src, size_t size);

    void flush();
    size_t remove(size_t size);

    cbuf *next;

  private:
    
    /////////////////////////////////////////////////
  
    inline char* wrap_if_bufend(char* ptr) const 
    {
      return (ptr == _bufend) ? _buf : ptr;
    }
    
    /////////////////////////////////////////////////

    size_t _size;
    char* _buf;
    const char* _bufend;
    char* _begin;
    char* _end;
};

#endif		// _ASYNC_ETHERNET_CBUF_HPP_
