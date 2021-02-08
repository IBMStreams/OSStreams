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

#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <dlfcn.h>
#include <jni.h>

#include <SAM/SAMHelperFunctions.h>
#include <SAM/SAMTypes.h>
#include <SAM/augmentedApplicationModel.h>
#include <SAM/physicalApplicationModel.h>

#include <SPL/Debugger/ProbePointServices.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorContextImpl.h>
#include <SPL/Runtime/Operator/OperatorMetricsImpl.h>
#include <SPL/Runtime/Operator/Port/ActiveQueue.h>
#include <SPL/Runtime/Operator/Port/DebugHook.h>
#include <SPL/Runtime/Operator/Port/OperatorInputPort.h>
#include <SPL/Runtime/Operator/Port/OperatorOutputPort.h>
#include <SPL/Runtime/Operator/Port/ProcessSignal.h>
#include <SPL/Runtime/Operator/Port/StreamProperty.h>
#include <SPL/Runtime/Operator/Port/StreamPropertyCollection.h>
#include <SPL/Runtime/Operator/Port/SubmitSignal.h>
#include <SPL/Runtime/Operator/State/CheckpointContextImpl.h>
#include <SPL/Runtime/Operator/State/StateHandler.h>
#include <SPL/Runtime/Operator/ThreadingModelContext.h>
#include <SPL/Runtime/ProcessingElement/PECallbackRegistery.h>
#include <SPL/Runtime/ProcessingElement/PECheckpointService.h>
#include <SPL/Runtime/ProcessingElement/PECleaners.h>
#include <SPL/Runtime/ProcessingElement/PEConsistentRegionService.h>
#include <SPL/Runtime/ProcessingElement/PEDebugService.h>
#include <SPL/Runtime/ProcessingElement/PEOPortConnectionCallback.h>
#include <SPL/Runtime/ProcessingElement/PEVisualizer.h>
#include <SPL/Runtime/ProcessingElement/PEWorkItems.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/ProcessingElement/ViewProperties.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Utility/BackoffSpinner.h>
#include <SPL/Runtime/Utility/JNIUtils.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Runtime/Utility/MessageFormatter.h>
#include <SPL/Runtime/Utility/Singleton.t>

#include <TRANS/DataReceiver.h>
#include <TRANS/DynDataSender.h>
#include <UTILS/CV.h>
#include <UTILS/Mutex.h>
#include <UTILS/SupportFunctions.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include <sys/types.h>

#include <sched.h>

using namespace boost;
using namespace SPL;
using namespace Distillery;
using namespace Streams;
using namespace AADL;
using namespace std;
using namespace std::tr1;
namespace bf = boost::filesystem;

template class Singleton<SPL::PEImpl>; // explicitly instantiate the singleton here

const ThreadingModelKind DEFAULT_THREADING_MODEL = AUTOMATIC;

// PEC creates threads that call some of the PEImpl methods.
// If those methods access Java code through JNI, the lower-level Java
// code attaches the thread to the JVM, and does not detach it.
// This can cause a problem with the PEC threads, which typically are
// destroyed after a single call into a PEImpl method.
// This class helps with that problem by detaching a thread from the
// JVM, but only in the case where it was not already attached to the
// the the instance of this class was created.
//
// On construction, test whether the current thread is attached to a
// JVM.  On destruction, if it was not attached, test again whether
// it is attached, and detach it it is.
class AutoJVMDetach
{
  public:
    AutoJVMDetach(PEImpl const& thisPe)
      : pe(thisPe)
      , wasAttached()
    {
        wasAttached = isThreadAttachedToJVM();
    }

    ~AutoJVMDetach()
    {
        if (!wasAttached && isThreadAttachedToJVM()) {
            detachThreadFromJVM();
        }
    }

  private:
    bool isThreadAttachedToJVM() const
    {
        bool isAttached = false;
        JavaVM* vm = pe.getJavaVM();
        if (vm) {
            void* env = 0;
            jint result = vm->GetEnv(&env, JNI_VERSION_1_4);
            isAttached = (result == JNI_OK);
        }
        return isAttached;
    }

    void detachThreadFromJVM() const
    {
        JavaVM* vm = pe.getJavaVM();
        if (vm) {
            vm->DetachCurrentThread();
        }
    }

    PEImpl const& pe;
    bool wasAttached; // Was thread attached to JVM at construction?
};

// timer for processing interval based metrics
class PEImpl::MetricsIntervalTimer : public Thread
{
  public:
    static const uint32_t METRICS_INTERVAL_SECS = 300;
    MetricsIntervalTimer(PEImpl& pe)
      : pe_(pe)
      , shutdown_(false)
    {}
    ~MetricsIntervalTimer() {}
    void* run(void* arg)
    {
        struct timespec interval;
        interval.tv_sec = METRICS_INTERVAL_SECS;
        interval.tv_nsec = 0;
        Distillery::AutoMutex am(mutex_);
        while (!shutdown_) {
            cv_.waitFor(mutex_, interval);
            if (shutdown_) {
                break;
            }
            pe_.processIntervalMetrics();
        }
        return NULL;
    }
    void shutdown()
    {
        Distillery::AutoMutex am(mutex_);
        shutdown_ = true;
        cv_.signal();
    }

  private:
    PEImpl& pe_;
    bool shutdown_;
    Distillery::Mutex mutex_;
    Distillery::CV cv_;
};

/**
 * RAII wrapper for AAS session token
 */
class AutoUserSession
{
  public:
    AutoUserSession(PlatformAdapter& pa, std::string& token)
      : pa_(pa)
      , token_(token)
    {}
    ~AutoUserSession()
    {
        if (!token_.empty()) {
            pa_.closeUserSession(token_);
        }
    }

  private:
    PlatformAdapter& pa_;
    std::string& token_;
};

ProcessingElement& ProcessingElement::pe()
{
    return PEImpl::instance();
}

static string findInstallDir()
{
    const char* installDir = getenv("STREAMS_INSTALL");
    if (NULL == installDir) {
        THROW(SPLRuntime, SPL_RUNTIME_ENVIRONMENT_UNDEFINED("STREAMS_INSTALL"));
    }
    return installDir;
}

PEImpl::PEImpl(SplPE& pex)
  : Singleton<PEImpl>(*this)
  , streamsInstallDir_(findInstallDir())
  , pe_(pex)
  , peModel_(NULL)
  , platform_(NULL)
  , isStandalone_(true)
  , isInConsistentRegion_(false)
  , globalThreadingModel_(DEFAULT_THREADING_MODEL)
  , metricsIntervalTimer_(NULL)
  , connStateListener_(NULL)
  , handback_(NULL)
  , visualizer_(new PEVisualizer(*this))
  , javaVMArgs_(NULL)
  , javaVMFactory_(new SPL::JNI::JVMFactory(getStreamsInstallDirectory(), javaVMArgs_.get()))
  , javaSPLRuntime_(new JavaSPLRuntime(*this, *(javaVMFactory_.get())))
  , oldSigPipeHandler_(NULL)
  , mustRethrowException_(false)
{}

PEImpl::PEImpl(SplPE& pex, bool standalone)
  : Singleton<PEImpl>(*this)
  , streamsInstallDir_(findInstallDir())
  , pe_(pex)
  , peModel_(NULL)
  , platform_(NULL)
  , isStandalone_(standalone)
  , isInConsistentRegion_(false)
  , globalThreadingModel_(DEFAULT_THREADING_MODEL)
  , metricsIntervalTimer_(NULL)
  , connStateListener_(NULL)
  , handback_(NULL)
  , visualizer_(standalone ? NULL : new PEVisualizer(*this))
  , javaVMArgs_(NULL)
  , javaVMFactory_(new SPL::JNI::JVMFactory(getStreamsInstallDirectory(), javaVMArgs_.get()))
  , javaSPLRuntime_(new JavaSPLRuntime(*this, *(javaVMFactory_.get())))
  , oldSigPipeHandler_(NULL)
  , mustRethrowException_(false)
{}

static bool isCheckpointing(PEModel& pemod)
{
    using namespace xmlns::prod::streams::application;
    operInstancesType::operInstance_const_iterator it;
    const operInstancesType::operInstance_sequence& ops = pemod.operInstances().operInstance();

    for (it = ops.begin(); it != ops.end(); it++) {
        if (it->checkpointing().present()) {
            return true;
        }
    }
    return false;
}

// A PE is in a CC region if at least one
// operator is in a CC region. This can be
// determined by the ccRegionEntry annotation in the AADL
static bool isInCCRegion(PEModel& pemod)
{
    using namespace xmlns::prod::streams::application;
    operInstancesType::operInstance_const_iterator it;
    const operInstancesType::operInstance_sequence& ops = pemod.operInstances().operInstance();

    for (it = ops.begin(); it != ops.end(); it++) {
        const OPModel& opmod = (*it);
        const PhysOPModel& physOPModel = static_cast<const PhysOPModel&>(opmod);
        PADL::physOperInstanceType::annotation_const_iterator annotIt;
        PADL::physOperInstanceType::annotation_sequence const& annots = physOPModel.annotation();
        for (annotIt = annots.begin(); annotIt != annots.end(); annotIt++) {
            const PADL::physOperInstanceType::annotation_type& annot = *annotIt;
            if (annot.tag() == CC_REGION_ENTRY_ANNOTATION) {
                return true;
            }
        }
    }
    return false;
}

// Get the number of operators in the CC region
static int getNumCROperator(PEModel& pemod)
{
    int numOps = 0;
    using namespace xmlns::prod::streams::application;
    operInstancesType::operInstance_const_iterator it;
    const operInstancesType::operInstance_sequence& ops = pemod.operInstances().operInstance();

    for (it = ops.begin(); it != ops.end(); it++) {
        const OPModel& opmod = (*it);
        const PhysOPModel& physOPModel = static_cast<const PhysOPModel&>(opmod);
        PADL::physOperInstanceType::annotation_const_iterator annotIt;
        PADL::physOperInstanceType::annotation_sequence const& annots = physOPModel.annotation();
        for (annotIt = annots.begin(); annotIt != annots.end(); annotIt++) {
            const PADL::physOperInstanceType::annotation_type& annot = *annotIt;
            if (annot.tag() == CC_REGION_ENTRY_ANNOTATION) {
                numOps++;
                break;
            }
        }
    }
    return numOps;
}

// Set by PEImpl::initialize
int PE_Version = 0;

void setYieldBehaviour(string behaviour)
{
    if (behaviour == "auto") {
        BackoffSpinner::setYieldBehaviour(BackoffSpinner::Auto);
    }
    if (behaviour == "yes") {
        BackoffSpinner::setYieldBehaviour(BackoffSpinner::Yes);
    }
    if (behaviour == "no") {
        BackoffSpinner::setYieldBehaviour(BackoffSpinner::No);
    }
}

