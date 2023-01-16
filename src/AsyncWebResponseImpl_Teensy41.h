/****************************************************************************************************************************
  AsyncWebResponseImpl_Teensy41.h - Dead simple AsyncWebServer for Teensy41 QNEthernet

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

#ifndef ASYNCWEBSERVERRESPONSEIMPL_TEENSY41_H_
#define ASYNCWEBSERVERRESPONSEIMPL_TEENSY41_H_

#ifdef Arduino_h
  // arduino is not compatible with std::vector
  #undef min
  #undef max
#endif

#include <vector>

// It is possible to restore these defines, but one can use _min and _max instead. Or std::min, std::max.

/////////////////////////////////////////////////

class AsyncBasicResponse: public AsyncWebServerResponse
{
  private:
    String _content;

    char *_contentCstr;      // RSMOD
    String _partialHeader;

  public:
    AsyncBasicResponse(int code, const String& contentType = String(), const String& content = String());

    AsyncBasicResponse(int code, const String& contentType, const char *content = nullptr);     // RSMOD

    void _respond(AsyncWebServerRequest *request);

    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return true;
    }

    /////////////////////////////////////////////////
};

/////////////////////////////////////////////////

class AsyncAbstractResponse: public AsyncWebServerResponse
{
  private:
    String _head;
    // Data is inserted into cache at begin().
    // This is inefficient with vector, but if we use some other container,
    // we won't be able to access it as contiguous array of bytes when reading from it,
    // so by gaining performance in one place, we'll lose it in another.
    std::vector<uint8_t> _cache;
    size_t _readDataFromCacheOrContent(uint8_t* data, const size_t len);
    size_t _fillBufferAndProcessTemplates(uint8_t* buf, size_t maxLen);

  protected:
    AwsTemplateProcessor _callback;

  public:
    AsyncAbstractResponse(AwsTemplateProcessor callback = nullptr);
    void _respond(AsyncWebServerRequest *request);
    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return false;
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf __attribute__((unused)), size_t maxLen __attribute__((unused)))
    {
      return 0;
    }

    /////////////////////////////////////////////////
};

/////////////////////////////////////////////////

#ifndef TEMPLATE_PLACEHOLDER
  #define TEMPLATE_PLACEHOLDER '%'
#endif

/////////////////////////////////////////////////

#define TEMPLATE_PARAM_NAME_LENGTH 32

/////////////////////////////////////////////////

class AsyncStreamResponse: public AsyncAbstractResponse
{
  private:
    Stream *_content;

  public:
    AsyncStreamResponse(Stream &stream, const String& contentType, size_t len, AwsTemplateProcessor callback = nullptr);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return !!(_content);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class AsyncCallbackResponse: public AsyncAbstractResponse
{
  private:
    AwsResponseFiller _content;
    size_t _filledLength;

  public:
    AsyncCallbackResponse(const String& contentType, size_t len, AwsResponseFiller callback,
                          AwsTemplateProcessor templateCallback = nullptr);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return !!(_content);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class AsyncChunkedResponse: public AsyncAbstractResponse
{
  private:
    AwsResponseFiller _content;
    size_t _filledLength;

  public:
    AsyncChunkedResponse(const String& contentType, AwsResponseFiller callback,
                         AwsTemplateProcessor templateCallback = nullptr);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return !!(_content);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class cbuf;

/////////////////////////////////////////////////

class AsyncProgmemResponse: public AsyncAbstractResponse
{
  private:
    const uint8_t * _content;
    size_t _readLength;

  public:
    AsyncProgmemResponse(int code, const String& contentType, const uint8_t * content, size_t len,
                         AwsTemplateProcessor callback = nullptr);

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return (_state < RESPONSE_END);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

/////////////////////////////////////////////////

class AsyncResponseStream: public AsyncAbstractResponse, public Print
{
  private:
    cbuf *_content;

  public:
    AsyncResponseStream(const String& contentType, size_t bufferSize);
    ~AsyncResponseStream();

    /////////////////////////////////////////////////

    inline bool _sourceValid() const
    {
      return (_state < RESPONSE_END);
    }

    /////////////////////////////////////////////////

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
    size_t write(const uint8_t *data, size_t len);
    size_t write(uint8_t data);
    using Print::write;
};

/////////////////////////////////////////////////

#endif /* ASYNCWEBSERVERRESPONSEIMPL_TEENSY41_H_ */
