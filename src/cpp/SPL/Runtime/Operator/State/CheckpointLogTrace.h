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

/*
 * \file CheckpointLogTrace.h \brief Definition of Macros for logging and tracing in checkpointing
 * API
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_LOG_TRACE_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_LOG_TRAXE_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Utility/FormattableMessage.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <TRC/DistilleryDebug.h>
#include <assert.h>
#include <stdint.h>
#include <string>

namespace SPL {
/// Generate aspect for a checkpointing/restore related log message
/// @param opName operator name
/// @return aspect in the form of "#splapplog,opName,spl_ckpt"
std::string splCkptLogAspect(const std::string& opName) DLL_PUBLIC;

/// Generate aspect for a checkpointing/restore related trace message
/// @param opName operator name
/// @return aspect in the form of "#splapptrc,opName,spl_ckpt"
std::string splCkptTrcAspect(const std::string& opName) DLL_PUBLIC;

/// Record a FormattableMessage in log
/// @param ilvl log level
/// @param msg a FormattableMessage
/// @param aspect aspect of message
/// @param function function name
/// @param file file name
/// @param line line number
void splCkptLogMessage(int ilvl,
                       const FormattableMessage& msg,
                       const std::string& aspect,
                       const std::string& function,
                       const std::string& file,
                       uint32_t line) DLL_PUBLIC;

/// Record a FormattableMessage in trace
/// @param ilvl trace level
/// @param msg a FormattableMessage
/// @param aspect aspect of message
/// @param function function name
/// @param file file name
/// @param line line number
void splCkptTraceMessage(int ilvl,
                         const FormattableMessage& msg,
                         const std::string& aspect,
                         const std::string& function,
                         const std::string& file,
                         uint32_t line) DLL_PUBLIC;
} // namespace SPL

/// \brief Macros for logging, tracing, and exception handling
///
/// Note that since checkpointing APIs may be called by non-operator-threads, we cannot use
/// SPLAPPLOG, SPLAPPTRC, SPLLOGMSG, SPLTRACEMSG, or SPLTRACEMSGANDTHROW which retrieve
/// operator names from operator tracker
#define SPLCKPTLOG(lvl, opName, msg) SPCLOG_NO_BUNDLE(lvl, msg, SPL::splCkptLogAspect(opName))

#define SPLCKPTTRC(lvl, opName, msg) APPTRC(lvl, msg, SPL::splCkptTrcAspect(opName))

#define SPLCKPTLOGMSG(lvl, opName, error, explanation)                                             \
    do {                                                                                           \
        assert(lvl <= L_WARN);                                                                     \
        assert(lvl >= L_ERROR);                                                                    \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl];                   \
        if (__builtin_expect((ilvl <= Distillery::debug::logger_level), 0)) {                      \
            const FormattableMessage& fm = SPL_RUNTIME_CKPT_ERROR(opName, error, explanation);     \
            SPL::splCkptLogMessage(ilvl, fm, SPL::splCkptLogAspect(opName), __FUNCTION__,          \
                                   __FILE__, __LINE__);                                            \
        }                                                                                          \
    } while (0)

#define SPLCKPTTRCMSG(lvl, opName, error, explanation)                                             \
    do {                                                                                           \
        assert(lvl <= L_WARN);                                                                     \
        assert(lvl >= L_ERROR);                                                                    \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl];                   \
        if (__builtin_expect((ilvl <= Distillery::debug::app_trace_level), 0)) {                   \
            const FormattableMessage& fm = SPL_RUNTIME_CKPT_ERROR(opName, error, explanation);     \
            SPL::splCkptTraceMessage(ilvl, fm, SPL::splCkptTrcAspect(opName), __FUNCTION__,        \
                                     __FILE__, __LINE__);                                          \
        }                                                                                          \
    } while (0)

#define SPLCKPT_HANDLE_EXCEPTION(opName, error, explanation)                                       \
    do {                                                                                           \
        const FormattableMessage& fm = SPL_RUNTIME_CKPT_ERROR(opName, error, explanation);         \
        std::string msg = fm.key();                                                                \
        msg += ": ";                                                                               \
        msg += (std::string)fm;                                                                    \
        SPLCKPTLOG(L_ERROR, opName, msg);                                                          \
        SPLCKPTTRC(L_ERROR, opName, msg);                                                          \
        THROW_STRING(DataStore, fm);                                                               \
    } while (0)

#define SPLCKPT_HANDLE_EXCEPTION_NESTED(opName, error, explanation, exception)                     \
    do {                                                                                           \
        const FormattableMessage& fm = SPL_RUNTIME_CKPT_ERROR(opName, error, explanation);         \
        std::string msg = fm.key();                                                                \
        msg += ": ";                                                                               \
        msg += (std::string)fm;                                                                    \
        SPLCKPTLOG(L_ERROR, opName, msg);                                                          \
        SPLCKPTTRC(L_ERROR, opName, msg);                                                          \
        THROW_NESTED(DataStore, fm, exception);                                                    \
    } while (0)

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_LOG_TRACE_H
