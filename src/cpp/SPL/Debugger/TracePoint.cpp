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

#include <SPL/Debugger/TracePoint.h>

#include <SPL/Debugger/ProbePointServices.h>
#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/Mutex.h>

#include <iomanip>

#define SET_PREC std::setprecision(std::numeric_limits<SPL::float64>::digits10)

using namespace SPL;
UTILS_NAMESPACE_USE

static UTILS_NAMESPACE::Mutex _SDB_TracePoint_mutex;

TracePoint::TracePoint(ProbePointServices& probePointServices,
                       PEImpl& peHandle,
                       const std::string& opName,
                       PEImpl::PortType portType,
                       uint32_t portIndex)
  : ProbePoint(peHandle, opName, portType, portIndex)
  , _probePointServices(probePointServices)
{
    _maxRecords = 25;
    _enabledTime = getWallClockTime();
    _enabledCount = 0;
    std::stringstream size;
    size << "0/" << _maxRecords;
    setCondition("cached", size.str());
    setCondition("traced", "0 in 00 secs");
}

TracePoint::~TracePoint()
{
    SPCDBG(L_INFO, "Deconstructing tracepoint ...", SPL_APP_DBG);
    _records.clear();
}

bool TracePoint::process(uint32_t hookId, Tuple* tuple, Punctuation* p)
{
    SPCDBG(L_INFO, "Invoking tracepoint callback, hookId(" << hookId << ")", SPL_APP_DBG);
    if (_probePointServices.isShuttingDown()) {
        SPCDBG(L_INFO, "Debugger is shutting down, tracepoint ignored, hookId(" << hookId << ")",
               SPL_APP_DBG);
        return true;
    }
    if (!isEnabled()) {
        SPCDBG(L_INFO, "Tracepoint is currently disabled, hookId(" << hookId << ")", SPL_APP_DBG);
        return true;
    }
    if (_predicate.get() != NULL) {
        SPCDBG(L_INFO, "Evaluating predicate..., hookId(" << hookId << ")", SPL_APP_DBG);
        if (!_predicate->evaluate(_probePointServices, dynamic_cast<ProbePoint*>(this), tuple, p)) {
            SPCDBG(L_INFO,
                   "Tracepoint not fired because predicate evaluated to false, hookId(" << hookId
                                                                                        << ")",
                   SPL_APP_DBG);
            return true;
        }
    }

    _enabledCount++;
    double curTime = getWallClockTime();

    {
        AutoMutex am(_SDB_TracePoint_mutex);
        if (_records.size() + 1 > _maxRecords) {
            _records.pop_front();
        }

        TraceRecordType* traceRecord = new TraceRecordType();
        TraceRecordPtr traceRecordPtr(traceRecord);
        if (tuple != NULL) {
            traceRecordPtr->tuple = tuple->clone();
        } else {
            traceRecordPtr->punct = p;
        }
        traceRecordPtr->arrivalTime = curTime;

        _records.push_back(traceRecordPtr);

    } // End AutoMutex am(_SDB_TracePoint_mutex)

    {
        AutoMutex amLog(_logMutex);
        if (_logStream.is_open()) {
            std::stringstream logText;

            if (tuple != NULL) {
                if (!_loggedMetaData) {
                    _probePointServices.formatTupleKeysCSV(tuple, logText);
                    logText << "\n";
                    _probePointServices.formatTupleDatatypeCSV(tuple, logText);
                    logText << "\n";
                    _loggedMetaData = true;
                }
                _probePointServices.formatTupleCSV(tuple, logText);
            } else {
                logText << "######### "
                        << " --> punctuation marker at time " << SET_PREC << curTime
                        << " value=" << *p;
            }

            _logStream << logText.str() << "\n" << std::flush;
            if (_logStream.fail()) {
                _logStream.close();
                deleteCondition(TRACEPOINT_LOGFILE);
                _logFileName = "";

                std::cout << "Stopped logging Tracepoint " << getOperationName() << ' '
                          << getPortType() << ' ' << getPortIndex() << " to file " << _logFileName
                          << std::endl
                          << std::flush;
            }
        }
    } // End AutoMutex amLog(_logMutex)

    return true;
};

