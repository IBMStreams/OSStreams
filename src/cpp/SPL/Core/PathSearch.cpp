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

#include <SPL/Core/PathSearch.h>

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/toolkitListModel.h>
#include <SPL/Core/toolkitModel.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <SPL/Utility/XMLDiagnostic.h>
#include <UTILS/SupportFunctions.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <dst-config.h>

#include <algorithm>
#include <cerrno>
#include <fstream>
#include <sys/stat.h>

namespace std {
namespace tr1 {
template<>
struct hash<struct stat>
{
    size_t operator()(const struct stat& s) const { return s.st_ino ^ s.st_dev; }
};
}
}

bool operator==(const struct stat& l, const struct stat& r)
{
    return l.st_ino == r.st_ino && l.st_dev == r.st_dev;
}

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace xmlns::prod::streams::spl::toolkit;

namespace bf = boost::filesystem;

template class Singleton<PathSearch>; // Explicitly instantiate the base class here

PathSearch::PathSearch()
  : Singleton<PathSearch>(*this)
  , _config(CompilerConfiguration::instance())
  , _builtinFcnNumber(-1)
{}

PathSearch::~PathSearch()
{
    for (unordered_map<NameLocation, NonFunctionResult*>::const_iterator it =
           _nonFunctionMap.begin();
         it != _nonFunctionMap.end(); ++it) {
        delete it->second;
    }
    for (uint32_t i = 0; i < _dependencies.size(); i++)
        delete _dependencies[i];
    for (unordered_map<string, ModelMap>::const_iterator it = _operatorModelMap.begin();
         it != _operatorModelMap.end(); ++it) {
        delete it->second.first;
    }
    for (uint32_t i = 0; i < _modelsToDelete.size(); i++)
        delete _modelsToDelete[i];
}

const string* PathSearch::addString(const string* str)
{
    if (!str)
        return NULL;
    unordered_set<string>::iterator it = _uniqueStrings.insert(*str).first;
    return &(*it);
}

bool PathSearch::setPaths(const vector<string>& dirs)
{
    // Walk through the list of directories
    // - ignore duplicates silently
    // - print warning and ignore non-directories
    // - save any left in _dirNames
    int num = dirs.size();
    typedef unordered_set<struct stat> statHash;
    statHash seen;
    for (int i = 0; i < num; i++) {
        // Have we seen this before?  To implement this, use the struct stat
        // and the dev/inode numbers to check for duplicates (handles symlinks,
        // etc.)
        const string& dir = dirs[i];
        struct stat s;
        if (stat(dir.c_str(), &s) < 0) { // failed to find it at all
            SysOut::warnln(SPL_CORE_PATH_IS_NOT_A_DIRECTORY_WARN(dir));
            continue;
        }
        statHash::const_iterator it = seen.find(s);
        if (it == seen.end())
            seen.insert(s);
        else {
            SysOut::warnln(SPL_CORE_PATH_IS_DUPLICATE(dir));
            continue;
        }
        _dirNames.push_back(dir);
    }
    return _dirNames.size() > 0;
}

bool PathSearch::toolkitSorter(const Toolkit* left, const Toolkit* right)
{
    return left->name() < right->name();
}

bool PathSearch::checkDependencies()
{
    bool success = true;

    // Check that each toolkit has the compiler version that it wants
    VersionInfo productVersion(STREAMS_VERSION);
    vector<const Toolkit*>::const_iterator it2;
    for (it2 = _toolkits.begin(); it2 != _toolkits.end(); it2++) {
        VersionRange toolkitWants((*it2)->model().requiredProductVersion());
        if (!toolkitWants.inRange(productVersion)) {
            SysOut::errorln(SPL_CORE_TOOLKIT_MISMATCH_PRODUCT_VERSION(
              (*it2)->name(), (*it2)->model().requiredProductVersion(), STREAMS_VERSION));
            success = false;
        }
    }

    unordered_map<string, const Toolkit*> toolkitMap;
    vector<const Toolkit*>::const_iterator it3;
    for (it3 = _toolkits.begin(); it3 != _toolkits.end(); it3++) {
        string const& name = (*it3)->name();
        VersionInfo version((*it3)->model().version());
        if (toolkitMap.count(name) != 0) {
            string const& pvstr = toolkitMap[name]->model().version();
            VersionInfo pversion(pvstr);
            // ensure we take the first one if they are the same version
            if (pversion >= version)
                continue;
        }
        toolkitMap[name] = *it3;
    }

    if (CompilerConfiguration::instance().isSet(CompilerConfiguration::Verbose)) {
        // List all the toolkits used in sorted order
        vector<const Toolkit*> usedToolkits;
        unordered_map<string, const Toolkit*>::const_iterator it;
        for (it = toolkitMap.begin(); it != toolkitMap.end(); it++)
            usedToolkits.push_back(it->second);
        sort(usedToolkits.begin(), usedToolkits.end(), toolkitSorter);
        for (vector<const Toolkit*>::const_iterator i = usedToolkits.begin();
             i != usedToolkits.end(); i++) {
            SysOut::verboseln(
              SPL_CORE_USING_TOOLKIT((*i)->name(), (*i)->model().version(), (*i)->file()), cout);
        }
    }

    // Check that all the dependencies are met by some toolkit
    vector<const ToolkitInfo::ToolkitInfoModel*>::const_iterator it;
    for (it = _infos.begin(); it != _infos.end(); it++) {
        if (*it == NULL)
            continue; // toolkit had no info.xml file
        std::vector<ToolkitInfo::DependencyPtr>::const_iterator it4;
        for (it4 = (*it)->getDependencies().begin(); it4 != (*it)->getDependencies().end(); it4++) {
            const string& lookingFor = (*it4)->getName();
            const string& wantVersion = (*it4)->getVersion();
            VersionRange want(wantVersion);
            vector<ToolkitModelPtr>::const_iterator it5;
            if (toolkitMap.count(lookingFor) == 0) {
                SysOut::errorln(
                  SPL_CORE_TOOLKIT_DEPENDENCY_MISSING((*it)->getName(), lookingFor, wantVersion));
                success = false;
            } else {
                string const& gotStr = toolkitMap[lookingFor]->model().version();
                VersionInfo got(gotStr);
                if (!want.inRange(got)) {
                    SysOut::errorln(SPL_CORE_TOOLKIT_DEPENDENCY_MISMATCH(
                      (*it)->getName(), lookingFor, wantVersion, gotStr));
                    success = false;
                }
            }
        }
    }
    return success;
}

static int toolkitsReverseSortFcn(const OneToolkit& a, const OneToolkit& b)
{
    return a.first > b.first;
}

bool PathSearch::loadToolkits()
{
    locateToolkits();

    // Take the highest version of each toolkit
    ToolkitMap::iterator it;
    for (it = _allToolkits.begin(); it != _allToolkits.end(); it++) {
        vector<OneToolkit>& tkts = it->second;
        if (tkts.size() > 1)
            std::sort(tkts.begin(), tkts.end(), toolkitsReverseSortFcn);
        _toolkits.push_back(tkts[0].second);
        _infos.push_back(tkts[0].second->infoModel());
    }

    // We picked which ones we wanted.   Now load them
    bool success = true;
    if (!loadTheToolkits())
        success = false;

    // Okay, we have loaded all the toolkits.  Check dependencies
    if (success)
        success = checkDependencies();

    if (!success)
        SysOut::die(SPL_CORE_DIE_ERROR_LOADING_TOOLKITS);
    return success;
}

void PathSearch::checkToolkits(unordered_set<string>& outOfDateDirectories)
{
    uint32_t numDirs = _dirNames.size();
    for (uint32_t i = 0; i < numDirs; i++) {
        bf::path dir(_dirNames[i]);
        // Is this an XML file giving paths or a directory?
        if (bf::is_directory(dir))
            checkOneToolkit(outOfDateDirectories, _dirNames[i]);
        else
            checkToolkitList(outOfDateDirectories, _dirNames[i]);
    }
}

bool PathSearch::locateToolkits()
{
    AutoErrorTrap aet();
    uint32_t numDirs = _dirNames.size();
    bool success = true;

    if (_config.getBooleanValue(CompilerConfiguration::RebuildToolkits)) {
        unordered_set<string> outOfDate;
        checkToolkits(outOfDate);
        unordered_set<string>::const_iterator it;
        bf::path maketk = _config.getStringValue(CompilerConfiguration::InstallDir);
        maketk /= "bin";
        maketk /= "spl-make-toolkit";

        for (it = outOfDate.begin(); it != outOfDate.end(); it++) {
            const string& dir = *it;
            SysOut::warnln(SPL_CORE_TOOLKIT_WILL_BE_REBUILT(dir));

            // We need to index the application directory first
            vector<string> args;
            if (_config.isSet(CompilerConfiguration::NoMixedModePreprocessing)) {
                args.push_back("--no-mixed-mode-preprocessing");
            }
            args.push_back("-i");
            args.push_back(dir);
            if (0 != ProcessLauncher::runProcess(maketk.string(), args)) {
                exit(EXIT_FAILURE); // spl-make-toolkit will print the proper message
            }
        }
    }

    // Used to detect duplicate toolkit names - contains index into _toolkits
    unordered_map<string, uint32_t> toolkitNames;

    // Dependency information
    for (uint32_t i = 0; i < numDirs; i++) {
        bf::path dir(_dirNames[i]);
        // Is this an XML file giving paths or a directory?
        if (bf::is_directory(dir)) {
            processOneToolkit(_dirNames[i], toolkitNames, success);
        } else {
            processToolkitList(_dirNames[i], toolkitNames, success);
        }
    }
    return true;
}