// Note: this member function is part of the BasePEImpl interface, and needs
// to be protected from asynchronous access. However, it does not access operators_
// or outputPorts_, so it does not need to acquire any mutexes.
void PEImpl::initialize(PEModel& pemod, PlatformAdapter& platform)
{
    /*
    // trace the PE model
    ostringstream pemodstr;
    xmlns::prod::streams::application::augmented::augmentedPE(pemodstr, pemod);
    APPTRC(L_TRACE, pemodstr.str(), SPL_PE_DBG);
    */

    string version(pemod.version());
    istringstream istr(version);
    istr >> PE_Version;
    APPTRC(L_INFO, "PE version is " << version, SPL_PE_DBG);
    APPTRC(L_INFO, "PE launch count is " << pemod.launchCount(), SPL_PE_DBG);
    init();

    orb_ = NULL;
    rc_ = NULL;

    peModel_ = &pemod;
    platform_ = &platform;
    isOptimized_ = pemod.optimized();
    isCheckpointing_ = isCheckpointing(pemod);
    isDebugging_ = pemod.executable().wrapper() == "sdb";
    shutdownRequested_ = false;
    isJobCancellation_ = false;
    isCkptDeletionDone_ = false;
    allPortsReady_ = false;
    operatorsCreated_ = false;
    recordOldSigPipeHandler();
    peId_ = pemod.id();
    /*
     * NOTE(xrg) We have a semantic discrepancy here. The model defines the concept of
     * "launch count" ranging from [1, ∞) while the PE uses the concept of "relaunch count"
     * ranging from [0, ∞). We address that discrepancy here.
     */
    relaunchCount_ = pemod.launchCount() - 1;
    jobId_ = pemod.jobId();
    if (pemod.dataDirectory().present()) {
        dataDirectory_ = pemod.dataDirectory().get();
    }
    if (pemod.currentWorkingDirectory().present()) {
        currentWorkingDirectory_ = pemod.currentWorkingDirectory().get();
    }
    applicationDirectory_ = pemod.applicationDirectory();
    outputDirectory_ = pemod.outputDirectory();
    checkpointDirectory_ = pemod.checkpointDirectory().get(); // sc always populates this
    if (!isStandalone_) {
        assert(pemod.profileDirectory().present());
        profileDirectory_ = pemod.profileDirectory().get();
    }

    {
        // Build some mappings that enable us to find the location of a toolkit by index or by name
        // At the same time register the toolkit with the formatter in case it has resources
        SPL::RuntimeMessageFormatter& formatter = SPL::RuntimeMessageFormatter::instance();
        const ToolkitsMapType& toolkitsMap = pemod.toolkitsMap();
        for (ToolkitsMapType::toolkitMap_const_iterator it = toolkitsMap.toolkitMap().begin(),
                                                        itEnd = toolkitsMap.toolkitMap().end();
             it != itEnd; ++it) {
            const ToolkitMapType& tkMap = *it;
            const string& tkName = tkMap.name();
            toolkitNameMap_.insert(make_pair(tkMap.index(), tkName));
            toolkitIndexMap_.insert(make_pair(tkName, tkMap.index()));

            string path = getToolkitDirectory(tkName);
            formatter.registerToolkit(tkName, path);
        }
    }

    // Create the extra aspects for logging
    {
        ostringstream os;
        os << "J[" << jobId_ << "],P[" << peId_ << "],";
        extraAspectInfo_ = os.str();
    }

    if (dataDirectory_.empty()) {
        APPTRC(L_INFO, "No data directory specified", SPL_PE_DBG);
    }
    if (!currentWorkingDirectory_.empty()) {
        // Change working directory to data directory if one was specified
        bf::path cwd(currentWorkingDirectory_);
        assert(cwd.is_complete());
        chdir(cwd.string().c_str());
        APPTRC(L_INFO, "PE current working directory: " << cwd.string(), SPL_PE_DBG);
    }

    inputPorts_.reset(new PETransportIPortCollection(*this));
    outputPorts_.reset(new PETransportOPortCollection(*this));
    checkpointService_.reset(new PECheckpointService(*this));
    if (isDebugging_) {
        debugRegistery_.reset(new PECallbackRegistery());
    }

    // Abort if we are running a pre v4.2 app.
    if (PE_Version < 4200) {
        APPTRC(L_ERROR, "Pre-4.2 application support is deprecated", SPL_PE_DBG);
        abort();
    }

    { // Initialize PE metrics
        APPTRC(L_DEBUG, "Initializing PE performance metric collection...", SPL_PE_DBG);
        PEMetricsImpl* metric = new PEMetricsImpl(*this);
        peMetricHandle_ = metric;
        peMetric_.reset(metric);
        APPTRC(L_DEBUG, "Initialized PE performance metric collection", SPL_PE_DBG);
    }

    using namespace AADL;
    using namespace ADL;
    // Walk all the operators in the PE model and see if we need a JVM.
    if (PE_Version >= 4200) {
        // Starting in v4.2 the yield setting is in the ADL/AADL
        setYieldBehaviour(peModel_->avoidRuntimeYield());
        assert(NULL == javaVMArgs_.get());
        const operInstancesType::operInstance_sequence& ops =
          peModel_->operInstances().operInstance();
        for (operInstancesType::operInstance_const_iterator it = ops.begin(), itEnd = ops.end();
             it != itEnd; it++) {
            const OPModel& opmod = (*it);
            const PhysOPModel& physOPModel = static_cast<const PhysOPModel&>(opmod);
            if (physOPModel.jvmArguments().present()) {
                if (NULL == javaVMArgs_.get()) {
                    javaVMArgs_.reset(new vector<string>());
                }
                PADL::jvmArgumentsType args = physOPModel.jvmArguments().get();
                PADL::jvmArgumentsType::jvmArgument_const_iterator it1 = args.jvmArgument().begin();
                PADL::jvmArgumentsType::jvmArgument_const_iterator it1End =
                  args.jvmArgument().end();
                for (; it1 != it1End; ++it1) {
                    // Ensure we've not alread set this one
                    bool argFound = false;
                    for (vector<string>::const_iterator it2 = javaVMArgs_->begin(),
                                                        it2End = javaVMArgs_->end();
                         it2 != it2End; ++it2) {
                        if (*it2 == *it1) {
                            argFound = true;
                            break;
                        }
                    }
                    if (!argFound) {
                        javaVMArgs_->push_back(*it1);
                    }
                }
            }
        }
    }

    // create these before all other threads; other threads need to use the registry
    threadRegistry_.reset(new ThreadRegistry());
    int samplesPerSecond = 100;
    threadProfiler_.reset(new ThreadProfiler(samplesPerSecond));

    isInConsistentRegion_ = isInCCRegion(pemod);
    if (isInConsistentRegion_) {
        crService_.reset(new PEConsistentRegionService(*this));
    }
    numCROps_ = 0;
    numCROps_ = getNumCROperator(pemod);

    if (!isStandalone_ && NULL != javaVMArgs_.get()) {
        // In distributed mode the code above checked to see if we have java
        // arguments.  If so we need a JVM. In standalone a JVM will already
        // have been created.
        javaVMFactory_->setArguments(*javaVMArgs_.get());
        javaVMFactory_->getJVM(); // Start the JVM
        APPTRC(L_INFO, "Java SPL runtime has been initialized", SPL_PE_DBG);
    }

    // In both standalone and distributed we need to initialize the java runtime if there are
    // java operators
    if (javaVMFactory_->isJVMStarted()) {
        javaSPLRuntime_->setupJVM();
    } else {
        // We have not started a JVM, however there still might be a JVM started if there is a
        // non-java operator in a consistent region. Register the java spl runtime so that it gets
        // initialized if the JVM is started
        javaVMFactory_->addListener(javaSPLRuntime_.get());
    }

    autoCheckpointServices_.reset(new AutoCheckpointServices(*this));
    autoDebugServices_.reset(new AutoDebugServices(*this));
    autoCreateOperators_.reset(new AutoCreateOperators(*this));
    autoPePorts_.reset(new AutoPePorts(*this));
    autoMetricsIntervalTimer_.reset(new AutoMetricsIntervalTimer(*this));

    // Create temporary mapping of operator indicies
    {
        operInstancesType::operInstance_const_iterator it;
        const operInstancesType::operInstance_sequence& ops =
          peModel_->operInstances().operInstance();
        uint32_t i = 0;
        for (it = ops.begin(); it != ops.end(); it++) {
            modelOperIndexToOperIndex_[it->index()] = i++;
        }
    }

    { // create output port mappings
        augmentedPEType::outputPorts_type::outputPort_sequence::const_iterator it;
        const augmentedPEType::outputPorts_type::outputPort_sequence& ports =
          pemod.outputPorts().outputPort();
        exportedPorts_.reserve(ports.size());
        exportedPortsBlockOnCongestion_.reserve(ports.size());
        multiThreadedOPorts_.reserve(ports.size());
        uint32_t index = 0;
        for (it = ports.begin(); it != ports.end(); it++, index++) {
            const augmentedPEOutputPortType& port = *it;
            // Set value for non-exported ports and exported ports with default policy
            bool isCongestionPolicyWait = true;
            // Is it exported?
            exportedPorts_.push_back(port.exportedStream().present());
            // Has exported port the congestionPolicy != wait?
            if (port.exportedStream().present()) {
                if (port.exportedStream().get().congestionPolicy().present()) {
                    if (port.exportedStream().get().congestionPolicy().get().wait !=
                        port.exportedStream().get().congestionPolicy().get()) {
                        isCongestionPolicyWait = false;
                    }
                }
            }
            exportedPortsBlockOnCongestion_.push_back(isCongestionPolicyWait);

            // Mappings
            oportIdToPortIndex_[port.id()] = index;
            portIndexToOportId_[index] = port.id();
            const operInstancePEOutputConnectionType& conn = port.operInstanceConnection();
            uint32_t oidx = modelOperIndexToOperIndex_[conn.operInstanceIndex()];
            pair<uint32_t, uint32_t> operPort(oidx, conn.oportIndex());
            operPortToOportIndex_[operPort] = index;
            multiThreadedOPorts_.push_back(port.singleThreadedOnOutput() ? 0 : 1);
            APPTRC(L_DEBUG,
                   "Output port " << index << ": outPortId=" << portIndexToOportId_[index]
                                  << " exported=" << static_cast<uint8>(exportedPorts_[index])
                                  << " blocksOnCongestion="
                                  << static_cast<uint8>(exportedPortsBlockOnCongestion_[index])
                                  << " multiThreadedOnOutput="
                                  << static_cast<uint8>(multiThreadedOPorts_[index]),
                   SPL_PE_DBG);
        }
    }

    { // create input port mappings
        augmentedPEType::inputPorts_type::inputPort_sequence::const_iterator it;
        const augmentedPEType::inputPorts_type::inputPort_sequence& ports =
          pemod.inputPorts().inputPort();
        importedPorts_.reserve(ports.size());
        uint32_t index = 0;
        for (it = ports.begin(); it != ports.end(); it++, index++) {
            const augmentedPEInputPortType& port = *it;
            bool isImpPort = false;
            if (port.importedStreams().present()) {
                const importedStreamsType& is = port.importedStreams().get();
                if (is.nameBasedImport().size() > 0 || is.propertyBasedImport().present()) {
                    isImpPort = true;
                }
            }
            importedPorts_.push_back(isImpPort ? 1 : 0);
            // Mappings
            iportIdToPortIndex_[port.id()] = index;
            portIndexToIportId_[index] = port.id();
            const operInstancePEInputConnectionType& conn = port.operInstanceConnection();
            uint32_t oidx = modelOperIndexToOperIndex_[conn.operInstanceIndex()];
            pair<uint32_t, uint32_t> operPort(oidx, conn.iportIndex());
            operPortToIportIndex_[operPort] = index;
        }
    }

    if (visualizer_.get()) {
        visualizer_->create();
    }

    { // operator definitions
        operDefinitionsType::operDefinition_const_iterator it;
        const operDefinitionsType::operDefinition_sequence& operDefs =
          pemod.operDefinitions().operDefinition();
        for (it = operDefs.begin(); it != operDefs.end(); it++) {
            operDefinitionsMap_.insert(make_pair((uint32_t)it->index(), &*it));
        }
    }

    { // custom metrics
        if (pemod.customMetrics().present()) {
            customMetricsType::customMetric_const_iterator it;
            const customMetricsType::customMetric_sequence& metrics =
              pemod.customMetrics().get().customMetric();
            for (it = metrics.begin(); it != metrics.end(); it++) {
                customMetricMap_.insert(make_pair((uint32_t)it->index(), &*it));
            }
        }
    }

    if (isInConsistentRegion_ || isCheckpointing_) {
        threadPool_.reset(new FixedThreadPool(100 /* queue size */, 2 /* workers */));
    }

    if (isStandalone_ == false && (isCheckpointing_ == true || isInConsistentRegion_ == true)) {
        CheckpointContextImpl::initialize();
    }
}

PEImpl::~PEImpl()
{
    APPTRC(L_INFO, "cleaning up...", SPL_PE_DBG);
    if (visualizer_.get()) {
        visualizer_->shutdown();
    }
    ScheduledPortMap::iterator mit = scheduledPorts_.begin();
    for (; mit != scheduledPorts_.end(); mit++) {
        delete mit->second;
    }
}

const std::string& PEImpl::getApplicationScope() const
{
    return peModel_->applicationScope();
}

std::string PEImpl::getApplicationName() const
{
    return peModel_->applicationName();
}

std::string PEImpl::getJobGroup() const
{
    return peModel_->jobGroup();
}

std::string PEImpl::getJobName() const
{
    return peModel_->jobName();
}

std::string PEImpl::getToolkitDirectory(uint32_t toolkitIndex) const
{
    unordered_map<uint32_t, std::string>::const_iterator it = toolkitNameMap_.find(toolkitIndex);
    if (it != toolkitNameMap_.end()) {
        bf::path toolkitDir(outputDirectory_);
        toolkitDir /= "toolkits";
        toolkitDir /= it->second;
        return toolkitDir.string();
    }
    assert(false);
    return "";
}

std::string PEImpl::getToolkitDirectory() const
{
    OperatorImpl& op = *operators_[OperatorTracker::getCurrentOperator()];
    uint32_t toolkitIndex = op.getToolkitIndex();
    unordered_map<uint32_t, std::string>::const_iterator it = toolkitNameMap_.find(toolkitIndex);
    if (it != toolkitNameMap_.end()) {
        bf::path toolkitDir(outputDirectory_);
        toolkitDir /= "toolkits";
        toolkitDir /= it->second;
        return toolkitDir.string();
    }
    assert(false);
    return "";
}

std::string PEImpl::getToolkitDirectory(const std::string& toolkitName) const
{
    unordered_map<std::string, uint32_t>::const_iterator it = toolkitIndexMap_.find(toolkitName);
    if (it != toolkitIndexMap_.end()) {
        bf::path toolkitDir(outputDirectory_);
        toolkitDir /= "toolkits";
        toolkitDir /= toolkitName;
        return toolkitDir.string();
    }
    return "";
}

void PEImpl::handleOperatorFailure(const std::string& appMsg, const std::string& exceptionMsg)
{
    std::string opName = getOperatorName(OperatorTracker::getCurrentOperator());
    handleOperatorFailure(appMsg, exceptionMsg, splAppTrcAspect(SPL_OPER_DBG));
}

