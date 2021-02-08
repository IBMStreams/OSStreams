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

#include <SPL/Core/Toolkit.h> // include this header first

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/toolkitInfoModel.h>
#include <SPL/Core/toolkitModel.h>
#include <SPL/Utility/Utility.h>

#include <SPL/Runtime/Utility/Singleton.t>
#include <UTILS/SupportFunctions.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/pool/object_pool.hpp>

#include <cassert>
using namespace std;
using namespace xmlns::prod::streams::spl::toolkitInfo;

namespace bf = boost::filesystem;

namespace SPL {
template class Singleton<ToolkitFactory>;
static boost::object_pool<Toolkit> toolkitPool;

Toolkit::Toolkit(const Params& params)
  : _model(params._model)
  , _path(params._path)
  , _time(params._time)
  , _file(params._file)
  , _infoModel(params._infoModel)
{

    if (_model.sabFiles().present()) {
        const ToolkitModel::sabFiles_type& sabFiles = _model.sabFiles().get();
        const sabFilesType::include_sequence& includes = sabFiles.include();
        for (sabFilesType::include_const_iterator it = includes.begin(), itEnd = includes.end();
             it != itEnd; ++it) {
            const includeType& include = *it;
            string antPattern = include.path();
            string regEx = Utility::convertAntPatternToRegEx(antPattern);

            BundleInclude::RootType root = BundleInclude::ToolkitDir;
            if (include.root().present()) {
                const string& rootType = include.root().get();
                if (rootType == "applicationDir")
                    root = BundleInclude::ApplicationDir;
                else if (rootType == "outputDir")
                    root = BundleInclude::OutputDir;
                // else it is the toolkit dir
            }
            _bundleIncludes.push_back(BundleInclude(regEx, root));
        }
        const sabFilesType::exclude_sequence& excludes = sabFiles.exclude();
        for (sabFilesType::exclude_const_iterator it1 = excludes.begin(), it1End = excludes.end();
             it1 != it1End; ++it1) {
            const excludeType& exclude = *it1;
            const string& antPattern = exclude.path();
            string regEx = Utility::convertAntPatternToRegEx(antPattern);
            _bundleExcludes.push_back(regEx);
        }
    }
}

const string& Toolkit::name() const
{
    return _model.name();
}

const string& Toolkit::version() const
{
    return _model.version();
}

bool Toolkit::dependsOn(const Toolkit& toolkit) const
{
    const Toolkit::ToolkitInfoModel* info = infoModel();
    if (info) {
        const vector<ToolkitInfo::DependencyPtr>& deps = info->getDependencies();
        for (uint32_t i = 0, size = deps.size(); i < size; i++) {
            const ToolkitInfo::Dependency& dep = *deps[i];
            const string& depName = dep.getName();
            if (depName == toolkit.name()) {
                VersionInfo vi(toolkit.version());
                VersionRange vr(dep.getVersion());
                if (vr.inRange(vi)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Toolkit::satisfiesDependencies(const Toolkit& toolkit) const
{
    const Toolkit::ToolkitInfoModel* info = toolkit.infoModel();
    if (info) {
        const vector<ToolkitInfo::DependencyPtr>& deps = info->getDependencies();
        for (uint32_t i = 0, size = deps.size(); i < size; i++) {
            const ToolkitInfo::Dependency& dep = *deps[i];
            const string& depName = dep.getName();
            if (depName == name()) {
                VersionInfo vi(version());
                VersionRange vr(dep.getVersion());
                if (!vr.inRange(vi)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool Toolkit::isSPLToolkit() const
{
    return (name() == "spl");
}

bool Toolkit::isCurrentToolkit() const
{
    return (_path.string() == ".");
}

ToolkitFactory::ToolkitFactory()
  : Singleton<ToolkitFactory>(*this)
{}

ToolkitFactory::~ToolkitFactory()
{
    // The toolkits are allocated in a boost pool which will call the destructors
}

Toolkit& ToolkitFactory::createToolkit(const Toolkit::ToolkitModel& model,
                                       const Path& path,
                                       const time_t& time,
                                       const string& file,
                                       const ToolkitInfo::ToolkitInfoModelPtr& infoModel)
{
    Toolkit::Params params(model, path, time, file, infoModel);
    Toolkit* toolkit = toolkitPool.construct(params);

    return *toolkit;
}

VersionInfo::VersionInfo(const string& v)
{
    vector<string> nums;
    Distillery::tokenize(v, nums, ".", true);
    _major = (nums.size() > 0) ? atoi(nums[0].c_str()) : 0;
    _minor = (nums.size() > 1) ? atoi(nums[1].c_str()) : 0;
    _micro = (nums.size() > 2) ? atoi(nums[2].c_str()) : 0;
    _build = (nums.size() > 3) ? nums[3] : "";
}

bool VersionInfo::operator==(const VersionInfo& rhs) const
{
    return _major == rhs._major && _minor == rhs._minor && _micro == rhs._micro &&
           _build == rhs._build;
}

bool VersionInfo::operator>(const VersionInfo& rhs) const
{
    if (_major < rhs._major)
        return false;
    if (_major > rhs._major)
        return true;
    if (_minor < rhs._minor)
        return false;
    if (_minor > rhs._minor)
        return true;
    if (_micro < rhs._micro)
        return false;
    if (_micro > rhs._micro)
        return true;
    if (_build <= rhs._build)
        return false;
    return true;
}

bool VersionInfo::operator>=(const VersionInfo& rhs) const
{
    if (_major < rhs._major)
        return false;
    if (_major > rhs._major)
        return true;
    if (_minor < rhs._minor)
        return false;
    if (_minor > rhs._minor)
        return true;
    if (_micro < rhs._micro)
        return false;
    if (_micro > rhs._micro)
        return true;
    if (_build < rhs._build)
        return false;
    return true;
}

VersionRange::VersionRange(const string& v)
{
    if (v[0] != '[' && v[0] != '(') {
        // single value
        _noUpper = true;
        _lower = VersionInfo(v);
        return;
    }
    // A range
    _noUpper = false;
    _lowerEqualBound = v[0] == '[';
    _upperEqualBound = v[v.size() - 1] == ']';

    // Split into the 2 numbers
    vector<string> versions;
    Distillery::tokenize(v.substr(1, v.size() - 2), versions, ",", true);
    _lower = VersionInfo(versions[0]);
    _upper = VersionInfo(versions[1]);
}

bool VersionRange::inRange(const VersionInfo& v)
{
    if (_noUpper) {
        // Just compare to lower
        return v >= _lower;
    }

    // Need to check bounds properly
    if (_lowerEqualBound) {
        if (!(v >= _lower))
            return false;
    } else {
        if (!(v > _lower))
            return false;
    }

    if (_upperEqualBound) {
        if (_upper >= v)
            return true;
    } else {
        if (_upper > v)
            return true;
    }

    return false;
}

} // namespace SPL
