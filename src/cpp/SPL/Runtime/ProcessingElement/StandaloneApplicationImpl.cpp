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

#include <SPL/Runtime/ProcessingElement/StandaloneApplicationImpl.h>

#include <SAM/SAMNamespaceDefs.h>
#include <SAM/SAMTypes.h>
#include <SAM/augmentedApplicationModel.h>
#include <SAM/v4200/adlModel.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/ProcessingElement/BasePE.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/StandalonePlatform.h>
#include <SPL/Runtime/Utility/JNIUtils.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>
#include <SPL/Runtime/Utility/Thread.h>

#include <TRC/ConsoleTracer.h>
#include <TRC/DistilleryDebug.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include <dlfcn.h>
#include <fstream>
#include <semaphore.h>

#include <jni.h>

using namespace std;
using namespace std::tr1;
using namespace Distillery;
using namespace SPL::JNI;

namespace bf = boost::filesystem;
using namespace SPL;

/// Remember if shutdown is called from within the signal handler
static volatile bool shutdownPECalledFromSignal_ = false;

#define JTHROW(env, where)                                                                         \
    {                                                                                              \
        env->ExceptionDescribe();                                                                  \
        env->ExceptionClear();                                                                     \
        SPLTRACEMSGANDTHROW(SPLRuntimeOperator, L_ERROR,                                           \
                            (SPL_APPLICATION_RUNTIME_JAVAOP_EXCEPTION((where))), SPL_FUNC_DBG);    \
    }

/// This class is used to shutdown the PE when a signal is received
class SignalThread : public SPL::Thread
{
  public:
    SignalThread(PEImpl& pe)
      : signaled_(false)
      , pe_(pe)
    {
#ifndef NDEBUG
        int r =
#endif // NDEBUG
          sem_init(&sem_, 0, 0);
        assert(r == 0);
    }
    ~SignalThread() { sem_destroy(&sem_); }

    virtual void* run(void* arg)
    {
        registerThread("Signal");
        while (!signaled_) {
            sem_wait(&sem_);
        }
        pe_.shutdown();
        return NULL;
    }
    void signal()
    {
        signaled_ = true;
        sem_post(&sem_);
    }

  private:
    sem_t sem_;
    volatile bool signaled_;
    PEImpl& pe_;
};
static std::auto_ptr<SignalThread> signalThread_;

static string SIGNAL_SIGINT_TRAPPED_SHUTDOWN_;
static string SIGNAL_SIGQUIT_TRAPPED_SHUTDOWN_;
static string SIGNAL_SIGABRT_TRAPPED_SHUTDOWN_;
static string SIGNAL_SIGINT_TRAPPED_ABORT_;
static void initSignalHandlerMessages()
{
    SIGNAL_SIGINT_TRAPPED_SHUTDOWN_ =
      SPL_RUNTIME_STANDALONE_APPLICATION_SIGNAL_TRAPPED_SHUTDOWN("SIGINT");
    SIGNAL_SIGQUIT_TRAPPED_SHUTDOWN_ =
      SPL_RUNTIME_STANDALONE_APPLICATION_SIGNAL_TRAPPED_SHUTDOWN("SIGQUIT");
    SIGNAL_SIGABRT_TRAPPED_SHUTDOWN_ =
      SPL_RUNTIME_STANDALONE_APPLICATION_SIGNAL_TRAPPED_SHUTDOWN("SIGABRT");
    SIGNAL_SIGINT_TRAPPED_ABORT_ =
      SPL_RUNTIME_STANDALONE_APPLICATION_SIGNAL_TRAPPED_ABORT("SIGINT");
}

void StandaloneApplicationImpl::shutdownFromSignal(int sig)
{
    // avoid message catalog code inside a signal handler
    string defaultMessage = "";
    char const* message = defaultMessage.c_str();
    switch (sig) {
        case SIGINT:
            message = SIGNAL_SIGINT_TRAPPED_SHUTDOWN_.c_str();
            break;
        case SIGQUIT:
            message = SIGNAL_SIGQUIT_TRAPPED_SHUTDOWN_.c_str();
            break;
        case SIGABRT:
            message = SIGNAL_SIGABRT_TRAPPED_SHUTDOWN_.c_str();
            break;
        default:
            break;
    }
    async_safe_printf(2, "%s\n", message);
    shutdownPECalledFromSignal_ = true;
    signalThread_->signal();
}

