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

#ifndef NODECPUINFO_H
#define NODECPUINFO_H

#include <UTILS/DistilleryException.h>
#include <UTILS/UTILSTypes.h>
#include <map>
#include <string>
#include <vector>

UTILS_NAMESPACE_BEGIN

/// This class stores node information gleaned from /cpu/info
class NodeCPUInfo
{
  public:
    static std::string getValueFor(const unsigned procn, const std::string& key);
    static unsigned getNCPUs(void);
    static unsigned getNCores(const unsigned procn = 0);
    static float getBogomips(const unsigned procn = 0);
    static bool hasSSE(void);
    static bool hasSSE2(void);
    static void print(std::ostream& o);

  private:
    static void init(void);
    static std::vector<std::map<std::string, std::string> > processors;
};

DECL_EXCEPTION(UTILS_NAMESPACE, CPUInfoKeyNotFound, Utils);

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
