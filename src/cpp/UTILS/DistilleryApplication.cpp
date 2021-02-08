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

#include <fcntl.h>
#include <poll.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <ctype.h>
#include <getopt.h>

#include <xercesc/util/PlatformUtils.hpp>

#include <TRC/ColorConsoleTracer.h>
#include <TRC/ConsoleTracer.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/FilteredThColorConsoleTracer.h>
#include <TRC/LogFactory.h>
#include <TRC/MemoryTracer.h>
#include <TRC/RingTracer.h>
#include <TRC/RollingFileLogger.h>
#include <TRC/RollingFileTracer.h>
#include <TRC/RuntimeTrcAspects.h>
#include <TRC/TRCUtils.h>
#include <TRC/ThColorConsoleTracer.h>
#include <UTILS/BacktraceDumper.h>
#include <UTILS/DSTAutoConf.h>
#include <UTILS/Directory.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/DistilleryKnobs.h>
#include <UTILS/Formatter.h>
#include <UTILS/HostInfo.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/auto_array.h>
#ifdef DST_DBG_INST
#include <UTILS/SymbolReader.h>
#endif
#include <UTILS/Product.h>
#include <boost/algorithm/string.hpp>
#include <langinfo.h>
#include <unicode/locid.h>

UTILS_NAMESPACE_USE;
using namespace std;
XERCES_CPP_NAMESPACE_USE
DEBUG_NAMESPACE_USE

extern Distillery::DistilleryApplication* instantiate_new_app(void);

DistilleryApplication* DistilleryApplication::app = NULL;
// see "man 5 environ" for details
extern char** environ;

DistilleryApplication::int_option_t DistilleryApplication::int_ign;
DistilleryApplication::str_option_t DistilleryApplication::str_ign;
DistilleryApplication* DistilleryApplication::_thisApp = NULL;
int DistilleryApplication::_connector_max_retries = 60;
const string STREAMS_INITIAL_TRACE_LEVEL = "STREAMS_INITIAL_TRACE_LEVEL";
static debug::Tracer* _stdouterr_tracer = NULL;

typedef struct _atexit_list_t
{
    void (*fct)(void*);
    void* arg;
    struct _atexit_list_t* next;
} atexit_list_t;

static atexit_list_t* atexit_list = 0;

const char* DistilleryApplication::SIGNAL_NAMES[] = { "????",
                                                      /* 1 */ "SIGHUP",
                                                      /* 2 */ "SIGINT",
                                                      /* 3 */ "SIGQUIT",
                                                      /* 4 */ "SIGILL",
                                                      /* 5 */ "SIGTRAP",
                                                      /* 6 */ "SIGABRT",
                                                      /* 7 */ "SIGBUS",
                                                      /* 8 */ "SIGFPE",
                                                      /* 9 */ "SIGKILL",
                                                      /* 10 */ "SIGUSR1",
                                                      /* 11 */ "SIGSEGV",
                                                      /* 12 */ "SIGUSR2",
                                                      /* 13 */ "SIGPIPE",
                                                      /* 14 */ "SIGALRM",
                                                      /* 15 */ "SIGTERM",
                                                      /* 16 */ "SIGSTKFLT",
                                                      /* 17 */ "SIGCHLD",
                                                      /* 18 */ "SIGCONT",
                                                      /* 19 */ "SIGSTOP",
                                                      /* 20 */ "SIGTSTP",
                                                      /* 21 */ "SIGTTIN",
                                                      /* 22 */ "SIGTTOU",
                                                      /* 23 */ "SIGURG",
                                                      /* 24 */ "SIGXCPU",
                                                      /* 25 */ "SIGXFSZ",
                                                      /* 26 */ "SIGVTALRM",
                                                      /* 27 */ "SIGPROF",
                                                      /* 28 */ "SIGWINCH",
                                                      /* 29 */ "SIGIO",
                                                      /* 30 */ "SIGPWR",
                                                      /* 31 */ "SIGSYS",
                                                      /* 34 */ "SIGRTMIN",
                                                      /* 35 */ "SIGRTMIN+1",
                                                      /* 36 */ "SIGRTMIN+2",
                                                      /* 37 */ "SIGRTMIN+3",
                                                      /* 38 */ "SIGRTMIN+4",
                                                      /* 39 */ "SIGRTMIN+5",
                                                      /* 40 */ "SIGRTMIN+6",
                                                      /* 41 */ "SIGRTMIN+7",
                                                      /* 42 */ "SIGRTMIN+8",
                                                      /* 43 */ "SIGRTMIN+9",
                                                      /* 44 */ "SIGRTMIN+10",
                                                      /* 45 */ "SIGRTMIN+11",
                                                      /* 46 */ "SIGRTMIN+12",
                                                      /* 47 */ "SIGRTMIN+13",
                                                      /* 48 */ "SIGRTMIN+14",
                                                      /* 49 */ "SIGRTMIN+15",
                                                      /* 50 */ "SIGRTMAX-14",
                                                      /* 51 */ "SIGRTMAX-13",
                                                      /* 52 */ "SIGRTMAX-12",
                                                      /* 53 */ "SIGRTMAX-11",
                                                      /* 54 */ "SIGRTMAX-10",
                                                      /* 55 */ "SIGRTMAX-9",
                                                      /* 56 */ "SIGRTMAX-8",
                                                      /* 57 */ "SIGRTMAX-7",
                                                      /* 58 */ "SIGRTMAX-6",
                                                      /* 59 */ "SIGRTMAX-5",
                                                      /* 60 */ "SIGRTMAX-4",
                                                      /* 61 */ "SIGRTMAX-3",
                                                      /* 62 */ "SIGRTMAX-2",
                                                      /* 63 */ "SIGRTMAX-1",
                                                      /* 64 */ "SIGRTMAX" };

static void atexit_handler(void)
{
    atexit_list_t* ptr = atexit_list;

    while (ptr) {
        ptr->fct(ptr->arg);
        atexit_list_t* next = ptr->next;
        delete ptr;
        ptr = next;
    }
}

DistilleryApplication::DistilleryApplication()
  : _handler_key(0)
  , _keep_remaining_args(false)
  , _argv_0("<undefined>")
  , _argv(NULL)
  , _envpfence(NULL)
  , _savedArgv(NULL)
  , _argvState(false)
  , _envpsize(0)
  , _argc(0)
  , _install_signal_handler(true)
  , _call_destructor(true)
  , _spc_tracer(NULL)
  , _trc_settings("")
  , _tracerSwitched(false)
  , _log_logger(NULL)
  , _init_log_level(iL_WARN)
  , _log_settings("")
  , _stdouterr_settings("")
  , _stdouterrThread(NULL)
  , _audit_logger(NULL)
  , _respawn_argv(NULL)
  , _respawn_argc(0)
  , _testMode(false)
{
    _thisApp = this;

    string initTraceLevel = get_environment_variable(STREAMS_INITIAL_TRACE_LEVEL, "off");
    _init_trc_level = levelFromStr2Int(initTraceLevel);
    _spc_tracer = new debug::MemoryTracer;
    debug::set_streams_tracer(_spc_tracer);
    debug::set_streams_trclevel(_init_trc_level);

    atexit(atexit_handler);
}

