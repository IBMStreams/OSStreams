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

#include <SPL/Debugger/ProbePointServices.h>

#include <SAM/augmentedApplicationModel.h>
#include <SPL/Debugger/ApplicationModel.h>
#include <SPL/Debugger/BreakPoint.h>
#include <SPL/Debugger/InjectPoint.h>
#include <SPL/Debugger/ProbePointManager.h>
#include <SPL/Debugger/ProbePointServices.h>
#include <SPL/Debugger/SdbConsoleLogger.h>
#include <SPL/Debugger/StandaloneClient.h>
#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Type/Meta/BaseType.h>
#include <SPL/Runtime/Type/Meta/Type.h>
#include <UTILS/Base64.h>
#include <UTILS/Directory.h>
#include <UTILS/Mutex.h>
#include <UTILS/RegEx.h>

#include <SPL/Core/META.h>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/tuple/to_list.hpp>

#include <assert.h>
#include <fstream>
#include <iomanip>
#include <list>
#include <sstream>

UTILS_NAMESPACE_USE
using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace Distillery;

extern std::list<DEBUG_NAMESPACE_QUALIFIER Logger*> loggers;

ProbePointServices::ProbePointServices(PEImpl& pe)
  : PEDebugService(pe)
  , _startup(false)
  , _shuttingDown(false)
  , _standaloneClient(NULL)
  , _probePointManager(NULL)
{
    _probePointManager = new ProbePointManager();
    _standaloneClient = new StandaloneClient(*this);
    _applicationModel = new ApplicationModel(pe);

    if (!isStandaloneApplication()) {
        // Deployed PE (not standalone app)
        string wrapper = pe_.getPEModel().executable().wrapper();
        // Debugger is active if the wrapper is SDB
        _debuggerActive = wrapper == "sdb";
    } else {
        // Debugger is active in standalone executable
        _debuggerActive = true;
    }
}

ProbePointServices::~ProbePointServices()
{
    delete _probePointManager;
    delete _standaloneClient;
    delete _applicationModel;
    delete _consoleLogger;
}

void* ProbePointServices::run(void* threadArgs)
{
    SPCDBG(L_INFO, "run - ProbePointServices starting", SPL_APP_DBG);
    if (!isDebuggerActive()) {
        SPCDBG(L_INFO, "run - ProbePointServices ending -- debugger is not active", SPL_APP_DBG);
        return NULL;
    }
    _standaloneClient->init();

    // Trump existing logger for now
    // This sets the logging level for all existing loggers to zero and installs
    // a new logger that works well with the debug environment.
    // We will set the log level for the new logger to zero. The sdb user
    // can use the 'setdbg loglevel' command to change the level as needed.
    set_streams_trclevel(0);
    _consoleLogger = new SdbConsoleLogger(iL_ERROR);
    _consoleLogger->setStandaloneClient(_standaloneClient);
    debug::set_log_logger(_consoleLogger);
    _consoleTracer = new SdbConsoleTracer(iL_ERROR);
    _consoleTracer->setStandaloneClient(_standaloneClient);
    debug::set_app_tracer(_consoleTracer);

    while (!_shuttingDown && _standaloneClient->queryUser()) {
        ;
    }

    SPCDBG(L_INFO, "run - ProbePointServices ending ", SPL_APP_DBG);
    return NULL;
}

// Test if debugger is running in a standalone executable application vs. deployed PE
bool ProbePointServices::isStandaloneApplication()
{
    return pe_.isStandalone();
}

// Note: Once can block here to stop PE code
// from starting to process data
void ProbePointServices::prepareToProcess()
{
    if (!isDebuggerActive()) {
        return; // Debugger is not active, so do not suspend PE (deployed PE not in x11 wrapper)
    }

    if (!isStandaloneApplication()) {
        cout << "PE execution is suspended." << endl << flush;
    } else {
        cout << "Standalone application execution is suspended." << endl << flush;
    }
    cout << "Set initial probe points, then run \"g\" command to continue execution." << endl
         << flush;

    _startup = true;
    SPCDBG(L_TRACE, "ProbePointServices::prepareToProcess(): acquiring startup mutex", SPL_APP_DBG);
    {
        AutoMutex lck(_startup_mutex);
        SPCDBG(L_TRACE, "ProbePointServices::prepareToProcess(): startup mutex acquired",
               SPL_APP_DBG);
        _startup_cv.wait(_startup_mutex);
    }
    SPCDBG(L_TRACE, "ProbePointServices::prepareToProcess(): startup signal received", SPL_APP_DBG);
    _startup = false;
}

