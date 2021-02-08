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

#ifndef SPL_RUNTIME_WINDOW_WINDOW_COMMON_H
#define SPL_RUNTIME_WINDOW_WINDOW_COMMON_H

/*!
 * \file WindowCommon.h \brief Definition of the SPL::Window class.
 */

#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Operator/State/Checkpoint.h>
#include <SPL/Runtime/Window/WindowPolicy.h>
#include <SPL/Runtime/Window/PartitionEvictionPolicy.h>
#include <SPL/Runtime/Window/TriggerInfoProvider.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <UTILS/HashMapHelpers.h>
#include <deque>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    // Forward declarations
    class Operator;
    class OperatorImpl;
    template <class T, class G, class D, class S>
    class WindowEvent;
    template <class T, class G, class D, class S>
    class WindowImpl;
    template <class T, class G, class D, class S>
    class AutoWindowDataAcquirer;

#ifndef DOXYGEN_SKIP_FOR_USERS
    class TriggerInfoProvider;
    class WatermarkConsumer;

    /// @ingroup Window
    /// @brief Base of window classes.
    class BaseWindow
    {
    public:
        /// @brief Constructs a %BaseWindow and registers it with the
        /// specified operator.
        BaseWindow(Operator & oper, uint32_t port);

        /// @brief Constructs a %BaseWindow.
        ///
        /// This constructor is typically called from a derived class
        /// when the window must not register with an operator.
        BaseWindow();

        /// Destructor
        virtual ~BaseWindow();

        /// @brief Returns the operator associated with the window.
        /// @throw SPLRuntimeInvalidOperationException if no operator is registered
        /// with the window
        Operator & getOperator() const;

        /// @brief Returns the port index of the operator associated with the
        /// window, or std::numeric_limits<uint32_t>::max() if the window is
        /// not associated with an operator.
        uint32_t getPort() const
        { return port_; }

    protected:
        friend class OperatorImpl;
        virtual void shutdown() = 0;
        virtual void start() = 0;
        virtual void join() = 0;
        OperatorImpl & oper_;
        uint32_t port_;
    };

    template <class P>
    struct ObjectPointerDeleter {
        static void deletePointer(const P&) {} // do nothing....
    };
    template <class P>
    struct ObjectPointerDeleter<P*> {
        static void deletePointer(P* p) { delete  p; }
    };
