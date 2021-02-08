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

#ifndef SPL_DEBUGGER_STAND_ALONE_CLIENT_H
#define SPL_DEBUGGER_STAND_ALONE_CLIENT_H

#include <SPL/Debugger/ProbePoint.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/ProcessingElement/PE.h>

#include <iostream>
#include <libtecla.h>
#include <sstream>
#include <string>
#include <utility>

#define DEFAULT_INDENT 3

namespace SPL {

class ProbePointServices;
class OperatorImpl;
namespace Meta {
class BaseType;
};
/*!
 * The StandaloneClient class provides a simple user interface for
 * managing the debug session.
 *
 * It is assumed that this client is resides in the SPL runtime library,
 * and is used for debugging standalone applications, only.
 */
class StandaloneClient
{
    inline std::string getPrompt() { return "(sdb) "; }

    inline int getIndent() { return 3; }

    /// Commands supported by the debugger
    enum Commands
    {
        HELP = 0,           /* h */
        GO,                 /* g */
        QUIT,               /* q */
        CONTINUE,           /* c */
        SET_BREAKPOINT,     /* b */
        SET_INJECTPOINT,    /* i */
        SET_TRACEPOINT,     /* t */
        SET_PREDICATE,      /* p */
        LIST_PROBEPOINTS,   /* l */
        LIST_OPERATORS,     /* o */
        FIND_FIELDS,        /* find */
        ENABLE_PROBEPOINT,  /* e */
        DISABLE_PROBEPOINT, /* d */
        REMOVE_PROBEPOINT,  /* r */
        SHOW_TRACEPOINT,    /* s */
        UPDATE_TUPLE,       /* u */
        DROP_TUPLE,         /* x */
        CLEAR,              /* clear */
        LOG,                /* log */
        SAVE,               /* save */
        SAVECFG,            /* savecfg */
        LOADCFG,            /* loadcfg */
        SETDBG,             /* setdbg */
        GDB                 /* gdb */
    };

    typedef std::map<Commands, std::string> CommandsUsageInfoType;
    CommandsUsageInfoType _commandsUsageInfo;
    ProbePointServices& _probePointServices;

    GetLine* _gl;            /* libtecla's command line resource object */
    int _outputEventPipe[2]; /* pipe used to post tecla's callback function */

  public:
    /// Constructor
    /// @param probePointServices Probe Point Services
    StandaloneClient(ProbePointServices& probePointServices);

    /// Destructor
    ~StandaloneClient();

    /// Print one probe point
    /// @param hookId Probe point id
    /// @param probePoint Probe point to print
    void printProbePoint(uint32_t hookId, ProbePoint* probePoint);

    /// Print a tuple - using tecla getline
    /// @param hookId Probe point id
    /// @param probePoint Probe point associated with hookId
    /// @param tuple Tuple to print
    void asyncPrintTuple(uint32_t hookId, ProbePoint* probePoint, Tuple* tuple);

    /// Print a tuple to the console
    /// @param tuple Tuple to print
    void printTuple(Tuple* tuple);

    /// Print a tuple record to the console
    /// @param tuple Tuple to print
    void printTupleRecord(Tuple* tuple);

    /// Print a tuple header to the console
    /// @param tuple Tuple to print
    void printTupleRecordHeader(Tuple* tuple);

    /// Print a punctuation - using tecla getline
    /// @param hookId Probe point id
    /// @param probePoint Probe point associated with hookId
    /// @param punct Punctuation to print
    void asyncPrintPunctuation(uint32_t hookId, ProbePoint* probePoint, Punctuation& punct);

    /// Print a punctuation to the console
    /// @param punct Punctuation to print
    void printPunctuation(Punctuation& punct);

    /// Initialize Standalone client
    void init();

    /// Read and execute one line of user input
    /// @return false if the debugger should stop
    bool queryUser();

    /// Execute one SDB command
    /// @param inputCommand Command to execute
    void executeCommand(std::istringstream& inputCommand);

    /// print a message using tecla interface
    /// @param message Message to print
    void asyncPrintMessage(const std::string& message);

  private:
    std::string getDatatypeString(const Meta::BaseType& dataType);
    void printOperators();
    void printOperator(const std::string& opName, PEImpl::PortType portType, uint32_t portIndex);
    void printProbePoints();
    bool isProbePointArgsValid(const std::string& opName,
                               const std::string& opType,
                               const std::string& portType,
                               uint32_t portIndex);
    PEImpl::PortType getPortTypeEnum(const std::string& portTypeString);
    void printProbePointStatus(uint32_t id, const char* statusMsg);

    // Given an attribute and a maxLength, truncate the attribute to the maxLength
    inline std::string truncateAttributeField(const std::string& attribute, int maxLength)
    {
        if (attribute.length() < (uint32_t)maxLength) {
            return attribute;
        }
        return attribute.substr(0, maxLength - 3) + "..";
    }

    void formatProbePoint(uint32_t hookId, ProbePoint* probePoint, std::string* outputString);
    void formatTuple(Tuple* tuple, std::string* outputString);
    void formatTupleRecord(Tuple* tuple, std::string* outputString);
    void formatTupleRecordHeader(Tuple* tuple, std::string* outputString);
    void formatPunctuation(Punctuation& p, std::string* outputString);
    void asyncPrintProbePoint(uint32_t hookId, ProbePoint* probePoint);
    void asyncPrintTuple(Tuple* tuple);
    void asyncPrintPunctuation(Punctuation& p);
    void getExecutableLocation(std::string& exeDirPath, std::string& exeFileName);
    int createGdbCommands(ProbePointPtr probePoint, std::string& outputFileName);
    int createGdbCommands(const std::string& opName, std::string& outputFileName);
    std::map<std::string, OperatorImpl*> _opPropMap;
};

}

#endif // SPL_DEBUGGER_STAND_ALONE_CLIENT_H
