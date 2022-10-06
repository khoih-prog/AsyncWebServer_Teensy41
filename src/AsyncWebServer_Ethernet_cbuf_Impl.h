/****************************************************************************************************************************
  AsyncWebServer_Ethernet_cbuf_Impl.h
  
  cbuf.cpp - Circular buffer implementation
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

#ifndef _ASYNC_ETHERNET_CBUF_IMPL_H_
#define _ASYNC_ETHERNET_CBUF_IMPL_H_

#include "AsyncWebServer_Ethernet_cbuf.hpp"

/////////////////////////////////////////////////

cbuf::cbuf(size_t size) : next(NULL), _size(size), _buf(new char[size]), _bufend(_buf + size), _begin(_buf), _end(_begin) 
{
}

/////////////////////////////////////////////////

cbuf::~cbuf() 
{
  if (_buf != nullptr)
    delete[] _buf;
}

/////////////////////////////////////////////////

size_t cbuf::resizeAdd(size_t addSize) 
{
  return resize(_size + addSize);
}

/////////////////////////////////////////////////

size_t cbuf::resize(size_t newSize) 
{
  size_t bytes_available = available();

  // not lose any data
  // if data can be lost use remove or flush before resize
  if ((newSize <= bytes_available) || (newSize == _size)) 
  {
    return _size;
  }

  char *newbuf = new char[newSize];
  char *oldbuf = _buf;

  if (!newbuf) 
  {
    return _size;
  }

  if (_buf) 
  {
    read(newbuf, bytes_available);
    memset((newbuf + bytes_available), 0x00, (newSize - bytes_available));
  }

  _begin = newbuf;
  _end = newbuf + bytes_available;
  _bufend = newbuf + newSize;
  _size = newSize;

  _buf = newbuf;
  
  delete[] oldbuf;

  return _size;
}

/////////////////////////////////////////////////

size_t cbuf::available() const 
{
  if (_end >= _begin) 
  {
    return _end - _begin;
  }
  
  return _size - (_begin - _end);
}

/////////////////////////////////////////////////

size_t cbuf::size() 
{
  return _size;
}

/////////////////////////////////////////////////

size_t cbuf::room() const 
{
  if (_end >= _begin) 
  {
    return _size - (_end - _begin) - 1;
  }
  
  return _begin - _end - 1;
}

/////////////////////////////////////////////////

int cbuf::peek() 
{
  if (empty())
    return -1;

  return static_cast<int>(*_begin);
}

/////////////////////////////////////////////////

size_t cbuf::peek(char *dst, size_t size) 
{
  size_t bytes_available = available();
  size_t size_to_read = (size < bytes_available) ? size : bytes_available;
  size_t size_read = size_to_read;
  char * begin = _begin;
  
  if (_end < _begin && size_to_read > (size_t) (_bufend - _begin)) 
  {
    size_t top_size = _bufend - _begin;
    memcpy(dst, _begin, top_size);
    begin = _buf;
    size_to_read -= top_size;
    dst += top_size;
  }
  
  memcpy(dst, begin, size_to_read);
  
  return size_read;
}

/////////////////////////////////////////////////

int cbuf::read() 
{
  if (empty())
    return -1;

  char result = *_begin;
  _begin = wrap_if_bufend(_begin + 1);
  
  return static_cast<int>(result);
}

/////////////////////////////////////////////////

size_t cbuf::read(char* dst, size_t size) 
{
  size_t bytes_available = available();
  size_t size_to_read = (size < bytes_available) ? size : bytes_available;
  size_t size_read = size_to_read;
  
  if (_end < _begin && size_to_read > (size_t) (_bufend - _begin)) 
  {
    size_t top_size = _bufend - _begin;
    
    memcpy(dst, _begin, top_size);
    _begin = _buf;
    size_to_read -= top_size;
    dst += top_size;
  }
  
  memcpy(dst, _begin, size_to_read);
  _begin = wrap_if_bufend(_begin + size_to_read);
  
  return size_read;
}

/////////////////////////////////////////////////

size_t cbuf::write(char c) 
{
  if (full())
    return 0;

  *_end = c;
  _end = wrap_if_bufend(_end + 1);
  
  return 1;
}

/////////////////////////////////////////////////

size_t cbuf::write(const char* src, size_t size) 
{
  size_t bytes_available = room();
  size_t size_to_write = (size < bytes_available) ? size : bytes_available;
  size_t size_written = size_to_write;
  
  if (_end >= _begin && size_to_write > (size_t) (_bufend - _end)) 
  {
    size_t top_size = _bufend - _end;
    
    memcpy(_end, src, top_size);
    _end = _buf;
    size_to_write -= top_size;
    src += top_size;
  }
  
  memcpy(_end, src, size_to_write);
  _end = wrap_if_bufend(_end + size_to_write);
  
  return size_written;
}

/////////////////////////////////////////////////

void cbuf::flush() 
{
  _begin = _buf;
  _end = _buf;
}

/////////////////////////////////////////////////

size_t cbuf::remove(size_t size) 
{
  size_t bytes_available = available();
  
  if (size >= bytes_available) 
  {
    flush();
    
    return 0;
  }
  
  size_t size_to_remove = (size < bytes_available) ? size : bytes_available;
  
  if (_end < _begin && size_to_remove > (size_t) (_bufend - _begin)) 
  {
    size_t top_size = _bufend - _begin;
    _begin = _buf;
    size_to_remove -= top_size;
  }
  
  _begin = wrap_if_bufend(_begin + size_to_remove);
  
  return available();
}

/////////////////////////////////////////////////

#endif		// _ASYNC_ETHERNET_CBUF_IMPL_H_