#endif /*DOXYGEN_SKIP_FOR_USERS */

    /// @ingroup Window
    /// @brief Common base for window classes with template parameters.
    template <class T, class G=int32_t, class D=std::deque<T>,
              class S=std::tr1::unordered_map<G, D> >
    class Window : public BaseWindow
    {
    public:
        typedef T TupleType; //!< tuple type
        typedef G PartitionType; //!< partition type
        typedef D DataType; //!< data type
        typedef S StorageType; //!< storage type
        typedef WindowEvent<T,G,D,S> WindowEventType; //!< window event type

        /// Constructor
        /// @param oper operator that holds this window
        /// @param port operator input port index for the window
        Window(Operator & oper, uint32_t port)
            : BaseWindow(oper, port),
              afterTupleInsertion_(NULL),
              beforeTupleInsertion_(NULL),
              onWindowPartitionEvictionSelection_(NULL),
              onWindowPartitionEviction_(NULL),
              onSerialization_(NULL) {}

        /// Constructor
        /// @param oper operator that holds this window
        /// @param port operator input port index for the window
        /// @param partitionEvictionPolicy partition eviction policy of the window
        Window(Operator & oper, uint32_t port,
               PartitionEvictionPolicy const& partitionEvictionPolicy)
            : BaseWindow(oper, port),
              afterTupleInsertion_(NULL),
              beforeTupleInsertion_(NULL),
              onWindowPartitionEvictionSelection_(NULL),
              onWindowPartitionEviction_(NULL),
              onSerialization_(NULL),
              partitionEvictionPolicy_(&partitionEvictionPolicy.clone()) {}

        /// Constructor to use when the window does not register with an
        /// operator.
        Window() : BaseWindow(),
              afterTupleInsertion_(NULL),
              beforeTupleInsertion_(NULL),
              onWindowPartitionEvictionSelection_(NULL),
              onWindowPartitionEviction_(NULL),
              onSerialization_(NULL) {}

        /// Destructor
        ///
        virtual ~Window() {}

        /// Get the data associated with this window
        /// @return data associated with this window
        StorageType const & getWindowStorage() const
            { return data_; }

        /// Get the data associated with this window
        /// @return data associated with this window
        StorageType & getWindowStorage()
            { return data_; }

        /// Get the data associated with this window for the given \a partition
        /// @param partition partition for the tuples stored in the window
        /// @return data associated with this window for the given \a partition
        DataType const & getWindowData(PartitionType const & partition) const
            { return data_.find(partition)->second; }

        /// Get the data associated with this window for the given \a partition
        /// @param partition partition for the tuples stored in the window
        /// @return data associated with this window for the given \a partition
        DataType & getWindowData(PartitionType const & partition)
            { return data_.find(partition)->second; }

        /// Get the data associated with this window (for windows with a default
        /// partition). This will result in creating the default partition, if
        /// it does not already exist.
        /// @return data associated with this window
        DataType & getWindowData()
        {
            PartitionType const & partition = PartitionType();
            typename StorageType::iterator it = data_.find(partition);
            if (it != data_.end()) {
                return it->second;
            }
            data_.insert(std::make_pair(partition, DataType()));
            return data_.find(partition)->second;
        }

        /// Get the data associated with this window. If this window involves
        /// eviction or trigger policies that are time based, appropriate
        /// locking is performed. This call should be followed by a
        /// \c releaseData() call for releasing the lock in these cases. See
        /// SPL::AutoWindowDataAcquirer for a safer, RAII based access to the
        /// contents of the window.
        /// @return contents of the window
        virtual StorageType const & acquireData() const = 0;

        /// Get the data associated with this window. If this window involves
        /// eviction or trigger policies that are time based, appropriate
        /// locking is performed. This call should be followed by a
        /// \c releaseData() call for releasing the lock in these cases. See
        /// SPL::AutoWindowDataAcquirer for a safer, RAII based access to the
        /// contents of the window.
        /// @return contents of the window
        virtual StorageType & acquireData()
        {
            return const_cast<StorageType&>(const_cast<Window<T,G,D,S> const *>(this)->acquireData());
        }

        /// Release the window data
        /// @return void
        virtual void releaseData() const = 0;

        /// Insert a tuple into the window
        /// @param tuple tuple to be inserted
        /// @param partition the partition the \a tuple belongs to (optional)
        virtual void insert(TupleType const & tuple, PartitionType const & partition = PartitionType()) = 0;

        /// Notify the window about the existence of a punctuation in the stream
        /// @param punct the punctuation that is received
        virtual void insert(Punctuation const & punct) = 0;

        /// Drain outstanding window processing.
        virtual void drain() = 0;

        /// Save the window state into the specified @c Checkpoint.
        ///
        /// The window state includes data kept in the window and state
        /// associated with the evict and trigger policies.  Tumbling window
        /// summarizers are notified and expected to save their state.
        /// @param checkpoint the checkpoint where the window state is saved
        /// @see TumblingWindowSummarizer::onCheckpointEvent(Checkpoint &)
        virtual void checkpoint(Checkpoint & checkpoint) const = 0;

        /// Restore the window to the state read from the specified
        /// @c Checkpoint.
        /// @param checkpoint the checkpoint object where the window state is
        /// read from.
        /// @see Window::checkpoint(Checkpoint&)
        virtual void reset(Checkpoint & checkpoint) = 0;

        /// Restore the window to the initial state.
        virtual void resetToInitialState() = 0;

        /// Register a handler for before tuple insertion event
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any
        /// @return the old event handler, or NULL if none was found
        WindowEventType * registerBeforeTupleInsertionHandler(WindowEventType * e);

        /// Register a handler for after tuple insertion event
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any
        /// @return the old event handler, or NULL if none was found
        WindowEventType * registerAfterTupleInsertionHandler(WindowEventType * e);

        /// Register a handler for on partition eviction selection
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any
        /// @return the old event handler, or NULL if none was found
        WindowEventType * registerOnWindowPartitionEvictionSelection(WindowEventType * e);

        /// Register a handler for on partition eviction
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any
        /// @return the old event handler, or NULL if none was found
        WindowEventType * registerOnWindowPartitionEviction(WindowEventType * e);

        /// Register a handler for serialization events: @c onCheckpointEvent,
        /// @c onResetEvent, @c onResetToInitialStateEvent.
        /// @param e the event handler, could be NULL. It replaces the existing
        /// one, if any.
        /// @return the old event handler, or NULL if none was found
        WindowEventType * registerSerializationHandler(WindowEventType * e);

        /// Delete all the objects pointed to in the window.  This is a no-op unless the
        /// window contains pointers to objects
        void deleteWindowObjects() {
            typename StorageType::const_iterator it;
            for (it = data_.begin(); it != data_.end(); it++) {
                const DataType& data = it->second;
                for (typename DataType::const_iterator it2 = data.begin();
                        it2 != data.end(); it2++) {
                    ObjectPointerDeleter<TupleType>::deletePointer(*it2);
                }
            }
        }

        /// Return the partition eviction policy, if any
        /// @return return the partition eviction policy, or NULL
        PartitionEvictionPolicy const* getPartitionEvictionPolicy() const
        {
            return partitionEvictionPolicy_.get();
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
    public:
        /// @brief Returns a string representation of the window.
        ///
        /// The result is a textual representation of the window which is
        /// easy to read by a person.
        /// @return the string representation of the window
        std::string toString() const
        {   std::ostringstream s; print(s); return s.str(); }

    protected:
        /// @brief Writes a textual representation of the window into the specified stream.
        ///
        /// Subclasses should override and call this method from their implementation.
        /// @see toString()
        virtual void print(std::ostream& s) const;

    public:
        /// Get the number of tuples in the given \a partition
        /// @param partition partition for the tuples stored in the window
        /// @return the number of tuples in the given \a partition
        virtual typename DataType::size_type getTupleCount(PartitionType const & partition) const
            { return data_.find(partition)->second.size(); }

    protected:
        friend class WindowImpl<T,G,D,S>;
        friend class AutoWindowDataAcquirer<T,G,D,S>;

        StorageType data_;
        WindowEventType * afterTupleInsertion_;
        WindowEventType * beforeTupleInsertion_;
        WindowEventType * onWindowPartitionEvictionSelection_;
        WindowEventType * onWindowPartitionEviction_;
        WindowEventType * onSerialization_;
        std::auto_ptr<PartitionEvictionPolicy const> partitionEvictionPolicy_;

#endif /* DOXYGEN_SKIP_FOR_USERS */
    };

    /*!
     * @ingroup Window
     * @brief Class that provides an RAII handler for window data access.

        This class is used for acquiring the data associated with a window, with
        locking if required. Example usage is as follows:

        \verbatim
        { AutoWindowDataAcquirer awda(window);
          ...
        }
        \endverbatim

        This class, when constructed, calls acquireData() on the window,
        and when destructed, calls releaseData() on the window
    */
    template <class T, class G=int32_t, class D=std::deque<T>,
              class S=std::tr1::unordered_map<G, D> >
    class AutoWindowDataAcquirer
    {
    public:
        typedef T TupleType;
        typedef G PartitionType;
        typedef D DataType;
        typedef S StorageType;
        typedef Window<T,G,D,S> WindowType;
        /// Constructor
        /// @param window window whose data is to be acquired
        AutoWindowDataAcquirer(WindowType const & window)
            : window_ (window)
        {
            window_.acquireData();
        }
        /// Destructor
        ~AutoWindowDataAcquirer()
        {
            window_.releaseData();
        }
        /// Access window data
        StorageType const & getWindowStorage() const
        {
            return window_.getWindowStorage();
        }
    private:
        WindowType const & window_;
    };

#ifndef DOXYGEN_SKIP_FOR_USERS

    class WatermarkConsumer {
    public:
        virtual void insert(Punctuation const & punct) = 0;
    protected:
        virtual ~WatermarkConsumer(){}
    };
#endif // DOXYGEN_SKIP_FOR_USERS

    ///////////////////////////////////////////////////////////////////////
    template <typename T, typename G, typename D, typename S>
    typename Window<T,G,D,S>::WindowEventType *
    Window<T,G,D,S>::registerBeforeTupleInsertionHandler(WindowEventType * e)
    {
        WindowEventType * o = beforeTupleInsertion_;
        beforeTupleInsertion_ = e;
        return o;
    }

    template <typename T, typename G, typename D, typename S>
    typename Window<T,G,D,S>::WindowEventType *
    Window<T,G,D,S>::registerAfterTupleInsertionHandler(WindowEventType * e)
    {
        WindowEventType * o = afterTupleInsertion_;
        afterTupleInsertion_ = e;
        return o;
    }

    template <typename T, typename G, typename D, typename S>
    typename Window<T,G,D,S>::WindowEventType *
    Window<T,G,D,S>::registerOnWindowPartitionEvictionSelection(WindowEventType * e)
    {
        WindowEventType * o = onWindowPartitionEvictionSelection_;
        onWindowPartitionEvictionSelection_ = e;
        return o;
    }

    template <typename T, typename G, typename D, typename S>
    typename Window<T,G,D,S>::WindowEventType *
    Window<T,G,D,S>::registerOnWindowPartitionEviction(WindowEventType * e)
    {
        WindowEventType * o = onWindowPartitionEviction_;
        onWindowPartitionEviction_ = e;
        return o;
    }

    template <typename T, typename G, typename D, typename S>
    typename Window<T,G,D,S>::WindowEventType *
    Window<T,G,D,S>::registerSerializationHandler(WindowEventType * e)
    {
        WindowEventType * o = onSerialization_;
        onSerialization_ = e;
        return o;
    }

    template <typename T, typename G, typename D, typename S>
    void Window<T,G,D,S>::print(std::ostream& s) const
    {
      s << "Window={";

#ifdef WINDOW_PRINT_HANDLERS
      s << " afterTupleInsertion_=" << afterTupleInsertion_ <<
          " beforeTupleInsertion_=" << beforeTupleInsertion_ <<
          " onWindowPartitionEvictionSelection_=" << onWindowPartitionEvictionSelection_ <<
          " onWindowPartitionEviction_=" << onWindowPartitionEviction_ <<
          " onSerialization_=" << onSerialization_;
#endif
      s << '}';
    }
};

#endif /* SPL_RUNTIME_WINDOW_WINDOW_COMMON_H */
