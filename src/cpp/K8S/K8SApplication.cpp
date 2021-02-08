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

#include <K8S/K8SApplication.h>
#include <K8S/K8SPlatform.h>
#include <SAM/SAMHelperFunctions.h>
#include <SAM/augmentedApplicationModel.h>
#include <SPL/Runtime/ProcessingElement/BasePEImpl.h>
#include <SPL/Runtime/ProcessingElement/PE.h>
#include <TRC/ConsoleTracer.h>
#include <TRC/TRCUtils.h>
#include <curl/curl.h>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>

DEBUG_NAMESPACE_USE
K8S_NAMESPACE_USE
UTILS_NAMESPACE_USE

using namespace boost;

static void loadAADL(std::string& str)
{
    /*
     * Grab the hostname.
     */
    std::string peName = get_environment_variable("STREAMS_PE_NAME", "default");
    /*
     * Open the AADL file.
     *
     * Note: this must match how we store the AADL in the config map in the
     * controller. See:
     *    com.ibm.streams.controller.k8s.utils.ConfigMapUtils
     */
    std::string filename = "/etc/config/pe/aadl.xml";
    std::ifstream file(filename);
    /*
     * Check if the file is valid.
     */
    if (file.fail()) {
        SPCDBG(L_ERROR, "Cannot read the AADL file, aborting.", K8S_GENERAL);
        THROW(K8SPlatform, "Cannot read the AADL file, aborting.");
    }
    /*
     * Reserve the necessary space in the string.
     */
    file.seekg(0, std::ios::end);
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    /*
     * Load and close the file.
     */
    str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
}

K8SApplication::K8SApplication()
  : m_metrics()
  , m_probe()
  , m_subscriptions()
  , m_retCode(0)
{}

int K8SApplication::run(const UTILS_NAMESPACE::arg_vector_t& args)
{
    curl_global_init(CURL_GLOBAL_ALL);
    /*
     * Load the AADL.
     */
    std::string descStr;
    loadAADL(descStr);
    /*
     * Decode the AADL.
     */
    shared_ptr<AADL::augmentedPEType> peSpec(new AADL::augmentedPEType);
    SAM_NAMESPACE::fromString(*peSpec, descStr);
    /*
     * Grab the launch count and override the AADL definition.
     */
    std::string lcString = get_environment_variable("STREAMS_PE_LAUNCH_COUNT", "1");
    int launchCount = boost::lexical_cast<int>(lcString);
    peSpec->launchCount() = launchCount;
    /*
     * Get the namespace.
     */
    std::string ns = get_environment_variable("STREAMS_INSTANCE_ID", "default");
    /*
     * Grab the job name.
     */
    std::string jobName = get_environment_variable("STREAMS_JOB_NAME", "default");
    /*
     * Determine the application trace level.
     */
    int trcAADL = iL_ERROR;
    int trcENV = levelFromStr2Int(get_environment_variable("STREAMS_APP_TRACE_LEVEL", "ERROR"));
    if (peSpec->currentTracing().present()) {
        trcAADL = levelFromStr2Int(peSpec->currentTracing()->tracingUpdate().level());
    }
    int level = iL_OFF;
    if (trcAADL >= trcENV) {
        SPCDBG(L_DEBUG, "Using tracing information from the AADL", K8S_GENERAL);
        level = trcAADL;
    } else {
        SPCDBG(L_DEBUG, "Using tracing information from STREAMS_APP_TRACE_LEVEL", K8S_GENERAL);
        level = trcENV;
    }
    /*
     * Build the APP tracer.
     */
    Distillery::debug::Tracer* tracer = new ConsoleTracer(level);
    set_app_tracer(tracer);
    set_app_trclevel(level);
    /*
     * Create the PE. Because the metrics thread needs a reference to the PE, the platform
     * needs a reference to the metrics thread, and the PE needs a reference to the platform,
     * the objects need to be instantiated in this particular order.
     */
    m_pe.reset(new SPL::SplPE(false /* is_standalone */));
    m_metrics = std::make_shared<K8SMetricsThread>(jobName, m_pe, peSpec->id());
    auto platform = std::make_shared<K8SPlatform>(m_metrics, ns, jobName, peSpec->id());
    m_pe->getImpl().initialize(*peSpec, *platform);
    m_pe->getImpl().logLevelUpdate(level);
    /*
     * Create the probe thread.
     */
    m_probe = std::make_shared<K8SProbeThread>(m_pe);
    m_probe->create();
    /*
     * Create the health thread.
     */
    m_connectivity = std::make_shared<K8SConnectivityThread>(m_pe, ns, jobName, peSpec->id());
    m_connectivity->create();
    /*
     * Create the metrics thread.
     */
    m_metrics->create();
    /*
     * Create the subscriptions thread.
     */
    m_subscriptions = std::make_shared<K8SSubscriptionsThread>(m_pe, jobName, peSpec->id(), ns);
    m_subscriptions->create();
    /*
     * Create the consistent region notification thread.
     */
    m_consistentregion =
      std::make_shared<K8SConsistentRegionThread>(m_pe, jobName, peSpec->id(), ns);
    m_consistentregion->create();
    /*
     * Start the PE.
     */
    try {
        m_pe->getImpl().process();
    } catch (DistilleryException const& e) {
        SPCDBG(L_ERROR, "Exception caught: " << e.what(), K8S_GENERAL);
        SPCDBG(L_ERROR, e.getBacktrace(), K8S_GENERAL);
        m_retCode = 1;
    } catch (...) {
        SPCDBG(L_ERROR, "An unspecified exception was caught", K8S_GENERAL);
        m_retCode = 1;
    }
    SPCDBG(L_INFO, "PE terminated with error code: " << m_retCode, K8S_GENERAL);
    /*
     * Stop all threads.
     */
    m_subscriptions->stop();
    m_metrics->stop();
    m_probe->stop();
    m_connectivity->stop();
    m_consistentregion->stop();
    /*
     * Join all threads.
     */
    m_subscriptions->join();
    m_metrics->join();
    m_probe->join();
    m_connectivity->join();
    m_consistentregion->join();
    /*
     * NOTE(xrg)
     *
     * CURL, if built to use NSS, will cause Valgrind to bark about still reachable memory.
     * This is the global NSS state that should be cleaned-up by CURL using PR_Cleanup().
     * However, it cannot be done from a shared library so it is not done in our case. See:
     *
     * https://bugzilla.redhat.com/show_bug.cgi?id=1510247#c26
     */
    curl_global_cleanup();
    return m_retCode;
}

std::ostream& K8SApplication::printDescription(std::ostream& o) const
{
    o << "Streams Kubernetes PE";
    return o;
}

void K8SApplication::termHandler(int sig)
{
    /*
     * If sig == SIGTERM, we are being terminated by Kubernetes. That happens when the job
     * terminates, when the user deletes the pod, or when the liveness probe fails. The most
     * likely scenario is that the job has been terminated, so we treat it as such.
     */
    switch (sig) {
        case SIGQUIT:
            SPCDBG(L_INFO, "PE termination signal received", K8S_GENERAL);
            m_pe->getImpl().shutdown(false);
            break;
        default:
            SPCDBG(L_INFO, "Job termination signal received", K8S_GENERAL);
            m_pe->getImpl().shutdown(true);
            break;
    }
}