void ProbePointServices::prepareToShutdown()
{
    _shuttingDown = true;
    stopDebugger();
}

void ProbePointServices::stopDebugger()
{
    cout << "Debugger stopping" << endl << flush;

    // If debugger is still in started (user has not run "go" command)
    // Signal startup to continue so we can shutdown
    if (isStartup()) {
        cout << "Running \"g\" command" << endl << flush;
        signalStartup();
    }

    //  disable all probepoints
    vector<uint32_t> ids;
    getProbepoints(ids, NULL, NULL, NULL);
    vector<uint32_t>::const_iterator iter;
    for (iter = ids.begin(); iter != ids.end(); iter++) {
        cout << "Disabling breakpoint " << *iter << endl << flush;
        disableProbePoint(*iter);
    }

    // continue all stopped breakpoints so their threads can continue.
    vector<uint32_t> stoppedIds;
    getStoppedBreakpoints(stoppedIds);
    for (iter = stoppedIds.begin(); iter != stoppedIds.end(); iter++) {
        cout << "Continuing breakpoint " << *iter << endl << flush;
        continueRunning(*iter);
    }
    cout << "Debugger stopped" << endl << flush;
}

uint32_t ProbePointServices::setBreakPoint(ProbePointPtr breakPoint)
{
    bool callFirst;

    if (breakPoint->getPortType() == PEImpl::INPUT) {
        callFirst = false;
    } else {
        callFirst = true;
    }

    return _probePointManager->addProbePoint(breakPoint, callFirst);
}

uint32_t ProbePointServices::setInjectPoint(const string& opName,
                                            const string& portType,
                                            uint32_t portIndex)
{
    PEImpl::PortType portTypeEnum;

    if (!portType.compare("i")) {
        portTypeEnum = PEImpl::INPUT;
    } else {
        portTypeEnum = PEImpl::OUTPUT;
    }

    uint32_t hookId = pe_.registerInjectPoint(opName, portTypeEnum, portIndex);
    // Clear the associated tuple
    pe_.retrieveTuple(hookId).reset();

    ProbePointPtr probePoint(new InjectPoint(*this, pe_, opName, portTypeEnum, portIndex));
    _probePointManager->addProbePoint(hookId, probePoint);
    return hookId;
}

uint32_t ProbePointServices::setTracePoint(ProbePointPtr tracePoint)
{
    bool callFirst;

    if (tracePoint->getPortType() == PEImpl::INPUT) {
        callFirst = true;
    } else {
        callFirst = false;
    }

    return _probePointManager->addProbePoint(tracePoint, callFirst);
}

void ProbePointServices::getStoppedBreakpoints(vector<uint32_t>& stoppedProbePointIds) const
{
    vector<uint32_t> allProbePointIds;
    _probePointManager->getProbePoints(allProbePointIds);
    uint32_t hookId;

    for (vector<uint32_t>::iterator iter = allProbePointIds.begin(); iter != allProbePointIds.end();
         iter++) {
        hookId = *iter;
        ProbePointPtr probePoint = _probePointManager->getProbePoint(hookId);
        if (probePoint->isType("Breakpoint")) {
            BreakPoint& breakPoint = dynamic_cast<BreakPoint&>(*probePoint);
            if (breakPoint.isCondition("stopped", "true")) {
                stoppedProbePointIds.push_back(hookId);
            }
        }
    }
}

void ProbePointServices::getStoppedProbepoints(vector<uint32_t>& stoppedProbePointIds) const
{
    vector<uint32_t> allProbePointIds;
    _probePointManager->getProbePoints(allProbePointIds);
    uint32_t hookId;

    for (vector<uint32_t>::iterator iter = allProbePointIds.begin(); iter != allProbePointIds.end();
         iter++) {
        hookId = *iter;
        ProbePointPtr probePoint = _probePointManager->getProbePoint(hookId);
        if (probePoint->isType("Breakpoint")) {
            BreakPoint& breakPoint = dynamic_cast<BreakPoint&>(*probePoint);
            if (breakPoint.isCondition("stopped", "true")) {
                stoppedProbePointIds.push_back(hookId);
            }
        } else if (probePoint->isType("Injectpoint")) {
            stoppedProbePointIds.push_back(hookId);
        }
    }
}

