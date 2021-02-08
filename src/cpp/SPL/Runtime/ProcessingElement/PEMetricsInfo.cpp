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

#include <SPL/Runtime/ProcessingElement/PEMetricsInfo.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/SupportFunctions.h>

using namespace std;
SAM_NAMESPACE_USE
UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace SPL;

typedef vector<int64_t>::const_iterator ConstInt64It;
typedef vector<BaseMetricInfo>::const_iterator ConstBaseMetricInfoIt;
typedef vector<PortMetricsInfo>::const_iterator ConstPortMetricsInfoIt;
typedef vector<OperatorMetricsInfo>::const_iterator ConstOperMetricsInfoIt;
typedef map<IDPair, vector<int64_t> >::const_iterator ConstConnectionMetricsIt;
typedef vector<PEOutputPortMetricsInfo>::const_iterator ConstPEOutputPortMetricsInfoIt;

BaseMetricInfo::BaseMetricInfo()
  : _value(-1)
{}

BaseMetricInfo::BaseMetricInfo(const string& name, const int64_t value)
  : _name(name)
  , _value(value)
{}

void BaseMetricInfo::setValue(const int64_t value)
{
    _value = value;
}

void BaseMetricInfo::setName(const string& name)
{
    _name = name;
}

const string BaseMetricInfo::getName() const
{
    return _name;
}

const int64_t BaseMetricInfo::getValue() const
{
    return _value;
}

void BaseMetricInfo::print(std::ostream& s) const
{
    s << _name << "=" << _value;
}

PortMetricsInfo::PortMetricsInfo()
  : _index(-1)
{}

PortMetricsInfo::PortMetricsInfo(const uint32_t ind)
  : _index(ind)
{}

void PortMetricsInfo::setMetrics(const vector<int64_t>& metrics)
{
    _metrics.clear();
    for (ConstInt64It it = metrics.begin(); it != metrics.end(); ++it) {
        _metrics.push_back(*it);
    }
}

void PortMetricsInfo::addMetrics(const int64_t metric)
{
    _metrics.push_back(metric);
}

void PortMetricsInfo::setIndex(const uint32_t ind)
{
    _index = ind;
}

const uint32_t PortMetricsInfo::getIndex() const
{
    return _index;
}

const vector<int64_t>& PortMetricsInfo::getMetrics() const
{
    return _metrics;
}

void PortMetricsInfo::print(ostream& s, unsigned indent)
{
    string curIndentSpaces = indentedSpaces(indent);
    string subIndentSpaces = indentedSpaces(indent + 1);

    s << curIndentSpaces << "PortMetricsInfo: portIndex=" << _index << "\n" << subIndentSpaces;
    for (ConstInt64It it = _metrics.begin(); it != _metrics.end(); ++it) {
        s << *it << " ";
    }
}

PEOutputPortMetricsInfo::PEOutputPortMetricsInfo()
  : PortMetricsInfo()
{}

PEOutputPortMetricsInfo::PEOutputPortMetricsInfo(PortMetricsInfo info)
  : PortMetricsInfo(info)
{}

void PEOutputPortMetricsInfo::setConnectionMetrics(const map<IDPair, vector<int64_t> >& metrics)
{
    _connectionMetrics.clear();
    _connectionMetrics.insert(metrics.begin(), metrics.end());
}

void PEOutputPortMetricsInfo::addConnectionMetric(const PEID peID,
                                                  const ID portIndex,
                                                  const vector<int64_t>& values)
{
    IDPair pair(peID, portIndex);
    _connectionMetrics[pair] = values;
}

const map<IDPair, vector<int64_t> >& PEOutputPortMetricsInfo::getConnectionMetrics() const
{
    return _connectionMetrics;
}

