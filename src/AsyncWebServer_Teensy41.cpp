/****************************************************************************************************************************
  AsyncWebServer_Teensy41.cpp - Dead simple AsyncWebServer for Teensy41 QNEthernet
  
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
  You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.  
 
  Version: 1.4.1

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.4.1   K Hoang      18/03/2022 Initial coding for Teensy 4.1 using built-in QNEthernet.
                                  Bump up version to v1.4.1 to sync with AsyncWebServer_STM32 v1.4.1
 *****************************************************************************************************************************/

#define _AWS_TEENSY41_LOGLEVEL_     1

#include "AsyncWebServer_Teensy41_Debug.h"

#include "AsyncWebServer_Teensy41.hpp"
#include "AsyncWebHandlerImpl_Teensy41.h"

AsyncWebServer::AsyncWebServer(uint16_t port)
  : _server(port), _rewrites(LinkedList<AsyncWebRewrite * >([](AsyncWebRewrite * r)
{
  delete r;
}))
, _handlers(LinkedList<AsyncWebHandler*>([](AsyncWebHandler* h)
{
  delete h;
}))
{
  _catchAllHandler = new AsyncCallbackWebHandler();

  if (_catchAllHandler == NULL)
    return;

  _server.onClient([](void *s, AsyncClient * c)
  {
    if (c == NULL)
      return;

    // KH set no RxTimeout for slower Firefox / network
    //c->setRxTimeout(3);
    c->setRxTimeout(0);
    //////
    
    AsyncWebServerRequest *r = new AsyncWebServerRequest((AsyncWebServer*)s, c);

    if (r == NULL)
    {
      c->close(true);
      c->free();
      delete c;
    }
  }, this);
}

AsyncWebServer::~AsyncWebServer()
{
  reset();
  end();

  if (_catchAllHandler)
    delete _catchAllHandler;
}

AsyncWebRewrite& AsyncWebServer::addRewrite(AsyncWebRewrite* rewrite)
{
  _rewrites.add(rewrite);

  return *rewrite;
}

bool AsyncWebServer::removeRewrite(AsyncWebRewrite *rewrite)
{
  return _rewrites.remove(rewrite);
}

AsyncWebRewrite& AsyncWebServer::rewrite(const char* from, const char* to)
{
  return addRewrite(new AsyncWebRewrite(from, to));
}

AsyncWebHandler& AsyncWebServer::addHandler(AsyncWebHandler* handler)
{
  _handlers.add(handler);
  return *handler;
}

bool AsyncWebServer::removeHandler(AsyncWebHandler *handler)
{
  return _handlers.remove(handler);
}

void AsyncWebServer::begin()
{
  _server.setNoDelay(true);
  _server.begin();
}

void AsyncWebServer::end()
{
  _server.end();
}

#if ASYNC_TCP_SSL_ENABLED
void AsyncWebServer::onSslFileRequest(AcSSlFileHandler cb, void* arg)
{
  _server.onSslFileRequest(cb, arg);
}

void AsyncWebServer::beginSecure(const char *cert, const char *key, const char *password)
{
  _server.beginSecure(cert, key, password);
}
#endif

void AsyncWebServer::_handleDisconnect(AsyncWebServerRequest *request)
{
  delete request;
}

void AsyncWebServer::_rewriteRequest(AsyncWebServerRequest *request)
{
  for (const auto& r : _rewrites)
  {
    if (r->match(request))
    {
      request->_url = r->toUrl();
      request->_addGetParams(r->params());
    }
  }
}

void AsyncWebServer::_attachHandler(AsyncWebServerRequest *request)
{
  for (const auto& h : _handlers)
  {
    if (h->filter(request) && h->canHandle(request))
    {
      request->setHandler(h);
      return;
    }
  }

  request->addInterestingHeader("ANY");
  request->setHandler(_catchAllHandler);
}


AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest,
                                            ArUploadHandlerFunction onUpload, ArBodyHandlerFunction onBody)
{
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();

  handler->setUri(uri);
  handler->setMethod(method);
  handler->onRequest(onRequest);
  handler->onUpload(onUpload);
  handler->onBody(onBody);
  addHandler(handler);

  return *handler;
}

AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest, ArUploadHandlerFunction onUpload)
{
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();
  handler->setUri(uri);
  handler->setMethod(method);
  handler->onRequest(onRequest);
  handler->onUpload(onUpload);
  addHandler(handler);

  return *handler;
}

AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest)
{
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();
  handler->setUri(uri);
  handler->setMethod(method);
  handler->onRequest(onRequest);
  addHandler(handler);

  return *handler;
}

AsyncCallbackWebHandler& AsyncWebServer::on(const char* uri, ArRequestHandlerFunction onRequest)
{
  AsyncCallbackWebHandler* handler = new AsyncCallbackWebHandler();
  handler->setUri(uri);
  handler->onRequest(onRequest);
  addHandler(handler);

  return *handler;
}

void AsyncWebServer::onNotFound(ArRequestHandlerFunction fn)
{
  _catchAllHandler->onRequest(fn);
}

void AsyncWebServer::onRequestBody(ArBodyHandlerFunction fn)
{
  _catchAllHandler->onBody(fn);
}

void AsyncWebServer::reset()
{
  _rewrites.free();
  _handlers.free();

  if (_catchAllHandler != NULL)
  {
    _catchAllHandler->onRequest(NULL);
    _catchAllHandler->onUpload(NULL);
    _catchAllHandler->onBody(NULL);
  }
}
