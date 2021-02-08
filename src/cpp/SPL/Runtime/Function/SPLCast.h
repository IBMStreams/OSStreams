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

#ifndef SPL_RUNTIME_TYPE_SPL_CAST_H
#define SPL_RUNTIME_TYPE_SPL_CAST_H

#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>
#include <sstream>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/LogTraceMessage.h>

namespace SPL {
template<class TargetType, class SourceType>
struct spl_cast
{
    /// Template function used to simplify spl casts
    /// @param src Input source
    /// @return src casted to TargetType
    static inline TargetType cast(const SourceType& src) { return static_cast<TargetType>(src); }
};

template<class TargetType, class SourceType>
struct spl_cast<SPL::optional<TargetType>, SPL::optional<SourceType> >
{
    /// Template function used to simplify spl casts
    /// @param src Input source
    /// @return src casted to TargetType
    static inline SPL::optional<TargetType> cast(const SPL::optional<SourceType>& src)
    {
        if (src.isPresent()) {
            return SPL::optional<TargetType>(spl_cast<TargetType, SourceType>::cast(src.value()));
        }
        return SPL::optional<TargetType>();
    }
};

template<class T>
struct spl_cast<SPL::optional<T>, SPL::optional<T> >
{
    static inline SPL::optional<T> cast(const SPL::optional<T>& src) { return src; }
};

template<class T>
struct spl_cast<T, T>
{
    static inline T cast(const T& src) { return src; }
};

template<class T>
struct spl_cast<SPL::optional<T>, T>
{
    static inline SPL::optional<T> cast(const T& src) { return SPL::optional<T>(src); }
};

template<class TargetType, class SourceType>
struct spl_cast<SPL::optional<TargetType>, SourceType>
{
    static inline SPL::optional<TargetType> cast(const SourceType& src)
    {
        return SPL::optional<TargetType>(spl_cast<TargetType, SourceType>::cast(src));
    }
};

/*
     * Lets handle blobs
     */
template<>
struct spl_cast<SPL::list<SPL::uint8>, SPL::blob>
{
    /// Convert blob to list<uint8>
    /// @param src Input source
    /// @return src casted to list<uint8>
    static inline SPL::list<SPL::uint8> cast(const SPL::blob& src)
    {
        SPL::list<SPL::uint8> t;
        uint32_t bSize = src.getSize();
        for (uint32_t i = 0; i < bSize; i++) {
            t.push_back(static_cast<SPL::uint8>(src[i]));
        }
        return t;
    }
};

template<int32_t mSize>
struct spl_cast<SPL::blist<SPL::uint8, mSize>, SPL::blob>
{
    /// Convert blob to blist<uint8>[N]
    /// @param src Input source
    /// @return src casted to blist<uint8>[N]
    static inline SPL::blist<SPL::uint8, mSize> cast(const SPL::blob& src)
    {
        SPL::blist<SPL::uint8, mSize> t;
        uint32_t bSize = src.getSize();
        for (uint32_t i = 0; i < bSize; i++) {
            t.push_back(static_cast<SPL::uint8>(src[i]));
        }
        return t;
    }
};

/*
     * Special handling from list to complexXX
     */
// extern const std::string badComplexCast(const SPL::List&);
extern const FormattableMessage badComplexCastMsg(const SPL::List&);
#define LIST_TO_COMPLEX(COMPLEX)                                                                   \
    template<class T>                                                                              \
    struct spl_cast<SPL::COMPLEX, SPL::list<T> >                                                   \
    {                                                                                              \
        /* Convert list<T> to COMPLEX */                                                           \
        /* @param src Input source */                                                              \
        /* @return src casted to COMPLEX */                                                        \
        static inline SPL::COMPLEX cast(const SPL::list<T>& src)                                   \
        {                                                                                          \
            if (src.size() != 2)                                                                   \
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, badComplexCastMsg(src),       \
                                    SPL_FUNC_DBG);                                                 \
            return SPL::COMPLEX(src[0], src[1]);                                                   \
        }                                                                                          \
    };                                                                                             \
    template<class T, int32_t N>                                                                   \
    struct spl_cast<SPL::COMPLEX, SPL::blist<T, N> >                                               \
    {                                                                                              \
        /* Convert list<T> to COMPLEX */                                                           \
        /* @param src Input source */                                                              \
        /* @return src casted to COMPLEX */                                                        \
        static inline SPL::COMPLEX cast(const SPL::blist<T, N>& src)                               \
        {                                                                                          \
            if (src.size() != 2)                                                                   \
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, badComplexCastMsg(src),       \
                                    SPL_FUNC_DBG);                                                 \
            return SPL::COMPLEX(src[0], src[1]);                                                   \
        }                                                                                          \
    };
