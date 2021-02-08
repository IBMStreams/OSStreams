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

#ifndef TRCTYPES_H
#define TRCTYPES_H

#include <UTILS/UTILSTypes.h>

#define DEBUG_NAMESPACE debug
#define DEBUG_NAMESPACE_BEGIN namespace DEBUG_NAMESPACE {
#define DEBUG_NAMESPACE_END }
#define DEBUG_NAMESPACE_QUALIFIER UTILS_NAMESPACE_QUALIFIER DEBUG_NAMESPACE::
#define DEBUG_NAMESPACE_USE using namespace UTILS_NAMESPACE_QUALIFIER DEBUG_NAMESPACE;

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

/// These levels are for IBM Streams debugging internal processing.
/// Please never expose these levels to end user interface!!!
/// The external ones that visible to end user are defined with DistilleryDebug.h
/// (L_ERROR, L_INFO, ..., L_TRACE, etc)

/// Because of the history of Streams and backwards compatibility,
/// we have to keep these external level value unchanged. But as we enhance
/// Streams Log/Trace support and add new levels, we need to update the level
/// sequence somehow.
/// In order to fix this issue, the idea here is that, we maintain these internal
/// levels and update them as needed, but keep all external levels unchanged
/// (once defined), and create an internal method to do the map between
/// internal levels and external levels.

/// Since DistilleryApplication using -D...D for trace level, following levels
/// have to be sequential increased. And also, these levels have to match with
/// those from spcrun-remote.pl.

#define iL_OFF 0
#define iL_ERROR 1
#define iL_WARN 2
#define iL_INFO 3
#define iL_DEBUG 4
#define iL_TRACE 5

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif
