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

#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Mutex.h>
#include <UTILS/NodeCPUInfo.h>
#include <UTILS/SupportFunctions.h>
#include <cassert>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using namespace std;
UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

vector<map<string, string> > NodeCPUInfo::processors;

void NodeCPUInfo::init(void)
{
    static Mutex m;
    {
        AutoMutex amtx(m);
        ifstream in("/proc/cpuinfo");
        if (in) {
            int procn = -1;
            while (!in.eof()) {
                char buf[512];
                in.getline(buf, 512);
                if (buf[0] == '\0') {
                    continue;
                }
                vector<string> tokens;
                tokenize(buf, tokens, ":", false);
                if (tokens.size() > 2) {
                    cout << "ERROR: unexpected format '" << buf << "'" << endl;
                }
                assert(tokens.size() <= 2);
                tokens[0] = trim(tokens[0], " \t");
                if (tokens.size() == 2) {
                    tokens[1] = trim(tokens[1], " \t");
                    SPCDBG(L_DEBUG,
                           "processing /proc/cpuinfo: key: '" << tokens[0] << "' with value '"
                                                              << tokens[1] << "'",
                           CORE_UTILS);
                } else {
                    tokens.push_back(string(""));
                    SPCDBG(L_DEBUG,
                           "processing /proc/cpuinfo: key: '" << tokens[0] << "' with empty value",
                           CORE_UTILS);
                }
                if (tokens[0] == "processor") {
                    fromString<int>(procn, tokens[1]);
                    map<string, string> processor;
                    processor.insert(make_pair(tokens[0], tokens[1]));
                    processors.push_back(processor);
                } else {
                    assert(procn >= 0);
                    processors[procn].insert(make_pair(tokens[0], tokens[1]));
                }
            }
            in.close();
        } else {
            THROW(Distillery, "failed to open /proc/cpuinfo");
        }
    }
}

string NodeCPUInfo::getValueFor(const unsigned procn, const string& key)
{
    if (processors.empty()) {
        init();
    }
    if (procn > processors.size()) {
        THROW(Distillery, "processor " << procn << " does not exist (this node has "
                                       << processors.size() << " processors)");
    }
    map<string, string>::const_iterator kv = processors[procn].find(key);
    if (kv == processors[procn].end()) {
        THROW(CPUInfoKeyNotFound, "key '" + key + "' not found for processor " << procn);
    }
    return kv->second;
}

void NodeCPUInfo::print(ostream& o)
{
    if (processors.empty()) {
        init();
    }
    unsigned k = 0;
    for (vector<map<string, string> >::const_iterator i = processors.begin(); i != processors.end();
         ++i) {
        o << "Processor " << k << endl;
        for (map<string, string>::const_iterator j = i->begin(); j != i->end(); ++j) {
            o << j->first << ": " << j->second << endl;
        }
        k++;
    }
}

float NodeCPUInfo::getBogomips(const unsigned procn)
{
    if (processors.empty()) {
        init();
    }
    float toret = 0;
#if defined(__i386__)
    string bogomips(getValueFor(procn, "bogomips"));
    fromString<float>(toret, bogomips);
#endif
    return toret;
}

unsigned NodeCPUInfo::getNCPUs(void)
{
    if (processors.empty()) {
        init();
    }
    return processors.size();
}

unsigned NodeCPUInfo::getNCores(const unsigned procn)
{
    if (processors.empty()) {
        init();
    }
#if defined(__i386__)
    try {
        string cpucores(getValueFor(procn, "cpu cores"));
        unsigned toret;
        fromString<unsigned>(toret, cpucores);
        return toret;
    } catch (CPUInfoKeyNotFoundException&) {
    }
#endif
    return 1;
}

bool NodeCPUInfo::hasSSE(void)
{
    if (processors.empty()) {
        init();
    }
#if defined(__i386__)
    string flags(getValueFor(0, "flags"));
    if (flags.find("sse") != string::npos)
        return true;
#endif
    return false;
}

bool NodeCPUInfo::hasSSE2(void)
{
    if (processors.empty()) {
        init();
    }
#if defined(__i386__)
    string flags(getValueFor(0, "flags"));
    if (flags.find("sse2") != string::npos)
        return true;
#endif
    return false;
}

IMPL_EXCEPTION(Distillery, CPUInfoKeyNotFound, Utils);