void PEOutputPortMetricsInfo::print(ostream& s, unsigned indent)
{
    string curIndentSpaces = indentedSpaces(indent);
    string subIndentSpaces = indentedSpaces(indent + 1);

    s << curIndentSpaces << "PEOutputPortMetrics: portIndex=" << _index;

    // Metrics
    s << "\n" << subIndentSpaces << "Metrics: ";
    for (ConstInt64It it = _metrics.begin(); it != _metrics.end(); ++it) {
        s << *it << " ";
    }

    // ConstConnectionMetricsIt
    s << "\n" << subIndentSpaces << "Connection Metrics: ";
    for (ConstConnectionMetricsIt it = _connectionMetrics.begin(); it != _connectionMetrics.end();
         ++it) {
        s << "ipPEID=" << (*it).first.first << ",ipPortIndex=" << (*it).first.second
          << ",value=" << join((*it).second.begin(), (*it).second.end()) << " ";
    }
}

OperatorMetricsInfo::OperatorMetricsInfo()
  : _index(-1)
{}

OperatorMetricsInfo::OperatorMetricsInfo(const uint64_t ind,
                                         const std::string& name,
                                         const std::string& logicalName)
  : _index(ind)
  , _name(name)
  , _logicalName(logicalName)
{}

void OperatorMetricsInfo::setSysMetrics(const vector<int64_t>& metrics)
{
    _sysMetrics.clear();
    for (ConstInt64It it = metrics.begin(); it != metrics.end(); ++it) {
        _sysMetrics.push_back(*it);
    }
}

void OperatorMetricsInfo::addSysMetrics(const int64_t metric)
{
    _sysMetrics.push_back(metric);
    SPCDBG(L_TRACE, "Added metric " << metric, PEC_GET_METRICS);
}

void OperatorMetricsInfo::setMetrics(const vector<BaseMetricInfo>& metrics)
{
    _metrics.clear();
    for (ConstBaseMetricInfoIt it = metrics.begin(); it != metrics.end(); ++it) {
        _metrics.push_back(*it);
    }
}

void OperatorMetricsInfo::addMetrics(const string& name, const int64_t metric)
{
    BaseMetricInfo bmi(name, metric);
    _metrics.push_back(bmi);
}

void OperatorMetricsInfo::setInputPortMetrics(const vector<PortMetricsInfo>& ipms)
{
    _inputPortMetrics.clear();
    for (ConstPortMetricsInfoIt it = ipms.begin(); it != ipms.end(); ++it) {
        _inputPortMetrics.push_back(*it);
    }
}

void OperatorMetricsInfo::addInputPortMetrics(const PortMetricsInfo& ipm)
{
    _inputPortMetrics.push_back(ipm);
}

void OperatorMetricsInfo::setOutputPortMetrics(const vector<PortMetricsInfo>& opms)
{
    _outputPortMetrics.clear();
    for (ConstPortMetricsInfoIt it = opms.begin(); it != opms.end(); ++it) {
        _outputPortMetrics.push_back(*it);
    }
}

void OperatorMetricsInfo::addOutputPortMetrics(const PortMetricsInfo& opm)
{
    _outputPortMetrics.push_back(opm);
}

void OperatorMetricsInfo::setIndex(const uint64_t ind)
{
    _index = ind;
}

const uint64_t OperatorMetricsInfo::getIndex() const
{
    return _index;
}

const std::string OperatorMetricsInfo::getName() const
{
    return _name;
}

const std::string OperatorMetricsInfo::getLogicalName() const
{
    return _logicalName;
}

const vector<int64_t>& OperatorMetricsInfo::getSysMetrics() const
{
    return _sysMetrics;
}

const vector<BaseMetricInfo>& OperatorMetricsInfo::getMetrics() const
{
    return _metrics;
}

vector<PortMetricsInfo>& OperatorMetricsInfo::getInputPortMetrics()
{
    return _inputPortMetrics;
}

const vector<PortMetricsInfo>& OperatorMetricsInfo::getInputPortMetrics() const
{
    return _inputPortMetrics;
}