void PEImpl::handleOperatorFailure(const std::string& appMsg,
                                   const std::string& exceptionMsg,
                                   const std::string& aspect)
{
    std::string opName = getOperatorName(OperatorTracker::getCurrentOperator());
    setFailedOperator(opName);
    logTerminatingException(opName);
    APPTRC(L_DEBUG, appMsg << exceptionMsg, SPL_PE_DBG);
    SPLTRACEMSG(L_ERROR, SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_PROCESS(opName, exceptionMsg), aspect);
}

// Note: the OperatorImpl calls this upon construction. The operMutex_ is already held from the
// acquisition in createOperators().
void PEImpl::addOperator(OperatorImpl& op)
{
    std::string const& opName = op.getContextImpl().getName();
    operatorNames_.push_back(opName);
    operators_.push_back(&op);
    operatorMap_[opName] = &op;
}

void PEImpl::instantiateOperatorMetrics(OperatorImpl& op)
{
    OperatorMetrics& om = op.getContext().getMetrics();

    // First, find the operator definition
    uint32_t defIndex = op.getContextImpl().getGlobalDefinitionIndex();
    OperDefinitionMapType::const_iterator it = operDefinitionsMap_.find(defIndex);
    assert(it != operDefinitionsMap_.end());
    using namespace xmlns::prod::streams::application;
    const operDefinitionType& operDef = *it->second;

    // Walk the definition, looking for the metric indicies
    if (operDef.customMetricIndicies().present()) {
        const customMetricIndiciesType::customMetricIndex_sequence& indicies =
          operDef.customMetricIndicies().get().customMetricIndex();
        customMetricIndiciesType::customMetricIndex_const_iterator it2;
        for (it2 = indicies.begin(); it2 != indicies.end(); it2++) {
            // Find the custom metric number in the map
            CustomMetricMapType::const_iterator it3 = customMetricMap_.find(*it2);
            if (it3 != customMetricMap_.end()) {
                // Finally, do the creation.   Its about time!
                const customMetricType& cm = *it3->second;
                // must map from string to enum:  Assume same ordering
                om.createCustomMetric(
                  cm.name(), cm.description(),
                  static_cast<Metric::Kind>(static_cast<customMetricKindType::value>(cm.kind())));
            }
        }
    }
}

void PEImpl::createOperators()
{
    AutoMutex am(operMutex_);
    instantiateOperators();
    resolveThreadingModel();
    connectOperators();
    connectOperatorsToPE();
    overrideSigPipeHandlerIfOld();
    configureAutoRetryAfterReconnect();
    operatorsCreated_ = true;
    operCV_.broadcast();
}

void PEImpl::instantiateOperators()
{
    using namespace xmlns::prod::streams::application;
    operInstancesType::operInstance_const_iterator it;
    const operInstancesType::operInstance_sequence& ops = peModel_->operInstances().operInstance();

    uint32_t index = 0;
    for (it = ops.begin(); it != ops.end(); it++) {
        string const& name = it->name();
        string const& class_ = it->class_();

        const sharedObjectType* sharedObject = NULL;
        if (it->sharedObject().present()) {
            sharedObject = &it->sharedObject().get();
            string soName = sharedObject->sharedObjectUri();
            bf::path soPath(outputDirectory_);
            soPath /= soName;
            APPTRC(L_DEBUG, "Shared object is " + soPath.string(), SPL_PE_DBG);
            // The RTLD_GLOBAL flag was added to solve an issue on SUSE where comparing typeid
            // values failed when types same types are created in different shared objects. This is
            // fixed in a later version of the g++ compiler/runtime.  See rtc32566.
            void* dllHandle = dlopen(soPath.string().c_str(), RTLD_NOW | RTLD_GLOBAL);
            if (dllHandle == 0) {
                SPLTRACEMSGANDTHROW(SPLRuntime, L_ERROR,
                                    SPL_RUNTIME_CANNOT_LOAD_PE("", soPath.string(), dlerror()),
                                    SPL_OPER_DBG);
            }
        }
        OperatorImpl* o = NULL;
        try {
            OperatorTracker::setCurrentOperator(index++);
            APPTRC(L_DEBUG, "Calling getOperatorImpl()", SPL_PE_DBG);
            o = &Operator::instanceOf(class_)->getOperatorImpl();
            APPTRC(L_DEBUG, "Finish getOperatorImpl()", SPL_PE_DBG);
            OperatorTracker::resetCurrentOperator();
            APPTRC(L_DEBUG, "Finish resetCurrentOperator()", SPL_PE_DBG);
        } catch (DistilleryException const& e) {
            handleOperatorFailure("Exception received during instantiateOperator(): ",
                                  e.getExplanation(), SPL_OPER_DBG);
            throw;
        } catch (std::exception const& e) {
            handleOperatorFailure("Exception received during instantiateOperator(): ", e.what(),
                                  SPL_OPER_DBG);
            throw;
        } catch (...) {
            handleOperatorFailure("Exception received during instantiateOperator(): ",
                                  SPL_RUNTIME_UNKNOWN_EXCEPTION, SPL_OPER_DBG);
            throw;
        }
        APPTRC(L_DEBUG,
               "Operator " << o->getContextImpl().getName() << " created with local index "
                           << o->getIndex() << " (global index "
                           << o->getContextImpl().getGlobalIndex() << ')',
               SPL_PE_DBG);

        // If debugging, hook things up
        if (isDebugging_) {
            for (int i = 0, n = o->getNumberOfInputPorts(); i < n; i++) {
                debugRegistery_->addDebugHookLocation(name, i, INPUT, o->process_[i].get(),
                                                      o->process_[i].get());
            }
            for (int i = 0, n = o->getNumberOfOutputPorts(); i < n; i++) {
                debugRegistery_->addDebugHookLocation(name, i, OUTPUT, o->submit_[i].get(),
                                                      o->submit_[i].get());
            }
        }

        // Try to register StateHandler only after operator is constructed,
        // so that all runtime type information is available
        o->autoRegisterStateHandler();

        // Also register watermark handler if appropriate.
        o->initializeEventTimeContext();
    }
}

// We prefer this over boost::thread::hardware_concurrency() because this technique
// returns the number of logical processors we can actually be scheduled on. If our PE is
// restricted to use only a subset of the available logical processors, we want to know that
// when we cap the number of threads to use.
// Raised by defect 39641, we also need to check the cgroups restriction if running in container
// environment.
static size_t getNumberOfLogicalProcessors()
{
    // First we need to check cpu.cfs_quota_us, if it is not -1 then we are running in container
    // environment, otherwise the PE is running on bare metal or OS level VM and thus we check
    // the available logical processors using sched_getaffinity.
    int cpuno = SystemStatics::getContainerNCPUs();
    if (cpuno != -1) {
        return (size_t)cpuno;
    }
    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    int err = sched_getaffinity(0, sizeof(cpu_set_t), &cpus);
    if (err < 0) {
        std::string msg;
        switch (errno) {
            case EFAULT:
                msg = "EFAULT";
                break;
            case EINVAL:
                msg = "EINVAL";
                break;
            case EPERM:
                msg = "EPERM";
                break;
            case ESRCH:
                msg = "ESRCH";
                break;
            default:
                msg = "unknown";
        }
        APPTRC(L_ERROR, "Error trying to find number of logical CPUS; returned: " << msg,
               SPL_PE_DBG);
        return 1;
    }
    return CPU_COUNT(&cpus);
}

// Note that operMutex_ is held because of the acquisition in createOperators().
void PEImpl::resolveThreadingModel()
{
    // Prior to version 4.2, the threading model feature did not exist. Those PEs should use
    // MANUAL.
    if (PE_Version < 4200) {
        APPTRC(L_ERROR, "Pre-4.2 application support is deprecated", SPL_PE_DBG);
        abort();
    }

    uint32_t numScheduledPorts = 0;
    uint32_t requestedThreads = 1;
    bool explicitlyRequestedThreads = false;
    bool elastic = true;
    bool automatic = false;
    uint32_t numManual = 0;
    uint32_t numThreaded = 0;
    uint32_t numInputPorts = 0;
    uint32_t numRealThreadedPorts = 0;
    uint32_t numUDPThreadedPorts = 0;
    uint32_t numSourceOperators = 0;
    uint32_t maxNumInputPorts = 0;

    // Walk all operators
    using namespace xmlns::prod::streams::application;
    const operInstancesType::operInstance_sequence& ops = peModel_->operInstances().operInstance();
    operInstancesType::operInstance_const_iterator it;
    for (it = ops.begin(); it != ops.end(); it++) {
        OperatorImpl& op = *operators_[modelOperIndexToOperIndex_[it->index()]];
        OptionalContext* optContext = op.getContext().getOptionalContext(THREADING_ANNOTATION);
        ThreadingModelContext* threadingContext = NULL;
        if (optContext != NULL) {
            threadingContext = static_cast<ThreadingModelContext*>(optContext);
        }

        numInputPorts += op.getNumberOfInputPorts();
        if (op.getNumberOfInputPorts() > maxNumInputPorts) {
            maxNumInputPorts = op.getNumberOfInputPorts();
        }
        if (op.getNumberOfInputPorts() == 0) {
            ++numSourceOperators;
        }

        operInstanceInputPortsType::inputPort_const_iterator it2;
        const operInstanceInputPortsType::inputPort_sequence& iports = it->inputPorts().inputPort();
        for (it2 = iports.begin(); it2 != iports.end(); it2++) {
            if (it2->threadedPort().present()) {
                if (it2->threadedPort().get().isInjected()) {
                    if (threadingContext == NULL) {
                        ++numUDPThreadedPorts;
                    } else if (threadingContext->getKind() != DEDICATED) {
                        ++numUDPThreadedPorts;
                    } else {
                        ++numRealThreadedPorts;
                    }
                } else {
                    ++numRealThreadedPorts;
                }
            }
        }

        if (optContext != NULL) {
            std::string opName = "operator " + op.getContext().getName() + " threading model: ";
            switch (threadingContext->getKind()) {
                case MANUAL:
                    APPTRC(L_TRACE, opName << "manual", SPL_PE_DBG);
                    ++numManual;
                    break;
                case AUTOMATIC:
                    APPTRC(L_TRACE, opName << "automatic", SPL_PE_DBG);
                    automatic = true;
                    break;
                case DYNAMIC:
                    APPTRC(L_TRACE, opName << "dynamic", SPL_PE_DBG);
                    ++numScheduledPorts;
                    break;
                case DEDICATED:
                    APPTRC(L_TRACE, opName << "dedicated", SPL_PE_DBG);
                    ++numThreaded;
                    break;
                default:
                    APPTRC(L_ERROR,
                           "unknown threading model context kind: " << threadingContext->getKind(),
                           SPL_PE_DBG);
            }

            if (threadingContext->getKind() == DYNAMIC) {
                // we favor larger numbers of threads, and elasticity being off
                if (threadingContext->getThreads() > requestedThreads) {
                    requestedThreads = threadingContext->getThreads();
                    explicitlyRequestedThreads = true;
                }
                elastic = elastic && threadingContext->getElastic();
            }
        }
    }

    switch (globalThreadingModel_) {
        case MANUAL:
            APPTRC(L_INFO, "ThreadingModel default: manual", SPL_PE_DBG);
            break;
        case AUTOMATIC:
            APPTRC(L_INFO, "ThreadingModel default: automatic", SPL_PE_DBG);
            automatic = true;
            break;
        case DYNAMIC:
            APPTRC(L_INFO, "ThreadingModel default: dynamic", SPL_PE_DBG);
            break;
        case DEDICATED:
            APPTRC(L_INFO, "ThreadingModel default: dedicated", SPL_PE_DBG);
            break;
        default:
            APPTRC(L_ERROR, "Unknown default threading model kind: " << globalThreadingModel_,
                   SPL_PE_DBG);
    }

    uint32_t numLogicalProcessors = static_cast<uint32_t>(getNumberOfLogicalProcessors());
    uint32_t numPEInputPorts = peModel_->inputPorts().inputPort().size();
    uint32_t numNecessaryThreads = numPEInputPorts + numSourceOperators + numRealThreadedPorts;
    APPTRC(L_INFO,
           "ThreadingModel: " << ops.size() << " operators, " << numNecessaryThreads
                              << " pre-existing threads, " << numUDPThreadedPorts
                              << " injected threads, " << numLogicalProcessors << " logical cpus",
           SPL_PE_DBG);
    if (automatic && numManual < ops.size() && numThreaded < ops.size()) {
        std::string message = "ThreadingModel automatic chose ";
        if (numUDPThreadedPorts > numLogicalProcessors) {
            APPTRC(L_INFO, message + "dynamic", SPL_PE_DBG);
            globalThreadingModel_ = DYNAMIC;
        } else if (numNecessaryThreads >= ops.size() * 0.25 ||
                   numNecessaryThreads >= numLogicalProcessors * 0.5) {
            APPTRC(L_INFO, message + "manual", SPL_PE_DBG);
            globalThreadingModel_ = MANUAL;
        } else {
            APPTRC(L_INFO, message + "dynamic", SPL_PE_DBG);
            globalThreadingModel_ = DYNAMIC;
        }
    } else if (automatic) {
        // if we're here, then the PE is supposed to be under AUTOMATIC, but all operators
        // requested something else; switch the global to MANUAL
        APPTRC(L_INFO,
               "ThreadingModel automatic: all operators explicitly requested a threading model",
               SPL_PE_DBG);
        globalThreadingModel_ = MANUAL;
    }

    if (numScheduledPorts > 0 || globalThreadingModel_ == DYNAMIC) {
        if (numInputPorts > 0 && numManual < ops.size() && numThreaded < ops.size()) {
            uint32_t minThreads = std::min(maxNumInputPorts + 1, numLogicalProcessors);
            uint32_t maxThreads = std::max(maxNumInputPorts + 1, numLogicalProcessors);
            uint32_t activeThreads = 0;
            if (explicitlyRequestedThreads) {
                activeThreads = requestedThreads;
                if (activeThreads < minThreads) {
                    APPTRC(L_ERROR,
                           "using " << activeThreads
                                    << " as requested, but the reccommended minimum is "
                                    << minThreads << " to avoid deadlock",
                           SPL_PE_DBG);
                    minThreads = activeThreads;
                }
                if (activeThreads > maxThreads) {
                    APPTRC(L_ERROR,
                           "using " << activeThreads
                                    << " as requested, but the reccommended maximum is "
                                    << maxThreads << " to avoid overloading the systems",
                           SPL_PE_DBG);
                    maxThreads = activeThreads;
                }
            } else {
                activeThreads = minThreads;
            }

            APPTRC(L_INFO,
                   "Dynamic threading active with " << activeThreads << " active threads, min "
                                                    << minThreads << ", max " << maxThreads
                                                    << " and elasticity is: " << elastic,
                   SPL_PE_DBG);
            scheduledQueue_.reset(
              new ScheduledQueue(minThreads, maxThreads, activeThreads, elastic, numInputPorts));
        } else {
            APPTRC(L_INFO,
                   "Dynamic threading not active because the PE has no operators with input ports "
                   "that it would apply to",
                   SPL_PE_DBG);
        }
    }
}

