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

#ifndef SPL_DEBUGGER_TracePoint_H
#define SPL_DEBUGGER_TracePoint_H

#include <SPL/Debugger/ProbePoint.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <boost/bind.hpp>
#include <fstream>
#include <list>
#include <string>

#define TRACEPOINT_LOGFILE "logfile"

namespace SPL {
class Tuple;
class ProbePointServices;

typedef boost::shared_ptr<Tuple> TuplePtr;

typedef struct TypeRecord
{
    TypeRecord()
      : tuple(NULL)
      , punct(NULL)
      , arrivalTime(0)
    {}
    Tuple* tuple;
    Punctuation* punct;
    double arrivalTime;
} TraceRecordType;

typedef boost::shared_ptr<TraceRecordType> TraceRecordPtr;

/*!
 * Each instance of this class represents a trace point
 * on the SPL application's data path.  A trace point,
 * captures tuple data that transiting an operator's port.
 * The captured data is saved in a buffer, which may be
 * subsequently retrieved / deleted by the user.
 *
 * A trace point is a type of probe point; each trace point
 * corresponds to a operator and port.
 */
class TracePoint : public ProbePoint
{
    ProbePointServices& _probePointServices;
    // Each record captures a tuple. The list is ordered from oldest to newest
    std::list<TraceRecordPtr> _records;
    uint32_t _maxRecords;
    std::string _logFileName;
    std::ofstream _logStream;
    UTILS_NAMESPACE::Mutex _logMutex;
    bool _loggedMetaData;
    uint32_t _enabledCount;
    double _enabledTime;

  public:
    /// Constructor
    /// @param probePointServices Probe Point Services
    /// @param peHandle Processing Element
    /// @param opName Operator Name
    /// @param portType Input or output
    /// @param portIndex Port Number
    TracePoint(ProbePointServices& probePointServices,
               PEImpl& peHandle,
               const std::string& opName,
               PEImpl::PortType portType,
               uint32_t portIndex);

    /// Destructor
    ~TracePoint();

    /// Take action when a Tracepoint triggers
    /// @param hookId Tracepoint number
    /// @param tuple Current Tuple, or NULL if punct != NULL
    /// @param punct Current punctuation or NULL if tuple != NULL
    /// @return true if tuple is to be preserved, false to drop tuple
    bool process(uint32_t hookId, Tuple* tuple, Punctuation* punct);

    /// Enable or disable a trace point
    /// @param enabled 'true' to enable a probe point
    virtual void setEnabled(bool enabled);

    /// Clear the trace records
    void clear();

    /// Return the list of trace records
    /// @return list of trace records
    std::list<TraceRecordPtr>& getRecords();

    /// Set maximum number of record to remember
    /// @param maxRecords Number of records to remember
    void setMaxRecords(uint32_t maxRecords);

    /// Return maximum number of record to remember
    /// @return Number of records to remember
    uint32_t getMaxRecords();

    /// Return probe point type
    /// @return "Tracepoint"
    std::string getProbePointType() const;

    /// Returns a map that describes the operating conditions of the probe point
    /// e.g. number of trace records captured.
    /// @return operating conditions
    ProbePoint::ProbePointConditionsType& getConditions();

    /// Start remembering trace record
    void startLogging();

    /// Stop remembering trace record
    void stopLogging();
};
}

#endif /* SPL_DEBUGGER_TracePoint_H */
