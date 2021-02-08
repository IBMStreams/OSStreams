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

#ifndef PEMETRICSINFO_H_INCLUDED
#define PEMETRICSINFO_H_INCLUDED

#include <SAM/SAMTypes.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/UTILSTypes.h>
#include <map>
#include <vector>

namespace SPL {

typedef std::pair<uint64_t, uint64_t> IDPair;

class BaseMetricInfo
{
  public:
    /// Default constructor
    BaseMetricInfo();

    BaseMetricInfo(const std::string& name, const int64_t value);

    /// Set metric value.
    /// @param value metric value
    void setValue(const int64_t value);

    /// Set metric name
    /// @param name metric name
    void setName(const std::string& name);

    /// Get name
    const std::string getName() const;

    /// Get value.
    const int64_t getValue() const;

    /// Print metric
    /// @param s ostream
    void print(std::ostream& s) const;

  protected:
    std::string _name;
    int64_t _value;
};

class PortMetricsInfo
{
  public:
    /// Default constructor
    PortMetricsInfo();

    /// constructor
    /// @Param ind port index.
    PortMetricsInfo(const uint32_t ind);

    /// Set port metric values.
    /// @param metrics metric values
    void setMetrics(const std::vector<int64_t>& metrics);

    /// Add a metric value to the metrics list
    /// @param metric a metric value
    void addMetrics(const int64_t metric);

    /// Set port index
    /// @param ind port index
    void setIndex(const uint32_t ind);

    /// Get port index
    /// @return port index
    const uint32_t getIndex() const;

    /// Get metrics.
    /// @return list of metrics related to the port.
    const std::vector<int64_t>& getMetrics() const;

    /// Print port metrics
    /// @param s ostream
    /// @param indent how many spaces to indent
    virtual void print(std::ostream& s, unsigned indent = 0);

    virtual ~PortMetricsInfo(){};

  protected:
    uint32_t _index;
    std::vector<int64_t> _metrics;
};

// PE output port need to support metrics for congestion, which are special
// metrics:
// 1. Connection metrics with key of <PEID, PortIndex>, and value of integer.
// 2. List of congestion metrics can be changed dynamically for a PE output port.
class PEOutputPortMetricsInfo : public PortMetricsInfo
{
  public:
    /// Default constructor
    PEOutputPortMetricsInfo();

    /// constructor
    /// @Param info PortMetricsInfo object.
    PEOutputPortMetricsInfo(PortMetricsInfo info);

    /// Set port connection metric values.
    /// @param metrics connection metrics
    void setConnectionMetrics(const std::map<IDPair, std::vector<int64_t> >& metrics);

    /// Add a metric value to the metrics list
    /// @param peID input PE ID
    /// @param portIndex input port compile-time index value
    /// @param value a connection metric value
    void addConnectionMetric(const SAM_NAMESPACE::PEID peID,
                             const SAM_NAMESPACE::ID portIndex,
                             const std::vector<int64_t>& values);

    /// Get metrics.
    /// @return list of connection metrics related to the port.
    const std::map<IDPair, std::vector<int64_t> >& getConnectionMetrics() const;

    /// Print port metrics
    /// @param s ostream
    /// @param indent how many spaces to indent
    virtual void print(std::ostream& s, unsigned indent = 0);

    virtual ~PEOutputPortMetricsInfo(){};

  protected:
    std::map<IDPair, std::vector<int64_t> > _connectionMetrics;
};

class OperatorMetricsInfo
{
  public:
    /// Default constructor
    OperatorMetricsInfo();

    OperatorMetricsInfo(const uint64_t ind,
                        const std::string& name,
                        const std::string& logicalName);

    /// Set operator system metric values.
    /// @param metrics metric values
    void setSysMetrics(const std::vector<int64_t>& metrics);

    /// Add a metric value to the system metrics list
    /// @param metric a metric value
    void addSysMetrics(const int64_t metric);

    /// Set operator metric values.
    /// @param metrics metric values
    void setMetrics(const std::vector<BaseMetricInfo>& metrics);

    /// Add a metric value to the metrics list
    /// @param name a metric name
    /// @param metric a metric value
    void addMetrics(const std::string& name, const int64_t metric);

    /// Set input port metrics objects.
    /// @param ipms input port metrics object
    void setInputPortMetrics(const std::vector<PortMetricsInfo>& ipms);

    /// Add an input port metrics object to the list
    /// @param ipm input port metrics object
    void addInputPortMetrics(const PortMetricsInfo& ipm);

    /// Set output port metrics objects.
    /// @param opms input port metrics object
    void setOutputPortMetrics(const std::vector<PortMetricsInfo>& opms);

    /// Add an output port metrics object to the list
    /// @param opm input port metrics object
    void addOutputPortMetrics(const PortMetricsInfo& opm);

