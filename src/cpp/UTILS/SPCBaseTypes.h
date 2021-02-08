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

#ifndef _SPCBASETYPES_H_
#define _SPCBASETYPES_H_

#include <stdint.h> // We use Linux's integer types
#include <sys/types.h>

#include <UTILS/UTILSTypes.h>

#ifdef __cplusplus

#include <cassert>
#include <iterator>
#include <string>
#include <vector>

#include <SAM/SAMNamespaceDefs.h>
#include <UTILS/HashMapHelpers.h>

SPC_NAMESPACE_BEGIN

/// InfoSphere Streams job management identifiers
typedef uint64_t ID;

/// The system-assigned ID for a PE
typedef ID PEID;
/// The system-assigned ID for a stream
typedef ID StreamID;
/// The system-assigned ID for a job
typedef ID JobID;
/// The system-assigned ID for a port
typedef ID PortID;
typedef std::string Name;

typedef std::vector<std::string>::const_iterator ConstStringIterator;
typedef std::vector<std::string>::iterator StringIterator;

SPC_NAMESPACE_END
#endif
#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
