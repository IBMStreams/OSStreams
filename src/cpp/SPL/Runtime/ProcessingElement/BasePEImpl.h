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

#ifndef BASEPE_IMPL_H_INCLUDED
#define BASEPE_IMPL_H_INCLUDED

#include <SAM/SAMNamespaceDefs.h>
#include <SAM/SAMTypes.h>
#include <SPL/Runtime/ProcessingElement/BasePE.h>
#include <SPL/Runtime/ProcessingElement/PlatformAdapter.h>
#include <TRANS/ConnectionState.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/SPCException.h>
#include <UTILS/SPCTypes.h>
#include <UTILS/SupportFunctions.h>

namespace SPL {

#define PE_CANCEL_DELAY (UTILS_NAMESPACE::get_environment_variable("PE_CANCEL_DELAY", 30))

class PEMetricsInfo;

/// Base class that PE applications derives from.
class BasePEImpl : public UTILS_NAMESPACE::ConnectionStateEmitter
{
  public:
    virtual ~BasePEImpl(void) {}

    /// PE initialization routine, must be called before process()
    virtual void initialize(AADL::augmentedPEType& model, SPL::PlatformAdapter& platform) = 0;

    /// PE execution entry point, invoked on startup
    /// Return from this method indicates completion of PE-execution.
    virtual void process(void) = 0;

    /// PE shutdown handler, invoked on termination
    /// The PE-writer may perform cleanup tasks here.
    /// @param isJobShuttingDown true if the PE shutdown occurs due to a job shutdown. The default
    /// is false.
    virtual void shutdown(bool isJobShuttingDown = false) = 0;

    /// PE shutdown status
    virtual bool getShutdownRequested() const = 0;

    /// Notification about a new consumer for the stream produced by an
    /// output port
    /// @param ntype notification type
    /// @param oportid the output port producing the stream (run-time ID)
    /// @param nslabel the name service entry key for the input port
    /// @param portIndex the compile-time index value of the input (remote) port
    /// @param filterExpression value of filter expression for add/update
    /// that will consume the stream
    virtual void routingInfoNotificationHandler(const SAM_NAMESPACE::RoutingNotificationType ntype,
                                                const SAM_NAMESPACE::PortID& oportid,
                                                const std::string& nslabel,
                                                const SAM_NAMESPACE::ID& portIndex,
                                                const std::string& filterExpression) = 0;

    /// Get the state of all the connections of all the PE output ports.
    /// @param[out] states on return contains the connection state values
    virtual void getConnectionStates(
      std::vector<UTILS_NAMESPACE::ConnectionState>& states) const = 0;

    /// Get the labels of all the static connections of all the PE input ports.
    /// @param[out] labels on return contains the connection labels values
    virtual void getStaticInputConnectionLabels(std::vector<std::string>& labels) const = 0;

    /// Get the labels of all the static connections of all the PE output ports.
    /// @param[out] labels on return contains the connection labels values
    virtual void getStaticOutputConnectionLabels(std::vector<std::string>& labels) const = 0;

    /// Get all PE metrics data (including operator metrics)
    /// @param peMetrics (out parameter) metrics to populate
    /// @pre peMetrics is newly constructed, and thus empty
    /// @return false if metrics are not available, true otherwise
    virtual bool getMetrics(PEMetricsInfo& peMetrics) const = 0;

    /// Stream view notification handler
    /// @param ntype notification type
    /// @param operatorIndex producer operator index
    /// @param operatorOutputPortIndex producer operator output port index
    /// @param nslabel the name service entry key for the input port
    /// @param viewID the view id
    /// @param properties view runtime properties
    virtual void streamViewNotificationHandler(
      const SAM_NAMESPACE::RoutingNotificationType ntype,
      const SAM_NAMESPACE::ID& operatorIndex,
      const SAM_NAMESPACE::ID& operatorOutputPortIndex,
      const std::string& nslabel,
      const SAM_NAMESPACE::ID& viewID,
      const std::vector<Streams::nameValuePair_t>& properties) = 0;

    /// Grab the notification listener
    // @param handback user data for the listener
    virtual UTILS_NAMESPACE::ConnectionStateListener* getNotificationListener(
      void** handback) const = 0;

    /// Notification of changed log level
    /// @param level New log level
    virtual void logLevelUpdate(int level) = 0;

    /// Notification of changed trace level
    /// @param level New trace level
    virtual void traceLevelUpdate(int level) = 0;

    virtual void updatePEAttributes(const std::map<std::string, std::string>& properties) = 0;

    virtual void forwardConsistentRegionNotification(int32_t regionIndex,
                                                     std::string const& type,
                                                     std::string const& msg) = 0;

    virtual bool allConsistentRegionOpReady() = 0;

    virtual void setConnectedToJCP() = 0;

    virtual bool isInConsistentRegion() = 0;
};

}

#endif
