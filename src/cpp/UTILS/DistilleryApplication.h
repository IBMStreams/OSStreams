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

//----------------------------------------------------------------------------
//
// Title-
//       DistilleryApplication.h
//
// Purpose-
//       Distillery Application Class
//       This class with provides:
//         - argument processing
//         - Signal catching for segfault, sigpipe
//         - Catching of wild exceptions (the one that made their way to
//           the top without being caught)
//         - print the usage and argument description
//
//----------------------------------------------------------------------------

#ifndef _DISTILLERYAPPLICATION_H_
#define _DISTILLERYAPPLICATION_H_

#include <map>
#include <memory>
#include <ostream>
#include <signal.h>
#include <string>
#include <vector>

#include <TRC/DistilleryDebugLogger.h>
#include <UTILS/DistilleryApplicationThreads.h>
#include <UTILS/DistilleryCallback.h>
#include <UTILS/DistilleryKnobs.h>
#include <UTILS/UTILSTypes.h>
#include <UTILS/utils_vector.h>

// extern int main(int argc, char** argv, char **envp) ;

UTILS_NAMESPACE_BEGIN

typedef std::vector<std::string> arg_vector_t;

/**
   \internal

   This class implements the support for describing a Distillery
   application, i.e., a common set of interfaces for handling
   command-line arguments, just-in-time debugging, signal handling,
   process table string manipulation, etc
*/

#define INT_OPT(fct) mem_cb(this, &fct), str_ign
#define STR_OPT(fct) int_ign, mem_cb(this, &fct)

/// Finish application up graciously
void DistAppTerminate(int) __attribute__((noreturn));
/// Finish application up if a dynamic memory allocation fails for
/// lack of available memory
void DistAppNoMoreMemoryHandler(void);

class DistilleryApplication
{
  public:
    struct option_T;

    typedef Callback_1<int> signal_handler_t;
    typedef Callback_2<const struct option_T*, int> int_option_t;
    typedef Callback_2<const struct option_T*, const char*> str_option_t;

    static int_option_t int_ign;
    static str_option_t str_ign;

    static const char* SIGNAL_NAMES[];
    static DistilleryApplication* _thisApp;

#define ARG 1
#define REQUIRED 2

    typedef struct option_T
    {
        /// short version (single character) of the command line switch
        char short_arg;
        /// long version (multi-character) of the command line switch
        std::string long_arg;
        /// does this option take an argument?
        int flags;
        /// The environment variable associated with this option
        std::string envar;
        /// this option's description
        std::string description;
        /// this option's callback (if it is a numeric value)
        int_option_t int_fct;
        /// this option's callback (if it is a character-based value)
        str_option_t str_fct;
        bool hidden;
        bool already_set;
    } option_t;

    typedef utils_vector<option_t> option_vector_t;
    typedef std::map<std::string, std::string>::const_iterator ConstENVIterator;

    /// Destructor
    virtual ~DistilleryApplication();

    // The main function that calls the instantiator
    // same arguments as system main (see MAIN_APP #define)
    static int main(int argc, char** argv, char** envp);

    /// Change argv[0]
    /// @param name what it should be changed to
    void setARGV0(const char* name);

    /// Obtain argv[0]
    /// @return argv[0]
    const std::string& getARGV0(void) { return _argv_0; }

    /// Obtain a reference to the application handle
    /// @return the application handle
    static DistilleryApplication& getThisApp(void);

    /// Initialize the application -- this method is called before command-line
    /// arguments are processed, but after signal handlers are installed
    virtual int init(void);

    /// The application run-time code (i.e., its entry point in lieu of the
    /// main function, which is not longer available to the application writer).
    /// @param args the command line arguments
    /// @return the error code that will be passed to the shell
    virtual int run(const arg_vector_t& args) = 0;

    /// Call the application run-time code and catch all Distillery
    /// exceptions.  Error messages are printed to ensure that
    /// exceptional terminations are more or less graceful
    /// @param remaining_args the command line arguments that are not preceeded
    /// by a dash (-) or two dashes (--)
    /// @return the error code that will be passed to the shell
    virtual int runSafe(const arg_vector_t& remaining_args);