void StandaloneApplicationImpl::mySigTermHandler(int sig)
{
    shutdownFromSignal(sig);
}

void StandaloneApplicationImpl::mySigQuitHandler(int sig)
{
    shutdownFromSignal(sig);
}

static volatile bool sigIntReceived_ = false;
void StandaloneApplicationImpl::mySigIntHandler(int sig)
{
    if (sigIntReceived_) {
        async_safe_printf(2, "%s\n", SIGNAL_SIGINT_TRAPPED_ABORT_.c_str());
        ::abort();
    } else {
        shutdownFromSignal(sig);
        sigIntReceived_ = true;
    }
}

StandaloneApplicationImpl::StandaloneApplicationImpl()
  : _logLevelSet(false)
  , _traceLevelSet(false)
  , _stderrLoggerSet(false)
  , _theModel(NULL)
  , _thePE(NULL)
  , _shutdownTimer(0)
  , _appLogger(NULL)
  , _logLogger(NULL)
{
    APPTRC(L_INFO, "StandaloneApplication constructor", SPL_APP_DBG);
    _thePlatform.reset(new StandalonePlatform());
}

StandaloneApplicationImpl::~StandaloneApplicationImpl()
{
    if (signalThread_.get()) {
        signalThread_->signal();
        signalThread_->join();
    }
}

int StandaloneApplicationImpl::init()
{
    int r = DistilleryApplication::init();

    try {
        loadADL();
    } catch (SPLRuntimeUnableToLoadADLException const& e) {
        cerr << e.getExplanation() << endl;
        cerr << SPL_RUNTIME_CANNOT_LOAD_ADL_MAKE_SURE_THERE_IS_A_MAIN << endl;
        exit(EXIT_FAILURE);
    }

    return r;
}

int StandaloneApplicationImpl::run(vector<string> const& args)
{
    APPTRC(L_INFO, "Entering MyApplication::run()", SPL_APP_DBG);

    setArguments(args);
    try {
        createPE();
        PEImpl& peImpl = dynamic_cast<PEImpl&>(_thePE->getImpl());
        signalThread_.reset(new SignalThread(peImpl));
        signalThread_->create();
        setSignalHandlers();

        APPTRC(L_INFO, "Starting the standalone application processing", SPL_APP_DBG);
        process();
        APPTRC(L_INFO, "Completed the standalone application processing", SPL_APP_DBG);

        APPTRC(L_INFO, "Leaving MyApplication::run()", SPL_APP_DBG);
    } catch (...) {
        return 1;
    }
    return 0;
}

void StandaloneApplicationImpl::setSignalHandlers()
{
    initSignalHandlerMessages();
    // We will handle the following three signals ourselves: SIGTERM, SIGINT, SIGQUIT
    DistilleryApplication::getThisApp().removeSignalHandler(SIGTERM);
    DistilleryApplication::getThisApp().installSignalHandler(
      SIGTERM, mem_cb(this, &StandaloneApplicationImpl::mySigTermHandler));
    DistilleryApplication::getThisApp().removeSignalHandler(SIGINT);
    DistilleryApplication::getThisApp().installSignalHandler(
      SIGINT, mem_cb(this, &StandaloneApplicationImpl::mySigIntHandler));
    DistilleryApplication::getThisApp().removeSignalHandler(SIGQUIT);
    DistilleryApplication::getThisApp().installSignalHandler(
      SIGQUIT, mem_cb(this, &StandaloneApplicationImpl::mySigQuitHandler));

    // We want the default behavior for SIGABRT, not a stack trace
    DistilleryApplication::getThisApp().removeSignalHandler(SIGABRT);
}

