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

#ifndef SPL_RUNTIME_WINDOW_IMPL_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_WINDOW_IMPL_H

#include "Serialization.h"

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/State/Checkpoint.h>
#include <SPL/Runtime/Window/PartitionEvictionImpl.h>
#include <SPL/Runtime/Window/PartitionEvictionPolicy.h>
#include <boost/atomic/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <tr1/functional>
#include <tr1/memory>
#include <tr1/type_traits>
#include <tr1/unordered_map>

namespace SPL {
// Forward declarations
template<class T, class G, class D, class S>
class AutoWinImplDataAcquirer;

// Memory allocation strategy
template<typename T>
struct Allocator
{
    static void allocate(const T&) {}
    static void deallocate(const T&) {}
};

template<typename T>
struct Allocator<T*>
{
    static void allocate(T*& t) { t = new T; }
    static void deallocate(T* t) { delete t; }
};

// Support for referencing / dereferencing pointers
template<typename T>
struct Referencer
{
    typedef T dereference_t;
    typedef T reference_t;
    static T& dereference(T& t) { return t; }
    static T const& dereference(T const& t) { return t; }
    static T& reference(T& t) { return t; }
    static T const& reference(T const& t) { return t; }
};

template<typename T>
struct Referencer<T*>
{
    typedef T dereference_t;
    typedef T* reference_t;
    static T& dereference(T* t) { return *t; }
    static T const& dereference(T const* t) { return *t; }
    static T* reference(T& t) { return &t; }
    static T const* reference(T const& t) { return &t; }
};

template<typename T>
struct Referencer<T const*>
{
    typedef T const dereference_t;
    typedef T const* reference_t;
    static T const& dereference(T const* t) { return *t; }
    static T const* reference(T const& t) { return &t; }
};

template<typename T>
struct Referencer<boost::shared_ptr<T> >
{
    typedef T dereference_t;
    typedef T* reference_t;
    static T& dereference(boost::shared_ptr<T>& t) { return *t; }
    static T const& dereference(boost::shared_ptr<T> const& t) { return *t; }
    static T* reference(boost::shared_ptr<T>& t) { return t.get(); }
    static T const* reference(boost::shared_ptr<T> const& t) { return t.get(); }
};

template<typename T>
struct Referencer<boost::shared_ptr<T const> >
{
    typedef T const dereference_t;
    typedef T const* reference_t;
    static T const& dereference(boost::shared_ptr<T const> const& t) { return *t; }
    static T const* reference(boost::shared_ptr<T const> const& t) { return t.get(); }
};

template<typename T>
struct Referencer<std::tr1::shared_ptr<T> >
{
    typedef T dereference_t;
    typedef T* reference_t;
    static T& dereference(std::tr1::shared_ptr<T>& t) { return *t; }
    static T const& dereference(std::tr1::shared_ptr<T> const& t) { return *t; }
    static T* reference(std::tr1::shared_ptr<T>& t) { return t.get(); }
    static T const* reference(std::tr1::shared_ptr<T> const& t) { return t.get(); }
};

template<typename T>
struct Referencer<std::tr1::shared_ptr<T const> >
{
    typedef T const dereference_t;
    typedef T const* reference_t;
    static T const& dereference(std::tr1::shared_ptr<T const> const& t) { return *t; }
    static T const* reference(std::tr1::shared_ptr<T const> const& t) { return t.get(); }
};

// Create a new tuple as a copy of a tuple.  This will create a
// local tuple, a pointer to a tuple, or a shared_ptr to a tuple
// as appropriate.  The Allocator::deallocate templates can be used to
// safely destroy objects created by Creator.
template<typename T>
struct Creator
{
    static T create(T const& t) { return T(t); }
};

template<typename T>
struct Creator<T*>
{
    static T* create(T const& t) { return new T(t); }
    static T* create(T* t) { return t; }
};

template<typename T>
struct Creator<boost::shared_ptr<T> >
{
    static boost::shared_ptr<T> create(T const& t) { return boost::shared_ptr<T>(new T(t)); }
    // This assumes ownership of the pointer.
    static boost::shared_ptr<T> create(T* t) { return boost::shared_ptr<T>(t); }
};

template<typename T>
struct Creator<std::tr1::shared_ptr<T> >
{
    static std::tr1::shared_ptr<T> create(T const& t) { return std::tr1::shared_ptr<T>(new T(t)); }
    // This assumes ownership of the pointer.
    static std::tr1::shared_ptr<T> create(T* t) { return std::tr1::shared_ptr<T>(t); }
};

template<class T, class G, class D, class S>
class WindowImpl
{
  public:
    typedef T TupleType;
    typedef G PartitionType;
    typedef D DataType;
    typedef S StorageType;
    typedef Window<T, G, D, S> WindowType;
    typedef WindowEvent<T, G, D, S> WindowEventType;
    typedef TupleCountPartitionEvictionImpl<T, G, D, S> TupleCountPartitionEvictionImplType;

