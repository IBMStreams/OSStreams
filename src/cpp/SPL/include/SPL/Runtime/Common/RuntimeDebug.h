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

#ifndef SPL_RUNTIME_COMMON_RUNTIME_DEBUG
#define SPL_RUNTIME_COMMON_RUNTIME_DEBUG

/// \file RuntimeDebug.h \brief Definition of the \ref SPLAPPLOG and \ref SPLAPPTRC macros.

#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#ifndef DOXYGEN_SKIP_FOR_USERS
#include <TRC/DistilleryDebug.h>
#include <SPL/Runtime/Utility/Visibility.h>
#endif /* DOXYGEN_SKIP_FOR_USERS */

#ifndef DOXYGEN_SKIP_FOR_USERS

    namespace SPL
    {
        std::string spllogAspect(const std::string& aspect) DLL_PUBLIC;
        std::string splAppLogAspect(const std::string& aspect) DLL_PUBLIC;
        std::string splAppTrcAspect(const std::string& aspect) DLL_PUBLIC;
    }

    #define SPLLOG(lvl, text, aspect)                       \
        APPTRC((lvl), text, SPL::spllogAspect(aspect))

    #define SPLAPPTRC(lvl, text, aspect)                       \
        APPTRC((lvl), text, SPL::splAppTrcAspect(aspect))

    #define SPLAPPLOG(lvl, text, aspect)                                   \
    do {                                                                   \
        assert(lvl <= L_WARN);                                             \
        assert(lvl >= L_ERROR);                                            \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl]; \
        if (ilvl <= Distillery::debug::logger_level) {                     \
            std::stringstream __my_strm;                                   \
             __my_strm << text;                                            \
             Distillery::debug::write_log(ilvl,                            \
                                          SPL::splAppLogAspect(aspect),    \
                                          __FUNCTION__, __FILE__, __LINE__,\
                                          __my_strm.str());                \
        }                                                                  \
    } while(0)
#else

    /// Record an application logging message
    /// @param level logging level (one of \c L_INFO, \c L_WARN, \c L_ERROR)
    /// @param text text to print as part of the message
    /// @param aspect debug aspects (string containing a comma separated list of
    /// aspects)
    ///
    /// Example usage:
    /// \code SPLAPPLOG(L_ERROR, "Total operator failure!! Restart system!", "myoper, input") \endcode
    #define SPLAPPLOG(level, text, aspect) ...

    /// Record an application tracing message
    /// @param level logging level (one of \c L_TRACE, \c L_DEBUG, \c L_INFO, \c
    /// L_WARN, \c L_ERROR)
    /// @param text text to print as part of the message
    /// @param aspect debug aspects (string containing a comma separated list of
    /// aspects)
    ///
    /// Example usage:
    /// \code SPLAPPTRC(L_INFO, "The processing of tuples on port index '" << port << "' has failed", "myoper, input") \endcode
    #define SPLAPPTRC(level, text, aspect) ...

    /// Log a trace message (Deprecated - use \ref SPLAPPTRC)
    /// @param level logging level (one of \c L_TRACE, \c L_DEBUG, \c L_INFO, \c
    /// L_ERROR)
    /// @param text text to print as part of the message
    /// @param aspect debug aspects (string containing a comma seperated list of
    /// aspects)
    ///
    /// Example usage:
    /// \code SPLLOG(L_INFO, "The processing of tuples on port index '" << port << "' has failed", "myoper, input") \endcode
    #define SPLLOG(level, text, aspect) ...

#endif /* DOXYGEN_SKIP_FOR_USERS */


#endif /* SPL_RUNTIME_COMMON_RUNTIME_DEBUG */