DistilleryApplication::~DistilleryApplication()
{
    for (int i = 0; i < _argc; ++i) {
        delete[] _savedArgv[i];
    }
    delete[] _savedArgv;

    for (int i = 0; i < _respawn_argc; ++i) {
        delete[] _respawn_argv[i];
    }
    delete[] _respawn_argv;

    if (_spc_tracer) {
        if (!app->isTracerSwitched() && !(_spc_tracer->getMessages()).empty()) {
            // If we do not have chance to set up a "real" logger yet
            // but service shutdown (This generally happen when service crash),
            // printout logging messages to stdout.
            cout << "Log messages from service that not written to log file:" << endl
                 << _spc_tracer->getMessages() << endl;
        }
        debug::set_streams_tracer(NULL);
        delete _spc_tracer;
        _spc_tracer = NULL;
    }
    if (_log_logger) {
        debug::set_log_logger(NULL);
        delete _log_logger;
        _log_logger = NULL;
    }
    if (_audit_logger) {
        debug::set_audit_logger(NULL);
        delete _audit_logger;
        _audit_logger = NULL;
    }

    if (_stdouterr_tracer) {
        // Redirect stdout and stderr to the original file handler.
        // No error checking needed.
        dup2(_originalStdoutFD, stdout->_fileno);
        dup2(_originalStderrFD, stderr->_fileno);

        // close pipe to trigger stdout/stderr tracing management thread to end.
        close(_stdouterrPipe[1]);
    }

    // if(NULL != _stdouterrThread)
    //{
    // No join needed, as thread created with PTHREAD_CREATE_DETACHED

    // delete _stdouterrThread;
    //_stdouterrThread = NULL;
    //}
}

ostream& DistilleryApplication::error(const string& message)
{
    cerr << _argv_0 << ": " << message;
    return cerr;
}

void DistilleryApplication::exit(const string& message, int code)
{
    error(message) << endl;
    exit(code);
    ::exit(code); // will never happen, but gcc gets confused
}

void DistilleryApplication::exit(int code)
{
    ::exit(code);
}

int DistilleryApplication::init()
{
    return 0;
}

void DistilleryApplication::addAtExit(void (*fct)(void*), void* arg)
{
    atexit_list_t* elem = new atexit_list_t;

    elem->fct = fct;
    elem->arg = arg;
    elem->next = atexit_list;
    atexit_list = elem;
}

void DistilleryApplication::replaceArgv(const int which, char const* str, const bool mangleArgv)
{
    // WHEN MANGLEARGV IS FALSE:
    // with the exception of argv[0], replacing other args only produces the
    // desired result for 'ps' (i.e., replacing the arg) when the new arg is
    // equal or smaller in size than the original arg. If it is larger, than
    // 'ps' displays only the first arg correctly (Suse 8.1 kernel
    // 2.4.21-231-default)
    // WHEN MANGLEARGV IS true (which is the default):
    // we obtain the right result as far as 'ps' is concerned, but argv gets
    // mangled up (because \0 are replace with ' ')
    if (which >= _argc) {
        return;
    }
    int adj = strlen(str) - strlen(_savedArgv[which]);
    if (_argv[which] + strlen(str) > _envpfence) {
        char* last = (char*)_envpfence;
        strncpy(_argv[which], str, last - _argv[which] - 1);
        *last = '\0';
        // oh well, we are losing the other args
        if (_argvState && which != _argc - 1) {
            _argvState = false;
        }

        // avoid mem leaks
        for (int j = which + 1; j < _argc; ++j) {
            delete[] _savedArgv[j];
            _savedArgv[j] = NULL;
        }

        _argc = which + 1;
    } else {
        if (adj < 0) {
            memset(_argv[which], ' ', strlen(_savedArgv[which]) - adj);
        }
        strcpy(_argv[which], str);
    }
    for (int i = which + 1; i < _argc; ++i) {
        _argv[i] = _argv[i] + adj;
        if (_argv[i] + strlen(_savedArgv[i]) > _envpfence) {
            char* last = (char*)_envpfence;
            strncpy(_argv[i], _savedArgv[i], last - _argv[i] - 1);
            *last = '\0';
            // oh well, we are losing the other args
            if (_argvState && i != _argc - 1) {
                _argvState = false;
            }

            // avoid mem leaks
            for (int j = i + 1; j < _argc; ++j) {
                delete[] _savedArgv[j];
                _savedArgv[j] = NULL;
            }

            _argc = i + 1;
        } else {
            if (adj < 0) {
                memset(_argv[i], ' ', strlen(_savedArgv[i]) - adj);
            }
            strcpy(_argv[i], _savedArgv[i]);
        }
    }
    saveArgs(_argc, _argv);
    if (mangleArgv) {
        for (int i = 1; i < _argc; ++i) {
            *(_argv[i] - 1) = ' ';
        }
    }
}

void DistilleryApplication::printArgs(void) const
{
    for (int i = 0; i < _argc; ++i) {
        cout << "Arg " << i << " " << (void*)&(_argv[i]) << " " << (void*)&(_argv[i][0]) << " "
             << _argv[i] << endl;
    }
}

void DistilleryApplication::printEnviron(void) const
{
    for (ConstENVIterator i = ENV.begin(); i != ENV.end(); ++i) {
        cout << i->first << "=" << i->second << endl;
    }
}

void DistilleryApplication::saveEnvironment(char** envp)
{
    // let's check if this method has already been called
    if (_envpsize > 0) {
        return;
    }

    int i;
    char** p;

    for (i = 0; environ[i] != NULL; i++) {
        _envpsize += strlen(environ[i]) + 1;
    }

    p = new char*[i + 1];

    for (i = 0; environ[i] != NULL; i++) {
        p[i] = StrDup(environ[i]);
    }
    p[i] = NULL;

    _envpfence = (char*)(&envp[0][0]) + _envpsize;
    environ = p;
}

void DistilleryApplication::saveArgs(const int argc, char** argv)
{
    if (_savedArgv) {
        for (int i = 0; i < _argc; ++i) {
            delete[] _savedArgv[i];
        }
        delete[] _savedArgv;
    }
    _argc = argc;
    _savedArgv = new char*[argc];
    for (int i = 0; i < argc; ++i) {
        _savedArgv[i] = new char[strlen(argv[i]) + 1];
        if (i < argc - 1) {
            int len = argv[i + 1] - argv[i] - 1;
            strncpy(_savedArgv[i], argv[i], len);
            _savedArgv[i][len] = '\0';
        } else {
            strcpy(_savedArgv[i], argv[i]);
        }
    }

    // only save the args for respawn on the first call to this function
    if (_respawn_argc == 0) {
        _respawn_argc = argc;
        _respawn_argv = new char*[argc + 1];

        char actualpath[PATH_MAX + 1];
        char* ptr = realpath(argv[0], actualpath);

        if (ptr) {
            _respawn_argv[0] = StrDup(ptr);
        } else {
            _respawn_argv[0] = StrDup(argv[0]);
        }

        for (int i = 1; i < argc; ++i) {
            _respawn_argv[i] = StrDup(argv[i]);
        }

        _respawn_argv[argc] = NULL;
    }
}