// Note that operMutex_ is held because of the acquisition in createOperators().
PortSignal* PEImpl::outgoingConnectionToInputPort(
  const xmlns::prod::streams::application::operInstanceOutgoingConnectionType& conn)
{
    uint32_t operIndex = modelOperIndexToOperIndex_[conn.operInstanceIndex()];
    OperatorImpl& op = *operators_[operIndex];

    // Is there an active queue or scheduled port for the input?
    ActiveQueueMap::const_iterator at = activeQueues_.find(std::make_pair(&op, conn.iportIndex()));
    ScheduledPortMap::const_iterator st =
      scheduledPorts_.find(std::make_pair(&op, conn.iportIndex()));
    if (at != activeQueues_.end()) {
        return at->second;
    } else if (st != scheduledPorts_.end()) {
        return st->second;
    } else {
        return op.process_[conn.iportIndex()].get();
    }
}

void PEImpl::addThreadedPort(OperatorImpl& op,
                             uint32_t iPort,
                             const xmlns::prod::streams::application::threadedPortType& tpt)
{
    using namespace xmlns::prod::streams::application;

    ActiveQueue* aq;
    ActiveQueue::CongestionPolicy policy = ActiveQueue::Wait;
    congestionPolicyType::value adlPolicy = tpt.congestionPolicy();
    switch (adlPolicy) {
        case congestionPolicyType::wait:
            policy = ActiveQueue::Wait;
            break;
        case congestionPolicyType::dropFirst:
            policy = ActiveQueue::DropFirst;
            break;
        case congestionPolicyType::dropLast:
            policy = ActiveQueue::DropLast;
            break;
        default:
            assert(!"cannot happen");
    }
    APPTRC(L_DEBUG,
           "Threaded port present on operator " << op.getIndex() << ", input port " << iPort,
           SPL_PE_DBG);
    if (tpt.queueSize().present()) {
        aq = new ActiveQueue(op, iPort, policy, tpt.singleThreadedOnInput(), tpt.queueSize().get());
    } else {
        aq = new ActiveQueue(op, iPort, policy, tpt.singleThreadedOnInput());
    }

    APPTRC(L_DEBUG,
           "Adding threaded port on operator " << op.getIndex() << ", input port " << iPort,
           SPL_PE_DBG);
    activeQueues_[std::make_pair(&op, iPort)] = aq;
}

void PEImpl::addScheduledPort(OperatorImpl& op, uint32_t iPort)
{
    APPTRC(L_DEBUG,
           "Injecting dynamic threading port on operator " << op.getIndex() << ", input port "
                                                           << iPort,
           SPL_PE_DBG);
    scheduledPorts_[std::make_pair(&op, iPort)] =
      new ScheduledPort(op, iPort, *scheduledQueue_.get());
}

// Note that operMutex_ is held because of the acquisition in createOperators().
void PEImpl::connectOperators()
{
    using namespace xmlns::prod::streams::application;
    operInstancesType::operInstance_const_iterator it;
    const operInstancesType::operInstance_sequence& ops = peModel_->operInstances().operInstance();

    // Find all the input ports with active queues, and create the queues.
    for (it = ops.begin(); it != ops.end(); it++) {
        OperatorImpl& op = *operators_[modelOperIndexToOperIndex_[it->index()]];
        OptionalContext* optContext = op.getContext().getOptionalContext(THREADING_ANNOTATION);
        ThreadingModelContext* threadingContext = NULL;
        if (optContext != NULL) {
            threadingContext = static_cast<ThreadingModelContext*>(optContext);
        }

        // Now walk though the input ports
        operInstanceInputPortsType::inputPort_const_iterator it2;
        const operInstanceInputPortsType::inputPort_sequence& iports = it->inputPorts().inputPort();
        for (it2 = iports.begin(); it2 != iports.end(); it2++) {
            uint32_t iPort = it2->index();

            if (it2->threadedPort().present()) {
                const threadedPortType& tpt = it2->threadedPort().get();

                // If the threaded port was injected, then the user did not specify it, and we
                // inserted it during the transformation process, probably for parallelism. If
                // that's true, and we're under a dynamic threading model, then we should let that
                // mechanism provide parallelism.
                if (tpt.isInjected()) {
                    if (threadingContext != NULL) {
                        switch (threadingContext->getKind()) {
                            case DYNAMIC:
                                addScheduledPort(op, iPort);
                                break;
                            case MANUAL:
                            case DEDICATED:
                                addThreadedPort(op, iPort, tpt);
                                break;
                            case AUTOMATIC:
                                switch (globalThreadingModel_) {
                                    case DYNAMIC:
                                        addScheduledPort(op, iPort);
                                        break;
                                    case MANUAL:
                                        addThreadedPort(op, iPort, tpt);
                                        break;
                                    default:
                                        APPTRC(L_ERROR,
                                               "invalid global threading model kind during port "
                                               "creation: "
                                                 << globalThreadingModel_,
                                               SPL_PE_DBG);
                                }
                                break;
                            default:
                                APPTRC(L_ERROR,
                                       "invalid ThreadingModelContext during port creation: "
                                         << threadingContext->getKind(),
                                       SPL_PE_DBG);
                        }
                    } else if (globalThreadingModel_ == DYNAMIC) {
                        addScheduledPort(op, iPort);
                    } else {
                        addThreadedPort(op, iPort, tpt);
                    }
                } else {
                    addThreadedPort(op, iPort, tpt);
                }
            } else if (threadingContext != NULL && (threadingContext->getKind() == DYNAMIC ||
                                                    (threadingContext->getKind() == AUTOMATIC &&
                                                     globalThreadingModel_ == DYNAMIC))) {
                addScheduledPort(op, iPort);
            } else if (threadingContext == NULL && globalThreadingModel_ == DYNAMIC) {
                addScheduledPort(op, iPort);
            }
            // All of the remaining cases are variants of "manual" or "dedicated", in which case we
            // don't take any additional action. For "manual" we take no action by design, and for
            // "dedicated", we already injected the threaded ports at submission time.
        }
    }

    // create tuple type map (goes from type index to type)
    std::map<uint32_t, TupleTypeType const*> indexToTuple;
    const tupleTypesType::tupleType_sequence& tupleTypes = peModel_->tupleTypes().tupleType();
    tupleTypesType::tupleType_sequence::const_iterator tup;
    for (tup = tupleTypes.begin(); tup != tupleTypes.end(); ++tup) {
        indexToTuple[tup->index()] = &(*tup);
    }

    // Walk through all the operators, and connect output ports to input ones
    for (it = ops.begin(); it != ops.end(); it++) {
        OperatorImpl& op = *operators_[modelOperIndexToOperIndex_[it->index()]];
        op.connected_ = true;

        // Now walk though the output ports
        operInstanceOutputPortsType::outputPort_const_iterator it2;
        const operInstanceOutputPortsType::outputPort_sequence& oports =
          it->outputPorts().outputPort();

        for (it2 = oports.begin(); it2 != oports.end(); it2++) {
            const operInstanceOutputPortType& oport = *it2;

            const operInstanceOutputPortType::intraPeConnection_sequence& conns =
              oport.intraPeConnection();
            operInstanceOutputPortType::intraPeConnection_const_iterator it3;
            for (it3 = conns.begin(); it3 != conns.end(); it3++) {
                const operInstanceOutgoingConnectionType& conn = *it3;
                op.submit_[oport.index()]->connect(outgoingConnectionToInputPort(conn));
            }

            // create the splitters, if we have any
            const operInstanceOutputPortType::splitter_sequence& modelSplitters = oport.splitter();
            operInstanceOutputPortType::splitter_const_iterator sit;
            for (sit = modelSplitters.begin(); sit != modelSplitters.end(); ++sit) {
                const splitterType& modelSplitter = *sit;
                Splitter& physSplitter = createSplitter(
                  op.getIndex(), oport.index(), modelSplitter, indexToTuple, oport.tupleTypeIndex(),
                  op.getContextImpl().getChannel(), *(op.submit_[oport.index()]));
                op.submit_[oport.index()]->addSplitter(physSplitter);
            }
        }
    }
    javaSPLRuntime_->connectJavaOperators();
}

Splitter& PEImpl::createSplitter(uint32_t operIndex,
                                 uint32_t portIndex,
                                 const SplitterType& modelSplitter,
                                 std::map<uint32_t, TupleTypeType const*>& indexToTuple,
                                 uint32_t tupleTypeIndex,
                                 int32_t channel,
                                 SubmitSignal& subsig)
{
    // what kind of routing algorithm, Hash or RoundRobin?
    const SplitterType::parallelChannel_sequence& modelChannels = modelSplitter.parallelChannel();
    uint32_t channelSize = modelChannels.size();

    SplitterAlgorithm* alg = NULL;
    if (modelSplitter.kind() == "Hash") {
        const TupleTypeType::attribute_sequence& tupleAttributes =
          indexToTuple[tupleTypeIndex]->attribute();

        uint32_t index = 0;
        std::map<std::string, uint32_t> nameToIndex;
        TupleTypeType::attribute_const_iterator t;
        for (t = tupleAttributes.begin(); t != tupleAttributes.end(); ++t) {
            nameToIndex[t->name()] = index++;
        }

        std::vector<uint32_t> attributes;
        const PartitioningAttributesType::attribute_sequence& modelAttributes =
          modelSplitter.attributes().get().attribute();
        PartitioningAttributesType::attribute_const_iterator at;
        for (at = modelAttributes.begin(); at != modelAttributes.end(); ++at) {
            APPTRC(L_DEBUG,
                   "Hash using attribute name '" << at->name() << "' which is at location "
                                                 << nameToIndex[at->name()] << " in the tuple",
                   SPL_PE_DBG);
            attributes.push_back(nameToIndex[at->name()]);
        }

        APPTRC(L_DEBUG, "Hash splitter routing algorithm with " << channelSize << " channels",
               SPL_PE_DBG);
        alg = new AttributeHash(attributes, channelSize);
    } else if (modelSplitter.kind() == "RoundRobin") {
        APPTRC(L_DEBUG, "RoundRobin splitter routing algorithm with " << channelSize << " channels",
               SPL_PE_DBG);
        alg = new RoundRobin(channelSize, channel);
    } else if (modelSplitter.kind() == "Broadcast") {
        APPTRC(L_DEBUG, "Broadcast enabled with " << channelSize << " channels", SPL_PE_DBG);
    } else {
        THROW(SPLRuntime, "Unknown splitter kind: " << modelSplitter.kind());
    }

    bool dropWindowPuncts = false;
    if (modelSplitter.dropWindowPuncts().present()) {
        dropWindowPuncts = modelSplitter.dropWindowPuncts().get();
    }

    APPTRC(L_DEBUG,
           "Splitter " << modelSplitter.index() << " for operator " << operIndex << ", output port "
                       << portIndex << " dropWindowPuncts is set to: " << dropWindowPuncts,
           SPL_PE_DBG);

    Splitter* splitter =
      new Splitter(alg, dropWindowPuncts, splitterMap_[operIndex][portIndex].size());
    modelSplitterIndexToSplitterIndex_[operIndex][modelSplitter.index()] = splitter->index();
    splitterMap_[operIndex][portIndex][splitter->index()] = splitter;

    for (SplitterType::parallelChannel_const_iterator cit = modelChannels.begin();
         cit != modelChannels.end(); ++cit) {
        createParallelChannel(operIndex, portIndex, *splitter, *cit, indexToTuple, tupleTypeIndex,
                              subsig);
    }
    return *splitter;
}