static int toolkitsSortFcn(const OneToolkit& a, const OneToolkit& b)
{
    return b.first > a.first;
}

static int toolkitSortFcn(const Toolkit* a, const Toolkit* b)
{
    return b->name() > a->name();
}

static int solutionSortFcn(const Solution& a, const Solution& b)
{
    return b.compareTo(a) < 0;
}

void PathSearch::toolkitsThatSatisfyDependency(const ToolkitMap& toolkitsNeedingResolution,
                                               const OneDep& dep,
                                               vector<const Toolkit*>& toolkitsThatSatisfyDep)
{
    for (ToolkitMap::const_iterator it = toolkitsNeedingResolution.begin();
         it != toolkitsNeedingResolution.end(); ++it) {
        const vector<OneToolkit>& tkts = it->second;
        const string& tkName = it->first;
        if (tkName == dep._dependentName) {
            for (vector<OneToolkit>::const_iterator it1 = tkts.begin(); it1 != tkts.end(); ++it1) {
                const Toolkit& toolkit = *it1->second;
                const Toolkit::ToolkitModel model = toolkit.model();
                const string& tkVersion = model.version();
                VersionInfo vi(tkVersion);
                VersionRange vr(dep._versionRange);
                if (vr.inRange(vi)) {
                    toolkitsThatSatisfyDep.push_back(&toolkit);
                    SPLPRODDBG("Toolkit " << tkName << " version " << tkVersion
                                          << " satisfies dependency " << dep._dependentName
                                          << " version range " << dep._versionRange,
                               Debug::Core);
                }
            }
            return;
        }
    }
}

void PathSearch::addSingletonDependencies(ToolkitMap& toolkitsNeedingResolution,
                                          unordered_map<string, const Toolkit*>& toolkitsToBeLoaded)
{
    unordered_set<string> toolkitsSeen;
    unordered_set<string>::const_iterator cit;
    bool changed;
    do {
        changed = false;
        for (unordered_map<string, const Toolkit*>::iterator it = toolkitsToBeLoaded.begin();
             it != toolkitsToBeLoaded.end(); ++it) {
            const Toolkit& toolkit = *it->second;
            const string& tkName = toolkit.name();
            cit = toolkitsSeen.find(tkName);
            if (cit != toolkitsSeen.end())
                continue;
            toolkitsSeen.insert(tkName);

            const Toolkit::ToolkitModel model = toolkit.model();
            const Toolkit::ToolkitInfoModel* info = toolkit.infoModel();
            if (info) {
                const vector<ToolkitInfo::DependencyPtr>& deps = info->getDependencies();
                for (uint32_t i = 0, size = deps.size(); i < size; i++) {
                    const ToolkitInfo::Dependency& dep = *deps[i];
                    OneDep d(dep.getName(), dep.getVersion(), tkName, model.version());
                    vector<const Toolkit*> toolkitsThatSatisfyDep;
                    toolkitsThatSatisfyDependency(toolkitsNeedingResolution, d,
                                                  toolkitsThatSatisfyDep);
                    if (toolkitsThatSatisfyDep.size() == 1) {
                        // There is only one toolkit that satisfies the dependency.  Use it and remove it from the list of toolkits needing resolution
                        const Toolkit& tk = *toolkitsThatSatisfyDep[0];
                        toolkitsNeedingResolution.erase(tk.name());
                        toolkitsToBeLoaded.insert(make_pair(tk.name(), &tk));
                        changed = true;
                    }
                }
            }
        }
    } while (changed);
}

bool PathSearch::hasDependent(const vector<const Toolkit*>& toolkitsToConsider,
                              const Toolkit& toolkit) const
{
    // For each toolkit in the list of toolkits to consider, see if it has a dependency on the specified toolkit
    for (vector<const Toolkit*>::const_iterator it = toolkitsToConsider.begin();
         it != toolkitsToConsider.end(); ++it) {
        const Toolkit& tk = **it;
        if (tk.dependsOn(toolkit)) {
            return true;
        }
    }
    return false;
}

void PathSearch::findRootToolkits(const vector<const Toolkit*>& toolkitsToConsider,
                                  vector<const Toolkit*>& rootToolkits) const
{
    // For each toolkit in the list of toolkits to consider, see if it has any toolkit that is dependent on it
    for (vector<const Toolkit*>::const_iterator it = toolkitsToConsider.begin();
         it != toolkitsToConsider.end(); ++it) {
        const Toolkit& toolkit = **it;
        if (!hasDependent(toolkitsToConsider, toolkit)) {
            rootToolkits.push_back(&toolkit);
        }
    }
}

void PathSearch::findToolkitsDependedOn(const vector<const Toolkit*>& toolkitsToConsider,
                                        const Toolkit& toolkit,
                                        vector<const Toolkit*>& deps) const
{
    for (vector<const Toolkit*>::const_iterator it = toolkitsToConsider.begin();
         it != toolkitsToConsider.end(); ++it) {
        const Toolkit& tk = **it;
        if (toolkit.dependsOn(tk)) {
            deps.push_back(&tk);
        }
    }
}

bool PathSearch::computePartialSolution(
  const vector<const Toolkit*>& toolkitsToConsider,
  const unordered_map<string, const Toolkit*>& toolkitsToBeLoaded,
  unordered_map<string, const Toolkit*>& toolkitsSeen,
  vector<const Toolkit*>& tkList,
  vector<Solution>& partialSolutions) const
{
    // The toolkit we are visiting is the one at the end of the list
    const Toolkit& toolkit = *tkList.back();
    SPLPRODDBG("Visiting toolkit " << toolkit.name() << " version " << toolkit.version(),
               Debug::Core);
    vector<const Toolkit*> deps;
    // check for cycle and dont calculates deps if there is one. This means that the partial solutions would be
    // calculated from the current set of toolkits that have been seen.
    // the cycle is only detected if there is more than one element in tkList otherwise a single element will always be a cycle.
    if (tkList.size() == 1 || toolkitsSeen.find(toolkit.name()) == toolkitsSeen.end()) {
        findToolkitsDependedOn(toolkitsToConsider, toolkit, deps);
        for (vector<const Toolkit*>::const_iterator it = deps.begin(); it != deps.end(); ++it) {
            const Toolkit& tk = **it;
            // do not throw error when there is a cycle. A cycle would already have been detected and we would not get here.
            //        if (toolkitsSeen.find(tk.name()) != toolkitsSeen.end()) {
            //            SysOut::errorln(SPL_CORE_CYCLIC_DEPENDENCY);
            //            for (unordered_map<string, const Toolkit*>::const_iterator cit = toolkitsSeen.begin(); cit != toolkitsSeen.end(); ++cit) {
            //                const Toolkit& t = *cit->second;
            //                SysOut::detailsErrorln(SPL_CORE_TOOLKIT_CYCLE_DETAIL(t.name(), t.version()));
            //            }
            //            return false;
            //        }
            unordered_map<string, const Toolkit*> tmpToolkitsSeen = toolkitsSeen;
            tmpToolkitsSeen.insert(make_pair(tk.name(), &tk));
            vector<const Toolkit*> tmpList = tkList;
            tmpList.push_back(&tk);
            if (!computePartialSolution(toolkitsToConsider, toolkitsToBeLoaded, tmpToolkitsSeen,
                                        tmpList, partialSolutions)) {
                return false;
            }
        }
    }
    if (0 == deps.size()) {
        // Create partial solutions for those paths:
        //  - where every toolkit is already represented in the list of required toolkits
        //  - where no toolkit is a different version than one already in the list of required toolkits
        // Check that not all toolkits in the solution are already in the list and that none are in conflict with those already in the list
        bool solutionValid = true;
        bool everyToolkitIsAlreadyDestinedToBeLoaded = true;
        for (int32_t i = 0, numToolkits = tkList.size(); i < numToolkits; ++i) {
            const Toolkit& tk = *tkList[i];
            // If this exact toolkit is not already in the set to load, keep track of that fact
            unordered_map<std::string, const Toolkit*>::const_iterator cit =
              toolkitsToBeLoaded.find(tk.name());
            if (cit == toolkitsToBeLoaded.end()) {
                everyToolkitIsAlreadyDestinedToBeLoaded = false;
            } else {
                // There already is a version of this toolkit in the list.  Is it the same?
                const Toolkit* t = cit->second;
                if (t != &tk) {
                    // The versions are not the same so this solution is not valid
                    solutionValid = false;
                    break;
                }
            }
        }
        // Add this partial solution to the set if it is valid and not already being loaded
        if (solutionValid && !everyToolkitIsAlreadyDestinedToBeLoaded) {
            partialSolutions.push_back(Solution(tkList));
        }
    }
    return true;
}

