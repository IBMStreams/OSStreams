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

#ifndef SPL_RUNTIME_WINDOW_PARTITION_EVICTION_IMPL_H
#define SPL_RUNTIME_WINDOW_PARTITION_EVICTION_IMPL_H

#include <SPL/Runtime/Window/AgeLRUPartitionEvictionImpl.h>
#include <SPL/Runtime/Window/PartitionAgePartitionEvictionPolicy.h>
#include <SPL/Runtime/Window/PartitionCountLRUPartitionEvictionImpl.h>
#include <SPL/Runtime/Window/PartitionCountPartitionEvictionPolicy.h>
#include <SPL/Runtime/Window/TupleCountLRUPartitionEvictionImpl.h>
#include <SPL/Runtime/Window/TupleCountPartitionEvictionPolicy.h>
#include <SPL/Runtime/Window/Window.h>

#include <list>
#include <memory>
#include <sys/time.h>
#include <tr1/unordered_map>
#include <utility>

namespace SPL {
/// Class that represents when partitions should be evicted from the window
template<class T,
         class G = int32_t,
         class D = std::deque<T>,
         class S = std::tr1::unordered_map<G, D> >
class PartitionAgePartitionEvictionImpl;
template<class T,
         class G = int32_t,
         class D = std::deque<T>,
         class S = std::tr1::unordered_map<G, D> >
class PartitionCountPartitionEvictionImpl;
template<class T,
         class G = int32_t,
         class D = std::deque<T>,
         class S = std::tr1::unordered_map<G, D> >
class TupleCountPartitionEvictionImpl;
template<class T,
         class G = int32_t,
         class D = std::deque<T>,
         class S = std::tr1::unordered_map<G, D> >
class PartitionEvictionImpl
{
  public:
    typedef T TupleType;     //!< tuple type
    typedef G PartitionType; //!< partition type
    typedef D DataType;      //!< data type
    typedef S StorageType;   //!< storage type
    typedef Window<T, G, D, S> WindowType;
    typedef WindowEvent<T, G, D, S> WindowEventType;

    typedef typename WindowEventType::TimeType TimeType;
    typedef typename WindowEventType::PartitionOrdering PartitionOrdering;
    typedef typename WindowEventType::TimeOrderedPartition TimeOrderedPartition;

    /// Type of the eviction policy
    enum Type
    {
        None = 0,           //!< No partition eviction policy
        PartitionAge = 1,   //!< Partition not updated recently
        PartitionCount = 2, //!< Too many partitions
        TupleCount = 3      //!< Too many tuples
    };

    /// Constructor
    ///
    PartitionEvictionImpl(Type type)
      : type_(type)
    {}

    /// Destructor
    ///
    virtual ~PartitionEvictionImpl() {}

    /// Get the implementation type of the eviction policy
    /// @return type of the eviction policy
    Type getType() const { return type_; }

    /// Should the partition eviction for this policy be done before insertion
    /// @return Returns true if this eviction policy should be applied before tuple insertion,
    /// otherwise false.
    bool evictBeforeInsertion() const { return type_ == PartitionAge; }

    /// Partition flushed
    /// @param window Window being managed
    /// @param partition Partition being evicted (flushed) by the Window
    /// @param data data in the partition being removed
    virtual void partitionFlushed(WindowType const& window, PartitionType const& partition) {}

    /// Partition removed
    /// @param window Window being managed
    /// @param partition Partition being removed by the Window
    /// @param data data in the partition being removed
    virtual void partitionRemoved(WindowType const& window, PartitionType const& partition) {}

    /// Tuple inserted into window
    /// @param window Window being managed
    /// @param partition Partition being removed by the Window
    /// @param tuple data in the partition being inserted
    virtual void tupleInserted(WindowType const& window,
                               PartitionType const& partition,
                               TupleType const& tuple)
    {}

    /// Tuple removed from window
    /// @param window Window being managed
    /// @param partition Partition being removed by the Window
    /// @param tuple data in the partition being removed
    virtual void tupleRemoved(WindowType const& window,
                              PartitionType const& partition,
                              TupleType const& tuple)
    {}

    /// Do partitions need to be evicted from the window?
    /// @param window Window being managed
    /// @param partitions list of partitions in the window ordered by last use
    /// @return 'true' if one or more partitions need to be removed from the Window
    virtual bool evictionNeeded(WindowType const& window,
                                TimeOrderedPartition const& partitions,
                                typename TimeOrderedPartition::size_type partitionsCount) = 0;

    /// Create the proper implementation corresponding to the policy.
    static PartitionEvictionImpl& create(const PartitionEvictionPolicy& policy, WindowType& window)
    {
        PartitionEvictionPolicy::When when = policy.getWhen();
        WindowEventType* selector = NULL;

        // Always create a LRU policy
        if (when == PartitionEvictionPolicy::PartitionAge) {
            double age = static_cast<const PartitionAgePartitionEvictionPolicy&>(policy).getAge();
            selector = new AgeLRUPartitionEvictionImpl<T, G, D, S>(age);
        } else if (when == PartitionEvictionPolicy::PartitionCount) {
            uint32_t count =
              static_cast<const PartitionCountPartitionEvictionPolicy&>(policy).getCount();
            selector = new PartitionCountLRUPartitionEvictionImpl<T, G, D, S>(count);
        } else {
            selector = new TupleCountLRUPartitionEvictionImpl<T, G, D, S>();
        }

        PartitionEvictionImpl* result = NULL;

        switch (when) {
            case PartitionEvictionPolicy::PartitionAge: {
                double age =
                  static_cast<const PartitionAgePartitionEvictionPolicy&>(policy).getAge();
                result = new PartitionAgePartitionEvictionImpl<T, G, D, S>(age);
                break;
            }
            case PartitionEvictionPolicy::PartitionCount: {
                uint32_t count =
                  static_cast<const PartitionCountPartitionEvictionPolicy&>(policy).getCount();
                result = new PartitionCountPartitionEvictionImpl<T, G, D, S>(count);
                break;
            }
            case PartitionEvictionPolicy::TupleCount: {
                uint32_t count =
                  static_cast<const TupleCountPartitionEvictionPolicy&>(policy).getCount();
                result = new TupleCountPartitionEvictionImpl<T, G, D, S>(count);
                break;
            }
        }

        // Save for use later
        result->selector_.reset(selector);
        if (policy.getHow() == PartitionEvictionPolicy::LRU) {
            window.registerOnWindowPartitionEvictionSelection(selector);
        }
        return *result;
    }

    /// Return the default LRU partition evictor.
    /// @return proper LRU paritition evictor
    WindowEventType& LRUPartitionEvictor() const
    {
        assert(selector_.get() != NULL);
        return *selector_;
    }

  private:
    Type const type_;
    std::auto_ptr<WindowEventType> selector_;
};
}

#endif /* SPL_RUNTIME_WINDOW_PARTITION_EVICTION_IMPL_H */