void PEImpl::createParallelChannel(uint32_t operIndex,
                                   uint32_t portIndex,
                                   Splitter& splitter,
                                   const ParallelChannelType& modelParallelChannel,
                                   std::map<uint32_t, TupleTypeType const*>& indexToTuple,
                                   uint32_t tupleTypeIndex,
                                   SubmitSignal& subsig)
{
    ParallelChannel& pc = splitter.addParallelChannel();
    modelChannelIndexToChannelIndex_[operIndex][splitter.index()][modelParallelChannel.index()] =
      pc.index();

    // for channels to operators, we create their connections here; PE splitter connections
    // are established when we go over PE output ports
    const ParallelChannelType::intraPeConnection_sequence& toOps =
      modelParallelChannel.intraPeConnection();
    ParallelChannelType::intraPeConnection_const_iterator conn;
    std::vector<PortSignal*> channel;
    for (conn = toOps.begin(); conn != toOps.end(); ++conn) {
        APPTRC(L_DEBUG,
               "Connecting operator " << operIndex << ", port " << portIndex << " to splitter "
                                      << splitter.index() << " on channel " << pc.index(),
               SPL_PE_DBG);
        OperatorParallelConnection* parConn =
          new OperatorParallelConnection(subsig, outgoingConnectionToInputPort(*conn));
        pc.connect(parConn);
    }

    // If there are any nested splitters, walk them looking for any intra-PE connections
    const ParallelChannelType::splitter_sequence& nestedModelSplitters =
      modelParallelChannel.splitter();
    ParallelChannelType::splitter_const_iterator sit1, sit1End;
    for (sit1 = nestedModelSplitters.begin(), sit1End = nestedModelSplitters.end(); sit1 != sit1End;
         ++sit1) {
        const SplitterType& modelSplitter = *sit1;
        Splitter& nestedSplitter = createSplitter(operIndex, portIndex, modelSplitter, indexToTuple,
                                                  tupleTypeIndex, pc.index(), subsig);
        pc.addSplitter(nestedSplitter);
    }
}

void PEImpl::configureAutoRetryAfterReconnect()
{
    for (uint32_t i = 0; i < outputPortToOperPort_.size(); i++) {
        SPLAPPTRC(L_TRACE, "PE Output port: " << i, SPL_OPER_DBG);
        OperatorImpl& opImpl = outputPortToOperPort_[i]->getOperatorImpl();
        SPLAPPTRC(L_TRACE, "Operator instance name: " << opImpl.getContext().getName(),
                  SPL_OPER_DBG);
        ConsistentRegionContextImpl* crContext = opImpl.getConsistentRegionContext();
        if (crContext == NULL) {
            autoRetryAfterReconnect_.push_back(true);
        } else if (crContext->isEndOfRegion()) {
            autoRetryAfterReconnect_.push_back(true);
        } else {
            SPLAPPTRC(L_TRACE, "Must configure data sender", SPL_OPER_DBG);
            autoRetryAfterReconnect_.push_back(false);
        }
    }
}

// Note that operMutex_ is held because of the acquisition in createOperators().
void PEImpl::connectOperatorsToPE()
{
    using namespace xmlns::prod::streams::application;
    using namespace xmlns::prod::streams::application::augmented;

    { // output ports
        augmentedPEType::outputPorts_type::outputPort_sequence::const_iterator it;
        const augmentedPEType::outputPorts_type::outputPort_sequence& ports =
          peModel_->outputPorts().outputPort();
        oportBuffers_.resize(ports.size());
        facadeOPorts_.reserve(ports.size());
        outputPortToOperPort_.reserve(ports.size());
        uint32_t index = 0;
        for (it = ports.begin(); it != ports.end(); it++, index++) {
            const augmentedPEOutputPortType& port = *it;
            const operInstancePEOutputConnectionType& conn = port.operInstanceConnection();
            uint32_t oidx = modelOperIndexToOperIndex_[conn.operInstanceIndex()];
            uint32_t pidx = conn.oportIndex();

            SubmitSignal* ssig = operators_[oidx]->submit_[pidx].get();
            facadeOPorts_.push_back(ssig->isFacade());
            outputPortToOperPort_.push_back(ssig);

            PEOutputPortTupleDelegate tcall(this, index);
            PEOutputPortPunctDelegate pcall(this, index);
            if (conn.splitterIndex().present() && conn.channelIndex().present()) {
                uint32_t splitterIndex =
                  modelSplitterIndexToSplitterIndex_[oidx][conn.splitterIndex().get()];
                Splitter* splitter = splitterMap_[oidx][pidx][splitterIndex];
                assert(NULL != splitter);
                uint32_t channelIndex =
                  modelChannelIndexToChannelIndex_[oidx][splitterIndex][conn.channelIndex().get()];
                APPTRC(L_DEBUG,
                       "Connecting PE output port "
                         << index << " to operator " << oidx << " output port index " << pidx
                         << " splitter " << splitterIndex << " on channel " << channelIndex,
                       SPL_PE_DBG);
                PEOutputPortParallelConnection* parConn =
                  new PEOutputPortParallelConnection(PEOutputPortDelegates(tcall, pcall));
                splitter->connectParallelChannel(channelIndex, *parConn);
            } else {
                PEOutputPortNativeByteBufferDelegate btcall(this, index);
                outputPortToOperPort_[index]->connect(tcall, pcall, btcall);
            }
        }
    }

    { // input ports
        augmentedPEType::inputPorts_type::inputPort_sequence::const_iterator it;
        const augmentedPEType::inputPorts_type::inputPort_sequence& ports =
          peModel_->inputPorts().inputPort();
        iportTuples_.reserve(ports.size());
        facadeIPorts_.reserve(ports.size());
        inputPortToOperPort_.reserve(ports.size());
        uint32_t index = 0;
        for (it = ports.begin(); it != ports.end(); it++, index++) {
            const augmentedPEInputPortType& port = *it;
            const operInstancePEInputConnectionType& conn = port.operInstanceConnection();
            uint32_t oidx = modelOperIndexToOperIndex_[conn.operInstanceIndex()];
            uint32_t pidx = conn.iportIndex();
            OperatorImpl* op = operators_[oidx];
            ProcessSignal* processSig = op->process_[pidx].get();

            facadeIPorts_.push_back(processSig->isFacade());
            iportTuples_.push_back(processSig->createTuple().release());

            PortSignal* portSig = static_cast<PortSignal*>(processSig);
            ActiveQueueMap::const_iterator it4 = activeQueues_.find(std::make_pair(op, pidx));
            ScheduledPortMap::const_iterator it5 = scheduledPorts_.find(std::make_pair(op, pidx));
            if (it5 != scheduledPorts_.end()) {
                portSig = it5->second;
            } else if (it4 != activeQueues_.end()) {
                portSig = it4->second;
            }
            inputPortToOperPort_.push_back(portSig);
        }
    }
}

void PEImpl::createMetricsIntervalTimer()
{
    APPTRC(L_DEBUG, "Creating metrics interval timer thread", SPL_PE_DBG);
    metricsIntervalTimer_ = new MetricsIntervalTimer(*this);
    metricsIntervalTimer_->create();
}

void PEImpl::terminateMetricsIntervalTimer()
{
    if (metricsIntervalTimer_ != NULL) {
        metricsIntervalTimer_->shutdown();
        APPTRC(L_DEBUG, "Joining metrics interval timer thread", SPL_PE_DBG);
        metricsIntervalTimer_->join();
        APPTRC(L_DEBUG, "Joined metrics interval timer thread", SPL_PE_DBG);
        delete metricsIntervalTimer_;
    }
}

void PEImpl::processIntervalMetrics()
{
    APPTRC(L_DEBUG, "Processing interval based metrics", SPL_APP_DBG);
    typedef vector<OperatorImpl*>::const_iterator Iter;
    for (Iter oit = operators_.begin(); oit != operators_.end(); ++oit) {
        OperatorImpl const& oper = **oit;
        oper.getContextImpl().getMetricsImpl().processIntervalMetrics(oper);
    }
}

uint32_t PEImpl::getIntervalMetricsPeriod()
{
    return MetricsIntervalTimer::METRICS_INTERVAL_SECS;
};

void PEImpl::registerThread()
{
    /*
     * NOTE(xrg): the original PEC used to store the current PE ID to the current's thread local
     * data. That piece of information is not used anywhere, so I removed the call. We may want to
     * later revisit that function.
     */
}

void PEImpl::notifyScheduledQueuePortClosed()
{
    if (scheduledQueue_.get() != NULL) {
        scheduledQueue_->portClosed();
    }
}

OperatorImpl& PEImpl::getOperator(const std::string& opname)
{
    if (operatorMap_.count(opname) == 0) {
        THROW_STRING(SPLRuntimeInvalidOperator, SPL_RUNTIME_UKNOWN_OPERATOR(opname));
    }
    return *((operatorMap_.find(opname))->second);
}

OperatorImpl& PEImpl::getOperatorImpl(Operator& oper)
{
    return oper.getOperatorImpl();
}

// Note: this member function is part of the BasePEImpl interface, and needs
// to be protected from asynchronous access. In this case, the functions it calls
// acquire the necessary mutexes.
void PEImpl::process()
{
    APPTRC(L_INFO, "Start processing...", SPL_PE_DBG);

    OperatorTracker::init();
    try {
        autoCreateOperators_->initialize();
        restoreCheckpoint(); // load up operator state
        autoCheckpointServices_->initialize();
        autoDebugServices_->initialize();
        autoPePorts_->initialize();
        autoMetricsIntervalTimer_->initialize();

        startScheduledQueue();
        startProfilingThread();

        waitForConnections();
        notifyAllPortsReady();
        processInputs();

        joinOperatorThreads();
        joinWindowThreads();
        joinActiveQueues();
        joinScheduledQueue();

        if (visualizer_.get()) {
            visualizer_->shutdown();
        }

        if (!isStandalone_) {
            // distributed PEs stay up until explicitly shut down
            blockUntilShutdownRequest();
        }

        // stop periodic checkpointing
        autoCheckpointServices_->finalize();

        // if job is being cancelled, delete all the checkpoints of all operators in this PE
        if (isStandalone_ == false && isJobCancellation_ == true &&
            (isCheckpointing_ == true || isInConsistentRegion_ == true)) {
            APPTRC(L_INFO, "Job is being cancelled, delete all checkpoints of operators in this PE",
                   SPL_PE_DBG);
            deleteAllCheckpoints();
            isCkptDeletionDone_ = true;
        }

        // clean up resources as early as possible; this must be
        // before we shutdown the Java SPL runtime and destroy the JVM
        // so that Java operators can do clean operator shutdowns
        autoMetricsIntervalTimer_->finalize();
        autoPePorts_->finalize();
        autoDebugServices_->finalize();
        autoCreateOperators_->finalize();

        // make sure this is BEFORE the profiling thread, as it still may be
        // possible for Java code to enter profiling code
        javaSPLRuntime_->shutdown(true);
        if (javaVMFactory_->isJVMStarted()) {
            javaVMFactory_->destroyJVM();
        }

        // make sure profiling thread is joined after all threads that
        // are involved in profiling
        joinProfilingThread();
    } catch (DistilleryException const& e) {
        SPLTRACEMSG(L_ERROR, SPL_RUNTIME_EXCEPTION_PE_PROCESSING(e.getExplanation()), SPL_PE_DBG);
        shutdown(); // in case PEC agent does not call shutdown

        // stop periodic checkpointing
        autoCheckpointServices_->finalize_nothrow();

        // if job is being cancelled, delete all the checkpoints of all operators in this PE
        if (isStandalone_ == false && isJobCancellation_ == true && isCkptDeletionDone_ == false &&
            (isCheckpointing_ == true || isInConsistentRegion_ == true)) {
            APPTRC(L_INFO, "Job is being cancelled, delete all checkpoints of operators in this PE",
                   SPL_PE_DBG);
            deleteAllCheckpoints();
        }

        // In case PEC agent does not call our destructor
        autoMetricsIntervalTimer_->finalize_nothrow();
        autoPePorts_->finalize_nothrow();
        autoDebugServices_->finalize_nothrow();
        autoCreateOperators_->finalize_nothrow();

        // joining with the profiling thread can cause problems in this exception handler, but
        // we can stop it for a cleaner shutdown
        stopProfilingThread();

        if (failedOperators_.empty()) {
            throw;
        }
        // otherwise we will throw below
    }

    // Did any operator have an error on a different thread
    if (!failedOperators_.empty()) {
        stringstream s;
        s.imbue(locale::classic());
        for (uint32_t i = 0; i < failedOperators_.size(); i++) {
            if (i > 0) {
                s << ", ";
            }
            s << failedOperators_[i];
        }
        APPTRC(L_INFO, "Following operators failed at runtime: '" << s.str() << "'.  Throwing...",
               SPL_PE_DBG);
        SPLTRACEMSG(L_ERROR, SPL_RUNTIME_OPERATOR_FAILURE(s.str()), SPL_OPER_DBG);
        THROW_STRING(SPLRuntimeFailedOperators, SPL_RUNTIME_OPERATOR_FAILURE(s.str()));
    }
    APPTRC(L_INFO, "Stopped processing", SPL_PE_DBG);
}