bool PathSearch::satisfiesAllDependencies(const Toolkit& toolkit,
                                          unordered_map<string, const Toolkit*>& potentialSolution,
                                          vector<FormattableMessage*>& details) const
{
    for (unordered_map<string, const Toolkit*>::const_iterator cit = potentialSolution.begin();
         cit != potentialSolution.end(); ++cit) {
        const Toolkit& tk = *cit->second;
        if (!toolkit.satisfiesDependencies(tk) || !tk.satisfiesDependencies(toolkit)) {
            FormattableMessage* msg = new FormattableMessage(SPL_CORE_UNRECONCILABLE_TOOLKIT(
              tk.name(), tk.version(), toolkit.name(), toolkit.version()));
            details.push_back(msg);
            return false;
        }
    }
    return true;
}

bool PathSearch::addPartialSolution(const Solution& ps,
                                    unordered_map<string, const Toolkit*>& potentialSolution,
                                    vector<FormattableMessage*>& details) const
{
    // Add this partial solution as long as all the toolkits in the solution satisfy the dependencies of the toolkits already in the potential solution
    const vector<const Toolkit*>& tks = ps.toolkits();
    for (vector<const Toolkit*>::const_iterator cit = tks.begin(); cit != tks.end(); ++cit) {
        const Toolkit& tk = **cit;
        if (!satisfiesAllDependencies(tk, potentialSolution, details)) {
            return false;
        }
    }
    // We got here so we satisfy all dependencies.  Add the toolkits to the potential solution
    for (vector<const Toolkit*>::const_iterator cit = tks.begin(); cit != tks.end(); ++cit) {
        const Toolkit& tk = **cit;
        potentialSolution.insert(make_pair(tk.name(), &tk));
    }

    return true;
}

bool PathSearch::satisfiesAllDependencies(
  const unordered_map<string, const Toolkit*>& potentialSolution,
  vector<FormattableMessage*>& details) const
{
    // This checks to ensure that for each toolkit in the potential solution, all the other toolkits in the potential solution
    // satisfy its requirements.  This doesn't check for missing toolkits.
    for (unordered_map<string, const Toolkit*>::const_iterator outer = potentialSolution.begin();
         outer != potentialSolution.end(); ++outer) {
        const Toolkit& tkOuter = *outer->second;
        for (unordered_map<string, const Toolkit*>::const_iterator inner =
               potentialSolution.begin();
             inner != potentialSolution.end(); ++inner) {
            if (outer == inner)
                continue;
            const Toolkit& tkInner = *inner->second;
            if (!tkInner.satisfiesDependencies(tkOuter)) {
                FormattableMessage* msg = new FormattableMessage(SPL_CORE_UNRECONCILABLE_TOOLKIT(
                  tkOuter.name(), tkOuter.version(), tkInner.name(), tkInner.version()));
                details.push_back(msg);
                return false;
            }
        }
    }
    return true;
}

bool PathSearch::resolveToolkitList()
{
    // We have to figure out which toolkits we are going to be using
    // Perform the following steps:
    // 1 - Add the toolkit being built to the set of toolkits to use
    // 2 - Add all singleton toolkits to the set of toolkits to use
    // 3 - Find all the remaining toolkits that must be included based on dependencies in the toolkits already included
    // 4 - Find the set of toolkits that are on the LHS of a directed graph based on dependencies
    // 5 - Walk the graph and compute all possible solutions
    // 6 - Sort the possible solutions by decreasing weight
    // 7 - Remove any toolkits that are already selected
    // 8 - Compute the sets of possible solutions until the first one that satsisfies all the dependencies is found.

    // Step 1 is covered by the toolkit search mechanism.  Only one toolkit with the same name as the application is included

    // Order the toolkit versions and add any singletons to the set of required toolkits

    unordered_map<string, const Toolkit*> toolkitsToBeLoaded;
    ToolkitMap toolkitsNeedingResolution;
    ToolkitMap::iterator it;
    for (it = _allToolkits.begin(); it != _allToolkits.end(); it++) {
        SPLPRODDBG("Processing toolkit: " << it->first, Debug::Core);
        vector<OneToolkit>& tkts = it->second;
        if (tkts.size() > 1) {
            std::sort(tkts.begin(), tkts.end(), toolkitsSortFcn);
            SPLPRODDBG("Adding toolkit " << it->first
                                         << " to the set of toolkits needing version resolution",
                       Debug::Core);
            toolkitsNeedingResolution.insert(make_pair(it->first, it->second));
        } else {
            // There is only one version of this toolkit.  We'll just use it.
            const Toolkit& tk = *tkts[0].second;
            SPLPRODDBG("Adding toolkit " << tk.name() << " to the set of toolkits to be loaded",
                       Debug::Core);
            toolkitsToBeLoaded.insert(make_pair(tk.name(), &tk));
        }
        if (prodindebug(Debug::Core)) {
            std::stringstream ss;
            ss << "Located toolkit " << it->first << ": ";
            for (uint32_t i = 0, size = tkts.size(); i < size; ++i) {
                ss << tkts[i].second->model().version() << ' ';
            }
            SysOut::debug(ss.str());
        }
    }

    if (0 < toolkitsNeedingResolution.size()) {
        // Add any toolkits that have a single version that satisfies the dependency of a toolkit already in the set of required toolkits
        addSingletonDependencies(toolkitsNeedingResolution, toolkitsToBeLoaded);
    }

    if (0 < toolkitsNeedingResolution.size()) {
        // Find the set of toolkits on the LHS of a dependency graph
        vector<const Toolkit*> toolkitsToConsider;
        for (unordered_map<string, const Toolkit*>::const_iterator cit = toolkitsToBeLoaded.begin();
             cit != toolkitsToBeLoaded.end(); ++cit) {
            toolkitsToConsider.push_back(cit->second);
        }
        for (it = toolkitsNeedingResolution.begin(); it != toolkitsNeedingResolution.end(); ++it) {
            vector<OneToolkit>& tkts = it->second;
            for (uint32_t i = 0, size = tkts.size(); i < size; ++i) {
                toolkitsToConsider.push_back(tkts[i].second);
            }
        }
        vector<const Toolkit*> rootToolkits;
        findRootToolkits(toolkitsToConsider, rootToolkits);

        vector<Solution> partialSolutions;
        unordered_map<string, const Toolkit*> toolkitsHandled;
        for (vector<const Toolkit*>::const_iterator it1 = rootToolkits.begin();
             it1 != rootToolkits.end(); ++it1) {
            unordered_map<string, const Toolkit*> toolkitsSeen;
            const Toolkit& tk = **it1;
            toolkitsSeen.insert(make_pair(tk.name(), &tk));
            vector<const Toolkit*> tkList;
            tkList.push_back(&tk);
            if (!computePartialSolution(toolkitsToConsider, toolkitsToBeLoaded, toolkitsSeen,
                                        tkList, partialSolutions)) {
                return false;
            }
            if (toolkitsHandled.find(tk.name()) == toolkitsHandled.end()) {
                toolkitsHandled.insert(make_pair(tk.name(), &tk));
            }
        }

        // if the root toolkit was in the toolkitsNeedingResolution list then remove it from the list.
        for (ToolkitMap::const_iterator it1 = toolkitsNeedingResolution.begin();
             it1 != toolkitsNeedingResolution.end(); ++it1) {
            const vector<OneToolkit>& tkts = it1->second;
            for (vector<OneToolkit>::const_iterator it2 = tkts.begin(); it2 != tkts.end(); ++it2) {
                const Toolkit& tk = *it2->second;
                if (toolkitsHandled.find(tk.name()) != toolkitsHandled.end()) {
                    toolkitsNeedingResolution.erase(tk.name());
                }
            }
        }
        // At this point, the only toolkits needing resolution would be toolkits that are involved in a circular loop.
        // Construct partial solutions using this list as well.
        for (ToolkitMap::const_iterator it1 = toolkitsNeedingResolution.begin();
             it1 != toolkitsNeedingResolution.end(); ++it1) {
            unordered_map<string, const Toolkit*> toolkitsSeen;
            const vector<OneToolkit>& tkts = it1->second;
            for (vector<OneToolkit>::const_iterator it2 = tkts.begin(); it2 != tkts.end(); ++it2) {
                const Toolkit& tk = *it2->second;
                toolkitsSeen.insert(make_pair(tk.name(), &tk));
                vector<const Toolkit*> tkList;
                tkList.push_back(&tk);
                if (!computePartialSolution(toolkitsToConsider, toolkitsToBeLoaded, toolkitsSeen,
                                            tkList, partialSolutions)) {
                    return false;
                }
            }
        }

        int32_t numPartialSolutions = partialSolutions.size();
        if (0 < numPartialSolutions) {
            if (1 < numPartialSolutions) {
                std::sort(partialSolutions.begin(), partialSolutions.end(), solutionSortFcn);
            }
            if (indebug(Debug::Core)) {
                SysOut::debug("Sorted partial solutions are:");
                for (int32_t i = 0; i < numPartialSolutions; ++i) {
                    const Solution& s = partialSolutions[i];
                    ostringstream ss;
                    ss << s;
                    SysOut::debug(ss.str());
                }
            }
            // Construct complete solutions until one is found that has no conflicts and satisfies all the constraints
            bool solutionFound = false;
            vector<FormattableMessage*> details;
            for (int32_t outer = 0; outer < numPartialSolutions; ++outer) {
                unordered_map<string, const Toolkit*> potentialSolution = toolkitsToBeLoaded;
                const Solution& so = partialSolutions[outer];
                if (!addPartialSolution(so, potentialSolution, details))
                    continue;
                for (int32_t inner = 0; inner < numPartialSolutions; ++inner) {
                    if (inner == outer)
                        continue;
                    const Solution& si = partialSolutions[inner];
                    addPartialSolution(si, potentialSolution, details);
                }
                if (satisfiesAllDependencies(potentialSolution, details)) {
                    solutionFound = true;
                    toolkitsToBeLoaded = potentialSolution;
                    break;
                }
            }
            if (!solutionFound) {
                SysOut::errorln(SPL_CORE_UNRECONCILABLE_DEPENDENCIES);
                for (int32_t i = 0, size = details.size(); i < size; ++i) {
                    FormattableMessage& msg = *details[i];
                    SysOut::detailsErrorln(msg);
                    delete &msg;
                }
                return false;
            } else {
                for (int32_t i = 0, size = details.size(); i < size; ++i) {
                    FormattableMessage& msg = *details[i];
                    delete &msg;
                }
            }
        }
    }

    // Add the toolkits we've selected to the list of toolkits to be loaded
    // We want the toolkit being compiled to be the first, followed by the SPL toolkit, followed by all other toolkits
    // We put the current toolkit first so that duplicate symbol messages show the current toolkit as the original declaring location
    {
        const Toolkit* splToolkit = NULL;
        const Toolkit* currentToolkit = NULL;
        for (unordered_map<string, const Toolkit*>::const_iterator cit = toolkitsToBeLoaded.begin();
             cit != toolkitsToBeLoaded.end(); ++cit) {
            const Toolkit& toolkit = *cit->second;
            SPLPRODDBG("Adding toolkit " << toolkit.name() << " version " << toolkit.version()
                                         << " to the list of toolkits to be loaded",
                       Debug::Core);
            if (currentToolkit == NULL && toolkit.isCurrentToolkit()) {
                currentToolkit = &toolkit;
            } else if (splToolkit == NULL && toolkit.isSPLToolkit()) {
                splToolkit = &toolkit;
            } else {
                _toolkits.push_back(&toolkit);
            }
            _infos.push_back(toolkit.infoModel());
        }
        // Sort the toolkits in the list by name.  This is done primarily so that the ordering provided by the map iterator
        // doesn't affect the tests that check the output.  The actual order doesn't really matter.
        std::sort(_toolkits.begin(), _toolkits.end(), toolkitSortFcn);
        SPLPRODDBG("Prepending toolkit " << splToolkit->name() << " version "
                                         << splToolkit->version()
                                         << " to the list of toolkits to be loaded",
                   Debug::Core);
        if (NULL != splToolkit)
            _toolkits.insert(_toolkits.begin(), splToolkit);
        SPLPRODDBG("Prepending toolkit " << currentToolkit->name() << " version "
                                         << currentToolkit->version()
                                         << " to the list of toolkits to be loaded",
                   Debug::Core);
        if (NULL != currentToolkit)
            _toolkits.insert(_toolkits.begin(), currentToolkit);
    }
    return true;
}