int DistilleryApplication::main(int argc, char** argv, char** envp)
{
    arg_vector_t remains;
    int rc = 1;

    set_new_handler(DistAppNoMoreMemoryHandler);

    try {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& ex) {
        cerr << "Error during Xerces-c Initialization: " << ex.getMessage() << "["
             << ex.getSrcFile() << ":" << ex.getSrcLine() << "]" << endl;
        return 1;
    }

    srand(time(0) + getpid());

#if (defined(DST_DBG_INST) || defined(DUMP_LINE_NUMBER))
#ifdef __i386__
    bfd_init();
#endif
#endif

    debug::cmdline = argv[0];
    debug::fadd_dynamic_aspect("CMD.%s", argv[0]);

    string::size_type space = debug::cmdline.find_first_of(' ');
    if (space == string::npos) {
        space = debug::cmdline.size();
    }
    debug::cmd = debug::cmdline.substr(0, space);

    string::size_type slash = debug::cmd.find_last_of('/');
    if (slash == string::npos) {
        slash = 0;
    } else {
        slash++;
    }
    debug::cmd = debug::cmd.substr(slash);

    char host[1024];
    try {
        strcpy(host, HostInfo::getMyHostName().c_str());
        debug::hostname = host;
    } catch (HostInfoException& e) {
        debug::hostname = "-";
    }

    for (int i = 1; i < argc; i++) {
        debug::cmdline += ' ';
        debug::cmdline += argv[i];
    }

    // Set locale from default enviornment
    // setlocale is used here instead of std::locale() because locale has a side effect that causes
    // stringstream (at a minimum) to format integers with digit groupings enabled. eg. 1234 is
    // output as 1,234
    char* localename = setlocale(LC_ALL, "");

    // localename is null if setlocale fails for some reason
    if (NULL == localename) {
        cerr << "setlocale failed" << endl;
        rc = 130;
    } else {

        DistilleryApplication::app = instantiate_new_app();
        DistilleryApplication* appl = DistilleryApplication::app;

        appl->saveEnvAndArgs(argc, argv, envp);
        appl->setARGV0(argv[0]);
        appl->installExitSignalHandlers();

        if (appl->init()) {
            cerr << "Init failed during startup" << endl;
            rc = 124;
        } else {
            try {
                appl->processOptions(argc, argv, remains);
                // If there is no tracer setting from startup options, such as using
                // --Xdebug-file or any other debug setting options, we will use default
                // tracer -- console tracer.
                if (!appl->isTracerSwitched()) {
                    appl->switchTracer(new debug::ConsoleTracer);
                }

                SPCDBG(L_TRACE, "Switched to console tracer", CORE_DAPP);
                // If end user set the config_path for distapprc, we will log but
                // ignore it.
                // Environment variable DISTAPPRC and .distapprc file used to be
                // supported from processOptions() method. When we disable supporting
                // them, as we never document it before, and do not want the warning
                // message shown up from commandline or Admin, we have to do this here
                // so to use SPCDBG.
                string config_path;
                if (getenv("DISTAPPRC")) {
                    config_path = getenv("DISTAPPRC");
                } else {
                    config_path = (getenv("HOME") ?: ".");
                    config_path += "/.distapprc";
                }
                if (Directory::sisAFile(config_path)) {
                    SPCDBG(L_INFO,
                           "InfoSphere Streams services no longer support setting"
                             << " options using environment variable DISTAPPRC and "
                             << " the .distapprc file. The setting options from " << config_path
                             << " were ignored.",
                           CORE_DAPP);
                }

                appl->installFaultSignalHandlers();
                rc = appl->runSafeCatchall(remains);
            } catch (InvalidOptionException& ex) {
                if (ex.getExplanation().length()) {
                    appl->error(ex.getExplanation()) << endl << endl;
                }
                appl->printUsage(cerr, false);
                cerr << endl;

                rc = 1;
            }
        }
    }

    DistAppTerminate(rc);
}

void DistilleryApplication::setARGV0(const char* name)
{
    _argv_0.clear();
    _argv_0.append(name);
}

DistilleryApplication& DistilleryApplication::getThisApp(void)
{
    return *_thisApp;
}

ostream& DistilleryApplication::printDescription(ostream& o) const
{
    o << " << NO DESCRIPTION AVAILABLE >>";

    return o;
}

void DistilleryApplication::getArguments(option_vector_t& options) {}

void DistilleryApplication::getDistilleryApplicationVisibleOptions(option_vector_t& options)
{
    option_t local_options[] = {
        { 'h', "help", 0, "", "Display this help and exit",
          STR_OPT(DistilleryApplication::processHelpOption) },
        { 0, "rolling-logger-file", ARG, "", "DEPRECATED. Rolling trace file, please use --tracing",
          STR_OPT(DistilleryApplication::processTracing) },
        { 0, "tracing", ARG, "",
          "Service tracing settings with format of filename:maxFileSize:maxFileNum",
          STR_OPT(DistilleryApplication::processTracing) },
        { 0, "logging", ARG, "",
          "Service logging settings with format of logType:filename:maxFileSize:maxFileNum",
          STR_OPT(DistilleryApplication::processLogging) },
        { 0, "log-level", ARG, "", "Service log level",
          INT_OPT(DistilleryApplication::processLogLevel) },
        { 0, "stdouterr", ARG, "",
          "Service stdout/stderr log settings with format of filename:maxFileSize:maxFileNum",
          STR_OPT(DistilleryApplication::processStdouterrSetting) },
        // Connector maximum binding retry parameters
        { 0, "connector-max-retries", ARG, "", "Connector binding maximum retries",
          INT_OPT(DistilleryApplication::processConnectorMaxRetries) },
    };

    APPEND_OPTIONS(options, local_options);
}