void PEImpl::openPorts()
{
    if (isStandalone_) {
        return;
    }
    inputPorts_->open();
    outputPorts_->open(autoRetryAfterReconnect_);
}

void PEImpl::closePorts()
{
    if (isStandalone_) {
        return;
    }
    inputPorts_->close();
    outputPorts_->close();
}

void PEImpl::waitForConnections()
{
    if (isStandalone_) {
        return;
    }
    size_t numInputs = inputPorts_->size();
    if (numInputs == 0) {
        APPTRC(L_DEBUG, "PE has no input ports to wait on", SPL_PE_DBG);
    } else {
        if (shutdownRequested_) {
            return;
        }
        inputPorts_->wait();
    }
}

void PEImpl::processInputs()
{
    if (isStandalone_) {
        return;
    }
    size_t numInputs = inputPorts_->size();
    if (numInputs == 0) {
        APPTRC(L_DEBUG, "PE has no input ports to run transport on", SPL_PE_DBG);
    } else {
        APPTRC(L_DEBUG, "Running the transport server on input ports...", SPL_PE_DBG);
        inputPorts_->run();
        APPTRC(L_DEBUG, "Completed running the transport server on input ports", SPL_PE_DBG);
    }
}

void PEImpl::preparePortsForTermination()
{
    if (isStandalone_) {
        return;
    }
    APPTRC(L_DEBUG, "Shutting down PE ports...", SPL_PE_DBG);
    inputPorts_->shutdown();
    outputPorts_->shutdown();
    APPTRC(L_DEBUG, "Completed shutting down PE ports", SPL_PE_DBG);
}

void PEImpl::runCheckpointService()
{
    if (isStandalone_) {
        return;
    }
    if (isCheckpointing_) {
        APPTRC(L_DEBUG, "Starting checkpointing thread", SPL_PE_DBG);
        checkpointService_->start();
    } else {
        APPTRC(L_DEBUG, "Not starting checkpointing thread", SPL_PE_DBG);
    }
}

void PEImpl::terminateCheckpointService()
{
    if (isStandalone_) {
        return;
    }
    int dummy;
    if (isCheckpointing_) {
        APPTRC(L_DEBUG, "Joining checkpointing thread", SPL_PE_DBG);
        checkpointService_->join(&dummy);
        APPTRC(L_DEBUG, "Joined checkpointing thread", SPL_PE_DBG);
    }
}

void PEImpl::restoreCheckpoint()
{
    if (isStandalone_) {
        return;
    }
    try {
        if (isCheckpointing_ && relaunchCount_ > 0) {
            checkpointService_->restorePEState();
        }
    } catch (SPLRuntimeInvalidCheckpointException const& e) {
        SPLTRACEMSG(L_ERROR, SPL_RUNTIME_EXCEPTION_RESTORE_CHECKPOINT(e.getExplanation()),
                    SPL_PE_DBG);
    }
}

void PEImpl::performSynchronousCheckpoint(OperatorImpl& oper, NetworkByteBuffer const& state)
{
    if (isStandalone_) {
        return;
    }
    if (oper.getContextImpl().isCheckpointingOn()) {
        checkpointService_->synchronousCheckpoint(oper, state);
    }
}

void PEImpl::notifyAllPortsReady()
{
    APPTRC(L_DEBUG, "Notifying operators of readiness of all ports...", SPL_PE_DBG);
    allPortsReady_ = true;
    vector<OperatorImpl*>::iterator oit;
    for (oit = operators_.begin(); oit != operators_.end(); ++oit) {
        (*oit)->allPortsReadyRaw();
    }
    APPTRC(L_INFO, "Notified operators of readiness of all ports", SPL_PE_DBG);
}

void PEImpl::prepareOperatorsForTermination()
{
    APPTRC(L_DEBUG, "Notifying operators of termination...", SPL_PE_DBG);
    vector<OperatorImpl*>::iterator oit;
    for (oit = operators_.begin(); oit != operators_.end(); ++oit) {
        (*oit)->prepareToShutdownRaw();
    }
    APPTRC(L_INFO, "Notified all operators of termination", SPL_PE_DBG);
}

void PEImpl::joinOperatorThreads()
{
    APPTRC(L_INFO, "Joining operator threads...", SPL_PE_DBG);
    vector<OperatorImpl*>::iterator oit;
    for (oit = operators_.begin(); oit != operators_.end(); ++oit) {
        (*oit)->joinOperatorThreads();
    }
    APPTRC(L_INFO, "Joined all operator threads", SPL_PE_DBG);
}

void PEImpl::joinActiveQueues()
{
    APPTRC(L_DEBUG, "Joining active queues...", SPL_PE_DBG);
    vector<OperatorImpl*>::iterator oit;
    for (oit = operators_.begin(); oit != operators_.end(); ++oit) {
        (*oit)->joinActiveQueueThreads();
    }
    APPTRC(L_DEBUG, "Joined active queues", SPL_PE_DBG);
}

void PEImpl::joinWindowThreads()
{
    APPTRC(L_DEBUG, "Joining window threads...", SPL_PE_DBG);
    vector<OperatorImpl*>::iterator oit;
    for (oit = operators_.begin(); oit != operators_.end(); ++oit) {
        (*oit)->joinWindowThreads();
    }
    APPTRC(L_DEBUG, "Joined all window threads", SPL_PE_DBG);
}

void PEImpl::startScheduledQueue()
{
    if (scheduledQueue_.get() != NULL) {
        scheduledQueue_->start();
    }
}

void PEImpl::joinScheduledQueue()
{
    if (scheduledQueue_.get() != NULL) {
        scheduledQueue_->joinThreads();
    }
}

void PEImpl::startProfilingThread()
{
    APPTRC(L_INFO, "Starting profiling thread", SPL_PE_DBG);
    ThreadProfiler& tp = getThreadProfiler();
    tp.create(NULL, false, NULL);
}

void PEImpl::stopProfilingThread()
{
    APPTRC(L_DEBUG, "Stopping profiling thread...", SPL_PE_DBG);
    ThreadProfiler& tp = getThreadProfiler();
    tp.shutdown();
    APPTRC(L_INFO, "Stopped profiling thread", SPL_PE_DBG);
}

void PEImpl::joinProfilingThread()
{
    APPTRC(L_DEBUG, "Stopping and joining profiling thread...", SPL_PE_DBG);
    ThreadProfiler& tp = getThreadProfiler();
    tp.shutdown();
    tp.join();
    APPTRC(L_INFO, "Joined profiling thread", SPL_PE_DBG);
}

void PEImpl::deleteOperators()
{
    APPTRC(L_DEBUG, "Flushing operator profile metrics...", SPL_PE_DBG);
    vector<OperatorImpl*>::iterator oit;
    for (oit = operators_.begin(); oit != operators_.end(); ++oit) {
        (*oit)->getContextImpl().getMetricsImpl().flush();
    }
    APPTRC(L_DEBUG, "Flushed all operator profile metrics", SPL_PE_DBG);

    APPTRC(L_DEBUG, "Deleting active queues...", SPL_PE_DBG);
    ActiveQueueMap::const_iterator ait;
    for (ait = activeQueues_.begin(); ait != activeQueues_.end(); ait++) {
        delete ait->second;
    }
    APPTRC(L_DEBUG, "Deleted active queues", SPL_PE_DBG);

    APPTRC(L_DEBUG, "Deleting input port tuple cache...", SPL_PE_DBG);
    vector<Tuple*>::const_iterator tit;
    for (tit = iportTuples_.begin(); tit != iportTuples_.end(); tit++) {
        delete *tit;
    }
    APPTRC(L_DEBUG, "Deleted input port tuple cache", SPL_PE_DBG);

    { // make sure no getMetrics call is active
        AutoMutex am(operMutex_);
        operatorsCreated_ = false;
        APPTRC(L_DEBUG, "Deleting all operators...", SPL_PE_DBG);
        vector<OperatorImpl*>::iterator oit2;
        for (oit2 = operators_.begin(); oit2 != operators_.end(); ++oit2) {
            Operator* op = &(*oit2)->getOperator();
            OperatorTracker::setCurrentOperator(op->getIndex());
            delete op;
            OperatorTracker::resetCurrentOperator();
        }
        APPTRC(L_DEBUG, "Deleted all operators", SPL_PE_DBG);

        // wakeup view notification threads; operatorsCreated_ will be false, but
        // shutdownReqested_ should be true
        operCV_.broadcast();
    }
}

void PEImpl::runDebugServices()
{
    if (!isInDebugMode()) {
        return;
    }
    APPTRC(L_INFO, "Starting debug services...", SPL_PE_DBG);
    debugService_.reset(new ProbePointServices(*this));
    debugService_->create();
    APPTRC(L_INFO, "Started debug services", SPL_PE_DBG);
    APPTRC(L_INFO, "Notifying debug services of processing startup...", SPL_PE_DBG);
    debugService_->prepareToProcess(); // this will block
    APPTRC(L_INFO, "Notified debug services of processing startup", SPL_PE_DBG);
}

void PEImpl::terminateDebugServices()
{
    if (debugService_.get() != NULL) {
        APPTRC(L_INFO, "Joining debug services thread...", SPL_PE_DBG);
        int dummy;
        debugService_->join(&dummy);
        APPTRC(L_INFO, "Joined debug services thread", SPL_PE_DBG);
    }
}

void PEImpl::prepareDebugServicesForTermination()
{
    if (debugService_.get() != NULL) {
        APPTRC(L_INFO, "Notifying debug services of termination...", SPL_PE_DBG);
        debugService_->prepareToShutdown();
        APPTRC(L_INFO, "Notified debug services of termination", SPL_PE_DBG);
    }
}

// cannot be called from within an operator thread
void PEImpl::shutdown(bool isJobShuttingDown)
{
    APPTRC(L_INFO, "Shutdown request received by PE...", SPL_PE_DBG);
    {
        AutoMutex am(shutdownMutex_);
        if (shutdownRequested_) {
            return;
        }

        // record if job is being cancelled; process() function will check this and
        // delete all the checkpoints associated with all operators in this PE
        isJobCancellation_ = isJobShuttingDown;
        APPTRC(L_INFO, "isJobCancellation is set to " << isJobCancellation_, SPL_PE_DBG);
        shutdownRequested_ = true;
        APPTRC(L_INFO, "shutdownRequested set to true", SPL_PE_DBG);
        shutdownCV_.broadcast();
    }

    if (isInConsistentRegion_) {
        crService_.get()->shutdown();
    }
    autoDebugServices_->prepareToFinalize();
    autoPePorts_->prepareToFinalize();
    autoCreateOperators_->prepareToFinalize();
    if (threadPool_.get() != NULL) {
        threadPool_->shutdown();
    }
    if (scheduledQueue_.get() != NULL) {
        scheduledQueue_->shutdown();
    }
    APPTRC(L_INFO, "Exit PEImpl::shutdown()", SPL_PE_DBG);
}

class ShutdownThread : public SPL::Thread
{
  public:
    struct Sync
    {
        Sync()
          : done_(false)
        {}
        void wait()
        {
            AutoMutex am(mutex_);
            while (!done_) {
                cv_.wait(mutex_);
            }
        }
        void signal()
        {
            AutoMutex am(mutex_);
            done_ = true;
            cv_.signal();
        }

      private:
        Distillery::CV cv_;
        Distillery::Mutex mutex_;
        bool done_;
    };
    ShutdownThread(PEImpl& pe)
      : pe_(pe)
    {}
    virtual void* run(void* arg)
    {
        registerThread("Shutdown");
        pair<Sync*, Sync*>& sync = *static_cast<pair<Sync*, Sync*>*>(arg);
        Sync& syncTo = *sync.first;
        Sync& syncFrom = *sync.second;
        syncTo.wait();
        syncFrom.signal();
        pe_.shutdown();
        delete this; // Kansas is going bye-bye
        return NULL;
    }

  private:
    PEImpl& pe_;
};

// can be called from within an operator thread
void PEImpl::shutdownFromWithinOperators()
{
    typedef ShutdownThread::Sync Sync;
    Sync syncTo, syncFrom;
    pair<Sync*, Sync*> sync(&syncTo, &syncFrom);
    ShutdownThread* t = new ShutdownThread(*this);
    t->create(NULL, true, &sync);
    syncTo.signal();
    syncFrom.wait();
}

void PEImpl::blockUntilShutdownRequest() const
{
    AutoMutex am(shutdownMutex_);
    while (!shutdownRequested_) {
        shutdownCV_.wait(shutdownMutex_);
    }
}

bool PEImpl::blockUntilShutdownRequest(const timespec& relativeTimeout) const
{
    AutoMutex am(shutdownMutex_);
    if (!shutdownRequested_) {
        return !shutdownCV_.waitFor(shutdownMutex_, relativeTimeout);
    }
    return true;
}