LIST_TO_COMPLEX(complex32)
LIST_TO_COMPLEX(complex64)
#undef LIST_TO_COMPLEX

/*
     * That handled most cases.  We need special handling for decimalXX,
     * since there is no default cast support
     */

template<>
struct spl_cast<SPL::float32, SPL::decimal32>
{
    /// Convert decimal32 to float32
    /// @param src Input source
    /// @return src casted to float32
    static inline SPL::float32 cast(const SPL::decimal32& src)
    {
        return ibm::decimal::decimal32_to_long_double(src);
    }
};

template<>
struct spl_cast<SPL::float32, SPL::decimal64>
{
    /// Convert decimal64 to float32
    /// @param src Input source
    /// @return src casted to float64
    static inline SPL::float32 cast(const decimal64& src)
    {
        return ibm::decimal::decimal64_to_long_double(src);
    }
};

template<>
struct spl_cast<SPL::float32, SPL::decimal128>
{
    /// Convert decimal128 to float32
    /// @param src Input source
    /// @return src casted to float128
    static inline SPL::float32 cast(const SPL::decimal128& src)
    {
        return ibm::decimal::decimal128_to_long_double(src);
    }
};

template<>
struct spl_cast<SPL::float64, SPL::decimal32>
{
    /// Convert decimal32 to float64
    /// @param src Input source
    /// @return src casted to float64
    static inline SPL::float64 cast(const SPL::decimal32& src)
    {
        return ibm::decimal::decimal32_to_long_double(src);
    }
};

template<>
struct spl_cast<SPL::float64, SPL::decimal64>
{
    /// Convert decimal64 to float64
    /// @param src Input source
    /// @return src casted to float64
    static inline SPL::float64 cast(const SPL::decimal64& src)
    {
        return ibm::decimal::decimal64_to_long_double(src);
    }
};

template<>
struct spl_cast<SPL::float64, SPL::decimal128>
{
    /// Convert decimal128 to float64
    /// @param src Input source
    /// @return src casted to float64
    static inline SPL::float64 cast(const SPL::decimal128& src)
    {
        return ibm::decimal::decimal128_to_long_double(src);
    }
};

// Inter decimal conversions

template<>
struct spl_cast<SPL::decimal32, SPL::decimal64>
{
    /// Convert decimal64 to decimal32
    /// @param src Input source
    /// @return src casted to decimal64
    static inline SPL::decimal32 cast(const decimal64& src) { return SPL::decimal32(src); }
};

template<>
struct spl_cast<SPL::decimal32, SPL::decimal128>
{
    /// Convert decimal128 to decimal32
    /// @param src Input source
    /// @return src casted to decimal128
    static inline SPL::decimal32 cast(const SPL::decimal128& src) { return SPL::decimal32(src); }
};

template<>
struct spl_cast<SPL::decimal64, SPL::decimal32>
{
    /// Convert decimal32 to decimal64
    /// @param src Input source
    /// @return src casted to decimal64
    static inline SPL::decimal64 cast(const SPL::decimal32& src) { return SPL::decimal64(src); }
};

#if 0
    template <>
    struct spl_cast<SPL::optional<SPL::decimal64>, SPL::decimal32> {
        /// Convert decimal32 to decimal64
        /// @param src Input source
        /// @return src casted to decimal64
        static inline SPL::optional<SPL::decimal64> cast(const SPL::decimal32 & src)
        {
            return SPL::optional<SPL::decimal64>(src);
        }
    };
#endif

template<>
struct spl_cast<SPL::decimal64, SPL::decimal128>
{
    /// Convert decimal128 to decimal64
    /// @param src Input source
    /// @return src casted to decimal64
    static inline SPL::decimal64 cast(const SPL::decimal128& src) { return SPL::decimal64(src); }
};