void DistilleryApplication::getDistilleryApplicationOptions(option_vector_t& options)
{
    getDistilleryApplicationVisibleOptions(options);

    option_t local_options[] = {
        // hidden options
        { 'D', "debug", 0, "", "", STR_OPT(DistilleryApplication::processDebugLevel), true },
        { 0, "Xhelp", 0, "", "", STR_OPT(DistilleryApplication::processXHelpOption), true },
        { 0, "Xdebug-on-screen", 0, "", "",
          STR_OPT(DistilleryApplication::processDeprecatedOptions), true },
        { 0, "Xdebug-file", ARG, "DEBUG_FILE", "", STR_OPT(DistilleryApplication::processDebugFile),
          true },
#ifdef DST_DBG_INST
        { 0, "Xdebug-enable-file", ARG, "", "",
          STR_OPT(DistilleryApplication::processDebugEnableFile), true },
        { 0, "Xdebug-toggle-file", ARG, "DEBUG_TOGGLE_FILE", "",
          STR_OPT(DistilleryApplication::processDebugToggleFile), true },
#endif
        { 0, "Xdebug-color", ARG, "DEBUG_COLOR", "",
          STR_OPT(DistilleryApplication::processDebugColor), true },
        { 0, "Xdebug-th-color", ARG, "DEBUG_TH_COLOR", "",
          STR_OPT(DistilleryApplication::processDebugThColor), true },
        { 0, "Xdebug-filter-th-color", ARG, "DEBUG_FILTER_TH_COLOR", "",
          STR_OPT(DistilleryApplication::processDebugFilterThColor), true },
        { 0, "Xdebug-ring", ARG, "", "", INT_OPT(DistilleryApplication::processDebugRing), true },
        { 0, "Xno-signal-handler", 0, "SIGNAL_HANDLER", "",
          STR_OPT(DistilleryApplication::processNoSignalHandler), true },
        { 0, "Xdebug-console-fmt", ARG, "", "",
          STR_OPT(DistilleryApplication::processDebugConsoleFmt), true },
        { 0, "Xno-tty", 0, "", "", STR_OPT(DistilleryApplication::processNoTTY), true },
        { 0, "Xnop", 0, "", "", STR_OPT(DistilleryApplication::processNop), true },
        { 0, "Xsnop", ARG, "", "", STR_OPT(DistilleryApplication::processSNop), true },
        { 0, "Xinop", ARG, "", "", INT_OPT(DistilleryApplication::processINop), true },
        { 0, "affinity", ARG, "AFFINITY", "", INT_OPT(DistilleryApplication::processAffinity),
          true },
        { 0, "unique-id", ARG, "", "unique identifier for this service",
          STR_OPT(DistilleryApplication::processUniqueId), true },
        { 0, "testMode", 0, "", "Start up daemon in test mode",
          STR_OPT(DistilleryApplication::processTestMode), true },
    };

    APPEND_OPTIONS(options, local_options);
}

void DistilleryApplication::getAllOptions(option_vector_t& options)
{
    getDistilleryApplicationOptions(options);
    getArguments(options);
}

void DistilleryApplication::setArgument(const option_t* option, const char* value)
{
    THROW(InvalidOption, "Invalid Argument");
}

ostream& DistilleryApplication::printUsage(ostream& o, bool extended)
{
    printDescription(o) << " (" << Product::getName() << " " << Product::getVersion() << ")" << endl
                        << endl;

    option_vector_t options;
    getAllOptions(options);

    int max_size = -1;

    string options_desc[options.size()];

    for (unsigned int i = 0; i < options.size(); i++) {
        if (options[i].hidden && !extended) {
            continue;
        }

        options_desc[i] += "  ";

        if (options[i].short_arg) {
            options_desc[i] += "-";
            options_desc[i] += (char)options[i].short_arg;
        }

        if (!options[i].long_arg.empty()) {
            if (options[i].short_arg) {
                options_desc[i] += ", ";
            }

            options_desc[i] += "--";
            options_desc[i] += options[i].long_arg;

            if (options[i].flags & ARG) {
                if (options[i].int_fct.isSet()) {
                    options_desc[i] += "=INT";
                } else {
                    options_desc[i] += "=VALUE";
                }
            }
        }

        int len = (int)options_desc[i].length();
        if (len > max_size) {
            max_size = len;
        }
    }

    if (max_size > 30) {
        max_size = 30;
    }

    for (unsigned int i = 0; i < options.size(); i++) {
        if (options[i].hidden && !extended) {
            continue;
        }

        if ((int)options_desc[i].length() > max_size) {
            string pad;
            pad.append(max_size, ' ');
            o << options_desc[i] << endl << pad;
        } else {
            options_desc[i].append(max_size - options_desc[i].length(), ' ');
            o << options_desc[i];
        }

        o << "  " << options[i].description;

        if (extended) {
            if (!options[i].envar.empty()) {
                o << "  [" << options[i].envar << "]";
            }
        }

        o << endl;
    }

    return o;
}

void DistilleryApplication::processTestMode(const option_t* option, const char* value)
{
    _testMode = true;
}

void DistilleryApplication::processHelpOption(const option_t* option, const char* value)
{
    printUsage(cerr, false);
    cerr << endl;
    UTILS_NAMESPACE::DistAppTerminate(1);
}

void DistilleryApplication::processXHelpOption(const option_t* option, const char* value)
{
    printUsage(cerr, true);
    cerr << endl;
    UTILS_NAMESPACE::DistAppTerminate(1);
}

void DistilleryApplication::processDebugLevel(const option_t* option, const char* value)
{
#ifdef DST_DBG_INST
    static int level = -1;

    level++;

    SymbolReader s;
    s.setByLevel(level);
#else
    if (_spc_tracer) {
        _spc_tracer->increaseLevel();
        debug::set_streams_trclevel(_spc_tracer->getLevel());
    }

    increaseDebugLevel();
#endif
}

void DistilleryApplication::resetDebugLevel(int level)
{
    if (_init_trc_level != level) {
        if (_spc_tracer) {
            _spc_tracer->setLevel(level);
            debug::set_streams_trclevel(level);
        }
        _init_trc_level = level;
    }
}

void DistilleryApplication::processDeprecatedOptions(const option_t* option, const char* value) {}

void DistilleryApplication::increaseDebugLevel(void)
{
    ++_init_trc_level;
}

void DistilleryApplication::processDebugFile(const option_t* option, const char* value)
{
    switchTracer(new debug::FileTracer(value));
}

void DistilleryApplication::processTracing(const option_t* option, const char* value)
{
    if (value != NULL) {
        _trc_settings.assign(value);
        if (!_testMode) {
            switchTracer(new debug::RollingFileTracer(_trc_settings, _init_trc_level, true));
        } else {
            switchTracer(new debug::RollingFileTracer(_trc_settings, _init_trc_level));
        }
    }
}

void DistilleryApplication::processRollingLoggerFile(const option_t* option, const char* value)
{
    processTracing(option, value);
}

void DistilleryApplication::processLogging(const option_t* option, const char* value)
{
    if (value != NULL) {
        _log_settings.assign(value);
        size_t pos = _log_settings.find(":");
        // Expected format as logType:logFile[:maxSize:maxNum].
        // If format not correct, just not generate logger.
        if (pos != string::npos && pos != 0 && pos != (_log_settings.size() - 1)) {
            string logType = _log_settings.substr(0, pos);
            string logFile = _log_settings.substr(pos + 1);
            _log_logger = debug::LogFactory::getLogger(logType, _init_log_level, logFile);
            if (NULL != _log_logger) {
                debug::set_log_logger(_log_logger);
            } else {
                stringstream ss;
                ss << "Invalid setting for '" << option->long_arg
                   << "' command line option: " << value
                   << ". Expected format is logType:logFile[:maxFileSize:maxFileNum],"
                   << " and logType can only be 'file' or 'syslog'.";
                THROW(InvalidOption, ss.str());
            }
        } else {
            stringstream ss;
            ss << "Invalid setting for '" << option->long_arg << "' command line option: " << value
               << ". Expected format is logType:logFile[:maxFileSize:maxFileNum].";
            THROW(InvalidOption, ss.str());
        }
    }
}