bool PathSearch::resolveToolkits(const string& namespaceName, const string& compositeName)
{

    if (!resolveToolkitList())
        return false;

    // We picked which ones we wanted.   Now load them
    bool success = loadTheToolkits();

    // Okay, we have loaded all the toolkits.  Check dependencies
    if (success)
        success = checkDependencies();

    if (!success)
        SysOut::die(SPL_CORE_DIE_ERROR_LOADING_TOOLKITS);
    return success;
}

void PathSearch::processOneToolkit(const string& dir,
                                   unordered_map<string, uint32_t>& toolkitNames,
                                   bool& success)
{
    bf::path dPath = dir;
    bf::path toolkit = dPath / "toolkit.xml";
    ifstream toolkitStr(toolkit.string().c_str(), fstream::in);
    if (!toolkitStr) {
        // We don't have a toolkit.xml.   Do we have a bunch of subdirectories with toolkit.xml(s)?
        bool foundSubdirectoryWithToolkit = false;
        bf::directory_iterator end_itr; // default construction yields past-the-end
        if (bf::exists(dPath)) {
            for (bf::directory_iterator it(dPath); it != end_itr; ++it) {
                bf::path subDir(*it);
                if (is_directory(subDir)) {
                    bf::path tk = subDir / "toolkit.xml";
                    if (exists(tk) && !is_directory(tk)) {
                        ifstream tkStr(tk.string().c_str(), fstream::in);
                        if (tkStr) {
                            foundSubdirectoryWithToolkit = true;
                            processOneDirectory(tkStr, tk.string(), subDir.string(), toolkitNames,
                                                success);
                        }
                    }
                }
            }
        }
        if (!foundSubdirectoryWithToolkit &&
            Utility::cleanPath(bf::system_complete(dPath)) !=
              Utility::cleanPath(
                bf::path(_config.getStringValue(CompilerConfiguration::ApplicationDir))) &&
            !_config.getBooleanValue(CompilerConfiguration::TolerateMissingToolkits))
            SysOut::warnln(SPL_CORE_MISSING_TOOLKIT(toolkit.string()));
    } else {
        processOneDirectory(toolkitStr, toolkit.string(), dir, toolkitNames, success);
    }
}

void PathSearch::processToolkitList(const string& fileName,
                                    unordered_map<string, uint32_t>& toolkitNames,
                                    bool& success)
{
    // Try to load this as a toolkit list XML
    SPLDBG("Starting processing toolkit list " << fileName, Debug::Core);
    using namespace xmlns::prod::streams::spl::toolkitList;
    ifstream toolkitListStr(fileName.c_str(), fstream::in);
    if (!toolkitListStr) {
        SPLDBG("Unable to open " << fileName, Debug::Core);
        SysOut::errorln(SPL_CORE_CANNOT_OPEN_FILE(fileName, RuntimeUtility::getErrorNoStr()));
        return;
    }
    auto_ptr<toolkitListType> tkList;
    string xsddir = _config.getStringValue(CompilerConfiguration::XsdDir);

    // Need the directory name for relative paths
    bf::path dir = fileName;
    dir = dir.branch_path();

    string toolkitListSchemaName((bf::path(xsddir) / "toolkitListModel.xsd").string());
    XMLDiag::XMLDiagPrinter errorPrinter(fileName);
    try {
        ::xml_schema::properties props;
        props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/toolkitList",
                              toolkitListSchemaName);
        tkList = toolkitList(toolkitListStr, errorPrinter, 0, props);
    } catch (::xsd::cxx::exception& e) {
        SPLDBG("Error processing toolkit list " << fileName, Debug::Core);
        SysOut::errorln(SPL_CORE_ERROR_LOADING_TKLIST_MODEL(e.what()),
                        SourceLocation(fileName, 0, 0));
        return;
    }

    // Walk the information in the toolkit list
    const toolkitListType::toolkit_sequence& tks = tkList->toolkit();
    for (toolkitListType::toolkit_const_iterator it = tks.begin(); it != tks.end(); it++) {
        const xmlns::prod::streams::spl::toolkitList::toolkitType& tk = *it;
        if (tk.directory().present()) {
            bf::path directory = tk.directory().get();
            if (!directory.is_complete())
                directory = dir / directory;
            processOneToolkit(directory.string(), toolkitNames, success);
        }
        if (tk.listFile().present()) {
            bf::path listFile = tk.listFile().get();
            if (!listFile.is_complete())
                listFile = dir / listFile;
            processToolkitList(listFile.string(), toolkitNames, success);
        }
    }
    SPLDBG("Finished processing toolkit list " << fileName, Debug::Core);
}