template<>
struct spl_cast<SPL::decimal128, SPL::decimal32>
{
    /// Convert decimal32 to decimal128
    /// @param src Input source
    /// @return src casted to decimal128
    static inline SPL::decimal128 cast(const SPL::decimal32& src) { return SPL::decimal128(src); }
};

template<>
struct spl_cast<SPL::decimal128, SPL::decimal64>
{
    /// Convert decimal64 to decimal128
    /// @param src Input source
    /// @return src casted to decimal128
    static inline SPL::decimal128 cast(const SPL::decimal64& src) { return SPL::decimal128(src); }
};

template<class TargetType>
struct spl_cast<TargetType, SPL::decimal32>
{
    /// Convert decimal32 to any integral SPL type
    /// @param src Input source
    /// @return src casted to TargetType
    static inline TargetType cast(const SPL::decimal32& src)
    {
        return static_cast<TargetType>(static_cast<long long>(src));
    }
};

template<>
struct spl_cast<SPL::decimal32, SPL::decimal32>
{
    /// Convert decimal32 to decimal32
    /// @param src Input source
    /// @return src casted to decimal32
    static inline SPL::decimal32 cast(const SPL::decimal32& src) { return src; }
};

template<>
struct spl_cast<SPL::optional<SPL::decimal32>, SPL::decimal32>
{
    /// Convert decimal32 to optional<decimal32>
    /// @param src Input source
    /// @return src casted to optional<decimal32>
    static inline SPL::optional<SPL::decimal32> cast(const SPL::decimal32& src)
    {
        return SPL::optional<SPL::decimal32>(src);
    }
};

template<class TargetType>
struct spl_cast<TargetType, SPL::decimal64>
{
    /// Convert decimal64 to any integral SPL type
    /// @param src Input source
    /// @return src casted to TargetType
    static inline TargetType cast(const SPL::decimal64& src)
    {
        return static_cast<TargetType>(static_cast<long long>(src));
    }
};

template<>
struct spl_cast<SPL::decimal64, SPL::decimal64>
{
    /// Convert decimal64 to decimal64
    /// @param src Input source
    /// @return src casted to decimal64
    static inline SPL::decimal64 cast(const SPL::decimal64& src) { return src; }
};

template<>
struct spl_cast<SPL::optional<SPL::decimal64>, SPL::decimal64>
{
    /// Convert decimal64 to optional<decimal64>
    /// @param src Input source
    /// @return src casted to optional<decimal64>
    static inline SPL::optional<SPL::decimal64> cast(const SPL::decimal64& src)
    {
        return SPL::optional<SPL::decimal64>(src);
    }
};

template<class TargetType>
struct spl_cast<TargetType, SPL::decimal128>
{
    /// Convert decimal128 to any integral SPL type
    /// @param src Input source
    /// @return src casted to TargetType
    static inline TargetType cast(const SPL::decimal128& src)
    {
        return static_cast<TargetType>(static_cast<long long>(src));
    }
};

template<>
struct spl_cast<SPL::decimal128, SPL::decimal128>
{
    /// Convert decimal128 to decimal128
    /// @param src Input source
    /// @return src casted to decimal128
    static inline SPL::decimal128 cast(const SPL::decimal128& src) { return src; }
};

template<>
struct spl_cast<SPL::optional<SPL::decimal128>, SPL::decimal128>
{
    /// Convert decimal128 to optional<decimal128>
    /// @param src Input source
    /// @return src casted to optional<decimal128>
    static inline SPL::optional<SPL::decimal128> cast(const SPL::decimal128& src)
    {
        return SPL::optional<SPL::decimal128>(src);
    }
};

/*
    ** We also need handling for timestamp->float64
    */
template<>
struct spl_cast<SPL::float64, SPL::timestamp>
{
    /// Convert timestamp to float64
    /// @param src Input timestamp
    /// @return src casted to float64
    static inline SPL::float64 cast(const SPL::timestamp& src)
    {
        return static_cast<SPL::float64>(src.getSeconds()) +
               static_cast<SPL::float64>(src.getNanoseconds() / 1.0e9);
    }
};

/*
     ** Special handling for enums
     */

