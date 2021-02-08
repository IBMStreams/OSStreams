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

//----------------------------------------------------------------------------
//
// Title-
//       SPCTypes.h
//
// Purpose-
//       SPC API types.
//
//----------------------------------------------------------------------------
#ifndef _SPCTYPES_H_
#define _SPCTYPES_H_

#ifdef DEBUGLOG
#define DBG(x) x
#else
#define DBG(x)
#endif

#ifdef __cplusplus

//----------------------------------------------------------------------------
// Macros
//----------------------------------------------------------------------------

#include <SAM/SAMNamespaceDefs.h>
#include <UTILS/HashMapHelpers.h>
#include <UTILS/SPCBaseTypes.h>
#include <UTILS/SPCException.h>
#include <set>
#include <sstream>
#include <string>

SPC_NAMESPACE_BEGIN

//----------------------------------------------------------------------------
// Global typedefs
//----------------------------------------------------------------------------
// conversion from string to our IDs.
#define STR2ID(a) ((ID)strtoll((a), NULL, 0))
#define IDS2NAME(a, b, c) (a + "." + b + "." + c)

typedef std::vector<ID> IDList;
typedef std::set<ID> IDSet;
typedef std::pair<std::string, std::vector<std::string> > nameValuePair_t;

#define SHORT_ACK_TIMEOUT (3)

// Different states a PE can be in.
typedef enum
{
    unknownPe,
    peStarting,
    peRunning,
    peFinalizing
} PeStatus;

class StreamDesc;
class StreamDescCollection;
typedef std::vector<StreamDesc*>::const_iterator ConstSDIterator;
typedef std::vector<StreamDesc*>::iterator SDIterator;

inline std::string getRoutingServiceName(const PEID& peid,
                                         const std::string hostIp,
                                         bool isTestMode = false)
{
    std::stringstream rcsname;
    if (!isTestMode) {
        rcsname << "pec_" << hostIp << "_" << peid;
    } else {
        rcsname << "rcs_" << peid;
    }
    return rcsname.str();
}

SPC_NAMESPACE_END

#endif

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