void PathSearch::checkOneDirectory(unordered_set<string>& outOfDateDirectories,
                                   ifstream& toolkitStr,
                                   const string& toolkit,
                                   const string& dir)
{
    toolkitModelType* model;
    try {
        model = toolkitModel(toolkitStr, xsd::cxx::tree::flags::dont_validate).release();
        _modelsToDelete.push_back(model);
    } catch (::xsd::cxx::exception& e) {
        // Ignore this
        return;
    }

    // Timestamp handling
    time_t toolkitTime = bf::last_write_time(bf::path(toolkit));

    // Walk all the toolkits in the model
    const toolkitModelType::toolkit_sequence& toolkits1 = model->toolkit();
    for (toolkitModelType::toolkit_const_iterator it = toolkits1.begin(); it != toolkits1.end();
         it++) {
        const toolkitType& tk = *it;

        // Process each toolkit
        // Load the URI table
        URITableType uriTable;
        if (tk.uriTable().present()) {
            const uriTableType& table = tk.uriTable().get();
            const uriTableType::uri_sequence& uris = table.uri();
            for (uriTableType::uri_const_iterator it1 = uris.begin(); it1 != uris.end(); it1++)
                uriTable.insert(make_pair(it1->index(), Utility::uriDecode(it1->value())));
        }

        // Walk all the namespaces
        const toolkitType::namespace_sequence& namespaces = tk.namespace_();
        for (toolkitType::namespace_const_iterator it2 = namespaces.begin();
             it2 != namespaces.end(); ++it2) {
            const toolkitNamespaceType& ns = *it2;

            // Check the operators
            const toolkitNamespaceType::primitiveOp_sequence& prims = ns.primitiveOp();
            for (toolkitNamespaceType::primitiveOp_const_iterator it3 = prims.begin();
                 it3 != prims.end(); ++it3) {
                URITableType::const_iterator it4 = uriTable.find(it3->modelUriIndex());
                if (it4 != uriTable.end()) {
                    if (checkNonFunction(dir, it4->second, it3->name(), toolkitTime)) {
                        outOfDateDirectories.insert(dir);
                        break;
                    }
                }
            }

            const toolkitNamespaceType::compositeOp_sequence& comps = ns.compositeOp();
            for (toolkitNamespaceType::compositeOp_const_iterator it3 = comps.begin();
                 it3 != comps.end(); ++it3) {
                URITableType::const_iterator it4 = uriTable.find(it3->uriIndex());
                if (it4 != uriTable.end()) {
                    if (checkNonFunction(dir, it4->second, "", toolkitTime)) {
                        outOfDateDirectories.insert(dir);
                        break;
                    }
                }
            }

            // Types too
            const toolkitNamespaceType::type_sequence& types = ns.type();
            for (toolkitNamespaceType::type_const_iterator it3 = types.begin(); it3 != types.end();
                 ++it3) {
                URITableType::const_iterator it4 = uriTable.find(it3->uriIndex());
                if (it4 != uriTable.end()) {
                    if (checkNonFunction(dir, it4->second, "", toolkitTime)) {
                        outOfDateDirectories.insert(dir);
                        break;
                    }
                }
            }

            // And enums
            const toolkitNamespaceType::enumLiterals_sequence& enums = ns.enumLiterals();
            for (toolkitNamespaceType::enumLiterals_const_iterator it3 = enums.begin();
                 it3 != enums.end(); ++it3) {
                const enumLiteralsType::enum_sequence& lits = it3->enum_();
                for (enumLiteralsType::enum_const_iterator it4 = lits.begin(); it4 != lits.end();
                     it4++) {
                    URITableType::const_iterator it5 = uriTable.find(it3->uriIndex());
                    if (it5 != uriTable.end()) {
                        if (checkNonFunction(dir, it5->second, "", toolkitTime)) {
                            outOfDateDirectories.insert(dir);
                            break;
                        }
                    }
                }
            }

            // Now check the functions
            const toolkitNamespaceType::function_sequence& splFunctions = ns.function();
            for (toolkitNamespaceType::function_const_iterator it3 = splFunctions.begin();
                 it3 != splFunctions.end(); ++it3) {
                if (checkFunctionsFromModel(dir, *it3, uriTable, toolkitTime)) {
                    outOfDateDirectories.insert(dir);
                    break;
                }
            }
        }
    }
}

// return true if out of date
bool PathSearch::checkNonFunction(const string& dir,
                                  const string& path,
                                  const string& opName,
                                  time_t toolkitTime)
{
    string dir_path;
    if (path.substr(0, 7) == "file://")
        dir_path = (dir == ".") ? path.substr(7) : (dir + '/' + path.substr(7));
    else
        dir_path = (dir == ".") ? path : (dir + '/' + path);
    bf::path fileToCheck;
    if (!opName.empty())
        fileToCheck = dir_path + '/' + opName + ".xml";
    else
        fileToCheck = dir_path;
    if (!exists(fileToCheck))
        return true; // must be out of date
    if (bf::last_write_time(fileToCheck) > toolkitTime) {
        return true; // definitely out of date
    }

    return false;
}

bool PathSearch::checkFunctionsFromModel(const string& dir,
                                         const toolkitFunctionType& function,
                                         const URITableType& uriTable,
                                         time_t toolkitTime)
{
    // There is now only a function.xml file for native functions.   Handle the SPL ones quickly
    bf::path d = dir;
    if (!(function.native().present() && function.native().get())) {
        string fileName = fileFromUriIndex(uriTable, function.uriIndex());
        bf::path splFile = Utility::cleanPath(d / fileName);
        // Check the timestamp
        if (!exists(splFile)) {
            return true; // must be out of date
        }
        if (bf::last_write_time(splFile) > toolkitTime) {
            return true; // definitely out of date
        }
        return false;
    }

    // Okay, this is a native function.   We need some information from the file.
    if (!function.modelUriIndex().present())
        return true;
    int32_t uriIndex = function.modelUriIndex().get();
    bf::path toolkitDir(dir);
    bf::path modelFile = toolkitDir / fileFromUriIndex(uriTable, uriIndex);
    string modelFileName = modelFile.string();
    if (bf::last_write_time(modelFileName) > toolkitTime) {
        return true; // definitely out of date
    }
    return false;
}

void PathSearch::checkOneToolkit(unordered_set<string>& outOfDateDirectories, const string& dir)
{
    bf::path dPath = dir;
    bf::path toolkit = dPath / "toolkit.xml";
    ifstream toolkitStr(toolkit.string().c_str(), fstream::in);
    if (!toolkitStr) {
        // We don't have a toolkit.xml.   Do we have a bunch of subdirectories with toolkit.xml(s)?
        bf::directory_iterator end_itr; // default construction yields past-the-end
        if (bf::exists(dPath)) {
            for (bf::directory_iterator it(dPath); it != end_itr; ++it) {
                bf::path subDir(*it);
                if (is_directory(subDir)) {
                    bf::path tk = subDir / "toolkit.xml";
                    if (exists(tk) && !is_directory(tk)) {
                        ifstream tkStr(tk.string().c_str(), fstream::in);
                        if (tkStr) {
                            checkOneDirectory(outOfDateDirectories, tkStr, tk.string(),
                                              subDir.string());
                        }
                    }
                }
            }
        }
    } else {
        checkOneDirectory(outOfDateDirectories, toolkitStr, toolkit.string(), dir);
    }
}

void PathSearch::checkToolkitList(unordered_set<string>& outOfDateDirectories,
                                  const string& fileName)
{
    // Check this toolkit list
    SPLDBG("Starting checking toolkit list " << fileName, Debug::Core);
    using namespace xmlns::prod::streams::spl::toolkitList;
    ifstream toolkitListStr(fileName.c_str(), fstream::in);
    if (!toolkitListStr) {
        SPLDBG("Unable to open " << fileName, Debug::Core);
        SysOut::errorln(SPL_CORE_CANNOT_OPEN_FILE(fileName, RuntimeUtility::getErrorNoStr()));
        return;
    }
    auto_ptr<toolkitListType> tkList;
    string xsddir = _config.getStringValue(CompilerConfiguration::XsdDir);

    // Need the directory name for relative paths
    bf::path dir = fileName;
    dir = dir.branch_path();

    string toolkitListSchemaName((bf::path(xsddir) / "toolkitListModel.xsd").string());
    XMLDiag::XMLDiagPrinter errorPrinter(fileName);
    try {
        ::xml_schema::properties props;
        props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/toolkitList",
                              toolkitListSchemaName);
        tkList = toolkitList(toolkitListStr, errorPrinter, 0, props);
    } catch (::xsd::cxx::exception& e) {
        SPLDBG("Error checking toolkit list " << fileName, Debug::Core);
        SysOut::errorln(SPL_CORE_ERROR_LOADING_TKLIST_MODEL(e.what()),
                        SourceLocation(fileName, 0, 0));
        return;
    }

    // Walk the information in the toolkit list
    const toolkitListType::toolkit_sequence& tks = tkList->toolkit();
    for (toolkitListType::toolkit_const_iterator it = tks.begin(); it != tks.end(); it++) {
        const xmlns::prod::streams::spl::toolkitList::toolkitType& tk = *it;
        if (tk.directory().present()) {
            bf::path directory = tk.directory().get();
            if (!directory.is_complete())
                directory = dir / directory;
            checkOneToolkit(outOfDateDirectories, directory.string());
        }
        if (tk.listFile().present()) {
            bf::path listFile = tk.listFile().get();
            if (!listFile.is_complete())
                listFile = dir / listFile;
            checkToolkitList(outOfDateDirectories, listFile.string());
        }
    }
    SPLDBG("Finished checking toolkit list " << fileName, Debug::Core);
}

