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

#ifndef SPL_RUNTIME_TYPE_INTEGER_H
#define SPL_RUNTIME_TYPE_INTEGER_H

/*!
 * \file Integer.h \brief Definitions of the %SPL integer types and boolean type.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Type/TypeHelper.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <limits>
#include <stdint.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>

#define TYPEDEF_SPL_INT(bits) \
  typedef int##bits##_t int##bits ;

#define TYPEDEF_SPL_UINT(bits) \
  typedef uint##bits##_t uint##bits ;

// GCC 4.4 seems to emit some spurious strict-aliasing
// warnings.
#if (__GNUC__ == 4 && __GNUC_MINOR__ == 4)
     #define SPL_MAY_ALIAS __attribute__((__may_alias__))
#else
     #define SPL_MAY_ALIAS
#endif

#define SPL_MAKE_NUMERIC_TYPE(name,base)       \
  class SPL_MAY_ALIAS name {                   \
  public:                                      \
    name() : val_() {}                         \
    name(const name &o)                        \
      : val_(o.val_) {}                        \
    name(const base &o)                        \
      : val_(o) {}                             \
    template <class T>                         \
    explicit name(const T & o)                 \
      : val_((base)o) {}                       \
    operator base & ()                         \
      { return val_; }                         \
    operator const base & () const             \
      { return val_; }                         \
    const name & operator=(const base & o)     \
      { val_ = o;                              \
        const name*t = this; return *t; }      \
    template<class T>                          \
    const name & operator=(const T & o)        \
      { val_ = (base)o;                        \
        const name* t = this; return *t; }     \
  private:                                     \
    base val_;                                 \
  };                                           \

PUSH_DLL_PUBLIC
namespace SPL
{
  // std::vector does not treat bool vectors the same way it treats the other
  // types, so we roll our own boolean
#ifndef DOXYGEN_SKIP_FOR_USERS
  SPL_MAKE_NUMERIC_TYPE(boolean, bool);
#else
  typedef _<...> boolean;
#endif /*DOXYGEN_SKIP_FOR_USERS*/

  // std::iostream uses <<,>> on int8 and uint8 assuming it is a character
  // so we roll our own implementations
#ifndef DOXYGEN_SKIP_FOR_USERS
  SPL_MAKE_NUMERIC_TYPE(int8, signed char);
  SPL_MAKE_NUMERIC_TYPE(uint8, unsigned char);
#else
  typedef _<...> int8;
  typedef _<...> uint8;
#endif /*DOXYGEN_SKIP_FOR_USERS*/

  TYPEDEF_SPL_INT(16);
  TYPEDEF_SPL_INT(32);
  TYPEDEF_SPL_INT(64);

  TYPEDEF_SPL_UINT(16);
  TYPEDEF_SPL_UINT(32);
  TYPEDEF_SPL_UINT(64);

  // use ByteBufferHelpers.h for <<, >> binary de/serialization
  // use std::iostream for <<, >> character de/serializarion
  // use std::swap

#ifndef DOXYGEN_SKIP_FOR_USERS
  inline VirtualByteBuffer & operator<<(VirtualByteBuffer & sbuf, const boolean & x)
    { sbuf.addBool(x); return sbuf; }

  inline VirtualByteBuffer & operator>>(VirtualByteBuffer & sbuf, boolean & x)
    { x = sbuf.getBool(); return sbuf; }

  inline VirtualByteBuffer & operator<<(VirtualByteBuffer & sbuf, const int8 & x)
    { sbuf.addInt8(x); return sbuf; }

  inline VirtualByteBuffer & operator>>(VirtualByteBuffer & sbuf, int8 & x)
    { x = sbuf.getInt8(); return sbuf; }

  inline VirtualByteBuffer & operator<<(VirtualByteBuffer & sbuf, const uint8 & x)
    { sbuf.addUInt8(x); return sbuf; }

  inline VirtualByteBuffer & operator>>(VirtualByteBuffer & sbuf, uint8 & x)
    { x = sbuf.getUInt8(); return sbuf; }