    typedef typename S::const_iterator StorageConstIterator;
    typedef typename D::const_iterator DataConstIterator;

    enum resetCause
    {
        /// Reset to initial state internally triggered (via reset())
        INTERNAL,
        /// Reset to initial state externally triggered (via resetToInitialState())
        EXTERNAL
    };

    WindowImpl(WindowType& window)
      : isShutdown_(false)
      , window_(window)
      , data_(window.data_)
      , orderedPartitionsSize_(0)
    {}
    WindowImpl(WindowType& window, PartitionEvictionImpl<T, G, D, S>& partitionEviction)
      : isShutdown_(false)
      , window_(window)
      , data_(window.data_)
      , partitionEviction_(&partitionEviction)
      , orderedPartitionsSize_(0)
    {}
    virtual ~WindowImpl() {}
    virtual void insert(TupleType const& tuple, PartitionType const& partition) = 0;
    virtual void insert(Punctuation const& punct) = 0;
    virtual void createEvictionThread() {}
    virtual void createTriggerThread() {}
    virtual void joinEvictionThread() {}
    virtual void joinTriggerThread() {}

    virtual void shutdown() { isShutdown_ = true; }

    virtual StorageType const& acquireData() const { return this->data_; }

    virtual void releaseData() const {}

    virtual typename DataType::size_type getTupleCount(PartitionType const& partition) const
    {
        return partitionAt(partition).size();
    }

    virtual void drain() {}

    virtual void checkpoint(Checkpoint& ckptStream) const
    {
        AutoWinImplDataAcquirer<T, G, D, S> awda(*this);
        this->doCheckpoint(ckptStream);
        if (window_.onSerialization_ != NULL) {
            window_.onSerialization_->onCheckpointEvent(ckptStream);
        }
    }

    virtual void reset(Checkpoint& ckptStream)
    {
        AutoWinImplDataAcquirer<T, G, D, S> awda(*this);
        this->doResetToInitialState(INTERNAL);
        this->doReset(ckptStream);
        if (window_.onSerialization_ != NULL) {
            window_.onSerialization_->onResetEvent(ckptStream);
        }
    }

    virtual void resetToInitialState()
    {
        AutoWinImplDataAcquirer<T, G, D, S> awda(*this);
        this->doResetToInitialState(EXTERNAL);
        if (window_.onSerialization_ != NULL) {
            window_.onSerialization_->onResetToInitialStateEvent();
        }
    }

    void emitBeforeTupleInsertionEvent(TupleType const& tuple, PartitionType const& partition)
    {
        if (window_.beforeTupleInsertion_) {
            window_.beforeTupleInsertion_->beforeTupleInsertionEvent(window_, tuple, partition);
        }
    }

    void emitAfterTupleInsertionEvent(TupleType& tuple, PartitionType const& partition)
    {
        if (window_.afterTupleInsertion_) {
            window_.afterTupleInsertion_->afterTupleInsertionEvent(window_, tuple, partition);
        }
    }