void StandaloneApplicationImpl::setArguments(vector<string> const& args)
{
    _rawArgs.assign(args.begin(), args.end());
    for (size_t i = 0, iu = args.size(); i < iu; ++i) {
        string const& arg = args[i];
        string::size_type pos = arg.find("=");
        if (pos == string::npos) {
            continue; // not found
        }
        if (pos == 0) {
            continue; // =xxx  (no name field)
        }
        string name = arg.substr(0, pos);
        string value = arg.substr(pos + 1);
        pair<string, string> tmpPair = make_pair(name, value);
        _namedArgValues.push_back(tmpPair);
    }
}

void StandaloneApplicationImpl::createPE()
{
    // Need to create the PE first as it contains the JVM factory
    BasePE* basePE = new SPL::SplPE(true /* is_standalone */);
    PEImpl& pi = dynamic_cast<SPL::PEImpl&>(basePE->getImpl());
    JVMFactory::instance().setArguments(_finalVMArgs);

    // Create the PEModel
    createAADL();
    initializeEnv();

    // Now we can initialize the PE further
    pi.initialize(*_theModel, *_thePlatform);
    _thePE = &pi.getPE();
    assert(_thePE != NULL);
}

string StandaloneApplicationImpl::getADLFileName()
{
    std::string exe;
    bf::path adl_path;
    getExecutableLocation(adl_path, exe);

    // ADL is in ../etc
    adl_path = (adl_path.branch_path() / "etc" / "adl");
    return adl_path.string();
}

void findSubmissionTimeValues(
  const xmlns::prod::streams::application::v4200::applicationSetType& adl,
  unordered_set<string>& requiredValues,
  unordered_map<string, string>& optionalValues)
{
    using namespace xmlns::prod::streams::application::v4200;

    // Get the required command line arguments from ADL
    const applicationType& appl = adl.splApplication()[0];
    if (!appl.submissionTimeValues().present()) {
        return;
    }

    const submissionTimeValuesType::submissionTimeValue_sequence& subValues =
      appl.submissionTimeValues().get().submissionTimeValue();
    submissionTimeValuesType::submissionTimeValue_const_iterator it;
    for (it = subValues.begin(); it != subValues.end(); it++) {
        if (!it->defaultValue().present()) {
            ostringstream ostr;
            ostr << '[' << it->compositeName() << ".]" << it->name();
            requiredValues.insert(ostr.str());
        } else {
            ostringstream ostr;
            ostr << '[' << it->compositeName() << ".]" << it->name();
            optionalValues.insert(make_pair(ostr.str(), it->defaultValue().get()));
        }
    }
}

void StandaloneApplicationImpl::checkForJVMArgs(
  const xmlns::prod::streams::application::v4200::compositeOperInstanceType& compositeInstance)
{
    using namespace xmlns::prod::streams::application::v4200;

    // Recurse down all nested composite instances
    for (compositeOperInstanceType::compositeOperInstance_const_iterator
           it = compositeInstance.compositeOperInstance().begin(),
           itEnd = compositeInstance.compositeOperInstance().end();
         it != itEnd; ++it) {
        const compositeOperInstanceType& compositeOperInstance = *it;
        checkForJVMArgs(compositeOperInstance);
    }
    // Check all primitive operators for JVM args
    for (compositeOperInstanceType::primitiveOperInstance_const_iterator
           it1 = compositeInstance.primitiveOperInstance().begin(),
           it1End = compositeInstance.primitiveOperInstance().end();
         it1 != it1End; ++it1) {
        const primitiveOperInstanceType& primtiveOperInst = *it1;
        if (primtiveOperInst.jvmArguments().present()) {
            const jvmArgumentsType& jvmArgs = primtiveOperInst.jvmArguments().get();
            for (jvmArgumentsType::jvmArgument_const_iterator it2 = jvmArgs.jvmArgument().begin(),
                                                              it2End = jvmArgs.jvmArgument().end();
                 it2 != it2End; ++it2) {
                if (_jvmArgs.find(*it2) == _jvmArgs.end()) {
                    _jvmArgs.insert(*it2);
                    _finalVMArgs.push_back(*it2);
                }
            }
        }
    }
}

