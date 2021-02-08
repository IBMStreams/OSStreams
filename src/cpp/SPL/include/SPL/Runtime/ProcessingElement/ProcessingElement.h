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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PROCESSING_ELEMENT_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PROCESSING_ELEMENT_H

/**
 * @file ProcessingElement.h @brief Definition of the SPL::ProcessingElement class.
 */

#include <map>
#include <stdint.h>
#include <vector>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Function/TimeFunctions.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <string>
struct JavaVM_;
typedef struct JavaVM_ JavaVM;
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL {
enum AdjustDirection
{
    UP,
    DOWN
};
class PEMetrics;

/// Class that represents a processing element
class DLL_PUBLIC ProcessingElement
{
  public:
    class OutputConnection
    {
      public:
        /// Type of congestion factor
        typedef uint8_t CongestionFactorType;

        /// @brief Constructs an OutputConnection.
        /// @param peId_  destination PE number
        /// @param inputPortIndex_  PE input port of destination PE
        /// @param congestionFactor_  measure of connection congestion (0..100)
        /// @param nTuplesFilteredOut_  Number of tuples filtered out for this connection
        OutputConnection(int64_t peId_,
                         int64_t inputPortIndex_,
                         CongestionFactorType congestionFactor_,
                         uint64_t nTuplesFilteredOut_) :
          _peId(peId_),
          _inputPortIndex(inputPortIndex_)
        {
            _data.reserve(2);
            _data.push_back(congestionFactor_);
            _data.push_back(nTuplesFilteredOut_);
        }
        /// @brief Returns the PE id.
        /// @return destination PE number
        int64_t peId() const { return _peId; }

        /// @brief Returns the input port on destination PE.
        /// @return PE input port number
        int64_t inputPortIndex() const { return _inputPortIndex; }

        /// @brief Returns the congestion factor for this connection.
        /// @return congestion factor (0..100)
        CongestionFactorType congestionFactor() const { return _data[0]; }

        /// @brief Returns the number of tuples filtered out for this connection.
        /// @return count of tuples not passed through this connection
        uint64_t nTuplesFilteredOut() const { return _data[1]; }

      private:
        uint64_t _peId;
        uint64_t _inputPortIndex;
        std::vector<int64_t> _data;
    };

#ifndef DOXYGEN_SKIP_FOR_USERS
    /// Destructor
    ///
    virtual ~ProcessingElement() {}
#endif /*DOXYGEN_SKIP_FOR_USERS*/

    /// @brief Determines if a data directory was specified at compile or submission time.
    /// @return Returns true if a data directory was specified, otherwise false.
    virtual bool hasDataDirectory() const = 0;

    /// @brief Returns the path to the data directory of the %SPL application.
    /// @return working directory of the PE
    virtual const std::string& getDataDirectory() const = 0;

    /// @brief Returns the path to the checkpointing directory of the PE.
    /// @return checkpointing directory of the PE
    /// @deprecated The new checkpoint API no longer uses this API or the related --checkpoint-directory compiler option.
    virtual const std::string& getCheckpointDirectory() const = 0;

    /// @brief Returns the application scope for the job the PE is running in.
    /// @return application scope
    virtual const std::string& getApplicationScope() const = 0;

    /// @brief Returns the path to the %SPL application directory.
    /// @return application directory
    virtual const std::string& getApplicationDirectory() const = 0;

    /// @brief Returns the path to the %SPL application output directory.
    /// @return application output directory
    virtual const std::string& getOutputDirectory() const = 0;

    /// @brief Returns the absolute path of the toolkit containing the operator for this PE.
    /// @return returns the toolkit directory
    virtual std::string getToolkitDirectory() const = 0;

    /// @brief Returns the absolute path of the specified toolkit.
    /// @param toolkitName name of the toolkit to return a path to
    /// @return returns the toolkit directory
    virtual std::string getToolkitDirectory(const std::string& toolkitName) const = 0;

    /// @brief Returns the runtime identifier for this PE.
    /// @return runtime identifier for this PE
    virtual uint64_t getPEId() const = 0;

    /// @brief Returns the runtime identifier for the job containing this PE.
    /// @return runtime identifier for the job containing this PE
    virtual uint64_t getJobId() const = 0;

    /// @brief Returns the number of times this PE has been relaunched. For the first.
    /// execution, the value will be 0.
    /// @return the relaunch count
    virtual uint32_t getRelaunchCount() const = 0;

    /// @brief Returns the number of inputs for this PE.
    /// @return the number of inputs for this PE
    virtual uint32_t getNumberOfInputPorts() const = 0;

    /// @brief Returns the number of outputs for this PE.
    /// @return the number of outputs for this PE
    virtual uint32_t getNumberOfOutputPorts() const = 0;

    /// @brief Checks if the PE is running in standalone mode.
    /// @return true if running in standalone mode, false otherwise
    virtual bool isStandalone() const = 0;

    /// @brief Checks if the PE is running under optimized mode.
    /// @return true if running in optimized mode, false otherwise
    virtual bool isOptimized() const = 0;

    /// @brief Checks if the PE is running under fused mode.
    /// @return true if running in fused mode, false otherwise
    virtual bool isInFusedMode() const = 0;

    /// @brief Checks if the PE is running under debug mode.
    /// @return true if running in debug mode, false otherwise
    virtual bool isInDebugMode() const = 0;

    /// @brief Checks if shutdown was requested.
    /// @return true if shutdown was requested, false otherwise
    virtual bool getShutdownRequested() const = 0;

    /// @brief Blocks until shutdown is requested.
    /// @return void
    virtual void blockUntilShutdownRequest() const = 0;

    /// @brief Blocks until shutdown is requested.
    /// @param absTime absolute time to wait until if shutdown is not requested
    /// @return true if unblocked due to the shutdown request, false otherwise
    virtual bool blockUntilShutdownRequest(const timespec& absTime) const = 0;

    /// @brief Blocks until shutdown is requested.
    /// @param timeInSeconds maximum wait time in seconds (could be
    /// fractional) if shutdown is not requested
    /// @return true if unblocked due to the shutdown request, false otherwise
    virtual bool blockUntilShutdownRequest(const double timeInSeconds) const = 0;

    /// @brief Returns the PE metrics.
    /// @return PE metrics
    virtual PEMetrics& getMetrics() = 0;

    /// @brief Returns the current PE.
    /// @return the ProcessingElement for the current process
    static ProcessingElement& pe();

    /// @brief Returns the instance ID for the current processing element.
    /// @return the instance ID for the current processing element (username if running standalone)
    virtual std::string getInstanceID() const = 0;

    /// @brief Returns the connection and congestion information for a PE output port.
    /// @param port the output port number
    /// @param connections  output connection information
    /// @return 'true' for success, 'false' for invalid port number
    virtual bool getOutputPortConnections(uint32_t port,
                                          std::vector<OutputConnection>& connections) const = 0;

    /// @brief Returns the congestion factor for a PE output port.
    /// @param port the output port number
    /// @return -1 for invalid port, or 0..100 for congestion factor
    virtual int getOutputPortCongestionFactor(uint32_t port) const = 0;

    /// @brief Returns the Java virtual machine.
    /// @return NULL if no JVM, or the pointer to the created JVM
    virtual JavaVM* getJavaVM() const = 0;

    /// @brief Returns the domain ID for the current processing element.
    /// @return the domain ID for the current processing element (username if running standalone)
    virtual std::string getDomainID() const = 0;

    /// @brief Returns the name of the %SPL application the PE is running.
    /// @return application name
    /// @since InfoSphere&reg; Streams Version 4.1
    virtual std::string getApplicationName() const = 0;

    /// @brief Returns the name of the job group for the job the processing element is running in.
    /// @return job group name
    /// @since InfoSphere&reg; Streams Version 4.1
    virtual std::string getJobGroup() const = 0;

    /// @brief Returns the name of the job the processing element is running in.
    /// @return name for the job the processing element is running in
    /// @since InfoSphere&reg; Streams Version 4.0.1
    virtual std::string getJobName() const = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
    /// @brief Indicates if an operator must rethrow a caught exception.
    ///
    /// An operator must rethrow if the exception was caught from
    /// a different operator in the PE.
    /// @return true if an operator must rethrow, false otherwise
    virtual bool mustRethrowException() const = 0;
#endif /* DOXYGEN_SKIP_FOR_USERS */

    /// @brief Returns the application configuration for the given @c name.
    ///
    /// The application configuration is a set of properties where each
    /// key and its value in the property set is a string.
    /// This method copies all the properties from the specified
    /// configuration into the properties output map.
    ///
    /// @param configName Application configuration name.
    /// @param[out] properties Output map where the configuration properties
    ///     are to be inserted.
    /// @return 0 if the configuration specified by name was found,
    ///     otherwise -1.
    /// @since IBM Streams Version 4.2.0.
    virtual int32_t getApplicationConfiguration(std::string const& configName,
                                                std::map<std::string, std::string>& properties) = 0;

    /// @brief Returns the value associated with the given configuration
    /// property @c propertyName from the configuration identified by
    /// @c configName.
    ///
    /// An empty string is returned if there is no value associated with
    /// the @c propertyName, or if the configuration does not exist.
    ///
    /// @param configName Application configuration name.
    /// @param propertyName Configuration property name.
    /// @param[out] errorCode Pointer to an output error code value.  If the
    ///     pointer is not null, the value is set to 0 for success, -1 if
    ///     the configuration could not be found, -2 if the configuration
    ///     property could not be found.
    /// @return The value associated with the property name or an empty
    ///     string if there is no value associated with the @c propertyName
    ///     or if the configuration does not exist.
    /// @since IBM Streams Version 4.2.0.
    virtual std::string getApplicationConfigurationProperty(std::string const& configName,
                                                            std::string const& propertyName,
                                                            int32_t* errorCode = NULL) = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
    /// @brief Check if the job is being cancelled.
    /// @return true if the job is being cancelled, false otherwise
    /// @since IBM Streams Version 4.2.0.
    virtual bool isJobCancelling() const = 0;
#endif

    /// Return the relative operator metric for the current operator
    /// @return 0..100 for relative operator metric
    /// @since IBM Streams Version 4.2.3.0.
    virtual int getOperatorRelativeCost(uint32_t index) const = 0;
};
}

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PROCESSING_ELEMENT_H */