    void emitAfterTupleInsertionEventForConstTuple(TupleType const& tuple,
                                                   PartitionType const& partition)
    {
        if (window_.afterTupleInsertion_) {
            TupleType ctuple(tuple);
            window_.afterTupleInsertion_->afterTupleInsertionEvent(window_, ctuple, partition);
        }
    }

    DataType& insertPartition(PartitionType const& partition)
    {
        typename StorageType::iterator it = data_.find(partition);
        if (it == data_.end()) {
            DataType d;
            return data_.insert(std::make_pair(partition, d)).first->second;
        } else {
            return it->second;
        }
    }

    void removePartition(PartitionType const& partition)
    {
        typename StorageType::iterator it = data_.find(partition);
        if (it != data_.end()) {
            partitionRemoved(partition);
            data_.erase(it);
        }
    }

    void partitionFlushed(PartitionType const& partition)
    {
        if (partitionEviction_.get()) {
            partitionEviction_->partitionFlushed(window_, partition);
        }
    }

    virtual void partitionEvicted(PartitionType const& partition) {}

    void partitionRemoved(PartitionType const& partition)
    {
        if (partitionEviction_.get()) {
            partitionEviction_->partitionRemoved(window_, partition);
            // remove from orderedPartitionMap_ as well
            typename OrderedPartitionMapType::iterator it = orderedPartitionMap_.find(partition);
            assert(it != orderedPartitionMap_.end());
            orderedPartitions_.erase(it->second);
            --orderedPartitionsSize_;
            orderedPartitionMap_.erase(it);
        }
    }

    void updateOrderedPartitionMap(PartitionType const& partition)
    {
        if (partitionEviction_.get()) {
            // Maintain our time ordered map
            typename OrderedPartitionMapType::iterator it = orderedPartitionMap_.find(partition);
            struct timeval tval;
            gettimeofday(&tval, NULL);
            typename WindowEventType::PartitionOrdering val =
              std::make_pair(partition, tval.tv_sec);
            if (it != orderedPartitionMap_.end()) {
                // remove partition from the current location in the ordered list
                orderedPartitions_.erase(it->second);
                // add to end
                it->second = orderedPartitions_.insert(orderedPartitions_.end(), val);
            } else {
                // first time we have seen this partition
                typename WindowEventType::TimeOrderedPartition::iterator it2 =
                  orderedPartitions_.insert(orderedPartitions_.end(), val);
                ++orderedPartitionsSize_;
                orderedPartitionMap_.insert(std::make_pair(partition, it2));
            }
        }
    }

    // Perform any actions that need to happen before a tuple is inserted
    void beforeTupleInserted()
    {
        // partitionAge requires checking for eviction before inserting a tuple
        // (as insertion updates the partition's age)
        if (partitionEviction_.get() && partitionEviction_->evictBeforeInsertion()) {
            performPartitionEviction();
        }
    }

    void beforeTriggerEventProcessing()
    {
        // partitionAge requires checking for eviction before processing punctuation
        if (partitionEviction_.get() && partitionEviction_->evictBeforeInsertion()) {
            performPartitionEviction();
        }
    }

    void beforePunctInserted()
    {
        // partitionAge requires checking for eviction before processing punctuation
        if (partitionEviction_.get() && partitionEviction_->evictBeforeInsertion()) {
            performPartitionEviction();
        }
    }

