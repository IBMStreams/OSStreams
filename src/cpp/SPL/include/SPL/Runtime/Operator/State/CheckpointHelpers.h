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

#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_HELPERS_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_HELPERS_H

/*!
 * \file CheckpointHelpers.h \brief Definitions of the << and >> operator
 * overloads for serializing/deserializing various data types to SPL::Checkpoint class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <UTILS/HashMapHelpers.h>
#include <deque>
#include <new>
#include <deque>
#include <tr1/memory>
#include <boost/shared_ptr.hpp>
#endif /* DOXYGEN_SKIP_FOR_USERS */

namespace SPL
{
PUSH_DLL_PUBLIC

/// Add/serialize a NULL-terminated string to the Checkpoint
/// @param ckpt the ByteBuffer<Checkpoint> instance
/// @param str a NULL-terminated string
/// @return the ByteBuffer<Checkpoint> instance
/// @throws DataStoreException if the Checkpoint is used for restoring or
/// if the given data cannot be added to Checkpoint
inline SPL::ByteBuffer<Checkpoint> & operator << (SPL::ByteBuffer<Checkpoint> & ckpt, const char * & str)
{
    ckpt.addNTStr(str);
    return ckpt;
}

/// Add/serialize a NULL-terminated string to the Checkpoint
/// @param ckpt the ByteBuffer<Checkpoint> instance
/// @param str a NULL-terminated string
/// @return the ByteBuffer<Checkpoint> instance
/// @throws DataStoreException if the Checkpoint is used for restoring or
/// if the given data cannot be added to Checkpoint
inline SPL::ByteBuffer<Checkpoint> & operator << (SPL::ByteBuffer<Checkpoint> & ckpt, const char * str)
{
    ckpt.addNTStr(str);
    return ckpt;
}

/// Get/deserialize a NULL-terminated string from the Checkpoint
/// @param ckpt the ByteBuffer<Checkpoint> instance
/// @param str return a NULL-terminated string extracted from the Checkpoint
/// @return the ByteBuffer<Checkpoint> instance
/// @throws DataStoreException if the Checkpoint is used for checkpointing or
/// if data cannot be extracted from Checkpoint
inline SPL::ByteBuffer<Checkpoint> & operator >> (SPL::ByteBuffer<Checkpoint> & ckpt, char * & str)
{
    str = ckpt.getNTStr();
    return ckpt;
}

/// Add/serialize data pointed by the given pointer to the Checkpoint.
/// The type T must be serializable to ByteBuffer<Checkpoint>.
/// @param ckpt the ByteBuffer<Checkpoint> instance
/// @param value a pointer
/// @return the ByteBuffer<Checkpoint> instance
/// @throws DataStoreException if the Checkpoint is used for restoring or
/// if the given data cannot be added to Checkpoint
template <class T>
SPL::ByteBuffer<Checkpoint> & operator << (SPL::ByteBuffer<Checkpoint> & ckpt, const T * & value)
{
    return (ckpt << *value);
}

/// Add/serialize data pointed by the given pointer to the Checkpoint.
/// The type T must be serializable to ByteBuffer<Checkpoint>.
/// @param ckpt the ByteBuffer<Checkpoint> instance
/// @param value a pointer
/// @return the ByteBuffer<Checkpoint> instance
/// @throws DataStoreException if the Checkpoint is used for restoring or
/// if the given data cannot be added to Checkpoint
template <class T>
SPL::ByteBuffer<Checkpoint> & operator << (SPL::ByteBuffer<Checkpoint> & ckpt, const T * value)
{
    return (ckpt << *value);
}

/// Extract/de-serialize data of type T from the Checkpoint.
/// The type T must be de-serializable from ByteBuffer<Checkpoint>.
/// The caller is responsible for de-allocating the memory returned by value with delete.
/// @param ckpt the ByteBuffer<Checkpoint> instance
/// @param value return the data extracted from the Checkpoint
/// @return the ByteBuffer<Checkpoint> instance
/// @throws DataStoreException if the Checkpoint is used for checkpointing or
/// if data cannot be extracted from Checkpoint or if memory allocation fails
template <class T>
SPL::ByteBuffer<Checkpoint> & operator >> (SPL::ByteBuffer<Checkpoint> & ckpt, T * & value)
{
    try {
        value = new T;
    }
    catch (const std::bad_alloc & e) {
        THROW_CHAR(DataStore, "cannot allocate memory");
    }
    ckpt >> *value;
    return ckpt;
}

// HACK
// We need to think about whether and how to checkpoint and restore
// shared pointers.  Ideally it should result in the same shared pointers
// being restored as were saved.  This implementation will turn the shared
// pointers into unique pointers, although still wrapped as shared pointers.
template <class T>
SPL::ByteBuffer<Checkpoint> & operator << (SPL::ByteBuffer<Checkpoint> & ckpt, boost::shared_ptr<T> & value) {
    return ckpt << value.get();
}

template <class T>
SPL::ByteBuffer<Checkpoint> & operator >> (SPL::ByteBuffer<Checkpoint> & ckpt, boost::shared_ptr<T> & value) {
    T* t = 0;
    ckpt >> t;
    value.reset(t);
    return ckpt;
}

template <class T>
SPL::ByteBuffer<Checkpoint> & operator << (SPL::ByteBuffer<Checkpoint> & ckpt, std::tr1::shared_ptr<T> & value) {
    return ckpt << value.get();
}

template <class T>
SPL::ByteBuffer<Checkpoint> & operator >> (SPL::ByteBuffer<Checkpoint> & ckpt, std::tr1::shared_ptr<T> & value) {
    T* t = 0;
    ckpt >> t;
    value.reset(t);
    return ckpt;
}

/// Extract/de-serialize data of type const T from the Checkpoint.
/// The type T must be de-serializable from ByteBuffer<Checkpoint>.
/// The caller is responsible for de-allocating the memory returned by value with delete.
/// @param ckpt the ByteBuffer<Checkpoint> instance
/// @param value return the data extracted from the Checkpoint
/// @return the ByteBuffer<Checkpoint> instance
/// @throws DataStoreException if the Checkpoint is used for checkpointing or
/// if data cannot be extracted from Checkpoint or if memory allocation fails
template <class T>
inline SPL::ByteBuffer<Checkpoint> & operator >> (SPL::ByteBuffer<Checkpoint> & ckpt, const T * & value)
{
    T * temp;
    try {
        temp = new T;
    }
    catch (const std::bad_alloc & e) {
        THROW_CHAR(DataStore, "cannot allocate memory");
    }
    ckpt >> *temp;
    value = temp;
    return ckpt;
}

/// Add/serialize an std::deque to the Checkpoint
/// @param ckpt the ByteBuffer<Checkpoint> instance
/// @param value an std::deque
/// @return the ByteBuffer<Checkpoint> instance
/// @throws DataStoreException if the Checkpoint is used for restoring or
/// if the given data cannot be added to Checkpoint
template <class T>
inline SPL::ByteBuffer<Checkpoint> & operator << (SPL::ByteBuffer<Checkpoint> & ckpt, const std::deque<T> & value)
{
    ckpt.addUInt32(static_cast<uint32_t>(value.size()));
    for (typename std::deque<T>::const_iterator it = value.begin(); it != value.end(); ++it) {
        ckpt << (*it);
    }
    return ckpt;
}

/// Extract/de-serialize an std::deque from the Checkpoint
/// @param ckpt the ByteBuffer<Checkpoint> instance
/// @param value return the std::deque extracted from the Checkpoint
/// @return the ByteBuffer<Checkpoint> instance
/// @throws DataStoreException if the Checkpoint is used for checkpointing or
/// if data cannot be extracted from Checkpoint
template <class T>
inline SPL::ByteBuffer<Checkpoint> & operator >> (SPL::ByteBuffer<Checkpoint> & ckpt, std::deque<T> & value)
{
    uint32_t size = ckpt.getUInt32();
    value.clear();
    T element;
    for (uint32_t i = 0; i < size; ++i) {
        ckpt >> element;
        value.push_back(element);
    }
    return ckpt;
}

/// Add/serialize an std::tr1::unordered_map to the Checkpoint
/// @param ckpt the ByteBuffer<Checkpoint> instance
/// @param value an std::tr1::unordered_map
/// @return the ByteBuffer<Checkpoint> instance
/// @throws DataStoreException if the Checkpoint is used for restoring or
/// if the given data cannot be added to Checkpoint
template <class K, class V>
inline SPL::ByteBuffer<Checkpoint> & operator << (SPL::ByteBuffer<Checkpoint> & ckpt, const std::tr1::unordered_map<K, V> & value)
{
    ckpt.addUInt32(static_cast<uint32_t>(value.size()));
    for(typename std::tr1::unordered_map<K,V>::const_iterator it = value.begin();
        it != value.end(); ++ it) {
        ckpt << it->first;
        ckpt << it->second;
    }
    return ckpt;
}

/// Extract/de-serialize an std::tr1::unordered_map from the Checkpoint
/// @param ckpt the ByteBuffer<Checkpoint> instance
/// @param value return the std::tr1::unordered_map extracted from the Checkpoint
/// @return the ByteBuffer<Checkpoint> instance
/// @throws DataStoreException if the Checkpoint is used for checkpointing or
/// if data cannot be extracted from Checkpoint
template <class K, class V>
inline SPL::ByteBuffer<Checkpoint> & operator >> (SPL::ByteBuffer<Checkpoint> & ckpt, std::tr1::unordered_map<K, V> & value)
{
    uint32_t size = ckpt.getUInt32();
    value.clear();
    std::pair<K,V> pr;
    for(uint32_t i = 0; i < size; ++ i) {
        ckpt >> pr.first >> pr.second;
        value.insert(pr);
    }
    return ckpt;
}

POP_DLL_PUBLIC
}

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_HELPERS_H
