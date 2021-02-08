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

#ifndef CODEGEN_FUNCTIONINFORMATION_H
#define CODEGEN_FUNCTIONINFORMATION_H

#include <CodeGen/Statement.h>
#include <UTILS/HashMapHelpers.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

namespace SPL {
class FunctionHeadSym;

struct ProvisionedLibrary
{
    ProvisionedLibrary(const std::string& description,
                       const std::string& name,
                       const std::string& version)
      : _description(description)
      , _name(name)
      , _version(version)
    {}

    ProvisionedLibrary(const ProvisionedLibrary& rhs)
      : _description(rhs._description)
      , _name(rhs._name)
      , _version(rhs._version)
    {}

    bool operator==(const ProvisionedLibrary& rhs) const
    {
        return _description == rhs._description && _name == rhs._name && _version == rhs._version;
    }

    std::string _description;
    std::string _name;
    std::string _version;
};
};

namespace std {
namespace tr1 {
template<>
struct hash<SPL::ProvisionedLibrary>
{
    size_t operator()(const SPL::ProvisionedLibrary& pl) const
    {
        hash<std::string> myhash;
        return myhash(pl._description + pl._name + pl._version);
    }
};
}
}

namespace SPL {
class RootTyp;

/// Information needed to compile & link a set of overloaded SPL functions
class FunctionInformation
{
  public:
    /// Add a function to be generated.  Take over responsibility for
    /// freeing the memory of the statement
    /// @param fcnHead  Function header
    /// @param block    Function Body
    void addFunction(const FunctionHeadSym& fcnHead, BlockStatement& block);

    /// One function header/body pair
    struct OneFcnInfo
    {
        OneFcnInfo()
          : _header(NULL)
        {}
        OneFcnInfo(const FunctionHeadSym& fcnHead, BlockStatement& block)
          : _header(&fcnHead)
          , _body(&block)
        {}

        const FunctionHeadSym& header() const { return *_header; }
        BlockStatement& body() const { return *_body; }

      private:
        const FunctionHeadSym* _header;
        boost::shared_ptr<BlockStatement> _body;
    };

    /// Return the overloaded functions
    /// @return vector of (possibly overloaded) functions
    const std::vector<OneFcnInfo>& getFunctions() const { return _fcns; }

    /// Add a ProvisionedLibrary to the overloaded functions
    /// @param lib library to add
    void addProvisionedLibrary(const ProvisionedLibrary& lib);

    /// Return the provisioned libraries
    /// @return set of provisioned libs
    const std::tr1::unordered_set<ProvisionedLibrary>& getProvisionedLibs() const
    {
        return _fcnProvs;
    }

    /// Compute include names from types
    /// @param types set of types referenced by fcns
    void typeIncludes(const std::tr1::unordered_set<const RootTyp*>& types);

    /// Return the includes generated from types
    /// @return collected set of type includes
    const std::tr1::unordered_set<std::string>& typeIncludes() const { return _typeIncludes; }

    /// Compute include names from functions
    /// @param fcns set of functions being called
    void functionIncludes(const std::tr1::unordered_set<const FunctionHeadSym*>& fcns);

    /// Return the includes generated from functions
    /// @return collected set of function includes
    const std::tr1::unordered_set<std::string>& functionIncludes() const { return _fcnIncludes; }

    /// Return the native file includes generated from functions
    /// @return collected set of native file function includes
    const std::tr1::unordered_set<std::string>& nativeFunctionIncludes() const
    {
        return _fcnNativeIncludes;
    }

    /// Set the 'root' filename for the set of overloaded functions
    /// @param name filename
    void setRootFileName(const std::string& name) { _rootFileName = name; }

    /// Return the 'root' filename for the set of overloaded functions
    /// @return root filename
    const std::string& getRootFileName() const { return _rootFileName; }

    /// Return the include paths generated from functions
    /// @return collected set of function include paths
    const std::tr1::unordered_set<std::string>& functionIncludePaths() const
    {
        return _fcnIncludePaths;
    }

    /// Return the library paths generated from functions
    /// @return collected set of function library paths
    const std::tr1::unordered_set<std::string>& functionLibraryPaths() const
    {
        return _fcnLibPaths;
    }

  private:
    //!< The set of overloaded functions
    std::vector<OneFcnInfo> _fcns;

    //!< base filename for the set
    std::string _rootFileName;

    //!< includes needed for enums/tuples
    std::tr1::unordered_set<std::string> _typeIncludes;

    //!< includes needed for referenced fcns
    std::tr1::unordered_set<std::string> _fcnIncludes;

    //!< includes from native functions needed for referenced fcns
    std::tr1::unordered_set<std::string> _fcnNativeIncludes;

    //!< -I includes for compiling
    std::tr1::unordered_set<std::string> _fcnIncludePaths;

    //!< -L libpaths for linking
    std::tr1::unordered_set<std::string> _fcnLibPaths;

    //!< -l libs for linking
    std::tr1::unordered_set<std::string> _fcnLibs;

    //!< Provtool description/name/version
    std::tr1::unordered_set<ProvisionedLibrary> _fcnProvs;
};
};

#endif