    /// Call the application run-time code and catch all remaining
    /// exceptions.  Error messages are printed to ensure that
    /// exceptional terminations are more or less graceful
    /// @param remaining_args the command line arguments that are not preceeded
    /// by a dash (-) or two dashes (--)
    /// @return the error code that will be passed to the shell
    virtual int runSafeCatchall(const arg_vector_t& remaining_args);

    /// Call the application run-time code and catch all Distillery
    /// exceptions.  Error messages are printed to ensure that
    /// exceptional terminations are more or less graceful
    /// @param remaining_args the command line arguments that are not preceeded
    /// by a dash (-) or two dashes (--)
    /// @return the error code that will be passed to the shell
    virtual int runSafeCatchDst(const arg_vector_t& remaining_args);

    /// Replace the process table name with another string
    /// @param name the new name
    virtual void setMyProcessTableName(const char* name);

    /// Replace the process table state with a state string (can be used to
    /// report application status through the process table returned by ps)
    /// The status information is added at the end of the command line as
    /// [state]
    /// @param state the new state
    virtual void setMyProcessTableState(const char* state);

    /// This method is called when the debug level is
    /// incremented. Override it to add more action when the user
    /// request higher logging level.
    virtual void increaseDebugLevel(void);

    /// Replace an argument (in the original argv) with a new string
    /// @param which the entry to be modified (between 0 and argc)
    /// @param str the string to replace the argv entry to be modified
    /// @param mangleArgv a flag that determines whether the original argv
    /// can be mangled (i.e., the argument separator will be a space instead
    /// of \\0)
    virtual void replaceArgv(const int which, char const* str, const bool mangleArgv = true);

    /// Print the usage information for the application
    /// @param o output stream
    /// @return a reference to an output stream
    virtual std::ostream& printUsage(std::ostream& o, bool extended);

    /// Add an "atexit" function. The function will be added to a list
    /// of function to call on terminaison. When the application exits,
    /// each function will be called in the reverse order of insertion
    virtual void addAtExit( /// Pointer to the function to call. this
                            /// function can either be a standalone
                            /// function or a static member of a
                            /// class. No assumption should be made about
                            /// the state of the application when this
                            /// function will be called.
      void (*fct)(void*),
      /// Value to pass to the function when called
      void* arg);

    /// Obtain this app's environment settings
    const std::map<std::string, std::string>& getEnv(void) const;

    /// Obtain Distillery Knobs
    /// @return a reference to the Distillery Knobs in this object
    DistilleryKnobs& getDistilleryKnobs(void);

    /// Check if we already done switch logger or not
    bool isTracerSwitched(void);

    /// Add function to quiesce list. Registered functions are executed
    /// LIFO.
    /// @param function Quiesce handler
    /// @return handler key for unregisterQuiesce
    virtual int registerQuiesce(Callback_0 function);

    /// Remove function from quiesce list
    /// @param handler key returned from registerQuiesce
    virtual void unregisterQuiesce(int key);

    /// Add function to shutdown list. Registered functions are executed
    /// LIFO.
    /// @param function Shutdown handler
    /// @return handler key for unregisterShutdown
    virtual int registerShutdown(Callback_0 function);

    /// Remove function from shutdown list
    /// @param handler key returned from registerShutdown
    virtual void unregisterShutdown(int key);

    /// Run quiesce handlers
    virtual void runQuiesce(void);

    /// Run quiesce handlers
    virtual void runShutdown(void);

    /// Install a signal handler for a particular signal
    /// @param sig the signal for which the signal handler is being installed
    /// @param handler the callback signal hanlder
    virtual void installSignalHandler(int sig, signal_handler_t handler);

    /// Remove signal handler for a particular signal
    /// @param sig the signal for which the signal handler is being removed
    virtual void removeSignalHandler(int sig);

    /// Obtain maximum retry setting for client/service connector binding.
    /// @return the max retry setting.
    static int getConnectorMaxRetries(void);