    void tupleInserted(PartitionType const& partition, TupleType const& tuple)
    {
        if (partitionEviction_.get()) {
            partitionEviction_->tupleInserted(window_, partition, tuple);
            updateOrderedPartitionMap(partition);
            if (!partitionEviction_->evictBeforeInsertion()) {
                performPartitionEviction();
            }
        }
    }

#ifndef NDEBUG
    /**
     * @brief Generates a hash value of this object.
     *
     * Descendent classes must invoke the base implementation in
     * addition to hashing their own fields.  Note that we don't hash
     * tuple contents.
     */
    virtual size_t hash() const
    {
        // 64-bit FNV offset basis
        size_t result = static_cast<size_t>(14695981039346656037ULL);
        StorageType storage = this->data_;

        for (StorageConstIterator storageIt = storage.begin(); storageIt != storage.end();
             ++storageIt) {
            result ^= std::tr1::hash<PartitionType>()(storageIt->first);
            result ^= std::tr1::hash<size_t>()(storageIt->second.size());
            // XXX We don't hash tuples
        }

        // Hash the ordered partition list:
        result ^= std::tr1::hash<size_t>()(orderedPartitionsSize_);
        for (typename WindowEventType::TimeOrderedPartition::const_iterator it =
               orderedPartitions_.begin();
             it != orderedPartitions_.end(); ++it) {
            result ^= std::tr1::hash<PartitionType>()(it->first);
            result ^= std::tr1::hash<size_t>()(it->second);
        }
        return result;
    }
#endif
    /**
     * @brief Writes a string representation of the object to the specified stream.
     * Descendents should override.
     * @param stream
     */
    virtual void print(std::ostream& stream) const
    {
        stream << "WindowImpl={";
        StorageType storage = this->data_;
        stream << "storage[size=" << static_cast<uint64_t>(storage.size());
        stream << "]={";
        for (StorageConstIterator storageIt = storage.begin(); storageIt != storage.end();
             ++storageIt) {
            PartitionType const partition = storageIt->first;
            stream << "partition=" << partition;
            DataType const& data = storageIt->second;
            stream << ", data[size=" << data.size();
            stream << "]={";
            /* Not printing tuples
            for (DataConstIterator dataIt = data.begin(); dataIt != data.end(); ++dataIt) {
                ckptStream << *dataIt; // *dataIt is of type TupleType const &
            }
            */
            stream << "...}, ";
        }
        stream << "} ";

        // Save the ordered partition list
        stream << "orderedPartitions[size=" << static_cast<uint64_t>(orderedPartitionsSize_)
               << "]={";
        for (typename WindowEventType::TimeOrderedPartition::const_iterator it =
               orderedPartitions_.begin();
             it != orderedPartitions_.end(); ++it) {
            stream << "(" << it->first << ", " << it->second
                   << "), "; // save partition and reference time
        }
        stream << "}}";
    }

    /**
     * @brief Returns a string representation of the window.
     *
     * The result is a textual representation of the window which is easy
     * to read by a person.
     * @return the string representation of the window
     */
    std::string toString() const
    {
        std::ostringstream s;
        this->print(s);
        return s.str();
    }

  protected:
    // Throws std::out_of_range if data_ does not have the specified partition
    DataType& partitionAt(PartitionType const& partition) const
    {
        // TODO C++11 use 'return data_.at(partition)'
        typename StorageType::iterator it = data_.find(partition);
        if (it == data_.end()) {
            std::ostringstream s;
            s << partition;
            throw std::out_of_range(s.str());
        } else {
            return it->second;
        }
    }

    void tupleRemoved(PartitionType const& partition, TupleType const& tuple)
    {
        if (partitionEviction_.get()) {
            partitionEviction_->tupleRemoved(window_, partition, tuple);
        }
    }

    /**
     * Saves internal state to the specified checkpoint stream.
     * Descendent classes should invoke the base implementation from the
     * overridden method.
     *
     * @param ckptStream
     */
    virtual void doCheckpoint(Checkpoint& ckptStream) const
    {
        // Save format:
        // uint64_t=storage.size(), <PartitionType, uint64_t=data.size(), <TupleType>[data.size()]
        // >[storage.size()]

        // Save partition data
        WRITE_MARKER(ckptStream, "WindowImpl");
        ckptStream << this->data_;

        // Save the ordered partition list:
        ckptStream << static_cast<uint64_t>(orderedPartitionsSize_);
        for (typename WindowEventType::TimeOrderedPartition::const_iterator it =
               orderedPartitions_.begin();
             it != orderedPartitions_.end(); ++it) {
            // save partition and reference time
            ckptStream << it->first;
            ckptStream << it->second;
        }

        // Save the partition eviction related state
        if (partitionEviction_.get()) {
            ckptStream << static_cast<uint32_t>(partitionEviction_->getType());
            if (PartitionEvictionImpl<T, G, D, S>::TupleCount == partitionEviction_->getType()) {
                TupleCountPartitionEvictionImplType* evictor =
                  static_cast<TupleCountPartitionEvictionImplType*>(partitionEviction_.get());
                assert(evictor != NULL);
                ckptStream << evictor->getCurrentTupleCount();
            }
        } else {
            ckptStream << static_cast<uint32_t>(PartitionEvictionImpl<T, G, D, S>::None);
        }
    }

