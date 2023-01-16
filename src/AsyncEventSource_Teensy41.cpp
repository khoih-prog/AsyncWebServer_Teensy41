/****************************************************************************************************************************
  AsyncEventSource_Teensy41.cpp - Dead simple AsyncWebServer for Teensy41 QNEthernet

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

#if !defined(_AWS_TEENSY41_LOGLEVEL_)
  #define _AWS_TEENSY41_LOGLEVEL_     1
#endif

#include "AsyncWebServer_Teensy41_Debug.h"

#include "Arduino.h"
#include "AsyncEventSource_Teensy41.h"

/////////////////////////////////////////////////////////

static String generateEventMessage(const char *message, const char *event, uint32_t id, uint32_t reconnect)
{
  String ev = "";

  if (reconnect)
  {
    ev += "retry: ";
    ev += String(reconnect);
    ev += "\r\n";
  }

  if (id)
  {
    ev += "id: ";
    ev += String(id);
    ev += "\r\n";
  }

  if (event != NULL)
  {
    ev += "event: ";
    ev += String(event);
    ev += "\r\n";
  }

  if (message != NULL)
  {
    size_t messageLen = strlen(message);
    char * lineStart = (char *)message;
    char * lineEnd;

    do
    {
      char * nextN = strchr(lineStart, '\n');
      char * nextR = strchr(lineStart, '\r');

      if (nextN == NULL && nextR == NULL)
      {
        size_t llen = ((char *)message + messageLen) - lineStart;
        char * ldata = (char *)malloc(llen + 1);

        if (ldata != NULL)
        {
          memcpy(ldata, lineStart, llen);
          ldata[llen] = 0;
          ev += "data: ";
          ev += ldata;
          ev += "\r\n\r\n";
          free(ldata);
        }

        lineStart = (char *)message + messageLen;
      }
      else
      {
        char * nextLine = NULL;

        if (nextN != NULL && nextR != NULL)
        {
          if (nextR < nextN)
          {
            lineEnd = nextR;

            if (nextN == (nextR + 1))
              nextLine = nextN + 1;
            else
              nextLine = nextR + 1;
          }
          else
          {
            lineEnd = nextN;

            if (nextR == (nextN + 1))
              nextLine = nextR + 1;
            else
              nextLine = nextN + 1;
          }
        }
        else if (nextN != NULL)
        {
          lineEnd = nextN;
          nextLine = nextN + 1;
        }
        else
        {
          lineEnd = nextR;
          nextLine = nextR + 1;
        }

        size_t llen = lineEnd - lineStart;
        char * ldata = (char *)malloc(llen + 1);

        if (ldata != NULL)
        {
          memcpy(ldata, lineStart, llen);
          ldata[llen] = 0;
          ev += "data: ";
          ev += ldata;
          ev += "\r\n";

          free(ldata);
        }

        lineStart = nextLine;

        if (lineStart == ((char *)message + messageLen))
          ev += "\r\n";
      }
    } while (lineStart < ((char *)message + messageLen));
  }

  return ev;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

// Message

AsyncEventSourceMessage::AsyncEventSourceMessage(const char * data, size_t len)
  : _data(nullptr), _len(len), _sent(0), _acked(0)
{
  _data = (uint8_t*)malloc(_len + 1);

  if (_data == nullptr)
  {
    _len = 0;
  }
  else
  {
    memcpy(_data, data, len);
    _data[_len] = 0;
  }
}

/////////////////////////////////////////////////////////

AsyncEventSourceMessage::~AsyncEventSourceMessage()
{
  if (_data != NULL)
    free(_data);
}

/////////////////////////////////////////////////////////

size_t AsyncEventSourceMessage::ack(size_t len, uint32_t time)
{
  AWS_TEENSY41_UNUSED(time);

  // If the whole message is now acked...
  if (_acked + len > _len)
  {
    // Return the number of extra bytes acked (they will be carried on to the next message)
    const size_t extra = _acked + len - _len;
    _acked = _len;

    return extra;
  }

  // Return that no extra bytes left.
  _acked += len;

  return 0;
}

/////////////////////////////////////////////////////////

size_t AsyncEventSourceMessage::send(AsyncClient *client)
{
  const size_t len = _len - _sent;

  if (client->space() < len)
  {
    return 0;
  }

  size_t sent = client->add((const char *)_data, len);

  if (client->canSend())
    client->send();

  _sent += sent;

  return sent;
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

// Client

AsyncEventSourceClient::AsyncEventSourceClient(AsyncWebServerRequest *request, AsyncEventSource *server)
  : _messageQueue(LinkedList<AsyncEventSourceMessage * >([](AsyncEventSourceMessage * m)
{
  delete  m;
}))
{
  _client = request->client();
  _server = server;
  _lastId = 0;

  if (request->hasHeader("Last-Event-ID"))
    _lastId = atoi(request->getHeader("Last-Event-ID")->value().c_str());

  _client->setRxTimeout(0);
  _client->onError(NULL, NULL);
  _client->onAck([](void *r, AsyncClient * c, size_t len, uint32_t time)
  {
    AWS_TEENSY41_UNUSED(c);
    ((AsyncEventSourceClient*)(r))->_onAck(len, time);
  }, this);

  _client->onPoll([](void *r, AsyncClient * c)
  {
    AWS_TEENSY41_UNUSED(c);
    ((AsyncEventSourceClient*)(r))->_onPoll();
  }, this);

  _client->onData(NULL, NULL);

  _client->onTimeout([this](void *r, AsyncClient * c __attribute__((unused)), uint32_t time)
  {
    ((AsyncEventSourceClient*)(r))->_onTimeout(time);
  }, this);

  _client->onDisconnect([this](void *r, AsyncClient * c)
  {
    ((AsyncEventSourceClient*)(r))->_onDisconnect();
    delete c;
  }, this);

  _server->_addClient(this);

  delete request;
}

/////////////////////////////////////////////////////////

AsyncEventSourceClient::~AsyncEventSourceClient()
{
  _messageQueue.free();
  close();
}

/////////////////////////////////////////////////////////

void AsyncEventSourceClient::_queueMessage(AsyncEventSourceMessage *dataMessage)
{
  if (dataMessage == NULL)
    return;

  if (!connected())
  {
    delete dataMessage;
    return;
  }

  if (_messageQueue.length() >= SSE_MAX_QUEUED_MESSAGES)
  {
    LOGERROR("ERROR: Too many messages queued");
    delete dataMessage;
  }
  else
  {
    _messageQueue.add(dataMessage);
  }

  if (_client->canSend())
    _runQueue();
}

/////////////////////////////////////////////////////////

void AsyncEventSourceClient::_onAck(size_t len, uint32_t time)
{
  while (len && !_messageQueue.isEmpty())
  {
    len = _messageQueue.front()->ack(len, time);

    if (_messageQueue.front()->finished())
      _messageQueue.remove(_messageQueue.front());
  }

  _runQueue();
}

/////////////////////////////////////////////////////////

void AsyncEventSourceClient::_onPoll()
{
  if (!_messageQueue.isEmpty())
  {
    _runQueue();
  }
}

/////////////////////////////////////////////////////////

void AsyncEventSourceClient::_onTimeout(uint32_t time __attribute__((unused)))
{
  _client->close(true);
}

/////////////////////////////////////////////////////////

void AsyncEventSourceClient::_onDisconnect()
{
  _client = NULL;
  _server->_handleDisconnect(this);
}

/////////////////////////////////////////////////////////

void AsyncEventSourceClient::close()
{
  if (_client != NULL)
    _client->close();
}

/////////////////////////////////////////////////////////

void AsyncEventSourceClient::write(const char * message, size_t len)
{
  _queueMessage(new AsyncEventSourceMessage(message, len));
}

/////////////////////////////////////////////////////////

void AsyncEventSourceClient::send(const char *message, const char *event, uint32_t id, uint32_t reconnect)
{
  String ev = generateEventMessage(message, event, id, reconnect);
  _queueMessage(new AsyncEventSourceMessage(ev.c_str(), ev.length()));
}

/////////////////////////////////////////////////////////

void AsyncEventSourceClient::_runQueue()
{
  while (!_messageQueue.isEmpty() && _messageQueue.front()->finished())
  {
    _messageQueue.remove(_messageQueue.front());
  }

  for (auto i = _messageQueue.begin(); i != _messageQueue.end(); ++i)
  {
    if (!(*i)->sent())
      (*i)->send(_client);
  }
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

// Handler

AsyncEventSource::AsyncEventSource(const String& url)
  : _url(url)
  , _clients(LinkedList<AsyncEventSourceClient * >([](AsyncEventSourceClient * c)
{
  delete c;
}))
, _connectcb(NULL)
{}

/////////////////////////////////////////////////////////

AsyncEventSource::~AsyncEventSource()
{
  close();
}

/////////////////////////////////////////////////////////

void AsyncEventSource::onConnect(ArEventHandlerFunction cb)
{
  _connectcb = cb;
}

/////////////////////////////////////////////////////////

void AsyncEventSource::_addClient(AsyncEventSourceClient * client)
{
  _clients.add(client);

  if (_connectcb)
    _connectcb(client);
}

/////////////////////////////////////////////////////////

void AsyncEventSource::_handleDisconnect(AsyncEventSourceClient * client)
{
  _clients.remove(client);
}

/////////////////////////////////////////////////////////

void AsyncEventSource::close()
{
  for (const auto &c : _clients)
  {
    if (c->connected())
      c->close();
  }
}

/////////////////////////////////////////////////////////

// pmb fix
size_t AsyncEventSource::avgPacketsWaiting() const
{
  if (_clients.isEmpty())
    return 0;

  size_t    aql = 0;
  uint32_t  nConnectedClients = 0;

  for (const auto &c : _clients)
  {
    if (c->connected())
    {
      aql += c->packetsWaiting();
      ++nConnectedClients;
    }
  }

  //  return aql / nConnectedClients;
  return ((aql) + (nConnectedClients / 2)) / (nConnectedClients); // round up
}

/////////////////////////////////////////////////////////

void AsyncEventSource::send(const char *message, const char *event, uint32_t id, uint32_t reconnect)
{
  String ev = generateEventMessage(message, event, id, reconnect);

  for (const auto &c : _clients)
  {
    if (c->connected())
    {
      c->write(ev.c_str(), ev.length());
    }
  }
}

/////////////////////////////////////////////////////////

size_t AsyncEventSource::count() const
{
  return _clients.count_if([](AsyncEventSourceClient * c)
  {
    return c->connected();
  });
}

/////////////////////////////////////////////////////////

bool AsyncEventSource::canHandle(AsyncWebServerRequest *request)
{
  if (request->method() != HTTP_GET || !request->url().equals(_url))
  {
    return false;
  }

  request->addInterestingHeader("Last-Event-ID");

  return true;
}

/////////////////////////////////////////////////////////

void AsyncEventSource::handleRequest(AsyncWebServerRequest *request)
{
  if ((_username != "" && _password != "") && !request->authenticate(_username.c_str(), _password.c_str()))
    return request->requestAuthentication();

  request->send(new AsyncEventSourceResponse(this));
}

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

// Response

AsyncEventSourceResponse::AsyncEventSourceResponse(AsyncEventSource *server)
{
  _server = server;
  _code = 200;
  _contentType = "text/event-stream";
  _sendContentLength = false;
  addHeader("Cache-Control", "no-cache");
  addHeader("Connection", "keep-alive");
}

/////////////////////////////////////////////////////////

void AsyncEventSourceResponse::_respond(AsyncWebServerRequest *request)
{
  String out = _assembleHead(request->version());
  request->client()->write(out.c_str(), _headLength);
  _state = RESPONSE_WAIT_ACK;
}

/////////////////////////////////////////////////////////

size_t AsyncEventSourceResponse::_ack(AsyncWebServerRequest *request, size_t len, uint32_t time __attribute__((unused)))
{
  if (len)
  {
    new AsyncEventSourceClient(request, _server);
  }

  return 0;
}

/////////////////////////////////////////////////////////


