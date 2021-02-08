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
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/TransportStatModelImpl.h>
#include <SPL/Core/TransportStats.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/SupportFunctions.h>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <fstream>
#include <iostream>

using namespace SPL;
using namespace std;
using namespace SPL::TransportStat;
namespace bf = boost::filesystem;

TransportStats::Data TransportStats::estimate(uint32_t size) const
{
    Data val(size, 0, 0, 0);
    vector<Data>::const_iterator it_lower =
      lower_bound(_data.begin(), _data.end(), val, DataSorter());
    if (it_lower->getTupleSize() == size)
        return *it_lower; // exact match
    int index = it_lower - _data.begin();

    const Data* d = NULL;
    uint32_t maxElem = _data.size();
    if (size < _data[0].getTupleSize()) {
        // smaller than first index
        d = &_data[0];
    } else if (size > _data[maxElem - 1].getTupleSize()) {
        // bigger than last index
        d = &_data[maxElem - 1];
    }
    if (d) // handle boundary cases
        return Data(size, d->getMaxRate(), d->getSenderCPUPercent(), d->getReceiverCPUPercent());

    // We are between index-1 and index
    // How far off are we?
    double ratio = double(size - _data[index - 1].getTupleSize()) /
                   double(_data[index].getTupleSize() - _data[index - 1].getTupleSize());

    uint32_t mr =
      _data[index - 1].getMaxRate() +
      int32_t(ratio * int32_t(_data[index].getMaxRate() - _data[index - 1].getMaxRate()));
    uint32_t perSrc = _data[index - 1].getSenderCPUPercent() +
                      int32_t(ratio * int32_t(_data[index].getSenderCPUPercent() -
                                              _data[index - 1].getSenderCPUPercent()));
    uint32_t perRcv = _data[index - 1].getReceiverCPUPercent() +
                      int32_t(ratio * int32_t(_data[index].getReceiverCPUPercent() -
                                              _data[index - 1].getReceiverCPUPercent()));
    return Data(size, mr, perSrc, perRcv);
}

ostream& TransportStats::print(ostream& str) const
{
    str << "Transport Stats" << endl;
    str << "BogoMips:\t" << _bogomips << endl;

    for (uint32_t i = 0; i < _data.size(); i++) {
        str << "Node " << i << ": ";
        _data[i].print(str);
    }
    return str;
}

bool TransportStats::foundTransportStatFile(boost::filesystem::path& stat_path)
{

    if (indebug(Debug::ReadSampleTransportStat)) { // in case we wamt old behavior
        stat_path /= "transportStatModelSample.xml";
        return exists(stat_path);
    }

    Config::TransportKind defTransport = CompilerApp::getSPLCompiler().getDefaultTransport();
    switch (defTransport) {
        case Config::TCP:
            stat_path /= "transportStatModelFor_tcp.xml";
            break;
        default:
            assert(false);
    }

    return exists(stat_path);
}

bool TransportStats::loadStats()
{
    // Load stat/transport-performance data
    // ensure that we are starting fresh
    _data.clear();

    // Get default global dir
    CompilerConfiguration const& config = CompilerConfiguration::instance();
    std::string defaultGlobalTransportStatDirName =
      config.getStringValue(CompilerConfiguration::StatsDir);
    std::string outDir = config.getStringValue(CompilerConfiguration::OutputDir);

    // Get install global dir
    std::string globalInstallTransportStatDirName =
      Distillery::getInstanceTemplateConfigurationDirectory("", false);

    SPLDBG("default global transport stat dir: " << defaultGlobalTransportStatDirName.c_str(),
           Debug::TraceTransportStatRead);
    SPLDBG("global install transport stat dir: " << globalInstallTransportStatDirName.c_str(),
           Debug::TraceTransportStatRead);

    std::string localTransportStatDirName;

    if (config.isSet(CompilerConfiguration::TransportStatDir))
        localTransportStatDirName = config.getStringValue(CompilerConfiguration::TransportStatDir);
    else {
        // use default local dir
        bf::path p(outDir + "/etc/stat");
        p = bf::system_complete(p);
        localTransportStatDirName = p.string();
    }

    boost::filesystem::path stat_path;

    SPLDBG("Checking for transport file in (local) dir: " << localTransportStatDirName.c_str(),
           Debug::TraceTransportStatRead);

    // first search in local or specified dir for transport stat file
    stat_path = localTransportStatDirName;
    if (!foundTransportStatFile(stat_path)) {
        // if not found, then search in global dir
        SPLDBG("Checking for transport file in (install) dir: "
                 << globalInstallTransportStatDirName.c_str(),
               Debug::TraceTransportStatRead);
        stat_path = globalInstallTransportStatDirName;
        if (!foundTransportStatFile(stat_path)) {
            SPLDBG("Checking for transport file in (global) dir: "
                     << defaultGlobalTransportStatDirName.c_str(),
                   Debug::TraceTransportStatRead);
            stat_path = defaultGlobalTransportStatDirName;
            if (!foundTransportStatFile(stat_path))
                SysOut::die(SPL_CORE_TRANPORT_STAT_FILE_NOT_FOUND(stat_path.string()));
        }
    }

    SPLDBG("Found transport Stat file here: " << stat_path.string().c_str(),
           Debug::TraceTransportStatRead);

    ifstream transportStatStr(stat_path.string().c_str(), fstream::in);
    assert(transportStatStr);

    SPLDBG("After opening xml file" << stat_path.string().c_str(), Debug::TraceTransportStatRead);

    std::auto_ptr<xmlns::prod::streams::spl::transportStat::transportStatModelType> xmlStatModel;
    std::auto_ptr<TransportStatModel> statModel;
    try {
        xmlStatModel = transportStatModel(transportStatStr, xsd::cxx::tree::flags::dont_validate);
        SPLDBG("After reading model", Debug::TraceTransportStatRead);
        statModel.reset(new TransportStat::TransportStatModel(*xmlStatModel));
    } catch (::xsd::cxx::exception& e) {
        SysOut::errorln(
          SPL_CORE_UNABLE_TO_LOAD_TRANSPORT_STAT_INFO_XML(stat_path.string(), e.what()));
    }

    // FIXME: probably should check for product version in XML file

    _bogomips = statModel->getBogoMips();

    const std::vector<TransportProfilePtr>& transportProfiles = statModel->getTransportProfiles();

    std::vector<TransportProfilePtr>::const_iterator tpIter;
    tpIter = transportProfiles.begin();

    assert(tpIter != transportProfiles.end());

    const std::vector<TuplePerfPtr>& tuplePerfs = (*tpIter)->getTuplePerfs();

    std::vector<TuplePerfPtr>::const_iterator tupleIter, tupleIterEnd;
    tupleIterEnd = tuplePerfs.end();
    for (tupleIter = tuplePerfs.begin(); tupleIter != tupleIterEnd; ++tupleIter) {
        uint32_t ts, mr, perSend, perRec;
        ts = (*tupleIter)->getSize();
        mr = (*tupleIter)->getRate();
        perSend = (*tupleIter)->getSenderUtil();
        perRec = (*tupleIter)->getReceiverUtil();
        _data.push_back(Data(ts, mr, perSend, perRec));
    }

    sort(_data.begin(), _data.end(), DataSorter());

    if (indebug(Debug::DumpTransportStatData) || indebug(Debug::TraceTransportStatRead))
        print(cout);
    return true;
}

ostream& TransportStats::Data::print(ostream& str) const
{
    str << "Size: " << _tupleSize << ", Rate: " << _maxRate << ", Sender%: " << _percentCPUSender
        << ", Receiver%: " << _percentCPUReceiver << endl;
    return str;
}