    /**
     * Restores internal state from the specified checkpoint stream.
     * Descendent classes should invoke this implementation from the
     * method override.
     *
     * @param ckptStream
     */
    virtual void doReset(Checkpoint& ckptStream)
    {
        // Load data
        VERIFY_MARKER(ckptStream, "WindowImpl");
        ckptStream >> data_;

        // Load the ordered partitions list and update the associated map
        orderedPartitionsSize_ = ckptStream.getUInt64();
        for (uint64_t partitionCount = orderedPartitionsSize_; partitionCount > 0;
             --partitionCount) {
            PartitionType partition;
            ckptStream >> partition;
            struct timeval accessTime;
            ckptStream >> accessTime.tv_sec;
            typename WindowEventType::TimeOrderedPartition::iterator it2 =
              orderedPartitions_.insert(orderedPartitions_.end(),
                                        std::make_pair(partition, accessTime.tv_sec));
            orderedPartitionMap_.insert(std::make_pair(partition, it2));
        }

        // Load partition eviction related state
        if (ckptStream.getUInt32() == PartitionEvictionImpl<T, G, D, S>::TupleCount) {
            TupleCountPartitionEvictionImplType* evictor =
              static_cast<TupleCountPartitionEvictionImplType*>(partitionEviction_.get());
            assert(evictor != NULL);
            evictor->setCurrentTupleCount(ckptStream.getUInt32());
        }
    }

    /**
     * Restores internal state to the initial values.
     * Descendent classes should invoke the base implementation from the
     * overridden method.
     *
     * @param cause determines the cause of the reset
     */
    virtual void doResetToInitialState(int cause)
    {
        deleteWindowTuples();

        typename StorageType::const_iterator it;
        while (this->data_.size() > 0) {
            it = this->data_.begin();
            // Don't make it reference, due to erase in removePartition
            PartitionType const partition = it->first;
            removePartition(partition);
        }

        // Init partition eviction related state
        if (partitionEviction_.get() &&
            PartitionEvictionImpl<T, G, D, S>::TupleCount == partitionEviction_->getType()) {
            TupleCountPartitionEvictionImplType* evictor =
              static_cast<TupleCountPartitionEvictionImplType*>(partitionEviction_.get());
            assert(evictor != NULL);
            evictor->setCurrentTupleCount(0);
        }
    }

    void deleteWindowTuples()
    {
        typename StorageType::const_iterator it;
        for (it = data_.begin(); it != data_.end(); it++) {
            const DataType& data = it->second;
            for (typename DataType::const_iterator it2 = data.begin(); it2 != data.end(); it2++) {
                Allocator<TupleType>::deallocate(*it2);
            }
        }
    }

  private:
    // Perform eviction if needed
    void performPartitionEviction()
    {
        // Any eviction policy to check?
        if (!partitionEviction_.get() || !window_.onWindowPartitionEvictionSelection_) {
            return;
        }

        // Is it currently happy?
        if (orderedPartitionsSize_ == 0 || !partitionEviction_->evictionNeeded(
                                             window_, orderedPartitions_, orderedPartitionsSize_)) {
            return;
        }

        // We need to do something
        doEviction(*window_.onWindowPartitionEvictionSelection_);

        // Was it good enough?  If the user didn't do enough, we force it
        if (orderedPartitionsSize_ > 0 && partitionEviction_->evictionNeeded(
                                            window_, orderedPartitions_, orderedPartitionsSize_)) {
            doEviction(partitionEviction_->LRUPartitionEvictor());
        }
    }