vector<PortMetricsInfo>& OperatorMetricsInfo::getOutputPortMetrics()
{
    return _outputPortMetrics;
}

const vector<PortMetricsInfo>& OperatorMetricsInfo::getOutputPortMetrics() const
{
    return _outputPortMetrics;
}

void OperatorMetricsInfo::print(ostream& s, unsigned indent)
{
    string curIndentSpaces = indentedSpaces(indent);
    string subIndentSpaces = indentedSpaces(indent + 1);

    s << curIndentSpaces << "OperatorMetricsInfo: index=" << _index;

    // Metrics
    s << "\n" << subIndentSpaces << "Operator metrics: ";
    if (_sysMetrics.size() > 0) {
        for (unsigned int i = 0; i < _sysMetrics.size(); i++) {
            s << _sysMetrics[i] << " ";
        }
    }
    for (ConstBaseMetricInfoIt it1 = _metrics.begin(); it1 != _metrics.end(); ++it1) {
        stringstream ss;
        BaseMetricInfo bm = *it1;
        bm.print(ss);
        s << ss.str() << " ";
    }

    // Input port metrics
    s << "\n" << subIndentSpaces << "Operator input port metrics: ";
    for (ConstPortMetricsInfoIt it2 = _inputPortMetrics.begin(); it2 != _inputPortMetrics.end();
         ++it2) {
        stringstream ss;
        PortMetricsInfo pm = *it2;
        pm.print(ss, indent + 2);
        s << "\n" << ss.str();
    }

    // Output port metrics
    s << "\n" << subIndentSpaces << "Operator output port metrics: ";
    for (ConstPortMetricsInfoIt it3 = _outputPortMetrics.begin(); it3 != _outputPortMetrics.end();
         ++it3) {
        stringstream ss;
        PortMetricsInfo pm = *it3;
        pm.print(ss, indent + 2);
        s << "\n" << ss.str();
    }
}

PEMetricsInfo::PEMetricsInfo()
  : _id(-1)
  , _index(-1)
  , _timestamp(0)
  , _reset(false)
{}

PEMetricsInfo::PEMetricsInfo(const PEID& id)
  : _id(id)
  , _index(-1)
  , _timestamp(0)
  , _reset(false)
{}

void PEMetricsInfo::reset()
{
    _index = -1;
    _timestamp = 0;
    _metrics.clear();
    _operatorMetrics.clear();
    _inputPortMetrics.clear();
    _outputPortMetrics.clear();
    _reset = true;
}

bool PEMetricsInfo::isReset()
{
    return _reset;
}

void PEMetricsInfo::doneSet()
{
    _reset = false;
}

void PEMetricsInfo::setMetrics(const vector<int64_t>& metrics)
{
    _metrics.clear();
    for (ConstInt64It it = metrics.begin(); it != metrics.end(); ++it) {
        _metrics.push_back(*it);
    }
}

void PEMetricsInfo::addMetrics(const int64_t metric)
{
    _metrics.push_back(metric);
    SPCDBG(L_DEBUG, "Added metric " << metric, PEC_GET_METRICS);
}

void PEMetricsInfo::setOperatorMetrics(const vector<OperatorMetricsInfo>& oms)
{
    _operatorMetrics.clear();
    for (ConstOperMetricsInfoIt it = oms.begin(); it != oms.end(); ++it) {
        _operatorMetrics.push_back(*it);
    }
}

void PEMetricsInfo::addOperatorMetrics(const OperatorMetricsInfo& om)
{
    _operatorMetrics.push_back(om);
}

void PEMetricsInfo::setInputPortMetrics(const vector<PortMetricsInfo>& ipms)
{
    _inputPortMetrics.clear();
    for (ConstPortMetricsInfoIt it = ipms.begin(); it != ipms.end(); ++it) {
        _inputPortMetrics.push_back(*it);
    }
}