void StandaloneApplicationImpl::loadADL()
{
    // Read in the ADL, and convert to AADL
    string adl_path = getADLFileName();

    ifstream istr(adl_path.c_str(), fstream::in);
    if (!istr) {
        SPLTRACEMSGANDTHROW(SPLRuntimeUnableToLoadADL, L_ERROR,
                            SPL_RUNTIME_ADL_OPEN_FAILED(adl_path, RuntimeUtility::getErrorNoStr()),
                            SPL_OPER_DBG);
    }

    // Load the ADL
    using namespace xmlns::prod::streams::application::v4200;
    try {
        _applicationSet = applicationSet(istr, xsd::cxx::tree::flags::dont_validate);
    } catch (::xsd::cxx::exception& e) {
        SPLTRACEMSGANDTHROW(SPLRuntimeUnableToLoadADL, L_ERROR,
                            SPL_RUNTIME_ADL_LOAD_FAILED(adl_path, e.what()), SPL_OPER_DBG);
    }

    // We need to do some patching to the ADL
    applicationType& appl = _applicationSet->splApplication()[0];
    assert(appl.standaloneApplicationDirectory().present());
    appl.applicationDirectory(appl.standaloneApplicationDirectory().get());

    // Walk the ADL and see if any of the primitive operators need a JVM
    const compositesType& composites = appl.composites();
    const compositeOperInstancesType& compositeInstances = composites.compositeOperInstances();
    for (compositeOperInstancesType::compositeOperInstance_const_iterator
           it = compositeInstances.compositeOperInstance().begin(),
           itEnd = compositeInstances.compositeOperInstance().end();
         it != itEnd; ++it) {
        const compositeOperInstanceType& compositeOperInstance = *it;
        checkForJVMArgs(compositeOperInstance);
    }

    // Find about the required command line arguments
    findSubmissionTimeValues(*_applicationSet, _requiredArgs, _optionalArgs);
}

static string makeAbsolutePath(string const& opath, string const& root)
{
    bf::path op = opath;
    if (!op.is_complete()) {
        op = root / op;
    }
    return op.string();
}

static jobject buildProperties(JVMEnv& jvmEnv, vector<pair<string, string> > subTimePairs)
{
    jclass propertiesClass = jvmEnv->FindClass("java/util/Properties");
    if (jvmEnv->ExceptionCheck()) {
        JTHROW(jvmEnv.get(), "Properties building"); // Internal error if this can't be found
    }
    jmethodID ctor = jvmEnv->GetMethodID(propertiesClass, "<init>", "()V");
    if (jvmEnv->ExceptionCheck()) {
        JTHROW(jvmEnv.get(), "Properties building"); // Internal error if this can't be found
    }
    jmethodID setProp = jvmEnv->GetMethodID(
      propertiesClass, "setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");
    if (jvmEnv->ExceptionCheck()) {
        JTHROW(jvmEnv.get(), "Properties building"); // Internal error if this can't be found
    }
    jobject properties = jvmEnv->NewObject(propertiesClass, ctor);
    if (jvmEnv->ExceptionCheck()) {
        JTHROW(jvmEnv.get(), "Properties building"); // Internal error if this can't be found
    }

    for (vector<pair<string, string> >::const_iterator it = subTimePairs.begin(),
                                                       itEnd = subTimePairs.end();
         it != itEnd; ++it) {
        SPL::JNI::JString key(jvmEnv.get(), it->first);
        SPL::JNI::JString value(jvmEnv.get(), it->second);
        jvmEnv->CallObjectMethod(properties, setProp, key.get(), value.get());
        if (jvmEnv->ExceptionCheck()) {
            JTHROW(jvmEnv.get(), "Properties building"); // Internal error if this can't be found
        }
    }

    return properties;
}