    /// Set operator index
    void setIndex(const uint64_t ind);

    /// Get operator index
    const uint64_t getIndex() const;

    /// Get operator name
    const std::string getName() const;

    /// Get operator logical name
    const std::string getLogicalName() const;

    /// Get operator system metrics.
    const std::vector<int64_t>& getSysMetrics() const;

    /// Get metrics.
    const std::vector<BaseMetricInfo>& getMetrics() const;

    /// Get input port metrics.
    std::vector<PortMetricsInfo>& getInputPortMetrics();

    /// Get input port metrics.
    const std::vector<PortMetricsInfo>& getInputPortMetrics() const;

    /// Get output port metrics.
    std::vector<PortMetricsInfo>& getOutputPortMetrics();

    /// Get output port metrics.
    const std::vector<PortMetricsInfo>& getOutputPortMetrics() const;

    /// Print port metrics
    /// @param s ostream
    /// @param indent how many spaces to indent
    virtual void print(std::ostream& s, unsigned indent = 0);

    virtual ~OperatorMetricsInfo(){};

  protected:
    uint64_t _index;
    std::string _name;
    std::string _logicalName;
    std::vector<int64_t> _sysMetrics;
    std::vector<BaseMetricInfo> _metrics;
    std::vector<PortMetricsInfo> _inputPortMetrics;
    std::vector<PortMetricsInfo> _outputPortMetrics;
};

class PEMetricsInfo
{
  public:
    /// Default constructor
    PEMetricsInfo();

    PEMetricsInfo(const SAM_NAMESPACE::PEID& id);

    void reset();
    bool isReset();
    void doneSet();

    /// Set PE metric values.
    /// @param metrics metric values
    void setMetrics(const std::vector<int64_t>& metrics);

    /// Add a metric value to the metrics list
    /// @param metric a metric value
    void addMetrics(const int64_t metric);

    /// Set operator metrics objects.
    /// @param oms operator metrics object
    void setOperatorMetrics(const std::vector<OperatorMetricsInfo>& oms);

    /// Add an operator metrics object to the list
    /// @param om operator metrics object
    void addOperatorMetrics(const OperatorMetricsInfo& om);

    /// Set input port metrics objects.
    /// @param ipms input port metrics object
    void setInputPortMetrics(const std::vector<PortMetricsInfo>& ipms);

    /// Add an input port metrics object to the list
    /// @param ipm input port metrics object
    void addInputPortMetrics(const PortMetricsInfo& ipm);

    /// Set output port metrics objects.
    /// @param opms input port metrics object
    void setOutputPortMetrics(const std::vector<PEOutputPortMetricsInfo>& opms);

    /// Add an output port metrics object to the list
    /// @param opm input port metrics object
    void addOutputPortMetrics(const PEOutputPortMetricsInfo& opm);

    /// Set PE ID
    void setID(const SAM_NAMESPACE::PEID& id);

    /// Get PE ID
    const SAM_NAMESPACE::PEID getID() const;

    /// Get PE logical index
    const SAM_NAMESPACE::ID getIndex() const;

    /// Set PE logical index
    void setIndex(const SAM_NAMESPACE::ID& id);

    /// Set last PE collection time stamp.
    /// @param lastcollection last PE collection time stamp
    void setTimestamp(const uint64_t timestamp);

    /// Get last PE collection time stamp.
    /// @return last PE collection time stamp
    const uint64_t getTimestamp() const;

    /// Get PE metrics.
    const std::vector<int64_t>& getMetrics() const;

    /// Get operator metrics.
    std::vector<OperatorMetricsInfo>& getOperatorMetrics();

    /// Get operator metrics.
    const std::vector<OperatorMetricsInfo>& getOperatorMetrics() const;

    /// Get input port metrics.
    std::vector<PortMetricsInfo>& getInputPortMetrics();

    /// Get input port metrics.
    const std::vector<PortMetricsInfo>& getInputPortMetrics() const;

    /// Get output port metrics.
    std::vector<PEOutputPortMetricsInfo>& getOutputPortMetrics();

    /// Get output port metrics.
    const std::vector<PEOutputPortMetricsInfo>& getOutputPortMetrics() const;

    /// Print port metrics
    /// @param s ostream
    /// @param how many spaces to indent.
    virtual void print(std::ostream& s, unsigned indent = 0);

    virtual ~PEMetricsInfo(){};

  protected:
    SAM_NAMESPACE::PEID _id;
    SAM_NAMESPACE::ID _index;
    uint64_t _timestamp;
    std::vector<int64_t> _metrics;
    std::vector<OperatorMetricsInfo> _operatorMetrics;
    std::vector<PortMetricsInfo> _inputPortMetrics;
    std::vector<PEOutputPortMetricsInfo> _outputPortMetrics;
    volatile bool _reset;
};

}

#endif