static int nsSortFcn(const toolkitNamespaceType* a, const toolkitNamespaceType* b)
{
    return a->name() < b->name();
}

bool PathSearch::processOneDirectory(ifstream& toolkitStr,
                                     const string& toolkit,
                                     const string& dir,
                                     unordered_map<string, uint32_t>& toolkitNames,
                                     bool& success)
{
    toolkitModelType* model;
    try {
        model = toolkitModel(toolkitStr, xsd::cxx::tree::flags::dont_validate).release();
        _modelsToDelete.push_back(model);
    } catch (::xsd::cxx::exception& e) {
        SysOut::errorln(SPL_CORE_UNABLE_TO_LOAD_TOOLKIT_XML(toolkit, e.what()));
        return false;
    }

    // And the info.xml
    using namespace ToolkitInfo;
    string xsddir = _config.getStringValue(CompilerConfiguration::XsdDir);
    bf::path toolkitInfo = bf::path(dir) / "info.xml";
    ifstream toolkitInfoStr(toolkitInfo.string().c_str(), fstream::in);
    auto_ptr<ToolkitInfoModel> infoModel;
    ToolkitInfoModelPtr infoPtr;
    if (toolkitInfoStr) {
        string toolkitInfoSchemaName((bf::path(xsddir) / "toolkitInfoModel.xsd").string());
        XMLDiag::XMLDiagPrinter errorPrinter(toolkitInfo.string());
        try {
            ::xml_schema::properties props;
            props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/toolkitInfo",
                                  toolkitInfoSchemaName);
            auto_ptr<toolkitInfoModelType> rawToolkitInfoModel =
              toolkitInfoModel(toolkitInfoStr, errorPrinter, 0, props);
            infoPtr = ToolkitInfoModelPtr(new ToolkitInfo::ToolkitInfoModel(*rawToolkitInfoModel));
        } catch (::xsd::cxx::exception& e) {
            SysOut::errorln(SPL_CORE_ERROR_LOADING_INFO_MODEL(e.what()),
                            SourceLocation(toolkitInfo.string(), 0, 0));
        }
    }

    // Timestamp handling
    time_t toolkitTime = bf::last_write_time(bf::path(toolkit));

    // Walk all the toolkits in the model
    const toolkitModelType::toolkit_sequence& toolkits1 = model->toolkit();
    for (toolkitModelType::toolkit_const_iterator it = toolkits1.begin(); it != toolkits1.end();
         it++) {
        const toolkitType& tt = *it;

        // Remember the toolkits that we need
        const string& tkName = tt.name();
        ToolkitMap::iterator tkIt = _allToolkits.find(tkName);

        VersionInfo version(it->version());
        if (tkIt == _allToolkits.end()) {
            // First time for this toolkit.   Enter the info
            SPLPRODDBG("Adding toolkit " << tkName << ", file " << toolkit << ", description: "
                                         << it->description() << ", version " << it->version(),
                       Debug::Core);
            Toolkit& newToolkit =
              _toolkitFactory.createToolkit(tt, dir, toolkitTime, toolkit, infoPtr);
            _allToolkits.insert(
              make_pair(tkName, vector<OneToolkit>(1, make_pair(version, &newToolkit))));
        } else {
            // We prefer the one in '.' if we have it
            vector<OneToolkit>& tkts = tkIt->second;
            if (tkts[0].second->path().string() == ".") {
                SysOut::warnln(SPL_CORE_SKIPPING_TOOLKIT(tkName), SourceLocation(toolkit, 0, 0));
                SPLPRODDBG("Skipping duplicate (of .) toolkit "
                             << tkName << ", file " << toolkit << ", description: "
                             << it->description() << ", version " << it->version(),
                           Debug::Core);
                continue;
            }

            // Have we seen exactly this version before?
            bool isDuplicate = false;
            for (uint32_t i = 0, ui = tkts.size(); i < ui; i++) {
                if (version == tkts[i].first) {
                    SPLPRODDBG("Skipping duplicate toolkit "
                                 << tkName << ", file " << toolkit << ", description: "
                                 << it->description() << ", version " << it->version(),
                               Debug::Core);
                    isDuplicate = true;
                    break;
                }
            }
            if (isDuplicate)
                continue;

            // Just remember this
            SPLPRODDBG("Adding toolkit " << tkName << ", file " << toolkit << ", description: "
                                         << it->description() << ", version " << it->version(),
                       Debug::Core);
            Toolkit& newToolkit =
              _toolkitFactory.createToolkit(tt, dir, toolkitTime, toolkit, infoPtr);
            tkts.push_back(make_pair(version, &newToolkit));
        }
    }
    return true;
}

bool PathSearch::loadTheToolkits()
{
    bool success = true;
    for (uint32_t i = 0, n = _toolkits.size(); i < n; i++) {
        const Toolkit& it = *_toolkits[i];
        const Toolkit::ToolkitModel& tk = it.model();
        const string& tkName = tk.name();
        const string& dir = it.path().string();
        bool isBuiltinToolkitDir =
          (bf::canonical(bf::path(dir)) ==
           bf::canonical(bf::path(_config.getStringValue(CompilerConfiguration::ToolkitDir))));
        // Now load each toolkit
        SPLPRODDBG("Loading toolkit " << tkName << ", file " << it.file() << ", description: "
                                      << tk.description() << ", version " << tk.version(),
                   Debug::Core);

        // Load the URI table
        URITableType uriTable;
        if (tk.uriTable().present()) {
            const uriTableType& table = tk.uriTable().get();
            const uriTableType::uri_sequence& uris = table.uri();
            for (uriTableType::uri_const_iterator it1 = uris.begin(); it1 != uris.end(); it1++) {
                uriTable.insert(make_pair(it1->index(), Utility::uriDecode(it1->value())));
            }
        }

        // In order to have a repeatable order for builtin function evaluation, we need
        // to process the namespaces in a known order.   Sort them by namespace name
        const toolkitType::namespace_sequence& namespaces = tk.namespace_();
        vector<const toolkitNamespaceType*> sortedNamespaces;
        sortedNamespaces.reserve(namespaces.size());
        for (toolkitType::namespace_const_iterator it2 = namespaces.begin();
             it2 != namespaces.end(); ++it2) {
            sortedNamespaces.push_back(&*it2);
        }
        std::sort(sortedNamespaces.begin(), sortedNamespaces.end(), nsSortFcn);

        // Now walk them in a known order
        for (vector<const toolkitNamespaceType*>::const_iterator it2 = sortedNamespaces.begin();
             it2 != sortedNamespaces.end(); ++it2) {
            const toolkitNamespaceType* ns = *it2;

            const string& nsName = ns->name();
            SPLDBG("Process namespace " << nsName, Debug::Core);
            _toolkitNamespaces.insert(nsName);

            bool builtinTkDir = nsName == "spl.XML" ? false : isBuiltinToolkitDir;

            // Walk the operators
            const toolkitNamespaceType::primitiveOp_sequence& prims = ns->primitiveOp();
            for (toolkitNamespaceType::primitiveOp_const_iterator it3 = prims.begin();
                 it3 != prims.end(); ++it3) {
                URITableType::const_iterator it4 = uriTable.find(it3->modelUriIndex());
                if (it4 != uriTable.end()) {
                    success &= addNonFunction(dir, ns, &*it3, it4->second, it.file(), i);
                }
            }

            const toolkitNamespaceType::compositeOp_sequence& comps = ns->compositeOp();
            for (toolkitNamespaceType::compositeOp_const_iterator it3 = comps.begin();
                 it3 != comps.end(); ++it3) {
                URITableType::const_iterator it4 = uriTable.find(it3->uriIndex());
                if (it4 != uriTable.end()) {
                    success &= addNonFunction(dir, ns, &*it3, it4->second, it.file(), i);
                }
            }

            // Types too
            const toolkitNamespaceType::type_sequence& types = ns->type();
            for (toolkitNamespaceType::type_const_iterator it3 = types.begin(); it3 != types.end();
                 ++it3) {
                URITableType::const_iterator it4 = uriTable.find(it3->uriIndex());
                if (it4 != uriTable.end()) {
                    success &= addNonFunction(dir, ns, it4->second, it3->name(), it3->line(),
                                              it3->column(), false, it.file(), i);
                }
            }

            // And enums
            const toolkitNamespaceType::enumLiterals_sequence& enums = ns->enumLiterals();
            for (toolkitNamespaceType::enumLiterals_const_iterator it3 = enums.begin();
                 it3 != enums.end(); ++it3) {
                const enumLiteralsType::enum_sequence& lits = it3->enum_();
                for (enumLiteralsType::enum_const_iterator it4 = lits.begin(); it4 != lits.end();
                     it4++) {
                    URITableType::const_iterator it5 = uriTable.find(it3->uriIndex());
                    if (it5 != uriTable.end()) {
                        success &= addNonFunction(dir, ns, it5->second, *it4, it3->line(),
                                                  it3->column(), false, it.file(), i);
                    }
                }
            }

            // Now walk the functions
            const toolkitNamespaceType::function_sequence& splFunctions = ns->function();
            for (toolkitNamespaceType::function_const_iterator it3 = splFunctions.begin();
                 it3 != splFunctions.end(); ++it3) {
                success &=
                  addFunctionsFromModel(dir, ns, *it3, it.file(), builtinTkDir, uriTable, i);
            }
        }
        SPLDBG("Finished loading toolkit file " << it.file(), Debug::Core);
    }
    return success;
}

