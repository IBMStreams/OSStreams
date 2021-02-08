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

#ifndef SPL_MAKE_TOOLKIT_APP_H
#define SPL_MAKE_TOOLKIT_APP_H

#include <dlfcn.h>
#include <dst-config.h>
#include <jni.h>

#include <SPL/CodeGen/Config.h>
#include <SPL/Core/ToolkitInfoModelImpl.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Utility/ModelDumper.h>
#include <SPL/Utility/NameLocation.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/HashMapHelpers.h>
#include <UTILS/RegEx.h>

#include <boost/filesystem/path.hpp>

namespace xmlns {
namespace prod {
namespace streams {
namespace spl {
namespace operator_ {
class opModelType;
};
namespace toolkitInfo {
class toolkitInfoModelType;
};
namespace sourceCode {
class sourceFileType;
class compilationUnitType;
}
namespace toolkit {
class toolkitModelType;
class toolkitNamespaceType;
class toolkitType;
class toolkitTypeType;
class primitiveToolkitOperatorType;
class compositeToolkitOperatorType;
class enumLiteralsType;
}
}
}
}
};

namespace SPL {
namespace ToolkitInfo {
class MessageSet;
}
class OperatorGraph;
class CompilerConfiguration;
class PathSearch;
class TransportStats;
class FrontEndDriver;
class UsedOperatorInvocations;
class SourceLocation;

class DLL_PUBLIC MakeToolkit
  : public Distillery::DistilleryApplication
  , public Singleton<MakeToolkit>
{
  public:
    /// Constructor
    MakeToolkit();
    ~MakeToolkit();

    // Interface to signature generation in Front End

    /// Set the namespace for the current SPL file being parsed
    /// @param ns Namespace name
    /// @param loc Namespace location
    virtual void setNamespace(const std::string& ns, const SourceLocation& loc);

    /// Add an SPL function in the current namespace
    /// @param f Function name
    /// @param hdr Function header as an AST
    virtual void addFunction(const std::string& f, AstNode& hdr);

    /// Add an SPL composite in the current namespace
    /// @param c Composite name
    /// @param hdr Composite header as an AST
    virtual void addComposite(const std::string& c, AstNode& hdr);

    /// Finish an SPL composite in the current namespace
    /// @param c Composite name
    /// @param hdr Composite header as an AST
    virtual void finishComposite(const std::string& c, AstNode& hdr);

    /// Add an SPL typedef in the current namespace
    /// @param t Typedef name
    /// @param hdr Typedef header as an AST
    virtual void addTypedef(const std::string& t, AstNode& hdr);

    /// Finish an SPL typedef in the current namespace
    /// @param t Typedef name
    /// @param hdr Typedef header as an AST
    virtual void finishTypedef(const std::string& t, AstNode& hdr);

    /// Add an SPL enum in the current namespace
    /// @param n AST Node for enum
    virtual void addEnum(AstNode& n);

    /// Return the verbose model error setting
    /// @return 'true' if --verbose-model-errors parameter was seen
    bool verboseModelErrors() const { return _verboseModelErrors; }

    /// Run the correct model verifier, and gen the output as an error
    /// @param verifier string to indicate which verifier to run
    /// @param file File to check
    void runModelVerifier(const std::string& verifier, const std::string& file) const;

    /// Add a source code model
    /// @param m the source code model to add
    void addSourceCodeModel(xmlns::prod::streams::spl::sourceCode::compilationUnitType& m,
                            const std::string& fileName);

  protected:
    virtual std::ostream& printDescription(std::ostream& o) const;
    virtual int run(const std::vector<std::string>& remains);
    virtual void getDistilleryApplicationVisibleOptions(option_vector_t& options);
    virtual void getArguments(option_vector_t& options);
    virtual int init();
    void mySigIntHandler(int /*signum*/);
    void iceHandler(int /*signum*/);

    bool getInterruptable() { return _interruptable; }
    void setInterruptable(bool enable) { _interruptable = enable; }

    // Option Processing routines
    void setHelp(const option_t* option, const char* value);
    void setClean(const option_t* option, const char* value);
    void setDescription(const option_t* option, const char* value);
    void setDirectory(const option_t* option, const char* value);
    void setSilent(const option_t* option, const char* value);
    void setDebugString(const option_t* option, const char* value);
    void setName(const option_t* option, const char* value);
    void setVersion(const option_t* option, const char* value);
    void setMakeOperator(const option_t* option, const char* value);
    void setCompilerVersion(const option_t* option, const char* value);
    void setNoMixedModePreprocessing(const option_t* option, const char* value);
    void setVerboseModelErrors(const option_t* option, const char* value);
    void setForceGenerate(const option_t* option, const char* value);
    void setGenSourceModel(const option_t* option, const char* value);

  protected:
    void installFaultSignalHandlers(void);

  private:
    // Primitive operators
    class NameSpaces;
    class PrimitiveOperators
    {
        typedef std::vector<std::pair<std::string, std::string> > OperatorInformation;

      public:
        void add(const std::string& opName, const std::string& modelFile)
        {
            _ops.push_back(std::make_pair(opName, modelFile));
        }

        void populate(const boost::filesystem::path& rootDir,
                      xmlns::prod::streams::spl::toolkit::toolkitNamespaceType& ns,
                      NameSpaces& n,
                      const MakeToolkit& mt) const;

        std::string find(const std::string& opName) const
        {
            // This is a linear search, but there are likely not enough operators ina namespace to make this an issue.
            for (OperatorInformation::size_type i = 0, sz = _ops.size(); i < sz; ++i) {
                if (_ops[i].first == opName) {
                    return _ops[i].second;
                }
            }
            return "";
        }

      private:
        OperatorInformation _ops;
        void populate(const boost::filesystem::path& tkdir,
                      const boost::filesystem::path& path,
                      xmlns::prod::streams::spl::toolkit::primitiveToolkitOperatorType& prim,
                      const xmlns::prod::streams::spl::operator_::opModelType& opModel,
                      bool isJava,
                      NameSpaces& n) const;
    };

    // Native Functions
    class NativeFunctions
    {
      public:
        void add(const std::string& functionModelFile)
        {
            _nativeFunctions.push_back(functionModelFile);
        }
        void populate(const boost::filesystem::path& rootDir,
                      xmlns::prod::streams::spl::toolkit::toolkitNamespaceType& ns,
                      NameSpaces& n,
                      const MakeToolkit& mt) const;

      private:
        std::vector<std::string> _nativeFunctions;
    };

    // SPL Types
    class SPLTypes
    {
      public:
        void add(const NameLocation& type, int splFileIndex, AstNode& ast)
        {
            _types.push_back(TypeInformation(type, splFileIndex, ast));
        }
        void populate(xmlns::prod::streams::spl::toolkit::toolkitNamespaceType& ns) const;
        void populate(xmlns::prod::streams::spl::toolkit::compositeToolkitOperatorType& c) const;

      private:
        struct TypeInformation
        {
            TypeInformation(const NameLocation& type, int splFileIndex, AstNode& a)
              : _type(type)
              , _splFileIndex(splFileIndex)
              , _ast(&a)
            {}
            std::auto_ptr<xmlns::prod::streams::spl::toolkit::toolkitTypeType> populate() const;
            AstNode& ast() const { return *_ast; }

          private:
            NameLocation _type;
            int _splFileIndex;
            AstNode* _ast;
        };
        std::vector<TypeInformation> _types;
    };

    // SPL Functions
    class SPLFunctions
    {
      public:
        void add(const NameLocation& fcn, int splFileIndex, AstNode& ast)
        {
            _functions.push_back(FunctionInformation(fcn, splFileIndex, ast));
        }
        void populate(const boost::filesystem::path& rootDir,
                      xmlns::prod::streams::spl::toolkit::toolkitNamespaceType& ns) const;

      private:
        struct FunctionInformation
        {
            FunctionInformation(const NameLocation& fcn, int splFileIndex, AstNode& ast)
              : _fcn(fcn)
              , _splFileIndex(splFileIndex)
              , _ast(&ast)
            {}
            void populate(const boost::filesystem::path& rootDir,
                          xmlns::prod::streams::spl::toolkit::toolkitNamespaceType& ns) const;

          private:
            NameLocation _fcn;
            int _splFileIndex;
            AstNode* _ast;
        };
        std::vector<FunctionInformation> _functions;
    };

    // SPL Enum Literals
    class SPLEnumLiterals
    {
      public:
        void add(const NameLocation& enumLit, int splFileIndex, AstNode& ast, AstNode* typeDef)
        {
            _enumLits.push_back(EnumInformation(enumLit, splFileIndex, ast, typeDef));
        }
        void populate(xmlns::prod::streams::spl::toolkit::toolkitNamespaceType& ns) const;
        void populate(xmlns::prod::streams::spl::toolkit::compositeToolkitOperatorType& c) const;

      private:
        struct EnumInformation
        {
            EnumInformation(const NameLocation& enumLit,
                            int splFileIndex,
                            AstNode& ast,
                            AstNode* typeDef)
              : _enumLit(enumLit)
              , _splFileIndex(splFileIndex)
              , _ast(&ast)
              , _typeDef(typeDef)
            {}
            std::auto_ptr<xmlns::prod::streams::spl::toolkit::enumLiteralsType> populate() const;

          private:
            NameLocation _enumLit;
            int _splFileIndex;
            AstNode* _ast;
            AstNode* _typeDef;
        };
        std::vector<EnumInformation> _enumLits;
    };

    // Composite operators
    class CompositeOperators
    {
      public:
        void add(const NameLocation& op, int splFileIndex, AstNode& hdr)
        {
            _ops.push_back(OperatorInformation(op, splFileIndex, hdr));
        }

        void addType(const NameLocation& op, int splFileIndex, AstNode& ast)
        {
            if (!_ops.empty()) {
                _ops.back().addType(op, splFileIndex, ast);
            }
        }

        void addEnum(const NameLocation& op, int splFileIndex, AstNode& ast, AstNode* typeDef)
        {
            if (!_ops.empty()) {
                _ops.back().addEnum(op, splFileIndex, ast, typeDef);
            }
        }

        void populate(const boost::filesystem::path& rootDir,
                      xmlns::prod::streams::spl::toolkit::toolkitNamespaceType& ns) const;

      private:
        struct OperatorInformation
        {
            OperatorInformation(const NameLocation& op, int splFileIndex, AstNode& ast)
              : _op(op)
              , _splFileIndex(splFileIndex)
              , _ast(&ast)
            {}
            void populate(xmlns::prod::streams::spl::toolkit::toolkitNamespaceType& ns) const;
            void addType(const NameLocation& op, int splFileIndex, AstNode& ast)
            {
                _types.add(op, splFileIndex, ast);
            }
            void addEnum(const NameLocation& op, int splFileIndex, AstNode& ast, AstNode* typeDef)
            {
                _enums.add(op, splFileIndex, ast, typeDef);
            }

          private:
            NameLocation _op;
            int _splFileIndex;
            AstNode* _ast;
            SPLTypes _types;
            SPLEnumLiterals _enums;
        };
        std::vector<OperatorInformation> _ops;
    };

    // Each namespace has composites, types, functions, primitives, enums
    class NameSpaces
    {
      public:
        NameSpaces();
        void setFile(const boost::filesystem::path& file, const std::string& fileName);
        void setNamespace(const std::string& ns, const std::string& raw);
        void setNamespace(const std::string& ns, const SourceLocation& loc);
        void addNamespace(const std::string& ns);
        void addNativeFile(const std::string& ns, const std::string& nativeFunctionModelFile);
        void addPrimitiveOperator(const std::string& ns,
                                  const std::string& opName,
                                  const std::string& fileName);
        void addFunction(AstNode& hdr);
        void addComposite(const std::string& compName, AstNode& hdr);
        void finishComposite(const std::string& compName, AstNode& hdr);
        void addType(const std::string& typeName, AstNode& hdr);
        void finishType(const std::string& typeName, AstNode& hdr);
        void addEnum(AstNode& n);
        void populate(const boost::filesystem::path& tkdir,
                      xmlns::prod::streams::spl::toolkit::toolkitType& tk,
                      const MakeToolkit& mt);
        bool nothingToDo() const;
        int uriIndex(const std::string& uri);
        int fileUriIndex(const std::string& fileName);
        typedef std::tr1::unordered_map<NameLocation, std::string> NameLoc;
        bool checkName(const std::string& namespaceName,
                       const std::string& fileName,
                       NameLoc& map,
                       const NameLocation& nl);
        bool checkNameInNamespace(const std::string& namespaceName,
                                  const std::string& fileName,
                                  const NameLocation& nl);

      private:
        struct NSInfo
        {
            PrimitiveOperators _primitives;
            NativeFunctions _nativeFunctions;
            CompositeOperators _composites;
            SPLTypes _splTypes;
            SPLFunctions _splFunctions;
            SPLEnumLiterals _splEnumLits;
            NameLoc _allNames;
        };

        typedef std::tr1::unordered_map<std::string, NSInfo> NSMap;
        NSMap _map;
        bool _inComposite;
        NameLoc _currentCompositeNames;
        AstNode* _currentTypedef;

        // namespace we are in (derived from path) with '/' replaced by '.'
        std::string _currentNamespace;

        // namespace we are in (derived from path) as seen in the directory tree
        std::string _rawNamespace;

        // File we are processing (and as an index)
        std::string _currentSPLFile;
        int _currentSPLFileIndex;

        // Information in path format
        boost::filesystem::path _file;

        // Map for URIs
        typedef std::tr1::unordered_map<std::string, int> URITableType;
        URITableType _uriTable;
    };

    typedef void (MakeToolkit::*WalkerFcn)(const boost::filesystem::path& rootDir,
                                           const boost::filesystem::path& file,
                                           const std::string& insideReservedDirectory,
                                           const bool insidePrimitiveOperatorDirectory);

    void walkDirectoryTree(const boost::filesystem::path& rootDir,
                           const boost::filesystem::path& dir,
                           WalkerFcn fcn,
                           const std::string& insideReservedDirectory,
                           const bool insidePrimitiveOperatorDirectory);

    // Routines used with walkDirectoryTree to look for specific files
    void deletePMFiles(const boost::filesystem::path& rootDir,
                       const boost::filesystem::path& file,
                       const std::string& insideReservedDirectory,
                       const bool insidePrimitiveOperatorDirectory);
    void findPrimitiveOperators(const boost::filesystem::path& rootDir,
                                const boost::filesystem::path& file,
                                const std::string& insideReservedDirectory,
                                const bool insidePrimitiveOperatorDirectory);
    void findNativeFunctionFiles(const boost::filesystem::path& rootDir,
                                 const boost::filesystem::path& file,
                                 const std::string& insideReservedDirectory,
                                 const bool insidePrimitiveOperatorDirectory);
    void findDotNamespaceFiles(const boost::filesystem::path& rootDir,
                               const boost::filesystem::path& file,
                               const std::string& insideReservedDirectory,
                               const bool insidePrimitiveOperatorDirectory);
    void scanSPLFiles(const boost::filesystem::path& rootDir,
                      const boost::filesystem::path& file,
                      const std::string& insideReservedDirectory,
                      const bool insidePrimitiveOperatorDirectory);
    void recordMixedModeFile(const boost::filesystem::path& rootDir,
                             const boost::filesystem::path& file,
                             const std::string& insideReservedDirectory,
                             const bool insidePrimitiveOperatorDirectory);

    // And the routines that actually do something
    void cleanupToolkit(const boost::filesystem::path& tkdir);
    void runJavaIndexer(const boost::filesystem::path& tkdir);
    JavaVM* createJavaVM(void*& env, JavaVMInitArgs& vm_args);
    void addPrimitiveOperators(const boost::filesystem::path& tkdir);
    void addNativeFunctionFiles(const boost::filesystem::path& tkdir);
    void addDotNamespaceFiles(const boost::filesystem::path& tkdir);
    void scanSPLFiles(const boost::filesystem::path& tkdir);
    //        void setSourceTreeBuild(const option_t * option, const char * value);
    std::string checkStreamsInstallation();
    void computeSPLPaths();
    void createSourceModelFile(const boost::filesystem::path& tkdir);
    void createToolkitXMLFiles(
      const boost::filesystem::path& tkdir,
      const std::vector<std::pair<std::string, ToolkitInfo::ToolkitInfoModel::RootType> >& includes,
      const std::vector<std::string>& excludes);
    void populateToolkit(
      const boost::filesystem::path& tkdir,
      xmlns::prod::streams::spl::toolkit::toolkitModelType& tk,
      const std::vector<std::pair<std::string, ToolkitInfo::ToolkitInfoModel::RootType> >& includes,
      const std::vector<std::string>& excludes);

    bool computeCompilerPaths();
    void locateMixedModeFiles(const boost::filesystem::path& tkdir);
    void preprocessMixedModeFiles(const boost::filesystem::path& tkdir);
    void cleanupNLArtifacts(const boost::filesystem::path& tkdir);
    void cleanupPreprocessedMixedModeFiles();
    bool isNonSplReservedDirectory(const std::string& dirName);
    bool isPrimitiveDirectory(const boost::filesystem::path& rootDir,
                              const boost::filesystem::path& dirName);

    void buildMessageBundle(const ToolkitInfo::MessageSet& messageSet,
                            const boost::filesystem::path& tkdir);

    // All the collected information about each namespace
    NameSpaces _namespaces;

    typedef std::tr1::unordered_map<std::string, std::string> NameSpaceMapType;
    NameSpaceMapType _namespacePathMap;

    bool checkAndFixupNamespacePath(const boost::filesystem::path& path,
                                    std::string& namespacePath,
                                    const std::string& item);

    // Interfaces to other parts of the compiler
    std::auto_ptr<CompilerConfiguration> _config;
    std::auto_ptr<ModelDumper> _modelDumper;
    std::auto_ptr<FrontEndDriver> _frontEnd;
    std::auto_ptr<PathSearch> _pathSearch;

    // Ctrl-C handling (not needed?)
    volatile bool _interruptable;

    bool _forceGeneration, _verboseModelErrors, _genSourceModel;

    // Command line to invoke spl-make-operator
    std::string _splMakeOperator;

    // Directory in which to create the source model
    std::string _sourceModelDir;

    // Preprocessor arguments passed to spl-make-toolkit
    std::vector<std::string> _preprocessorArgs;

    // Info model for dependencies
    std::auto_ptr<xmlns::prod::streams::spl::toolkitInfo::toolkitInfoModelType> _info;

    // Number of components in rootDir
    uint32_t _rootDirCount;

    Distillery::RegEx _validNamespaceComponent;

    typedef std::vector<xmlns::prod::streams::spl::sourceCode::sourceFileType*> SourceCodeModels;
    SourceCodeModels _sourceCodeModels;
};
};

#endif /* SPL_MAKE_TOOLKIT_APP_H */