    /// Method to manage application stdout/stderr logging.
    virtual void stdouterrTracing();

    /// running the monitor thread for this server to monitor its health.
    virtual void runMonitorThread();

  protected:
    // -------- internal debug -------------

    /// Print the arguments (for debug purposes)
    ///
    virtual void printArgs(void) const;

    /// Print the current environment (for debug purposes)
    ///
    virtual void printEnviron(void) const;

    // -------- ENVP and ARGV handling -------------

    /// Save the original command line arguments and environment
    /// @param argc the number of arguments in the command line
    /// @param argv the original argv
    /// @param envp the original envp
    virtual void saveEnvAndArgs(const int argc, char** argv, char** envp);

    /// Save the original environment
    /// @param envp the original envp
    virtual void saveEnvironment(char** envp);

    /// Save the original command line argument list
    /// @param argc the number of arguments in the command line
    /// @param argv the original argv
    virtual void saveArgs(const int argc, char** argv);

    // -------- argument management --------

    /// Print the application description
    /// @param o output stream
    virtual std::ostream& printDescription(std::ostream& o) const;

    /// Obtain the default non-hidden application command-line options
    /// @param options the list of option_t descriptors
    virtual void getDistilleryApplicationVisibleOptions(option_vector_t& options);

    /// Obtain the default application command-line options
    /// @param options the list of default option_t descriptors
    virtual void getDistilleryApplicationOptions(option_vector_t& options);

    /// Obtain all the application command-line options
    /// @param options the list of all option_t descriptors
    virtual void getAllOptions(option_vector_t& options);

    /// Obtain all application-specific command-line options
    /// @param options the list of all option_t descriptors
    virtual void getArguments(option_vector_t& options);

    /// Set a command-line argument with a value
    /// @param option the option to be set
    /// @param value the option's new value
    virtual void setArgument(const option_t* option, const char* value);

    /// Process the help command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processHelpOption(const option_t* option, const char* value);

    /// Process the extended help command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processXHelpOption(const option_t* option, const char* value);

    /// Process the help command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processDeprecatedOptions(const option_t* option, const char* value);

    /// Process the debug command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processDebugLevel(const option_t* option, const char* value);

    /// Process the debug file command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processDebugFile(const option_t* option, const char* value);

    /// Process service tracing command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processTracing(const option_t* option, const char* value);
    /// Deprecated method for processing trace file. Kept for backward compatibility purpose.
    virtual void processRollingLoggerFile(const option_t* option, const char* value);

    /// Process service logging command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processLogging(const option_t* option, const char* value);

    /// Process log level command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processLogLevel(const option_t* option, int value);

    /// Process service stdout/stderr log setting command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processStdouterrSetting(const option_t* option, const char* value);

    virtual void processDebugConsoleFmt(const option_t* option, const char* value);

#ifdef DST_DBG_INST
    virtual void processDebugEnableFile(const option_t* option, const char* value);
    virtual void processDebugToggleFile(const option_t* option, const char* value);
#endif

    /// Process the threaded debug color command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processDebugThColor(const option_t* option, const char* value);

    /// Process the filtered threaded debug color command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processDebugFilterThColor(const option_t* option, const char* value);

    /// Process the debug color command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processDebugColor(const option_t* option, const char* value);

    /// Process the debug ring command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processDebugRing(const option_t* option, int value);

    /// Process the no-signal-handler command line request
    /// @param option the help option_t description
    /// @param value the value passed with a command line help invocation
    virtual void processNoSignalHandler(const option_t* option, const char* value);

    virtual void processNoTTY(const option_t* option, const char* value);

    virtual void processNop(const option_t* option, const char* value);
    virtual void processSNop(const option_t* option, const char* value);
    virtual void processINop(const option_t* option, int value);
    virtual void processAffinity(const option_t* option, int value);

    /// Process unique identify of this service.
    /// One purpose of service unique identify is to make this service startup
    /// entry to be instance/host/process unique enough for us to get pid easily.
    /// In current release, we use it for dname startup and also for PEC cleanup.
    /// As with above purpose, this method simply do nothing.
    /// But can be enriched for other purpose in the future.
    virtual void processUniqueId(const option_t* option, const char* value);