template<>
struct spl_cast<SPL::rstring, SPL::Enum>
{
    /// Convert enum to rstring
    /// @param src Input enum
    /// @return string representation of enum
    static inline SPL::rstring cast(const SPL::Enum& src) { return src.getValue(); }
};

template<>
struct spl_cast<SPL::ustring, SPL::Enum>
{
    /// Convert enum to ustring
    /// @param src Input enum
    /// @return string representation of enum
    static inline SPL::ustring cast(const SPL::Enum& src)
    {
        return SPL::ustring::fromUTF8(src.getValue());
    }
};

template<int32_t msize>
struct spl_cast<SPL::bstring<msize>, SPL::Enum>
{
    /// Convert enum to bounded string
    /// @param src Input enum
    /// @return string representation of enum
    static inline SPL::bstring<msize> cast(const SPL::Enum& src) { return src.getValue(); }
};

template<class SourceType>
struct spl_cast<SPL::rstring, SourceType>
{
    /// Convert XX to rstring
    /// @param src Input source
    /// @return src casted to TargetType
    static inline SPL::rstring cast(const SourceType& src)
    {
        std::ostringstream s;
        s.imbue(std::locale::classic());
        SPL::serializeWithPrecision(s, src);
        if (!s) {
            SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                                SPL_APPLICATION_RUNTIME_CONVERSION_TO_STRING_FAILED(
                                  SPL::RuntimeUtility::demangleTypeName<SourceType>()),
                                SPL_FUNC_DBG);
        }
        return s.str();
    }
};
template<int32_t size>
struct spl_cast<SPL::rstring, SPL::bstring<size> >
{
    static inline SPL::rstring cast(const SPL::bstring<size>& src) { return src; }
};
template<class SourceType, int32_t size>
struct spl_cast<SPL::bstring<size>, SourceType>
{
    static inline SPL::bstring<size> cast(const SourceType& src)
    {
        std::ostringstream s;
        s.imbue(std::locale::classic());
        SPL::serializeWithPrecision(s, src);
        if (!s) {
            SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                                SPL_APPLICATION_RUNTIME_CONVERSION_TO_STRING_FAILED(
                                  SPL::RuntimeUtility::demangleTypeName<SourceType>()),
                                SPL_FUNC_DBG);
        }
        return SPL::bstring<size>(s.str());
    }
};

template<int32_t size>
struct spl_cast<SPL::bstring<size>, SPL::rstring>
{
    static inline SPL::bstring<size> cast(const SPL::rstring& src) { return src; }
};

template<int32_t size>
struct spl_cast<SPL::bstring<size>, SPL::bstring<size> >
{
    static inline SPL::bstring<size> cast(const SPL::bstring<size>& src) { return src; }
};

template<int32_t size>
struct spl_cast<SPL::optional<SPL::bstring<size> >, SPL::bstring<size> >
{
    static inline SPL::optional<SPL::bstring<size> > cast(const SPL::bstring<size>& src)
    {
        return SPL::optional<SPL::bstring<size> >(src);
    }
};

template<>
struct spl_cast<SPL::rstring, SPL::xml>
{
    static inline SPL::rstring cast(const SPL::xml& src) { return src.getValue(); }
};

template<class TargetType>
struct spl_cast<TargetType, SPL::xml>
{
    static inline TargetType cast(const SPL::xml& src)
    {
        std::stringstream sstr(src.getValue());
        sstr.imbue(std::locale::classic());
        TargetType result;
        result.fromXML(sstr);
        return result;
    }
};

inline void skipWhiteSpace(std::istream& str)
{
    for (;;) {
        if (str.eof()) {
            return;
        }
        int c = str.peek();
        if (c == '\t' || c == ' ' || c == '\r' || c == '\n') {
            (void)str.get();
        } else {
            break;
        }
    }
}

template<class TargetType>
struct spl_cast<TargetType, SPL::rstring>
{
    /// Convert rstring to XX
    /// @param src Input source (rstring)
    /// @return src casted to TargetType
    static inline TargetType cast(const SPL::rstring& src)
    {
        std::stringstream sstr(src);
        sstr.imbue(std::locale::classic());
        TargetType result;
        sstr >> result;
        skipWhiteSpace(sstr);
        if (!sstr || sstr.peek() != EOF) {
            SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                                SPL_APPLICATION_RUNTIME_CONVERSION_FROM_STRING_CPP_FAILED(
                                  src, SPL::RuntimeUtility::demangleTypeName<TargetType>()),
                                SPL_FUNC_DBG);
        }
        return result;
    }
};