bool PEImpl::blockUntilShutdownRequest(const double timeInSeconds) const
{
    struct timespec req;
    req.tv_sec = static_cast<time_t>(static_cast<int>(timeInSeconds));
    req.tv_nsec =
      static_cast<long>((timeInSeconds - static_cast<double>(req.tv_sec)) * 1000 * 1000 * 1000);
    return blockUntilShutdownRequest(req);
}

// outlined code to reduce register usage in hot routine, do not put back!
void PEImpl::dropTuple(uint32_t port, bool error)
{
    if (error) {
        THROW_STRING(SPLRuntime,
                     SPL_RUNTIME_PE_OUTPUT_PORT_NOT_ACTIVE_DROPPING_TUPLE_OR_PUNCT(port));
    }
    SPLTRACEMSG(L_ERROR, SPL_RUNTIME_PE_OUTPUT_PORT_NOT_ACTIVE_DROPPING_TUPLE_OR_PUNCT(port),
                SPL_PE_DBG);
}

// Note: this member function is part of the BasePEImpl interface, and needs
// to be protected from asynchronous access.
void PEImpl::routingInfoNotificationHandler(const RoutingNotificationType ntype,
                                            const PortID& oportid,
                                            const string& nslabel,
                                            const ID& remotePortIndex,
                                            const string& filterExpression)
{
    AutoJVMDetach autoDetach(*this);

    if (!outputPorts_->isOpen()) {
        APPTRC(L_DEBUG, "Waiting for output ports to be opened...", SPL_PE_DBG);
        outputPorts_->blockUntilOpenOrShutdown();
        if (shutdownRequested_) {
            return;
        }
        APPTRC(L_DEBUG, "Output ports are open", SPL_PE_DBG);
    }
    unsigned opidx = getOutputPortIndex(oportid);
    PETransportOPort* top = (*outputPorts_)[opidx];
    DataSender& sender = top->getDataSender();
    DynDataSender* dcc = dynamic_cast<DynDataSender*>(&sender);
    if (dcc == NULL) {
        THROW(SPLRuntime, "Stream from output port index " << opidx << " is not exported");
    }
    DataSender::ConnectionId id = 0;
    bool succeeded = true;
    switch (ntype) {
        case Streams::ADD:
            APPTRC(L_DEBUG,
                   "Stream from output port index "
                     << opidx << " has a new consumer whose TCP endpoint is '" << nslabel
                     << "' -- adding connection",
                   SPL_PE_DBG);
            try {
                Distillery::DataSender::ConnectionCallback* c =
                  new PEOPortConnectionCallback(*this, opidx, oportid, nslabel);
                boost::shared_ptr<Distillery::DataSender::ConnectionCallback> callback(c);
                top->newConnectionStarting();
                Distillery::DataSender::PortDescription pd(
                  nslabel, callback, remotePortIndex, isExportedPortWithBlockOnCongestion(opidx));
                id = dcc->addConnection(pd);
                peMetric_->addOutputPortConnection(opidx);
            } catch (ConnectErrorException const& e) {
                APPTRC(L_DEBUG,
                       "Stream from output port index "
                         << opidx << " has a new consumer whose TCP endpoint is '" << nslabel
                         << "' -- connection cannot be added, reason" << e.getExplanation(),
                       SPL_PE_DBG);
                succeeded = false;
            }
            APPTRC(L_DEBUG,
                   "Stream from output port index "
                     << opidx << " has a new consumer whose TCP endpoint is '" << nslabel
                     << "' with id '" << id << "' -- added connection",
                   SPL_PE_DBG);
            if (succeeded) {
                if (filterExpression.empty()) {
                    // Remember even the ones with no filters, in case we get one later
                    top->addConnectionWithFilter(NULL, id, nslabel, filterExpression);
                } else {
                    std::auto_ptr<Tuple> tp = outputPortToOperPort_[opidx]->createTuple();
                    assert(tp.get());
                    top->addConnectionWithFilter(tp.get(), id, nslabel, filterExpression);
                }
            }
            break;
        case Streams::DEL:
            APPTRC(L_DEBUG,
                   "Stream from output port index "
                     << opidx << " has lost a consumer whose TCP endpoint is '" << nslabel
                     << "' -- removing connection",
                   SPL_PE_DBG);
            dcc->removeConnection(nslabel);
            peMetric_->deleteOutputPortConnection(opidx);
            APPTRC(L_DEBUG,
                   "Stream from output port index "
                     << opidx << " has lost a consumer whose TCP endpoint is '" << nslabel
                     << "' -- removed connection",
                   SPL_PE_DBG);
            top->removeConnection(nslabel);
            break;
        case Streams::UPDATE:
            if (filterExpression.empty()) {
                APPTRC(L_DEBUG,
                       "Stream from output port index "
                         << opidx << " has updated a consumer whose TCP endpoint is '" << nslabel
                         << " -- removing filter",
                       SPL_PE_DBG);
                // We don't need a tuple
                top->updateFilter(NULL, nslabel, filterExpression);
            } else {
                APPTRC(L_DEBUG,
                       "Stream from output port index "
                         << opidx << " has updated a consumer whose TCP endpoint is '" << nslabel
                         << " -- updating filter to " << filterExpression,
                       SPL_PE_DBG);
                std::auto_ptr<Tuple> tp = outputPortToOperPort_[opidx]->createTuple();
                assert(tp.get());
                top->updateFilter(tp.get(), nslabel, filterExpression);
            }
            break;
    }
}

// Note: this member function is part of the BasePEImpl interface, and needs
// to be protected from asynchronous access.
void PEImpl::getConnectionStates(vector<ConnectionState>& states) const
{
    AutoJVMDetach autoDetach(*this);
    if (!outputPorts_->isOpen()) {
        APPTRC(L_DEBUG, "Waiting for output ports to be opened...", SPL_PE_DBG);
        outputPorts_->blockUntilOpenOrShutdown();
        if (shutdownRequested_) {
            return;
        }
        APPTRC(L_DEBUG, "Output ports are open", SPL_PE_DBG);
    }
    outputPorts_->getConnectionStates(states);
}

// Note: this member function is part of the BasePEImpl interface, and needs
// to be protected from asynchronous access.
void PEImpl::getStaticInputConnectionLabels(vector<string>& labels) const
{
    AutoJVMDetach autoDetach(*this);
    /*
     * Grab the input connection labels.
     */
    if (!inputPorts_->isOpen()) {
        APPTRC(L_DEBUG, "Waiting for input ports to be opened...", SPL_PE_DBG);
        inputPorts_->blockUntilOpenOrShutdown();
        if (shutdownRequested_) {
            return;
        }
        APPTRC(L_DEBUG, "Input ports are open", SPL_PE_DBG);
    }
    inputPorts_->getStaticConnectionLabels(labels);
}

// Note: this member function is part of the BasePEImpl interface, and needs
// to be protected from asynchronous access.
void PEImpl::getStaticOutputConnectionLabels(vector<string>& labels) const
{
    AutoJVMDetach autoDetach(*this);
    /*
     * Grab the output connection labels.
     */
    if (!outputPorts_->isOpen()) {
        APPTRC(L_DEBUG, "Waiting for output ports to be opened...", SPL_PE_DBG);
        outputPorts_->blockUntilOpenOrShutdown();
        if (shutdownRequested_) {
            return;
        }
        APPTRC(L_DEBUG, "Output ports are open", SPL_PE_DBG);
    }
    outputPorts_->getStaticConnectionLabels(labels);
}

static string dumpProps(const vector<nameValuePair_t>& properties)
{
    string result;
    for (vector<nameValuePair_t>::const_iterator it = properties.begin(); it != properties.end();
         it++) {
        const nameValuePair_t& nvp = *it;
        result += nvp.first;
        result += ": ";
        for (vector<string>::const_iterator it2 = nvp.second.begin(); it2 != nvp.second.end();
             it2++) {
            if (it2 != nvp.second.begin()) {
                result += ", ";
            }
            result += *it2;
        }
        result += "; ";
    }
    return result;
}

// Note: this member function is part of the BasePEImpl interface, and needs
// to be protected from asynchronous access.
void PEImpl::streamViewNotificationHandler(const RoutingNotificationType ntype,
                                           const ID& operatorIndex,
                                           const ID& operatorOutputPortIndex,
                                           const string& nslabel,
                                           const ID& viewID,
                                           const vector<nameValuePair_t>& properties)
{
    AutoJVMDetach autoDetach(*this);

    if (operatorIndex > numeric_limits<uint32>::max()) {
        APPTRC(L_DEBUG,
               "Cannot process view notification - illegal operatorIndex value " << operatorIndex,
               SPL_PE_DBG);
        return;
    }
    if (operatorOutputPortIndex > numeric_limits<uint32>::max()) {
        APPTRC(L_DEBUG,
               "Cannot process view notification - illegal operatorOutputPortIndex value "
                 << operatorOutputPortIndex,
               SPL_PE_DBG);
        return;
    }

    AutoMutex am(operMutex_);
    if (!operatorsCreated_) {
        while (!shutdownRequested_ && !operatorsCreated_) {
            operCV_.wait(operMutex_);
        }
        if (shutdownRequested_) {
            return;
        }
    }

    // Remap to internal operator number
    uint32_t opIndex = modelOperIndexToOperIndex_[operatorIndex];

    // Dispatch notification type
    if (ntype == Streams::ADD) {
        addView(viewID, opIndex, operatorOutputPortIndex, properties, nslabel);
    } else if (ntype == Streams::DEL) {
        deleteView(viewID, opIndex, operatorOutputPortIndex);
    } else {
        assert(ntype == UPDATE);
        updateView(viewID, opIndex, properties);
    }
}

void PEImpl::addView(const ID& viewID,
                     uint32_t opIndex,
                     const ID& operatorOutputPortIndex,
                     const vector<nameValuePair_t>& properties,
                     const string& nslabel)
{
    ViewPropertiesMapKey viewKey(viewID, opIndex);
    OperatorImpl& op = *operators_[opIndex];
    APPTRC(L_DEBUG,
           "Add view " << viewKey << " operator name " << op.getOperator().getContext().getName()
                       << " of output port " << operatorOutputPortIndex << " to view server '"
                       << nslabel << "' with properties " << dumpProps(properties),
           SPL_PE_DBG);
    ViewPropertiesMap::iterator it = viewProperties_.find(viewKey);
    if (it == viewProperties_.end()) {
        // First one?
        if (viewProperties_.empty()) {
            visualizer_->openPort(nslabel);
        }
        ViewProperties* v = new ViewProperties(viewID, op, operatorOutputPortIndex, properties);
        viewProperties_.insert(make_pair(viewKey, v));
        op.addView(*v);
        javaSPLRuntime_->addView(opIndex, operatorOutputPortIndex, v);
        visualizerUpdate(true);
    } else {
        APPTRC(L_DEBUG, "View " << viewKey << " already in view map", SPL_PE_DBG);
    }
}

void PEImpl::deleteView(const ID& viewID, uint32_t opIndex, const ID& operatorOutputPortIndex)
{
    ViewPropertiesMapKey viewKey(viewID, opIndex);
    APPTRC(L_DEBUG, "Delete view " << viewKey, SPL_PE_DBG);
    ViewPropertiesMap::iterator it = viewProperties_.find(viewKey);
    if (it != viewProperties_.end()) {
        ViewProperties& v = *it->second;
        v.getOperator().deleteView(v);
        javaSPLRuntime_->deleteView(opIndex, operatorOutputPortIndex, &v);
        // remove the properties
        delete it->second;
        viewProperties_.erase(it);

        // Last one?
        if (viewProperties_.empty()) {
            visualizer_->closePort();
            visualizerUpdate(false);
        }
    } else {
        APPTRC(L_DEBUG, "View " << viewKey << " not found in view map", SPL_PE_DBG);
    }
}

void PEImpl::updateView(const ID& viewID,
                        uint32_t opIndex,
                        const vector<nameValuePair_t>& properties)
{
    ViewPropertiesMapKey viewKey(viewID, opIndex);
    APPTRC(L_DEBUG, "Update view " << viewKey << " with properties " << dumpProps(properties),
           SPL_PE_DBG);
    ViewPropertiesMap::iterator it = viewProperties_.find(viewKey);
    if (it != viewProperties_.end()) {
        it->second->update(properties);
    } else {
        APPTRC(L_DEBUG, "View " << viewKey << " not found in view map", SPL_PE_DBG);
    }
}

// Debugging interfaces

uint32_t PEImpl::registerProbePoint(DebugHookPtr cbHandle,
                                    const string& opName,
                                    PortType pType,
                                    uint32_t pIdx,
                                    bool callFirst)
{
    return debugRegistery_->registerProbePoint(cbHandle, opName, pType, pIdx, callFirst);
}

void PEImpl::unregisterProbePoint(uint32_t hId)
{
    debugRegistery_->unregisterProbePoint(hId);
}

uint32_t PEImpl::registerInjectPoint(const string& opName, PortType pType, uint32_t pIdx)
{
    return debugRegistery_->registerInjectPoint(opName, pType, pIdx);
}

void PEImpl::unregisterInjectPoint(uint32_t hId)
{
    debugRegistery_->unregisterInjectPoint(hId);
}

