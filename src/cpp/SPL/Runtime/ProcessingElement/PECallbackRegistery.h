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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_CALLBACK_REGISTERY_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_CALLBACK_REGISTERY_H

#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <UTILS/Mutex.h>
#include <UTILS/ThreadPool.h>
#include <UTILS/WorkerThread.h>

#include <boost/shared_ptr.hpp>

namespace SPL {
class Tuple;
class DebugHook;
class DirectPortSignal;

/// Class that represents a registery managing the debug hooks registered on
/// operator ports
class PECallbackRegistery
{
  public:
    typedef PEImpl::PortType PortType;

    /// Constructor
    PECallbackRegistery();

    /// Destructor
    ~PECallbackRegistery();

    /// Add a debugging hook for an input/output port on an operator
    /// @param opname Operator Name
    /// @param pidx Port index
    /// @param pt INPUT/OUTPUT for port type
    /// @param tupleSignal action to take when receiving/sending a tuple
    /// @param punctSignal action to take when receiving/sending a punctuation
    void addDebugHookLocation(std::string const& opname,
                              uint32_t pidx,
                              PortType pt,
                              DirectPortSignal* tupleSignal,
                              DirectPortSignal* punctSignal);

    /// Register a debugger probe point on an input/output port on an operator, to be triggered
    /// when a tuple or punctuation is received.
    /// @param hndl Debug Hook action
    /// @param opname Operator Name
    /// @param pt INPUT/OUTPUT for port type
    /// @param pidx Port index
    /// @param callFirst 'true' if this should be added to the hed of the chain, rather than the
    /// tail
    /// @return the hook id associated with this probe point
    uint32_t registerProbePoint(boost::shared_ptr<DebugHook> hndl,
                                std::string const& opname,
                                PortType pt,
                                uint32_t pidx,
                                bool callFirst = false);

    /// Unregister a debugger probe point on an input/output port on an operator
    /// @param hookId Hook id returned from registerProbePoint
    void unregisterProbePoint(uint32_t hookId);

    /// Register a debugger injection point to be used to add tuples/punctuations to a stream
    /// @param opname Operator Name
    /// @param pt INPUT/OUTPUT for port type
    /// @param pidx Port index
    /// @return the hook id associated with injection probe point
    uint32_t registerInjectPoint(const std::string& opname, PortType pt, uint32_t pidx);

    /// Unregister a debugger injection point on an input/output port on an operator
    /// @param hookId Hook id returned from registerInjectPoint
    void unregisterInjectPoint(uint32_t hookId);

    /// Return the tuple associated with an injection point
    /// @pre hookId came from registerInjectPoint
    /// @param hookId Hook id returned from registerInjectPoint
    /// @return tuple associated with this injection point
    Tuple& retrieveTuple(uint32_t hookId);

    /// Submit the tuple associated with a hook id from an injection point.   It was already
    /// updated by the debugger
    /// @pre hookId came from registerInjectPoint
    /// @param hookId Hook id returned from registerInjectPoint
    void submitTuple(uint32_t hookId);

    /// Submit punctuation associated with a hook id from an injection point.
    /// @pre hookId came from registerInjectPoint
    /// @param hookId Hook id returned from registerInjectPoint
    /// @param punct Punctuation to be submitted
    void submitPunctuation(uint32_t hookId, Punctuation const& punct);

  private:
    typedef std::pair<std::string, uint32_t> OperPortPair;
    typedef std::pair<DirectPortSignal*, DirectPortSignal*> PortSignalPair;
    PortSignalPair& getPortSignals(PortType pt, OperPortPair const& port);
    void checkHookId(uint32_t hookId);
    void checkProbePoint(uint32_t hookId);
    void checkInjectPoint(uint32_t hookId);

  private:
    std::tr1::unordered_map<OperPortPair, PortSignalPair> iport2Signals_;
    std::tr1::unordered_map<OperPortPair, PortSignalPair> oport2Signals_;
    std::tr1::unordered_map<uint32_t, Tuple*> hookId2Tuple_; // for inject points
    std::tr1::unordered_map<uint32_t, PortType> hookId2PortType_;
    std::tr1::unordered_map<uint32_t, OperPortPair> hookId2Port_;

    uint32_t hookId_; // current hook
    Distillery::Mutex mutex_;
    Distillery::FixedThreadPool tpool_;

    class Submitter : public Distillery::WorkItem
    {
      public:
        Submitter(DirectPortSignal& sig)
          : sig_(sig)
        {}
        virtual ~Submitter() {}
        virtual void satisfy() = 0;

      public:
        DirectPortSignal& sig_;
    };

    class TupleSubmitter : public Submitter
    {
      public:
        TupleSubmitter(DirectPortSignal& sig, Tuple* tuple)
          : Submitter(sig)
          , tuple_(tuple)
        {}
        void satisfy();
        virtual ~TupleSubmitter();

      private:
        Tuple* tuple_;
    };

    class PunctuationSubmitter : public Submitter
    {
      public:
        PunctuationSubmitter(DirectPortSignal& sig, const Punctuation& punc)
          : Submitter(sig)
          , punc_(punc)
        {}
        virtual ~PunctuationSubmitter() {}
        void satisfy();

      private:
        Punctuation punc_;
    };
};
}

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PE_DEBUG_CALLBACK_REGISTERY_H */
