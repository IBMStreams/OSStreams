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

#ifndef SPL_RUNTIME_WINDOW_WINDOW_EVENT_H
#define SPL_RUNTIME_WINDOW_WINDOW_EVENT_H

/*!
 * \file WindowEvent.h \brief Definition of the SPL::WindowEvent class.
 */

#include <SPL/Runtime/Operator/State/Checkpoint.h>
#include <SPL/Runtime/Operator/EventTime/EventTimeContext.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#include <deque>
#include <vector>
#include <list>
#include <tr1/unordered_map>
#include <stdint.h>
#include <stddef.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
    template<class T, class G, class D, class S>
    class Window;

    /// @ingroup Window
    /// @brief Class that defines an interface for window events.
    ///
    /// Inherit from this class and override select methods to handle window
    /// events you are interested in.
    template <class T, class G=int32_t, class D=std::deque<T>,
              class S=std::tr1::unordered_map<G, D> >
    class WindowEvent {
    public:
        typedef T TupleType; //!< tuple type
        typedef G PartitionType; //!< partition type
        typedef D DataType; //!< data type
        typedef Window<T,G,D,S> WindowType; //!< window type

        /// Destructor
        ///
        virtual ~WindowEvent() {}

        /// @brief This event is fired right before a tuple is inserted into the window
        /// @param window the window on which this event has fired
        /// @param tuple the tuple that is to be inserted into the window
        /// @param partition the partition that the \a tuple belongs to (for partitioned windows)
        virtual void beforeTupleInsertionEvent(WindowType & window,
                                               TupleType const & tuple,
                                               PartitionType const & partition) {}

        /// @brief This event is fired right after a tuple is inserted into the window
        /// @param window the window on which this event has fired
        /// @param tuple the tuple that is just inserted into the window
        /// @param partition the partition that the \a tuple belongs to (for partitioned windows)
        virtual void afterTupleInsertionEvent(WindowType & window,
                                              TupleType & tuple,
                                              PartitionType const & partition) {}

        /// @brief This event is fired right before a tuple is evicted from a sliding window
        /// @param window the window on which this event has fired
        /// @param tuple the tuple that is to be evicted from the window
        /// @param partition the partition that the \a tuple belongs to (for partitioned windows)
        virtual void beforeTupleEvictionEvent(WindowType & window,
                                              TupleType & tuple,
                                              PartitionType const & partition) {}

        /// @brief This event is fired right after a tuple is evicted from a sliding window
        /// @param window the window on which this event has fired
        /// @param tuple the tuple that is just evicted from the window
        /// @param partition the partition that the \a tuple belongs to (for partitioned windows)
        virtual void afterTupleEvictionEvent(WindowType & window,
                                             TupleType & tuple,
                                             PartitionType const & partition) {}

        /// @brief This event is fired right before a TumblingWindow is flushed.
        /// @param window the window on which this event has fired
        /// @param partition the partition that the window flush applies to (for partitioned windows)
        virtual void beforeWindowFlushEvent(WindowType & window,
                                            PartitionType const & partition) {}

        /// @brief This event is fired right after a tumbling window is flushed
        /// @param window the window on which this event has fired
        /// @param partition the partition that the window flush applies to (for partitioned windows)
        virtual void afterWindowFlushEvent(WindowType & window,
                                           PartitionType const & partition) {}

        /// @brief This event is fired when a window is triggered.
        /// @param window the window on which this event has fired
        /// @param partition the partition that the window trigger applies to (for partitioned windows)
        /// @see SlidingWindow
        /// @see TimeIntervalWindowPane
        virtual void onWindowTriggerEvent(WindowType & window,
                                          PartitionType const & partition) {}

        /// @brief This event is fired when a SlidingWindow is full for the first
        /// time, or when a TimeIntervalWindowPane becomes complete.
        /// @param window the window on which this event has fired
        /// @param partition the partition that the window trigger applies to (for partitioned windows)
        virtual void onWindowInitialFullEvent(WindowType & window,
                                              PartitionType const & partition) {}

        /// @brief This event is fired right before a TimeIntervalWindowPane transitions from
        /// %TimeIntervalWindowPane::Complete to %TimeIntervalWindowPane::Closed.
        ///
        /// The client code cannot insert tuples into closed windows. The
        /// handler should use this event for releasing any of its resources
        /// associated to this window.
        /// @param window the window on which this event has fired. Can be cast to TimeIntervalWindowPane.
        /// @param partition the partition that the window trigger applies to (for partitioned windows)
        virtual void beforeWindowCloseEvent(WindowType & window,
                                            PartitionType const & partition) {}

        // Data definitions for Partition Eviction

        /// Units of time for aging partitions
        typedef uint64_t TimeType;

        /// Partition Key, Time mapping
        typedef std::pair<PartitionType, TimeType> PartitionOrdering;

        /// List of partitions in time order
        /// begin() gives the least recently referenced partition, end() the newest referenced
        typedef std::list<PartitionOrdering> TimeOrderedPartition;

        /// @brief Class that iterates over a list of partitions which are about to be
        /// removed from the window.  This allows cleanup of associated data. Only used with
        /// the onWindowPartitionEviction event.
        class PartitionIterator : public TimeOrderedPartition::const_iterator {
            typedef typename TimeOrderedPartition::const_iterator BaseClass;
        public:
            /// Empty Constructor
            PartitionIterator() {}

            /// Constructor
            /// @param rhs iterator to be copied
            PartitionIterator(PartitionIterator const& rhs) : BaseClass(rhs) {}

#ifndef DOXYGEN_SKIP_FOR_USERS
            /// Constructor
            /// @param iter iterator into the list of partitions
            PartitionIterator(typename TimeOrderedPartition::const_iterator const& iter) : BaseClass (iter) {}

            /// Constructor
            /// @param iter iterator into the list of partitions
            PartitionIterator(typename TimeOrderedPartition::iterator const& iter) : BaseClass (iter) {}
#endif /*DOXYGEN_SKIP_FOR_USERS*/

            /// Return the partition value pointed to by the iterator
            /// @return the partition value for the current element
            PartitionType const& partition() const { return this->operator*().first; }

            /// Return the last modified time pointed to by the iterator
            /// @return the last modified time  for the current element
            TimeType const& time() const { return this->operator*().second; }
        };

        /// @brief Class that iterates over a list of partitions, allowing
        /// selection of partitions to be removed.  Only used in the
        /// onWindowPartitionEvictionSelection event.
        class PartitionSelectionIterator : public PartitionIterator {
        public:
            /// Empty Constructor
            PartitionSelectionIterator()
                : PartitionIterator(), partitionsToBeEvicted_(NULL) {}

            /// Copy Constructor
            /// @param rhs iterator to be copied
            PartitionSelectionIterator(PartitionSelectionIterator const & rhs)
                : PartitionIterator(rhs), partitionsToBeEvicted_(rhs.partitionsToBeEvicted_) {}

#ifndef DOXYGEN_SKIP_FOR_USERS
            /// Constructor
            /// @param partitionsToBeEvicted holder for partitions selected to be removed
            /// @param iter iterator into the list of partitions
            PartitionSelectionIterator(std::vector<const PartitionType*>& partitionsToBeEvicted,
                                       typename TimeOrderedPartition::const_iterator const & iter)
                : PartitionIterator(iter), partitionsToBeEvicted_(&partitionsToBeEvicted) {}
#endif /*DOXYGEN_SKIP_FOR_USERS*/

            /// Mark the partition pointed to by the iterator for eviction.
            /// The partition will be removed after the onWindowPartitionEvictionSelection
            /// call returns.
            void markForRemoval ()
            {
                if (partitionsToBeEvicted_) {
                    partitionsToBeEvicted_->push_back(&this->partition());
                }
            }
        private:
            std::vector<const PartitionType*> *partitionsToBeEvicted_;
        };

        // Partition Eviction Events

        /// @brief This event is fired when partitions are to be selected for removal.
        ///
        /// This event is only fired for windows that have a partition
        /// eviction policy with a OperatorDefined partition selection. The
        /// markForRemoval function should be called by the implementer of
        /// this function on the iterators that point to the partitions that
        /// are desired to be removed.
        /// @param window the window on which this event has fired
        /// @param begin iterator to first (oldest) partition
        /// @param end iterator just past the last (newest) partition
        virtual void onWindowPartitionEvictionSelection(WindowType & window,
                                                        PartitionSelectionIterator const& begin,
                                                        PartitionSelectionIterator const& end) {}

        /// @brief This event is fired when some partitions are about to be evicted.
        /// @param window the window on which this event has fired
        /// @param begin iterator to first (oldest) partition to be evicted
        /// @param end iterator just past the last (newest) partition to be evicted
        virtual void onWindowPartitionEviction(WindowType & window,
                                               PartitionIterator const& begin,
                                               PartitionIterator const& end) {}

        // More Window Events

        /// @brief This event is fired when a punctuation is inserted in a
        /// <tt>tumbling, punct</tt> window, and there are no tuples in any
        /// partition.
        /// @param window the window on which this event has fired
        virtual void onEmptyWindowPunctEvent(WindowType & window) {}

        // Consistent region behavior

        /// @brief This event is fired after the window is being saved to the
        /// specified checkpoint.
        ///
        /// The event handler is responsible for writing its state to the
        /// checkpoint stream.
        /// @param ckpt the @c Checkpoint stream
        virtual void onCheckpointEvent(Checkpoint & ckpt) const {}

        /// @brief This event is fired after the window's state is being
        /// read from the specified checkpoint. The event handler is
        /// responsible for reading its state which was saved by @c onCheckpointEvent().
        /// @param ckpt the @c Checkpoint stream
        virtual void onResetEvent(Checkpoint & ckpt) {}

        /// @brief This event is fired after the window's state is being initialized.
        ///
        /// The event handler is responsible for initializing its state.
        virtual void onResetToInitialStateEvent() {}
    };
};
POP_DLL_PUBLIC

#endif /* SPL_RUNTIME_WINDOW_WINDOW_EVENT_H */