Tuple& PEImpl::retrieveTuple(uint32_t hId)
{
    return debugRegistery_->retrieveTuple(hId);
}

void PEImpl::submitTuple(uint32_t hId)
{
    debugRegistery_->submitTuple(hId);
}

void PEImpl::submitPunctuation(uint32_t hId, Punctuation const& punct)
{
    debugRegistery_->submitPunctuation(hId, punct);
}

// Fill in the peMetrics for the PEC
// Note: this member function is part of the BasePEImpl interface, and needs
// to be protected from asynchronous access.
bool PEImpl::getMetrics(PEMetricsInfo& peMetrics) const
{
    // We do this check without grabbing the lock so that this call returns quickly during
    // PE startup. The HC monitors this call, and uses it taking too long as a sign that
    // something is wrong. This is safe to do outside of the lock as we're using an atomic
    // variable.
    if (!operatorsCreated_) {
        return false;
    }

    {
        AutoMutex am(operMutex_);
        AutoJVMDetach autoDetach(*this);

        // Add the pe-level metrics
        peMetric_->getMetrics(peMetrics);
        // Add the operator-level metrics
        std::vector<int64_t> relativeCost;
        relativeCost.resize(operators_.size(), 0);
        ThreadProfiler& tp = getThreadProfiler();
        tp.getOperatorRelativeCost(relativeCost);
        typedef vector<OperatorImpl*>::const_iterator Iter;
        unsigned int i = 0;
        for (Iter oit = operators_.begin(); oit != operators_.end(); ++oit) {
            OperatorImpl const& oper = **oit;
            oper.getContextImpl().getMetricsImpl().getMetrics(peMetrics, oper, relativeCost[i]);
            ++i;
        }
    }

    return true;
}

uint64_t PEImpl::getOperatorsNTuplesSubmitted() const
{
    AutoMutex am(operMutex_);
    if (!operatorsCreated_) {
        return 0;
    }

    uint64_t nTuples = 0;
    typedef vector<OperatorImpl*>::const_iterator Iter;
    for (Iter oit = operators_.begin(); oit != operators_.end(); ++oit) {
        OperatorImpl const& oper = **oit;
        for (uint32_t i = 0; i < oper.getContextImpl().getNumberOfOutputPorts(); ++i) {
            nTuples += oper.getContextImpl()
                         .getMetricsImpl()
                         .getOutputPortMetric(i, OperatorMetrics::nTuplesSubmitted)
                         .getValue();
        }
    }
    return nTuples;
}

void PEImpl::logTerminatingException(const string& opName)
{
    PEImpl& pe = PEImpl::instance();
    SPLLOGMSG(L_ERROR, SPL_RUNTIME_EXCEPTION_TERMINATE_PE(pe.getPEId(), opName), SPL_PE_DBG);
}

bool PEImpl::getOutputPortConnections(uint32_t port, vector<OutputConnection>& connections) const
{
    if (port >= portIndexToOportId_.size()) {
        return false;
    }
    vector<DataSender::OutputCongestion> congestionfactors;
    vector<PETransportOPort::FilterMetric> metrics;
    getCongestionFactorsAndMetrics(port, congestionfactors, metrics);
    connections.clear();
    for (uint32_t i = 0, ui = congestionfactors.size(); i < ui; i++) {
        DataSender::OutputCongestion& oc = congestionfactors[i];
        int64_t nTuplesFilteredOut = 0;
        for (uint32_t k = 0, uk = metrics.size(); k < uk; k++) {
            PETransportOPort::FilterMetric& fm = metrics[k];
            if (fm.match(oc.receiverPEId, oc.receiverPortId)) {
                nTuplesFilteredOut = fm.nTuplesFilteredOut();
                break;
            }
        }
        OutputConnection oic(oc.receiverPEId, oc.receiverPortIndex, oc.congestionFactor,
                             nTuplesFilteredOut);
        connections.push_back(oic);
    }
    return true;
}

int PEImpl::getOutputPortCongestionFactor(uint32_t port) const
{
    if (port >= portIndexToOportId_.size()) {
        return -1;
    }
    vector<DataSender::OutputCongestion> congestionfactors;
    getCongestionFactors(port, congestionfactors);
    OutputConnection::CongestionFactorType maxCf = 0;
    for (uint32_t i = 0, ui = congestionfactors.size(); i < ui; i++) {
        OutputConnection::CongestionFactorType cf = congestionfactors[i].congestionFactor;
        if (cf > maxCf) {
            maxCf = cf;
        }
    }
    return maxCf;
}

int PEImpl::getOperatorRelativeCost(uint32_t idx) const
{
    ThreadProfiler& tp = getThreadProfiler();
    return int(tp.getOperatorRelativeCost(idx));
}

static void logAndIgnoreSignalHandler(int signalNumber)
{
    SPLTRACEMSG(L_DEBUG, SPL_RUNTIME_IGNORED_SIGNAL(signalNumber), SPL_PE_DBG);
}

void PEImpl::recordOldSigPipeHandler()
{
    // can only check current handler by replacing it, so replace with default, then restore
    oldSigPipeHandler_ = signal(SIGPIPE, SIG_DFL);
    signal(SIGPIPE, oldSigPipeHandler_);
}

void PEImpl::overrideSigPipeHandlerIfOld()
{
    // can only check current handler by replacing it, so replace with default, then restore
    void (*newSigPipeHandler)(int) = signal(SIGPIPE, SIG_DFL);
    if (oldSigPipeHandler_ == newSigPipeHandler) {
        signal(SIGPIPE, logAndIgnoreSignalHandler);
    } else {
        signal(SIGPIPE, newSigPipeHandler);
    }
}

// Note: this member function is part of the BasePEImpl interface, and needs
// to be protected from asynchronous access. However, it does not access operators_
// or outputPorts_, so it does not need to acquire any mutexes.
void PEImpl::logLevelUpdate(int level)
{
    AutoJVMDetach autoDetach(*this);
    javaSPLRuntime_->logLevelUpdate(level);
}

// Note: this member function is part of the BasePEImpl interface, and needs
// to be protected from asynchronous access. However, it does not access operators_
// or outputPorts_, so it does not need to acquire any mutexes.
void PEImpl::traceLevelUpdate(int level)
{
    AutoJVMDetach autoDetach(*this);
    javaSPLRuntime_->traceLevelUpdate(level);
}

void PEImpl::visualizerUpdate(bool isVisualizing)
{
    javaSPLRuntime_->visualizerUpdate(isVisualizing);
}

void PEImpl::deleteAllCheckpoints()
{
    try {
        vector<OperatorImpl*>::iterator oit;
        for (oit = operators_.begin(); oit != operators_.end(); ++oit) {
            CheckpointContextImpl* ckptContext =
              (*oit)->getContextImpl().getCheckpointContextImpl();
            if (ckptContext != NULL) {
                ckptContext->deleteAllCheckpoints();
            }
        }
        CheckpointContextImpl::finalize();
    } catch (DataStoreException const& e) {
        APPTRC(L_INFO, "Failed to delete checkpoints: received exception: " << e.getExplanation(),
               SPL_PE_DBG);
    } catch (std::exception const& e) {
        APPTRC(L_INFO, "Failed to delete checkpoints: received exception: " << e.what(),
               SPL_PE_DBG);
    } catch (...) {
        APPTRC(L_INFO, "Failed to delete checkpoints: received unknown exception", SPL_PE_DBG);
    }
}

OperatorImpl& PEImpl::getOperatorImpl(uint32_t operatorIndex)
{
    if (operatorIndex < operators_.size()) {
        return *operators_[operatorIndex];
    }
    if (operators_.empty()) {
        THROW(SPLRuntime, "The operators list is empty");
    } else {
        THROW(SPLRuntime,
              "Operator index " << operatorIndex << " out of range 0-" << operators_.size() - 1);
    }
}

JavaVM* PEImpl::getJavaVM() const
{
    return javaVMFactory_->isJVMStarted() ? javaVMFactory_->getJVM() : NULL;
}

void PEImpl::setTagData(const string& tagName, const std::map<string, string>& tagValues)
{
    if (isStandalone_) {
        return;
    }
    OperatorImpl& op = *operators_[OperatorTracker::getCurrentOperator()];
    std::string opIndexStr = boost::lexical_cast<std::string>(op.getContextImpl().getGlobalIndex());
    if (tagName == Operator::TagNames::OperatorIGC) {
        APPTRC(L_DEBUG,
               "Setting tag name " << tagName << " for operator (index=" << opIndexStr << ")",
               SPL_OPER_DBG);
        // check for empty key in tag values
        if (tagValues.find("") != tagValues.end()) {
            APPTRC(L_ERROR,
                   "Invalid tag data for tag name " << tagName
                                                    << " for operator (index=" << opIndexStr
                                                    << "): empty-string as key is not allowed",
                   SPL_OPER_DBG);
            return;
        }
        if (tagValues.find("operatorIndex") != tagValues.end()) {
            APPTRC(L_ERROR,
                   "Invalid tag data for tag name " << tagName
                                                    << " for operator (index=" << opIndexStr
                                                    << "): \"operatorIndex\" is a reserved key",
                   SPL_OPER_DBG);
            return;
        }
        try {
            std::map<string, string> tagValuesInternal(tagValues);
            // inject a key-value pair: operatorIndex=index
            tagValuesInternal["operatorIndex"] = opIndexStr;
            platform_->registerOperatorIGC(tagValuesInternal);
            APPTRC(L_DEBUG,
                   "Successfully set tag name " << tagName << " for operator (index=" << opIndexStr
                                                << ")",
                   SPL_OPER_DBG);
        } catch (DistilleryException const& e) {
            APPTRC(L_ERROR,
                   "Failed to set tag name " << tagName << " for operator (index=" << opIndexStr
                                             << "): received exception: " << e.getExplanation(),
                   SPL_PE_DBG);
        } catch (std::exception const& e) {
            APPTRC(L_ERROR,
                   "Failed to set tag name " << tagName << " for operator (index=" << opIndexStr
                                             << "): received exception: " << e.what(),
                   SPL_PE_DBG);
        } catch (...) {
            APPTRC(L_ERROR,
                   "Failed to set tag name " << tagName << " for operator (index=" << opIndexStr
                                             << "): received unknown exception",
                   SPL_PE_DBG);
        }
    } else {
        APPTRC(L_WARN,
               "Unknown tag name " << tagName << " for operator (index=" << opIndexStr
                                   << "); Tag values discarded",
               SPL_PE_DBG);
    }
}

int32_t PEImpl::getApplicationConfiguration(string const& configName,
                                            std::map<string, string>& properties)
{
    try {
        PlatformAdapter& pa = getPlatform();
        std::string token = pa.getUserSessionId();
        AutoUserSession aus(pa, token);
        pa.getAppConfig(token, configName, properties);
        APPTRC(L_DEBUG, "Successfully got application configuration " << configName, SPL_PE_DBG);
        return 0;
    } catch (Distillery::DistilleryException const& e) {
        APPTRC(L_ERROR,
               "Failed to get application configuration "
                 << configName << " because of exception: " << e.getExplanation(),
               SPL_PE_DBG);
    }
    // Let other exceptions bubble up
    return -1;
}

std::string PEImpl::getApplicationConfigurationProperty(std::string const& configName,
                                                        std::string const& propertyName,
                                                        int32_t* errorCode)
{
    std::map<string, string> properties;
    int32_t rc = getApplicationConfiguration(configName, properties);
    if (rc != 0) {
        if (errorCode != NULL) {
            *errorCode = -1; // cannot find configName
        }
        return "";
    }
    std::map<string, string>::const_iterator result = properties.find(propertyName);
    if (result != properties.end()) {
        if (errorCode != NULL) {
            *errorCode = 0; // success
        }
        APPTRC(L_DEBUG,
               "Successfully got application configuration property "
                 << propertyName << " for configuration " << configName,
               SPL_PE_DBG);
        return result->second;
    } else {
        if (errorCode != NULL) {
            *errorCode = -2; // cannot find propertyName
        }
        APPTRC(L_DEBUG,
               "Failed to get application configuration property "
                 << propertyName << " for configuration " << configName,
               SPL_PE_DBG);
        return "";
    }
}

void PEImpl::updatePEAttributes(const std::map<string, string>& properties) {}

bool PEImpl::isJobCancelling() const
{
    AutoMutex am(shutdownMutex_);
    return isJobCancellation_;
}

void PEImpl::forwardConsistentRegionNotification(int32_t regionIndex,
                                                 std::string const& type,
                                                 std::string const& msg)
{
    getConsistentRegionService().forwardNotification(regionIndex, type, msg);
}

bool PEImpl::allConsistentRegionOpReady()
{
    APPTRC(L_INFO,
           "number of cr ops " << numCROps_ << " registered ops "
                               << getConsistentRegionService().getNumRegisteredOps(),
           SPL_PE_DBG);
    if (getConsistentRegionService().getNumRegisteredOps() == numCROps_) {
        return true;
    }
    return false;
}

void PEImpl::setConnectedToJCP()
{
    getConsistentRegionService().setConnectedToJCP();
}
