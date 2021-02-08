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

#ifndef SPL_MAKEFILE_GENERATOR_H
#define SPL_MAKEFILE_GENERATOR_H

#include <SPL/CodeGen/CodeGenHelper.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <UTILS/HashMapHelpers.h>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>
#include <errno.h>
#include <iostream>

namespace xmlns {
namespace prod {
namespace streams {
namespace spl {
namespace bundleInfoModel {
class bundleInfoModelType;
}
}
}
}
}

namespace SPL {

class CompilerConfiguration;
class EnumTyp;
class FunctionHeadSym;
class OperatorGraphNode;
class RootTyp;
class Toolkit;
class TupleTyp;
class TypeGenerator;
class UsedSPLFunctions;

namespace Enumeration_ {
class EnumerationModel;
};

namespace Tuple_ {
class TupleModel;
};

namespace XML_ {
class XMLModel;
};

namespace Common {
class Library;
};

class MakefileGenerator
{
    class Functions;

  public:
    MakefileGenerator(std::string adlFileName);
    void generate();
    void compile();
    const std::string& directoryPermissionMode() const { return _directoryPermissionMode; }
    const Functions& getFunctions() const { return _functions; }

  private:
    class Types
    {
      public:
        Types(const MakefileGenerator&);

        void collect(const std::string& lineSeparator);
        const std::string& objectFiles() { return _typeObjs; }
        void createRules(std::ofstream& mf);

      private:
        void genTypeRule(std::ofstream& mf, const std::string& name, const std::string& niceName);

        std::vector<const Tuple_::TupleModel*> _tuples;
        std::vector<const Enumeration_::EnumerationModel*> _enums;
        std::vector<const XML_::XMLModel*> _xmls;
        std::string _typeObjs;
        const TypeGenerator& _typeGenerator;
        const MakefileGenerator& _gen;
    };

    class Functions
    {
      public:
        typedef std::tr1::unordered_set<const RootTyp*> TypeSet;
        typedef std::tr1::unordered_set<const FunctionHeadSym*> FunctionSet;
        typedef std::tr1::unordered_map<std::string, TypeSet> FunctionTypes;
        typedef std::tr1::unordered_map<std::string, FunctionSet> FunctionsCalled;

        Functions(const MakefileGenerator&);

        void collect(const std::string& lineSeparator);
        const std::string& objectFiles() { return _fcnObjs; }
        void createRules(std::ofstream& mf, std::string adlFileName);
        const TypeSet& getTypes(const std::string& name) const
        {
            return _fcnTypes.find(name)->second;
        }
        const FunctionSet& getCalledFunctions(const std::string& name) const
        {
            return _fcnsCalled.find(name)->second;
        }

      private:
        std::string _fcnObjs;
        const UsedSPLFunctions& _splFunctions;
        const MakefileGenerator& _gen;
        FunctionTypes _fcnTypes;
        FunctionsCalled _fcnsCalled;
    };

    class Operators
    {
      public:
        Operators(const MakefileGenerator& gen)
          : _gen(gen)
        {}

        void collect(const std::string& lineSeparator);
        const std::string& objectFiles() { return _operatorObjs; }
        void createRules(std::ofstream& mf, std::string adlFileName);

      private:
        void collectOperatorUses(const OperatorGraphNode& node,
                                 std::tr1::unordered_set<const RootTyp*>& types);

        std::string _operatorObjs;
        const MakefileGenerator& _gen;
    };

    class PEs
    {
      public:
        PEs(const MakefileGenerator& gen, const CompilerConfiguration& config)
          : _gen(gen)
          , _config(config)
        {}

        void collect(const std::string& lineSeparator);
        const std::string& executableFiles() { return _peBins; }
        void createRules(std::ofstream& mf);

      private:
        const MakefileGenerator& _gen;
        const CompilerConfiguration& _config;
        std::string _peBins;

        void addCalledFunctions(const std::string& functionName,
                                std::tr1::unordered_set<const FunctionHeadSym*>& duplicateFunctions,
                                std::tr1::unordered_set<std::string>& duplicates,
                                std::string& objectDeps,
                                std::string& linkCmd);
        void grabLinkInformation(std::vector<boost::shared_ptr<Common::Library> > const& libs,
                                 uint32_t toolkitIndex,
                                 std::string& linkCmd,
                                 std::tr1::unordered_set<std::string>& dups);
        void grabLinkInformation(Common::Library const& lib,
                                 uint32_t toolkitIndex,
                                 std::string& linkCmd,
                                 std::tr1::unordered_set<std::string>& dups);
    };

    void collect();
    void createHeader(std::ofstream& mf);
    void createBundleRules(std::ofstream& mf);
    void createStandaloneRules(std::ofstream& mf);
    std::string shortenName(std::string const& name) const;
    void copyToolkit(const boost::filesystem::path& toolkitRoot,
                     const Toolkit& toolkit,
                     xmlns::prod::streams::spl::bundleInfoModel::bundleInfoModelType& biModel,
                     std::string& bundleDepInfo,
                     std::vector<std::string>& applicationRegExs,
                     std::vector<std::string>& outputDirRegExs) const;
    void recursivelyAddDirectory(
      const boost::filesystem::path& srcDir,
      const boost::filesystem::path& tgtDir,
      const std::vector<std::string>& includeRegExs,
      const std::vector<std::string>& excludeRegExs,
      std::vector<std::pair<boost::filesystem::path, boost::filesystem::path> >& files) const;

    // Helpers
    const CompilerConfiguration& _config;

    Types _types;
    Functions _functions;
    Operators _operators;
    PEs _pes;
    std::string _directoryPermissionMode;
    std::string _adlFileName;
    std::string _bundleFile;
};
};

#endif /* SPL_MAKEFILE_GENERATOR_H */