void DistilleryApplication::processLogLevel(const option_t* option, int value)
{
    if (_log_logger) {
        debug::set_log_level(value);
    }
    _init_log_level = value;
}

void DistilleryApplication::processStdouterrSetting(const option_t* option, const char* value)
{
    if (value != NULL) {
        // Set stdout/stderr trace setting
        _stdouterr_settings.assign(value);

        // process pipe that listening to stdout/stderr
        int rc = pipe(_stdouterrPipe);
        if (0 != rc) {
            THROW(FailedSystemCall, "Pipe creating failed with " << strerror(errno));
        }
        setFDFlags(_stdouterrPipe[0], O_NONBLOCK); // set the read no block.

        // Get new log facility ready for writing stdout/stderr messages.
        _stdouterr_tracer = new debug::RollingFileTracer(_stdouterr_settings, _init_trc_level,
                                                         debug::RollingFileTracer::STDOUT_STDERR);

        // Creating a separate thread to process stdout/stderr
        _stdouterrThread = new StdouterrLoggingThread;
        rc = _stdouterrThread->create(NULL, true, this, false);
        if (rc != 0) {
            THROW(Distillery,
                  "Stdout/stderr management thread creating failed with " << strerror(errno));
        }

        // Redirect stdout and stderr to the input of pipe.
        _originalStdoutFD = dup2(_stdouterrPipe[1], stdout->_fileno);
        if (-1 == _originalStdoutFD) {
            THROW(FailedSystemCall, "Can not redirect stdout to pipe with " << strerror(errno));
        }
        _originalStderrFD = dup2(_stdouterrPipe[1], stderr->_fileno);
        if (-1 == _originalStderrFD) {
            THROW(FailedSystemCall, "Can not redirect stderr to pipe with " << strerror(errno));
        }
    }
}

void DistilleryApplication::stdouterrTracing()
{
    // Note: do NOT log or trace or even cout/cerr inside this method without
    //       updating processStdouterrSetting() above.

    // The way pipe works as:
    // stdout/stderr -> _stdouterrPipe[1] -----> _stdouterrPipe[0] -> rolling_logger_file

    // Creating a pollfd to check and grab messages from _stdouterrPipe[0].
    struct pollfd fds[1];
    fds[0].fd = _stdouterrPipe[0];
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    for (;;) {
        int rc = poll(fds, 1, -1); // block until pipe is ready
        if (rc > 0) {
            if (fds[0].revents) {
                if (fds[0].revents & POLLIN) {
                    char data[1024 + 1];
                    ssize_t res = 0;
                    do {
                        res = read(fds[0].fd, data, 1024);
                        if ((res > 0) && _stdouterr_tracer) {
                            _stdouterr_tracer->writeMessage(string(data, res));
                        }
                    } while ((res > 0) && _stdouterr_tracer);
                    fds[0].revents &= ~POLLIN;
                }
                if (fds[0].revents & POLLHUP) {
                    fds[0].fd = -1;
                    fds[0].events = 0;
                    fds[0].revents &= ~POLLHUP;
                    break;
                }
                if (fds[0].revents) {
                    THROW(FailedSystemCall,
                          "poll() while waiting for pipe return an unexpected status: "
                            << PollEventFormat(fds[0].revents));
                }
            }
        } else if (rc < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            } else {
                THROW(FailedSystemCall, "poll() while waiting for pipe return an error of '"
                                          << ErrnoFormat(errno) << "'.");
            }
        }
        // No need to check 0==rc case, as poll() will block until pipe is ready.
    }
}

#ifdef DST_DBG_INST
void DistilleryApplication::processDebugEnableFile(const option_t* option, const char* value)
{
    SymbolReader s;
    s.setByFile(value);
}

void DistilleryApplication::processDebugToggleFile(const option_t* option, const char* value)
{
    SymbolReader s;
    s.processToggleFile(_argv_0, value);
}
#endif

void DistilleryApplication::processDebugConsoleFmt(const option_t* option, const char* value)
{
    debug::streams_default_trace_format = value;
}

void DistilleryApplication::processNop(const option_t* option, const char* value) {}

void DistilleryApplication::processSNop(const option_t* option, const char* value) {}

void DistilleryApplication::processINop(const option_t* option, int value) {}

void DistilleryApplication::processAffinity(const option_t* option, int value)
{
    if (value > 0) {
        cpu_set_t aff;

        CPU_ZERO(&aff);
        int cpu = 0;

        while (value) {
            if (value & 1) {
                CPU_SET(cpu, &aff);
            }

            value >>= 1;
            cpu++;
        }

        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &aff)) {
            cerr << "Call to sched_setaffinity failed: " << strerror(errno) << endl;
        }
    }
}

void DistilleryApplication::processDebugColor(const option_t* option, const char* value)
{
    switchTracer(new debug::ColorConsoleTracer(value));
}

void DistilleryApplication::processDebugThColor(const option_t* option, const char* value)
{
    switchTracer(new debug::ThColorConsoleTracer(value));
}

void DistilleryApplication::processDebugFilterThColor(const option_t* option, const char* value)
{
    vector<string> params;
    tokenize(string(value), params, ":", false);
    if (params.size() > 1) {
        vector<string> aspects;
        tokenize(params[1], aspects, ",", false);
        switchTracer(new debug::FilteredThColorConsoleTracer(params[0], aspects));
    } else {
        stringstream ss;
        ss << "Invalid setting for '" << option->long_arg << "' command line option: " << value
           << ". Expected format is color:aspects.";
        THROW(InvalidOption, ss.str());
    }
}

void DistilleryApplication::processDebugRing(const option_t* option, int value)
{
    debug::Tracer* output_tracer = _spc_tracer;
    if (!_tracerSwitched) {
        output_tracer = new debug::ConsoleTracer;
    }
    debug::RingTracer* ring_tracer = new debug::RingTracer(output_tracer, value);

    debug::Tracer* old_tracer = _spc_tracer;
    _spc_tracer = ring_tracer;
    debug::set_streams_tracer(ring_tracer);
    if (old_tracer) {
        debug::set_streams_trclevel(old_tracer->getLevel());
        // Grab any logging messages that saved with MemoryLogger.
        ring_tracer->writeMessage(old_tracer->getMessages());
        if (!_tracerSwitched) {
            delete old_tracer;
        }
    }
    _tracerSwitched = true;
}

void DistilleryApplication::processNoSignalHandler(const option_t* option, const char* value)
{
    _install_signal_handler = false;
}

void DistilleryApplication::processNoTTY(const option_t* option, const char* value)
{
    int fd = open("/dev/tty", O_RDWR);
    if (fd >= 0) {
        ioctl(fd, TIOCNOTTY, 0);
        close(fd);
    }
}

void DistilleryApplication::processUniqueId(const option_t* option, const char* value) {}