bool PathSearch::addFunctionsFromModel(const string& dir,
                                       const toolkitNamespaceType* ns,
                                       const toolkitFunctionType& function,
                                       const string& toolkit,
                                       bool isBuiltinToolkitDir,
                                       const URITableType& uriTable,
                                       uint32_t toolkitIndex)
{
    const string& ns_name = ns->name();
    bool success = true;

    // There is now only a function.xml file for native functions.   Handle the SPL ones quickly
    bf::path d = dir;
    if (!(function.native().present() && function.native().get())) {
        string fileName = fileFromUriIndex(uriTable, function.uriIndex());
        bf::path splFile = Utility::cleanPath(d / fileName);
        // Check the timestamp
        if (!exists(splFile)) {
            SysOut::errorln(SPL_CORE_SPL_FILE_MISSING(splFile.string(), toolkit));
            return false;
        }
        if (bf::last_write_time(splFile) > _toolkits[toolkitIndex]->time() &&
            !_config.getBooleanValue(CompilerConfiguration::IgnoreTimestamps)) {
            SysOut::errorln(SPL_CORE_TOOLKIT_OLDER_THAN_XML(toolkit, splFile.string()));
            return false;
        }
        int line = function.line().present() ? function.line().get() : 0;
        int col = function.column().present() ? function.column().get() : 0;
        FunctionHeadSym& f = FunctionHeadSym::analyzeFunctionFromToolkitModel(
          ns_name, function.prototype(), fileName, splFile.string(), line, col, "", "", false, NULL,
          -1, toolkitIndex, "", "");
        SPLDBG("Add SPL function " << f, Debug::Core);
        _locMap.insert(make_pair(SourceLocation::fileName2idx(splFile.string()), toolkit));
        return true;
    }

    // Okay, this is a native function.   We need some information from the file.
    // Have we already loaded this file?
    if (!function.modelUriIndex().present())
        return false;
    int32_t uriIndex = function.modelUriIndex().get();
    bf::path toolkitDir(dir);
    bf::path modelFile = toolkitDir / fileFromUriIndex(uriTable, uriIndex);
    string modelFileName = modelFile.string();
    if (_nativeFunctionModelsSeen.count(modelFileName) > 0)
        return true;

    // Check timestamp
    if (bf::last_write_time(modelFile) > _toolkits[toolkitIndex]->time() &&
        !_config.getBooleanValue(CompilerConfiguration::IgnoreTimestamps)) {
        SysOut::errorln(SPL_CORE_TOOLKIT_OLDER_THAN_XML(toolkit, modelFileName));
        _nativeFunctionModelsSeen.insert(modelFileName);
        return false;
    }

    // Open the file, and process all the information in it.  We will
    _nativeFunctionModelsSeen.insert(modelFileName);
    ifstream istr(modelFileName.c_str(), fstream::in);
    if (!istr) {
        SysOut::errorln(SPL_CORE_XML_MISSING(modelFileName, toolkit));
        return false;
    }
    using namespace Function;
    using namespace Common;
    auto_ptr<functionModelType> rawModel;
    string xsddir = _config.getStringValue(CompilerConfiguration::XsdDir);
    string schemaName((bf::path(xsddir) / "functionModel.xsd").string());
    XMLDiag::XMLDiagPrinter errorPrinter(modelFileName);
    try {
        ::xml_schema::properties props;
        props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/function", schemaName);
        rawModel = functionModel(istr, errorPrinter, 0, props);
    } catch (::xsd::cxx::exception& e) {
        SysOut::errorln(SPL_CORE_UNABLE_TO_LOAD_FUNCTION_MODEL(e.what()),
                        SourceLocation(modelFileName, 0, 0));
        return false;
    }
    auto_ptr<FunctionModel> model;
    try {
        model.reset(new FunctionModel(*rawModel, modelFile.branch_path().string()));
    } catch (SPLCompilerException& e) {
        SysOut::errorln(SPL_CORE_UNABLE_TO_LOAD_TOOLKIT(modelFileName, e.getExplanation()));
        return false;
    }

    // We now have the function model.  Load the information found there
    const vector<FunctionSetPtr>& funcSets = model->getFunctionSets();
    uint32_t numSets = funcSets.size();

    // Remember which function number we are for figuring out the interface to
    // the expression evaluator for builtins
    string modelDir = modelFile.branch_path().string();
    for (uint32_t i = 0; i < numSets; i++) {
        // each set may have a list of dependencies
        FunctionSet& fs = *funcSets[i];

        string headerFile = fs.getHeaderFileName();
        vector<Function::LibraryPtr>* deps = NULL;
        vector<LibraryPtr>& theDeps = fs.getDependencies();
        if (!theDeps.empty()) {
            deps = new vector<LibraryPtr>(theDeps);
            _dependencies.push_back(deps);
        }
        // each set has a list of functions, and (possibly) dependencies
        string cppNamespace = fs.hasCppNamespaceName() ? fs.getCppNamespaceName() : "";
        const vector<Function::CppNameLocation>& functions = fs.getFunctions();
        for (uint32_t j = 0; j < functions.size(); j++) {
            const Function::CppNameLocation& f = functions[j];
            const string& source = f.name();
            if (isBuiltinToolkitDir)
                _builtinFcnNumber++;
            string cppFcnName = f.hasCppName() ? f.getCppName() : "";
            FunctionHeadSym& fcn = FunctionHeadSym::analyzeFunctionFromToolkitModel(
              ns_name, source, modelFileName, "", functions[j].line(), functions[j].col(),
              headerFile, modelDir, true, deps, _builtinFcnNumber, toolkitIndex, cppNamespace,
              cppFcnName);
            if (isBuiltinToolkitDir)
                SPLDBG("Add evaluatable (" << _builtinFcnNumber << ") native function " << fcn,
                       Debug::Core);
            else
                SPLDBG("Add native function " << fcn, Debug::Core);
        }
    }
    return success;
}

string PathSearch::fileFromUriIndex(const URITableType& uriTable, int uriIndex)
{
    URITableType::const_iterator it = uriTable.find(uriIndex);
    if (it == uriTable.end())
        return "<missing file>";
    if (it->second.substr(0, 7) == "file://")
        return it->second.substr(7);
    return it->second;
}

bool PathSearch::addNonFunction(const string& dir,
                                const toolkitNamespaceType* ns,
                                const compositeToolkitOperatorType* op,
                                const std::string& path,
                                const string& toolkit,
                                uint32_t toolkitIndex)
{
    return addNonFunction(dir, ns, path, op->name(), op->line(), op->column(), false, toolkit,
                          toolkitIndex);
}

bool PathSearch::addNonFunction(const string& dir,
                                const toolkitNamespaceType* ns,
                                const primitiveToolkitOperatorType* op,
                                const std::string& path,
                                const string& toolkit,
                                uint32_t toolkitIndex)
{
    return addNonFunction(dir, ns, path, op->name(), 0, 0, true, toolkit, toolkitIndex);
}