void TracePoint::setEnabled(bool enabled)
{
    bool changedState = (enabled != isEnabled());
    this->ProbePoint::setEnabled(enabled);

    if (isEnabled() && changedState) {
        // Went from disabled to enabled, reset clock and count
        _enabledTime = getWallClockTime();
        _enabledCount = 0;
    }

    {
        AutoMutex amLog(_logMutex);
        if (_logStream.is_open()) {
            double curTime = getWallClockTime();
            if (isEnabled()) {
                _logStream << "# Trace point enabled at time " << SET_PREC << curTime << "\n"
                           << std::flush;
            } else {
                _logStream << "# Trace point disabled at time " << SET_PREC << curTime << "\n"
                           << std::flush;
            }
        }
    } // End AutoMutex amLog(_logMutex)
}

void TracePoint::clear()
{
    _records.clear();
}

std::list<TraceRecordPtr>& TracePoint::getRecords()
{
    AutoMutex am(_SDB_TracePoint_mutex);
    return _records;
}

uint32_t TracePoint::getMaxRecords()
{
    AutoMutex am(_SDB_TracePoint_mutex);
    return _maxRecords;
}

void TracePoint::setMaxRecords(uint32_t maxRecords)
{
    AutoMutex am(_SDB_TracePoint_mutex);
    SPCDBG(L_INFO, "Set tracepoint maxRecords=" << maxRecords, SPL_APP_DBG);
    _maxRecords = maxRecords;
}

std::string TracePoint::getProbePointType() const
{
    return "Tracepoint";
}

ProbePoint::ProbePointConditionsType& TracePoint::getConditions()
{
    AutoMutex am(_SDB_TracePoint_mutex);
    std::stringstream size;
    size << (int)_records.size() << "/" << (int)_maxRecords;
    setCondition("cached", size.str());

    int elapsedTime = (int)(getWallClockTime() - _enabledTime);
    int elapsedMinutes = elapsedTime / 60;
    int elapsedSeconds = elapsedTime % 60;
    std::stringstream tupleCount;
    tupleCount << (int)_enabledCount << " in ";
    if (elapsedMinutes > 0) {
        tupleCount << ((elapsedMinutes < 10) ? "0" : "") << ((int)elapsedMinutes) << ":"
                   << ((elapsedSeconds < 10) ? "0" : "") << ((int)elapsedSeconds) << " mins";
    } else {
        tupleCount << ((elapsedSeconds < 10) ? "0" : "") << ((int)elapsedSeconds) << " secs";
    }

    setCondition("traced", tupleCount.str());
    return _conditions;
}

void TracePoint::startLogging()
{
    std::stringstream logFileName;
    std::string portType;
    if (getPortType() == PEImpl::INPUT) {
        portType = ".i.";
    } else {
        portType = ".o.";
    }

    logFileName << _probePointServices.getPeHandle().getDataDirectory() << "/";
    if (_probePointServices.isStandaloneApplication()) {
        logFileName << "StandaloneApp_";
    } else {
        logFileName << "PE_" << _probePointServices.getPeHandle().getPEId() << "_";
    }
    logFileName << getOperationName() << portType << getPortIndex() << ".log";

    {
        AutoMutex amLog(_logMutex);
        if (_logStream.is_open()) {
            std::cerr << "Cannot start logging, logging is already started for "
                      << getOperationName() << ' ' << getPortType() << ' ' << getPortIndex()
                      << " to file " << _logFileName << std::endl
                      << std::flush;
            return;
        }
        _logStream.open(logFileName.str().c_str());
        if (_logStream.is_open()) {
            _logFileName = logFileName.str();
            _loggedMetaData = false;
            setCondition(TRACEPOINT_LOGFILE, _logFileName);

            std::cout << "Started logging Tracepoint " << getOperationName() << ' ' << getPortType()
                      << ' ' << getPortIndex() << " to file " << logFileName.str() << std::endl
                      << std::flush;
        } else {
            std::cerr << "Failed to properly create output file '" << logFileName << "'"
                      << std::endl
                      << std::flush;
        }
    }
}

void TracePoint::stopLogging()
{
    {
        AutoMutex amLog(_logMutex);
        if (!_logStream.is_open()) {
            std::cerr << "Cannot stop logging, logging is not currently started for "
                      << getOperationName() << ' ' << getPortType() << ' ' << getPortIndex()
                      << std::endl
                      << std::flush;
            return;
        }
        _logStream.close();
        std::cout << "Stopped logging Tracepoint " << getOperationName() << ' ' << getPortType()
                  << ' ' << getPortIndex() << " to file " << _logFileName << std::endl
                  << std::flush;
        deleteCondition(TRACEPOINT_LOGFILE);
        _logFileName = "";
    }
}
