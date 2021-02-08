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

#ifndef SPL_TOOLKIT_SEARCH_APP_H
#define SPL_TOOLKIT_SEARCH_APP_H

#include <dst-config.h>

#include <SPL/CodeGen/Config.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/HashMapHelpers.h>

#include <boost/filesystem/path.hpp>

namespace SPL {
class CompilerConfiguration;
class FrontEndDriver;
class PathSearch;

class DLL_PUBLIC ToolkitSearch
  : public Distillery::DistilleryApplication
  , public Singleton<ToolkitSearch>
{
  public:
    /// Constructor
    ToolkitSearch();
    ~ToolkitSearch();

  protected:
    virtual int run(const std::vector<std::string>& remains);
    virtual void getDistilleryApplicationVisibleOptions(option_vector_t& options);
    virtual void getArguments(option_vector_t& options);
    virtual int init();
    void mySigIntHandler(int /*signum*/);
    void iceHandler(int /*signum*/);
    void installFaultSignalHandlers(void);

    void setSPLPath(const option_t*, const char* value);

  private:
    void readMetaFile(std::vector<std::string>& paths);
    std::string checkStreamsInstallation();
    std::auto_ptr<CompilerConfiguration> _config;
    std::auto_ptr<PathSearch> _pathSearch;
    std::auto_ptr<FrontEndDriver> _frontEnd;
};
}

#endif /* SPL_TOOLKIT_SEARCH_APP_H */