void StandaloneApplicationImpl::createAADL()
{
    // Load a JVM and call it to get the aadl
    {
        APPTRC(L_INFO, "StandaloneApplicationImpl: Starting PE JVM Setup", SPL_JAVAOP_DBG);
        {
            JVMEnv jvmEnv(true);
            JNIEnv* env = NULL;
            jvmEnv.getJNIEnv(&env);
            SPL::JNI::JClass aadlBuilder(env, "com/ibm/streams/spl/internal/application/ADLLoader");
            jmethodID aadlLoaderFunction = aadlBuilder.getStaticMethodID(
              "getAADL", "(Ljava/lang/String;Ljava/util/Properties;II)Ljava/nio/ByteBuffer;");
            string adlFileName = getADLFileName();
            SPL::JNI::JString javaString(env, adlFileName);

            // Build the submission-time value properties
            jobject properties = buildProperties(jvmEnv, _namedArgValues);

            jobject jo =
              env->CallStaticObjectMethod(aadlBuilder.get(), aadlLoaderFunction, javaString.get(),
                                          properties, _logLevel, _traceLevel);
            if (jo == NULL) {
                const FormattableMessage& m = SPL_RUNTIME_FAILED_AADL_CREATION;
                THROW_STRING(SPLRuntimeUnableToCreateAADL, m);
            }
            if (jvmEnv->ExceptionCheck()) {
                JTHROW(jvmEnv.get(), "call to getAADL");
            }
            void* buff = jvmEnv->GetDirectBufferAddress(jo);
            int64_t buffLen = jvmEnv->GetDirectBufferCapacity(jo);

            string aadlString((char*)buff, buffLen);
            APPTRC(L_TRACE, aadlString, SPL_APP_DBG);

            // Load the AADL from the string
            istringstream istr(aadlString);
            using namespace xmlns::prod::streams::application::augmented;
            try {
                _theModel = augmentedPE(istr, xsd::cxx::tree::flags::dont_validate);
            } catch (::xsd::cxx::exception& e) {
                SPLTRACEMSGANDTHROW(SPLRuntimeUnableToParseAADL, L_ERROR,
                                    SPL_RUNTIME_FAILED_AADL_PARSE, SPL_OPER_DBG);
            }
        }
    }

    // fix-up relative paths
    std::string exe;
    bf::path dir; // dir is output/bin
    getExecutableLocation(dir, exe);
    string base = bf::system_complete(dir.branch_path()).string();

    _theModel->outputDirectory(base);
    APPTRC(L_INFO, "PE output directory is set to: '" << _theModel->outputDirectory() << "'",
           SPL_PE_DBG);
    _theModel->applicationDirectory(makeAbsolutePath(_theModel->applicationDirectory(), base));
    APPTRC(L_INFO,
           "PE application directory is set to: '" << _theModel->applicationDirectory() << "'",
           SPL_PE_DBG);

    if (!_dataDirectory.empty()) {
        // _dataDirectory came fro the option. Make sure it is fully qualified otherwise it will
        // be considered to be in the output dir
        bf::path p = bf::absolute(_dataDirectory);
        _theModel->dataDirectory(p.string());
    }
    if (_theModel->dataDirectory().present()) {
        _theModel->dataDirectory(makeAbsolutePath(_theModel->dataDirectory().get(), base));
    }

    APPTRC(L_INFO, "PE data directory is set to: '" << _theModel->dataDirectory() << "'",
           SPL_PE_DBG);
    if (_theModel->checkpointDirectory().present()) {
        _theModel->checkpointDirectory().set(
          makeAbsolutePath(_theModel->checkpointDirectory().get(), base));
        APPTRC(L_INFO,
               "PE checkpoint directory is set to: '" << _theModel->checkpointDirectory() << "'",
               SPL_PE_DBG);
    }
    _theModel->executable().executableUri(
      makeAbsolutePath(_theModel->executable().executableUri(), base));
    APPTRC(L_INFO,
           "PE executable directory is set to: '" << _theModel->executable().executableUri() << "'",
           SPL_PE_DBG);
}

void StandaloneApplicationImpl::initializeEnv()
{
    // If the AADL specifies any env vars set them
    const EnvVars& envVars = _theModel->envVars();
    EnvVars::envVar_sequence::const_iterator it = envVars.envVar().begin();
    for (; it != envVars.envVar().end(); ++it) {
        const EnvVar& envVar = *it;
        setenv(envVar.name().c_str(), envVar.value().c_str(), 1);
        APPTRC(L_INFO,
               "Environment variable " << envVar.name() << " set to: '" << envVar.value() << "'",
               SPL_PE_DBG);
    }
}

