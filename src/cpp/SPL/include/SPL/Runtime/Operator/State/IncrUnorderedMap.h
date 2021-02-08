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

/*
 * \file IncrUnorderedMap.h \brief Definition of the SPL::IncrUnorderedMap class template.
 */

#ifndef SPL_RUNTIME_OPERATOR_STATE_INCR_UNORDERED_MAP_H
#define SPL_RUNTIME_OPERATOR_STATE_INCR_UNORDERED_MAP_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif
#include <SPL/Runtime/Operator/State/IncrementalCheckpointType.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <tr1/unordered_map>
#include <utility>
#include <assert.h>

#ifndef DOXYGEN_SKIP_FOR_USERS

PUSH_DLL_PUBLIC
namespace SPL
{
    /// Forward desclaration
    template<typename Key, typename T>
    class IncrUnorderedMap;

    /// \brief Class template that represents a log of UpdateRecrods on a IncrUnorderedMap
    template<typename Key, typename T>
    class IncrUnorderedMapLog : public UpdateLog
    {
    public:
        /// Constructor
        /// @param param opaque parameter for initialization
        IncrUnorderedMapLog(void * param)
            : UpdateLog(param), cleared_(false) { }

        /// Log an insert() call
        /// @param key the key to insert
        void logInsert(Key const & key)
        {
            try {
                mapLog_[key] = INSERT_INDEX;
            }
            catch(...) {
                onExceptionDuringLogging();
            }
        }

        /// Log an erase() call
        /// @param key the key to erase
        void logErase(Key const & key)
        {
            typename std::tr1::unordered_map<Key, uint8_t>::iterator iter = mapLog_.find(key);
            if (iter != mapLog_.end()) {
                if (iter->second != INSERT_INDEX && iter->second != FIND2_INDEX) {
                    iter->second = ERASE_INDEX;
                }
                else {
                    mapLog_.erase(iter);
                }
            }
            else {
                try {
                    mapLog_[key] = ERASE_INDEX;
                }
                catch(...) {
                    onExceptionDuringLogging();
                }
            }
        }

        /// Log a find() call
        /// @param key the key to find
        void logFind(Key const & key)
        {
            typename std::tr1::unordered_map<Key, uint8_t>::iterator iter = mapLog_.find(key);
            if (iter != mapLog_.end()) {
                if (iter->second == INSERT_INDEX || iter->second == FIND2_INDEX) {
                    iter->second = FIND2_INDEX;
                }
                else {
                    iter->second = FIND_INDEX;
                }
            }
            else {
                try {
                    mapLog_[key] = FIND_INDEX;
                }
                catch(...) {
                    onExceptionDuringLogging();
                }
            }
        }

        /// Log a clear() call
        void logClear()
        {
            mapLog_.clear();
            cleared_ = true;
        }

        /// Discard all logged update records
        void clear()
        {
            mapLog_.clear();
            cleared_ = false; // set to initial state
        }

        /// Serialize the log to a Checkpoint
        /// @param ckpt a Checkpoint handle
        /// @param data the original data
        void serialize(Checkpoint & ckpt, void * data)
        {
            IncrUnorderedMap<Key, T> * origMap = static_cast<IncrUnorderedMap<Key, T> *>(data);
            ckpt << cleared_;
            ckpt.addUInt64(mapLog_.size());
            typename std::tr1::unordered_map<Key, uint8_t>::iterator iter = mapLog_.begin();

            for (; iter != mapLog_.end(); ++ iter) {
                uint8_t index = iter->second;
                ckpt << index; // i.e., type of update record
                ckpt << iter->first; // key
                if (index == INSERT_INDEX || index == FIND_INDEX || index == FIND2_INDEX) { // insert or find (in-place update)
                    typename std::tr1::unordered_map<Key, T>::iterator origIter = origMap->getHandle().find(iter->first);
                    assert(origIter != origMap->end());
                    ckpt << origIter->second; // inserted or updated value
                }
            }
        }

        /// Deserialize the log from a Checkpoint and apply to the original data
        /// @param ckpt a Checkpoint handle
        /// @param data the original data
        void deserializeAndApply(Checkpoint & ckpt, void * data)
        {
            IncrUnorderedMap<Key, T> * origMap = static_cast<IncrUnorderedMap<Key, T> *>(data);
            ckpt >> cleared_;
            if (cleared_ == true) {
                origMap->clear();
            }
            uint64_t logSize = ckpt.getUInt64();
            for (uint64_t i = 0; i < logSize; i ++) {
                uint8_t index = ckpt.getUInt8();
                Key key;
                ckpt >> key;
                if (index == INSERT_INDEX) {
                    T dummyValue;
                    T & value = origMap->insert(std::make_pair(key, dummyValue)).first->second;
                    ckpt >> value;
                }
                else if (index == FIND_INDEX || index == FIND2_INDEX) {
                    T & value = origMap->find(key)->second;
                    ckpt >> value;
                }
                else if (index == ERASE_INDEX) {
                    typename IncrUnorderedMap<Key, T>::iterator iter = origMap->find(key);
                    if (iter != origMap->end()) {
                        origMap->erase(iter);
                    }
                }
            }
        }

