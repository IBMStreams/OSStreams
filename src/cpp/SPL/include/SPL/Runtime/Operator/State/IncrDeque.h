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
 * \file IncrDeque.h \brief Definition of the SPL::IncrDeque class template.
 */

#ifndef SPL_RUNTIME_OPERATOR_STATE_INCR_DEQUE_H
#define SPL_RUNTIME_OPERATOR_STATE_INCR_DEQUE_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif
#include <SPL/Runtime/Operator/State/IncrementalCheckpointType.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <deque>
#include <utility>
#include <assert.h>

#ifndef DOXYGEN_SKIP_FOR_USERS

PUSH_DLL_PUBLIC
namespace SPL
{
    /// Forward desclaration
    template<typename T>
    class IncrDeque;

    /// \brief Class template that represents a UpdataLog for incremental deque
    template<typename T>
    class DequeUpdateLog : public UpdateLog
    {
    public:
        /// Constructor
        /// @param param opaque paramter for initialization
        DequeUpdateLog(void * param)
            : UpdateLog(param), popCount_(0), queueSize_(0), minErasePos_(0), maxErasePos_(0) { }

        /// Log a pop_front() call
        void logPopFront()
        {
            popCount_ ++;
        }

        /// Log a clear() call
        void logClear()
        {
            minErasePos_ = 0;
            maxErasePos_ = 0;
            erasePositions_.clear();
            popCount_ = queueSize_;
        }

        /// Log an erase() call
        /// @param the offset of the erased item
        void logErase(const uint64_t pos)
        {
            try {
                // determine the offset of the erased position w.r.t. the start of base queue
                uint64_t offset = popCount_ + pos;
                if (offset < queueSize_) {
                    if (maxErasePos_ <= pos) {
                        offset -= erasePositions_.size();
                        maxErasePos_ = pos;
                        erasePositions_.push_back(offset);
                    }
                    else if (minErasePos_ >= pos) {
                        minErasePos_ = pos;
                        erasePositions_.push_back(offset);
                    }
                    else {
                        for (typename std::deque<uint64_t>::iterator iter = erasePositions_.begin();
                            iter != erasePositions_.end(); ++iter) {
                            if (*iter <= pos) {
                                offset--;
                            }
                        }
                        erasePositions_.push_back(offset);
                    }
                }
            }
            catch(...) {
                onExceptionDuringLogging();
            }
        }

        /// Discard all logged update records
        void clear()
        {
            popCount_ = 0;
            erasePositions_.clear();
            minErasePos_ = 0;
            maxErasePos_ = 0;
        }

        /// Serialize the log to a Checkpoint
        /// @param ckpt a Checkpoint handle
        /// @param data the original data
        void serialize(Checkpoint & ckpt, void * data)
        {
            IncrDeque<T> * queue = static_cast<IncrDeque<T> *>(data);
            ckpt << queueSize_;
            ckpt << popCount_;
            ckpt.addUInt64(uint64_t(erasePositions_.size()));
            for (typename std::deque<uint64_t>::const_iterator iter = erasePositions_.begin();
                iter != erasePositions_.end(); ++iter) {
                ckpt << (*iter);
            }
            typename std::deque<T>::const_iterator iter = queue->begin();
            if (popCount_ + erasePositions_.size() < queueSize_) {
                iter += (queueSize_ - popCount_ - erasePositions_.size());
            }
            uint64_t pushCount = uint64_t(queue->end() - iter);
            ckpt << pushCount;
            for (; iter != queue->end(); ++ iter) {
                ckpt << (*iter);
            }
            queueSize_ = queue->size();
        }

        /// Deserialize the log from a Checkpoint and apply to the original data
        /// @param ckpt a Checkpoint handle
        /// @param data the original data
        void deserializeAndApply(Checkpoint & ckpt, void * data)
        {
            IncrDeque<T> * queue = static_cast<IncrDeque<T> *>(data);
            ckpt >> queueSize_;
            ckpt >> popCount_;
            uint64_t eraseCount = ckpt.getUInt64();
            for (uint64_t i = 0; i < eraseCount; i ++) {
                uint64_t offset = ckpt.getUInt64();
                // assume offsets are sorted
                typename std::deque<T>::iterator iter = queue->begin() + offset + i;
                queue->erase(iter);
            }
            if (popCount_ < queue->size()) {
                for (uint64_t i = 0; i < popCount_; i ++) {
                    queue->pop_front();
                }
            }
            else {
                queue->clear();
            }
            uint64_t pushCount = ckpt.getUInt64();
            for (uint64_t j = 0; j < pushCount; j ++) {
                T value;
                ckpt >> value;
                queue->push_back(value);
            }
            queueSize_ = queue->size();
        }

