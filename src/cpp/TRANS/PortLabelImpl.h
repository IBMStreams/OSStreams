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

#ifndef TRANS_PORTLABELIMPL_H_
#define TRANS_PORTLABELIMPL_H_

#include <TRANS/PortLabel.h>

#include <NAM/NAM_NameService.h>

UTILS_NAMESPACE_BEGIN

/**
 * @internal
 * Extension used by Transport to register, unregister, and resolve a
 * PortLabel with the Streams Name Service.
 *
 * Note that none of the class member functions are virtual.  Class extension
 * is used here to provide an extended interface for name service operations.
 */
class PortLabelImpl : public PortLabel
{
  public:
    PortLabelImpl()
      : PortLabel()
    {}

    /**
     * Constructs an instance from a PE id and a PE port id.
     */
    PortLabelImpl(unsigned long long peId, unsigned long long portId, unsigned long long jobId)
      : PortLabel(peId, portId, jobId)
    {}

    /**
     * Constructs an instance from a PortLabel instance.
     */
    PortLabelImpl(const PortLabel& other)
      : PortLabel(other)
    {}

    /**
     * Constructs an instance from a string port label.
     */
    PortLabelImpl(const std::string& label)
      : PortLabel(label)
    {}

    /**
     * Constructs an instance from a C string.
     */
    PortLabelImpl(const char* s)
      : PortLabel(s)
    {}

    /**
     * Binds this label to the specified hostName and port using the
     * specified name service.
     *
     * If the given retryWaitMicroSecs is zero, no retries are made in case
     * of a name service error; otherwise, this implementation will retry
     * forever the name service call in case of failure and sleep
     * retryWaitMicroSecs between retries.
     *
     * @param ns name service used for the operation
     * @param hostName host name
     * @param port TCP port
     * @param retryWaitMicroSecs if non-zero, it indicates the period of
     *  sleep in microseconds after which the name server call is retried
     *  after failure
     */
    void bind(NAM_NAMESPACE::NAM_NameService* const ns,
              const char* hostName,
              unsigned short port,
              uint retryWaitMicroSecs = 0) const
    {
        bind(ns, hostName, port, false, retryWaitMicroSecs);
    }

    /**
     * Binds this object to the specified name service, overwriting any
     * existing binding.
     *
     * If the given retryWaitMicroSecs is zero, no retries are made in case
     * of a name service error; otherwise, this implementation will retry
     * forever the name service call in case of failure and sleep
     * retryWaitMicroSecs between retries.
     *
     * @param ns name service used for the operation
     * @param hostName host name
     * @param port TCP port
     * @param retryWaitMicroSecs if non-zero, it indicates the period of
     *  sleep in microseconds after which the name server call is retried
     *  after failure
     */
    void rebind(NAM_NAMESPACE::NAM_NameService* const ns,
                const char* hostName,
                unsigned short port,
                uint retryWaitMicroSecs = 0) const
    {
        bind(ns, hostName, port, true, retryWaitMicroSecs);
    }

    /**
     * Unbinds this label.  This method logs a trace messagee in case the
     * service call fails (including when the label is not found) and does
     * not rethrow any exception.
     *
     * @param ns name service used for the operation
     */
    void unbind(NAM_NAMESPACE::NAM_NameService* const ns) const;

    /**
     * Retrieves the data associated with this label from the specified name
     * service.
     *
     * @param ns name service used for the operation
     * @param hostName pointer to the string where this function writes the
     *      host name associated with this PortLabel
     * @param port pointer to the TCP port where this function writes the TCP
     *  port value associated with this PortLabel
     * @throws NameNotFoundException if theis label is not bound with the
     *  name service
     * @throws NAMException on other service failures
     */
    void lookup(/* const */ NAM_NAMESPACE::NAM_NameService* const ns,
                std::string* hostName,
                unsigned short* port,
                const bool force = false);

  private:
    void bind(NAM_NAMESPACE::NAM_NameService* const ns,
              const char* hostName,
              int port,
              bool rebind,
              uint retryWaitMicroSecs) const;
};

UTILS_NAMESPACE_END

#endif /* !TRANS_PORTLABELIMPL_H_ */