void PEMetricsInfo::addInputPortMetrics(const PortMetricsInfo& ipm)
{
    _inputPortMetrics.push_back(ipm);
}

void PEMetricsInfo::setOutputPortMetrics(const vector<PEOutputPortMetricsInfo>& opms)
{
    _outputPortMetrics.clear();
    for (ConstPEOutputPortMetricsInfoIt it = opms.begin(); it != opms.end(); ++it) {
        _outputPortMetrics.push_back(*it);
    }
}

void PEMetricsInfo::addOutputPortMetrics(const PEOutputPortMetricsInfo& opm)
{
    _outputPortMetrics.push_back(opm);
}

void PEMetricsInfo::setID(const PEID& id)
{
    _id = id;
}

const PEID PEMetricsInfo::getID() const
{
    return _id;
}

const ID PEMetricsInfo::getIndex() const
{
    return _index;
}

void PEMetricsInfo::setIndex(const ID& id)
{
    _index = id;
}

void PEMetricsInfo::setTimestamp(const uint64_t timestamp)
{
    _timestamp = timestamp;
}

const uint64_t PEMetricsInfo::getTimestamp() const
{
    return _timestamp;
}

const vector<int64_t>& PEMetricsInfo::getMetrics() const
{
    return _metrics;
}

vector<OperatorMetricsInfo>& PEMetricsInfo::getOperatorMetrics()
{
    return _operatorMetrics;
}

const vector<OperatorMetricsInfo>& PEMetricsInfo::getOperatorMetrics() const
{
    return _operatorMetrics;
}

vector<PortMetricsInfo>& PEMetricsInfo::getInputPortMetrics()
{
    return _inputPortMetrics;
}

const vector<PortMetricsInfo>& PEMetricsInfo::getInputPortMetrics() const
{
    return _inputPortMetrics;
}

vector<PEOutputPortMetricsInfo>& PEMetricsInfo::getOutputPortMetrics()
{
    return _outputPortMetrics;
}

const vector<PEOutputPortMetricsInfo>& PEMetricsInfo::getOutputPortMetrics() const
{
    return _outputPortMetrics;
}

void PEMetricsInfo::print(ostream& s, unsigned indent)
{
    string curIndentSpaces = indentedSpaces(indent);
    string subIndentSpaces = indentedSpaces(indent + 1);

    s << curIndentSpaces << "PEMetricsInfo: PEID=" << _id;

    // Metrics
    s << "\n" << subIndentSpaces << "PE metrics: ";
    for (ConstInt64It it1 = _metrics.begin(); it1 != _metrics.end(); ++it1) {
        s << *it1 << " ";
    }

    // operator metrics
    s << "\n" << subIndentSpaces << "PE operator metrics: ";
    for (ConstOperMetricsInfoIt it2 = _operatorMetrics.begin(); it2 != _operatorMetrics.end();
         ++it2) {
        stringstream ss;
        OperatorMetricsInfo pm = *it2;
        pm.print(ss, indent + 2);
        s << "\n" << ss.str();
    }

    // Input port metrics
    s << "\n" << subIndentSpaces << "PE input port metrics: ";
    for (ConstPortMetricsInfoIt it3 = _inputPortMetrics.begin(); it3 != _inputPortMetrics.end();
         ++it3) {
        stringstream ss;
        PortMetricsInfo pm = *it3;
        pm.print(ss, indent + 2);
        s << "\n" << ss.str();
    }

    // Output port metrics
    s << "\n" << subIndentSpaces << "PE output port metrics: ";
    for (ConstPEOutputPortMetricsInfoIt it4 = _outputPortMetrics.begin();
         it4 != _outputPortMetrics.end(); ++it4) {
        stringstream ss;
        PEOutputPortMetricsInfo pm = *it4;
        pm.print(ss, indent + 2);
        s << "\n" << ss.str();
    }
    s << "\n";
}