void ProbePointServices::getProbepoints(vector<uint32_t>& probePointIds,
                                        const char* inclProbePointType,
                                        const char* inclKey,
                                        const char* inclValue) const
{
    vector<uint32_t> allProbePointIds;
    _probePointManager->getProbePoints(allProbePointIds);
    uint32_t hookId;

    for (vector<uint32_t>::iterator iter = allProbePointIds.begin(); iter != allProbePointIds.end();
         iter++) {
        hookId = *iter;
        bool include = true;
        ProbePointPtr probePoint = _probePointManager->getProbePoint(hookId);
        if ((inclProbePointType != NULL) && (!probePoint->isType(inclProbePointType))) {
            include = false;
        }
        if (inclKey != NULL) {
            if (inclValue != NULL) {
                // do not include the probe point if the condition key is not set to the condition
                // value
                if (!probePoint->isCondition(inclKey, inclValue)) {
                    include = false;
                }
            } else {
                // Do not include the probe point if the condition key is not set
                if (probePoint->getCondition(inclKey).size() == 0) {
                    include = false;
                }
            }
        }
        if (include) {
            probePointIds.push_back(hookId);
        }
    }
}

// Continue execution for the one, and only, stopped breakpoint
void ProbePointServices::continueRunning() {}

// Continue execution for the specified (stopped) probe point
void ProbePointServices::continueRunning(uint32_t probepointId)
{
    ProbePointPtr probePoint = getProbePoint(probepointId);

    if (probePoint == NULL) {
        cerr << "Invalid probe point ID, " << probepointId << "!" << endl << flush;
        return;
    }

    if (probePoint->isType("Breakpoint")) {
        BreakPoint& breakPoint = dynamic_cast<BreakPoint&>(*probePoint);

        if (breakPoint.isCondition("stopped", "true")) {
            if (breakPoint.isCondition("remove-pending", "true")) {
                _probePointManager->remProbePoint(probepointId);
            }
            breakPoint.signal();
        } else {
            cerr << "Probe point (" << probepointId << ") "
                 << "is not stopped!" << endl
                 << flush;
        }
    } else if (probePoint->isType("Injectpoint")) {
        pe_.submitTuple(probepointId);
        cout << "    Tuple injected " << endl << flush;
    } else {
        cerr << "Specified probe point is a " << probePoint->getProbePointType()
             << ", Breakpoint expected." << endl
             << flush;
    }
}

// Continue execution for the specified (stopped) breakpoint
void ProbePointServices::continueRunning(uint32_t probepointId, BreakPoint& breakPoint)
{
    if (breakPoint.isCondition("stopped", "true")) {
        if (breakPoint.isCondition("remove-pending", "true")) {
            _probePointManager->remProbePoint(probepointId);
        }
        breakPoint.signal();
    } else {
        cerr << "Probe point (" << probepointId << ") "
             << "is not stopped!" << endl
             << flush;
    }
}

// Continue execution for the specified injectPoint
void ProbePointServices::continueRunning(uint32_t probepointId,
                                         InjectPoint& injectPoint,
                                         const string& punctOrder)
{
    bool submitPunct = false;
    bool submitTuple = false;
    bool punctOrderBefore = false;

    if (!punctOrder.compare("o")) {
        submitPunct = true;
    } else if (!punctOrder.compare("b")) {
        submitPunct = true;
        punctOrderBefore = true;
        submitTuple = true;
    } else if (!punctOrder.compare("a")) {
        submitPunct = true;
        submitTuple = true;
    } else {
        submitTuple = true;
    }

    if (submitPunct && punctOrderBefore) {
        pe_.submitPunctuation(probepointId, Punctuation::WindowMarker);
        cout << "    Punctuation injected " << endl << flush;
    }
    if (submitTuple) {
        pe_.submitTuple(probepointId);
        cout << "    Tuple injected " << endl << flush;
    }
    if (submitPunct && !punctOrderBefore) {
        pe_.submitPunctuation(probepointId, Punctuation::WindowMarker);
        cout << "    Punctuation injected " << endl << flush;
    }
}

// Update a tuple value for the one, and only, stopped breakpoint
void ProbePointServices::updateTuple(const string& attributeName, const string& attributeValue)
{
    vector<uint32_t> stoppedIds;
    getStoppedProbepoints(stoppedIds);
    int numStopped = stoppedIds.size();

    if (numStopped == 1) {
        updateTuple(attributeName, attributeValue, stoppedIds.front());
    } else if (numStopped == 0) {
        cerr << "No breakpoint stopped; nothing to update!" << endl << flush;
    } else {
        vector<uint32_t>::const_iterator iter;
        stringstream idsString;
        for (iter = stoppedIds.begin(); iter != stoppedIds.end(); iter++) {
            idsString << *iter << ' ';
        }
        cerr << "Ambiguous. Specify one of the stopped breakpoints: " << idsString.str() << endl
             << flush;
    }
}

