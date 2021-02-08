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

#ifndef SPL_TOOLKIT_H
#define SPL_TOOLKIT_H

#include <SPL/Core/ToolkitInfoModelImpl.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <UTILS/HashMapHelpers.h>

#include <boost/filesystem/path.hpp>
#include <string>
#include <time.h>

namespace xmlns {
namespace prod {
namespace streams {
namespace spl {
namespace toolkit {
class toolkitType;
}
}
}
}
}

namespace SPL {

class VersionInfo
{
  public:
    VersionInfo()
      : _major(0)
      , _minor(0)
      , _micro(0)
    {}
    VersionInfo(const std::string& v);
    VersionInfo(uint32_t major, uint32_t minor, uint32_t micro, const std::string& build)
      : _major(major)
      , _minor(minor)
      , _micro(micro)
      , _build(build)
    {}
    bool operator>(const VersionInfo& rhs) const;
    bool operator>=(const VersionInfo& rhs) const;
    bool operator==(const VersionInfo& rhs) const;

  private:
    uint32_t _major, _minor, _micro;
    std::string _build;
};

struct VersionRange
{
    VersionRange(const std::string& r);

    bool inRange(const VersionInfo& v);

  private:
    VersionInfo _lower, _upper;
    bool _noUpper;
    bool _lowerEqualBound;
    bool _upperEqualBound;
};

typedef boost::filesystem::path Path;

class BundleInclude
{
  public:
    /// The root directory under which the subdirectories matching the regEx are to be found.
    enum RootType
    {
        ToolkitDir,
        ApplicationDir,
        OutputDir
    };

    BundleInclude() {}
    BundleInclude(const std::string& regEx_, RootType root_)
      : _regEx(regEx_)
      , _root(root_)
    {}

    /// Get the regex associated with this include
    const std::string& regEx() const { return _regEx; }

    /// Get the relative root for this include
    RootType root() const { return _root; }

  private:
    std::string _regEx;
    RootType _root;
};

/// Represents a toolkit seen during compilation
class Toolkit
{
  public:
    typedef const xmlns::prod::streams::spl::toolkit::toolkitType ToolkitModel;
    typedef ToolkitInfo::ToolkitInfoModel ToolkitInfoModel;

    // Can only construct objects in boost pools with 3 or less parameters :-(
    struct Params
    {
        Params(const ToolkitModel& model,
               const Path& path,
               const time_t& time,
               const std::string& file,
               const ToolkitInfo::ToolkitInfoModelPtr& infoModel)
          : _model(model)
          , _path(path)
          , _time(time)
          , _file(file)
          , _infoModel(infoModel)
        {}
        const ToolkitModel& _model;
        const Path& _path;
        const time_t& _time;
        const std::string& _file;
        const ToolkitInfo::ToolkitInfoModelPtr _infoModel;
    };

    Toolkit(const Params& params);

    /// Returns the name of the toolkit
    /// @return The toolkit name
    const std::string& name() const;

    /// Returns the version of the toolkit
    /// @return Returns the version of the toolkit
    const std::string& version() const;

    /// Returns the path to the toolkit's root directory.
    /// @return The toolkit's path
    const Path& path() const { return _path; }

    /// Get the timestamp for the model file describing this toolkit
    /// @return Returns the timestamp
    const time_t& time() const { return _time; }

    /// Get the toolkit model associated with this toolkit.
    /// @return Returns the toolkit model
    const ToolkitModel& model() const { return _model; }

    /// Get the info model associated with this toolkit.  There may not be one
    /// @return Returns the info model or NULL
    const ToolkitInfoModel* infoModel() const { return _infoModel.get(); }

    /// Get the name of the model file containning this toolkit
    /// @return Return the file name
    const std::string& file() const { return _file; }

    /// Determine if this toolkit has a dependency on the specified toolkit
    /// @return Returns true if this toolkit depends on the specified toolkit, otherwise false.
    bool dependsOn(const Toolkit& toolkit) const;

    /// Determine if this toolkit satisfies the dependencies of the specified toolkit
    /// Satisfaction is defined as a) the specified toolkit does not depend on this toolkit
    ///                            b) the specified toolkit depends on this toolkit and the version of this toolkit is within the range of toolkits
    ///                               allowed by the specified toolkit
    /// @return Returns true if this toolkit satisfies the specified toolkit's dependencies, otherwise false.
    bool satisfiesDependencies(const Toolkit& toolkit) const;

    /// Determine if this toolkit is the toolkit being compiled
    /// @return Returns true if this is the current toolkit, otherwise false.
    bool isCurrentToolkit() const;

    /// Determine if this toolkit is the SPL standard library toolkit
    /// @return Returns true if this is the SPL toolkit, otherwise false.
    bool isSPLToolkit() const;

    const std::vector<BundleInclude>& bundleIncludes() const { return _bundleIncludes; }
    const std::vector<std::string>& bundleExcludes() const { return _bundleExcludes; }

  private:
    Toolkit(const Toolkit& tk); // Disable copy construction

    ToolkitModel& _model; // the toolkit model (from the toolkit.xml file)
    Path _path;           // the root directory of the toolkit
    time_t _time;         // timestamp of the model file
    std::string _file;    // The toolkit filename
    const ToolkitInfo::ToolkitInfoModelPtr _infoModel; // The toolkit info model
    std::vector<BundleInclude> _bundleIncludes;
    std::vector<std::string> _bundleExcludes;
};

/// The ToolkitFactory class creates Toolkit objects and deletes them on shutdown
class ToolkitFactory : public Singleton<ToolkitFactory>
{
  public:
    ToolkitFactory();
    ~ToolkitFactory();

    /// Creates an instance of a toolkit and adds it to the toolkit map
    static Toolkit& createToolkit(const Toolkit::ToolkitModel& model,
                                  const Path& path,
                                  const time_t& time,
                                  const std::string& file,
                                  const ToolkitInfo::ToolkitInfoModelPtr& infoModel);
};

} // namespace SPL

#endif // SPL_TOOLKIT_H
