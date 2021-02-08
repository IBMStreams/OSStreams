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

#ifndef PATH_SEARCH_H
#define PATH_SEARCH_H

#include <SPL/Core/FunctionModelImpl.h>
#include <SPL/Core/Toolkit.h>
#include <SPL/Core/ToolkitInfoModelImpl.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Utility/NameLocation.h>
#include <UTILS/HashMapHelpers.h>
#include <string>
#include <vector>

namespace xmlns {
namespace prod {
namespace streams {
namespace spl {
namespace toolkit {
class toolkitType;
class toolkitModelType;
class toolkitNamespaceType;
class primitiveToolkitOperatorType;
class compositeToolkitOperatorType;
class toolkitFunctionType;
}
}
}
}
}

namespace SPL {
//! Search SPL paths to locate operators in toolkit.xml and SPL files
class CompilerConfiguration;
class FunctionHeadSym;
class OverloadedFunctionsSym;
class SourceLocation;
class FormattableMessage;
namespace Operator {
class OperatorModel;
};

typedef std::pair<VersionInfo, const Toolkit*> OneToolkit;
struct OneDep
{
    OneDep(const std::string& dependentName,
           const std::string& versionRange,
           const std::string& dependeeName,
           const std::string& dependeeVersion)
      : _dependentName(dependentName)
      , _versionRange(versionRange)
      , _dependeeName(dependeeName)
      , _dependeeVersion(dependeeVersion)
      , _consider(true)
    {}
    std::string _dependentName;
    std::string _versionRange;
    std::string _dependeeName;
    std::string _dependeeVersion;
    bool _consider;
};

class Solution
{
  public:
    Solution(const std::vector<const Toolkit*>& path);

    void print(std::ostream& ostr) const;
    const std::vector<const Toolkit*>& toolkits() const { return _path; }

    /// Compare two solutions
    /// - They are equal if EVERY toolkit in Solution A has the same version in Solution B or does
    ///   not exist in Solution B.
    /// - Solution A > Solution B if a toolkit in Solution A that also exists in Solution B such that
    ///   Solution A - toolkit version > Solution B - toolkit version.
    int compareTo(const Solution& other) const;
    const std::tr1::unordered_map<std::string, std::string> getToolkitMap() const { return _map; }

  private:
    std::vector<const Toolkit*> _path;
    std::tr1::unordered_map<std::string, std::string> _map;
};

inline std::ostream& operator<<(std::ostream& str, const Solution& s)
{
    s.print(str);
    return str;
}

class DLL_PUBLIC PathSearch : public Singleton<PathSearch>
{
  public:
    class NonFunctionResult; // result from findNonFunction
    typedef const xmlns::prod::streams::spl::toolkit::toolkitType* ToolkitModelPtr;

    /// Initialization
    /// No work done here
    PathSearch();

    /// Termination
    /// Delete the data in _map
    ~PathSearch();

    /// Set the list of directories to be used
    /// Remove duplicates
    /// Warn about non-directories
    /// @param dirs Mangled name to be shorted
    /// @return 'true' if there is at least one valid directory
    bool setPaths(const std::vector<std::string>& dirs);

    /// Locate all the toolkits
    /// @return 'true' if there were no errors issued
    bool locateToolkits();

    /// Resolve multiple toolkit versions and load the right namespaces
    /// Issue errors if multiple objects in same namespace exist
    /// @param namespaceName namespace of main composite (or empty if none)
    /// @param compositeName name of main composite
    /// @return 'true' if there were no errors issued
    bool resolveToolkits(const std::string& namespaceName, const std::string& compositeName);

    /// Load toolkits when there is no main composite
    /// Issue errors if multiple objects in same namespace exist
    /// @return 'true' if there were no errors issued
    bool loadToolkits();

    /// Resolve the list of toolkits to be loaded. the actual loading will occur
    /// by calling loadToolkits or calling resolveToolkits which will load as well.
    bool resolveToolkitList();

    /// Lookup a fully qualified namespace.name, and return if found
    /// Look through information loaded from toolkit.xml (s)
    /// @param namespaceName Namespace to use to lookup the symbol
    /// @param symbolName symbol we are seeking
    /// @return pointer to NonFunctionResult if found or NULL
    const NonFunctionResult* findNonFunction(const std::string& namespaceName,
                                             const std::string& symbolName);

    /// Return information from a findNonFunction
    /// Contain information from a toolkit.xml file
    class NonFunctionResult
    {
      public:
        /// Constructor
        /// @param path path to directory containing operator XML or path to SPL file
        /// @param isPrimitiveOperator 'true' if this is a primitive operator
        /// @param toolkitIndex Which toolkit did this operator come from?
        ///        0 is current source file
        NonFunctionResult(const std::string* path,
                          bool isPrimitiveOperatorIn,
                          uint32_t toolkitIndex)
          : _path(path)
          , _isPrimitiveOperator(isPrimitiveOperatorIn)
          , _toolkitIndex(toolkitIndex)
        {}