    /// Process the max retries option for binding from client to service.
    /// @param option the option_t description
    /// @param value the value passed with on the command line
    virtual void processConnectorMaxRetries(const option_t* option, int value);

    /// Set flag if this HC is a test mode service.
    /// @param option option name
    /// @param value option value
    virtual void processTestMode(const option_t* option, const char* value);

    // -------------------------------------

    /// Process the command line arguments
    /// @param argc the number of arguments in the command line
    /// @param argv the original argv
    /// @param remains the command line arguments that are not preceeded
    /// by a dash (-) or two dashes (--)
    virtual void processOptions(int argc, char** argv, arg_vector_t& remains);

    /// Process a [argc,argv] pair
    /// @param argc the number of arguments in argv
    /// @param argv the arguments
    /// @param options List of all the recognized options
    /// @param remains the command line arguments that are not preceeded
    /// by a dash (-) or two dashes (--)
    void processCommandLine(int argc, char** argv, option_vector_t& options, arg_vector_t& remains);

    /// Process the variables defined in the environment
    /// @param options List of all the recognized options
    void processEnvironment(option_vector_t& options);

    void callOptionFct(option_t* option, const char* value);

    /// Store the original command line arguments
    /// @param argc the number of arguments in the command line
    /// @param argv the original argv
    /// @param remains a copy of the  original command-line arguments
    virtual void loadArgs(int argc, char** argv, arg_vector_t& remains);

    /// Install default signal handler for signals that cause segmentation
    /// faults
    virtual void installFaultSignalHandlers(void);

    /// Install default signal handler for signals that cause clean process
    /// termination
    virtual void installExitSignalHandlers(void);

    // -------------------------------------

    /// Print an error message
    /// @param message the message to be printed out
    /// @return a reference to the cerr output stream
    virtual std::ostream& error(const std::string& message);

    /// Print an error message and exit with an error code
    /// @param message the message to be printed out
    /// @param code error code to be returned to the parent shell
    virtual void exit(const std::string& message, int code) __attribute__((noreturn));

    /// Terminate and exit with an error code
    /// @param code error code to be returned to the parent shell
    virtual void exit(int code) __attribute__((noreturn));

    // -------- signal management --------

    /// List of quiesce and shutdown handlers. Term signal handlers
    /// should be registered as shutdown handlers.
    std::map<int, Callback_0> _quiesce_map;
    std::map<int, Callback_0> _shutdown_map;

    /// Handler key used for both _quiesce_map and _shutdown_map
    int _handler_key;
    Mutex _handler_key_m;

    /// List of signal handlers
    std::map<int, signal_handler_t> _signal_handlers;

    /// Signal handler dispatcher
    /// @param signal the signal received by the process
    static void errorSignalHandler(int signal);

    /// Obtain a reference to the signal handler callback function
    /// @return a reference to the callback function
    virtual sighandler_t getErrorSignalHandler(void);

    virtual void faultHandler(int sig);

    /// Abort signal handler
    /// @param sig the signal that triggered the violation
    void sigAbortHandler(int sig);

    /// Segmentation Violation signal handler
    /// @param sig the signal that triggered the violation
    void sigSegVHandler(int sig);

    /// Broken pipe signal handler
    /// @param sig the signal that triggered the violation
    void sigPipeHandler(int sig);

    /// SIGILL signal handler
    /// @param sig the signal that triggered the violation
    void sigIllHandler(int sig);

    /// Floating point exception signal handler
    /// @param sig the signal that triggered the violation
    void sigFPEHandler(int sig);

    /// Bus error signal handler
    /// @param sig the signal that triggered the violation
    void sigBusHandler(int sig);

    virtual void termHandler(int sig);

    /// SIGINT signal handler
    /// @param sig the signal that triggered the violation
    void sigIntHandler(int sig);

    /// SIGTERM signal handler
    /// @param sig the signal that triggered the violation
    void sigTermHandler(int sig);

