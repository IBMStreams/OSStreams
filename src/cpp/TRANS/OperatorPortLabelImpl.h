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

#ifndef TRANS_OPERATORPORTLABELIMPL_H_
#define TRANS_OPERATORPORTLABELIMPL_H_

#include <TRANS/OperatorPortLabel.h>

#include <NAM/NAM_NameService.h>

UTILS_NAMESPACE_BEGIN

/**
 * @internal
 * Extension used by Transport to register, unregister, and resolve an
 * OperatorPortLabel with the Streams Name Service.
 *
 * Note that none of the class member functions are virtual.  Class extension
 * is used here to provide an extended interface for name service operations.
 */
class OperatorPortLabelImpl : public OperatorPortLabel
{
  public:
    OperatorPortLabelImpl()
      : OperatorPortLabel()
    {}

    /**
     * Constructs an instance from a job id, operator index, and operator port index.
     */
    OperatorPortLabelImpl(unsigned long long jobId,
                          unsigned long long operatorIndex,
                          unsigned long long operatorPortIndex)
      : OperatorPortLabel(jobId, operatorIndex, operatorPortIndex)
    {}

    /**
     * Constructs an instance from a OperatorPortLabel instance.
     */
    OperatorPortLabelImpl(const OperatorPortLabel& other)
      : OperatorPortLabel(other)
    {}

    /**
     * Constructs an instance from a string port label.
     */
    OperatorPortLabelImpl(const std::string& label)
      : OperatorPortLabel(label)
    {}

    /**
     * Constructs an instance from a C string.
     */
    OperatorPortLabelImpl(const char* s)
      : OperatorPortLabel(s)
    {}

    /**
     * Binds this label to the specified peId and portId using the
     * specified name service.
     *
     * If the given retryWaitMicroSecs is zero, no retries are made in case
     * of a name service error; otherwise, this implementation will retry
     * forever the name service call in case of failure and sleep
     * retryWaitMicroSecs between retries.
     *
     * @param ns name service used for the operation
     * @param peId PE ID
     * @param portId port ID
     * @param retryWaitMicroSecs if non-zero, it indicates the period of
     *  sleep in microseconds after which the name server call is retried
     *  after failure
     */
    void bind(NAM_NAMESPACE::NAM_NameService* const ns,
              unsigned long long peId,
              unsigned long long portId,
              uint retryWaitMicroSecs = 0) const
    {
        bind(ns, peId, portId, false, retryWaitMicroSecs);
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
     * @param peId PE ID
     * @param portID port ID
     * @param retryWaitMicroSecs if non-zero, it indicates the period of
     *  sleep in microseconds after which the name server call is retried
     *  after failure
     */
    void rebind(NAM_NAMESPACE::NAM_NameService* const ns,
                unsigned long long peId,
                unsigned long long portId,
                uint retryWaitMicroSecs = 0) const
    {
        bind(ns, peId, portId, true, retryWaitMicroSecs);
    }

    /**
     * Unbinds this label.  This method logs a trace message in case the
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
     * @param peID pointer to the long long where this function writes the
     *        PE ID associated with this OperatorPortLabel
     * @param portID pointer to the long long where this function writes the
     *        port ID associated with this OperatorPortLabel
     * @throws NameNotFoundException if this label is not bound with the
     *  name service
     * @throws NAMException on other service failures
     */
    void lookup(/* const */ NAM_NAMESPACE::NAM_NameService* const ns,
                unsigned long long* peId,
                unsigned long long* portId,
                unsigned long long* jobId);

  private:
    void bind(NAM_NAMESPACE::NAM_NameService* const ns,
              unsigned long long peId,
              unsigned long long portId,
              bool rebind,
              uint retryWaitMicroSecs) const;
};

UTILS_NAMESPACE_END

#endif /* !TRANS_OPERATORPORTLABELIMPL_H_ */
