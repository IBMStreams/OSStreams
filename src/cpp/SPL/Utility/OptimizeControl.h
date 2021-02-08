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

#ifndef SPL_OPTIMIZECONTROL_H
#define SPL_OPTIMIZECONTROL_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/HashMapHelpers.h>

#include <sstream>
#include <string>

namespace SPL {

class OptimizeControl : public SPL::Singleton<OptimizeControl>
{
  public:
    OptimizeControl();

    /// Enum DebugFlag
    /// Specify some information to be dumped during the compile
    enum OptimizeControlFlag
    {
        NoBackwardStoreMotion,      /*!< don't move stores out of loops */
        NoExpandAssignFrom,         /*!< don't expand assignFrom function call */
        NoOptimizeFunction,         /*!< don't optimize SPL functions */
        NoOptimizeImmutableStreams, /*!< don't optimize immutable streams */
        NoOptimizeMappedOp,         /*!< don't optimize mapped operators */
        NoOptimizeStatement,        /*!< don't optimize SPL statements */
        NoSplitMappedOp, /*!< don't split mapped op expressions into multiple SPL statements */
        NumOptimizeControlFlags
        //*** NOTE:  IF YOU ADD AN ENUMERATOR TO THIS LIST YOU MUST ALSO UPDATE OptimizeControl.cpp
        //*******
    };

    bool checkOptimizeControl(OptimizeControlFlag flag) const { return _flags[flag]; }
    void addOptimizeControlString(std::string const& str);

  private:
    void addString(std::string const& str);
    bool _flags[NumOptimizeControlFlags];
};

inline bool optimizeControlCheck(OptimizeControl::OptimizeControlFlag flag)
{
    return OptimizeControl::instance().checkOptimizeControl(flag);
}

} // namespace SPL

#endif // SPL_OPTIMIZECONTROL_H