template<>
struct spl_cast<SPL::rstring, SPL::rstring>
{
    static inline SPL::rstring cast(const SPL::rstring& src) { return src; }
};

template<>
struct spl_cast<SPL::optional<SPL::rstring>, SPL::rstring>
{
    static inline SPL::optional<SPL::rstring> cast(const SPL::rstring& src)
    {
        return SPL::optional<SPL::rstring>(src);
    }
};

template<>
struct spl_cast<SPL::ustring, SPL::ustring>
{
    static inline SPL::ustring cast(const SPL::ustring& src) { return src; }
};

template<>
struct spl_cast<SPL::optional<SPL::ustring>, SPL::ustring>
{
    static inline SPL::optional<SPL::ustring> cast(const SPL::ustring& src)
    {
        return SPL::optional<SPL::ustring>(src);
    }
};

template<int32_t size>
struct spl_cast<SPL::bstring<size>, SPL::ustring>
{
    static inline SPL::bstring<size> cast(const SPL::ustring& src)
    {
        return SPL::ustring::toUTF8(src);
    }
};

template<>
struct spl_cast<SPL::rstring, SPL::ustring>
{
    static inline SPL::rstring cast(const SPL::ustring& src) { return SPL::ustring::toUTF8(src); }
};

template<>
struct spl_cast<SPL::ustring, SPL::rstring>
{
    static inline SPL::ustring cast(const SPL::rstring& src) { return SPL::ustring::fromUTF8(src); }
};

template<>
struct spl_cast<SPL::optional<SPL::ustring>, SPL::rstring>
{
    static inline SPL::optional<SPL::ustring> cast(const SPL::rstring& src)
    {
        return SPL::optional<SPL::ustring>(SPL::ustring::fromUTF8(src));
    }
};

template<int32_t size>
struct spl_cast<SPL::ustring, SPL::bstring<size> >
{
    static inline SPL::ustring cast(const SPL::bstring<size>& src)
    {
        return SPL::ustring::fromUTF8(src);
    }
};

template<class TargetType, int32_t size>
struct spl_cast<TargetType, SPL::bstring<size> >
{
    static inline TargetType cast(const SPL::bstring<size>& src)
    {
        std::stringstream sstr(src.getCString());
        sstr.imbue(std::locale::classic());
        TargetType result;
        sstr >> result;
        skipWhiteSpace(sstr);
        if (!sstr || sstr.peek() != EOF) {
            SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                                SPL_APPLICATION_RUNTIME_CONVERSION_FROM_STRING_CPP_FAILED(
                                  src, SPL::RuntimeUtility::demangleTypeName<TargetType>()),
                                SPL_FUNC_DBG);
        }
        return result;
    }
};

// Special cases due to template ambiguities
#define toString(T)                                                                                \
    template<>                                                                                     \
    struct spl_cast<SPL::rstring, T>                                                               \
    {                                                                                              \
        static inline SPL::rstring cast(const T& src)                                              \
        {                                                                                          \
            std::ostringstream s;                                                                  \
            s.imbue(std::locale::classic());                                                       \
            SPL::serializeWithPrecision(s, src);                                                   \
            if (!s)                                                                                \
                SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,                       \
                                    SPL_APPLICATION_RUNTIME_CONVERSION_TO_STRING_FAILED(           \
                                      SPL::RuntimeUtility::demangleTypeName<T>()),                 \
                                    SPL_FUNC_DBG);                                                 \
            return s.str();                                                                        \
        }                                                                                          \
    };                                                                                             \
    template<>                                                                                     \
    struct spl_cast<SPL::ustring, T>                                                               \
    {                                                                                              \
        static inline SPL::ustring cast(const T& src)                                              \
        {                                                                                          \
            return SPL::ustring::fromUTF8(SPL::spl_cast<SPL::rstring, T>::cast(src));              \
        }                                                                                          \
    };

