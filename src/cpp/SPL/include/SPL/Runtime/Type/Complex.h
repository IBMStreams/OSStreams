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

#ifndef SPL_RUNTIME_TYPE_COMPLEX_H
#define SPL_RUNTIME_TYPE_COMPLEX_H

/*!
 * \file Complex.h \brief Definitions of the %SPL complex types.
 */

#include <SPL/Runtime/Type/Float.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#include <complex>
#endif /*DOXYGEN_SKIP_FOR_USERS*/
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>

#define TYPEDEF_SPL_COMPLEX(bits) \
  typedef std::complex< float##bits > complex##bits ;

PUSH_DLL_PUBLIC
namespace SPL
{

  TYPEDEF_SPL_COMPLEX(32);
  TYPEDEF_SPL_COMPLEX(64);

  inline std::ostream & serializeWithPrecision(std::ostream & s, complex32 const & v)
  {
      std::streamsize p = s.precision(std::numeric_limits<float32>::digits10);
      s << v;
      s.precision(p);
      return s;
  }

  inline std::ostream & serializeWithPrecision(std::ostream & s, complex64 const & v)
  {
      std::streamsize p = s.precision(std::numeric_limits<float64>::digits10);
      s << v;
      s.precision(p);
      return s;
  }

  template <class Buffer, class T>
  inline void extractIntoComplex(Buffer& buf, std::complex<T>& x)
  {
#if __cplusplus >= 201103L
    // in C++11, std::complex::real and ::imag no longer return a reference
    T real;
    T imag;
    buf >> real;
    buf >> imag;
    x = std::complex<T>(real, imag);
#else
    buf >> x.real();
    buf >> x.imag();
#endif
  }

#ifndef DOXYGEN_SKIP_FOR_USERS
  template <class T>
  inline VirtualByteBuffer & operator<<(VirtualByteBuffer & buf, const std::complex<T> & x)
  {
    buf << x.real();
    buf << x.imag();
    return buf;
  }

  template <class T>
  inline VirtualByteBuffer & operator>>(VirtualByteBuffer & buf, std::complex<T> & x)
  {
    extractIntoComplex(buf, x);
    return buf;
  }
#endif /* DOXYGEN_SKIP_FOR_USERS */

  template <class T>
  inline NetworkByteBuffer & operator<<(NetworkByteBuffer & buf, const std::complex<T> & x)
  {
    buf << x.real();
    buf << x.imag();
    return buf;
  }

  template <class T>
  inline NativeByteBuffer & operator<<(NativeByteBuffer & buf, const std::complex<T> & x)
  {
    buf << x.real();
    buf << x.imag();
    return buf;
  }

  template <class S, class T>
  inline ByteBuffer<S> & operator<<(ByteBuffer<S> & buf, const std::complex<T> & x)
  {
    buf << x.real();
    buf << x.imag();
    return buf;
  }

  template <class T>
  inline NativeByteBuffer & operator>>(NativeByteBuffer & buf, std::complex<T> & x)
  {
    extractIntoComplex(buf, x);
    return buf;
  }

  template <class T>
  inline NetworkByteBuffer & operator>>(NetworkByteBuffer & buf, std::complex<T> & x)
  {
    extractIntoComplex(buf, x);
    return buf;
  }

  template <class S, class T>
  inline ByteBuffer<S> & operator>>(ByteBuffer<S> & buf, std::complex<T> & x)
  {
    extractIntoComplex(buf, x);
    return buf;
  }

  // use std::iostream for <<, >> character de/serializarion
  // use std::complex for swap
};

#ifndef DOXYGEN_SKIP_FOR_USERS
// Hashability requirements
namespace std {
    namespace tr1 {
        template<class T>
        struct hash<std::complex<T> > {
            inline size_t operator()(const std::complex<T> & s) const
            {
                size_t r = 17;
                hash<T> hs;
                r = 37 * r + hs(s.real());
                r = 37 * r + hs(s.imag());
                return r;
            }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_COMPLEX_H */
