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

#ifndef SPL_RUNTIME_UTILITY_TRACEMESSAGE
#define SPL_RUNTIME_UTILITY_TRACEMESSAGE

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/FormattableMessage.h>
#include <TRC/DistilleryDebug.h>
#include <string>

namespace SPL {
void splLogMessage(int ilvl,
                   const FormattableMessage& msg,
                   const std::string& aspect,
                   const std::string& function,
                   const std::string& file,
                   uint32_t line) DLL_PUBLIC;
void splTraceMessage(int ilvl,
                     const FormattableMessage& msg,
                     const std::string& aspect,
                     const std::string& function,
                     const std::string& file,
                     uint32_t line) DLL_PUBLIC;
}

#define SPLTRACEMSG(lvl, msg, aspect)                                                              \
    do {                                                                                           \
        assert(lvl <= L_WARN);                                                                     \
        assert(lvl >= L_ERROR);                                                                    \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl];                   \
        if (__builtin_expect((ilvl <= Distillery::debug::app_trace_level), 0)) {                   \
            const FormattableMessage& m = msg;                                                     \
            splTraceMessage(ilvl, m, aspect, __FUNCTION__, __FILE__, __LINE__);                    \
        }                                                                                          \
    } while (0)

#define SPLTRACEMSGANDTHROW(type, lvl, msg, aspect)                                                \
    do {                                                                                           \
        assert(lvl <= L_WARN);                                                                     \
        assert(lvl >= L_ERROR);                                                                    \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl];                   \
        const FormattableMessage& m = msg;                                                         \
        if (__builtin_expect((ilvl <= Distillery::debug::app_trace_level), 0)) {                   \
            splTraceMessage(ilvl, m, aspect, __FUNCTION__, __FILE__, __LINE__);                    \
        }                                                                                          \
        THROW_STRING(type, m);                                                                     \
    } while (0)

#define SPLTRACEMSGANDTHROWNESTED(type, ex, lvl, msg, aspect)                                      \
    do {                                                                                           \
        assert(lvl <= L_WARN);                                                                     \
        assert(lvl >= L_ERROR);                                                                    \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl];                   \
        const FormattableMessage& m = msg;                                                         \
        if (__builtin_expect((ilvl <= Distillery::debug::app_trace_level), 0)) {                   \
            splTraceMessage(ilvl, m, aspect, __FUNCTION__, __FILE__, __LINE__);                    \
        }                                                                                          \
        THROW_NESTED(type, m, ex);                                                                 \
    } while (0)

#define SPLLOGMSG(lvl, msg, aspect)                                                                \
    do {                                                                                           \
        assert(lvl <= L_WARN);                                                                     \
        assert(lvl >= L_ERROR);                                                                    \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl];                   \
        if (__builtin_expect((ilvl <= Distillery::debug::logger_level), 0)) {                      \
            const FormattableMessage& m = msg;                                                     \
            splLogMessage(ilvl, m, aspect, __FUNCTION__, __FILE__, __LINE__);                      \
        }                                                                                          \
    } while (0)

#endif /* DOXYGEN_SKIP_FOR_USERS */

#endif /* SPL_RUNTIME_UTILITY_TRACEMESSAGE */