void DistilleryApplication::processConnectorMaxRetries(const option_t* option, int value)
{
    if (value >= 0) {
        _connector_max_retries = value;
    }
}

ostream& UTILS_NAMESPACE_QUALIFIER operator<<(ostream& s,
                                              const DistilleryApplication::option_t& opt)
{
    if (!opt.long_arg.empty()) {
        s << "--" << opt.long_arg;

        if (isprint(opt.short_arg)) {
            s << " (-" << opt.short_arg << ")";
        }
    } else {
        if (isprint(opt.short_arg)) {
            s << "-" << opt.short_arg;
        }
    }

    return s;
}

typedef struct option getopt_option_t;

void DistilleryApplication::setMyProcessTableName(const char* name)
{
    replaceArgv(0, name, true);
}

void DistilleryApplication::setMyProcessTableState(const char* state)
{
    if (!_argvState) {
        if (_argv[_argc - 1] + strlen(_savedArgv[_argc - 1]) + strlen(state) >= _envpfence) {
            SPCDBG(L_ERROR, "setMyProcessTableState: No space available to add state information",
                   CORE_DAPP);
            cerr << "setMyProcessTableState: No space available to add state information" << endl;
            return;
        }
        char** nsavedArgv = new char*[_argc + 1];
        int i;
        for (i = 0; i < _argc; ++i) {
            nsavedArgv[i] = StrDup(_savedArgv[i]);
        }
        nsavedArgv[i] = new char[1];
        nsavedArgv[i][0] = '\0';
        _argv[i] = _argv[i - 1] + strlen(_savedArgv[i - 1]) + 1;
        _argv[i][0] = '\0';
        for (i = 0; i < _argc; ++i) {
            delete[] _savedArgv[i];
        }
        delete[] _savedArgv;
        _savedArgv = nsavedArgv;
        _argc++;
        _argvState = true;
    }
    char* astate = new char[strlen(state) + 3];
    sprintf(astate, "[%s]", state);
    replaceArgv(_argc - 1, astate, true);
    delete[] astate;
}

void DistilleryApplication::switchTracer(debug::Tracer* new_tracer)
{
    debug::Tracer* old_tracer = _spc_tracer;

    debug::set_streams_tracer(new_tracer);
    _spc_tracer = new_tracer;

    if (old_tracer) {
        debug::set_streams_trclevel(old_tracer->getLevel());
        // Grab any logging messages that saved with MemoryLogger.
        new_tracer->writeMessage(old_tracer->getMessages());
        old_tracer->removeTraceFile();
        delete old_tracer;
    }

    _tracerSwitched = true;
}

void DistilleryApplication::saveEnvAndArgs(const int argc, char** argv, char** envp)
{
    _argv = argv;
    saveEnvironment(envp);
    saveArgs(argc, argv);
}

void DistilleryApplication::processOptions(int argc, char** argv, arg_vector_t& remains)
{
    option_vector_t options;
    getAllOptions(options);

#ifdef DST_DBG_INST
    // increase the debug level to at least 0
    processDebugLevel(NULL, NULL);
#endif

    processCommandLine(argc, argv, options, remains);
    processEnvironment(options);

    bool missing_options = false;
    stringstream missing_strm;

    for (unsigned int i = 0; i < options.size(); i++) {
        if ((options[i].flags & REQUIRED) && !options[i].already_set) {
            if (missing_options) {
                missing_strm << endl;
            }
            missing_strm << "Option " << options[i] << " is required.";
            missing_options = true;
        }
    }

    if (missing_options) {
        THROW(InvalidOption, missing_strm.str());
    }
}

void DistilleryApplication::processCommandLine(int argc,
                                               char** argv,
                                               option_vector_t& options,
                                               arg_vector_t& remains)
{
    string short_args = ":";

    int local_optind;
    int local_optopt;
    int found_option = -1;
    const int option_offset = 1024;

    auto_array<getopt_option_t> long_options(new getopt_option_t[options.size() + 1]);

    for (size_t i = 0; i < options.size(); i++) {
        if (options[i].short_arg) {
            short_args += options[i].short_arg;
            if (options[i].flags & ARG) {
                short_args += ":";
            }
        }

        long_options[i].name = options[i].long_arg.c_str();
        long_options[i].has_arg = (options[i].flags & ARG) ? 1 : 0;
        long_options[i].flag = &found_option;
        long_options[i].val = i + option_offset;
    }

    memset(&(long_options[options.size()]), 0, sizeof(getopt_option_t));

    opterr = 0; // quiet
    optind = 0;

    string cmdline(argv[0]);
    for (int i = 1; i < argc; ++i) {
        cmdline += " " + string(argv[i]);
    }

    while (1) {

        int option_index = 0;
        int c;
        found_option = -1;

        c = getopt_long(argc, argv, short_args.c_str(), long_options.get(), &option_index);

        local_optopt = optopt;
        local_optind = optind;

        // cerr << "optopt=" << local_optopt << " optind=" << local_optind << " c=" << c << endl ;

        if (c == -1) {
            break;
        }

        if (c == '?' || c == ':') {
            if (local_optopt >= option_offset) {
                // getopt put in optopt the flag value supplied earlier

                if (local_optopt < option_offset + (int)options.size()) {
                    found_option = local_optopt - option_offset;
                }
            } else {
                if (local_optopt) {
                    for (unsigned int i = 0; i < options.size(); i++) {
                        if (options[i].short_arg && options[i].short_arg == local_optopt) {
                            found_option = i;
                            break;
                        }
                    }
                }
            }

            stringstream error_strm;

            if (c == '?') {
                error_strm << "Invalid or ambiguous option in '" << cmdline << "': ";
            } else {
                error_strm << "Missing argument for option in '" << cmdline << "': ";
            }

            if (found_option != -1) {
                error_strm << options[found_option];
            } else {
                if (!local_optopt && local_optind <= argc) {
                    error_strm << argv[local_optind - 1];
                } else {
                    if (local_optopt < 255) {
                        error_strm << "-" << (char)local_optopt;
                    }
                }
            }

            THROW(InvalidOption, error_strm.str());
        }

        if (c) {
            for (unsigned int i = 0; i < options.size(); i++) {
                if (options[i].short_arg == c) {
                    found_option = i;
                    break;
                }
            }
        }

        if (found_option == -1) {
            THROW(InvalidOption, "Invalid option");
        } else {
            if (found_option >= option_offset) {
                found_option -= option_offset;
            }

            try {
                callOptionFct(&(options[found_option]), optarg);
            } catch (const char* ex) {
                stringstream error_strm;
                error_strm << "Invalid value for option " << options[found_option] << ": " << ex
                           << " in '" << cmdline << "'" << endl;
                THROW(InvalidOption, error_strm.str());
            }
        }
    }

    if (local_optind < argc) {
        if (_keep_remaining_args) {
            while (local_optind < argc) {
                remains.push_back(argv[local_optind++]);
            }
        } else {
            stringstream error_strm;

            error_strm << "Invalid argument: ";
            while (local_optind < argc) {
                error_strm << argv[local_optind++] << " ";
            }
            error_strm << "in '" << cmdline << "'";

            THROW(InvalidOption, error_strm.str());
        }
    }
}

