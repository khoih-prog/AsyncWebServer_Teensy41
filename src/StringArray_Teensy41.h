/****************************************************************************************************************************
  StringArray_Teensy41.h - Dead simple AsyncWebServer for Teensy41 QNEthernet

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

#ifndef STRINGARRAY_TEENSY41_H_
#define STRINGARRAY_TEENSY41_H_

#include "stddef.h"
#include "WString.h"

/////////////////////////////////////////////////

template <typename T>
class LinkedListNode
{
    T _value;

  public:
    LinkedListNode<T>* next;
    LinkedListNode(const T val): _value(val), next(nullptr) {}
    ~LinkedListNode() {}

    /////////////////////////////////////////////////

    inline const T& value() const
    {
      return _value;
    };

    /////////////////////////////////////////////////

    inline T& value()
    {
      return _value;
    }
};

/////////////////////////////////////////////////
/////////////////////////////////////////////////

template <typename T, template<typename> class Item = LinkedListNode>
class LinkedList
{
  public:
    typedef Item<T> ItemType;
    typedef std::function<void(const T&)> OnRemove;
    typedef std::function<bool(const T&)> Predicate;

  private:
    ItemType* _root;
    OnRemove _onRemove;

    class Iterator
    {
        ItemType* _node;

      public:
        Iterator(ItemType* current = nullptr) : _node(current) {}
        Iterator(const Iterator& i) : _node(i._node) {}

        /////////////////////////////////////////////////

        inline Iterator& operator ++()
        {
          _node = _node->next;
          return *this;
        }

        /////////////////////////////////////////////////

        inline bool operator != (const Iterator& i) const
        {
          return _node != i._node;
        }

        /////////////////////////////////////////////////

        inline const T& operator * () const
        {
          return _node->value();
        }

        /////////////////////////////////////////////////

        inline const T* operator -> () const
        {
          return &_node->value();
        }
    };

  public:
    typedef const Iterator ConstIterator;

    /////////////////////////////////////////////////

    inline ConstIterator begin() const
    {
      return ConstIterator(_root);
    }

    /////////////////////////////////////////////////

    inline ConstIterator end() const
    {
      return ConstIterator(nullptr);
    }

    /////////////////////////////////////////////////

    LinkedList(OnRemove onRemove) : _root(nullptr), _onRemove(onRemove) {}
    ~LinkedList() {}

    /////////////////////////////////////////////////

    void add(const T& t)
    {
      auto it = new ItemType(t);

      if (!_root)
      {
        _root = it;
      }
      else
      {
        auto i = _root;

        while (i->next)
          i = i->next;

        i->next = it;
      }
    }

    /////////////////////////////////////////////////

    inline T& front() const
    {
      return _root->value();
    }

    /////////////////////////////////////////////////

    inline bool isEmpty() const
    {
      return _root == nullptr;
    }

    /////////////////////////////////////////////////

    size_t length() const
    {
      size_t i = 0;
      auto it = _root;

      while (it)
      {
        i++;
        it = it->next;
      }

      return i;
    }

    /////////////////////////////////////////////////

    size_t count_if(Predicate predicate) const
    {
      size_t i = 0;
      auto it = _root;

      while (it)
      {
        if (!predicate)
        {
          i++;
        }
        else if (predicate(it->value()))
        {
          i++;
        }

        it = it->next;
      }

      return i;
    }

    /////////////////////////////////////////////////

    const T* nth(size_t N) const
    {
      size_t i = 0;
      auto it = _root;

      while (it)
      {
        if (i++ == N)
          return &(it->value());

        it = it->next;
      }

      return nullptr;
    }

    /////////////////////////////////////////////////

    bool remove(const T& t)
    {
      auto it = _root;
      auto pit = _root;

      while (it)
      {
        if (it->value() == t)
        {
          if (it == _root)
          {
            _root = _root->next;
          }
          else
          {
            pit->next = it->next;
          }

          if (_onRemove)
          {
            _onRemove(it->value());
          }

          delete it;
          return true;
        }

        pit = it;
        it = it->next;
      }

      return false;
    }

    /////////////////////////////////////////////////

    bool remove_first(Predicate predicate)
    {
      auto it = _root;
      auto pit = _root;

      while (it)
      {
        if (predicate(it->value()))
        {
          if (it == _root)
          {
            _root = _root->next;
          }
          else
          {
            pit->next = it->next;
          }

          if (_onRemove)
          {
            _onRemove(it->value());
          }

          delete it;
          return true;
        }

        pit = it;
        it = it->next;
      }

      return false;
    }

    /////////////////////////////////////////////////

    void free()
    {
      while (_root != nullptr)
      {
        auto it = _root;
        _root = _root->next;

        if (_onRemove)
        {
          _onRemove(it->value());
        }

        delete it;
      }

      _root = nullptr;
    }
};

/////////////////////////////////////////////////
/////////////////////////////////////////////////

class StringArray : public LinkedList<String>
{
  public:

    StringArray() : LinkedList(nullptr) {}

    /////////////////////////////////////////////////

    bool containsIgnoreCase(const String& str)
    {
      for (const auto& s : *this)
      {
        if (str.equalsIgnoreCase(s))
        {
          return true;
        }
      }

      return false;
    }
};

#endif /* STRINGARRAY_TEENSY41_H_ */
