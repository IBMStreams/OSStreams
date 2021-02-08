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

#ifndef SPL_RUNTIME_TYPE_DECIMAL_H
#define SPL_RUNTIME_TYPE_DECIMAL_H

/*!
 * \file Decimal.h \brief Definitions of the %SPL decimal types.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Type/TypeHelper.h>
#include <SPL/Runtime/Utility/Visibility.h>
PUSH_DLL_PUBLIC
#define  _DEC_HAS_TYPE_TRAITS 1
#include <decimal>
#include <cdecfloat>
#define dec_isinf(x) ibm::decimal::_DecNumber(x)._IsINF()
#endif /* DOXYGEN_SKIP_FOR_USERS */
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>

namespace SPL
{
    class NetworkByteBuffer;

    typedef ibm::decimal::decimal32 decimal32;
    typedef ibm::decimal::decimal64 decimal64;
    typedef ibm::decimal::decimal128 decimal128;

    inline std::ostream & serializeWithPrecision(std::ostream & s, decimal32 const & v)
    {
        if (dec_isinf(v)) {
            s << ((v < 0) ? "-inf" : "inf");
        } else {
            std::streamsize p = s.precision(DEC32_MANT_DIG);
            s << v;
            s.precision(p);
        }
        return s;
    }

    inline std::ostream & serializeWithPrecision(std::ostream & s, decimal64 const & v)
    {
        if (dec_isinf(v)) {
            s << ((v < 0) ? "-inf" : "inf");
        } else {
            std::streamsize p = s.precision(DEC64_MANT_DIG);
            s << v;
            s.precision(p);
        }
        return s;
    }

    inline std::ostream & serializeWithPrecision(std::ostream & s, decimal128 const & v)
    {
        if (dec_isinf(v)) {
            s << ((v < 0) ? "-inf" : "inf");
        } else {
            std::streamsize p = s.precision(DEC128_MANT_DIG);
            s << v;
            s.precision(p);
        }
        return s;
    }

#ifndef DOXYGEN_SKIP_FOR_USERS
    VirtualByteBuffer & operator <<(VirtualByteBuffer & buf, const decimal32 & x);
    VirtualByteBuffer & operator >>(VirtualByteBuffer & buf, decimal32 & x);
    VirtualByteBuffer & operator <<(VirtualByteBuffer & buf, const decimal64 & x);
    VirtualByteBuffer & operator >>(VirtualByteBuffer & buf, decimal64 & x);
    VirtualByteBuffer & operator <<(VirtualByteBuffer & buf, const decimal128 & x);
    VirtualByteBuffer & operator >>(VirtualByteBuffer & buf, decimal128 & x);
    ByteBuffer<SPL::NetworkByteBuffer> & serialize(ByteBuffer<SPL::NetworkByteBuffer> & buf, const decimal32 & x);
    ByteBuffer<SPL::NetworkByteBuffer> & deserialize(ByteBuffer<SPL::NetworkByteBuffer> & buf, decimal32 & x);
    ByteBuffer<SPL::NetworkByteBuffer> & serialize(ByteBuffer<SPL::NetworkByteBuffer> & buf, const decimal64 & x);
    ByteBuffer<SPL::NetworkByteBuffer> & deserialize(ByteBuffer<SPL::NetworkByteBuffer> & buf, decimal64 & x);
    ByteBuffer<SPL::NetworkByteBuffer> & serialize(ByteBuffer<SPL::NetworkByteBuffer> & buf, const decimal128 & x);
    ByteBuffer<SPL::NetworkByteBuffer> & deserialize(ByteBuffer<SPL::NetworkByteBuffer> & buf, decimal128 & x);
    SPL::NetworkByteBuffer & serialize(SPL::NetworkByteBuffer & buf, const decimal32 & x);
    SPL::NetworkByteBuffer & deserialize(SPL::NetworkByteBuffer & buf, decimal32 & x);
    SPL::NetworkByteBuffer & serialize(SPL::NetworkByteBuffer & buf, const decimal64 & x);
    SPL::NetworkByteBuffer & deserialize(SPL::NetworkByteBuffer & buf, decimal64 & x);
    SPL::NetworkByteBuffer & serialize(SPL::NetworkByteBuffer & buf, const decimal128 & x);
    SPL::NetworkByteBuffer & deserialize(SPL::NetworkByteBuffer & buf, decimal128 & x);
#endif /* DOXYGEN_SKIP_FOR_USERS */

