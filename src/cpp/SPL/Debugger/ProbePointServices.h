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

#ifndef SPL_DEBUGGER_PROBE_POINT_SERVICES_H
#define SPL_DEBUGGER_PROBE_POINT_SERVICES_H

#include <libtecla.h>
#include <vector>

#include <SPL/Debugger/TracePoint.h>
#include <SPL/Runtime/ProcessingElement/PEDebugService.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

namespace SPL {
class ApplicationModel;
class BreakPoint;
class InjectPoint;
class ProbePointManager;
class SdbConsoleLogger;
class SdbConsoleTracer;
class StandaloneClient;

/*!
 * This class implements the user interactions required for
 * managing probe points.
 *
 * The current implementation assumes this debug client is
 * run as part of the SPL runtime library and is linked with
 * a standalone application which has been compiled for debugging (-g).
 * This class is instantiated by the runtime's debug services (see PE).
 * Only a single instance of this class is required.
 */
class ProbePointServices : public PEDebugService
{
  public:
    /// Constructor
    /// @param pe Processing Element
    ProbePointServices(PEImpl& pe);

    /// Destructor
    ~ProbePointServices();

    /// Main processing loop - run as thread
    /// @param threadArgs Thread arguments (not used)
    virtual void* run(void* threadArgs);

    /// Block here to stop PE code from starting to process data
    void prepareToProcess();

    /// Shut down services
    void prepareToShutdown();

    /// Stop debugging - allow program to continue
    /// Disable all probe points
    /// Continue all stopped breakpoints so their threads can continue
    void stopDebugger();

    /// Print the contents of the given tuple and corresponding probe point
    /// via the standalone client
    /// @param id probe point id
    /// @param probePoint Probe point
    /// @param tuple Tuple to print
    void printTuple(uint32_t id, ProbePoint* probePoint, Tuple* tuple);

    /// Print the contents of the given punctuation and corresponding probe point
    /// via the standalone client
    /// @param id probe point id
    /// @param probePoint Probe point
    /// @param punct Punctuation to print
    void printPunctuation(uint32_t id, ProbePoint* probePoint, Punctuation& punct);

    /// Print a message via the standalone client
    /// @param message Message to print
    void printMessage(const std::string& message);

    /// Return the PE
    /// @return Processing Element
    PEImpl& getPeHandle() const { return pe_; }

    /// Return all probe points
    /// @param probePointsVector Return the list of probe point ids
    void getProbePoints(std::vector<uint32_t>& probePointsVector) const;

    /// Return probe point for a give hook id
    /// @param probePointId Probe point id
    /// @return matching probe point or NULL if not found
    ProbePointPtr getProbePoint(uint32_t probePointId);

    /// Given probe point properties, Return the corresponding probe point id, or -1, if the
    /// probe point is not defined.
    /// @param opName Operator Name
    /// @param opType Probe point type
    /// @param portType Input or output
    /// @param portIndex Port Number
    /// @return prober point id, or -1 if not found
    int getProbePoint(const std::string& opName,
                      const std::string& opType,
                      PEImpl::PortType portType,
                      uint32_t portIndex);

    /// Continue execution for the one, and only, stopped breakpoint
    void continueRunning();

    /// Continue execution for the specified (stopped) probe point
    /// @param probepointId Probe point id
    void continueRunning(uint32_t probepointId);

    /// Continue execution for the specified (stopped) breakpoint
    /// @param probepointId Probe point id
    /// @param breakPoint Breakpoint to signal
    void continueRunning(uint32_t probepointId, BreakPoint& breakPoint);

    /// Continue execution for the specified injectPoint
    /// @param probepointId Probe point id
    /// @param injectPoint Injectpoint
    /// @param punctOrder "b" for before, "a" for after, "o" for omit
    void continueRunning(uint32_t probepointId,
                         InjectPoint& injectPoint,
                         const std::string& punctOrder);

    /// Set a breakpoint
    /// @param breakPoint Breakpoint to set
    uint32_t setBreakPoint(ProbePointPtr breakPoint);

    /// Set an injection point
    /// @param opName Operator Name
    /// @param portType Input or output
    /// @param portIndex Port Number
    uint32_t setInjectPoint(const std::string& opName,
                            const std::string& portType,
                            uint32_t portIndex);

    /// Set a tracepoint
    /// @param tracePoint Tracepoint to set
    uint32_t setTracePoint(ProbePointPtr tracePoint);

    /// Print a probe point nicely
    /// @param probePointId Probe point to print
    void showProbePoint(uint32_t probePointId);

    /// Print probe point records nicely
    /// @param probePointId Probe point to print
    void showProbePointRecords(uint32_t probePointId);

    /// Remove a probe point
    /// @param probePointId Probe point to remove
    void remProbePoint(uint32_t probePointId);

    /// Enable a probe point
    /// @param probePointId Probe point to enable
    void enableProbePoint(uint32_t probePointId);