        /// Directory Name
        /// @return fully qualified pathname to directory containing operator or
        ///  pathname to SPL file
        const std::string& getPath() const { return *_path; }

        /// Return whether a found operator is a primitive operator
        /// @return 'true' if the operator is a primitive operator
        bool isPrimitiveOperator() const { return _isPrimitiveOperator; }

        /// Return the toolkit index number for this operator
        /// @return toolkit index
        uint32_t getToolkitIndex() const { return _toolkitIndex; }

      private:
        const std::string* _path;
        bool _isPrimitiveOperator;
        uint32_t _toolkitIndex;
    };

    /// Return the path to the toolkit resource include directory
    /// for the toolkit containing the given operator
    /// @param operatorName Operator within the toolkit we are seeking
    /// @return returns a string pointing the the <toolkit>/impl/nl/include directory
    std::string findResourcePath(const std::string& operatorName);

    /// Lookup a fully qualified namespace::operator, and return the
    /// OperatorModel for it if found
    /// Look through information loaded from toolkit.xml (s)
    /// @param name Operator we are seeking
    /// @param directory (output) directory where operator XML was located
    /// @return pointer to OperatorModel if found or NULL
    const Operator::OperatorModel* findOperatorModel(const std::string& name,
                                                     std::string* directory);

    /// Lookup a fully qualified namespace::function, and return if found
    /// Look through information loaded from toolkit.xml (s)
    /// Several functions may have the same name and different arguments
    /// @param nameSpaceName  Namespace to use to lookup the operator
    /// @param simpleName   Function we are seeking
    /// @return 'NULL' if no function is found
    OverloadedFunctionsSym* findFunctionGroup(std::string const& nameSpaceName,
                                              std::string const& simpleName);

    /// Return the list of loaded toolkits
    /// @return toolkits
    const std::vector<const Toolkit*>& toolkits() const { return _toolkits; }

    /// Map a source location to the toolkit directory it came from
    /// @param loc SourceLocation
    /// @param toolkitDirectory result toolkit directory
    /// @return 'true' if location found, and toolkitDirectory was set
    bool mapSourceLocationToToolkitDirectory(const SourceLocation& loc,
                                             std::string& toolkitDirectory);

    /// Return whether the source location came from the standard toolkit.
    /// @param loc SourceLocation
    /// @return 'true' if the location came from the standard toolkit.
    bool isInStandardToolkit(const SourceLocation& loc);

    /// Return the set of all namespaces defined by toolkits
    /// @return all namespaces defined by loaded toolkits
    const std::tr1::unordered_set<std::string>& toolkitNamespaces() const
    {
        return _toolkitNamespaces;
    }

  private:
    typedef std::tr1::unordered_map<int, std::string> LocToTookitType;
    typedef std::tr1::unordered_map<int, std::string> URITableType;
    std::string fileFromUriIndex(const URITableType&, int uriIndex);
    const std::string* addString(const std::string* str);

    bool addNonFunction(const std::string& dir,
                        const xmlns::prod::streams::spl::toolkit::toolkitNamespaceType* ns,
                        const xmlns::prod::streams::spl::toolkit::primitiveToolkitOperatorType* op,
                        const std::string& path,
                        const std::string& toolkit,
                        uint32_t toolkitIndex);
    bool addNonFunction(const std::string& dir,
                        const xmlns::prod::streams::spl::toolkit::toolkitNamespaceType* ns,
                        const xmlns::prod::streams::spl::toolkit::compositeToolkitOperatorType* op,
                        const std::string& path,
                        const std::string& toolkit,
                        uint32_t toolkitIndex);
    bool addNonFunction(const std::string& dir,
                        const xmlns::prod::streams::spl::toolkit::toolkitNamespaceType* ns,
                        const std::string& path,
                        const std::string& name,
                        uint32_t line,
                        uint32_t column,
                        bool isPrimitiveOperator,
                        const std::string& toolkit,
                        uint32_t toolkitIndex);
    bool addFunctionsFromModel(
      const std::string& dir,
      const xmlns::prod::streams::spl::toolkit::toolkitNamespaceType* ns,
      const xmlns::prod::streams::spl::toolkit::toolkitFunctionType& function,
      const std::string& toolkit,
      bool isBuiltinToolkitDir,
      const URITableType& uriTable,
      uint32_t toolkitIndex);
    ToolkitFactory _toolkitFactory;
    CompilerConfiguration& _config;
    std::vector<std::string> _dirNames;
    std::tr1::unordered_map<NameLocation, NonFunctionResult*> _nonFunctionMap;
    std::vector<std::vector<Function::LibraryPtr>*> _dependencies;
    std::tr1::unordered_set<std::string> _uniqueStrings;
    std::tr1::unordered_set<std::string> _toolkitNamespaces;
    typedef std::pair<const Operator::OperatorModel*, std::string> ModelMap;
    std::tr1::unordered_map<std::string, ModelMap> _operatorModelMap;
    std::tr1::unordered_set<std::string> _nativeFunctionModelsSeen;
    std::vector<const ToolkitInfo::ToolkitInfoModel*> _infos;
    LocToTookitType _locMap;
    int32_t _builtinFcnNumber;