// Update a tuple value for a specified (stopped) breakpoint
void ProbePointServices::updateTuple(const string& attributeName,
                                     const string& attributeValue,
                                     uint32_t probepointId)
{
    ProbePointPtr probePoint = getProbePoint(probepointId);

    if (probePoint != NULL) {
        if (probePoint->isType("Breakpoint")) {
            BreakPoint& breakPoint = dynamic_cast<BreakPoint&>(*probePoint);
            if (breakPoint.isCondition("stopped", "false")) {
                cerr << "   Specified break point is not currently stopped." << endl << flush;
                return;
            }
            if (breakPoint.isCondition("dropped", "true")) {
                cerr << "   Tuple has been dropped and can't be updated." << endl << flush;
                return;
            }
            Tuple* tuple;
            if ((tuple = breakPoint.getStoppedTuple()) == NULL) {
                cerr << "   Specified break point contains punctuation!" << endl << flush;
                return;
            }
            updateTuple(tuple, attributeName, attributeValue);
        } else if (probePoint->isType("Injectpoint")) {
            if (!probePoint->isEnabled()) {
                cerr << "   Specified Injectpoint is currently disabled." << endl << flush;
                return;
            }
            Tuple& tuple = pe_.retrieveTuple(probepointId);
            updateTuple(&tuple, attributeName, attributeValue);
        } else {
            cerr << "Specified probe point is a " << probePoint->getProbePointType()
                 << ", Breakpoint expected." << endl
                 << flush;
            return;
        }
    } else {
        cerr << "Specified probe point ID, " << probepointId << ", does not exist." << endl
             << flush;
        return;
    }
}

static void updateError(const string& typeName, const string& value)
{
    cerr << "Can't update value: failed to convert '" << value << "' to type '" << typeName << '\''
         << endl
         << flush;
}

void ProbePointServices::updateTuple(Tuple* tuple,
                                     const string& attributeName,
                                     const string& attributeValue)
{
    assert(tuple != NULL);
    string attrName = attributeName;
    int32_t firstIndex = -1, lastIndex = -1;
    /***
     * Regex parsing for all forms of scalar and indexed list attribute names:
     *     attrName
     *     attrName[a]
     *     attrName[a:b]
     *
     * After parsing, the indices[] array will be set indicating whether one or two subscripts were
     * specified
     */
    static Distillery::RegEx attrbRegex("(^[^\\[]+)($|\\[([0-9]+)\\]|\\[([0-9]+):([0-9]+)\\])$");
    vector<string> matchList;
    attrbRegex.match(attributeName, matchList);
    if (matchList.size() == 0) {
        // failed to match
        cerr << "Invalid attribute '" << attributeName << "' specified." << endl << flush;
        return;
    }

    // grab information from matches
    attrName = matchList[1];
    switch (matchList.size()) {
        case 4: // one index
            firstIndex = atoi(matchList[3].c_str());
            break;
        case 5: // two indicies
            firstIndex = atoi(matchList[3].c_str());
            lastIndex = atoi(matchList[4].c_str());
            break;
    }

    /**
       cout << "        Parsed attribute: " << attrName;
       if (firstIndex >= 0) {
        cout << " with indices[" << firstIndex;
        if (lastIndex >= 0)
            cout << ':' << lastIndex;
        cout << ']';
       }
       cout << endl << endl << flush;
     **/

    // Make sure the attributeName exists in the tuple
    unordered_map<string, uint32_t> const& attributes = tuple->getAttributeNames();
    unordered_map<string, uint32_t>::const_iterator attrIter = attributes.find(attrName);
    if (attrIter == attributes.end()) {
        cerr << "Invalid attribute '" << attrName << "' specified." << endl << flush;
        return;
    }

    // Do the indicies match the type?

    // Get the attributes value and type
    ValueHandle valueHandle = tuple->getAttributeValue(attrName);
    const Meta::BaseType& baseType = Meta::BaseType::makeType(valueHandle);
    Meta::Type mt = baseType.getMetaType();

    switch (mt) {
        case Meta::Type::LIST:
        case Meta::Type::BLIST:
            if (firstIndex >= 0 && lastIndex >= 0 && firstIndex > lastIndex) {
                cerr << "Descending subscripts are not allowed." << endl << flush;
                return;
            }
            break;
        default:
            if (firstIndex >= 0) {
                cerr << "Attribute '" << attrName
                     << "' is not a list, and may not have a subscript." << endl
                     << flush;
                return;
            }
            break;
    }

    // Do the assignment

    // Two cases:  list with subscript
    //             anything else (whole object)
    if (firstIndex >= 0) {
        // List case
        SPL::List& l = valueHandle;
        int32_t size = l.getSize();
        if (lastIndex == -1) {
            lastIndex = firstIndex + 1;
        } else if (lastIndex > size) {
            lastIndex = size;
        }
        if (firstIndex >= size) {
            firstIndex = size;
        }
        const Meta::BaseType& baseType1 = Meta::BaseType::makeType(l.getElement(0));
        try {
            for (size_t i = firstIndex; i < (size_t)lastIndex; i++) {
                istringstream str(attributeValue);
                ValueHandle e = l.getElement(i);
                e.deserializeWithNanAndInfs(str, true);
                if (str.fail()) {
                    updateError(baseType1.getName(), attributeValue);
                    return;
                }
            }
        } catch (...) {
            updateError(baseType1.getName(), attributeValue);
            return;
        }
    } else {
        // Whole object
        try {
            istringstream str(attributeValue);
            valueHandle.deserializeWithNanAndInfs(str, true);
            if (str.fail()) {
                updateError(baseType.getName(), attributeValue);
                return;
            }
        } catch (...) {
            updateError(baseType.getName(), attributeValue);
            return;
        }
    }

    // Print out the tuple so that the user can validate the setting
    _standaloneClient->printTuple(tuple);
}

