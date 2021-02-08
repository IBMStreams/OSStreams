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

#include "boost/lexical_cast.hpp"
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/OperatorGraphNode.h>
#include <SPL/Core/OperatorStats.h>
#include <SPL/Runtime/operatorProfileModel.h>
#include <SPL/Utility/SysOut.h>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <fstream>

using namespace SPL;
using namespace std;
using namespace xmlns::prod::streams::spl::operator_::profile;

#define NSEC_TO_SEC(x) ((x)*1.0e-9)

OperatorStats::~OperatorStats()
{
    clean();
}

void OperatorStats::clean()
{
    for (vector<Data*>::const_iterator it = _nodes.begin(); it != _nodes.end(); ++it) {
        Data* p = *it;
        delete p;
    }
    _nodes.clear();
}

static void checkIndex(uint32_t i, uint32_t max)
{
    if (i >= max) {
        // out of range
        THROW(SPLCompilerInvalidIndex, "Invalid node index " << i << ", range is 0.." << max - 1);
    }
}

static void checkPortIndex(uint32_t i, uint32_t max)
{
    if (i >= max) {
        // out of range
        THROW(SPLCompilerInvalidIndex, "Invalid port index " << i << ", range is 0.." << max - 1);
    }
}

double OperatorStats::cpuFraction(uint32_t n) const
{
    checkIndex(n, _nodes.size());
    return _nodes[n]->_base._cpuFrac;
}

double OperatorStats::inputByteRate(uint32_t n, uint32_t p) const
{
    checkIndex(n, _nodes.size());
    Data& d = *_nodes[n];
    checkPortIndex(p, d._inputPorts.size());
    return d._inputPorts[p]._size._rate;
}

double OperatorStats::inputTupleRate(uint32_t n, uint32_t p) const
{
    checkIndex(n, _nodes.size());
    Data& d = *_nodes[n];
    checkPortIndex(p, d._inputPorts.size());
    return d._inputPorts[p]._num._rate;
}

double OperatorStats::outputByteRate(uint32_t n, uint32_t p) const
{
    checkIndex(n, _nodes.size());
    Data& d = *_nodes[n];
    checkPortIndex(p, d._outputPorts.size());
    return d._outputPorts[p]._size._rate;
}

double OperatorStats::outputTupleRate(uint32_t n, uint32_t p) const
{
    checkIndex(n, _nodes.size());
    Data& d = *_nodes[n];
    checkPortIndex(p, d._outputPorts.size());
    return d._outputPorts[p]._num._rate;
}

const OperatorStats::Data& OperatorStats::operator[](uint32_t i) const
{
    checkIndex(i, _nodes.size());
    return *_nodes[i];
}

ostream& OperatorStats::print(const OperatorGraph& graph, ostream& str) const
{
    str << "Operator Stats" << endl;
    if (_nodes.size() == 0)
        return str;

    for (uint32_t i = 0; i < _nodes.size(); i++) {
        str << "Node " << i << " (" << graph.getNode(i).getName() << "):\n";
        _nodes[i]->print(str);
    }
    return str;
}

bool OperatorStats::loadStats(const OperatorGraph& graph, const string& dir)
{
    // In directory 'dir', find the statistics for all the operators and
    // ports.  Do this for all operators in the graph

    // ensure that we are starting fresh
    clean();

    // Check that the directory exists
    namespace bf = boost::filesystem;
    bf::path file_path(dir);
    if (!exists(file_path))
        SysOut::die(SPL_CORE_PATH_DOES_NOT_EXIST(dir));
    if (!is_directory(file_path))
        SysOut::die(SPL_CORE_PATH_IS_NOT_A_DIRECTORY(dir));

    uint32_t numNodes1 = graph.numNodes();
    for (uint32_t i = 0; i < numNodes1; i++) {
        // For each operator in the graph
        const OperatorGraphNode& gn = graph.getNode(i);
        string fname = dir + '/' + gn.getName() + "_profile.xml";

        bf::path input = fname;
        if (!bf::exists(input)) {

            // Check to see if this operator was from a UDP run
            string udpFname = dir + '/' + gn.getName() + "[0]_profile.xml";
            bf::path udpInput = udpFname;
            if (bf::exists(udpInput)) {
                fname = udpFname;
            } else {
                SysOut::errorln(SPL_CORE_PATH_DOES_NOT_EXIST(fname));
                continue;
            }
        }
        auto_ptr<operatorProfileType> opProf;
        try {
            opProf = operatorProfile(fname, xsd::cxx::tree::flags::dont_validate);
        } catch (::xsd::cxx::exception& e) {
            SysOut::errorln(SPL_CORE_UNABLE_TO_LOAD_PROFILE_XML(fname, e.what()));
            continue;
        }
        // Operator base:
        Data* p = new Data;
        _nodes.push_back(p);
        p->load(*opProf.get(), _keepData);
    }
    return true;
}