    void doEviction(WindowEventType& ev)
    {
        std::vector<const PartitionType*> partitionsToBeEvicted;
        ev.onWindowPartitionEvictionSelection(window_,
                                              typename WindowEventType::PartitionSelectionIterator(
                                                partitionsToBeEvicted, orderedPartitions_.begin()),
                                              typename WindowEventType::PartitionSelectionIterator(
                                                partitionsToBeEvicted, orderedPartitions_.end()));
        // Tell everyone that we will be removing these
        TimeOrderedPartition partitionsToBeRemoved;

        if (!partitionsToBeEvicted.empty() /*&& window_.onWindowPartitionEviction_ */) {
            // Copy information from partitionsToBeEvicted to partitionsToBeRemoved
            typename std::vector<const PartitionType*>::const_iterator it;
            for (it = partitionsToBeEvicted.begin(); it != partitionsToBeEvicted.end(); it++) {
                const PartitionType& p = **it;
                typename OrderedPartitionMapType::iterator it2 = orderedPartitionMap_.find(p);
                if (it2 != orderedPartitionMap_.end()) {
                    // grab the time from the existing entry
                    typename WindowEventType::PartitionOrdering val =
                      std::make_pair(p, it2->second->second);
                    partitionsToBeRemoved.push_back(val);
                }
            }
            if (window_.onWindowPartitionEviction_) {
                window_.onWindowPartitionEviction_->onWindowPartitionEviction(
                  window_, partitionsToBeRemoved.begin(), partitionsToBeRemoved.end());
            }
        }

        // Now remove them
        for (typename TimeOrderedPartition::const_iterator it = partitionsToBeRemoved.begin();
             it != partitionsToBeRemoved.end(); it++) {
            PartitionType const& partition = it->first;
            removePartition(partition);
            // Let the window handle window specific actions,
            // such as removal from the window full map
            partitionEvicted(partition);
            APPTRC(L_DEBUG, "Partition " << partition << " was removed",
                   SPL_WINDOW_DBG); // TODO log operator name / port
        }
    }

  protected:
    boost::atomic<bool> isShutdown_;
    WindowType& window_;
    StorageType& data_;
    std::auto_ptr<PartitionEvictionImpl<T, G, D, S> > partitionEviction_;
    std::auto_ptr<PartitionEvictionPolicy> partionEvictionPolicy_;
    typedef typename WindowEventType::TimeOrderedPartition TimeOrderedPartition;
    typedef std::tr1::unordered_map<PartitionType, typename TimeOrderedPartition::iterator>
      OrderedPartitionMapType;

  private:
    TimeOrderedPartition orderedPartitions_;
    OrderedPartitionMapType orderedPartitionMap_;
    // C++11
    // orderedPartitionsSize_ should be the same value as
    // orderedPartitions_.size().  The performance of
    // std::list::size() is linear, and the size of the list is
    // needed on a performance-critical path, so the size is
    // tracked separately.  In C++ 11 std::list::size() is
    // guaranteed to be constant performance, so we can remove
    // this and just use orderedPartitions_.size() when we
    // move to C++ 11.
    typename TimeOrderedPartition::size_type orderedPartitionsSize_;
};

/*
 * RAII for WindowImpl data access.
 */
template<class T, class G, class D, class S>
class AutoWinImplDataAcquirer
{
  public:
    typedef T TupleType;
    typedef G PartitionType;
    typedef D DataType;
    typedef S StorageType;
    typedef WindowImpl<T, G, D, S> WindowType;
    /// Constructor
    /// @param window window whose data is to be acquired
    AutoWinImplDataAcquirer(WindowType const& window)
      : window_(window)
    {
        window_.acquireData();
    }
    /// Destructor
    ~AutoWinImplDataAcquirer() { window_.releaseData(); }
    /// Access window data
    StorageType const& getWindowStorage() const { return window_.getWindowStorage(); }

  private:
    WindowType const& window_;
};
};

#endif /* SPL_RUNTIME_WINDOW_IMPL_WINDOW_IMPL_H */