        /// Constants
        static const uint8_t INSERT_INDEX;
        static const uint8_t ERASE_INDEX;
        static const uint8_t FIND_INDEX;
        static const uint8_t FIND2_INDEX;

    private:
        std::tr1::unordered_map<Key, uint8_t> mapLog_;
        bool cleared_;
    };

    template<typename Key, typename T>
    const uint8_t IncrUnorderedMapLog<Key, T>::INSERT_INDEX = 0;
    template<typename Key, typename T>
    const uint8_t IncrUnorderedMapLog<Key, T>::ERASE_INDEX = 1;
    template<typename Key, typename T>
    const uint8_t IncrUnorderedMapLog<Key, T>::FIND_INDEX = 2;
    template<typename Key, typename T>
    const uint8_t IncrUnorderedMapLog<Key, T>::FIND2_INDEX = 3;

    /// \brief Class template that represents an unordered_map with incremental checkpointing capability
    template<typename Key, typename T>
    class IncrUnorderedMap : public IncrementalCheckpointType<IncrUnorderedMapLog<Key, T> >
    {
    public:
        typedef typename std::tr1::unordered_map<Key, T>::value_type value_type;
        typedef typename std::tr1::unordered_map<Key, T>::iterator iterator;
        typedef typename std::tr1::unordered_map<Key, T>::const_iterator const_iterator;
        typedef typename std::tr1::unordered_map<Key, T>::size_type size_type;

        /// Constructor
        IncrUnorderedMap() : IncrementalCheckpointType<IncrUnorderedMapLog<Key, T> >(NULL) { }

        /// Wrapper function for unordered_map::insert()
        std::pair<iterator, bool> insert(const value_type & value)
        {
            std::pair<iterator, bool> pos = map_.insert(value);
            if (this->log_.isLoggingEnabled()) {
                this->log_.logInsert(value.first);
            }
            return pos;
        }

        /// Wrapper function for unordered_map::erase()
        iterator erase(iterator pos)
        {
            Key key = pos->first;
            iterator iter = map_.erase(pos);
            if (this->log_.isLoggingEnabled()) {
                this->log_.logErase(key);
            }
            return iter;
        }

        /// Wrapper function for unordered_map::find()
        iterator find(const Key & key)
        {
            iterator iter = map_.find(key);
            if (this->log_.isLoggingEnabled()) {
                this->log_.logFind(key);
            }
            return iter;
        }

        /// Wrapper function for unordered_map::find()
        const_iterator find(const Key & key) const
        {
            const_iterator iter = map_.find(key);
            return iter;
        }

        /// Wrapper function for unordered_map::clear()
        void clear()
        {
            map_.clear();
            if (this->log_.isLoggingEnabled()) {
                this->log_.logClear();
            }
        }

        /// Wrapper function for unordered_map::begin()
        iterator begin()
        {
            return map_.begin();
        }

        /// Wrapper function for unordered_map::begin()
        const_iterator begin() const
        {
            return map_.begin();
        }

        /// Wrapper function for unordered_map::end()
        iterator end()
        {
            return map_.end();
        }

        /// Wrapper function for unordered_map::end()
        const_iterator end() const
        {
            return map_.end();
        }

        /// Wrapper function for unordered_map::size()
        size_type size() const
        {
            return map_.size();
        }

        /// Wrapper function for unordered_map::empty()
        bool empty() const
        {
            return map_.empty();
        }

        /// Get internal deque handle
        /// @return internal deque handle
        std::tr1::unordered_map<Key, T> & getHandle()
        {
            return map_;
        }

    private:
        /// serialize data in base form
        /// @param ckpt Checkpoint handle
        void serializeBase(Checkpoint & ckpt)
        {
            ckpt.addUInt64(static_cast<uint64_t>(map_.size()));
            for(typename std::tr1::unordered_map<Key, T>::iterator it = map_.begin();
                it != map_.end(); ++ it) {
                ckpt << it->first;
                ckpt << it->second;
            }
        }

        /// deserialize data in base form
        /// @param ckpt Checkpoint handle
        void deserializeBase(Checkpoint & ckpt)
        {
            uint64_t totalSize = ckpt.getUInt64();
            map_.clear();
            std::pair<Key, T> pr;
            for(uint64_t i = 0; i < totalSize; ++ i) {
                ckpt >> pr.first;
                ckpt >> pr.second;
                map_.insert(pr);
            }
        }

        std::tr1::unordered_map<Key, T> map_; // internal unordered map handle
    };
} // namespace SPL
POP_DLL_PUBLIC

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_INCR_UNORDERED_MAP_H
