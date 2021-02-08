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

#ifndef SPL_RUNTIME_WINDOW_SLIDING_WINDOW_H
#define SPL_RUNTIME_WINDOW_SLIDING_WINDOW_H

/*!
 * \file SlidingWindow.h \brief Definition of the SPL::SlidingWindow class.
 */

#include <SPL/Runtime/Window/WindowCommon.h>
#include <SPL/Runtime/Window/WindowPolicy.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Window/WindowImpl.h>
#include <SPL/Runtime/Window/PartitionAgePartitionEvictionImpl.h>
#include <SPL/Runtime/Window/PartitionCountPartitionEvictionImpl.h>
#include <SPL/Runtime/Window/TupleCountPartitionEvictionImpl.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
    class Operator;

    template <class T, class G, class D, class S>
    class SlidingWindowImpl;

    /// @ingroup Window
    /// @brief Class that represents a sliding window.
    ///
    /// Example: \c SlidingWindow<TupleType, PartitionType, StorageType>
    ///
    /// \c TupleType is the type of the tuples stored in the window (can be a
    /// pointer type or a non-pointer type). \c PartitionType, which is
    /// optional, is the type of the window partition specifier, and the \c
    /// StorageType, which is optional, is the internal data structure used for
    /// storing tuples (which maps partitions to a sequence of tuples e.g.: \c
    /// unordered_map<PartitionType, vector<TupleType> >).
    ///
    /// A sliding window operates by collecting tuples until a full window is
    /// reached, and then maintaining the full window at all times as new tuples
    /// are inserted, by evicting the oldest tuples from the window.
    ///
    /// For a detailed description of operational semantics for sliding windows,
    /// see window policy classes used to configure them: CountWindowPolicy,
    /// DeltaWindowPolicy, TimeWindowPolicy.
    ///
    /// Sliding widows allow registering event handlers for the following
    /// events defined in \c WindowEvent:
    ///   - \c beforeTupleInsertionEvent
    ///   - \c afterTupleInsertionEvent
    ///   - \c beforeTupleEvictionEvent
    ///   - \c afterTupleEvictionEvent
    ///   - \c windowTriggerEvent
    ///   - \c windowInitialFullEvent
    ///   - \c windoPartitionEvictionSelection
    ///   - \c windowPartitionEviction
    template <class T, class G=int32_t, class D=std::deque<T>,
              class S=std::tr1::unordered_map<G,D> >
    class SlidingWindow : public Window<T,G,D,S> {
        typedef Window<T,G,D,S> BaseClass;
    public:
        typedef typename BaseClass::TupleType TupleType; //!< tuple type
        typedef typename BaseClass::PartitionType PartitionType; //!< partition type
        typedef typename BaseClass::StorageType StorageType; //!< storage type
        typedef typename BaseClass::DataType DataType; //!< data type
        typedef typename BaseClass::WindowEventType WindowEventType; //!< window event type

        /// Constructor
        /// @param oper operator that holds this window
        /// @param port operator input port index for this window
        /// @param evictionPolicy eviction policy of the window
        /// @param triggerPolicy trigger policy of the window
        /// @throws SPLRuntimeInvalidSlidingWindowException if an invalid
        /// configuration is specified
        SlidingWindow(Operator & oper, uint32_t port,
                      WindowPolicy const & evictionPolicy,
                      WindowPolicy const & triggerPolicy);

        /// Constructor
        /// @param oper operator that holds this window
        /// @param port operator input port index for this window
        /// @param evictionPolicy eviction policy of the window
        /// @param triggerPolicy trigger policy of the window
        /// @param partitionEvictionPolicy partition eviction policy of the window
        /// @throws SPLRuntimeInvalidSlidingWindowException if an invalid
        /// configuration is specified
        SlidingWindow(Operator & oper, uint32_t port,
                      WindowPolicy const & evictionPolicy,
                      WindowPolicy const & triggerPolicy,
                      PartitionEvictionPolicy const& partitionEvictionPolicy);

        /// Destructor
        ///
        virtual ~SlidingWindow() {}

        /// Get the data associated with this window. If this window involves
        /// eviction or trigger policies that are time based, appropriate
        /// locking is performed. This call should be followed by a \c
        /// releaseData() call for releasing the lock in these cases. See
        /// SPL::AutoWindowDataAcquirer for safer, RAII based access to the
        /// contents of the window.
        /// @return contents of the window
        StorageType const & acquireData() const;

        /// Release the window data
        ///
        void releaseData() const;

        /// Insert a tuple into the window
        /// @param tuple tuple to be inserted
        /// @param partition the partition the \a tuple belongs to (optional)
        void insert(TupleType const & tuple, PartitionType const & partition=PartitionType());

        /// Notify the window about the existence of a punctuation in the stream
        /// @param punct the punctuation that is received
        void insert(Punctuation const & punct);

        /// @copydoc Window#drain()
        void drain();

        /// @copydoc Window#checkpoint(Checkpoint&)
        void checkpoint(Checkpoint & data) const;

        /// @copydoc Window#reset(Checkpoint&)
        void reset(Checkpoint & data);

        /// @copydoc Window#resetToInitialState()
        void resetToInitialState();

        /// Get the eviction policy for the window
        /// @return the window eviction policy
        WindowPolicy const & getEvictionPolicy() const
        {
            return *evict_;
        }

        /// Get the trigger policy for the window
        /// @return the window trigger policy
        WindowPolicy const & getTriggerPolicy() const
        {
            return *trigger_;
        }

        /// Register a handler for window trigger event
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any
        /// @return the old event handler, or NULL if none was found
        WindowEventType * registerOnWindowTriggerHandler(WindowEventType * e)
        {
            WindowEventType * o = onWindowTrigger_;
            onWindowTrigger_ = e;
            return o;
        }

        /// Register a handler for window initial full event
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any
        /// @return the old event handler, or NULL if none was found
        WindowEventType * registerOnWindowInitialFullHandler(WindowEventType * e)
        {
            WindowEventType * o = onWindowInitialFull_;
            onWindowInitialFull_ = e;
            return o;
        }

        /// Register a handler for before tuple eviction event
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any
        /// @return the old event handler, or NULL if none was found
        WindowEventType * registerBeforeTupleEvictionHandler(WindowEventType * e)
        {
            WindowEventType * o = beforeTupleEviction_;
            beforeTupleEviction_ = e;
            return o;
        }

        /// Register a handler for after tuple eviction event
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any
        /// @return the old event handler, or NULL if none was found
        WindowEventType * registerAfterTupleEvictionHandler(WindowEventType * e)
        {
            WindowEventType * o = afterTupleEviction_;
            afterTupleEviction_ = e;
            return o;
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
    protected:
        void join();
        void start();
        void shutdown();
        void print(std::ostream& ostream) const;

    protected:
        std::auto_ptr<WindowImpl<T,G,D,S> > impl_;
        std::auto_ptr<WindowPolicy> evict_;
        std::auto_ptr<WindowPolicy> trigger_;
        WindowEventType * onWindowTrigger_;
        WindowEventType * onWindowInitialFull_;
        WindowEventType * beforeTupleEviction_;
        WindowEventType * afterTupleEviction_;

        friend class SlidingWindowImpl<T,G,D,S>;
#endif
    };
};

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Window/SlidingWindowImpl.h>
#include <SPL/Runtime/Window/CountCountSlidingWindowImpl.h>
#include <SPL/Runtime/Window/CountDeltaSlidingWindowImpl.h>
#include <SPL/Runtime/Window/DeltaCountSlidingWindowImpl.h>
#include <SPL/Runtime/Window/DeltaDeltaSlidingWindowImpl.h>
#include <SPL/Runtime/Window/CountTimeSlidingWindowImpl.h>
#include <SPL/Runtime/Window/DeltaTimeSlidingWindowImpl.h>
#include <SPL/Runtime/Window/TimeCountSlidingWindowImpl.h>
#include <SPL/Runtime/Window/TimeDeltaSlidingWindowImpl.h>
#include <SPL/Runtime/Window/TimeTimeSlidingWindowImpl.h>

