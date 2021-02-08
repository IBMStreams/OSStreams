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

#include <SPL/Core/ToolkitSearch.h>

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/MixedModeLineMapper.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <SPL/Utility/XMLDiagnostic.h>
#include <UTILS/SupportFunctions.h>

#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/scoped_array.hpp>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;
using namespace SPL;
using namespace Distillery;
namespace bf = boost::filesystem;

#ifdef BOOST_WINDOWS_API
#define SPLPATH_SEP ";"
#else
#define SPLPATH_SEP ":"
#endif

#include <SPL/Runtime/Utility/Singleton.t>
template class Singleton<ToolkitSearch>;

ToolkitSearch::ToolkitSearch()
  : Singleton<ToolkitSearch>(*this)
{
    _config.reset(new CompilerConfiguration());
    _pathSearch.reset(new PathSearch());
    _frontEnd.reset(new FrontEndDriver());
}

ToolkitSearch::~ToolkitSearch() {}

void ToolkitSearch::mySigIntHandler(int /*signum*/)
{
    ostringstream ostr;
    ostr << "\n" << SPL_CORE_INTERRUPTED << "\n";
    async_safe_printf(1, ostr.str().c_str());
    exit(EXIT_FAILURE);
}

static bf::path toolkitListFile;
void ToolkitSearch::iceHandler(int sig)
{
    if (!toolkitListFile.empty()) {
        // Better to remove the file than leave an invalid one - messes up ICE
        try {
            bf::remove(toolkitListFile);
        } catch (...) {
        }
    }
    exit(EXIT_FAILURE);
}

void ToolkitSearch::installFaultSignalHandlers()
{
    if (indebug(Debug::Traceback)) {
        // Use default behaviour
        DistilleryApplication::installFaultSignalHandlers();
        return;
    }
    DistilleryApplication::getThisApp().removeSignalHandler(SIGINT);
    DistilleryApplication::getThisApp().installSignalHandler(
      SIGINT, mem_cb(this, &ToolkitSearch::mySigIntHandler));
    installSignalHandler(SIGABRT, mem_cb(this, &ToolkitSearch::iceHandler));
    installSignalHandler(SIGSEGV, mem_cb(this, &ToolkitSearch::iceHandler));
    installSignalHandler(SIGPIPE, mem_cb(this, &ToolkitSearch::iceHandler));
    installSignalHandler(SIGILL, mem_cb(this, &ToolkitSearch::iceHandler));
    installSignalHandler(SIGFPE, mem_cb(this, &ToolkitSearch::iceHandler));
    installSignalHandler(SIGBUS, mem_cb(this, &ToolkitSearch::iceHandler));
}

int ToolkitSearch::init()
{
    // run the base class init first....
    int r = DistilleryApplication::init();

    string installDir = checkStreamsInstallation();
    _config->setStringValue(CompilerConfiguration::InstallDir, installDir);

    return r;
}

void ToolkitSearch::getDistilleryApplicationVisibleOptions(option_vector_t& options)
{
    // We don't want any of the default visible options
}

void ToolkitSearch::getArguments(option_vector_t& options)
{
    setKeepRemainingArguments();
    option_t args[] = { { 't', "spl-path", ARG, "SPLPATH",
                          SPL_CORE_OPTION_SPL_PATH(string(SPLPATH_SEP)),
                          STR_OPT(ToolkitSearch::setSPLPath), 0, 0 } };
    APPEND_OPTIONS(options, args);
}

void ToolkitSearch::setSPLPath(const option_t*, const char* value)
{
    // remember this for later use in ::run
    if (_config->isSet(CompilerConfiguration::SPLPath)) {
        string app1 = _config->getStringValue(CompilerConfiguration::SPLPath) + SPLPATH_SEP + value;
        _config->setStringValue(CompilerConfiguration::SPLPath, app1);
    } else {
        _config->setStringValue(CompilerConfiguration::SPLPath, value);
    }
}

string ToolkitSearch::checkStreamsInstallation()
{
    string instDir;
    try {
        instDir = getInstallationDirectory();
    } catch (const DistilleryException& e) {
        SysOut::die(SPL_CORE_STREAMS_ERROR(e.getExplanation()));
    }
    return instDir;
}

/*
 * Main application processing routine
 */
