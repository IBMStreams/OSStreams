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

#ifndef SPL_PROCESS_LAUNCHER_H
#define SPL_PROCESS_LAUNCHER_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <ostream>
#include <vector>

namespace SPL {

//! Helper class to launch external processes
class DLL_PUBLIC ProcessLauncher
{
  public:
    /// Execute a process
    /// @param name path to the executable, could be partial, will be
    /// resolved using PATH
    /// @param args command line arguments of the process
    /// @param silenceStdOut supress standard output
    /// @param silenceStdErr supress standard error
    /// @param stdOutResults pointer to the ostream object that will hold the
    /// standard output from the process, NULL by default
    /// @param stdErrResults pointer to the ostream object that will hold the
    /// standard error from the process, NULL by default
    /// @return return code of the process
    /// @throws CompilerException
    static int runProcess(std::string const& name,
                          std::vector<std::string> const& args,
                          bool silenceStdOut = false,
                          bool silenceStdErr = false,
                          std::ostream* stdOutResults = NULL,
                          std::ostream* stdErrResults = NULL);

  private:
    static int run(std::string const& name,
                   std::vector<std::string> const& args,
                   bool silenceStdOut,
                   bool silenceStdErr,
                   std::ostream* stdOutResults,
                   std::ostream* stdErrResults);
};
};

#endif /* SPL_PROCESS_LAUNCHER_H */