void DistilleryApplication::processEnvironment(option_vector_t& options)
{
    for (unsigned int i = 0; i < options.size(); i++) {
        if (!options[i].already_set) {
            if (!options[i].envar.empty()) {
                string varname = options[i].envar;
                char* var = getenv(varname.c_str());
                if (var) {
                    callOptionFct(&(options[i]), var);
                } else {
                    varname = "STREAMS_";
                    varname += options[i].envar;
                    var = getenv(varname.c_str());
                    if (var) {
                        callOptionFct(&(options[i]), var);
                    }
                }
            }
        }
    }
    // storing environment settings
    vector<string> tokens;
    for (int i = 0; environ[i] != NULL; ++i) {
        tokenize(environ[i], tokens, "=", false);
        if (tokens.size() == 2) {
            ENV.insert(make_pair(tokens[0], tokens[1]));
        } else {
            ENV.insert(make_pair(tokens[0], ""));
        }
    }
}

void DistilleryApplication::callOptionFct(option_t* option, const char* value)
{
    if (option->int_fct.isSet()) {
        char* endptr;
        long int_value = 0;

        if (option->flags & ARG) {
            int_value = strtol(value, &endptr, 0);

            if (((int_value == LONG_MIN || int_value == LONG_MAX) && errno == ERANGE) ||
                (*value && *endptr)) {
                stringstream error_strm;
                error_strm << "Invalid value for option " << *option << ": " << value
                           << " is not a valid integer." << endl;
                THROW(InvalidOption, error_strm.str());
            }
        }

        option->int_fct(option, int_value);
    } else {
        if (option->str_fct.isSet()) {
            option->str_fct(option, (option->flags & ARG) ? value : NULL);
        } else {
            setArgument(option, (option->flags & ARG) ? value : NULL);
        }
    }

    option->already_set = true;
}

void DistilleryApplication::loadArgs(int argc, char** argv, arg_vector_t& remains)
{
    for (int i = 0; i < argc; i++) {
        remains.push_back(string(argv[i]));
    }
}

void DistilleryApplication::setKeepRemainingArguments()
{
    _keep_remaining_args = true;
}

int DistilleryApplication::runSafe(const arg_vector_t& remains)
{
    return run(remains);
}

int DistilleryApplication::runSafeCatchDst(const arg_vector_t& remains)
{
    try {

        debug::fadd_dynamic_aspect("PID.%d", getpid());

        // fetch the localename being used and log it
        string localename = setlocale(LC_ALL, NULL);
        string encoding = "Unknown";
        string utf8 = "UTF-8";

        // check to ensure that the current encoding of the locale is UTF-8
        encoding = nl_langinfo(CODESET);
        if (boost::iequals(encoding, utf8, locale("en_US")) != true) {
            error("Current locale is not UTF-8 encoding. Locale name is ")
              << localename << ". exiting..." << endl;
            return 131;
        }

        return runSafe(remains);
    } catch (DistilleryException& ex) {
        error("Exception (") << ex.getType() << "): " << ex.getExplanation() << endl;
        return 126;
    } catch (const string& str) {
        error("Exception(string&): ") << str << ". exiting..." << endl;
        return 127;
    } catch (const char* str) {
        error("Exception(char*): ") << str << ". exiting..." << endl;
        return 128;
    }
}