bool PathSearch::addNonFunction(const string& dir,
                                const toolkitNamespaceType* ns,
                                const string& path,
                                const string& symbolName,
                                uint32_t line,
                                uint32_t column,
                                bool isPrimitiveOperator,
                                const string& toolkit,
                                uint32_t toolkitIndex)
{
    const string& ns_name = ns->name();
    string name;
    if (ns_name.size() == 0)
        name = symbolName;
    else
        name = ns_name + "::" + symbolName;
    NameLocation nl(name, line, column);
    unordered_map<NameLocation, NonFunctionResult*>::const_iterator it = _nonFunctionMap.find(nl);
    string dir_path;
    if (path.substr(0, 7) == "file://")
        dir_path = (dir == ".") ? path.substr(7) : (dir + '/' + path.substr(7));
    else
        dir_path = (dir == ".") ? path : (dir + '/' + path);
    if (it == _nonFunctionMap.end()) {
        bf::path fileToCheck;
        if (isPrimitiveOperator) {
            fileToCheck = dir_path + '/' + symbolName + ".xml";
            if (!exists(fileToCheck)) {
                SysOut::errorln(SPL_CORE_XML_MISSING(fileToCheck.string(), toolkit));
                return false;
            }
        } else {
            fileToCheck = dir_path;
            if (!exists(fileToCheck)) {
                SysOut::errorln(SPL_CORE_SPL_FILE_MISSING(fileToCheck.string(), toolkit));
                return false;
            }
        }
        if (bf::last_write_time(fileToCheck) > _toolkits[toolkitIndex]->time() &&
            !_config.getBooleanValue(CompilerConfiguration::IgnoreTimestamps)) {
            SysOut::errorln(SPL_CORE_TOOLKIT_OLDER_THAN_XML(toolkit, fileToCheck.string()));
            return false;
        }
        SPLDBG("Add non-function " << name << ", path: " << dir_path, Debug::Core);
        NonFunctionResult* r =
          new NonFunctionResult(addString(&dir_path), isPrimitiveOperator, toolkitIndex);
        _nonFunctionMap.insert(make_pair(nl, r));
        if (!isPrimitiveOperator)
            _locMap.insert(make_pair(SourceLocation::fileName2idx(dir_path), toolkit));
        return true;
    }

    // Already have seen this.  Complain
    SourceLocation newLoc(dir_path, line, column);
    if (0 == ns_name.size())
        SysOut::errorln(SPL_CORE_DUPLICATE_TOOLKIT_NAME_DEFAULTNS(nl.name()), newLoc);
    else
        SysOut::errorln(SPL_CORE_DUPLICATE_TOOLKIT_NAME(name, ns_name), newLoc);
    SourceLocation oldLoc(it->second->getPath(), it->first.line(), it->first.col());
    SysOut::detailsErrorln(SPL_FRONT_END_PREVIOUS_LOCATION(name), oldLoc);

    return false;
}

string PathSearch::findResourcePath(const string& operatorName)
{
    string path;

    NameLocation nl(operatorName, 0, 0);
    unordered_map<NameLocation, NonFunctionResult*>::const_iterator it = _nonFunctionMap.find(nl);
    if (it != _nonFunctionMap.end()) {
        NonFunctionResult* r = it->second;
        uint32_t tkIndex = r->getToolkitIndex();
        const Toolkit& tk = *_toolkits[tkIndex];
        bf::path p = bf::path(tk.file()).branch_path();
        path = p.string();
    }

    return path;
}

const Operator::OperatorModel* PathSearch::findOperatorModel(const string& name, string* directory)
{
    // Have we looked this one up before?
    unordered_map<string, ModelMap>::const_iterator omit = _operatorModelMap.find(name);
    if (omit != _operatorModelMap.end()) {
        if (directory)
            *directory = omit->second.second;
        return omit->second.first;
    }

    // Haven't seen it before - Lookup name in our map
    NameLocation nl(name, 0, 0);
    unordered_map<NameLocation, NonFunctionResult*>::const_iterator it = _nonFunctionMap.find(nl);
    if (it == _nonFunctionMap.end())
        return NULL;
    NonFunctionResult* r = it->second;

    // We found one.  Is it a primitive operator?
    if (!r->isPrimitiveOperator())
        return NULL;

    // We have the operator.  Use the directory to get the XML
    string opName = name;
    size_t nameSpaceEnd = name.rfind("::");
    if (nameSpaceEnd != string::npos)
        opName = name.substr(nameSpaceEnd + 2);
    bf::path path(r->getPath());
    bf::path origPath(path);
    path /= opName + ".xml";
    SPLDBG("Loading operator model file" << path.string(), Debug::Core);
    ifstream istr(path.string().c_str(), fstream::in);
    if (!istr) {
        SPLDBG("Unable to open " << path.string(), Debug::Core);
        SysOut::die(SPL_CORE_CANNOT_OPEN_FILE(path.string(), RuntimeUtility::getErrorNoStr()));
        // Note: Front end cannot tolerate this
        return NULL;
    }
    // Load the operator model from the file
    using namespace xmlns::prod::streams::spl::operator_;

    auto_ptr<operatorModelType> rawModel;
    string xsddir = _config.getStringValue(CompilerConfiguration::XsdDir);
    string schemaName((bf::path(xsddir) / "operatorModel.xsd").string());
    XMLDiag::XMLDiagPrinter errorPrinter(path.string());
    try {
        ::xml_schema::properties props;
        props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/operator", schemaName);
        rawModel = operatorModel(istr, errorPrinter, 0, props);
    } catch (::xsd::cxx::exception& e) {
        SPLDBG("Unable to load Operator Model XML " << path.string() << ": " << e.what(),
               Debug::Core);
        SysOut::die(SPL_CORE_ERROR_LOADING_OPERATOR_MODEL(e.what()),
                    SourceLocation(path.string(), 0, 0));
        // Note: Front end cannot tolerate this
        return NULL;
    }

    // Convert
    Operator::OperatorModel* model;
    bf::path opModelPath = origPath / opName;
    try {
        model = new Operator::OperatorModel(*rawModel, opModelPath.branch_path().string());
    } catch (SPLCompilerException& e) {
        SPLDBG("Unable to load Operator Model XML " << path.string() << ": " << e.getExplanation(),
               Debug::Core);
        SysOut::die(SPL_CORE_ERROR_CHECKING_OPERATOR_MODEL(e.getExplanation()),
                    SourceLocation(opModelPath.string(), 0, 0));
        // Note: Front end cannot tolerate this
        return NULL;
    }

    // Let the bells ring!   We have seen the light and the model is here!
    SPLDBG("Operator model file loaded", Debug::Core);

    // Remember this for next time.
    _operatorModelMap.insert(make_pair(name, make_pair(model, origPath.string())));
    if (directory)
        *directory = origPath.string();
    return model;
}

const PathSearch::NonFunctionResult* PathSearch::findNonFunction(string const& namespaceName,
                                                                 string const& symbolName)
{
    // Lookup name in our map
    string name;
    if (namespaceName.empty())
        name = symbolName;
    else
        name = namespaceName + "::" + symbolName;

    NameLocation nl(name, 0, 0);
    unordered_map<NameLocation, NonFunctionResult*>::const_iterator it = _nonFunctionMap.find(nl);
    if (it == _nonFunctionMap.end())
        return NULL;
    return it->second;
}

OverloadedFunctionsSym* PathSearch::findFunctionGroup(string const& nameSpaceName,
                                                      string const& simpleName)
{
    Scope& topLevelScope = SymbolTable::instance().topLevelScope();
    if (!topLevelScope.has(nameSpaceName))
        return NULL;
    Scope* nameSpaceScope = topLevelScope.get(nameSpaceName).heldScope();
    return &nameSpaceScope->get(simpleName).as<OverloadedFunctionsSym>();
}

bool PathSearch::mapSourceLocationToToolkitDirectory(const SourceLocation& loc,
                                                     string& toolkitDirectory)
{
    LocToTookitType::const_iterator it = _locMap.find(loc.fileNameIdx());
    if (it != _locMap.end()) {
        toolkitDirectory = bf::path(it->second).branch_path().string();
        if (toolkitDirectory.empty())
            toolkitDirectory = '.';
        return true;
    }
    return false;
}

bool PathSearch::isInStandardToolkit(const SourceLocation& loc)
{
    CompilerConfiguration const& config = CompilerConfiguration::instance();
    bf::path standardToolkitPath(config.getStringValue(CompilerConfiguration::ToolkitDir));
    LocToTookitType::const_iterator it = _locMap.find(loc.fileNameIdx());
    if (it == _locMap.end()) {
        string s = boost::filesystem::canonical(standardToolkitPath).string();
        bf::path fileName(loc.fileName());
        bf::path canonized = boost::filesystem::canonical(fileName);
        if (0 == canonized.string().compare(0, s.size(), s)) {
            return true;
        }
    } else {
        bf::path toolkitModelPath(it->second);
        bf::path toolkitPath(toolkitModelPath.branch_path());
        if (boost::filesystem::equivalent(toolkitPath, standardToolkitPath))
            return true;
    }
    return false;
}

Solution::Solution(const std::vector<const Toolkit*>& path)
  : _path(path)
{
    for (int32_t i = 0, size = _path.size(); i < size; ++i) {
        const Toolkit& tk = *_path[i];
        _map.insert(make_pair(tk.name(), tk.version()));
    }
}

int Solution::compareTo(const Solution& other) const
{
    for (int32_t i = 0, size = _path.size(); i < size; ++i) {
        const Toolkit& tk = *_path[i];
        VersionInfo vi(tk.version());
        unordered_map<std::string, std::string> map = other.getToolkitMap();
        unordered_map<std::string, std::string>::const_iterator toolkit = map.find(tk.name());
        if (toolkit == map.end()) {
        } else {
            VersionInfo otherVI(toolkit->second);
            if (vi == otherVI) {
            } else if (vi > otherVI) {
                return 1;
            } else {
                return -1;
            }
        }
    }
    return 0;
}

void Solution::print(ostream& ostr) const
{
    ostr << "Solution is:";
    for (int32_t i = 0, size = _path.size(); i < size; ++i) {
        const Toolkit& tk = *_path[i];
        if (0 != i)
            ostr << "|";
        ostr << tk.name() << "(" << tk.version() << ")";
    }
}
