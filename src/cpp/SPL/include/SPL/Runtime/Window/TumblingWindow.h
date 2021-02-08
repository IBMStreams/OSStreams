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

#ifndef SPL_RUNTIME_WINDOW_TUMBLING_WINDOW_H
#define SPL_RUNTIME_WINDOW_TUMBLING_WINDOW_H

/*!
 * \file TumblingWindow.h \brief Definition of the SPL::TumblingWindow class.
 */

#include <SPL/Runtime/Window/WindowCommon.h>
#include <SPL/Runtime/Window/WindowPolicy.h>
#include <SPL/Runtime/Window/TumblingWindowSummarizer.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <boost/static_assert.hpp>
#include <SPL/Runtime/Window/WindowImpl.h>
#include <SPL/Runtime/Window/TumblingWindowSummarizerFactory.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
    template <class T, class G, class D, class S>
    class TumblingWindowImpl;

    /// @ingroup Window
    /// @brief Class that represents a tumbling window.
    ///
    /// Example: \c TumblingWindow<TupleType, PartitionType, StorageType>
    ///
    /// \c TupleType is the type of the tuples stored in the window (can be a
    /// pointer type or a non-pointer type). \c PartitionType, which is
    /// optional, is the type of the window partition specifier, and the \c
    /// StorageType, which is optional, is the internal data structure used for
    /// storing tuples (which maps partitions to a sequence of tuples e.g.: \c
    /// unordered_map<PartitionType, vector<TupleType> >).
    ///
    /// A tumbling window operates by collecting tuples until a full window is
    /// reached, and then flushing the window at once. Unlike sliding windows,
    /// there are no 'trigger' or 'eviction' events assocaited with tumbling
    /// windows, as both of these events are represented by a 'flush' event.
    ///
    /// For a detailed description of operational semantics for tumbling
    /// windows, see window policy classes used to configure them:
    /// CountWindowPolicy, DeltaWindowPolicy, TimeWindowPolicy,
    /// PunctWindowPolicy.
    ///
    /// Tumbling widows allow registering event handlers for the following
    /// events defined in WindowEvent:
    /// - \c beforeTupleInsertionEvent
    /// - \c afterTupleInsertionEvent
    /// - \c beforeWindowFlushEvent
    /// - \c afterWindowFlushEvent
    /// - \c onEmptyWindowPunctEvent
    /// - \c onWindowPartitionEvictionSelection
    /// - \c onWindowPartitionEviction
    template <class T, class G=int32_t, class D=std::deque<T>,
              class S=std::tr1::unordered_map<G,D> >
    class TumblingWindow : public Window<T,G,D,S>
    {
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
        /// @throws SPLRuntimeInvalidSlidingWindowException if an invalid
        /// configuration is specified
        TumblingWindow(Operator & oper, uint32_t port,
                       WindowPolicy const & evictionPolicy);

        /// Constructor
        /// @param oper operator that holds this window
        /// @param port operator input port index for this window
        /// @param evictionPolicy eviction policy of the window
        /// @param partitionEvictionPolicy partition eviction policy of the window
        /// @throws SPLRuntimeInvalidSlidingWindowException if an invalid
        /// configuration is specified
        TumblingWindow(Operator & oper, uint32_t port,
                       WindowPolicy const & evictionPolicy,
                       PartitionEvictionPolicy const& partitionEvictionPolicy);

        /// Destructor
        ///
        virtual ~TumblingWindow();

        /// Register a tumbling window summarizer with this window. If a
        /// summarizer is registered, then the window does not store the tuples
        /// in the window, but instead, keeps summarizers, one per active
        /// partition. The \c Summarizer class must be derived from \c
        /// TumblingWindowSummarizer<T,G> or else a compile-time error will be
        /// flagged.
        /// @note If a window summarizer is to be registered, then this method
        /// should be called right after the constructor, once.
        template<class Summarizer>
        void registerWindowSummarizer();

        /// Retrieve the summarizer associated with a given partition
        /// @param partition the partition the summarizer belongs to (optional)
        /// @return NULL if nonexistent, the summarizer otherwise
        TumblingWindowSummarizer<T,G> * getSummarizer(PartitionType const & partition = PartitionType());

        /// Get the data associated with this window. If this window involves
        /// eviction or trigger policies that are time based, appropriate
        /// locking is performed. This call should be followed by a
        /// \c releaseData() call for releasing the lock in these cases. See
        /// SPL::AutoWindowDataAcquirer for a safer, RAII based access to the
        /// contents of the window.
        /// @return contents of the window
        StorageType const & acquireData() const;

        /// Release the window data
        ///
        void releaseData() const;

	      /// @copydoc Window#getTupleCount(PartitionType const&)
        typename DataType::size_type getTupleCount(PartitionType const & partition) const
        { return impl_->getTupleCount(partition); }

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

        /// Register a handler for before window flush event
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any
        /// @return the old event handler, or NULL if none was found
        WindowEventType * registerBeforeWindowFlushHandler(WindowEventType * e)
        {
            WindowEventType * o = beforeWindowFlush_;
            beforeWindowFlush_ = e;
            return o;
        }

        /// Register a handler for after window flush event
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any
        /// @return the old event handler, or NULL if none was found
        WindowEventType *  registerAfterWindowFlushHandler(WindowEventType * e)
        {
            WindowEventType * o = afterWindowFlush_;
            afterWindowFlush_ = e;
            return o;
        }

        /// Register a handler for on empty window punct event
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any
        /// @return the old event handler, or NULL if none was found
        WindowEventType *  registerOnEmptyWindowPunctEvent(WindowEventType * e)
        {
            WindowEventType * o = onEmptyWindowPunct_;
            onEmptyWindowPunct_ = e;
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
        WindowEventType * afterWindowFlush_;
        WindowEventType * beforeWindowFlush_;
        WindowEventType * onEmptyWindowPunct_;
        TumblingWindowSummarizerFactory<T, G> * summarizerFactory_;

        friend class TumblingWindowImpl<T,G,D,S>;
#endif
    };
};

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Window/TumblingWindowImpl.h>
#include <SPL/Runtime/Window/CountTumblingWindowImpl.h>
#include <SPL/Runtime/Window/DeltaTumblingWindowImpl.h>
#include <SPL/Runtime/Window/TimeTumblingWindowImpl.h>
#include <SPL/Runtime/Window/PunctTumblingWindowImpl.h>

namespace SPL {

    template <class T, class G, class D, class S>
    TumblingWindow<T,G,D,S>::TumblingWindow(Operator & oper, uint32_t port,
                                          WindowPolicy const & evict)
        : Window<T,G,D,S>(oper, port),
          evict_(evict.clone()),
          afterWindowFlush_(NULL),
          beforeWindowFlush_(NULL),
          onEmptyWindowPunct_(NULL),
          summarizerFactory_(NULL)
    {
        WindowPolicy::Type wtype = evict.getType();
        switch(wtype)
        {
            case WindowPolicy::Count :
                impl_.reset(new CountTumblingWindowImpl<T,G,D,S>(*this));
                 break;
            case WindowPolicy::Delta :
                impl_.reset(new DeltaTumblingWindowImpl<T,G,D,S>(*this));
                break;
            case WindowPolicy::Punct :
                impl_.reset(new PunctTumblingWindowImpl<T,G,D,S>(*this));
                break;
            case WindowPolicy::Time  :
                impl_.reset(new TimeTumblingWindowImpl<T,G,D,S>(*this,this->oper_));
                break;
            case WindowPolicy::EventTime :
                assert(!"cannot use WindowPolicy::EventTime");
                break;
        }
    }

    template <class T, class G, class D, class S>
    TumblingWindow<T,G,D,S>::TumblingWindow(Operator & oper, uint32_t port,
                                            WindowPolicy const & evict,
                                            PartitionEvictionPolicy const& partitionEvictionPolicy)
        : Window<T,G,D,S>(oper, port, partitionEvictionPolicy),
          evict_(evict.clone()),
          afterWindowFlush_(NULL),
          beforeWindowFlush_(NULL),
          onEmptyWindowPunct_(NULL),
          summarizerFactory_(NULL)
    {
        WindowPolicy::Type wtype = evict.getType();
        PartitionEvictionImpl<T,G,D,S>& pEP =
            PartitionEvictionImpl<T,G,D,S>::create (partitionEvictionPolicy, *this);
        switch(wtype)
        {
            case WindowPolicy::Count :
                impl_.reset(new CountTumblingWindowImpl<T,G,D,S>(*this, pEP));
                 break;
            case WindowPolicy::Delta :
                impl_.reset(new DeltaTumblingWindowImpl<T,G,D,S>(*this, pEP));
                break;
            case WindowPolicy::Punct :
                impl_.reset(new PunctTumblingWindowImpl<T,G,D,S>(*this, pEP));
                break;
            case WindowPolicy::Time  :
                impl_.reset(new TimeTumblingWindowImpl<T,G,D,S>(*this,this->oper_, pEP));
                break;
            case WindowPolicy::EventTime :
                assert(!"cannot use WindowPolicy::EventTime");
                break;
        }
    }

    template <class T, class G, class D, class S>
    TumblingWindow<T,G,D,S>::~TumblingWindow()
    {
        delete summarizerFactory_;
    }

    template <class T, class G, class D, class S>
    template <class Summarizer>
    void TumblingWindow<T,G,D,S>::registerWindowSummarizer()
    {
        typedef TumblingWindowSummarizer<T,G> BaseSummarizerType;
        typedef TumblingWindowSummarizerFactory<T,G> FactoryType;
        BOOST_STATIC_ASSERT((std::tr1::is_base_of<BaseSummarizerType,Summarizer>::value)
                                    && "Summarizer must derive from TumblingWindowSummarizer<T,G>");
        Summarizer * dummy = NULL;
        summarizerFactory_ = FactoryType::createFactory(dummy);
    }

    template <class T, class G, class D, class S>
    TumblingWindowSummarizer<T,G> * TumblingWindow<T,G,D,S>::getSummarizer(PartitionType const & partition)
    {
        return static_cast<TumblingWindowImpl<T,G,D,S> *>(impl_.get())->getSummarizer(partition);
    }

    template <class T, class G, class D, class S>
    void TumblingWindow<T,G,D,S>::join()
    {
        impl_->joinEvictionThread();
    }

    template <class T, class G, class D, class S>
    void TumblingWindow<T,G,D,S>::start()
    {
        impl_->createEvictionThread();
    }

    template <class T, class G, class D, class S>
    void TumblingWindow<T,G,D,S>::shutdown()
    {
        impl_->shutdown();
    }

    template <class T, class G, class D, class S>
    typename TumblingWindow<T,G,D,S>::StorageType const & TumblingWindow<T,G,D,S>::acquireData() const
    {
        impl_->acquireData();
        return this->data_;
    }

    template <class T, class G, class D, class S>
    void TumblingWindow<T,G,D,S>::releaseData() const
    {
        impl_->releaseData();
    }

    template <class T, class G, class D, class S>
    void TumblingWindow<T,G,D,S>::insert(TupleType const & tuple, PartitionType const & partition)
    {
        impl_->insert(tuple, partition);
    }

    template <class T, class G, class D, class S>
    void TumblingWindow<T,G,D,S>::insert(Punctuation const & punct)
    {
        impl_->insert(punct);
    }

    template <class T, class G, class D, class S>
    void TumblingWindow<T,G,D,S>::drain()
    {
        impl_->drain();
    }

    template <class T, class G, class D, class S>
    void TumblingWindow<T,G,D,S>::checkpoint(Checkpoint & data) const
    {
        impl_->checkpoint(data);
    }

    template <class T, class G, class D, class S>
    void TumblingWindow<T,G,D,S>::reset(Checkpoint & data)
    {
        impl_->reset(data);
    }

    template <class T, class G, class D, class S>
    void TumblingWindow<T,G,D,S>::resetToInitialState()
    {
        impl_->resetToInitialState();
    }

    template <class T, class G, class D, class S>
    void TumblingWindow<T,G,D,S>::print(std::ostream& s) const
    {
        impl_->print(s); // TODO add WindowPolicy::toString()
    }
};

#endif
POP_DLL_PUBLIC

#endif /* SPL_RUNTIME_WINDOW_TUMBLING_WINDOW_H */