void ProbePointServices::getProbePoints(vector<uint32_t>& probePointsVector) const
{
    return _probePointManager->getProbePoints(probePointsVector);
}

ProbePointPtr ProbePointServices::getProbePoint(uint32_t probePointId)
{
    return _probePointManager->getProbePoint(probePointId);
}

int ProbePointServices::getProbePoint(const string& opName,
                                      const string& opType,
                                      PEImpl::PortType portType,
                                      uint32_t portIndex)
{
    return _probePointManager->getProbePoint(opName, opType, portType, portIndex);
}

void ProbePointServices::remProbePoint(uint32_t probePointId)
{
    _probePointManager->remProbePoint(probePointId);
}

void ProbePointServices::enableProbePoint(uint32_t probePointId)
{
    ProbePointPtr probePoint = _probePointManager->getProbePoint(probePointId);
    if (probePoint == NULL) {
        cerr << "Invalid probe point ID specified" << endl << flush;
        return;
    }
    if (!probePoint->isEnabled()) {
        probePoint->setEnabled(true);
    } else {
        // cerr << "Probe point " << probePointId << " is already enabled" << endl << flush;
        return;
    }
}

void ProbePointServices::disableProbePoint(uint32_t probePointId)
{
    ProbePointPtr probePoint = _probePointManager->getProbePoint(probePointId);
    if (probePoint == NULL) {
        cerr << "Invalid probe point ID specified" << endl << flush;
        return;
    }
    if (probePoint->isEnabled()) {
        probePoint->setEnabled(false);
    } else {
        // cerr << "Probe point " << probePointId << " is already disabled" << endl << flush;
        return;
    }
}

void ProbePointServices::saveProbePoint(uint32_t probePointId, const string& fileName)
{
    ProbePointPtr probePoint = _probePointManager->getProbePoint(probePointId);
    if (probePoint == NULL) {
        cerr << "Invalid probe point ID specified" << endl << flush;
        return;
    }
    if (probePoint->isType("Breakpoint")) {
        BreakPoint& breakPoint = dynamic_cast<BreakPoint&>(*probePoint);
        if (breakPoint.isCondition("stopped", "true")) {
            std::list<TraceRecordPtr> records;

            // A little hack, put the stopped tuple for this breakpoint
            // into a list of trace records and save it using the save trace records function
            TraceRecordType* traceRecord = new TraceRecordType();
            TraceRecordPtr traceRecordPtr(traceRecord);
            traceRecordPtr->tuple = breakPoint.getStoppedTuple()->clone();
            records.push_back(traceRecordPtr);
            writeTraceRecords(fileName, records);
        } else {
            cerr << "Breakpoint " << probePointId << " is not stopped, nothing to save" << endl
                 << flush;
        }
    } else if (probePoint->isType("Tracepoint")) {
        TracePoint& tracePoint = dynamic_cast<TracePoint&>(*probePoint);
        writeTraceRecords(fileName, tracePoint.getRecords());
    } else {
        cerr << "Probe point type, " << probePoint->getProbePointType()
             << ", not allowed for this command" << endl
             << flush;
    }
}