        /// A optional function to call upon the event of doing a base checkpoint
        /// @param data the original data
        void onBaseCheckpoint(void * data)
        {
            IncrDeque<T> * queue = static_cast<IncrDeque<T> *>(data);
            queueSize_ = queue->size();
        }

    private:
        uint64_t popCount_;
        uint64_t queueSize_;
        uint64_t minErasePos_;
        uint64_t maxErasePos_;
        std::deque<uint64_t> erasePositions_;
    };

    /// \brief Class template that represents a specialized UpdataLog for incremental deque of pointer type.
    template<typename T>
    class DequeUpdateLog<T *> : public UpdateLog
    {
    public:
        /// Constructor
        /// @param param opaque paramter for initialization
        DequeUpdateLog(void * param)
            : UpdateLog(param), popCount_(0), queueSize_(0), minErasePos_(0), maxErasePos_(0) { }

        /// Log a pop_front() call
        void logPopFront()
        {
            popCount_ ++;
        }

        /// Log a clear() call
        void logClear()
        {
            minErasePos_ = 0;
            maxErasePos_ = 0;
            erasePositions_.clear();
            popCount_ = queueSize_;
        }

        /// Log an erase() call
        /// @param the offset of the erased item
        void logErase(const uint64_t pos)
        {
            try {
                // determine the offset of the erased position w.r.t. the start of base queue
                uint64_t offset = popCount_ + pos;
                if (offset < queueSize_) {
                    if (maxErasePos_ <= pos) {
                        offset -= erasePositions_.size();
                        maxErasePos_ = pos;
                        erasePositions_.push_back(offset);
                    }
                    else if (minErasePos_ >= pos) {
                        minErasePos_ = pos;
                        erasePositions_.push_back(offset);
                    }
                    else {
                        for (typename std::deque<uint64_t>::iterator iter = erasePositions_.begin();
                            iter != erasePositions_.end(); ++iter) {
                            if (*iter <= pos) {
                                offset--;
                            }
                        }
                        erasePositions_.push_back(offset);
                    }
                }
            }
            catch(...) {
                onExceptionDuringLogging();
            }
        }

        /// Discard all logged update records
        void clear()
        {
            popCount_ = 0;
            erasePositions_.clear();
            minErasePos_ = 0;
            maxErasePos_ = 0;
        }

        /// Serialize the log to a Checkpoint
        /// @param ckpt a Checkpoint handle
        /// @param data the original data
        void serialize(Checkpoint & ckpt, void * data)
        {
            IncrDeque<T *> * queue = static_cast<IncrDeque<T *> *>(data);
            ckpt << queueSize_;
            ckpt << popCount_;
            ckpt.addUInt64(uint64_t(erasePositions_.size()));
            for (typename std::deque<uint64_t>::const_iterator iter = erasePositions_.begin();
                iter != erasePositions_.end(); ++iter) {
                ckpt << (*iter);
            }
            typename std::deque<T *>::const_iterator iter = queue->begin();
            if (popCount_ + erasePositions_.size() < queueSize_) {
                iter += (queueSize_ - popCount_ - erasePositions_.size());
            }
            uint64_t pushCount = uint64_t(queue->end() - iter);
            ckpt << pushCount;
            for (; iter != queue->end(); ++ iter) {
                ckpt << (*iter);
            }
            queueSize_ = queue->size();
        }

        /// Deserialize the log from a Checkpoint and apply to the original data
        /// @param ckpt a Checkpoint handle
        /// @param data the original data
        void deserializeAndApply(Checkpoint & ckpt, void * data)
        {
            IncrDeque<T *> * queue = static_cast<IncrDeque<T *> *>(data);
            ckpt >> queueSize_;
            ckpt >> popCount_;
            uint64_t eraseCount = ckpt.getUInt64();
            for (uint64_t i = 0; i < eraseCount; i ++) {
                uint64_t offset = ckpt.getUInt64();
                // assume offsets are sorted
                typename std::deque<T *>::iterator iter = queue->begin() + offset + i;
                delete *iter;
                queue->erase(iter);
            }
            uint64_t deleteCount = popCount_;
            if (popCount_ >= queue->size()) {
                deleteCount = queue->size();
            }
            for (; deleteCount > 0; deleteCount --) {
                delete *(queue->begin());
                queue->pop_front();
            }
            uint64_t pushCount = ckpt.getUInt64();
            for (uint64_t j = 0; j < pushCount; j ++) {
                T * value;
                ckpt >> value;
                queue->push_back(value);
            }
            queueSize_ = queue->size();
        }