void OperatorStats::Data::load(const operatorProfileType& prof, bool keepData)
{
    _base.load(prof.baseCost(), keepData);

    // compute cpuFrac based on base cost and ports
    if (_base._total._cnt)
        _base._cpuFrac = double(_base._total._sum) / double(_base._total._duration);
    else
        _base._cpuFrac = 0.0;

    // Inputs
    {
        const operatorProfileType::inputPort_sequence& inputs = prof.inputPort();
        operatorProfileType::inputPort_const_iterator it;
        for (it = inputs.begin(); it != inputs.end(); it++) {
            _inputPorts.push_back(Port());
            Port& p = _inputPorts.back();
            p.load(*it, keepData);
            _base._cpuFrac += NSEC_TO_SEC(p._time._total._avg) * p._num._rate;
        }
    }

    // Outputs
    {
        const operatorProfileType::outputPort_sequence& outputs = prof.outputPort();
        operatorProfileType::outputPort_const_iterator it;
        for (it = outputs.begin(); it != outputs.end(); it++) {
            _outputPorts.push_back(Port());
            Port& p = _outputPorts.back();
            p.load(*it, keepData);
            _base._cpuFrac -= NSEC_TO_SEC(p._time._total._avg) * p._num._rate;
        }
    }

    if (_base._cpuFrac < 0)
        _base._cpuFrac = -0.0;
}

void OperatorStats::OnePort::load(const bucketDataType& bucket, char fchar, bool keepData)
{
    Info::load(bucket, keepData);

    // Derive rate
    switch (fchar) {
        case 'n':
        case 's':
            if (_total._cnt > 0)
                _rate = _total._max / NSEC_TO_SEC(_total._duration);
            else
                _rate = 0;
            break;
        default: // case 't':   // doesn't make sense
            _rate = 0;
            break;
    }
}

void OperatorStats::Port::load(const portType& p, bool keepData)
{
    _num.load(p.number(), 'n', keepData);
    _size.load(p.size(), 's', keepData);
    _time.load(p.time(), 't', keepData);
}

void OperatorStats::Operator::load(const bucketDataType& bucket, bool keepData)
{
    Info::load(bucket, keepData);
}

OperatorStats::Info::Info()
{
    _buckets = NULL;
}

OperatorStats::Info::~Info()
{
    delete _buckets;
}

void OperatorStats::Info::load(const bucketDataType& bucket, bool keepData)
{
    _total.load(bucket.total());
    if (keepData) {
        _buckets = new std::vector<Row>;

        const bucketDataType::row_sequence& rows = bucket.row();
        bucketDataType::row_const_iterator it;
        for (it = rows.begin(); it != rows.end(); it++) {
            Row r;
            r._timeStamp = it->timestamp() - _total._initialTimestamp;
            r._value = it->value();
            _buckets->push_back(r);
        }
        sort(_buckets->begin(), _buckets->end(), RowSorter());
    }
}

void OperatorStats::Total::load(const totalType& tot)
{
    _initialTimestamp = tot.initialTimestamp();
    _finalTimestamp = tot.finalTimestamp();
    _duration = _finalTimestamp - _initialTimestamp;
    _cnt = tot.count();
    _sum = tot.sum();
    _avg = tot.average();
    _min = tot.min();
    _max = tot.max();
}

ostream& OperatorStats::Row::print(ostream& str) const
{
    str << "TimeStamp: " << _timeStamp << ", Value: " << _value << endl;
    return str;
}

ostream& OperatorStats::Total::print(ostream& str) const
{
    str << "Total Count: " << _cnt << ", Sum: " << _sum << ", Average: " << _avg
        << ", Min: " << _min << ", Max: " << _max << ", InitTS: " << _initialTimestamp
        << ", FinalTS: " << _finalTimestamp << endl;
    return str;
}

ostream& OperatorStats::Info::print(ostream& str) const
{
    if (_buckets) {
        for (uint32_t i = 0; i < _buckets->size(); i++) {
            str << "Row " << i << ": ";
            (*_buckets)[i].print(str);
        }
    }
    _total.print(str);
    return str;
}

ostream& OperatorStats::Operator::print(ostream& str) const
{
    Info::print(str);
    str << "Cpufrac: " << _cpuFrac << endl;
    return str;
}

ostream& OperatorStats::OnePort::print(ostream& str) const
{
    Info::print(str);
    str << "Rate: " << _rate << endl;
    return str;
}

ostream& OperatorStats::Port::print(ostream& str) const
{
    str << "Number of Tuples\n";
    _num.print(str);
    str << "Size of Tuples\n";
    _size.print(str);
    str << "Processing Time\n";
    _time.print(str);
    return str;
}

ostream& OperatorStats::Data::print(ostream& str) const
{
    str << "Base cost: ";
    _base.print(str);
    str << endl;
    if (_inputPorts.size() > 0) {
        for (uint32_t i = 0; i < _inputPorts.size(); i++) {
            str << "Input Port " << i << ":\n";
            _inputPorts[i].print(str);
            str << endl;
        }
    }
    if (_outputPorts.size() > 0) {
        for (uint32_t i = 0; i < _outputPorts.size(); i++) {
            str << "Output Port " << i << ":\n";
            _outputPorts[i].print(str);
            str << endl;
        }
    }
    return str;
}