void ProbePointServices::saveCfg(const string& cfgName)
{
    string outputFileName;

    outputFileName.append(pe_.getDataDirectory());
    outputFileName.append("/").append(cfgName).append("_debug.cfg");

    ofstream of;
    auto_ptr<ostream> o;

    of.open(outputFileName.c_str());
    if (!of) {
        cerr << "Failed to open output file '" << outputFileName << "'" << endl << flush;
        return;
    }

    // Setting output stream
    o.reset(new ostream(of.rdbuf()));

    vector<uint32_t> allProbePointIds;
    _probePointManager->getProbePoints(allProbePointIds);
    uint32_t hookId;
    uint32_t count = 0;

    for (vector<uint32_t>::iterator iter = allProbePointIds.begin(); iter != allProbePointIds.end();
         iter++) {
        hookId = *iter;
        stringstream outputLine;
        ProbePointPtr probePoint = _probePointManager->getProbePoint(hookId);

        // don't save any probe points that are pending removal.
        if (probePoint->isCondition("remove-pending", "true")) {
            continue;
        }

        string probePointType;
        if (probePoint->isType("Breakpoint")) {
            probePointType = "b ";
        }
        if (probePoint->isType("Injectpoint")) {
            probePointType = "i ";
        }
        if (probePoint->isType("Tracepoint")) {
            probePointType = "t ";
        } else {
            assert("invalid ProbePoint type!");
        }

        outputLine << probePointType << probePoint->getOperationName() << " "
                   << ((probePoint->getPortType() == PEImpl::INPUT) ? " i " : " o ")
                   << probePoint->getPortIndex() << " ";
        if (!probePoint->isEnabled()) {
            outputLine << "d ";
        }

        if (probePoint->isType("Tracepoint")) {
            TracePoint& tracepoint = dynamic_cast<TracePoint&>(*probePoint);
            outputLine << tracepoint.getMaxRecords() << " ";
            if (tracepoint.getCondition(TRACEPOINT_LOGFILE).size() > 0) {
                //       ProbePoint::ProbePointConditionsType & conditions =
                //       tracePoint.getConditions();
                // if (conditions.find(TRACEPOINT_LOGFILE) != conditions.end()) {
                outputLine << "log ";
            }
        }

        if (probePoint->getPredicate().get() != NULL) {
            outputLine << " p " << probePoint->getPredicate()->getExpression();
        }

        (*o) << outputLine.str() << "\n" << flush;
        if (of.fail()) {
            cerr << "Failed to write probe point definitions to output file '" << outputFileName
                 << "'" << endl
                 << flush;
            return;
        }
        count++;
    }

    of.close();
    cout << "Wrote " << count << " probe point definitions to file " << outputFileName << endl
         << flush;
}

void ProbePointServices::loadCfg(const string& cfgName)
{
    string inputFileName = pe_.getDataDirectory();
    inputFileName.append("/").append(cfgName).append("_debug.cfg");
    string inputLine;
    string commandName;

    ifstream stream(inputFileName.c_str());
    if (stream.is_open()) {
        while (!stream.eof()) {
            getline(stream, inputLine);
            if (inputLine.size() > 0 && inputLine.substr(0, 1).compare("#") != 0) {
                commandName = trim(inputLine.substr(0, inputLine.find_first_of(' ')));
                if (commandName.compare("loadcfg") != 0 && commandName.compare("savecfg") != 0) {
                    istringstream command(inputLine);
                    _standaloneClient->executeCommand(command);
                }
            }
        }
        stream.close();
    } else {
        cerr << "Failed to open input file '" << inputFileName << "'" << endl << flush;
    }
}

void ProbePointServices::startLogging(uint32_t probePointId)
{
    ProbePointPtr probePoint = _probePointManager->getProbePoint(probePointId);
    if (probePoint == NULL) {
        cerr << "Invalid probe point ID specified" << endl << flush;
        return;
    }
    if (probePoint->isType("Tracepoint")) {
        TracePoint& tracePoint = dynamic_cast<TracePoint&>(*probePoint);
        string logFile = tracePoint.getCondition("logfile");
        if (logFile.size() > 0) {
            cerr << "Tracepoint " << probePointId << " is already logging to " << logFile << endl
                 << flush;
        } else {
            tracePoint.startLogging();
        }
    } else {
        cerr << "Probe point type, " << probePoint->getProbePointType()
             << ", not allowed for this command" << endl
             << flush;
    }
}

void ProbePointServices::stopLogging(uint32_t probePointId)
{
    ProbePointPtr probePoint = _probePointManager->getProbePoint(probePointId);
    if (probePoint == NULL) {
        cerr << "Invalid probe point ID specified" << endl << flush;
        return;
    }
    if (probePoint->isType("Tracepoint")) {
        TracePoint& tracePoint = dynamic_cast<TracePoint&>(*probePoint);
        string logFile = tracePoint.getCondition("logfile");
        if (logFile.size() == 0) {
            cerr << "Logging is not active for Tracepoint is " << probePointId << endl << flush;
        } else {
            tracePoint.stopLogging();
        }
    } else {
        cerr << "Probe point type, " << probePoint->getProbePointType()
             << ", not allowed for this command" << endl
             << flush;
    }
}

