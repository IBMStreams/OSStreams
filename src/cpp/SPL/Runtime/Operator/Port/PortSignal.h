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

#ifndef SPL_RUNTIME_OPERATOR_PORT_PORT_SIGNAL_H
#define SPL_RUNTIME_OPERATOR_PORT_PORT_SIGNAL_H

#include <SPL/Runtime/Common/Prediction.h>
#include <SPL/Runtime/Operator/Port/Inline.h>
#include <SPL/Runtime/Operator/Port/TupleVisualizer.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/SpinLock.h>

#include <stdint.h>
#include <typeinfo>

#include <boost/shared_ptr.hpp>

using namespace std;

namespace SPL {
namespace Meta {
class BaseType;
}
class DebugHook;
class OperatorImpl;
class PortSignalHookRegistery;
class Punctuation;
class TupleVisualizer;
class ViewProperties;

/// Base class that represents a callable SPL port
class PortSignal
{
  public:
    /// Kind of the port signal
    enum Kind
    {
        Direct,
        Active,
        Scheduled
    };

    /// Constructor
    /// @param oper operator that holds this port
    /// @param kind kind of the port signal
    /// @param index index of the port
    /// @param mutating true if the port is mutating
    PortSignal(OperatorImpl& oper, Kind kind, uint32_t index, bool mutating)
      : oper_(oper)
      , kind_(kind)
      , index_(index)
      , mutating_(mutating)
    {}

    /// Destructor
    virtual ~PortSignal(){};

    /// Port index
    /// @return the index
    uint32_t getIndex() const { return index_; }

    /// Submit a tuple on this port
    /// @param tuple tuple to submit
    void submit(Tuple& tuple);

    /// Submit a punctuation on this port
    /// @param punct punctuation to submit
    void submit(Punctuation& punct);

    /// Submit a tupleas a buffer on this port
    /// @param buffer tuple to submit
    void submit(NativeByteBuffer& buffer);

    /// Check if the port is mutating
    /// @return true if the port is mutating
    bool isMutating() const { return mutating_; }

    /// Indicates whether this particular signal accepts
    /// raw data or does it only accept Tuple objects.
    /// @return true if raw data in the form of NativeByteBuffers can be accepted by this port
    virtual bool sendRawBufferData(void);

    /// Indicates whether a signal is in debug mode. The DirectPortSignal subclass which the
    /// parent for Submit and Process signal will override and provide a proper value instead of the
    /// default value specified here.
    virtual bool isInDebugMode() { return false; }

    /// Returns the OperatorImpl for the port
    OperatorImpl& getOperatorImpl() { return oper_; }

  protected:
    uint64_t getLogTimeInMicrosecs();

    OperatorImpl& oper_; //!< operator hosting the port
    Kind kind_;          //!< operator kind
    uint32_t index_;     //!< index of the port
    bool mutating_;      //!< is this a mutating port
};

/// \brief Base class that represents a callable SPL port directly attached
/// to an operator's port
class DirectPortSignal : public PortSignal
{
  public:
    /// Type of the singal
    enum Type
    {
        Process,
        Submit
    };

    /// Constructor
    /// @param oper operator that holds this port
    /// @param type type of the port signal
    /// @param index index of the port
    /// @param mutating true if the port is mutating
    DirectPortSignal(OperatorImpl& oper, Type type, uint32_t index, bool mutating);

    /// Destructor
    virtual ~DirectPortSignal();

    /// Submit a tuple on this port
    /// @param tuple tuple to submit
    void submit(Tuple& tuple) ALWAYS_INLINE
    {
        // inlined due to performance considerations
        if (UNLIKELY(visualize_)) {
            visualize(tuple);
        }
        if (UNLIKELY(debug_)) {
            submitDebug(tuple);
        } else {
            submitInternal(tuple);
        }
    }

    /// Submit a tuple as byte buffer on this port
    /// @param buffer Buffer to submit
    void submit(NativeByteBuffer& buffer) ALWAYS_INLINE
    {
        // inlined due to performance considerations
        if (UNLIKELY(visualize_)) {
            visualize(buffer);
        }
        if (UNLIKELY(debug_)) {
            // if we are in debug mode then dont bother with buffers and create a tuple instead
            // since performance is not a concern.
            auto_ptr<Tuple> tuple = createTuple();
            tuple->deserialize(buffer);
            submitDebug(*tuple.get());
        } else {
            submitInternal(buffer);
        }
    }

    /// Send a buffer to the visualizer
    /// @param buffer NativeByteBuffer to visualize
    void visualize(const NativeByteBuffer& buffer) { visualizer_->visualize(buffer); }

    /// Send a tuple to the visualizer
    /// @param tuple Tuple to visualize
    void visualize(const Tuple& tuple) { visualizer_->visualize(tuple); }

    /// Submit a tuple on this port, debug version
    /// @param tuple tuple to submit
    void submitDebug(Tuple& tuple);

    /// Submit a punctuation on this port
    /// @param punct punctuation to submit
    void submit(Punctuation& punct);

    // Debug interfaces

    /// Register a debug hook on this port
    /// @param hookId hook id for of the debug hook
    /// @param handler hook itseld
    /// @param callFirst if the hook is to be installed as a call-first hook
    /// @return true if the registration succeeds
    bool registerHook(uint32_t hookId,
                      boost::shared_ptr<DebugHook> handler,
                      bool callFirst = false);

    /// Unregister a debug hook on this port
    /// @return true if the unregistration succeeds
    bool unregisterHook(uint32_t hookId);

    /// Create a tuple from this port
    /// @return tuple that is created, memory is owned by the caller
    virtual std::auto_ptr<Tuple> createTuple() const = 0;

    /// Get the type id for the tuple type
    /// @return type id for the tuple type
    virtual std::type_info const& getTypeId() const = 0;

    /// Get the tuple type for the port
    /// @return tuple type for the port
    virtual Meta::BaseType const& getTupleType() const = 0;

    /// Check if the port carries facade tuples
    /// @return true if the port carries facade tuples, false otherwise
    virtual bool isFacade() const = 0;

    // Visualization Support

    /// Add a view stream to an output port
    /// @param view view to be added
    void addView(const ViewProperties& view);

    /// Remove a view stream from an output port
    /// @param view view to be deleted
    void deleteView(const ViewProperties& view);

    /// Override the base class function to return the value of the debug field.
    /// If we are debugging then we will not send tuples as Tuple objects instead
    // of NativeByteBuffers
    virtual bool isInDebugMode() { return debug_; }

    /// Visualize a tuple
    /// This is used by Java operators when they connect directly
    /// to each other.
    void visualizeTuple(const NativeByteBuffer& rawTuple);

  protected:
    inline void submitInternal(NativeByteBuffer& buffer) ALWAYS_INLINE;
    inline void submitInternal(Tuple& tuple) ALWAYS_INLINE;
    void submitInternal(Punctuation& punct);

    struct Counter
    {
        Counter(uint64_t wrap)
          : value_(0)
          , wrap_(wrap)
        {}
        bool increment() { return __sync_add_and_fetch(&value_, 1) % wrap_ == 0; }
        uint64_t value_;
        uint64_t wrap_;
    };

    Type type_;      //!< port type
    bool visualize_; //!< are we visualizing this stream?
    bool debug_;     //!< are we debugging

    std::auto_ptr<PortSignalHookRegistery> hooks_;

    // Visualizer
    std::auto_ptr<TupleVisualizer> visualizer_;
};
};

#endif /* SPL_RUNTIME_OPERATOR_PORT_PORT_SIGNAL_H */