toString(SPL::decimal32) toString(SPL::decimal64) toString(SPL::decimal128)
#undef toString

  template<class SourceType>
  struct spl_cast<SPL::ustring, SourceType>
{
    /// Convert XX to ustring
    /// @param src Input source
    /// @return src as a ustring
    static inline SPL::ustring cast(const SourceType& src)
    {
        return ::SPL::ustring::fromUTF8(spl_cast<SPL::rstring, SourceType>::cast(src));
    }
};

template<class TargetType>
struct spl_cast<TargetType, SPL::ustring>
{
    /// Convert ustring to XX
    /// @param src Input ustring
    /// @return src casted to TargetType
    static inline TargetType cast(const SPL::ustring& src)
    {
        return spl_cast<TargetType, SPL::rstring>::cast(::SPL::ustring::toUTF8(src));
    }
};

template<class TargetElementType, class SourceElementType>
struct spl_cast<SPL::list<TargetElementType>, SPL::list<SourceElementType> >
{
    /// Convert a list<xx> to a list<yy>
    /// @param src Input source
    /// @return src casted to TargetType
    static inline SPL::list<TargetElementType> cast(const SPL::list<SourceElementType>& src)
    {
        SPL::list<TargetElementType> result;
        typename SPL::list<SourceElementType>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.push_back(spl_cast<TargetElementType, SourceElementType>::cast(*it));
        }
        return result;
    }
};
template<class TargetElementType, class SourceElementType, int32_t srcSize>
struct spl_cast<SPL::list<TargetElementType>, SPL::blist<SourceElementType, srcSize> >
{
    static inline SPL::list<TargetElementType> cast(
      const SPL::blist<SourceElementType, srcSize>& src)
    {
        SPL::list<TargetElementType> result;
        typename SPL::blist<SourceElementType, srcSize>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.push_back(spl_cast<TargetElementType, SourceElementType>::cast(*it));
        }
        return result;
    }
};
template<class TargetElementType, int32_t trgSize, class SourceElementType>
struct spl_cast<SPL::blist<TargetElementType, trgSize>, SPL::list<SourceElementType> >
{
    static inline SPL::blist<TargetElementType, trgSize> cast(
      const SPL::list<SourceElementType>& src)
    {
        SPL::blist<TargetElementType, trgSize> result;
        typename SPL::list<SourceElementType>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.push_back(spl_cast<TargetElementType, SourceElementType>::cast(*it));
        }
        return result;
    }
};
template<class TargetElementType, int32_t trgSize, class SourceElementType, int32_t srcSize>
struct spl_cast<SPL::blist<TargetElementType, trgSize>, SPL::blist<SourceElementType, srcSize> >
{
    static inline SPL::blist<TargetElementType, trgSize> cast(
      const SPL::blist<SourceElementType, srcSize>& src)
    {
        SPL::blist<TargetElementType, trgSize> result;
        typename SPL::blist<SourceElementType, srcSize>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.push_back(spl_cast<TargetElementType, SourceElementType>::cast(*it));
        }
        return result;
    }
};

template<class TargetElementType, class SourceElementType>
struct spl_cast<SPL::set<TargetElementType>, SPL::set<SourceElementType> >
{
    /// Convert a set<xx> to a set<yy>
    /// @param src Input source
    /// @return src casted to TargetType
    static inline SPL::set<TargetElementType> cast(const SPL::set<SourceElementType>& src)
    {
        SPL::set<TargetElementType> result;
        typename SPL::set<SourceElementType>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.insert(spl_cast<TargetElementType, SourceElementType>::cast(*it));
        }
        return result;
    }
};