    /// Disable a probe point
    /// @param probePointId Probe point to disable
    void disableProbePoint(uint32_t probePointId);

    /// Save probe point data tp a file
    /// @param probePointId Probe point to save
    /// @param fileName File to write
    void saveProbePoint(uint32_t probePointId, const std::string& fileName);

    /// Start logging probe point data
    /// @param probePointId Probe point to log
    void startLogging(uint32_t probePointId);

    /// Stop logging probe point data
    /// @param probePointId Probe point to stop logging
    void stopLogging(uint32_t probePointId);

    /// Clear logging probe point data
    /// @param probePointId Probe point
    void clearProbePoint(uint32_t probePointId);

    /// Update a Tuple for the one, and only, stopped breakpoint
    /// @param attributeName attribute field name to update
    /// @param attributeValue New value for attribute in string form
    void updateTuple(const std::string& attributeName, const std::string& attributeValue);

    /// Update a Tuple for the specified (stopped) breakpoint
    /// @param attributeName attribute field name to update
    /// @param attributeValue New value for attribute in string form
    /// @param probepointId Probe point number with tuple
    void updateTuple(const std::string& attributeName,
                     const std::string& attributeValue,
                     uint32_t probepointId);

    /// Update an arbitrary Tuple
    /// @param tuple Tuple to update
    /// @param attributeName attribute field name to update
    /// @param attributeValue New value for attribute in string form
    void updateTuple(Tuple* tuple,
                     const std::string& attributeName,
                     const std::string& attributeValue);

    /// Is this a standalone application?
    /// @return true if it is a standalone application
    bool isStandaloneApplication();

    /// Is the debugger active
    /// @return true if the debugger is active
    inline bool isDebuggerActive() const { return _debuggerActive; }

    /// Is the debugger in startup phase
    /// @return true if the debugger is starting up
    inline bool isStartup() const { return _startup; }

    /// Is the debugger shutting down
    /// @return true if the debugger is shutting down
    inline bool isShuttingDown() const { return _shuttingDown; }

    /// Signal startup
    inline void signalStartup()
    {
        if (_startup) {
            _startup_cv.signal();
        }
    }

    const ApplicationModel& getApplicationModel() const { return *_applicationModel; }

    /// Format Tuple into buffer separated by ','
    /// @param tuple Tuple to dump
    /// @param buffer Buffer to write to
    void formatTupleCSV(Tuple* tuple, std::stringstream& buffer);

    /// Format Tuple attribute names into buffer separated by ','
    /// @param tuple Tuple to dump
    /// @param buffer Buffer to write to
    void formatTupleKeysCSV(Tuple* tuple, std::stringstream& buffer);

    /// Format Tuple attribute types into buffer separated by ','
    /// @param tuple Tuple to dump
    /// @param buffer Buffer to write to
    void formatTupleDatatypeCSV(Tuple* tuple, std::stringstream& buffer);

    /// Return the list of probe points matching constraints
    /// @param probePointIds returned vector of probe point ids
    /// @param inclProbePointType Probe point type to match or NULL for all
    /// @param inclKey Probe point condition to match or NULL for all
    /// @param inclValue Probe point condition value to match or NULL for all
    void getProbepoints(std::vector<uint32_t>& probePointIds,
                        const char* inclProbePointType,
                        const char* inclKey,
                        const char* inclValue) const;

    /// Return the list of stopped break points
    /// @param stoppedIds returned vector of probe point ids
    void getStoppedBreakpoints(std::vector<uint32_t>& stoppedIds) const;

    /// Return the list of stopped probe points
    /// @param stoppedIds returned vector of probe point ids
    void getStoppedProbepoints(std::vector<uint32_t>& stoppedIds) const;

    /// Save current configuration to a file
    /// @param cfgName File to save configuration to
    void saveCfg(const std::string& cfgName);

    /// Load current configuration from a file
    /// @param cfgName File to load configuration from
    void loadCfg(const std::string& cfgName);

    /// Get the current console logger
    /// @return Console logger
    SdbConsoleLogger* getConsoleLogger() const { return _consoleLogger; }

    /// Get the current console tracer
    /// @return Console logger
    SdbConsoleTracer* getConsoleTracer() const { return _consoleTracer; }

  private:
    Distillery::CV _startup_cv;
    Distillery::Mutex _startup_mutex;
    bool _debuggerActive;
    bool _startup;
    bool _shuttingDown;
    StandaloneClient* _standaloneClient;
    ProbePointManager* _probePointManager;
    ApplicationModel* _applicationModel;
    SdbConsoleLogger* _consoleLogger;
    SdbConsoleTracer* _consoleTracer;
    void writeTraceRecords(const std::string& fileName,
                           const std::list<TraceRecordPtr>& traceRecords);
};
}

#endif /* SPL_DEBUGGER_PROBE_POINT_SERVICES_H */
