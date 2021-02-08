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

#ifndef _AUTO_ARRAY_H_
#define _AUTO_ARRAY_H_

#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

template<typename _Tp>
class auto_array
{
  private:
    _Tp* _M_array;

  public:
    /// The pointed-to type.
    typedef _Tp element_type;

    /**
     *  @brief  An %auto_array is usually constructed from a raw pointer.
     *  @param  p  A pointer (defaults to NULL).
     *
     *  This object now @e owns the object pointed to by @a p.
     */
    explicit auto_array(element_type* __p = 0) throw()
      : _M_array(__p)
    {}

    /**
     *  @brief  An %auto_array can be constructed from another %auto_array.
     *  @param  a  Another %auto_array of the same type.
     *
     *  This object now @e owns the object previously owned by @a a, which has
     *  given up ownsership.
     */
    auto_array(auto_array& __a) throw()
      : _M_array(__a.release())
    {}

    /**
     *  @brief  An %auto_array can be constructed from another %auto_array.
     *  @param  a  Another %auto_array of a different but related type.
     *
     *  A pointer-to-Tp1 must be convertible to a pointer-to-Tp/element_type.
     *
     *  This object now @e owns the object previously owned by @a a, which has
     *  given up ownsership.
     */
    template<typename _Tp1>
    auto_array(auto_array<_Tp1>& __a) throw()
      : _M_array(__a.release())
    {}

    /**
     *  @brief  %auto_array assignment operator.
     *  @param  a  Another %auto_array of the same type.
     *
     *  This object now @e owns the object previously owned by @a a, which has
     *  given up ownsership.  The object that this one @e used to own and
     *  track has been deleted.
     */
    auto_array& operator=(auto_array& __a) throw()
    {
        reset(__a.release());
        return *this;
    }

    /**
     *  @brief  %auto_array assignment operator.
     *  @param  a  Another %auto_array of a different but related type.
     *
     *  A pointer-to-Tp1 must be convertible to a pointer-to-Tp/element_type.
     *
     *  This object now @e owns the object previously owned by @a a, which has
     *  given up ownsership.  The object that this one @e used to own and
     *  track has been deleted.
     */
    template<typename _Tp1>
    auto_array& operator=(auto_array<_Tp1>& __a) throw()
    {
        reset(__a.release());
        return *this;
    }

    element_type& operator[](size_t i) const { return _M_array[i]; }
    element_type& operator[](int i) const { return _M_array[i]; }

#if defined(__ia64) || defined(__x86_64) || defined(__amd64)
    element_type& operator[](unsigned int i) const { return _M_array[i]; }
#endif

    /**
     *  When the %auto_array goes out of scope, the object it owns is deleted.
     *  If it no longer owns anything (i.e., @c get() is @c NULL), then this
     *  has no effect.
     *
     *  @if maint
     *  The C++ standard says there is supposed to be an empty throw
     *  specification here, but omitting it is standard conforming.  Its
     *  presence can be detected only if _Tp::~_Tp() throws, but this is
     *  prohibited.  [17.4.3.6]/2
     *  @end maint
     */
    ~auto_array() { delete[] _M_array; }

    /**
     *  @brief  Bypassing the smart pointer.
     *  @return  The raw pointer being managed.
     *
     *  You can get a copy of the pointer that this object owns, for
     *  situations such as passing to a function which only accepts a raw
     *  pointer.
     *
     *  @note  This %auto_array still owns the memory.
     */
    element_type* get() const throw() { return _M_array; }
    element_type* operator*() const { return _M_array; }

    /**
     *  @brief  Bypassing the smart pointer.
     *  @return  The raw pointer being managed.
     *
     *  You can get a copy of the pointer that this object owns, for
     *  situations such as passing to a function which only accepts a raw
     *  pointer.
     *
     *  @note  This %auto_array no longer owns the memory.  When this object
     *  goes out of scope, nothing will happen.
     */
    element_type* release() throw()
    {
        element_type* __tmp = _M_array;
        _M_array = 0;
        return __tmp;
    }

    /**
     *  @brief  Forcibly deletes the managed object.
     *  @param  p  A pointer (defaults to NULL).
     *
     *  This object now @e owns the object pointed to by @a p.  The previous
     *  object has been deleted.
     */
    void reset(element_type* __p = 0) throw()
    {
        if (__p != _M_array) {
            delete[] _M_array;
            _M_array = __p;
        }
    }
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
