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

#ifndef SPL_RUNTIME_TYPE_MAPPED_OPERATORS_H
#define SPL_RUNTIME_TYPE_MAPPED_OPERATORS_H

/*!
 * \file MappedOperators.h \brief Definitions of all operator overloads for
 * mapped operations on %SPL types.
 */

#include <SPL/Runtime/Type/List.h>
#include <SPL/Runtime/Type/BList.h>
#include <SPL/Runtime/Type/Map.h>
#include <SPL/Runtime/Type/BMap.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>

#include <algorithm>

// .op functionality
#define DOT_OP(FCN,OP,MSG,RET)                                          \
    template<class T>                                                   \
    inline SPL::list<RET> FCN(const SPL::list<T>& lhs,                  \
                              const SPL::list<T>& rhs)                  \
    {                                                                   \
        uint32_t num = rhs.size();                                      \
        if (num != lhs.size())                                          \
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,     \
                  SPL_APPLICATION_RUNTIME_DOT_OP_ERROR("." #MSG),       \
                  SPL_FUNC_DBG);                                        \
        SPL::list<RET> result;                                          \
        result.reserve (num);                                           \
        std::transform(lhs.begin(), lhs.end(), rhs.begin(),             \
                       std::back_inserter(result), OP<T>());            \
        return result;                                                  \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline SPL::blist<RET,N> FCN(const SPL::blist<T,N>& lhs,            \
                                 const SPL::blist<T,N>& rhs)            \
    {                                                                   \
        SPL::blist<RET,N> result;                                       \
        std::transform(lhs.begin(), lhs.end(), rhs.begin(),             \
                       std::back_inserter(result), OP<T>());            \
        return result;                                                  \
    }                                                                   \
                                                                        \
    template<class T>                                                   \
    inline SPL::list<RET> FCN(const T& lhs,                             \
                              const SPL::list<T>& rhs)                  \
    {                                                                   \
        uint32_t num = rhs.size();                                      \
        SPL::list<RET> result;                                          \
        result.reserve (num);                                           \
        for (uint32_t i = 0; i < num; i++)                              \
            result.push_back (lhs MSG rhs[i]);                          \
        return result;                                                  \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline SPL::blist<RET,N> FCN(const T& lhs,                          \
                                 const SPL::blist<T,N>& rhs)            \
    {                                                                   \
        SPL::blist<RET,N> result;                                       \
        uint32_t num = rhs.size();                                      \
        for (uint32_t i = 0; i < num; i++)                              \
            result.push_back (lhs MSG rhs[i]);                          \
        return result;                                                  \
    }                                                                   \
                                                                        \
    template<class T>                                                   \
    inline SPL::list<RET> FCN(const SPL::list<T>& lhs,                  \
                              const T& rhs)                             \
    {                                                                   \
        uint32_t num = lhs.size();                                      \
        SPL::list<RET> result;                                          \
        result.reserve (num);                                           \
        for (uint32_t i = 0; i < num; i++)                              \
            result.push_back (lhs[i] MSG rhs);                          \
        return result;                                                  \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline SPL::blist<RET,N> FCN(const SPL::blist<T,N>& lhs,            \
                                 const T& rhs)                          \
    {                                                                   \
        SPL::blist<RET,N> result;                                       \
        uint32_t num = lhs.size();                                      \
        for (uint32_t i = 0; i < num; i++)                              \
            result.push_back (lhs[i] MSG rhs);                          \
        return result;                                                  \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline SPL::map<K,RET> FCN(const SPL::map<K,T>& lhs,                \
                               const SPL::map<K,T>& rhs)                \
    {                                                                   \
        SPL::map<K,RET> result;                                         \
        typename SPL::map<K,T>::const_iterator it;                      \
        for (it = lhs.begin();  it != lhs.end(); it++) {                \
            typename SPL::map<K,T>::const_iterator it2 =                \
                rhs.find(it->first);                                    \
            if (it2 == rhs.end())                                       \
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR, \
                   SPL_APPLICATION_RUNTIME_DOT_MAP_ERROR(#MSG),         \
                   SPL_FUNC_DBG);                                       \
            result.add (it->first, it->second MSG it2->second);         \
        }                                                               \
        return result;                                                  \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline SPL::bmap<K,RET,N> FCN(const SPL::bmap<K,T,N>& lhs,          \
                                  const SPL::bmap<K,T,N>& rhs)          \
    {                                                                   \
        SPL::bmap<K,RET,N> result;                                      \
        typename SPL::bmap<K,T,N>::const_iterator it;                   \
        for (it = lhs.begin();  it != lhs.end(); it++) {                \
            typename SPL::bmap<K,T,N>::const_iterator it2 =             \
                rhs.find(it->first);                                    \
            if (it2 == rhs.end())                                       \
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR, \
                    SPL_APPLICATION_RUNTIME_DOT_MAP_ERROR(#MSG),        \
                    SPL_FUNC_DBG);                                      \
            const T& t = it->second MSG it2->second;                    \
            result.insert (std::make_pair(it->first, t));               \
        }                                                               \
        return result;                                                  \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline SPL::map<K,RET> FCN(const T& lhs,                            \
                               const SPL::map<K,T>& rhs)                \
    {                                                                   \
        SPL::map<K,RET> result;                                         \
        typename SPL::map<K,T>::const_iterator it;                      \
        for (it = rhs.begin();  it != rhs.end(); it++)                  \
            result.add (it->first, lhs MSG it->second);                 \
        return result;                                                  \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline SPL::bmap<K,RET,N> FCN(const T& lhs,                         \
                                  const SPL::bmap<K,T,N>& rhs)          \
    {                                                                   \
        SPL::bmap<K,RET,N> result;                                      \
        typename SPL::bmap<K,T,N>::const_iterator it;                   \
        for (it = rhs.begin();  it != rhs.end(); it++) {                \
            const T& t = lhs MSG it->second;                            \
            result.insert (std::make_pair(it->first, t));               \
        }                                                               \
        return result;                                                  \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline SPL::map<K,RET> FCN(const SPL::map<K,T>& lhs,                \
                               const T& rhs)                            \
    {                                                                   \
        SPL::map<K,RET> result;                                         \
        typename SPL::map<K,T>::const_iterator it;                      \
        for (it = lhs.begin();  it != lhs.end(); it++)                  \
            result.add (it->first, it->second MSG rhs);                 \
        return result;                                                  \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline SPL::bmap<K,RET,N> FCN(const SPL::bmap<K,T,N>& lhs,          \
                                  const T& rhs)                         \
    {                                                                   \
        SPL::bmap<K,RET,N> result;                                      \
        typename SPL::bmap<K,T,N>::const_iterator it;                   \
        for (it = lhs.begin();  it != lhs.end(); it++) {                \
            const T& t = it->second MSG rhs;                            \
            result.insert (std::make_pair(it->first, t));               \
        }                                                               \
        return result;                                                  \
    }                                                                   \

// .op functionality with res
#define DOT_OP_RES(FCN,OP,MSG)                                          \
    template<class T>                                                   \
    inline void FCN(const SPL::list<T>& lhs,                            \
                    const SPL::list<T>& rhs,                            \
                          SPL::list<T>& result)                         \
    {                                                                   \
        uint32_t num = rhs.size();                                      \
        if (num != lhs.size())                                          \
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,     \
               SPL_APPLICATION_RUNTIME_DOT_OP_ERROR("." #MSG),          \
               SPL_FUNC_DBG);                                           \
      SPL::list<T> resultTemp;						                    \
      resultTemp.reserve (num);                                         \
      std::transform(lhs.begin(), lhs.end(), rhs.begin(),               \
                       std::back_inserter(resultTemp), OP<T>());        \
      result.swap(resultTemp);                                          \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline void FCN(const SPL::blist<T,N>& lhs,                         \
                    const SPL::blist<T,N>& rhs,                         \
                          SPL::blist<T,N>& result)                      \
    {                                                                   \
        uint32_t num = rhs.size();                                      \
        for (uint32_t i = 0; i < num; i++)                              \
              result[i] = lhs[i] MSG rhs[i];                            \
        result.resize(num);                                             \
    }                                                                   \
                                                                        \
    template<class T>                                                   \
    inline void FCN(const T& lhs,                                       \
                    const SPL::list<T>& rhs,                            \
                          SPL::list<T>& result)                         \
    {                                                                   \
        uint32_t num = rhs.size();                                      \
        SPL::list<T> resultTemp;					\
        resultTemp.reserve (num);                                       \
        for (uint32_t i = 0; i < num; i++)                              \
	  resultTemp.push_back( lhs MSG rhs[i] );			\
        result.swap(resultTemp);                                        \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline void FCN(const T& lhs,                                       \
                    const SPL::blist<T,N>& rhs,                         \
                          SPL::blist<T,N>& result)                      \
    {                                                                   \
        uint32_t num = rhs.size();                                      \
        for (uint32_t i = 0; i < num; i++)                              \
              result[i] = lhs MSG rhs[i];                               \
        result.resize(num);                                             \
    }                                                                   \
                                                                        \
    template<class T>                                                   \
    inline void FCN(const SPL::list<T>& lhs,                            \
                    const T& rhs,                                       \
                          SPL::list<T>& result)                         \
    {                                                                   \
        uint32_t num = lhs.size();                                      \
        SPL::list<T> resultTemp;					\
        resultTemp.reserve (num);                                       \
        for (uint32_t i = 0; i < num; i++)                              \
          resultTemp.push_back( lhs[i] MSG rhs );			\
        result.swap(resultTemp);                                        \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline void FCN(const SPL::blist<T,N>& lhs,                         \
                    const T& rhs,                                       \
                          SPL::blist<T,N>& result)                      \
    {                                                                   \
        uint32_t num = lhs.size();                                      \
        for (uint32_t i = 0; i < num; i++)                              \
              result[i] = lhs[i] MSG rhs;                               \
        result.resize(num);                                             \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline void FCN(const SPL::map<K,T>& lhs,                           \
                    const SPL::map<K,T>& rhs,                           \
                          SPL::map<K,T>& result)                        \
    {                                                                   \
        SPL::map<K,T> resultTemp;                                       \
        typename SPL::map<K,T>::const_iterator it;                      \
        for (it = lhs.begin();  it != lhs.end(); it++) {                \
            typename SPL::map<K,T>::const_iterator it2 =                \
                rhs.find(it->first);                                    \
            if (it2 == rhs.end())                                       \
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR, \
                   SPL_APPLICATION_RUNTIME_DOT_MAP_ERROR(#MSG),         \
                   SPL_FUNC_DBG);                                       \
            resultTemp.add (it->first, it->second MSG it2->second);     \
        }                                                               \
        result.swap(resultTemp);                                        \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline void FCN(const SPL::bmap<K,T,N>& lhs,                        \
                    const SPL::bmap<K,T,N>& rhs,                        \
                          SPL::bmap<K,T,N>& result)                     \
    {                                                                   \
        SPL::bmap<K,T,N> resultTemp;                                    \
        typename SPL::bmap<K,T,N>::const_iterator it;                   \
        for (it = lhs.begin();  it != lhs.end(); it++) {                \
            typename SPL::bmap<K,T,N>::const_iterator it2 =             \
                rhs.find(it->first);                                    \
            if (it2 == rhs.end())                                       \
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR, \
                   SPL_APPLICATION_RUNTIME_DOT_MAP_ERROR(#MSG),         \
                   SPL_FUNC_DBG);                                       \
            const T& t = it->second MSG it2->second;                    \
            resultTemp.insert (std::make_pair(it->first, t));           \
        }                                                               \
        result.swap(resultTemp);                                        \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline void FCN(const T& lhs,                                       \
                    const SPL::map<K,T>& rhs,                           \
                          SPL::map<K,T>& result)                        \
    {                                                                   \
        SPL::map<K,T> resultTemp;                                       \
        typename SPL::map<K,T>::const_iterator it;                      \
        for (it = rhs.begin();  it != rhs.end(); it++)                  \
            resultTemp.add (it->first, lhs MSG it->second);             \
        result.swap(resultTemp);                                        \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline void FCN(const T& lhs,                                       \
                    const SPL::bmap<K,T,N>& rhs,                        \
                          SPL::bmap<K,T,N>& result)                     \
    {                                                                   \
        SPL::bmap<K,T,N> resultTemp;                                    \
        typename SPL::bmap<K,T,N>::const_iterator it;                   \
        for (it = rhs.begin();  it != rhs.end(); it++) {                \
            const T& t = lhs MSG it->second;                            \
            resultTemp.insert (std::make_pair(it->first, t));               \
        }                                                               \
        result.swap(resultTemp);                                        \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline void FCN(const SPL::map<K,T>& lhs,                           \
                    const T& rhs,                                       \
                          SPL::map<K,T>& result)                        \
    {                                                                   \
        SPL::map<K,T> resultTemp;                                       \
        typename SPL::map<K,T>::const_iterator it;                      \
        for (it = lhs.begin();  it != lhs.end(); it++)                  \
            resultTemp.add (it->first, it->second MSG rhs);             \
        result.swap(resultTemp);                                        \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline void FCN(const SPL::bmap<K,T,N>& lhs,                        \
                    const T& rhs,                                       \
                          SPL::bmap<K,T,N>& result)                     \
    {                                                                   \
        SPL::bmap<K,T,N> resultTemp;                                    \
        typename SPL::bmap<K,T,N>::const_iterator it;                   \
        for (it = lhs.begin();  it != lhs.end(); it++) {                \
            const T& t = it->second MSG rhs;                            \
            resultTemp.insert (std::make_pair(it->first, t));           \
        }                                                               \
        result.swap(resultTemp);                                        \
    }                                                                   \


// .op overloads
#define DOT_OP_OVLD(FCN,OP)                                             \
    template<class T>                                                   \
    inline SPL::list<T> operator OP(const SPL::list<T>& lhs,            \
                                    const SPL::list<T>& rhs)            \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline SPL::blist<T,N> operator OP(const SPL::blist<T,N>& lhs,      \
                                       const SPL::blist<T,N>& rhs)      \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \
                                                                        \
    template<class T>                                                   \
    inline SPL::list<T> operator OP(const T& lhs,                       \
                                    const SPL::list<T>& rhs)            \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline SPL::blist<T,N> operator OP(const T& lhs,                    \
                                       const SPL::blist<T,N>& rhs)      \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \
                                                                        \
    template<class T>                                                   \
    inline SPL::list<T> operator OP(const SPL::list<T>& lhs,            \
                                    const T& rhs)                       \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline SPL::blist<T,N> operator OP(const SPL::blist<T,N>& lhs,      \
                                       const T& rhs)                    \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline SPL::map<K,T> operator OP(const SPL::map<K,T>& lhs,          \
                                     const SPL::map<K,T>& rhs)          \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline SPL::bmap<K,T,N> operator OP(const SPL::bmap<K,T,N>& lhs,    \
                                        const SPL::bmap<K,T,N>& rhs)    \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline SPL::map<K,T> operator OP(const T& lhs,                      \
                                     const SPL::map<K,T>& rhs)          \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline SPL::bmap<K,T,N> operator OP(const T& lhs,                   \
                                        const SPL::bmap<K,T,N>& rhs)    \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline SPL::map<K,T> operator OP(const SPL::map<K,T>& lhs,          \
                                     const T& rhs)                      \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline SPL::bmap<K,T,N> operator OP(const SPL::bmap<K,T,N>& lhs,    \
                                        const T& rhs)                   \
    {                                                                   \
        return FCN(lhs, rhs);                                           \
    }                                                                   \

// op= functionality
#define DOT_OP_ASSIGN(FCN,OP,MSG)                                       \
    template<class T>                                                   \
    inline void FCN(SPL::list<T>& lhs,                                  \
                    const SPL::list<T>& rhs)                            \
    {                                                                   \
        uint32_t num = rhs.size();                                      \
        if (num != lhs.size())                                          \
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,     \
               SPL_APPLICATION_RUNTIME_DOT_OP_ERROR(#MSG),              \
               SPL_FUNC_DBG);                                           \
        std::transform(lhs.begin(), lhs.end(), rhs.begin(),             \
                       lhs.begin(), OP<T>());                           \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline void FCN(SPL::blist<T,N>& lhs,                               \
                    const SPL::blist<T,N>& rhs)                         \
    {                                                                   \
        std::transform(lhs.begin(), lhs.end(), rhs.begin(),             \
                       lhs.begin(), OP<T>());                           \
    }                                                                   \
                                                                        \
    template<class T>                                                   \
    inline void FCN(SPL::list<T>& lhs,                                  \
                    const T& rhs)                                       \
    {                                                                   \
        uint32_t num = lhs.size();                                      \
        for (uint32_t i = 0; i < num; i++)                              \
            lhs[i] MSG rhs;                                             \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline void FCN(SPL::blist<T,N>& lhs,                               \
                    const T& rhs)                                       \
    {                                                                   \
        uint32_t num = lhs.size();                                      \
        for (uint32_t i = 0; i < num; i++)                              \
            lhs[i] MSG rhs;                                             \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline void FCN(SPL::map<K,T>& lhs,                                 \
                    const SPL::map<K,T>& rhs)                           \
    {                                                                   \
        typename SPL::map<K,T>::iterator it;                            \
        for (it = lhs.begin();  it != lhs.end(); it++) {                \
            typename SPL::map<K,T>::const_iterator it2 =                \
                rhs.find(lhs->first);                                   \
            if (it2 == rhs.end())                                       \
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR, \
                   SPL_APPLICATION_RUNTIME_DOT_MAP_ERROR(#MSG),         \
                   SPL_FUNC_DBG);                                       \
            it->second MSG it2->second;                                 \
        }                                                               \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline void FCN(SPL::bmap<K,T,N>& lhs,                              \
                    const SPL::bmap<K,T,N>& rhs)                        \
    {                                                                   \
        typename SPL::bmap<K,T,N>::iterator it;                         \
        for (it = lhs.begin();  it != lhs.end(); it++) {                \
            typename SPL::bmap<K,T,N>::const_iterator it2 =             \
                rhs.find(lhs->first);                                   \
            if (it2 == rhs.end())                                       \
                SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR, \
                   SPL_APPLICATION_RUNTIME_DOT_MAP_ERROR(#MSG),         \
                   SPL_FUNC_DBG);                                       \
            it->second MSG it2->second;                                 \
        }                                                               \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline void FCN(SPL::map<K,T>& lhs,                                 \
                    const T& rhs)                                       \
    {                                                                   \
        typename SPL::map<K,T>::iterator it;                            \
        for (it = lhs.begin();  it != lhs.end(); it++) {                \
            it->second MSG rhs;                                         \
        }                                                               \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline void FCN(SPL::bmap<K,T,N>& lhs,                              \
                    const T& rhs)                                       \
    {                                                                   \
        typename SPL::bmap<K,T,N>::iterator it;                         \
        for (it = lhs.begin();  it != lhs.end(); it++) {                \
            it->second MSG rhs;                                         \
        }                                                               \
    }                                                                   \

// op= overloads
#define DOT_OP_ASSIGN_OVLD(FCN,OP)                                      \
    template<class T>                                                   \
    inline SPL::list<T>& operator OP(SPL::list<T>& lhs,                 \
                                     const SPL::list<T>& rhs)           \
    {                                                                   \
        FCN(lhs, rhs);                                                  \
        return lhs;                                                     \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline SPL::blist<T,N>& operator OP(SPL::blist<T,N>& lhs,           \
                                        const SPL::blist<T,N>& rhs)     \
    {                                                                   \
        FCN(lhs, rhs);                                                  \
        return lhs;                                                     \
    }                                                                   \
                                                                        \
    template<class T>                                                   \
    inline SPL::list<T>& operator OP(SPL::list<T>& lhs,                 \
                                     const T& rhs)                      \
    {                                                                   \
        FCN(lhs, rhs);                                                  \
        return lhs;                                                     \
    }                                                                   \
                                                                        \
    template<class T, int32_t N>                                        \
    inline SPL::blist<T,N>& operator OP(SPL::blist<T,N>& lhs,           \
                                        const T& rhs)                   \
    {                                                                   \
        FCN(lhs, rhs);                                                  \
        return lhs;                                                     \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline SPL::map<K,T>& operator OP(SPL::map<K,T>& lhs,               \
                                      const SPL::map<K,T>& rhs)         \
    {                                                                   \
        FCN(lhs, rhs);                                                  \
        return lhs;                                                     \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline SPL::bmap<K,T,N>& operator OP(SPL::bmap<K,T,N>& lhs,         \
                                         const SPL::bmap<K,T,N>& rhs)   \
    {                                                                   \
        FCN(lhs, rhs);                                                  \
        return lhs;                                                     \
    }                                                                   \
                                                                        \
    template<class K, class T>                                          \
    inline SPL::map<K,T>& operator OP(SPL::map<K,T>& lhs,               \
                                      const T& rhs)                     \
    {                                                                   \
        FCN(lhs, rhs);                                                  \
        return lhs;                                                     \
    }                                                                   \
                                                                        \
    template<class K, class T, int32_t N>                               \
    inline SPL::bmap<K,T,N>& operator OP(SPL::bmap<K,T,N>& lhs,         \
                                         const T& rhs)                  \
    {                                                                   \
        FCN(lhs, rhs);                                                  \
        return lhs;                                                     \
    }                                                                   \

PUSH_DLL_PUBLIC

namespace SPL
{
#ifndef DOXYGEN_SKIP_FOR_USERS
    template<class T>
    struct _and : public std::binary_function<T, T, T> {
        T operator()(const T& l, const T& r) const { return l & r; }
    };
    template<class T>
    struct _or : public std::binary_function<T, T, T> {
        T operator()(const T& l, const T& r) const { return l | r; }
    };
    template<class T>
    struct lshift : public std::binary_function<T, T, T> {
        T operator()(const T& l, const T& r) const { return l << r; }
    };
    template<class T>
    struct rshift : public std::binary_function<T, T, T> {
        T operator()(const T& l, const T& r) const { return l >> r; }
    };
    template<class T>
    struct _xor : public std::binary_function<T, T, T> {
        T operator()(const T& l, const T& r) const { return l ^ r; }
    };
#endif /* DOXYGEN_SKIP_FOR_USERS */

    DOT_OP(dotPlus, std::plus, +, T)
    DOT_OP(dotMinus, std::minus, -, T)
    DOT_OP(dotTimes, std::multiplies, *, T)
    DOT_OP(dotDivides, std::divides, /, T)
    DOT_OP(dotModulus, std::modulus, %, T)
    DOT_OP(dotAnd, _and, &, T)
    DOT_OP(dotOr, _or, |, T)
    DOT_OP(dotXor, _xor, ^, T)
    DOT_OP(dotLshift, lshift, <<, T)
    DOT_OP(dotRshift, rshift, >>, T)

    DOT_OP_RES(dotPlus, std::plus, +)
    DOT_OP_RES(dotMinus, std::minus, -)
    DOT_OP_RES(dotTimes, std::multiplies, *)
    DOT_OP_RES(dotDivides, std::divides, /)
    DOT_OP_RES(dotModulus, std::modulus, %)
    DOT_OP_RES(dotAnd, _and, &)
    DOT_OP_RES(dotOr, _or, |)
    DOT_OP_RES(dotXor, _xor, ^)
    DOT_OP_RES(dotLshift, lshift, <<)
    DOT_OP_RES(dotRshift, rshift, >>)


    DOT_OP_OVLD(dotPlus, +)
    DOT_OP_OVLD(dotMinus, -)
    DOT_OP_OVLD(dotTimes, *)
    DOT_OP_OVLD(dotDivides, /)
    DOT_OP_OVLD(dotModulus,  %)
    DOT_OP_OVLD(dotAnd, &)
    DOT_OP_OVLD(dotOr, |)
    DOT_OP_OVLD(dotXor, ^)
    DOT_OP_OVLD(dotLshift, <<)
    DOT_OP_OVLD(dotRshift, >>)

    DOT_OP(dotEqual, std::equal_to, ==, SPL::boolean)
    DOT_OP(dotNotEqual, std::not_equal_to, !=, SPL::boolean)
    DOT_OP(dotGreaterThan, std::greater, >, SPL::boolean)
    DOT_OP(dotGreaterEqual, std::greater_equal, >=, SPL::boolean)
    DOT_OP(dotLessThan, std::less, <, SPL::boolean)
    DOT_OP(dotLessEqual, std::less_equal, <=, SPL::boolean)

    DOT_OP_ASSIGN(assignPlus, std::plus, +=)
    DOT_OP_ASSIGN(assignMinus, std::minus, -=)
    DOT_OP_ASSIGN(assignTimes, std::multiplies, *=)
    DOT_OP_ASSIGN(assignDivides, std::divides, /=)
    DOT_OP_ASSIGN(assignModulus, std::modulus, %=)
    DOT_OP_ASSIGN(assignAnd, _and, &=)
    DOT_OP_ASSIGN(assignOr, _or, |=)
    DOT_OP_ASSIGN(assignLshift, lshift, <<=)
    DOT_OP_ASSIGN(assignRshift, rshift, >>=)
    DOT_OP_ASSIGN(assignXor, _xor, ^=)

    DOT_OP_ASSIGN_OVLD(assignPlus, +=)
    DOT_OP_ASSIGN_OVLD(assignMinus, -=)
    DOT_OP_ASSIGN_OVLD(assignTimes, *=)
    DOT_OP_ASSIGN_OVLD(assignDivides, /=)
    DOT_OP_ASSIGN_OVLD(assignModulus, %=)
    DOT_OP_ASSIGN_OVLD(assignAnd, &=)
    DOT_OP_ASSIGN_OVLD(assignOr, |=)
    DOT_OP_ASSIGN_OVLD(assignLshift, <<=)
    DOT_OP_ASSIGN_OVLD(assignRshift, >>=)
    DOT_OP_ASSIGN_OVLD(assignXor, ^=)
};

#undef DOT_OP
#undef DOT_OP_OVLD
#undef DOT_OP_ASSIGN
#undef DOT_OP_ASSIGN_OVLD
#undef DOT_OP_RES

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_MAPPED_OPERATORS_H */
