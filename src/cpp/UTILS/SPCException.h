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
//       SPCExceptions.h
//
// Purpose-
//       Streams API types.
//
// Change date-
//       2004/11/11
//
//----------------------------------------------------------------------------
#ifndef _UTILSEXCEPTIONS_H_
#define _UTILSEXCEPTIONS_H_

#include <UTILS/DistilleryException.h>
#include <UTILS/SPCBaseTypes.h>
#include <UTILS/UTILSTypes.h>
#include <map>

SPC_NAMESPACE_BEGIN

DECL_EXCEPTION(SPC_NAMESPACE, SPC, UTILS_NAMESPACE::Distillery);
DECL_EXCEPTION(SPC_NAMESPACE, NC, SPC);
DECL_EXCEPTION(SPC_NAMESPACE, InvalidPEID, SPC);
DECL_EXCEPTION(SPC_NAMESPACE, InvalidPortID, SPC);
DECL_EXCEPTION(SPC_NAMESPACE, ParseError, SPC);
DECL_EXCEPTION(SPC_NAMESPACE, InternalError, SPC);
DECL_EXCEPTION(SPC_NAMESPACE, ApplicationBundleFailure, SPC);

SPC_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