int ToolkitSearch::run(const vector<string>& remains)
{
    // The line mapper needs to be created after splmm files have been created
    // but before the preprocessed spl file is processed.
    MixedModeLineMapper lineMapper;

    AutoErrorTrap aet(true);

    _config->setBooleanValue(CompilerConfiguration::IgnoreTimestamps, true);
    _config->setBooleanValue(CompilerConfiguration::TolerateMissingToolkits, true);
    _config->setBooleanValue(CompilerConfiguration::PrintSignature, true);

    bf::path instDir(_config->getStringValue(CompilerConfiguration::InstallDir));
    bf::path xsdDir = instDir / "etc" / "xsd" / "SPL";
    bf::path scriptDir = instDir / "bin";
    bf::path statDir = instDir / "/etc" / "stat";
    bf::path toolkitDir = instDir / "toolkits" / "spl";
    bf::path templateDir = instDir / "lib" / "spl" / "template";

    _config->setStringValue(CompilerConfiguration::XsdDir, xsdDir.string());
    _config->setStringValue(CompilerConfiguration::StatsDir, statDir.string());
    _config->setStringValue(CompilerConfiguration::ScriptDir, scriptDir.string());
    _config->setStringValue(CompilerConfiguration::ToolkitDir, toolkitDir.string());
    _config->setStringValue(CompilerConfiguration::TemplateDir, templateDir.string());

    // Setup all toolkit paths
    vector<string> allPaths;
    allPaths.push_back(".");

    // Do we have a user specified option?
    if (_config->isSet(CompilerConfiguration::SPLPath)) {
        string userPaths = _config->getStringValue(CompilerConfiguration::SPLPath);
        // Walk through the string, separating paths at SPLPATH_SEP
        vector<string> paths;
        Distillery::tokenize(userPaths, paths, SPLPATH_SEP, false);
        allPaths.insert(allPaths.end(), paths.begin(), paths.end());
    }
    // The default path for the spl toolkit
    allPaths.push_back(toolkitDir.string());
    // Grab the toolkits from the Meta-file
    readMetaFile(allPaths);
    SPLPRODDBG("Lookup paths for 'use's: ", Debug::Core);
    for (int i = 0; i < (int)allPaths.size(); i++) {
        SPLPRODDBG("  [" << i << "]\t" << allPaths[i], Debug::Core);
    }
    // Set it for path search

    _pathSearch->setPaths(allPaths);
    _pathSearch->locateToolkits();
    _pathSearch->resolveToolkitList();

    toolkitListFile = bf::current_path() / ".toolkitList";
    auto_ptr<ofstream> lfile(new ofstream(toolkitListFile.string().c_str()));
    if (!*lfile) {
        if (bf::exists(toolkitListFile)) {
            Utility::removeFile(toolkitListFile, false);
            lfile.reset(new ofstream(toolkitListFile.string().c_str()));
        }
        if (!*lfile)
            SysOut::die(SPL_CORE_CANNOT_CREATE_FILE(toolkitListFile.string(),
                                                    RuntimeUtility::getErrorNoStr()));
    }

    const vector<const Toolkit*>& toolkits = _pathSearch->toolkits();
    vector<const Toolkit*>::const_iterator it2 = toolkits.begin();
    vector<const Toolkit*>::const_iterator it2End = toolkits.end();
    for (; it2 != it2End; ++it2) {
        const Toolkit& toolkit = **it2;
        *lfile << toolkit.name() << "/" << toolkit.version() << endl;
    }

    lfile->flush();
    lfile->close();

    return aet.getNumErrorsInCurrentTrap() ? EXIT_FAILURE : EXIT_SUCCESS;
}

void ToolkitSearch::readMetaFile(vector<string>& paths)
{

    bf::path metaFile = bf::current_path();
    metaFile /= "META-INF";
    metaFile /= "SC.MF";

    ifstream infile;

    infile.open(metaFile.string().c_str(), ifstream::in);
    if (!infile) {
        return; // no file to work with
    }

    // Read the file line by line
    string l;
    while (getline(infile, l)) {
        // Ignore lines with leading #
        if (l.empty() || l[0] == '#') {
            continue;
        }
        // look for XXX=YYY
        size_t eq = l.find('=');
        if (eq == string::npos) {
            continue;
        }
        string word = l.substr(0, eq);
        if (word == "toolkit") {
            paths.push_back(l.substr(eq + 1));
        }
    }
}
