/****************************************************************************************************************************
  AsyncWebHandlerImpl_Teensy41.h - Dead simple AsyncWebServer for Teensy41 QNEthernet

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

#ifndef ASYNCWEBSERVERHANDLERIMPL_TEENSY41_H_
#define ASYNCWEBSERVERHANDLERIMPL_TEENSY41_H_

#include <string>

#ifdef ASYNCWEBSERVER_REGEX
  #include <regex>
#endif

#include "stddef.h"
#include <time.h>

/////////////////////////////////////////////////

class AsyncStaticWebHandler: public AsyncWebHandler
{
  private:
    uint8_t _countBits(const uint8_t value) const;

  protected:
    String _uri;
    String _path;
    String _cache_control;
    String _last_modified;
    AwsTemplateProcessor _callback;
    bool _isDir;
    bool _gzipFirst;
    uint8_t _gzipStats;

  public:
    AsyncStaticWebHandler(const char* uri, const char* path, const char* cache_control);
    virtual bool canHandle(AsyncWebServerRequest *request) override final;
    virtual void handleRequest(AsyncWebServerRequest *request) override final;
    AsyncStaticWebHandler& setIsDir(bool isDir);
    AsyncStaticWebHandler& setCacheControl(const char* cache_control);
    AsyncStaticWebHandler& setLastModified(const char* last_modified);
    AsyncStaticWebHandler& setLastModified(struct tm* last_modified);

    AsyncStaticWebHandler& setLastModified(time_t last_modified);
    AsyncStaticWebHandler& setLastModified(); //sets to current time. Make sure sntp is runing and time is updated

    /////////////////////////////////////////////////

    AsyncStaticWebHandler& setTemplateProcessor(AwsTemplateProcessor newCallback)
    {
      _callback = newCallback;
      return *this;
    }
};

class AsyncCallbackWebHandler: public AsyncWebHandler
{
  private:
  protected:
    String _uri;
    WebRequestMethodComposite _method;
    ArRequestHandlerFunction _onRequest;
    ArUploadHandlerFunction _onUpload;
    ArBodyHandlerFunction _onBody;
    bool _isRegex;

  public:
    AsyncCallbackWebHandler() : _uri(), _method(HTTP_ANY), _onRequest(NULL), _onUpload(NULL), _onBody(NULL),
      _isRegex(false) {}

    /////////////////////////////////////////////////

    inline void setUri(const String& uri)
    {
      _uri = uri;
      _isRegex = uri.startsWith("^") && uri.endsWith("$");
    }

    /////////////////////////////////////////////////

    inline void setMethod(WebRequestMethodComposite method)
    {
      _method = method;
    }

    /////////////////////////////////////////////////

    inline void onRequest(ArRequestHandlerFunction fn)
    {
      _onRequest = fn;
    }

    /////////////////////////////////////////////////

    inline void onUpload(ArUploadHandlerFunction fn)
    {
      _onUpload = fn;
    }

    /////////////////////////////////////////////////

    inline void onBody(ArBodyHandlerFunction fn)
    {
      _onBody = fn;
    }

    /////////////////////////////////////////////////

    virtual bool canHandle(AsyncWebServerRequest *request) override final
    {
      if (!_onRequest)
        return false;

      if (!(_method & request->method()))
        return false;

#ifdef ASYNCWEBSERVER_REGEX

      if (_isRegex)
      {
        std::regex pattern(_uri.c_str());
        std::smatch matches;
        std::string s(request->url().c_str());

        if (std::regex_search(s, matches, pattern))
        {
          for (size_t i = 1; i < matches.size(); ++i)
          {
            // start from 1
            request->_addPathParam(matches[i].str().c_str());
          }
        }
        else
        {
          return false;
        }
      }
      else
#endif
        if (_uri.length() && _uri.endsWith("*"))
        {
          String uriTemplate = String(_uri);
          uriTemplate = uriTemplate.substring(0, uriTemplate.length() - 1);

          if (!request->url().startsWith(uriTemplate))
            return false;
        }
        else if (_uri.length() && (_uri != request->url() && !request->url().startsWith(_uri + "/")))
          return false;

      request->addInterestingHeader("ANY");

      return true;
    }

    /////////////////////////////////////////////////

    virtual void handleRequest(AsyncWebServerRequest *request) override final
    {
      if (_onRequest)
        _onRequest(request);
      else
        request->send(500);
    }

    /////////////////////////////////////////////////

    virtual void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index,
                            size_t total) override final
    {
      if (_onBody)
        _onBody(request, data, len, index, total);
    }

    /////////////////////////////////////////////////

    virtual bool isRequestHandlerTrivial() override final
    {
      return _onRequest ? false : true;
    }
};

#endif /* ASYNCWEBSERVERHANDLERIMPL_TEENSY41_H_ */