    bool checkDependencies();
    bool processOneDirectory(std::ifstream& toolkitStr,
                             const std::string& toolkit,
                             const std::string& dir,
                             std::tr1::unordered_map<std::string, uint32_t>& toolkitNames,
                             bool& success);
    bool loadTheToolkits();
    std::vector<const Toolkit*> _toolkits;
    typedef std::tr1::unordered_map<std::string, std::vector<OneToolkit> > ToolkitMap;
    ToolkitMap _allToolkits;
    static bool toolkitSorter(const Toolkit* left, const Toolkit* right);
    std::vector<const xmlns::prod::streams::spl::toolkit::toolkitModelType*> _modelsToDelete;
    void processOneToolkit(const std::string& fileName,
                           std::tr1::unordered_map<std::string, uint32_t>& toolkitNames,
                           bool& success);
    void processToolkitList(const std::string& fileName,
                            std::tr1::unordered_map<std::string, uint32_t>& toolkitNames,
                            bool& success);
    void checkOneToolkit(std::tr1::unordered_set<std::string>& outOfDateDirectories,
                         const std::string& fileName);
    void checkToolkits(std::tr1::unordered_set<std::string>& outOfDateDirectories);
    void checkToolkitList(std::tr1::unordered_set<std::string>& outOfDateDirectories,
                          const std::string& fileName);
    void checkOneDirectory(std::tr1::unordered_set<std::string>& outOfDateDirectories,
                           std::ifstream& toolkitStr,
                           const std::string& toolkit,
                           const std::string& dir);
    bool checkNonFunction(const std::string& dir,
                          const std::string& path,
                          const std::string& opName,
                          time_t toolkitTime);
    bool checkFunctionsFromModel(
      const std::string& dir,
      const xmlns::prod::streams::spl::toolkit::toolkitFunctionType& function,
      const URITableType& uriTable,
      time_t toolkitTime);
    void toolkitsThatSatisfyDependency(const ToolkitMap& toolkitsNeedingResolution,
                                       const OneDep& dep,
                                       std::vector<const Toolkit*>& toolkitsThatSatisfyDependency);
    void findRootToolkits(const std::vector<const Toolkit*>& toolkitsToConsider,
                          std::vector<const Toolkit*>& rootToolkits) const;
    bool hasDependent(const std::vector<const Toolkit*>& toolkitsToConsider,
                      const Toolkit& toolkit) const;
    bool computePartialSolution(
      const std::vector<const Toolkit*>& toolkitsToConsider,
      const std::tr1::unordered_map<std::string, const Toolkit*>& toolkitsToBeLoaded,
      std::tr1::unordered_map<std::string, const Toolkit*>& toolkitsSeen,
      std::vector<const Toolkit*>& tkList,
      std::vector<Solution>& partialSolutions) const;
    void findToolkitsDependedOn(const std::vector<const Toolkit*>& toolkitsToConsider,
                                const Toolkit& toolkit,
                                std::vector<const Toolkit*>& deps) const;
    void addSingletonDependencies(
      ToolkitMap& toolkitsNeedingResolution,
      std::tr1::unordered_map<std::string, const Toolkit*>& toolkitsToBeLoaded);
    bool addPartialSolution(const Solution& ps,
                            std::tr1::unordered_map<std::string, const Toolkit*>& potentialSolution,
                            std::vector<FormattableMessage*>& details) const;
    bool satisfiesAllDependencies(
      const std::tr1::unordered_map<std::string, const Toolkit*>& potentialSolution,
      std::vector<FormattableMessage*>& details) const;
    bool satisfiesAllDependencies(
      const Toolkit& toolkit,
      std::tr1::unordered_map<std::string, const Toolkit*>& potentialSolution,
      std::vector<FormattableMessage*>& details) const;
};
};

#endif /* PATH_SEARCH_H */
