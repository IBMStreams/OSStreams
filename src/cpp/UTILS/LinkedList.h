/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <UTILS/UTILSTypes.h>
#include <cassert>
#include <ostream>

UTILS_NAMESPACE_BEGIN

template<typename T>
class LinkedList
{
  protected:
    struct list_element_t
    {
        list_element_t* _next;
    };

    struct data_element_t : public list_element_t
    {
        T _data;
    };

    struct data_element_t* _head;
    struct data_element_t* _tail;
    unsigned int _size;

  public:
    class list_iterator
    {
      protected:
        struct data_element_t* _current;

      public:
        list_iterator(struct data_element_t* head)
          : _current(head)
        {}

        inline void next()
        {
            assert(_current != NULL);

            _current = (data_element_t*)(_current->_next);
        }

        inline bool end() const { return _current == NULL; }

        inline T& operator*()
        {
            assert(_current != NULL);

            return _current->_data;
        }

        inline list_iterator& operator++(int)
        {
            next();
            return *this;
        }

        friend class LinkedList;
    };

    class list_const_iterator
    {
      protected:
        const struct data_element_t* _current;

      public:
        list_const_iterator(const struct data_element_t* current)
          : _current(current)
        {}

        inline void next()
        {
            assert(_current != NULL);

            _current = (data_element_t*)(_current->_next);
        }

        inline bool end() const { return _current == NULL; }

        inline const T& operator*()
        {
            assert(_current != NULL);

            return _current->_data;
        }

        inline list_const_iterator& operator++(int)
        {
            next();
            return *this;
        }

        friend class LinkedList;
    };

  protected:
    void erase_ptr(const data_element_t* ptr)
    {
        assert(_size > 0);

        struct data_element_t* c = _head;

        if (ptr == _head) {
            if (ptr == _tail) {
                assert(_size == 1);

                _head = NULL;
                _tail = NULL;
                _size = 0;
            } else {
                _head = (data_element_t*)_head->_next;
                _size--;
            }

            delete c;
        } else {
            while (c->_next != ptr) {
                c = (data_element_t*)c->_next;
                if (c == NULL) {
                    break;
                }
            }

            if (c != NULL) {
                if (c->_next == _tail) {
                    _tail = c;
                }

                struct data_element_t* c2 = (data_element_t*)c->_next;
                c->_next = c->_next->_next;
                delete c2;
                _size--;
            }
        }
    }

  public:
    typedef list_iterator iterator;
    typedef list_const_iterator const_iterator;

    LinkedList()
      : _head(NULL)
      , _tail(NULL)
      , _size(0)
    {}

    ~LinkedList()
    {
        if (_head) {
            struct data_element_t* e = _head;

            while (e) {
                struct data_element_t* c = e;
                e = (data_element_t*)e->_next;
                delete c;
            }
        }
    }

    void append(const T& e)
    {
        struct data_element_t* new_elem = new struct data_element_t;
        new_elem->_data = e;
        new_elem->_next = NULL;

        if (_tail) {
            _tail->_next = new_elem;
            _tail = new_elem;
        } else {
            _head = new_elem;
            _tail = _head;
        }

        _size++;
    }

    void erase(const list_iterator& it) { erase_ptr(it._current); }

    void erase(const list_const_iterator& it) { erase_ptr(it._current); }

    void clear()
    {
        while (size()) {
            list_iterator it(begin());
            erase(it);
        }
    }

    inline unsigned int size() const { return _size; }

    iterator begin() { return list_iterator(_head); }

    const_iterator begin() const { return list_const_iterator(_head); }

    void print(std::ostream& strm,
               const char* mark_begin,
               const char* mark_sep,
               const char* mark_end) const
    {
        list_const_iterator it(begin());

        strm << mark_begin;
        bool first = true;

        while (!it.end()) {
            if (!first) {
                strm << mark_sep;
            }
            strm << *it;

            first = false;
            it++;
        }

        strm << mark_end;
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& strm, const LinkedList<T>& list)
{
    list.print(strm, "<", ",", ">");
    return strm;
}

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