#endif /* DOXYGEN_SKIP_FOR_USERS */

  template <class S>
  inline ByteBuffer<S> & operator<<(ByteBuffer<S> & sbuf, const boolean & x)
    { sbuf.addBool(x); return sbuf; }

  inline NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, const boolean & x)
    { sbuf.addBool(x); return sbuf; }

  inline NativeByteBuffer & operator<<(NativeByteBuffer & sbuf, const boolean & x)
    { sbuf.addBool(x); return sbuf; }

  template <class S>
  inline ByteBuffer<S> & operator>>(ByteBuffer<S> & sbuf, boolean & x)
    { x = sbuf.getBool(); return sbuf; }

  inline NetworkByteBuffer & operator>>(NetworkByteBuffer & sbuf, boolean & x)
    { x = sbuf.getBool(); return sbuf; }

  inline NativeByteBuffer & operator>>(NativeByteBuffer & sbuf, boolean & x)
    { x = sbuf.getBool(); return sbuf; }

  template <class S>
  inline ByteBuffer<S> & operator<<(ByteBuffer<S> & sbuf, const int8 & x)
    { sbuf.addInt8(x); return sbuf; }

  inline NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, const int8 & x)
    { sbuf.addInt8(x); return sbuf; }

  inline NativeByteBuffer & operator<<(NativeByteBuffer & sbuf, const int8 & x)
    { sbuf.addInt8(x); return sbuf; }

  template <class S>
  inline ByteBuffer<S> & operator>>(ByteBuffer<S> & sbuf, int8 & x)
    { x = sbuf.getInt8(); return sbuf; }

  inline NetworkByteBuffer & operator>>(NetworkByteBuffer & sbuf, int8 & x)
    { x = sbuf.getInt8(); return sbuf; }

  inline NativeByteBuffer & operator>>(NativeByteBuffer & sbuf, int8 & x)
    { x = sbuf.getInt8(); return sbuf; }

  template <class S>
  inline ByteBuffer<S> & operator<<(ByteBuffer<S> & sbuf, const uint8 & x)
    { sbuf.addUInt8(x); return sbuf; }

  inline NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, const uint8 & x)
    { sbuf.addUInt8(x); return sbuf; }

  inline NativeByteBuffer & operator<<(NativeByteBuffer & sbuf, const uint8 & x)
    { sbuf.addUInt8(x); return sbuf; }

  template <class S>
  inline ByteBuffer<S> & operator>>(ByteBuffer<S> & sbuf, uint8 & x)
    { x = sbuf.getUInt8(); return sbuf; }

  inline NetworkByteBuffer & operator>>(NetworkByteBuffer & sbuf, uint8 & x)
    { x = sbuf.getUInt8(); return sbuf; }

  inline NativeByteBuffer & operator>>(NativeByteBuffer & sbuf, uint8 & x)
    { x = sbuf.getUInt8(); return sbuf; }

  inline std::ostream & operator<<(std::ostream & ostr, const boolean & x)
    {
        if (x) {
            ostr << "true";
        } else {
            ostr << "false";
        }
        return ostr;
    }

  std::istream & operator>>(std::istream & istr, boolean & x);

  inline std::ostream & operator<<(std::ostream & ostr, const int8 & x)
    { ostr << static_cast<int16_t>(x); return ostr;  }

  std::istream & operator>>(std::istream & istr, int8 & x);

  inline std::ostream & operator<<(std::ostream & ostr, const uint8 x)
    { ostr << static_cast<uint16_t>(x); return ostr;  }

  std::istream & operator>>(std::istream & istr, uint8 & x);


  inline void deserializeWithSuffix(std::istream & s, int8 & v)
  {
     parseWithSuffixes(s,v,'s','b');
  }

  inline void deserializeWithSuffix(std::istream & s, int16 & v)
  {
     parseWithSuffixes(s,v,'s','h');
  }

  inline void deserializeWithSuffix(std::istream & s, int32 & v)
  {
     parseWithSuffixes(s,v,'s','w');
  }

  inline void deserializeWithSuffix(std::istream & s, int64 & v)
  {
     parseWithSuffixes(s,v,'s','l');
  }

  inline void deserializeWithSuffix(std::istream & s, uint8 & v)
  {
     parseWithSuffixes(s,v,'u','b');
  }

  inline void deserializeWithSuffix(std::istream & s, uint16 & v)
  {
     parseWithSuffixes(s,v,'u','h');
  }

  inline void deserializeWithSuffix(std::istream & s, uint32 & v)
  {
     parseWithSuffixes(s,v,'u','w');
  }

  inline void deserializeWithSuffix(std::istream & s, uint64 & v)
  {
     parseWithSuffixes(s,v,'u','l');
  }

  inline void deserializeWithNanAndInfs(std::istream & s, int8 & v, bool withSuffix = false)
  {
      if (withSuffix) {
          parseWithSuffixes(s, v, 's', 'b');
      } else {
          s >> v;
      }
  }

  inline void deserializeWithNanAndInfs(std::istream & s, int16 & v, bool withSuffix = false)
  {
      if (withSuffix) {
          parseWithSuffixes(s, v, 's', 'h');
      } else {
          s >> v;
      }
  }

  inline void deserializeWithNanAndInfs(std::istream & s, int32 & v, bool withSuffix = false)
  {
      if (withSuffix) {
          parseWithSuffixes(s, v, 's', 'w');
      } else {
          s >> v;
      }
  }

  inline void deserializeWithNanAndInfs(std::istream & s, int64 & v, bool withSuffix = false)
  {
      if (withSuffix) {
          parseWithSuffixes(s, v, 's', 'l');
      } else {
          s >> v;
      }
  }

  inline void deserializeWithNanAndInfs(std::istream & s, uint8 & v, bool withSuffix = false)
  {
      if (withSuffix) {
          parseWithSuffixes(s, v, 'u', 'b');
      } else {
          s >> v;
      }
  }

  inline void deserializeWithNanAndInfs(std::istream & s, uint16 & v, bool withSuffix = false)
  {
      if (withSuffix) {
          parseWithSuffixes(s, v, 'u', 'h');
      } else {
          s >> v;
      }
  }

  inline void deserializeWithNanAndInfs(std::istream & s, uint32 & v, bool withSuffix = false)
  {
      if (withSuffix) {
          parseWithSuffixes(s, v, 'u', 'w');
      } else {
          s >> v;
      }
  }

  inline void deserializeWithNanAndInfs(std::istream & s, uint64 & v, bool withSuffix = false)
  {
      if (withSuffix) {
          parseWithSuffixes(s, v, 'u', 'l');
      } else {
          s >> v;
      }
  }

};

#ifndef DOXYGEN_SKIP_FOR_USERS
// Hashability requirements
namespace std {
    namespace tr1 {
        template<>
        struct hash<SPL::boolean> {
            inline size_t operator()(const SPL::boolean & b) const
            {
                return hash<size_t>()(static_cast<size_t>(b));
            }
        };

        template<>
        struct hash<SPL::int8> {
            inline size_t operator()(const SPL::int8 & b) const
            {
                return hash<size_t>()(static_cast<size_t>(b));
            }
        };

        template<>
        struct hash<SPL::uint8> {
            inline size_t operator()(const SPL::uint8 & b) const
            {
                return hash<size_t>()(static_cast<size_t>(b));
            }
        };
    }

    template<>
    struct numeric_limits<SPL::boolean> : public numeric_limits<bool> {};
    template<>
    struct numeric_limits<SPL::int8> : public numeric_limits<int8_t> {};
    template<>
    struct numeric_limits<SPL::uint8> : public numeric_limits<uint8_t> {};
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_INTEGER_H */