/// This class is used to shutdown the PE when a timer goes off
class ShutDownTimer : public SPL::Thread
{
  public:
    ShutDownTimer(double seconds, PEImpl& pe)
      : seconds_(seconds)
      , pe_(pe)
    {}
    ~ShutDownTimer() {}

    virtual void* run(void* arg)
    {
        registerThread("ShutDownTimer");
        APPTRC(L_INFO, "Timeout after " << seconds_ << " seconds", SPL_APP_DBG);
        pe_.blockUntilShutdownRequest(seconds_);
        APPTRC(L_INFO, "Shutdown Application", SPL_APP_DBG);
        pe_.shutdown();
        return NULL;
    }

  private:
    double seconds_;
    PEImpl& pe_;
};

SplPE* StandaloneApplicationImpl::getPE()
{
    return _thePE;
}

void StandaloneApplicationImpl::setupLogger()
{
    switchTracer(new debug::ConsoleTracer(0, !_stderrLoggerSet));

    // For standalone application, using ConsoleTrace object so to use
    // trace format for logging, to avoid format different.
    _logLogger = new debug::ConsoleTracer(0, false); // log to stderr
    debug::set_log_logger(_logLogger);
    if (!_logLevelSet) {
        _logLevel = iL_WARN;
    }
    debug::set_log_level(_logLevel);
    if (_spc_tracer) {
        _spc_tracer->setLevel(_logLevel);
    }
    _appLogger = new debug::ConsoleTracer(0, !_stderrLoggerSet);
    debug::set_app_tracer(_appLogger);
    if (!_traceLevelSet) {
        _traceLevel = iL_OFF;
    }
    debug::set_app_trclevel(_traceLevel);
}

static int translateLevel(const std::string& s)
{
    if (s == "trace") {
        return iL_TRACE;
    }
    if (s == "debug") {
        return iL_DEBUG;
    }
    if (s == "info") {
        return iL_INFO;
    }
    if (s == "off") {
        return iL_OFF;
    }
    if (s == "warn") {
        return iL_WARN;
    }

    // Default and error case
    return iL_ERROR;
}

void StandaloneApplicationImpl::process()
{
    auto_ptr<ShutDownTimer> t;
    PEImpl& pi = dynamic_cast<PEImpl&>(_thePE->getImpl());
    if (_shutdownTimer > 0) {
        // Create a thread to shutdown after _shutdownTimer seconds
        t.reset(new ShutDownTimer(_shutdownTimer, pi));
        t->create();
    }

    const std::string& level = "off";
    if (!_traceLevelSet) {
        int l;
        if (_theModel->currentTracing().present()) {
            l = translateLevel(_theModel->currentTracing().get().tracingUpdate().level());
        } else {
            l = translateLevel(level);
        }
        _spc_tracer->setLevel(l);
        _traceLevel = l;
        debug::set_streams_trclevel(l);
        debug::set_app_trclevel(l);
    }
    pi.traceLevelUpdate(_traceLevel);
    pi.logLevelUpdate(_logLevel);

    try {
        _thePE->getImpl().process();
    } catch (...) {
        // all error logging is done in _thePE->process() above
        exit(EXIT_FAILURE);
    }

    if (_shutdownTimer > 0) {
        t->join();
    }
}

ostream& StandaloneApplicationImpl::printDescription(ostream& o) const
{
    return o << "IBM Streams - " << SPL_RUNTIME_STANDALONE_SPL_APPLICATION;
}

ostream& StandaloneApplicationImpl::printUsage(ostream& o, bool extended)
{
    DistilleryApplication::printUsage(o, extended);
    o << endl;
    o << SPL_RUNTIME_STANDALONE_APPLICATION_ARGUMENT_HELP << endl;
    if (_requiredArgs.size() > 0) {
        ostringstream ostr;
        size_t i = 0, iu = _requiredArgs.size();
        typedef unordered_set<string>::const_iterator iter;
        for (iter it = _requiredArgs.begin(); it != _requiredArgs.end(); ++it, ++i) {
            ostr << *it;
            if (i != iu - 1) {
                ostr << ", ";
            }
        }
        o << SPL_RUNTIME_STANDALONE_APPLICATION_REQUIRED_ARGUMENT_HELP(ostr.str()) << endl;
    }
    if (_optionalArgs.size() > 0) {
        ostringstream ostr;
        size_t i = 0, iu = _optionalArgs.size();
        typedef unordered_map<string, string>::const_iterator iter;
        for (iter it = _optionalArgs.begin(); it != _optionalArgs.end(); ++it, ++i) {
            ostr << it->first << " (" << it->second << ")";
            if (i != iu - 1) {
                ostr << ", ";
            }
        }
        o << SPL_RUNTIME_STANDALONE_APPLICATION_OPTIONAL_ARGUMENT_HELP(ostr.str()) << endl;
    }
    return o;
}

