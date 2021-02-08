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

#include <SPL/Core/CommonModelImpl.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/CompilerMessage.h>

#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Common;
using boost::shared_ptr;
namespace bf = boost::filesystem;

PUSH_DLL_PUBLIC
IMPL_EXCEPTION(SPL, SPLCompilerInvalidLibraryModel, SPLCompiler);
POP_DLL_PUBLIC

Library::Library(const libraryType& xml, const std::string& baseDir)
  : description_(xml.description())
  , _managed(NULL)
{
    _managed = new ManagedLibrary(xml.managedLibrary(), baseDir);
}

Library::~Library()
{
    delete _managed;
}

std::string fixRelativePath(const std::string& p, const std::string& baseDir)
{
    bf::path path = p;
    if (!path.is_complete()) {
        if (baseDir == "." && p.substr(0, 2) == "./")
            return p;
        path = baseDir;
        path /= p;
        return path.normalize().string();
    }
    return p;
}

static void appendGeneratedNames(std::vector<std::string>& result,
                                 const std::string& cmd,
                                 const std::string& arg,
                                 const std::string& base,
                                 bool needsPathRelocation)
{
    // run the command, and process the output
    stringstream results;
    stringstream stderrOutput;
    int err = 0;
    try {
        std::vector<std::string> args;
        args.push_back(arg);
        err = ProcessLauncher::runProcess(cmd, args, false, false, &results, &stderrOutput);
        string stderrAsString = stderrOutput.str();
        if (!stderrAsString.empty())
            SysOut::warnln(SPL_CORE_SCRIPT_STDERR_OUTPUT(cmd, stderrAsString));
    } catch (SPLCompilerException& e) {
        SysOut::errorln(SPL_CORE_VERBATIM_ERROR(e.getExplanation()));
        THROW(SPLCompilerScriptFailed, SPL_CORE_CANNOT_RUN_SCRIPT(cmd));
    }
    if (err)
        THROW(SPLCompilerScriptFailed, SPL_CORE_ERROR_RUNNING_SCRIPT(err, cmd));

    // Parse the results
    // file format:
    // # comment
    // token
    while (!results.eof()) {
        string line;
        std::getline(results, line);
        string token = boost::trim_copy(line);
        if (token.size() == 0)
            continue;
        if (token[0] == '#')
            continue;
        if (needsPathRelocation)
            result.push_back(fixRelativePath(token, base));
        else
            result.push_back(token);
    }
}

ManagedLibrary::ManagedLibrary(const managedLibraryType& xml, const std::string& baseDir)
{
    typedef managedLibraryType::lib_const_iterator myiter;
    myiter litbeg = xml.lib().begin();
    myiter litend = xml.lib().end();
    for (myiter it = litbeg; it != litend; ++it) {
        const string& lib = *it;
        if (!boost::trim_copy(lib).empty())
            libs_.push_back(lib);
    }

    try {
        typedef managedLibraryType::libPath_const_iterator myiter;
        myiter lpitbeg = xml.libPath().begin();
        myiter lpitend = xml.libPath().end();
        for (myiter it = lpitbeg; it != lpitend; ++it) {
            const string& libPath = *it;
            if (!boost::trim_copy(libPath).empty())
                libPaths_.push_back(
                  fixRelativePath(Utility::expandEnvironmentVariables(libPath), baseDir));
        }

        typedef managedLibraryType::includePath_const_iterator myiter;
        myiter ipitbeg = xml.includePath().begin();
        myiter ipitend = xml.includePath().end();
        for (myiter it = ipitbeg; it != ipitend; ++it) {
            const string& incPath = *it;
            if (!boost::trim_copy(incPath).empty())
                includePaths_.push_back(
                  fixRelativePath(Utility::expandEnvironmentVariables(incPath), baseDir));
        }

        if (xml.command().present()) {
            string cmd = xml.command().get();
            if (!boost::trim_copy(cmd).empty()) {
                command_ = fixRelativePath(Utility::expandEnvironmentVariables(cmd), baseDir);
                appendGeneratedNames(libs_, command_, "lib", baseDir, false);
                appendGeneratedNames(libPaths_, command_, "libPath", baseDir, true);
                appendGeneratedNames(includePaths_, command_, "includePath", baseDir, true);
            }
        }
    } catch (SPLCompilerException const& e) {
        THROW(SPLCompilerInvalidLibraryModel, e.getExplanation());
    }
}

auto_ptr<libraryType> Library::toXsdInstance() const
{
    managedLibraryType p;
    const std::vector<std::string>& libs = _managed->getLibs();
    for (size_t i = 0, iu = libs.size(); i < iu; ++i)
        p.lib().push_back(libs[i]);

    const std::vector<std::string>& libPaths = _managed->getLibPaths();
    for (size_t i = 0, iu = libPaths.size(); i < iu; ++i)
        p.libPath().push_back(libPaths[i]);

    const std::vector<std::string>& includePaths = _managed->getIncludePaths();
    for (size_t i = 0, iu = includePaths.size(); i < iu; ++i)
        p.includePath().push_back(includePaths[i]);

    descriptionType description(description_);
    auto_ptr<libraryType> library(new libraryType(description, p));

    return library;
}