namespace SPL {

    template <class T, class G, class D, class S>
    SlidingWindow<T,G,D,S>::SlidingWindow(Operator & oper, uint32_t port,
                                          WindowPolicy const & evict,
                                          WindowPolicy const & trigger)
        : Window<T,G,D,S>(oper, port),
          evict_(evict.clone()),
          trigger_(trigger.clone()),
          onWindowTrigger_(NULL),
          onWindowInitialFull_(NULL),
          beforeTupleEviction_(NULL),
          afterTupleEviction_(NULL)
    {
        WindowPolicy::Type ewpt = evict.getType();
        WindowPolicy::Type twpt = trigger.getType();
        if(ewpt==WindowPolicy::Punct) {
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidSlidingWindow, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_SLIDING_WINDOW_WITH_PUNCT_EVICTION, SPL_WINDOW_DBG);
        }
        if(twpt==WindowPolicy::Punct) {
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidSlidingWindow, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_SLIDING_WINDOW_WITH_PUNCT_TRIGGER, SPL_WINDOW_DBG);
        }
        switch(ewpt)
        {
            case WindowPolicy::Count :
            {
                switch(twpt)
                {
                    case WindowPolicy::Count :
                        impl_.reset(new CountCountSlidingWindowImpl<T,G,D,S>(*this));
                        break;
                    case WindowPolicy::Delta :
                        impl_.reset(new CountDeltaSlidingWindowImpl<T,G,D,S>(*this));
                        break;
                    case WindowPolicy::Time  :
                        impl_.reset(new CountTimeSlidingWindowImpl<T,G,D,S>(*this,this->oper_));
                        break;
                    case WindowPolicy::Punct :
                        assert(!"cannot use WindowPolicy::Punct");
                        break;
                    case WindowPolicy::EventTime :
                        assert(!"cannot use WindowPolicy::EventTime");
                        break;
                }
                break;
            }
            case WindowPolicy::Delta :
            {
                switch(twpt)
                {
                    case WindowPolicy::Count :
                        impl_.reset(new DeltaCountSlidingWindowImpl<T,G,D,S>(*this));
                        break;
                    case WindowPolicy::Delta :
                        impl_.reset(new DeltaDeltaSlidingWindowImpl<T,G,D,S>(*this));
                        break;
                    case WindowPolicy::Time  :
                        impl_.reset(new DeltaTimeSlidingWindowImpl<T,G,D,S>(*this,this->oper_));
                        break;
                    case WindowPolicy::Punct :
                        assert(!"cannot use WindowPolicy::Punct");
                        break;
                    case WindowPolicy::EventTime :
                        assert(!"cannot use WindowPolicy::EventTime");
                        break;
                }
                break;
            }
            case WindowPolicy::Time  :
            {
                switch(twpt)
                {
                    case WindowPolicy::Count :
                        impl_.reset(new TimeCountSlidingWindowImpl<T,G,D,S>(*this,this->oper_));
                        break;
                    case WindowPolicy::Delta :
                        impl_.reset(new TimeDeltaSlidingWindowImpl<T,G,D,S>(*this,this->oper_));
                        break;
                    case WindowPolicy::Time  :
                        impl_.reset(new TimeTimeSlidingWindowImpl<T,G,D,S>(*this,this->oper_));
                        break;
                    case WindowPolicy::Punct :
                        assert(!"cannot use WindowPolicy::Punct");
                        break;
                    case WindowPolicy::EventTime :
                        assert(!"cannot use WindowPolicy::EventTime");
                        break;
                }
                break;
            }
            case WindowPolicy::Punct :
                assert(!"cannot use WindowPolicy::Punct");
                break;
            case WindowPolicy::EventTime :
                assert(!"cannot use WindowPolicy::EventTime");
                break;
       }
    }

    template <class T, class G, class D, class S>
    SlidingWindow<T,G,D,S>::SlidingWindow(Operator & oper, uint32_t port,
                                          WindowPolicy const & evict,
                                          WindowPolicy const & trigger,
                                          PartitionEvictionPolicy const& partitionEvictionPolicy)
        : Window<T,G,D,S>(oper, port, partitionEvictionPolicy),
          evict_(evict.clone()),
          trigger_(trigger.clone()),
          onWindowTrigger_(NULL),
          onWindowInitialFull_(NULL),
          beforeTupleEviction_(NULL),
          afterTupleEviction_(NULL)
    {
        WindowPolicy::Type ewpt = evict.getType();
        WindowPolicy::Type twpt = trigger.getType();
        if(ewpt==WindowPolicy::Punct) {
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidSlidingWindow, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_SLIDING_WINDOW_WITH_PUNCT_EVICTION, SPL_WINDOW_DBG);
        }
        if(twpt==WindowPolicy::Punct) {
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidSlidingWindow, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_SLIDING_WINDOW_WITH_PUNCT_TRIGGER, SPL_WINDOW_DBG);
        }
        PartitionEvictionImpl<T,G,D,S>& pEP = PartitionEvictionImpl<T,G,D,S>::create (partitionEvictionPolicy, *this);
        switch(ewpt)
        {
            case WindowPolicy::Count :
            {
                switch(twpt)
                {
                    case WindowPolicy::Count :
                        impl_.reset(new CountCountSlidingWindowImpl<T,G,D,S>(*this, pEP));
                        break;
                    case WindowPolicy::Delta :
                        impl_.reset(new CountDeltaSlidingWindowImpl<T,G,D,S>(*this, pEP));
                        break;
                    case WindowPolicy::Time  :
                        impl_.reset(new CountTimeSlidingWindowImpl<T,G,D,S>(*this,this->oper_, pEP));
                        break;
                    case WindowPolicy::Punct :
                        assert(!"cannot use WindowPolicy::Punct");
                        break;
                    case WindowPolicy::EventTime :
                        assert(!"cannot use WindowPolicy::EventTime");
                        break;
                }
                break;
            }
            case WindowPolicy::Delta :
            {
                switch(twpt)
                {
                    case WindowPolicy::Count :
                        impl_.reset(new DeltaCountSlidingWindowImpl<T,G,D,S>(*this, pEP));
                        break;
                    case WindowPolicy::Delta :
                        impl_.reset(new DeltaDeltaSlidingWindowImpl<T,G,D,S>(*this, pEP));
                        break;
                    case WindowPolicy::Time  :
                        impl_.reset(new DeltaTimeSlidingWindowImpl<T,G,D,S>(*this,this->oper_, pEP));
                        break;
                    case WindowPolicy::Punct :
                        assert(!"cannot use WindowPolicy::Punct");
                        break;
                    case WindowPolicy::EventTime :
                        assert(!"cannot use WindowPolicy::EventTime");
                        break;
                }
                break;
            }
            case WindowPolicy::Time  :
            {
                switch(twpt)
                {
                    case WindowPolicy::Count :
                        impl_.reset(new TimeCountSlidingWindowImpl<T,G,D,S>(*this,this->oper_, pEP));
                        break;
                    case WindowPolicy::Delta :
                        impl_.reset(new TimeDeltaSlidingWindowImpl<T,G,D,S>(*this,this->oper_, pEP));
                        break;
                    case WindowPolicy::Time  :
                        impl_.reset(new TimeTimeSlidingWindowImpl<T,G,D,S>(*this,this->oper_, pEP));
                        break;
                    case WindowPolicy::Punct :
                        assert(!"cannot use WindowPolicy::Punct");
                        break;
                    case WindowPolicy::EventTime :
                        assert(!"cannot use WindowPolicy::EventTime");
                        break;
                }
                break;
            }
            case WindowPolicy::Punct :
                assert(!"cannot use WindowPolicy::Punct");
                break;
            case WindowPolicy::EventTime :
                assert(!"cannot use WindowPolicy::EventTime");
                break;
        }
    }

    template <class T, class G, class D, class S>
    void SlidingWindow<T,G,D,S>::join()
    {
        impl_->joinEvictionThread();
        impl_->joinTriggerThread();
    }

    template <class T, class G, class D, class S>
    void SlidingWindow<T,G,D,S>::start()
    {
        impl_->createEvictionThread();
        impl_->createTriggerThread();
    }

    template <class T, class G, class D, class S>
    void SlidingWindow<T,G,D,S>::shutdown()
    {
        impl_->shutdown();
    }

    template <class T, class G, class D, class S>
    typename SlidingWindow<T,G,D,S>::StorageType const & SlidingWindow<T,G,D,S>::acquireData() const
    {
        impl_->acquireData();
        return this->data_;
    }

    template <class T, class G, class D, class S>
    void SlidingWindow<T,G,D,S>::releaseData() const
    {
        impl_->releaseData();
    }

    template <class T, class G, class D, class S>
    void SlidingWindow<T,G,D,S>::insert(TupleType const & tuple, PartitionType const & partition)
    {
        impl_->insert(tuple, partition);
    }

    template <class T, class G, class D, class S>
    void SlidingWindow<T,G,D,S>::insert(Punctuation const & punct)
    {
        impl_->insert(punct);
    }

    template <class T, class G, class D, class S>
    void SlidingWindow<T,G,D,S>::drain()
    {
        impl_->drain();
    }

    template <class T, class G, class D, class S>
    void SlidingWindow<T,G,D,S>::checkpoint(Checkpoint & data) const
    {
        impl_->checkpoint(data);
    }

    template <class T, class G, class D, class S>
    void SlidingWindow<T,G,D,S>::reset(Checkpoint & data)
    {
        impl_->reset(data);
    }

    template <class T, class G, class D, class S>
    void SlidingWindow<T,G,D,S>::resetToInitialState()
    {
        impl_->resetToInitialState();
    }

    template <class T, class G, class D, class S>
    void SlidingWindow<T,G,D,S>::print(std::ostream& s) const
    {
        impl_->print(s); // TODO add WindowPolicy::toString()
    }
};

#endif
POP_DLL_PUBLIC

#endif /* SPL_RUNTIME_WINDOW_SLIDING_WINDOW_H */