template<class TargetElementType, class SourceElementType, int32_t srcSize>
struct spl_cast<SPL::set<TargetElementType>, SPL::bset<SourceElementType, srcSize> >
{
    static inline SPL::set<TargetElementType> cast(const SPL::bset<SourceElementType, srcSize>& src)
    {
        SPL::set<TargetElementType> result;
        typename SPL::bset<SourceElementType, srcSize>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.insert(spl_cast<TargetElementType, SourceElementType>::cast(*it));
        }
        return result;
    }
};
template<class TargetElementType, int32_t trgSize, class SourceElementType>
struct spl_cast<SPL::bset<TargetElementType, trgSize>, SPL::set<SourceElementType> >
{
    static inline SPL::bset<TargetElementType, trgSize> cast(const SPL::set<SourceElementType>& src)
    {
        SPL::bset<TargetElementType, trgSize> result;
        typename SPL::set<SourceElementType>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.insert(spl_cast<TargetElementType, SourceElementType>::cast(*it));
        }
        return result;
    }
};
template<class TargetElementType, int32_t trgSize, class SourceElementType, int32_t srcSize>
struct spl_cast<SPL::bset<TargetElementType, trgSize>, SPL::bset<SourceElementType, srcSize> >
{
    static inline SPL::bset<TargetElementType, trgSize> cast(
      const SPL::bset<SourceElementType, srcSize>& src)
    {
        SPL::bset<TargetElementType, trgSize> result;
        typename SPL::bset<SourceElementType, srcSize>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.insert(spl_cast<TargetElementType, SourceElementType>::cast(*it));
        }
        return result;
    }
};

template<class TargetKey, class TargetValue, class SourceKey, class SourceValue>
struct spl_cast<SPL::map<TargetKey, TargetValue>, SPL::map<SourceKey, SourceValue> >
{
    /// Convert a map<xx> to a map<yy>
    /// @param src Input source
    /// @return src casted to TargetType
    static inline SPL::map<TargetKey, TargetValue> cast(const SPL::map<SourceKey, SourceValue>& src)
    {
        SPL::map<TargetKey, TargetValue> result;
        typename SPL::map<SourceKey, SourceValue>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.insert(std::make_pair(spl_cast<TargetKey, SourceKey>::cast(it->first),
                                         spl_cast<TargetValue, SourceValue>::cast(it->second)));
        }
        return result;
    }
};
template<class TargetKey, class TargetValue, class SourceKey, class SourceValue, int32_t srcSize>
struct spl_cast<SPL::map<TargetKey, TargetValue>, SPL::bmap<SourceKey, SourceValue, srcSize> >
{
    static inline SPL::map<TargetKey, TargetValue> cast(
      const SPL::bmap<SourceKey, SourceValue, srcSize>& src)
    {
        SPL::map<TargetKey, TargetValue> result;
        typename SPL::bmap<SourceKey, SourceValue, srcSize>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.insert(std::make_pair(spl_cast<TargetKey, SourceKey>::cast(it->first),
                                         spl_cast<TargetValue, SourceValue>::cast(it->second)));
        }
        return result;
    }
};
template<class TargetKey, class TargetValue, int32_t trgSize, class SourceKey, class SourceValue>
struct spl_cast<SPL::bmap<TargetKey, TargetValue, trgSize>, SPL::map<SourceKey, SourceValue> >
{
    static inline SPL::bmap<TargetKey, TargetValue, trgSize> cast(
      const SPL::map<SourceKey, SourceValue>& src)
    {
        SPL::bmap<TargetKey, TargetValue, trgSize> result;
        typename SPL::map<SourceKey, SourceValue>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.insert(std::make_pair(spl_cast<TargetKey, SourceKey>::cast(it->first),
                                         spl_cast<TargetValue, SourceValue>::cast(it->second)));
        }
        return result;
    }
};
template<class TargetKey,
         class TargetValue,
         int32_t trgSize,
         class SourceKey,
         class SourceValue,
         int32_t srcSize>
struct spl_cast<SPL::bmap<TargetKey, TargetValue, trgSize>,
                SPL::bmap<SourceKey, SourceValue, srcSize> >
{
    static inline SPL::bmap<TargetKey, TargetValue, trgSize> cast(
      const SPL::bmap<SourceKey, SourceValue, srcSize>& src)
    {
        SPL::bmap<TargetKey, TargetValue, trgSize> result;
        typename SPL::bmap<SourceKey, SourceValue, srcSize>::const_iterator it;
        for (it = src.begin(); it != src.end(); it++) {
            result.insert(std::make_pair(spl_cast<TargetKey, SourceKey>::cast(it->first),
                                         spl_cast<TargetValue, SourceValue>::cast(it->second)));
        }
        return result;
    }
};
}

#endif // DOXYGEN_SKIP_FOR_USERS
#endif /* SPL_RUNTIME_TYPE_SPL_CAST_H */