    template <class BufferType>
    inline ByteBuffer<BufferType> & operator <<(ByteBuffer<BufferType> & buf, const decimal32 & x)
    {
        buf.addCharSequence((char*)&x, sizeof(x));
        return buf;
    }

    inline NetworkByteBuffer & operator <<(NetworkByteBuffer & buf, const decimal32 & x)
    {
        return serialize (buf, x);
    }

    inline NativeByteBuffer & operator <<(NativeByteBuffer & buf, const decimal32 & x)
    {
        buf.addCharSequence((char*)&x, sizeof(x));
        return buf;
    }

    inline NetworkByteBuffer & operator >>(NetworkByteBuffer & buf, decimal32 & x)
    {
        return deserialize(buf, x);
    }

    inline NativeByteBuffer & operator >>(NativeByteBuffer & buf, decimal32 & x)
    {
        memcpy((void *)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
        return buf;
    }

    template <class BufferType>
    inline ByteBuffer<BufferType> & operator >>(ByteBuffer<BufferType> & buf, decimal32 & x)
    {
        memcpy((void *)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
        return buf;
    }

    template <>
    inline ByteBuffer<NetworkByteBuffer> & operator <<(ByteBuffer<NetworkByteBuffer> & buf, const decimal32 & x)
    {
        return serialize (buf, x);
    }

    template <>
    inline ByteBuffer<NativeByteBuffer> & operator <<(ByteBuffer<NativeByteBuffer> & buf, const decimal32 & x)
    {
        buf.addCharSequence((char*)&x, sizeof(x));
        return buf;
    }

    template <>
    inline ByteBuffer<NetworkByteBuffer> & operator >>(ByteBuffer<NetworkByteBuffer> & buf, decimal32 & x)
    {
        return deserialize(buf, x);
    }

    template <>
    inline ByteBuffer<NativeByteBuffer> & operator >>(ByteBuffer<NativeByteBuffer> & buf, decimal32 & x)
    {
        memcpy((void *)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
        return buf;
    }

    inline NetworkByteBuffer & operator <<(NetworkByteBuffer & buf, const decimal64 & x)
    {
        return serialize (buf, x);
    }

    inline NativeByteBuffer & operator <<(NativeByteBuffer & buf, const decimal64 & x)
    {
        buf.addCharSequence((char*)&x, sizeof(x));
        return buf;
    }

    inline NativeByteBuffer & operator >>(NativeByteBuffer & buf, decimal64 & x)
    {
        memcpy((void *)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
        return buf;
    }

    inline NetworkByteBuffer & operator >>(NetworkByteBuffer & buf, decimal64 & x)
    {
        return deserialize(buf, x);
    }

    template <class BufferType>
    inline ByteBuffer<BufferType> & operator <<(ByteBuffer<BufferType> & buf, const decimal64 & x)
    {
        buf.addCharSequence((char*)&x, sizeof(x));
        return buf;
    }

    template <class BufferType>
    inline ByteBuffer<BufferType> & operator >>(ByteBuffer<BufferType> & buf, decimal64 & x)
    {
        memcpy((void *)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
        return buf;
    }

    template <>
    inline ByteBuffer<NetworkByteBuffer> & operator <<(ByteBuffer<NetworkByteBuffer> & buf, const decimal64 & x)
    {
        return serialize (buf, x);
    }

    template <>
    inline ByteBuffer<NativeByteBuffer> & operator <<(ByteBuffer<NativeByteBuffer> & buf, const decimal64 & x)
    {
        buf.addCharSequence((char*)&x, sizeof(x));
        return buf;
    }

    template <>
    inline ByteBuffer<NetworkByteBuffer> & operator >>(ByteBuffer<NetworkByteBuffer> & buf, decimal64 & x)
    {
        return deserialize(buf, x);
    }

    template <>
    inline ByteBuffer<NativeByteBuffer> & operator >>(ByteBuffer<NativeByteBuffer> & buf, decimal64 & x)
    {
        memcpy((void *)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
        return buf;
    }

    inline NetworkByteBuffer & operator <<(NetworkByteBuffer & buf, const decimal128 & x)
    {
        return serialize (buf, x);
    }

    inline NativeByteBuffer & operator <<(NativeByteBuffer & buf, const decimal128 & x)
    {
        buf.addCharSequence((char*)&x, sizeof(x));
        return buf;
    }

    inline NativeByteBuffer & operator >>(NativeByteBuffer & buf, decimal128 & x)
    {
        memcpy((void *)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
        return buf;
    }

    inline NetworkByteBuffer & operator >>(NetworkByteBuffer & buf, decimal128 & x)
    {
        return deserialize(buf, x);
    }

    template <class BufferType>
    inline ByteBuffer<BufferType> & operator <<(ByteBuffer<BufferType> & buf, const decimal128 & x)
    {
        buf.addCharSequence((char*)&x, sizeof(x));
        return buf;
    }

    template <class BufferType>
    inline ByteBuffer<BufferType> & operator >>(ByteBuffer<BufferType> & buf, decimal128 & x)
    {
        memcpy((void *)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
        return buf;
    }

    template <>
    inline ByteBuffer<NetworkByteBuffer> & operator <<(ByteBuffer<NetworkByteBuffer> & buf, const decimal128 & x)
    {
        return serialize (buf, x);
    }

    template <>
    inline ByteBuffer<NativeByteBuffer> & operator <<(ByteBuffer<NativeByteBuffer> & buf, const decimal128 & x)
    {
        buf.addCharSequence((char*)&x, sizeof(x));
        return buf;
    }

    template <>
    inline ByteBuffer<NativeByteBuffer> & operator >>(ByteBuffer<NativeByteBuffer> & buf, decimal128 & x)
    {
        memcpy((void *)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
        return buf;
    }

    template <>
    inline ByteBuffer<NetworkByteBuffer> & operator >>(ByteBuffer<NetworkByteBuffer> & buf, decimal128 & x)
    {
        return deserialize(buf, x);
    }

    inline void deserializeWithSuffix(std::istream & s, decimal32 & v)
    {
      parseWithSuffixes(s,v,'d','w');
    }

    inline void deserializeWithSuffix(std::istream & s, decimal64 & v)
    {
      parseWithSuffixes(s,v,'d','l');
    }

    inline void deserializeWithSuffix(std::istream & s, decimal128 & v)
    {
      parseWithSuffixes(s,v,'d','q');
    }

    inline void deserializeWithNanAndInfs(std::istream & s, decimal32 & v, bool withSuffix = false)
    {
      parseWithNanAndInfs(s,v);
      if (withSuffix) {
          parseSuffixes(s, 'd', 'w');
      }
    }

    inline void deserializeWithNanAndInfs(std::istream & s, decimal64 & v, bool withSuffix = false)
    {
      parseWithNanAndInfs(s,v);
      if (withSuffix) {
          parseSuffixes(s, 'd', 'l');
      }
    }

    inline void deserializeWithNanAndInfs(std::istream & s, decimal128 & v, bool withSuffix = false)
    {
      parseWithNanAndInfs(s,v);
      if (withSuffix) {
          parseSuffixes(s, 'd', 'q');
      }
    }

};

#ifndef DOXYGEN_SKIP_FOR_USERS
// Hashability requirement
namespace std {
    namespace tr1 {
        template <>
        struct hash<SPL::decimal32>
        {
            inline size_t operator ()(const SPL::decimal32 & s) const
            {
                uint32_t x;
                memcpy (&x, &s, sizeof x);
                hash<uint32_t> hs;
                return hs(x);
            }
        };
        template <>
        struct hash<SPL::decimal64>
        {
            inline size_t operator ()(const SPL::decimal64 & s) const
            {
                uint64_t x;
                memcpy (&x, &s, sizeof x);
                hash<uint64_t> hs;
                return hs(x);
            }
        };
        template <>
        struct hash<SPL::decimal128>
        {
            inline size_t operator ()(const SPL::decimal128 & s) const
            {
                uint64_t x[2];
                memcpy (&x, &s, sizeof x);
                size_t r = 17;
                hash<uint64_t> hs;
                r = 37 * r + hs(x[0]);
                r = 37 * r + hs(x[1]);
                return r;
            }
        };
    }
}

#undef dec_isinf
#endif /* DOXYGEN_SKIP_FOR_USERS */

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_DECIMAL_H */