void ProbePointServices::clearProbePoint(uint32_t probePointId)
{
    ProbePointPtr probePoint = _probePointManager->getProbePoint(probePointId);
    if (probePoint == NULL) {
        cerr << "Invalid probe point ID specified" << endl << flush;
        return;
    }
    if (probePoint->isType("Tracepoint")) {
        TracePoint& tracePoint = dynamic_cast<TracePoint&>(*probePoint);
        tracePoint.clear();
    } else {
        cerr << "Probe point type, " << probePoint->getProbePointType()
             << ", not allowed for this command" << endl
             << flush;
    }
}

void ProbePointServices::showProbePoint(uint32_t probePointId)
{
    ProbePointPtr probePoint = _probePointManager->getProbePoint(probePointId);
    if (probePoint == NULL) {
        cerr << "Invalid probe point ID specified" << endl << flush;
        return;
    }
    if (probePoint->isType("Breakpoint")) {
        BreakPoint& breakPoint = dynamic_cast<BreakPoint&>(*probePoint);
        _standaloneClient->printProbePoint(probePointId, probePoint.get());
        if (breakPoint.isCondition("stopped", "true")) {
            _standaloneClient->printTuple(breakPoint.getStoppedTuple());
        }
    } else if (probePoint->isType("Injectpoint")) {
        _standaloneClient->printProbePoint(probePointId, probePoint.get());
        _standaloneClient->printTuple(&pe_.retrieveTuple(probePointId));
    } else if (probePoint->isType("Tracepoint")) {
        TracePoint& tracePoint = dynamic_cast<TracePoint&>(*probePoint);
        std::list<TraceRecordPtr>::const_iterator iter;
        std::list<TraceRecordPtr>& traceRecords = tracePoint.getRecords();
        _standaloneClient->printProbePoint(probePointId, probePoint.get());
        for (iter = traceRecords.begin(); iter != traceRecords.end(); iter++) {
            TraceRecordPtr traceRecord = *iter;
            if (traceRecord->tuple != NULL) {
                _standaloneClient->printTuple(traceRecord->tuple);
            } else {
                _standaloneClient->printPunctuation(*traceRecord->punct);
            }
        }
    } else {
        cerr << "Probe point type, " << probePoint->getProbePointType()
             << ", not allowed for this command" << endl
             << flush;
    }
}

//* Output probe point as records
void ProbePointServices::showProbePointRecords(uint32_t probePointId)
{
    ProbePointPtr probePoint = _probePointManager->getProbePoint(probePointId);
    if (probePoint == NULL) {
        cerr << "Invalid probe point ID specified" << endl << flush;
        return;
    }
    if (probePoint->isType("Breakpoint")) {
        BreakPoint& breakPoint = dynamic_cast<BreakPoint&>(*probePoint);
        _standaloneClient->printProbePoint(probePointId, probePoint.get());
        if (breakPoint.isCondition("stopped", "true")) {
            _standaloneClient->printTupleRecord(breakPoint.getStoppedTuple());
        }
    } else if (probePoint->isType("Tracepoint")) {
        TracePoint& tracePoint = dynamic_cast<TracePoint&>(*probePoint);
        std::list<TraceRecordPtr>::const_iterator iter;
        std::list<TraceRecordPtr>& traceRecords = tracePoint.getRecords();

        if (traceRecords.size() == 0) {
            return;
        }
        _standaloneClient->printProbePoint(probePointId, probePoint.get());
        // Print out header for tuple
        TraceRecordPtr traceRecord = *(traceRecords.begin());
        _standaloneClient->printTupleRecordHeader(traceRecord->tuple);

        for (iter = traceRecords.begin(); iter != traceRecords.end(); iter++) {
            traceRecord = *iter;
            if (traceRecord->tuple != NULL) {
                _standaloneClient->printTupleRecord(traceRecord->tuple);
            } else {
                _standaloneClient->printPunctuation(*traceRecord->punct);
            }
        }
    } else {
        cerr << "Probe point type, " << probePoint->getProbePointType()
             << ", not allowed for this command" << endl
             << flush;
    }
}

/*
 * Print the contents of the given tuple and corresponding probe point via the Standalone client.
 */
void ProbePointServices::printTuple(uint32_t id, ProbePoint* probePoint, Tuple* tuple)
{
    _standaloneClient->asyncPrintTuple(id, probePoint, tuple);
}