    /// SIGQUIT signal handler
    /// @param sig the signal that triggered the violation
    void sigQuitHandler(int sig);

    // -----------------------------------

    /// Set the keep remaining arguments flag -- which allows the application
    /// to receive non-options (in the getopt sense)
    void setKeepRemainingArguments(void);

    /// Set the value of _call_destructor variable. If the application
    /// object shouldnt be destroyed when exiting, set the value to
    /// false
    /// @param value new value of _call_destructor
    void setCallDestructor(bool value);

    /// Change the tracer used by the application
    void switchTracer(debug::Tracer* new_tracer);
    /// Reset debug tracer level
    void resetDebugLevel(int level);

    /// Control knobs
    DistilleryKnobs _dk;
    /// Flag that allows the application to receive non-options (in the getopt
    /// sense)
    bool _keep_remaining_args;
    std::string _argv_0;
    /// pointer to the original argv
    char** _argv;
    /// pointer to the last valid address within the envp area
    void* _envpfence;
    /// saved argument list
    char** _savedArgv;
    /// is the app state present in the argv presented by the process table?
    bool _argvState;
    /// size in bytes of the envp area
    int _envpsize;
    /// current argc
    int _argc;
    /// Should the signal handlers be installed
    bool _install_signal_handler;
    /// Should the destructor be called when the application exist
    bool _call_destructor;

    /// Pointer to the service tracer
    debug::Tracer* _spc_tracer;
    int _init_trc_level;
    std::string _trc_settings; // format of filename:maxSize:maxNum
    // mark if tracer switched in the course of application starting up.
    bool _tracerSwitched;
    /// Pointer to the service logger
    debug::Logger* _log_logger;
    int _init_log_level;
    std::string _log_settings; // format of logType:filename:maxSize:maxNum
    /// The service stdout/stderr logging
    std::string _stdouterr_settings; // format of filename:maxSize:maxNum
    int _stdouterrPipe[2];           // Pipe to manage stdout/stderr logging.
    int _originalStdoutFD, _originalStderrFD;
    StdouterrLoggingThread* _stdouterrThread;

    /// Pointer to the service audit logger
    debug::AuditLogger* _audit_logger;

    /// this app's environment settings
    std::map<std::string, std::string> ENV;
    char** _respawn_argv;
    int _respawn_argc;

    /// flag to mark that this service is in test mode (unit test)
    bool _testMode;

    /// Default constructor
    DistilleryApplication(void);
    // friend int ::main(int argc, char** argv, char** envp) ;
    /// Finish application up graciously
    friend void DistAppTerminate(int) __attribute__((noreturn));
    /// Finish application up if a dynamic memory allocation fails for
    /// lack of available memory
    friend void DistAppNoMoreMemoryHandler(void);

    static DistilleryApplication* app;

    /// Setting for maximum retry of binding of client to server.
    /// 1. If this application is a server, this setting is for all clients
    ///    used inside this service when binding to other services.
    /// 2. If this application is a client of a service, this setting is for
    ///    binding to the service.
    /// 3. If this application is a stand alone application that not initial
    ///    any service or client, this setting has no effect.
    static int _connector_max_retries;
};

std::ostream& operator<<(std::ostream& s, const DistilleryApplication::option_t& opt);

UTILS_NAMESPACE_END

#define APPEND_OPTIONS(vec, data) UTILS_VECTOR_APPEND(vec, option_t, data)
//#define MAIN_APP(classname) UTILS_NAMESPACE_QUALIFIER DistilleryApplication*
// instantiate_new_app(void) { return new classname ; }

#define MAIN_APP(classname)                                                                        \
    UTILS_NAMESPACE_QUALIFIER DistilleryApplication* instantiate_new_app(void)                     \
    {                                                                                              \
        return new classname;                                                                      \
    }                                                                                              \
    int main(int argc, char** argv, char** envp)                                                   \
    {                                                                                              \
        return UTILS_NAMESPACE_QUALIFIER DistilleryApplication::main(argc, argv, envp);            \
    }

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