int DistilleryApplication::runSafeCatchall(const arg_vector_t& remains)
{
#define CATCH_IT(extype)                                                                           \
    catch (extype & ex)                                                                            \
    {                                                                                              \
        error("exception! (type=" #extype ") what='") << ex.what() << "'. exiting..." << endl;     \
        return 129;                                                                                \
    }

    try {
        return runSafeCatchDst(remains);
    }
    CATCH_IT(domain_error)
    CATCH_IT(invalid_argument)
    CATCH_IT(length_error)
    CATCH_IT(out_of_range)
    CATCH_IT(logic_error)

    CATCH_IT(overflow_error)
    CATCH_IT(range_error)
    CATCH_IT(underflow_error)
    CATCH_IT(runtime_error)

    CATCH_IT(bad_alloc)
    CATCH_IT(bad_cast)
    CATCH_IT(bad_exception)
    CATCH_IT(bad_typeid)
    // CATCH_IT(failure)

    CATCH_IT(exception)

    catch (...)
    {
        error("Exception(unknown)! exiting...") << endl;
        return 129;
    }
    error("Uncaught unknown exception! exiting...") << endl;

#undef CATCH_IT
}

void DistilleryApplication::installSignalHandler(int sig, signal_handler_t handler)
{
    map<int, signal_handler_t>::iterator handler_it = _signal_handlers.find(sig);

    if (handler_it != _signal_handlers.end()) {
        stringstream error_strm;
        error_strm << "Handler already installed for " << SIGNAL_NAMES[sig]
                   << ". Replacing it is not allowed.";
        THROW(InvalidOption, error_strm.str());
    }
    _signal_handlers[sig] = handler;

    // When handling signals, block all signals except for QUIT (ctrl-\)
    struct sigaction setup_action;
    sigset_t block_mask;
    sigfillset(&block_mask);
    sigdelset(&block_mask, SIGQUIT);
    setup_action.sa_handler = getErrorSignalHandler();
    setup_action.sa_mask = block_mask;
    setup_action.sa_flags = 0;
    sigaction(sig, &setup_action, NULL);
}

void DistilleryApplication::removeSignalHandler(int sig)
{
    signal(sig, SIG_DFL);
    _signal_handlers.erase(sig);
    SPCDBG(L_INFO, "Erased handler " << SIGNAL_NAMES[sig], CORE_DAPP);
}

void DistilleryApplication::installFaultSignalHandlers()
{
    if (_install_signal_handler) {
        installSignalHandler(SIGABRT, mem_cb(this, &DistilleryApplication::sigAbortHandler));
        installSignalHandler(SIGSEGV, mem_cb(this, &DistilleryApplication::sigSegVHandler));
        installSignalHandler(SIGPIPE, mem_cb(this, &DistilleryApplication::sigPipeHandler));
        installSignalHandler(SIGILL, mem_cb(this, &DistilleryApplication::sigIllHandler));
        installSignalHandler(SIGFPE, mem_cb(this, &DistilleryApplication::sigFPEHandler));
        installSignalHandler(SIGBUS, mem_cb(this, &DistilleryApplication::sigBusHandler));
    } else {
        signal(SIGABRT, SIG_DFL);
        signal(SIGSEGV, SIG_DFL);
        signal(SIGPIPE, SIG_DFL);
        signal(SIGILL, SIG_DFL);
        signal(SIGFPE, SIG_DFL);
        signal(SIGBUS, SIG_DFL);
    }
}

void DistilleryApplication::installExitSignalHandlers()
{
    if (_install_signal_handler) {
        installSignalHandler(SIGINT, mem_cb(this, &DistilleryApplication::sigIntHandler));
        installSignalHandler(SIGTERM, mem_cb(this, &DistilleryApplication::sigTermHandler));
        installSignalHandler(SIGQUIT, mem_cb(this, &DistilleryApplication::sigQuitHandler));
    } else {
        signal(SIGINT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
    }
}

sighandler_t DistilleryApplication::getErrorSignalHandler()
{
    return &(DistilleryApplication::errorSignalHandler);
}

void DistilleryApplication::errorSignalHandler(int sig)
{
    map<int, signal_handler_t>::iterator handler_it = app->_signal_handlers.find(sig);

    if (handler_it != app->_signal_handlers.end()) {
        // Do not make a local copy here for signal handler, as it uses
        // Callback_1 copy constructor which calls MemberFunction_1::dup and cause
        // malloc and deadlock.

        if (!(handler_it->second).isSet()) {
            async_safe_printf(2, "Signal %d trapped but no handler defined! exiting...\n", sig);
            string curTime = getTRCFormattedDate();
            async_safe_printf(
              2, "%s =====================  SIGNAL %-11s ============================\n",
              curTime.c_str(), SIGNAL_NAMES[sig]);
            BacktraceDumper::dump(2, true, 0);
            async_safe_printf(
              2,
              "\n=============================================================================\n");
            ::exit(1);
        } else {
            (handler_it->second)(sig);
        }
    } else {
        async_safe_printf(2, "Signal %d trapped but no handler defined! exiting...\n", sig);
        string curTime = getTRCFormattedDate();
        async_safe_printf(2,
                          "%s =====================  SIGNAL %-11s ============================\n",
                          curTime.c_str(), SIGNAL_NAMES[sig]);
        BacktraceDumper::dump(2, true, 0);
        async_safe_printf(
          2, "\n=============================================================================\n");
        ::exit(1);
    }
}

void DistilleryApplication::faultHandler(int sig)
{
    string curTime = getTRCFormattedDate();
    async_safe_printf(2, "%s =====================  SIGNAL %-11s ============================\n",
                      curTime.c_str(), SIGNAL_NAMES[sig]);

    async_safe_printf(2,
                      "==========================  /proc/self/maps ===========================\n");
    // open /procs/self/maps and dump it to stderr
    int mapsfd = open("/proc/self/maps", O_RDONLY, 0);
    if (-1 == mapsfd) {
        async_safe_printf(2, "error opening maps file. errno: %d\n", errno);
    } else {
        char mapsbuf[4096];
        int n;
        while ((n = read(mapsfd, mapsbuf, 4096)) > 0) {
            write(2, mapsbuf, n);
        }
    }
    //
    async_safe_printf(2,
                      "\n=====================================================================\n");

    BacktraceDumper::dump(2, true, 0);
    async_safe_printf(
      2, "\n=============================================================================\n");

    // Add sleep to make sure all the dump are written to stdout/stderr file.
    sleep(2);

    _exit(1);
}

void DistilleryApplication::sigAbortHandler(int sig)
{
    faultHandler(sig);
}

void DistilleryApplication::sigSegVHandler(int sig)
{
    faultHandler(sig);
}

void DistilleryApplication::sigPipeHandler(int sig)
{
    faultHandler(sig);
}

void DistilleryApplication::sigIllHandler(int sig)
{
    faultHandler(sig);
}

void DistilleryApplication::sigFPEHandler(int sig)
{
    faultHandler(sig);
}

void DistilleryApplication::sigBusHandler(int sig)
{
    faultHandler(sig);
}

void DistilleryApplication::termHandler(int sig)
{
    if (sig != SIGINT) {
        string curTime = getTRCFormattedDate();
        async_safe_printf(2,
                          "%s =====================  SIGNAL %-11s ============================\n",
                          curTime.c_str(), SIGNAL_NAMES[sig]);
        BacktraceDumper::dump(2, true, 0);
        async_safe_printf(
          2, "\n=============================================================================\n");
    }
    ::exit(1);
}

void DistilleryApplication::sigIntHandler(int sig)
{
    termHandler(sig);
}

void DistilleryApplication::sigTermHandler(int sig)
{
    termHandler(sig);
}

void DistilleryApplication::sigQuitHandler(int sig)
{
    termHandler(sig);
}

void DistilleryApplication::setCallDestructor(bool value)
{
    _call_destructor = value;
}

const map<string, string>& DistilleryApplication::getEnv(void) const
{
    return ENV;
}

DistilleryKnobs& DistilleryApplication::getDistilleryKnobs(void)
{
    return _dk;
}

bool DistilleryApplication::isTracerSwitched(void)
{
    return _tracerSwitched;
}

int DistilleryApplication::registerQuiesce(Callback_0 function)
{
    // Add handlers in order so that we can excute them LIFO
    AutoMutex lck(_handler_key_m);
    _quiesce_map[++_handler_key] = function;
    return _handler_key;
}

void DistilleryApplication::unregisterQuiesce(int key)
{
    _quiesce_map.erase(key);
}

int DistilleryApplication::registerShutdown(Callback_0 function)
{
    SPCDBG(L_INFO, "registerShutdown", CORE_DAPP);
    // Add handlers in order so that we can excute them LIFO
    AutoMutex lck(_handler_key_m);
    _shutdown_map[++_handler_key] = function;
    return _handler_key;
}

void DistilleryApplication::unregisterShutdown(int key)
{
    _shutdown_map.erase(key);
}

void DistilleryApplication::runQuiesce()
{
    SPCDBG(L_INFO, "", CORE_DAPP);
    std::map<int, Callback_0>::reverse_iterator it = _quiesce_map.rbegin();
    while (it != _quiesce_map.rend()) {
        int key = (*it).first;
        ((*it).second)();
        _quiesce_map.erase(key);
        it = _quiesce_map.rbegin();
    }
    SPCDBG(L_INFO, "Done", CORE_DAPP);
}

void DistilleryApplication::runShutdown()
{
    SPCDBG(L_INFO, "shutting down...", CORE_DAPP);
    std::map<int, Callback_0>::reverse_iterator it = _shutdown_map.rbegin();
    while (it != _shutdown_map.rend()) {
        int key = (*it).first;
        ((*it).second)();
        _shutdown_map.erase(key);
        it = _shutdown_map.rbegin();
    }
    SPCDBG(L_INFO, "Done", CORE_DAPP);
}

int DistilleryApplication::getConnectorMaxRetries(void)
{
    return _connector_max_retries;
}

void DistilleryApplication::runMonitorThread() {}

UTILS_NAMESPACE_BEGIN

void DistAppNoMoreMemoryHandler(void)
{
    async_safe_printf(2, "the application has run out of memory and will abort...\n");
    abort();
}

void DistAppTerminate(int rc)
{
    DistilleryApplication* app = DistilleryApplication::app;

    if (app) {
        if (DistilleryApplication::app->_call_destructor) {
            delete DistilleryApplication::app;
        }
    }

    try {
        XMLPlatformUtils::Terminate();
    } catch (...) {
        // we don't care
    }

    exit(rc);
}

UTILS_NAMESPACE_END