void StandaloneApplicationImpl::getArguments(option_vector_t& options)
{
    setKeepRemainingArguments();
    option_t args[] = {
        { 'h', "help", 0, "", SPL_RUNTIME_OPTION_HELP,
          STR_OPT(StandaloneApplicationImpl::setHelp) },
        { 'c', "log-to-stderr", 0, "", SPL_RUNTIME_OPTION_CONSOLE,
          INT_OPT(StandaloneApplicationImpl::setConsole) },
        { 'd', "data-directory", ARG, "", SPL_RUNTIME_OPTION_DATA_DIRECTORY,
          STR_OPT(StandaloneApplicationImpl::setDataDirectory) },
        { 'k', "kill-after", ARG, "", SPL_RUNTIME_OPTION_KILL_AFTER,
          STR_OPT(StandaloneApplicationImpl::setShutdownTimer) },
        { 'l', "log-level", ARG, "", SPL_RUNTIME_OPTION_NEW_LOG_LEVEL,
          INT_OPT(StandaloneApplicationImpl::setLogLevel) },
        { 't', "trace-level", ARG, "", SPL_RUNTIME_OPTION_TRACE_LEVEL,
          INT_OPT(StandaloneApplicationImpl::setTraceLevel) },
    };
    APPEND_OPTIONS(options, args);
}

void StandaloneApplicationImpl::setDataDirectory(const option_t* option, const char* value)
{
    _dataDirectory = string(value);
}

void StandaloneApplicationImpl::setHelp(const option_t* option, const char* value)
{
    printUsage(cout, false);
    cout << endl;
    Distillery::DistAppTerminate(0);
}

void StandaloneApplicationImpl::setConsole(const option_t* option, int value)
{
    _stderrLoggerSet = true;
}

void StandaloneApplicationImpl::setLogLevel(const option_t* option, int value)
{
    switch (value) {
        case 0:
            _logLevel = iL_ERROR;
            _logLevelSet = true;
            break;
        case 1:
            _logLevel = iL_WARN;
            _logLevelSet = true;
            break;
        case 2:
            _logLevel = iL_INFO;
            _logLevelSet = true;
            break;
    }
}

void StandaloneApplicationImpl::setTraceLevel(const option_t* option, int value)
{
    if (value >= iL_OFF && value <= iL_TRACE) {
        _traceLevel = value;
        _traceLevelSet = true;
    }
}

void StandaloneApplicationImpl::setShutdownTimer(const option_t* option, const char* param)
{
    istringstream istr(param);
    istr.imbue(locale::classic());
    istr >> _shutdownTimer;
    if (!istr || _shutdownTimer <= 0) {
        cerr << SPL_RUNTIME_INVALID_SHUTDOWN_DURATION(string(param)) << endl;
        exit(EXIT_FAILURE);
    }
}

void StandaloneApplicationImpl::getExecutableLocation(bf::path& exeDirPath, string& exeFileName)
{
    string path(_argv_0);
    string::size_type idx = path.find_last_of('/');
    assert(idx != string::npos);
    bf::path exePath = path.substr(0, idx);
    exePath = (exePath == ".") ? bf::current_path() : bf::system_complete(exePath);

    // Clean up the path (some callers don't want '.' in it)
    for (bf::path::iterator it = exePath.begin(); it != exePath.end(); ++it) {
        if (*it != ".") {
            exeDirPath /= *it;
        }
    }
    exeFileName = path.substr(idx + 1);
}