        /// A optional function to call upon the event of doing a base checkpoint
        /// @param data the original data
        void onBaseCheckpoint(void * data)
        {
            IncrDeque<T *> * queue = static_cast<IncrDeque<T *> *>(data);
            queueSize_ = queue->size();
        }

    private:
        uint64_t popCount_;
        uint64_t queueSize_;
        uint64_t minErasePos_;
        uint64_t maxErasePos_;
        std::deque<uint64_t> erasePositions_;
    };

    /// \brief Class template that represents a deque with incremental checkpointing capability
    template<typename T>
    class IncrDeque : public IncrementalCheckpointType<DequeUpdateLog<T> >
    {
    public:
        typedef typename std::deque<T>::iterator iterator;
        typedef typename std::deque<T>::const_iterator const_iterator;
        typedef typename std::deque<T>::size_type size_type;
        typedef typename std::deque<T>::reference reference;
        typedef typename std::deque<T>::const_reference const_reference;
        typedef typename std::deque<T>::reverse_iterator reverse_iterator;
        typedef typename std::deque<T>::const_reverse_iterator const_reverse_iterator;

        /// Constructor
        IncrDeque() : IncrementalCheckpointType<DequeUpdateLog<T> >(NULL) { }

        /// Wrapper function for deque::push_back()
        /// @param v value to add to the back of queue
        void push_back(const T & v)
        {
            queue_.push_back(v);
            // no need to log push_back()
        }

        /// Wrapper function for deque::pop_front()
        void pop_front()
        {
            queue_.pop_front();
            if (this->log_.isLoggingEnabled()) {
                this->log_.logPopFront();
            }
        }

        /// Wrapper function for deque::clear()
        void clear()
        {
            queue_.clear();
            if (this->log_.isLoggingEnabled()) {
                this->log_.logClear();
            }
        }

        /// Wrapper function for deque::erase()
        iterator erase (iterator position)
        {
            iterator iter = queue_.erase(position);
            if (this->log_.isLoggingEnabled()) {
                this->log_.logErase(uint64_t(iter - queue_.begin()));
            }
            return iter;
        }

        /// Wrapper function for deque::begin()
        iterator begin()
        {
            return queue_.begin();
        }

        /// Wrapper function for deque::begin()
        const_iterator begin() const
        {
            return queue_.begin();
        }

        /// Wrapper function for deque::end()
        iterator end()
        {
            return queue_.end();
        }

        /// Wrapper function for deque::end()
        const_iterator end() const
        {
            return queue_.end();
        }

        /// Wrapper function for deque::size()
        size_type size() const
        {
            return queue_.size();
        }

        /// Wrapper function for deque::empty()
        bool empty() const
        {
            return queue_.empty();
        }

        /// Wrapper function for deque::back()
        reference back()
        {
            return queue_.back();
        }

        /// Wrapper function for deque::back()
        const_reference back() const
        {
            return queue_.back();
        }

        /// Wrapper function for deque::rbegin()
        reverse_iterator rbegin()
        {
            return queue_.rbegin();
        }

        /// Wrapper function for deque::rbegin()
        const_reverse_iterator rbegin() const
        {
            return queue_.rbegin();
        }

        /// Wrapper function for deque::rend()
        reverse_iterator rend()
        {
            return queue_.rend();
        }

        /// Wrapper function for deque::rend()
        const_reverse_iterator rend() const
        {
            return queue_.rend();
        }

        /// Wrapper function for deque::operator[] ()
        const_reference operator[] (size_type n) const
        {
            return queue_[n];
        }

        /// Get internal deque handle
        /// @return internal deque handle
        std::deque<T> & getHandle()
        {
            return queue_;
        }

    private:
        /// serialize data in base form
        /// @param ckpt Checkpoint handle
        void serializeBase(Checkpoint & ckpt)
        {
            ckpt << queue_;
        }

        /// deserialize data in base form
        /// @param ckpt Checkpoint handle
        void deserializeBase(Checkpoint & ckpt)
        {
            ckpt >> queue_;
        }

        std::deque<T> queue_; // internal deque handle
    };
} // namespace SPL
POP_DLL_PUBLIC

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_INCR_DEQUE_H