/*
 * Print the contents of the given punctuation and corresponding probe point via the Standalone
 * client.
 */
void ProbePointServices::printPunctuation(uint32_t id, ProbePoint* probePoint, Punctuation& p)
{
    _standaloneClient->asyncPrintPunctuation(id, probePoint, p);
}

/*
 * Print a message via the Standalone client.
 */
void ProbePointServices::printMessage(const string& message)
{
    _standaloneClient->asyncPrintMessage(message);
}

void ProbePointServices::formatTupleCSV(Tuple* tuple, stringstream& buffer)
{
    buffer.precision(32);
    bool firstValue = true;
    uint32_t nattr = tuple->getNumberOfAttributes();
    for (uint32_t i = 0; i < nattr; ++i) {
        if (!firstValue) {
            buffer << ",";
        }
        firstValue = false;
        string attributeKey = tuple->getAttributeName(i);
        ConstValueHandle attributeValueHandle(tuple->getAttributeValue(i));
        buffer << attributeValueHandle.toString();
    }
}

void ProbePointServices::formatTupleKeysCSV(Tuple* tuple, stringstream& buffer)
{
    buffer << "# ";
    bool firstValue = true;
    uint32_t nattr = tuple->getNumberOfAttributes();
    for (uint32_t i = 0; i < nattr; ++i) {
        if (!firstValue) {
            buffer << ",";
        }
        firstValue = false;
        buffer << tuple->getAttributeName(i);
    }
}

void ProbePointServices::formatTupleDatatypeCSV(Tuple* tuple, stringstream& buffer)
{
    buffer << "# ";
    bool firstValue = true;
    uint32_t nattr = tuple->getNumberOfAttributes();
    for (uint32_t i = 0; i < nattr; ++i) {
        if (!firstValue) {
            buffer << ",";
        }
        firstValue = false;
        ConstValueHandle attributeValueHandle(tuple->getAttributeValue(i));
        Meta::BaseType const& type = Meta::BaseType::makeType(attributeValueHandle);
        buffer << type.getName();
    }
}

void ProbePointServices::writeTraceRecords(const string& oFileName,
                                           const std::list<TraceRecordPtr>& traceRecords)
{
    string rn;
    string outputFileName;
    try {
        rn = Directory::srealpath(trim(oFileName));
    } catch (DirectoryException& e) {
        // fine
    }

    if (trim(oFileName) == rn) {
        outputFileName = rn;
    } else {
        outputFileName.append(pe_.getDataDirectory());
        outputFileName.append("/").append(oFileName);
    }

    if (outputFileName.empty()) {
        cerr << "No file specified" << endl << flush;
        return;
    } else {
        SPCDBG(L_INFO, "using '" << outputFileName << "' as the output file", SPL_APP_DBG);
    }
    cout << "writing to file " << outputFileName << endl << flush;

    ofstream of;
    auto_ptr<ostream> o;

    of.open(outputFileName.c_str());
    if (!of) {
        cerr << "failed to properly create output file '" << outputFileName << "'" << endl << flush;
        return;
    }

    // Setting output stream
    o.reset(new ostream(of.rdbuf()));

    uint32_t tupleCount = 0;
    uint32_t punctCount = 0;
    std::list<TraceRecordPtr>::const_iterator recordIter;
    Tuple* tuple;
    for (recordIter = traceRecords.begin(); recordIter != traceRecords.end(); recordIter++) {
        TraceRecordPtr traceRecord = *recordIter;
        tuple = traceRecord->tuple;
        if (tuple != NULL) {
            stringstream texttuple;

            if (tupleCount == 0) {
                // First tuple to write, first write attrnames and types
                formatTupleKeysCSV(tuple, texttuple);
                texttuple << "\n";
                formatTupleDatatypeCSV(tuple, texttuple);
                texttuple << "\n";
            }

            formatTupleCSV(tuple, texttuple);

            (*o) << texttuple.str() << "\n" << flush;
            if (of.fail()) {
                cerr << "failed to write tuple to output file '" << outputFileName << "'" << endl
                     << flush;
                return;
            }
            tupleCount++;
        } else {
            stringstream str;
            str << "######### "
                << " --> punctuation marker at time "
                << setprecision(numeric_limits<SPL::float64>::digits10) << traceRecord->arrivalTime
                << " value=" << *traceRecord->punct;
            (*o) << str.str() << "\n" << flush;
            if (of.fail()) {
                cerr << "failed to write punctuation to output file '" << outputFileName << "'"
                     << endl
                     << flush;
            }
            punctCount++;
        }
    }
    of.close();
}
