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

#include <SPL/Core/MakeToolkit.h>

#include <SPL/Core/BaseMangler.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/FunctionModelImpl.h>
#include <SPL/Core/MixedModeLineMapper.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/ToolkitInfoModelImpl.h>
#include <SPL/Core/commonModel.h>
#include <SPL/Core/operatorModel.h>
#include <SPL/Core/sourceCodeModel.h>
#include <SPL/Core/toolkitInfoModel.h>
#include <SPL/Core/toolkitModel.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/Runtime/Utility/MessageFormatter.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <SPL/Utility/XMLDiagnostic.h>
#include <UTILS/SupportFunctions.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/scoped_array.hpp>

#include <dst-config.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
using namespace SPL;
using namespace Distillery;
using namespace xmlns::prod::streams::spl::common;
using namespace xmlns::prod::streams::spl::toolkit;
using namespace xmlns::prod::streams::spl::toolkitInfo;
using namespace xmlns::prod::streams::spl::function;
using namespace xmlns::prod::streams::spl::sourceCode;
namespace bf = boost::filesystem;

#define XML_EXT ".XML"
#define SPL_EXT ".spl"

#ifdef BOOST_WINDOWS_API
#define SPLPATH_SEP ";"
#else
#define SPLPATH_SEP ":"
#endif

static bool sawInfo = false;

#include <SPL/Runtime/Utility/Singleton.t>
template class Singleton<MakeToolkit>;
template class Singleton<ModelDumper>;

MakeToolkit::MakeToolkit()
  : Singleton<MakeToolkit>(*this)
  , _interruptable(true)
  , _forceGeneration(false)
  , _verboseModelErrors(false)
  , _genSourceModel(false)
  , _validNamespaceComponent("^[A-Za-z_][A-Za-z0-9_]*$")
{
    _config.reset(new CompilerConfiguration());
    // Construct these after the options have been processed
    if (_config->isSet(CompilerConfiguration::ModelFile)) {
        _modelDumper.reset(
          new ModelDumper(_config->getStringValue(CompilerConfiguration::ModelFile)));
    }
    _frontEnd.reset(new FrontEndDriver());
    _pathSearch.reset(new PathSearch());
}

MakeToolkit::~MakeToolkit()
{
    for (SourceCodeModels::iterator it = _sourceCodeModels.begin(); it != _sourceCodeModels.end();
         ++it) {
        delete *it;
    }
}

void MakeToolkit::mySigIntHandler(int /*signum*/)
{
    if (!_interruptable)
        return;
    ostringstream ostr;
    ostr << "\n" << SPL_CORE_INTERRUPTED << "\n";
    async_safe_printf(1, ostr.str().c_str());
    exit(EXIT_FAILURE);
}

static bf::path toolkitFile;
void MakeToolkit::iceHandler(int sig)
{
    // We want better behaviour.   Print an ICE message
    SysOut::errorln(SPL_CORE_MAKE_TOOLKIT_INTERNAL_COMPILER_ERROR(sig));
    if (!toolkitFile.empty()) {
        // Better to remove the file than leave an invalid one - messes up ICE
        try {
            bf::remove(toolkitFile);
        } catch (...) {
        }
    }
    exit(EXIT_FAILURE);
}

void MakeToolkit::installFaultSignalHandlers()
{
    if (indebug(Debug::Traceback)) {
        // Use default behaviour
        DistilleryApplication::installFaultSignalHandlers();
        return;
    }
    DistilleryApplication::getThisApp().removeSignalHandler(SIGINT);
    DistilleryApplication::getThisApp().installSignalHandler(
      SIGINT, mem_cb(this, &MakeToolkit::mySigIntHandler));
    installSignalHandler(SIGABRT, mem_cb(this, &MakeToolkit::iceHandler));
    installSignalHandler(SIGSEGV, mem_cb(this, &MakeToolkit::iceHandler));
    installSignalHandler(SIGPIPE, mem_cb(this, &MakeToolkit::iceHandler));
    installSignalHandler(SIGILL, mem_cb(this, &MakeToolkit::iceHandler));
    installSignalHandler(SIGFPE, mem_cb(this, &MakeToolkit::iceHandler));
    installSignalHandler(SIGBUS, mem_cb(this, &MakeToolkit::iceHandler));
}

int MakeToolkit::init()
{
    // run the base class init first....
    return DistilleryApplication::init();
}

bool MakeToolkit::computeCompilerPaths()
{
    bf::path installDir = checkStreamsInstallation();
    _config->setStringValue(CompilerConfiguration::InstallDir, installDir.string());

    bf::path internalScriptDir = installDir / "system" / "impl" / "bin";
    _config->setStringValue(CompilerConfiguration::InternalScriptDir, internalScriptDir.string());

    //Always use the installed java or the java in the source tree install.
    char* javaHome = getenv("JAVA_HOME");
    if (NULL == javaHome) {
        SysOut::errorln(SPL_CORE_PATH_DOES_NOT_EXIST("JAVA_HOME"));
        return false;
    }
    _config->setStringValue(CompilerConfiguration::JavaHome, javaHome);
    _config->setBooleanValue(CompilerConfiguration::JavaHomeIsInstall, false);
    return true;
}

/*
 * Main application processing routine
 */
int MakeToolkit::run(const vector<string>& remains)
{
    SPLDBG("Started spl-make-toolkit...", Debug::Core);

    _preprocessorArgs = remains;

    // Check toolkit directory
    if (!_config->isSet(CompilerConfiguration::MakeToolkitDir)) {
        SysOut::errorln(SPL_CORE_TOOLKIT_MISSING_DIRECTORY);
        return EXIT_FAILURE;
    }
    bf::path tkdir = _config->getStringValue(CompilerConfiguration::MakeToolkitDir);
    if (!bf::exists(tkdir) || !bf::is_directory(tkdir)) {
        SysOut::errorln(SPL_CORE_PATH_IS_NOT_A_DIRECTORY(tkdir.string()));
        return EXIT_FAILURE;
    }

    // Clean up the name (remove trailing /, etc).   Count number of directories in the path
    tkdir = Utility::cleanPath(tkdir);
    bf::path::iterator it = tkdir.begin();
    _rootDirCount = 0;
    if (tkdir != ".") {
        while (it != tkdir.end()) {
            _rootDirCount++;
            it++;
        }
    }

    if (!computeCompilerPaths()) {
        return EXIT_FAILURE;
    }
    locateMixedModeFiles(tkdir);

    // The line mapper needs to be created after splmm files have been created
    // but before the preprocessed spl file is processed.
    MixedModeLineMapper lineMapper;

    // Notice if we generated errors
    AutoErrorTrap aet(false);

    // What are we trying to do?
    if (_config->isSet(CompilerConfiguration::Clean)) {
        cleanupToolkit(tkdir);
    } else {
        if (!_genSourceModel) {
            preprocessMixedModeFiles(tkdir);
        }

        vector<pair<string, ToolkitInfo::ToolkitInfoModel::RootType> > includes;
        vector<string> excludes;

        // The real work
        _config->setBooleanValue(CompilerConfiguration::IgnoreTimestamps, true);
        _config->setBooleanValue(CompilerConfiguration::TolerateMissingToolkits, true);
        _config->setBooleanValue(CompilerConfiguration::PrintSignature, true);
        computeSPLPaths();
        {
            // We don't want to die if we are forcing errors
            AutoErrorTrap aet2(!_forceGeneration);

            // Grab information from info.xml (if present)
            using namespace ToolkitInfo;
            bf::path toolkitInfo = tkdir / "info.xml";
            ifstream toolkitInfoStr(toolkitInfo.string().c_str(), fstream::in);
            if (toolkitInfoStr) {
                auto_ptr<ToolkitInfoModel> infoModel;
                XMLDiag::XMLDiagPrinter errorPrinter(toolkitInfo.string());
                string xsddir = _config->getStringValue(CompilerConfiguration::XsdDir);
                string toolkitInfoSchemaName((bf::path(xsddir) / "toolkitInfoModel.xsd").string());
                try {
                    ::xml_schema::properties props;
                    props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/toolkitInfo",
                                          toolkitInfoSchemaName);
                    _info = toolkitInfoModel(toolkitInfoStr, errorPrinter, 0, props);
                    infoModel.reset(new ToolkitInfo::ToolkitInfoModel(*_info));
                    sawInfo = true;
                } catch (::xsd::cxx::exception& e) {
                    SPLDBG("Unable to load Toolkit Info Model XML " << toolkitInfo.string() << ": "
                                                                    << e.what(),
                           Debug::Core);
                    if (verboseModelErrors()) {
                        SysOut::errorln(SPL_CORE_ERROR_LOADING_INFO_MODEL_BRIEF(e.what()),
                                        SourceLocation(toolkitInfo.string(), 0, 0));
                        runModelVerifier("toolkit-info", toolkitInfo.string());
                    } else {
                        SysOut::errorln(SPL_CORE_ERROR_LOADING_INFO_MODEL(e.what()),
                                        SourceLocation(toolkitInfo.string(), 0, 0));
                    }
                } catch (SPLCompilerException& e) {
                    SysOut::errorln(SPL_CORE_ERROR_LOADING_INFO_MODEL_BRIEF(e.getExplanation()),
                                    SourceLocation(toolkitInfo.string(), 0, 0));
                }

                // And use the information from the info file if not set from command line
                if (infoModel.get()) {
                    if (!_config->isSet(CompilerConfiguration::ToolkitName)) {
                        string tkName = infoModel->getName();
                        if (tkName.empty()) {
                            SysOut::errorln(SPL_CORE_EMPTY_TOOLKIT_NAME,
                                            SourceLocation(toolkitInfo.string(), 0, 0));
                            tkName = "error"; // prevent error check later
                        }
                        _config->setStringValue(CompilerConfiguration::ToolkitName, tkName);
                    }
                    if (!_config->isSet(CompilerConfiguration::ToolkitProductVersion))
                        _config->setStringValue(CompilerConfiguration::ToolkitProductVersion,
                                                infoModel->getRequiredProductVersion());
                    if (!_config->isSet(CompilerConfiguration::ToolkitVersion))
                        _config->setStringValue(CompilerConfiguration::ToolkitVersion,
                                                infoModel->getVersion());
                    if (!_config->isSet(CompilerConfiguration::ToolkitDescription))
                        _config->setStringValue(CompilerConfiguration::ToolkitDescription,
                                                infoModel->getDescription());

                    // If any resources were specified in the info file, generate those
                    if (!(_genSourceModel || infoModel->getMessageSets().empty())) {
                        const vector<ToolkitInfo::MessageSetPtr>& messageSets =
                          infoModel->getMessageSets();
                        vector<ToolkitInfo::MessageSetPtr>::const_iterator it1;
                        for (it1 = messageSets.begin(); it1 != messageSets.end(); ++it1) {
                            buildMessageBundle(**it1, tkdir);
                        }
                    }

                    // Grab the sab file includes/excludes from the info model
                    includes = infoModel->includes();
                    excludes = infoModel->excludes();
                }
            }

            // and the very last default settings (if nothing seen at all)
            if (!_config->isSet(CompilerConfiguration::ToolkitName)) {
                string dirName = Utility::cleanPath(bf::system_complete(tkdir)).filename().string();
                if (dirName == "spl") {
                    dirName = "spl_"; // Can't conflict with Standard Toolkit!
                }
                _config->setStringValue(CompilerConfiguration::ToolkitName, dirName);
            }
            if (!_config->isSet(CompilerConfiguration::ToolkitProductVersion))
                _config->setStringValue(CompilerConfiguration::ToolkitProductVersion, "4.0");
            if (!_config->isSet(CompilerConfiguration::ToolkitVersion))
                _config->setStringValue(CompilerConfiguration::ToolkitVersion, "1.0.0");
            if (!_config->isSet(CompilerConfiguration::ToolkitDescription))
                _config->setStringValue(CompilerConfiguration::ToolkitDescription,
                                        "Toolkit Description");

            // Check versions for legality
            Distillery::RegEx version("^[0-9][0-9]*(\\.[0-9][0-9]*(\\.[0-9][0-9]*(\\.[0-9a-zA-Z_\\-"
                                      "][0-9a-zA-Z_\\-]*)?)?)?$");
            if (!version.match(_config->getStringValue(CompilerConfiguration::ToolkitVersion)))
                SysOut::errorln(SPL_CORE_TOOLKIT_INVALID_VERSION_NUMBER(
                  _config->getStringValue(CompilerConfiguration::ToolkitVersion)));
            // Version range is more complicated
            Distillery::RegEx versionRange(
              "^(([0-9][0-9]*(\\.[0-9][0-9]*(\\.[0-9][0-9]*(\\.[0-9a-zA-Z_\\-][0-9a-zA-Z_\\-]*)?)?)"
              "?)|([[(][0-9][0-9]*(\\.[0-9][0-9]*(\\.[0-9][0-9]*(\\.[0-9a-zA-Z_\\-][0-9a-zA-Z_\\-]*"
              ")?)?)?,[0-9][0-9]*(\\.[0-9][0-9]*(\\.[0-9][0-9]*(\\.[0-9a-zA-Z_\\-][0-9a-zA-Z_\\-]*)"
              "?)?)?[])]))$");
            if (!versionRange.match(
                  _config->getStringValue(CompilerConfiguration::ToolkitProductVersion)))
                SysOut::errorln(SPL_CORE_TOOLKIT_INVALID_PRODUCT_VERSION_NUMBER(
                  _config->getStringValue(CompilerConfiguration::ToolkitProductVersion)));

            if (!_genSourceModel) {
                // Run the Java indexer that will generate java function and operator XML files
                runJavaIndexer(tkdir);
            }
            // Walk the toolkit tree looking for primitive operators
            addPrimitiveOperators(tkdir);
            // And then do it again looking for native function definition files
            addNativeFunctionFiles(tkdir);
            // And then for .namespace files
            addDotNamespaceFiles(tkdir);

            // And one last time looking for SPL files to pull information out of
            _pathSearch->loadToolkits();
            scanSPLFiles(tkdir);
        }
        if (_genSourceModel) {
            createSourceModelFile(tkdir);
        } else {
            // And now bundle it together, and produce the required XML files
            createToolkitXMLFiles(tkdir, includes, excludes);
        }
    }
    SPLDBG("Ended spl-make-toolkit...", Debug::Core);
    return aet.getNumErrorsInCurrentTrap() ? EXIT_FAILURE : EXIT_SUCCESS;
}

void MakeToolkit::buildMessageBundle(ToolkitInfo::MessageSet const& messageSet,
                                     const bf::path& tkdir)
{
    bf::path toolkitDir = (!tkdir.is_complete()) ? bf::current_path() / tkdir : tkdir;
    toolkitDir = Utility::cleanPath(toolkitDir);
    bf::path scriptDir(_config->getStringValue(CompilerConfiguration::InternalScriptDir));
    bf::path smr = scriptDir /= "spl-make-resource";
    vector<string> args;
    args.push_back("--dir");
    args.push_back(toolkitDir.string());
    args.push_back("--name");
    args.push_back(messageSet.name());
    args.push_back("--tname");
    args.push_back(_config->getStringValue(CompilerConfiguration::ToolkitName));
    args.push_back("--default");
    args.push_back(messageSet.defaultLanguage());
    for (size_t i = 0, sz = messageSet.languages().size(); i < sz; ++i)
        args.push_back(messageSet.languages()[i]);
    if (ProcessLauncher::runProcess(smr.string(), args) != 0)
        SysOut::die(SPL_CORE_FAILED_TO_GENERATE_RESOURCE(messageSet.name()));
}

ofstream& openStreamOrDieTrying(const bf::path& file)
{
    const char* fn = file.string().c_str();
    ofstream* tks = new ofstream(fn);
    if (!tks) {
        // We failed to create the file.  Handle the case where we have permission on the directory,
        // but can't write the file
        if (bf::exists(file)) {
            Utility::removeFile(file, false);
            tks = new ofstream(fn);
        }
        // Test it again...
        if (!tks)
            SysOut::die(
              SPL_CORE_CANNOT_CREATE_FILE(file.string(), RuntimeUtility::getErrorNoStr()));
    }
    return *tks;
}

void MakeToolkit::createSourceModelFile(const bf::path& tkdir)
{
    if (!bf::exists(_sourceModelDir))
        Utility::createDirectory(_sourceModelDir);
    bf::path modelFile = bf::path(_sourceModelDir) / ".sourceModel.xml";
    try {
        bf::remove(modelFile);
    } catch (...) {
    } // remove the existing one
    std::auto_ptr<ofstream> postr(&openStreamOrDieTrying(modelFile));
    ofstream& ostr = *postr;
    auto_ptr<sourceModelType> srcModel;
    try {
        srcModel.reset(new sourceModelType());
        xmlns::prod::streams::spl::sourceCode::sourceModelType::sourceFile_sequence& seq =
          srcModel->sourceFile();
        for (SourceCodeModels::iterator it = _sourceCodeModels.begin();
             it != _sourceCodeModels.end(); ++it) {
            seq.push_back(**it);
            *it = NULL;
        }
        xml_schema::namespace_infomap map;
        map["common"].name = "http://www.ibm.com/xmlns/prod/streams/spl/common";
        map["srcCode"].name = "http://www.ibm.com/xmlns/prod/streams/spl/sourceCode";
        map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
        sourceModel(ostr, *srcModel, map);
    } catch (::xsd::cxx::exception& e) {
        SysOut::die(SPL_CORE_XML_CREATION_FAILED(modelFile.string(), e.what()));
    }

    // Make sure we write out the whole thing....
    ostr.flush();
    ostr.close();
    if (!ostr)
        SysOut::die(
          SPL_CORE_CANNOT_CREATE_FILE(modelFile.string(), RuntimeUtility::getErrorNoStr()));
}

void MakeToolkit::createToolkitXMLFiles(
  const bf::path& tkdir,
  const vector<pair<string, ToolkitInfo::ToolkitInfoModel::RootType> >& includes,
  const vector<string>& excludes)
{
    bf::path tkFile = tkdir / "toolkit.xml";
    // If there are no artifacts, then do not create the toolkit.xml
    // This helps when compiling a main composite from a toolkit, and not from
    // the application directory (which may be empty and thus does not need toolkit.xml)
    if (_namespaces.nothingToDo()) {
        try {
            bf::remove(tkFile);
        } catch (...) {
        } // remove the existing one
        SysOut::warnln(SPL_CORE_WARNING_NOTHING_IN_TOOLKIT(tkdir.string()));
        return;
    }
    ostringstream ostr;
    ostr << tkFile.string() << '.' << getpid();
    const bf::path tempName = ostr.str();

    try {
        std::auto_ptr<ofstream> tks(&openStreamOrDieTrying(tempName));

        ofstream& tkstream = *tks;
        toolkitFile = tkFile;
        auto_ptr<toolkitModelType> tkmodel;
        try {
            tkmodel.reset(new toolkitModelType());
            populateToolkit(tkdir, *tkmodel.get(), includes, excludes);
            tkmodel->productVersion(STREAMS_VERSION);

            xml_schema::namespace_infomap map;
            map[""].name = "http://www.ibm.com/xmlns/prod/streams/spl/toolkit";
            map["common"].name = "http://www.ibm.com/xmlns/prod/streams/spl/common";
            map["ti"].name = "http://www.ibm.com/xmlns/prod/streams/spl/toolkitInfo";
            map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
            toolkitModel(tkstream, *tkmodel, map);
        } catch (::xsd::cxx::exception& e) {
            try {
                bf::remove(tempName);
            } catch (...) {
            } // remove the temporary file
            SysOut::die(SPL_CORE_XML_CREATION_FAILED(tempName.string(), e.what()));
        }

        // Make sure we write out the whole thing....
        tkstream.flush();
        tkstream.close();
        if (!tkstream) {
            try {
                bf::remove(tempName);
            } catch (...) {
            } // remove the temporary file
            SysOut::die(
              SPL_CORE_CANNOT_CREATE_FILE(tempName.string(), RuntimeUtility::getErrorNoStr()));
        }
    } catch (...) {
        try {
            bf::remove(tempName);
        } catch (...) {
        } // remove the temporary file
    }
    // rename to final name (toolkit.xml)
    int ret = ::rename(tempName.string().c_str(), tkFile.string().c_str());
    if (ret != 0) {
        try {
            bf::remove(tempName);
        } catch (...) {
        } // remove the temporary file
        SysOut::errorln(SPL_CORE_CANNOT_RENAME_FILE(tempName.string(), tkFile.string(),
                                                    RuntimeUtility::getErrorNoStr()));
    }
}

void MakeToolkit::cleanupToolkit(const bf::path& tkdir)
{
    // Remove the toolkit.xml first
    SPLDBG("Cleaning " << tkdir.string(), Debug::Core);
    bf::path xmlFile = tkdir / "toolkit.xml";
    if (bf::exists(xmlFile)) {
        SPLDBG("Removing " << xmlFile.string(), Debug::Core);
        try {
            bf::remove(xmlFile);
        } catch (...) {
        }
    }

    // Now walk through and delete .pm files (that are from operators only) and spl.function.xml files
    walkDirectoryTree(tkdir, tkdir, &MakeToolkit::deletePMFiles, "", false);

    /// remove .spl files generated from .splmm files
    cleanupPreprocessedMixedModeFiles();

    /// remove any artifacts of resource processing
    cleanupNLArtifacts(tkdir);
}

static string getJavaClassPath(CompilerConfiguration& config)
{
    bf::path internalJarLoc = config.getStringValue(CompilerConfiguration::InstallDir);
    internalJarLoc /= "system/impl/lib/com.ibm.streams.spl.tools.internal.jar";
    return internalJarLoc.string();
}

static bf::path getJavaLibPath(CompilerConfiguration& config)
{
    // Always use the internal java for indexing purposes.
    bf::path result = config.getStringValue(CompilerConfiguration::JavaHome);
    result /= "jre/lib";
    bf::directory_iterator dir_end;
    for (bf::directory_iterator iArch(result); iArch != dir_end; iArch++) {
        bf::path arch = *iArch;
        if (bf::is_directory(arch)) {
            for (bf::directory_iterator iVmType(arch); iVmType != dir_end; iVmType++) {
                bf::path vmType = *iVmType;
                if (vmType.leaf() == "j9vm" || vmType.leaf() == "server") {
                    result = vmType;
                    goto archLoopDone;
                }
            }
        }
    }
archLoopDone:
    result /= "libjvm.so";
    return result;
}

void MakeToolkit::runJavaIndexer(const bf::path& tkdir)
{
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_4;
    vm_args.nOptions = 2;
    vm_args.ignoreUnrecognized = JNI_TRUE;

    auto_ptr<JavaVMOption> options(new JavaVMOption[vm_args.nOptions]);
    JavaVMOption* pOption = options.get();
    std::string classPath = std::string("-Djava.class.path=") + getJavaClassPath(*_config.get());
    pOption->optionString = (char*)classPath.c_str();
    ++pOption;
    pOption->optionString = (char*)"-Dcom.ibm.tools.attach.enable=no";
    ++pOption;

    vm_args.options = options.get();

    void* env = NULL;
    JavaVM* jvm = createJavaVM(env, vm_args);
    jclass processor =
      ((JNIEnv*)env)
        ->FindClass("com/ibm/streams/spl/internal/tkindexer/ToolkitAnnotationProcessor");
    if (((JNIEnv*)env)->ExceptionCheck()) {
        SysOut::die(SPL_CORE_STREAMS_ERROR("JVM setup error"));
    }

    jobjectArray stringArray;
    jstring tmp;
    jclass clsString;
    jint size = 3;
    jint index = 0;

    clsString = ((JNIEnv*)env)->FindClass("java/lang/String");
    stringArray = ((JNIEnv*)env)->NewObjectArray(size, clsString, 0);

    tmp = ((JNIEnv*)env)->NewStringUTF(tkdir.string().c_str());
    ((JNIEnv*)env)->SetObjectArrayElement(stringArray, index++, tmp);

    tmp = ((JNIEnv*)env)
            ->NewStringUTF((_config->getStringValue(CompilerConfiguration::ToolkitName)).c_str());
    ((JNIEnv*)env)->SetObjectArrayElement(stringArray, index++, tmp);

    tmp =
      ((JNIEnv*)env)
        ->NewStringUTF((_config->getStringValue(CompilerConfiguration::ToolkitVersion)).c_str());
    ((JNIEnv*)env)->SetObjectArrayElement(stringArray, index++, tmp);

    jmethodID method =
      ((JNIEnv*)env)->GetStaticMethodID(processor, "main", "([Ljava/lang/String;)V");
    ((JNIEnv*)env)->CallStaticVoidMethod(processor, method, stringArray);
    jvm->DestroyJavaVM();
}

JavaVM* MakeToolkit::createJavaVM(void*& env, JavaVMInitArgs& vm_args)
{
    // Load the shared object file (DLL).
    bf::path javaLibPath = getJavaLibPath(*_config.get());
    void* javaLibHandle = dlopen(javaLibPath.string().c_str(), RTLD_LAZY | RTLD_DEEPBIND);
    if (NULL == javaLibHandle)
        SysOut::die(SPL_CORE_STREAMS_ERROR("dlopen for JVM failed."));
    typedef jint CreateJVMFun(JavaVM**, void**, void*);
    CreateJVMFun* createJVM = (CreateJVMFun*)dlsym(javaLibHandle, "JNI_CreateJavaVM");
    if (NULL == createJVM)
        SysOut::die(SPL_CORE_STREAMS_ERROR("dlsym for JNI_CreateJavaVM failed"));
    typedef jint GetCreatedJVMsFun(JavaVM**, jsize, jsize*);
    GetCreatedJVMsFun* getCreatedJVMs =
      (GetCreatedJVMsFun*)dlsym(javaLibHandle, "JNI_GetCreatedJavaVMs");
    if (NULL == getCreatedJVMs)
        SysOut::die(SPL_CORE_STREAMS_ERROR("dlsym for JNI_GetCreatedJavaVMs failed"));

    // Create the JVM.
    JavaVM* jvm = NULL;
    jint cresult = (*createJVM)(&jvm, &env, &vm_args);
    if (JNI_OK != cresult) {
        jsize vmCount;
        jint gresult = (*getCreatedJVMs)(&jvm, 1, &vmCount);
        if (JNI_OK != gresult)
            SysOut::die(SPL_CORE_STREAMS_ERROR("JVM error"));
        SPLDBG("JavaOp: Found Java VM", Debug::Core);
        jvm->AttachCurrentThread(&env, NULL);
    } else {
        SPLDBG("JavaOp: Found Java VM", Debug::Core);
    }
    return jvm;
}

void MakeToolkit::addPrimitiveOperators(const bf::path& tkdir)
{
    walkDirectoryTree(tkdir, tkdir, &MakeToolkit::findPrimitiveOperators, "", false);
}

void MakeToolkit::scanSPLFiles(const bf::path& tkdir)
{
    walkDirectoryTree(tkdir, tkdir, &MakeToolkit::scanSPLFiles, "", false);
}

void MakeToolkit::addNativeFunctionFiles(const bf::path& tkdir)
{
    walkDirectoryTree(tkdir, tkdir, &MakeToolkit::findNativeFunctionFiles, "", false);
}

void MakeToolkit::addDotNamespaceFiles(const bf::path& tkdir)
{
    walkDirectoryTree(tkdir, tkdir, &MakeToolkit::findDotNamespaceFiles, "", false);
}

void MakeToolkit::walkDirectoryTree(const bf::path& rootDir,
                                    const bf::path& dir,
                                    WalkerFcn fcn,
                                    const string& insideReservedDirectory,
                                    const bool insidePrimitiveOperatorDirectory)
{
    static bf::directory_iterator end_itr;
    bool atTopDir = dir == rootDir;
    for (bf::directory_iterator it(dir); it != end_itr; it++) {
        if (!bf::exists(*it))
            continue;

        // handle known directories at top level
        string badTree = insideReservedDirectory;
        if (atTopDir) {
            const string dirName = it->path().filename().string();
            if (dirName == "output" || dirName == "doc")
                continue; // skip these without commenting
            if (Utility::reservedDirectory(dirName))
                badTree = dirName;
            else {
                // is it the first component?
                size_t pos = dirName.find('.');
                if (pos != string::npos) {
                    string firstPart = dirName.substr(0, pos);
                    if (Utility::reservedDirectory(dirName.substr(0, pos))) {
                        // This is only bad if this is a directory
                        if (bf::is_directory(*it))
                            badTree = firstPart;
                    }
                }
            }
        }

        const bf::path cleanedFile = Utility::cleanPath(*it);
        try {
            if (bf::is_directory(cleanedFile)) {
                // Skip output directories
                if (bf::exists(cleanedFile / ".sc.outdir")) {
                    SPLDBG("Skipping output directory : " << cleanedFile.string(), Debug::Core);
                    continue;
                }
                if (cleanedFile.leaf().string()[0] == '.') {
                    SPLDBG("Skipping hidden directory : " << cleanedFile.string(), Debug::Core);
                    continue;
                }
                walkDirectoryTree(rootDir, cleanedFile, fcn, badTree,
                                  insidePrimitiveOperatorDirectory ||
                                    isPrimitiveDirectory(rootDir, cleanedFile));
            } else if (!bf::symbolic_link_exists(cleanedFile))
                (this->*fcn)(rootDir, cleanedFile, badTree, insidePrimitiveOperatorDirectory);
        } catch (...) {
            // ignore entries that have disappeared while we are running
            SPLDBG("Ignoring exception during processing of file: " << cleanedFile.string(),
                   Debug::Core);
        }
    }
}

bool MakeToolkit::isPrimitiveDirectory(const bf::path& rootDir, const bf::path& dirName)
{
    string dName = dirName.filename().string();

    // Ignore native.function/function.xml
    if (dName == "native.function")
        return false;

    // Ignore tookitDir/toolkit.xml
    if (dName == "toolkit.xml" && rootDir == dirName)
        return false;

    vector<string> paths;
    Distillery::tokenize(dName, paths, ".", true);
    string opName = paths[paths.size() - 1];
    if (bf::exists(dirName / (opName + ".xml")))
        return true;

    return false;
}

bool MakeToolkit::isNonSplReservedDirectory(const string& dirName)
{
    return dirName != "samples" && dirName != "opt";
}

/*
 * Look for SPL files.  If found, invoke the frontend to do a 'light parse'.   This will call back
 * to us passing useful information, so that we can save it up.
 */
void MakeToolkit::scanSPLFiles(const bf::path& rootDir,
                               const bf::path& file,
                               const string& insideReservedDirectory,
                               const bool insidePrimitiveOperatorDirectory)
{
    string component = file.filename().string();
    if (component.length() < 5) // sizeof ?.spl
        return;

    // only look at SPL files
    if (component.substr(component.length() - 4) != ".spl")
        return;

    // Are we in an ignored directory tree?
    if (!insideReservedDirectory.empty()) {
        if (isNonSplReservedDirectory(insideReservedDirectory))
            SysOut::warnln(SPL_CORE_MAKETOOLKIT_IGNORING(file.string()));
        return;
    }

    if (insidePrimitiveOperatorDirectory) {
        SysOut::warnln(SPL_CORE_MAKETOOLKIT_IGNORING_PRIM(file.string()));
        return;
    }

    // Tell the frontend what to process
    string fileName = file.string();
    _config->setStringValue(CompilerConfiguration::ExplicitInputFile, fileName);

    SPLDBG("Process SPL file " << fileName, Debug::Core);
    // Compute current namespace to compare to source file namespace
    string rawNamespace;
    string currentNamespace;
    if (fileName == component) {
        // just top level
        rawNamespace = "";
    } else {
        // Walk past the rootDir
        bf::path r;

        // Skip the initial part of the path
        bf::path::iterator it = file.begin();
        for (uint32_t i = 0; i < _rootDirCount; i++)
            ++it;

        // Copy the rest of the path
        while (it != file.end())
            r /= *it++;

        rawNamespace = r.parent_path().string();
        fileName = r.string();
        currentNamespace = rawNamespace;
        if (!checkAndFixupNamespacePath(r.parent_path(), currentNamespace, component))
            return; // ignore it
    }

    // Hold this information for the callbacks
    _namespaces.setNamespace(currentNamespace, rawNamespace);
    _namespaces.setFile(file, fileName);
    _frontEnd->extractSignatures(*this);
    SPLDBG("Finished SPL file " << fileName, Debug::Core);
}

bool MakeToolkit::checkAndFixupNamespacePath(const bf::path& path,
                                             string& namespacePath,
                                             const string& item)
{

    // Is the path composed of valid names that are legal for SPL ids?
    bool first = true;
    for (bf::path::const_iterator it = path.begin(); it != path.end(); it++) {
        if (it->empty())
            continue;
        // Check to make sure we have no invalid paths
        vector<string> paths;
        Distillery::tokenize((*it).string(), paths, ".", true);
        for (uint32_t i = 0, n = paths.size(); i < n; i++, first = false) {
            const string& c = paths[i];
            if (c.empty() || Utility::reservedName(c) || Utility::reservedDirectory(c) ||
                (first && c == "streams") || !_validNamespaceComponent.match(c)) {
                SysOut::warnln(SPL_CORE_INVALID_NAMESPACE_PATHNAME(path.string(), item));
                return false;
            }
        }
    }

    // remove trailing '/'
    while (namespacePath.size() > 0 && namespacePath[namespacePath.size() - 1] == '/')
        namespacePath.erase(namespacePath.size() - 1);
    string origPath = namespacePath;

    // replace '/' by '.'
    for (string::iterator it = namespacePath.begin(); it != namespacePath.end(); it++)
        if (*it == '/')
            *it = '.';

    NameSpaceMapType::const_iterator it = _namespacePathMap.find(namespacePath);
    if (it == _namespacePathMap.end())
        _namespacePathMap.insert(make_pair(namespacePath, origPath));
    else if (it->second != origPath) {
        // We have a mismatch - complain
        SysOut::errorln(
          SPL_CORE_MULTIPLE_PATHS_TO_SAME_NAMESPACE(namespacePath, it->second, origPath));

        // Use the new one to minimize error messages
        _namespacePathMap.insert(make_pair(namespacePath, origPath));
    }
    return true;
}

void MakeToolkit::findNativeFunctionFiles(const bf::path& rootDir,
                                          const bf::path& file,
                                          const string& insideReservedDirectory,
                                          const bool insidePrimitiveOperatorDirectory)
{
    string component = file.filename().string();
    if (component != "function.xml" && component != "javaFunction.xml")
        return;

    // parent must also be directory native.function
    if (file.parent_path().filename() != "native.function")
        return;

    // Are we in an ignored directory tree?
    if (!insideReservedDirectory.empty()) {
        if (isNonSplReservedDirectory(insideReservedDirectory))
            SysOut::warnln(SPL_CORE_MAKETOOLKIT_IGNORING(file.string()));
        return;
    }

    if (insidePrimitiveOperatorDirectory) {
        SysOut::warnln(SPL_CORE_MAKETOOLKIT_IGNORING_PRIM(file.string()));
        return;
    }

    // Compute namespace
    string fileName = file.string();
    string namespacePath;
    bf::path upTwo = file.parent_path().parent_path();
    if (upTwo.empty()) {
        namespacePath = "";
    } else {
        // Walk past the rootDir
        bf::path f;

        // Skip the initial part of the path
        bf::path::iterator it = file.begin();
        for (uint32_t i = 0; i < _rootDirCount; i++)
            ++it;

        // Copy the rest of the path(s)
        while (it != file.end())
            f /= *it++;
        namespacePath = f.parent_path().parent_path().string();
        fileName = f.string();
        if (!checkAndFixupNamespacePath(f.parent_path().parent_path(), namespacePath,
                                        "native.function/function.xml"))
            return; // ignore it
    }

    _namespaces.addNativeFile(namespacePath, fileName);
}

void MakeToolkit::findDotNamespaceFiles(const bf::path& rootDir,
                                        const bf::path& file,
                                        const string& insideReservedDirectory,
                                        const bool insidePrimitiveOperatorDirectory)
{
    string component = file.filename().string();
    if (component != ".namespace")
        return;

    // Are we in an ignored directory tree?
    if (!insideReservedDirectory.empty()) {
        if (isNonSplReservedDirectory(insideReservedDirectory))
            SysOut::warnln(SPL_CORE_MAKETOOLKIT_IGNORING(file.string()));
        return;
    }

    if (insidePrimitiveOperatorDirectory) {
        SysOut::warnln(SPL_CORE_MAKETOOLKIT_IGNORING_PRIM(file.string()));
        return;
    }

    // Compute namespace
    bf::path f;

    // Skip the initial part of the path
    bf::path::iterator it = file.begin();
    for (uint32_t i = 0; i < _rootDirCount; i++)
        ++it;

    // Copy the rest of the path(s)
    while (it != file.end())
        f /= *it++;

    bf::path upOne = f.parent_path();
    if (upOne.empty())
        return; // No need to add the namespace in the top level

    string namespacePath = upOne.string();

    if (!checkAndFixupNamespacePath(upOne, namespacePath, ".namespace"))
        return; // ignore it
    _namespaces.addNamespace(namespacePath);
}

void MakeToolkit::findPrimitiveOperators(const bf::path& rootDir,
                                         const bf::path& file,
                                         const string& insideReservedDirectory,
                                         const bool insidePrimitiveOperatorDirectory)
{
    string component = file.filename().string();
    if (component.length() < 5) // sizeof ?.xml
        return;

    // only look at XML files
    if (component.substr(component.length() - 4) != ".xml")
        return;

    // Valid names for the parent are [namespaceComponentName.]*Operator
    // is this a valid path?
    string parentDirName = file.parent_path().filename().string();

    // Ignore native.function/function.xml
    if (parentDirName == "native.function")
        return;

    vector<string> parentPaths;
    Distillery::tokenize(parentDirName, parentPaths, ".", true);
    string opName = parentPaths[parentPaths.size() - 1];

    // Does the last component match?
    if (parentPaths.size() == 0 || (parentPaths[parentPaths.size() - 1] + ".xml") != component)
        return; // Can't be operator XML

    string dirNamespace = parentDirName.substr(0, parentDirName.length() - opName.length());
    if (!dirNamespace.empty() && dirNamespace[dirNamespace.length() - 1] == '.')
        dirNamespace = dirNamespace.substr(0, dirNamespace.length() - 1);

    // Ignore tookitDir/toolkit.xml
    if (component == "toolkit.xml" && rootDir == file.parent_path())
        return;

    // Are we in an ignored directory tree?
    if (!insideReservedDirectory.empty()) {
        if (isNonSplReservedDirectory(insideReservedDirectory))
            SysOut::warnln(SPL_CORE_MAKETOOLKIT_IGNORING(file.string()));
        return;
    }

    // Okay, we have an operator XML file.   Remember it.
    // Compute the namespace for this operator

    // Walk past the rootDir
    bf::path f;

    // Skip the initial part of the path
    bf::path::iterator it = file.begin();
    for (uint32_t i = 0; i < _rootDirCount; i++)
        ++it;

    // Copy the rest of the path(s)
    while (it != file.end())
        f /= *it++;

    string namespacePath = f.parent_path().parent_path().string();
    // combine the two namespace paths
    if (!dirNamespace.empty()) {
        if (namespacePath.empty())
            namespacePath = dirNamespace;
        else
            namespacePath += "." + dirNamespace;
    }

    string fileName = f.parent_path().string();

    if (!checkAndFixupNamespacePath(f.parent_path(), namespacePath, component))
        return; // ignore it
    SPLDBG("Found operator " << namespacePath << "::" << opName, Debug::Core);
    if (_config->getBooleanValue(CompilerConfiguration::ToolkitMakeOperator)) {
        // Need to run spl-make-operator
        vector<string> args;
        if (_config->getBooleanValue(CompilerConfiguration::ToolkitSilent))
            args.push_back("-s");
        args.push_back("-i");
        args.push_back(file.parent_path().string());
        int res = ProcessLauncher::runProcess(_splMakeOperator, args);
        if (res != 0)
            SysOut::errorln(SPL_CORE_MAKE_OPERATOR_FAILED_DURING_TOOLKIT_INDEXING);
    }
    // Save this information for later
    _namespaces.addPrimitiveOperator(namespacePath, opName, fileName);
}

void MakeToolkit::deletePMFiles(const bf::path& rootDir,
                                const bf::path& file,
                                const string& insideReservedDirectory,
                                const bool insidePrimitiveOperatorDirectory)
{
    // If this isn't under a primitive operator, nothing to do
    if (!insidePrimitiveOperatorDirectory)
        return;

    // Valid names for the parent are [namespaceComponentName.]*Operator
    // is this a valid path?
    bf::path parent = file.parent_path();
    string parentDirName = parent.filename().string();

    // Ignore native.function/function.xml
    if (parentDirName == "native.function")
        return;

    vector<string> parentPaths;
    Distillery::tokenize(parentDirName, parentPaths, ".", true);
    string opName = parentPaths[parentPaths.size() - 1];

    // only look at XML files
    if (file.filename() != opName + ".xml")
        return;

    // remove <opName>_h.pm and <opName>_cpp.pm files
    try {
        bf::path f = parent / (opName + "_h.pm");
        if (bf::exists(f)) {
            SPLDBG("Removing " << f.string(), Debug::Core);
            bf::remove(f);
        }
    } catch (...) {
    }
    try {
        bf::path f = parent / (opName + "_cpp.pm");
        if (bf::exists(f)) {
            SPLDBG("Removing " << f.string(), Debug::Core);
            bf::remove(f);
        }
    } catch (...) {
    }
}

string MakeToolkit::checkStreamsInstallation()
{
    string instDir;
    try {
        instDir = getInstallationDirectory();
    } catch (const DistilleryException& e) {
        SysOut::die(SPL_CORE_STREAMS_ERROR(e.getExplanation()));
    }
    return instDir;
}

void MakeToolkit::computeSPLPaths()
{
    // Figure out the default pathnames to be used
    bf::path installDir = _config->getStringValue(CompilerConfiguration::InstallDir);
    bf::path xsdDir = installDir / "etc" / "xsd" / "SPL";
    bf::path scriptDir = installDir / "bin";
    bf::path statDir = installDir / "/etc" / "stat";
    bf::path toolkitDir = installDir / "toolkits" / "spl";
    bf::path templateDir = installDir / "lib" / "spl" / "template";
    _splMakeOperator = (scriptDir / "spl-make-operator").string();
    _config->setStringValue(CompilerConfiguration::XsdDir, xsdDir.string());
    _config->setStringValue(CompilerConfiguration::StatsDir, statDir.string());
    _config->setStringValue(CompilerConfiguration::ScriptDir, scriptDir.string());
    _config->setStringValue(CompilerConfiguration::ToolkitDir, toolkitDir.string());
    setenv("STREAMS_TOOLKIT_DIR", toolkitDir.string().c_str(), 0);
    _config->setStringValue(CompilerConfiguration::TemplateDir, templateDir.string());

    // Walk through the toolkit string (if any), splitting up the paths
    // append the default path

    // Do we have a user specified option?
    string userPaths;
    if (_config->isSet(CompilerConfiguration::SPLPath)) {
        userPaths = _config->getStringValue(CompilerConfiguration::SPLPath);
    }

    // Walk through the string, separating paths at SPLPATH_SEP
    vector<string> paths;
    Distillery::tokenize(userPaths, paths, SPLPATH_SEP, false);

    // Now the default path for builtins
    paths.push_back((toolkitDir).string());

    SPLDBG("Lookup paths for 'use's: ", Debug::Core);
    for (unsigned int i = 0; i < paths.size(); i++) {
        SPLDBG("  [" << i << "]\t" << paths[i], Debug::Core);
    }
    _pathSearch->setPaths(paths);
}

void MakeToolkit::getDistilleryApplicationVisibleOptions(option_vector_t& options)
{
    // We don't want any of the default visible options
}

void MakeToolkit::getArguments(option_vector_t& options)
{
    setKeepRemainingArguments();
    option_t args[] = {
        { 'h', "help", 0, "", SPL_CORE_OPTION_HELP, STR_OPT(MakeToolkit::setHelp), 0, 0 },
        //      { 'b', "build-using-source-tree", 0, "", SPL_CORE_OPTION_BUILD_USING_SOURCE_TREE, STR_OPT(MakeToolkit::setSourceTreeBuild), 1, 0},
        { 'c', "clean", 0, "", SPL_CORE_MAKE_TOOLKIT_OPTION_CLEAN, STR_OPT(MakeToolkit::setClean),
          0, 0 },
        { 'd', "description", ARG, "", SPL_CORE_MAKE_TOOLKIT_OPTION_DESCRIPTION,
          STR_OPT(MakeToolkit::setDescription), 0, 0 },
        { 0, "force-generate", 0, "", SPL_CORE_MAKE_TOOLKIT_OPTION_FORCE_GENERATE,
          STR_OPT(MakeToolkit::setForceGenerate), 1, 0 },
        { 'i', "directory", ARG, "", SPL_CORE_MAKE_TOOLKIT_OPTION_DIRECTORY,
          STR_OPT(MakeToolkit::setDirectory), 0, 0 },
        { 'm', "make-operator", 0, "", SPL_CORE_MAKE_TOOLKIT_OPTION_MAKE_OPERATOR,
          STR_OPT(MakeToolkit::setMakeOperator), 0, 0 },
        { 'n', "name", ARG, "", SPL_CORE_MAKE_TOOLKIT_OPTION_NAME, STR_OPT(MakeToolkit::setName), 0,
          0 },
        { 'r', "product-version", ARG, "", SPL_CORE_MAKE_TOOLKIT_OPTION_PRODUCT_VERSION,
          STR_OPT(MakeToolkit::setCompilerVersion), 0, 0 },
        { 's', "silent", 0, "", SPL_CORE_MAKE_TOOLKIT_OPTION_SILENT,
          STR_OPT(MakeToolkit::setSilent), 0, 0 },
        { 'v', "version", ARG, "", SPL_CORE_MAKE_TOOLKIT_OPTION_VERSION,
          STR_OPT(MakeToolkit::setVersion), 0, 0 },
        { 0, "no-mixed-mode-preprocessing", 0, "", SPL_CORE_OPTION_NO_MIXED_MODE_PREPROCESSING,
          STR_OPT(MakeToolkit::setNoMixedModePreprocessing), 0, 0 },
        { 0, "verbose-model-errors", 0, "", SPL_CORE_OPTION_VERBOSE_MODEL_ERRORS,
          STR_OPT(MakeToolkit::setVerboseModelErrors), 0, 0 },
        { 0, "spl-dump", ARG, "STREAMS_SPL_DUMP", SPL_CORE_OPTION_STREAMS_SPL_DUMP,
          STR_OPT(MakeToolkit::setDebugString), 1, 0 },
        { 0, "gen-source-model", ARG, "", SPL_CORE_OPTION_GEN_SOURCE_MODEL,
          STR_OPT(MakeToolkit::setGenSourceModel), 1, 0 }
    };
    APPEND_OPTIONS(options, args);
}

void MakeToolkit::setDebugString(const option_t* option, const char* value)
{
    Debug::instance().addDebugString(value);
}

void MakeToolkit::setHelp(const option_t* option, const char* value)
{
    printUsage(cout, false);
    cout << endl;
    Distillery::DistAppTerminate(0);
}

ostream& MakeToolkit::printDescription(ostream& o) const
{
    o << SPL_CORE_MAKE_TOOLKIT_USAGE;
    return o;
}

void MakeToolkit::setClean(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::Clean, true);
}

void MakeToolkit::setDescription(const option_t* option, const char* value)
{
    _config->setStringValue(CompilerConfiguration::ToolkitDescription, value);
}

void MakeToolkit::setForceGenerate(const option_t* option, const char* value)
{
    _forceGeneration = true;
}

void MakeToolkit::setDirectory(const option_t* option, const char* value)
{
    _config->setStringValue(CompilerConfiguration::MakeToolkitDir, value);
}

void MakeToolkit::setMakeOperator(const option_t* option, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::ToolkitMakeOperator, true);
}

void MakeToolkit::setName(const option_t* option, const char* value)
{
    _config->setStringValue(CompilerConfiguration::ToolkitName, value);
}

void MakeToolkit::setSilent(const option_t* option, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::ToolkitSilent, true);
}

void MakeToolkit::setCompilerVersion(const option_t* option, const char* value)
{
    _config->setStringValue(CompilerConfiguration::ToolkitProductVersion, value);
}

void MakeToolkit::setVersion(const option_t* option, const char* value)
{
    _config->setStringValue(CompilerConfiguration::ToolkitVersion, value);
}

void MakeToolkit::setNoMixedModePreprocessing(const option_t*, const char* value)
{
    _config->setBooleanValue(CompilerConfiguration::NoMixedModePreprocessing, true);
}

void MakeToolkit::setVerboseModelErrors(const option_t*, const char* value)
{
    _verboseModelErrors = true;
}

void MakeToolkit::setGenSourceModel(const option_t*, const char* value)
{
    _genSourceModel = true;
    _sourceModelDir = value;
}

/*
 * Callback routines from Frontend light parsing
 */
void MakeToolkit::setNamespace(const string& ns, const SourceLocation& loc)
{
    _namespaces.setNamespace(ns, loc);
}

void MakeToolkit::addFunction(const string& f, AstNode& hdr)
{
    _namespaces.addFunction(hdr);
}

void MakeToolkit::addComposite(const string& c, AstNode& hdr)
{
    _namespaces.addComposite(c, hdr);
}

void MakeToolkit::finishComposite(const string& c, AstNode& hdr)
{
    _namespaces.finishComposite(c, hdr);
}

void MakeToolkit::addTypedef(const string& t, AstNode& hdr)
{
    _namespaces.addType(t, hdr);
}

void MakeToolkit::finishTypedef(const string& t, AstNode& hdr)
{
    _namespaces.finishType(t, hdr);
}

void MakeToolkit::addEnum(AstNode& n)
{
    _namespaces.addEnum(n);
}

bool MakeToolkit::NameSpaces::checkName(const string& namespaceName,
                                        const string& fileName,
                                        NameLoc& map,
                                        const NameLocation& nl)
{
    NameLoc::const_iterator it = map.find(nl);
    if (it != map.end()) {
        SourceLocation newLoc(fileName, nl.line(), nl.col());
        if (0 == namespaceName.size())
            SysOut::errorln(SPL_CORE_DUPLICATE_TOOLKIT_NAME_DEFAULTNS(nl.name()), newLoc);
        else
            SysOut::errorln(SPL_CORE_DUPLICATE_TOOLKIT_NAME(nl.name(), namespaceName), newLoc);
        // If the name we are giving details on is a primitive operator, then manufacture a filename for it
        const PrimitiveOperators& pOps = _map[namespaceName]._primitives;
        string opFileName = pOps.find(nl.name());
        string firstFileName = (!opFileName.empty()) ? (opFileName + ".xml") : it->second;
        SourceLocation oldLoc(firstFileName, it->first.line(), it->first.col());
        SysOut::detailsErrorln(SPL_FRONT_END_PREVIOUS_LOCATION(nl.name()), oldLoc);
        return false;
    }
    map.insert(make_pair(nl, fileName));
    return true;
}

bool MakeToolkit::NameSpaces::checkNameInNamespace(const string& namespaceName,
                                                   const string& fileName,
                                                   const NameLocation& nl)
{
    return checkName(namespaceName, fileName, _map[namespaceName]._allNames, nl);
}

int MakeToolkit::NameSpaces::fileUriIndex(const string& fileName)
{
    bf::path f(fileName);
    if (f.is_complete())
        return uriIndex(string("file://", 7) + fileName);
    return uriIndex(fileName);
}

int MakeToolkit::NameSpaces::uriIndex(const string& uri)
{
    const string encodedUri = Utility::uriEncode(uri);
    URITableType::const_iterator it = _uriTable.find(encodedUri);
    if (it != _uriTable.end())
        return it->second;
    int result = _uriTable.size() + 1;
    _uriTable.insert(make_pair(encodedUri, result));
    return result;
}

void addExclude(sabFilesType& sabFiles, const string& path)
{
    auto_ptr<excludeType> exclude(new excludeType(path));
    sabFiles.exclude().push_back(exclude);
}

void addInclude(sabFilesType& sabFiles, const string& path, const string& root)
{
    auto_ptr<includeType> include(new includeType(path));
    include->root(root);
    sabFiles.include().push_back(include);
}

void MakeToolkit::populateToolkit(
  const bf::path& tkdir,
  toolkitModelType& tkm,
  const vector<pair<string, ToolkitInfo::ToolkitInfoModel::RootType> >& includes,
  const vector<string>& excludes)
{
    // Since we only support one toolkit per XML from spl-make-toolkit, do that here
    auto_ptr<toolkitType> tk(new toolkitType());

    // Information about the toolkit itself
    const string& tkName = _config->getStringValue(CompilerConfiguration::ToolkitName);
    if (tkName.empty())
        SysOut::errorln(SPL_CORE_EMPTY_TOOLKIT_NAME);
    tk->name(tkName);
    tk->version(_config->getStringValue(CompilerConfiguration::ToolkitVersion));
    tk->requiredProductVersion(
      _config->getStringValue(CompilerConfiguration::ToolkitProductVersion));
    tk->description(_config->getStringValue(CompilerConfiguration::ToolkitDescription));

    _namespaces.populate(tkdir, *tk.get(), *this);
    if (_info.get()) {
        // Copy dependencies to toolkit
        tk->dependency(_info->dependencies().toolkit());
    }

    static const char* roots[] = { "toolkitDir", "applicationDir", "outputDir" };

    auto_ptr<sabFilesType> sabFiles(new sabFilesType);
    sabFilesType& sf = *sabFiles.get();
    addInclude(sf, "toolkit.xml", "toolkitDir");
    bf::path impl("impl");
    bf::path implJavaLib = impl / "java" / "lib" / "**";
    addInclude(sf, implJavaLib.string(), "toolkitDir");
    bf::path implJavaBin = impl / "java" / "bin" / "**";
    addInclude(sf, implJavaBin.string(), "toolkitDir");
    bf::path implBin = impl / "bin" / "**";
    addInclude(sf, implBin.string(), "toolkitDir");
    bf::path implLib = impl / "lib" / "**";
    addInclude(sf, implLib.string(), "toolkitDir");
    bf::path implNl = impl / "nl" / "*.dat";
    addInclude(sf, implNl.string(), "toolkitDir");
    bf::path etc = bf::path("etc") / "**";
    addInclude(sf, etc.string(), "toolkitDir");
    bf::path lib = bf::path("lib") / "**";
    addInclude(sf, lib.string(), "toolkitDir");
    bf::path nl = bf::path("nl") / "**";
    addInclude(sf, nl.string(), "toolkitDir");
    bf::path opt = bf::path("opt") / "**";
    addInclude(sf, opt.string(), "toolkitDir");

    // Add the explicit includes
    for (vector<pair<string, ToolkitInfo::ToolkitInfoModel::RootType> >::const_iterator
           it = includes.begin(),
           itEnd = includes.end();
         it != itEnd; ++it) {
        const string& antPattern = it->first;
        if (bf::path(antPattern).is_relative()) {
            addInclude(sf, antPattern, roots[it->second]);
        } else {
            SysOut::errorln(
              SPL_CORE_NON_RELATIVE_INCLUDE_EXCLUDE_PATH((tkdir / "info.xml").string()));
        }
    }

    // Add the explicit excludes
    for (vector<string>::const_iterator it1 = excludes.begin(), it1End = excludes.end();
         it1 != it1End; ++it1) {
        addExclude(sf, *it1);
    }

    tk->sabFiles(sabFiles);

    tkm.toolkit().push_back(tk);
}

MakeToolkit::NameSpaces::NameSpaces()
  : _inComposite(false)
  , _currentTypedef(NULL)
{}

void MakeToolkit::NameSpaces::populate(const bf::path& tkdir,
                                       toolkitType& tk,
                                       const MakeToolkit& mt)
{
    // For each namespace in the toolkit, add the names in that namespace
    for (NSMap::const_iterator it = _map.begin(); it != _map.end(); it++) {
        auto_ptr<toolkitNamespaceType> ns(new toolkitNamespaceType());
        ns->name(it->first);

        // Add all the native functions to the toolkit
        it->second._nativeFunctions.populate(tkdir, *ns.get(), *this, mt);

        // Now the primitive operators
        it->second._primitives.populate(tkdir, *ns.get(), *this, mt);

        // composites operators
        it->second._composites.populate(tkdir, *ns.get());

        // SPL functions
        it->second._splFunctions.populate(tkdir, *ns.get());

        // SPL Types
        it->second._splTypes.populate(*ns.get());

        // Enum Literals
        it->second._splEnumLits.populate(*ns.get());

        tk.namespace_().push_back(ns);
    }

    // Fill in the URI Table, if present
    if (!_uriTable.empty()) {
        auto_ptr<uriTableType> uritable(new uriTableType());
        URITableType::const_iterator it = _uriTable.begin();
        for (; it != _uriTable.end(); it++) {
            uritable->uri().push_back(uriType(it->second, it->first));
        }
        tk.uriTable(uritable);
    }
}

static void populateBaseFunctionType(baseFunctionType& fcn, AstNode& ast)
{
    if (!AST::functionHead::is(ast))
        return; // error case
    fcn.name(astText(AST::functionHead::id(ast)));
    for (int i = 0, n = AST::functionHead::functionFormalCount(ast); i < n; i++) {
        auto_ptr<parameterType> param(new parameterType());
        AstNode& formal = AST::functionHead::functionFormal(ast, i);
        ostringstream os;
        prettyPrint(os, AST::functionFormal::type(formal));
        param->type(os.str());
        param->name(astText(AST::functionFormal::id(formal)));
        AstNode& modifiersAst = AST::functionFormal::modifiers(formal);
        for (int j = 0, nj = AST::modifiers::modifierCount(modifiersAst); j < nj; j++)
            if (AST::mutableT::is(AST::modifiers::modifier(modifiersAst, j)))
                param->mutable_(true);
        fcn.parameter().push_back(param);
    }

    ostringstream os;
    prettyPrint(os, AST::functionHead::type(ast));
    fcn.returnType(os.str());
}

static void populateBaseFunctionTypeProtoType(baseFunctionType& fcn, AstNode& ast)
{
    fcn.name(astText(AST::functionPrototype::id(ast)));
    AstNode& protoFormals = AST::functionPrototype::protoFormals(ast);
    if (!AST::protoFormals::is(protoFormals))
        return; // error case
    for (int i = 0, n = AST::protoFormals::protoFormalCount(protoFormals); i < n; i++) {
        auto_ptr<parameterType> param(new parameterType());
        AstNode& formal = AST::protoFormals::protoFormal(protoFormals, i);
        ostringstream os;
        prettyPrint(os, AST::protoFormal::type(formal));
        param->type(os.str());
        if (AST::protoFormal::has_id(formal))
            param->name(astText(AST::protoFormal::id(formal)));
        else {
            // make one up
            ostringstream os2;
            os2 << "__p" << i + 1;
            param->name(os2.str());
        }
        AstNode& modifiersAst = AST::protoFormal::modifiers(formal);
        for (int j = 0, nj = AST::modifiers::modifierCount(modifiersAst); j < nj; j++)
            if (AST::mutableT::is(AST::modifiers::modifier(modifiersAst, j))) {
                param->mutable_(true);
                break;
            }
        fcn.parameter().push_back(param);
    }

    ostringstream os;
    prettyPrint(os, AST::functionPrototype::type(ast));
    fcn.returnType(os.str());
}

static AstNode& getNativeFunctionInformation(baseFunctionType& func,
                                             const string& prototype,
                                             const bf::path& fileName,
                                             bool isNativeFunction)
{
    SPLAst& splAST =
      SPLAst::functionPrototypeFromString(fileName.string(), 0, 0, prototype, isNativeFunction);
    if (AST::functionPrototype::is(splAST.ast()))
        populateBaseFunctionTypeProtoType(func, splAST.ast());
    return splAST.ast();
}

using namespace xmlns::prod::streams::spl::operator_;
static enumWindowPunctInputModeType::value getPunctMode(windowPunctuationInputModeType::value v)
{
    switch (v) {
        case windowPunctuationInputModeType::Expecting:
            return enumWindowPunctInputModeType::Expecting;
        case windowPunctuationInputModeType::Oblivious:
            return enumWindowPunctInputModeType::Oblivious;
        case windowPunctuationInputModeType::WindowBound:
            return enumWindowPunctInputModeType::WindowBound;
    }
    return enumWindowPunctInputModeType::WindowBound;
}

static enumWindowPunctOutputModeType::value getPunctMode(windowPunctuationOutputModeType::value v)
{
    switch (v) {
        case windowPunctuationOutputModeType::Generating:
            return enumWindowPunctOutputModeType::Generating;
        case windowPunctuationOutputModeType::Free:
            return enumWindowPunctOutputModeType::Free;
        case windowPunctuationOutputModeType::Preserving:
            return enumWindowPunctOutputModeType::Preserving;
    }
    return enumWindowPunctOutputModeType::Preserving;
}

static enumWindowPolicyType::value getWindowMode(windowingModeType::value v)
{
    switch (v) {
        case windowingModeType::NonWindowed:
            return enumWindowPolicyType::NonWindowed;
        case windowingModeType::Windowed:
            return enumWindowPolicyType::Windowed;
        case windowingModeType::OptionallyWindowed:
            return enumWindowPolicyType::OptionallyWindowed;
    }
    return enumWindowPolicyType::OptionallyWindowed;
}

static enumExpressionModeType::value getExpressionMode(expressionModeType::value v)
{
    switch (v) {
        case expressionModeType::Attribute:
            return enumExpressionModeType::Attribute;
            break;
        case expressionModeType::AttributeFree:
            return enumExpressionModeType::AttributeFree;
            break;
        case expressionModeType::Constant:
            return enumExpressionModeType::Constant;
            break;
        case expressionModeType::CustomLiteral:
            return enumExpressionModeType::CustomLiteral;
            break;
        case expressionModeType::Expression:
            return enumExpressionModeType::Expression;
            break;
        case expressionModeType::Nonexistent:
            return enumExpressionModeType::Nonexistent;
    }
    return enumExpressionModeType::Nonexistent;
}

static string makeUri(const bf::path& path, const string& uri)
{
    // Is this a real URI?
    if (uri.find("://") != string::npos)
        return uri;

    // No, it is relative to the path of the xml
    if (uri.find("..") == 0) {
        // Lets clean this up if we can
        bf::path p = path.parent_path();
        bf::path u = uri;
        bool copying = false;
        for (bf::path::iterator it = u.begin(); it != u.end(); it++) {
            if (copying)
                p /= *it;
            else {
                // If we still have a leading .., clean up the path
                if (*it == ".." && !p.empty())
                    p = p.parent_path();
                else {
                    copying = true;
                    p /= *it;
                }
            }
        }
        return p.string();
    }
    return (path.parent_path() / uri).string();
}

// Extract the information from the prototype
void MakeToolkit::PrimitiveOperators::populate(
  const bf::path& tkdir,
  const bf::path& path,
  primitiveToolkitOperatorType& prim,
  const xmlns::prod::streams::spl::operator_::opModelType& model,
  bool isJava,
  NameSpaces& n) const
{
    // Start generating the information
    const contextType& context = model.context();
    if (context.description().present())
        prim.description(context.description().get());

    prim.public_(true);
    if (context.customLiterals().present()) {
        const enumerationsType& lits = context.customLiterals().get();
        const enumerationsType::enumeration_sequence& enums = lits.enumeration();
        enumerationsType::enumeration_const_iterator it;
        for (it = enums.begin(); it != enums.end(); it++) {
            auto_ptr<customLiteralsType> cLit(new customLiteralsType());
            cLit->name(it->name());
            const enumerationType::value_sequence& values = it->value();
            for (enumerationType::value_const_iterator it2 = values.begin(); it2 != values.end();
                 it2++) {
                auto_ptr<customLiteralValueType> v(new customLiteralValueType());
                v->name(*it2);
                cLit->value().push_back(v);
            }
            prim.customLiterals().push_back(cLit);
        }
    }

    if (context.customOutputFunctions().present()) {
        const customOutputFunctionsType& outputs = context.customOutputFunctions().get();
        const customOutputFunctionsType::customOutputFunction_sequence& ofs =
          outputs.customOutputFunction();
        customOutputFunctionsType::customOutputFunction_const_iterator it;
        typedef xmlns::prod::streams::spl::toolkit::outputFunctionsType outputFunctionsType;
        for (it = ofs.begin(); it != ofs.end(); it++) {
            auto_ptr<outputFunctionsType> cFunc(new outputFunctionsType());
            cFunc->name(it->name());
            const customOutputFunctionSetType::function_sequence& values = it->function();
            for (customOutputFunctionSetType::function_const_iterator it2 = values.begin();
                 it2 != values.end(); it2++) {
                auto_ptr<baseFunctionType> v(new baseFunctionType());
                v->prototype(it2->prototype());
                if (it2->description().present())
                    v->description(it2->description().get());
                // Extract the information from the prototype
                getNativeFunctionInformation(*v, it2->prototype(), path, false);
                cFunc->function().push_back(v);
            }
            prim.outputFunctions().push_back(cFunc);
        }
    }

    // Code templates for the operator
    if (context.codeTemplates().present()) {
        const codeTemplatesType& codeTemplates = context.codeTemplates().get();
        const codeTemplatesType::codeTemplate_sequence& tmplts = codeTemplates.codeTemplate();
        codeTemplatesType::codeTemplate_const_iterator it;
        typedef xmlns::prod::streams::spl::toolkit::codeTemplateType codeTemplateType;
        for (it = tmplts.begin(); it != tmplts.end(); it++) {
            auto_ptr<codeTemplateType> codeTemplate(new codeTemplateType());
            codeTemplate->name(it->name());
            if (it->description().present())
                codeTemplate->description(it->description().get());
            codeTemplate->template_(it->template_());
            prim.codeTemplate().push_back(codeTemplate);
        }
    }

    // And the image(s) too
    const contextType::iconUri_sequence& icons = context.iconUri();
    // Compute the relative start of the path
    bf::path::iterator tkit = tkdir.begin();
    bf::path::iterator pit = path.begin();
    bf::path relPath;

    if (tkdir.string() == ".")
        relPath = path;
    else {
        // Skip the leading path of tkdir in path
        for (; tkit != tkdir.end(); tkit++, pit++)
            ;
        for (; pit != path.end(); pit++)
            relPath /= *pit;
    }

    if (!icons.empty()) {
        auto_ptr<imagesType> images(new imagesType);
        contextType::iconUri_const_iterator it;
        for (it = icons.begin(); it != icons.end(); it++) {
            auto_ptr<imageType> image(new imageType);
            image->size(it->size());
            image->uriIndex(n.uriIndex(makeUri(relPath, *it)));
            images->image().push_back(image);
        }
        prim.images(images);
    }

    const parametersType& parameters = model.parameters();
    if (parameters.description().present()) {
        auto_ptr<descriptionType> desc(new descriptionType(parameters.description().get()));
        prim.parametersDescription().push_back(desc);
    }
    const parametersType::parameter_sequence& params = parameters.parameter();
    parametersType::parameter_const_iterator it;
    for (it = params.begin(); it != params.end(); it++) {
        if (isJava) {
            if (it->name() == "className" || it->name() == "classLibrary")
                continue; // skip these - they are artifacts
        }
        auto_ptr<primitiveOpParameterType> param(new primitiveOpParameterType());
        param->name(it->name());
        if (it->description().present())
            param->description(it->description().get());
        param->optional(it->optional());
        if (it->cardinality().present())
            param->cardinality().set((it->cardinality().get()));
        param->expressionMode(getExpressionMode(it->expressionMode()));
        if (it->type().present())
            param->type(it->type().get());
        if (it->portScope().present()) {
            const portScopeType::port_sequence& ports = it->portScope().get().port();
            for (portScopeType::port_const_iterator it2 = ports.begin(); it2 != ports.end();
                 it2++) {
                param->portScope().push_back(*it2);
            }
        }
        prim.parameter().push_back(param);
    }

    // Now work on the ports
    {
        const inputPortsType& inputPorts = model.inputPorts();
        const inputPortsType::inputPortSet_sequence& iPorts = inputPorts.inputPortSet();
        inputPortsType::inputPortSet_const_iterator it2;
        for (it2 = iPorts.begin(); it2 != iPorts.end(); it2++) {
            auto_ptr<primitiveOpInputPortType> i(new primitiveOpInputPortType());
            i->optional(it2->optional());
            i->minNumPorts(it2->cardinality());
            i->maxNumPorts(it2->cardinality());
            i->windowPunctInputMode(getPunctMode(it2->windowPunctuationInputMode()));
            i->windowPolicy(getWindowMode(it2->windowingMode()));
            if (it2->description().present())
                i->description(it2->description());
            prim.inputPort().push_back(i);
        }
        // Now open ports
        if (inputPorts.inputPortOpenSet().present()) {
            const inputPortOpenSetType& openInput = inputPorts.inputPortOpenSet().get();
            auto_ptr<primitiveOpInputPortType> i(new primitiveOpInputPortType());
            i->minNumPorts(0);
            i->windowPunctInputMode(getPunctMode(openInput.windowPunctuationInputMode()));
            i->windowPolicy(getWindowMode(openInput.windowingMode()));
            i->optional(true);
            if (openInput.description().present())
                i->description(openInput.description());
            prim.inputPort().push_back(i);
        }
    }
    {
        const outputPortsType& outputPorts = model.outputPorts();
        const outputPortsType::outputPortSet_sequence& iPorts = outputPorts.outputPortSet();
        outputPortsType::outputPortSet_const_iterator it2;
        for (it2 = iPorts.begin(); it2 != iPorts.end(); it2++) {
            auto_ptr<primitiveOpOutputPortType> i(new primitiveOpOutputPortType());
            i->optional(it2->optional());
            i->minNumPorts(it2->cardinality());
            i->maxNumPorts(it2->cardinality());
            i->windowPunctOutputMode(getPunctMode(it2->windowPunctuationOutputMode()));
            i->expressionMode(getExpressionMode(it2->expressionMode()));
            if (it2->description().present())
                i->description(it2->description());
            prim.outputPort().push_back(i);
        }
        // Now open ports
        if (outputPorts.outputPortOpenSet().present()) {
            const outputPortOpenSetType& openOutput = outputPorts.outputPortOpenSet().get();
            auto_ptr<primitiveOpOutputPortType> i(new primitiveOpOutputPortType());
            i->minNumPorts(0);
            i->windowPunctOutputMode(getPunctMode(openOutput.windowPunctuationOutputMode()));
            i->expressionMode(getExpressionMode(openOutput.expressionMode()));
            i->optional(true);
            if (openOutput.description().present())
                i->description(openOutput.description());
            prim.outputPort().push_back(i);
        }
    }
}

void MakeToolkit::runModelVerifier(const string& verifier, const string& file) const
{
    bf::path cmd(_config->getStringValue(CompilerConfiguration::ScriptDir));
    cmd /= (string("spl-verify-") + verifier);
    vector<string> args;
    args.push_back("-f");
    args.push_back(file);
    try {
        ostringstream ostr;
        ProcessLauncher::runProcess(cmd.string(), args, false, false, &ostr, &ostr);
        std::cerr << ostr.str();
    } catch (SPLCompilerException& e) {
        SysOut::die(SPL_CORE_VERBATIM_ERROR(e.getExplanation()));
    }
}

void MakeToolkit::PrimitiveOperators::populate(const bf::path& tkdir,
                                               toolkitNamespaceType& ns,
                                               NameSpaces& n,
                                               const MakeToolkit& mt) const
{
    OperatorInformation::const_iterator it;
    Distillery::RegEx lastComponent(".*\\.([A-Za-z0-9_]*$)");
    for (it = _ops.begin(); it != _ops.end(); it++) {
        auto_ptr<primitiveToolkitOperatorType> prim(new primitiveToolkitOperatorType());
        prim->name(it->first);
        prim->modelUriIndex(n.fileUriIndex(it->second));

        bool invalidOp = false;

        // Always load XML file to check for errors
        bf::path path = tkdir;
        path /= it->second;
        string f = path.filename().string();
        if (f.find('.') == string::npos)
            path /= f + ".xml";
        else {
            // Need only the last component
            vector<string> result;
            int cnt = lastComponent.match(f, result);
            if (cnt != 2) {
                assert(false);
            }
            path /= result[1] + ".xml";
        }
        path = Utility::cleanPath(path);
        SPLDBG("Loading operator model file" << path.string(), Debug::Core);
        ifstream istr(path.string().c_str(), fstream::in);
        if (!istr) {
            SPLDBG("Unable to open " << path.string(), Debug::Core);
            SysOut::errorln(
              SPL_CORE_CANNOT_OPEN_FILE(path.string(), RuntimeUtility::getErrorNoStr()));
            invalidOp = true;
        }

        // Load the operator model from the file
        using namespace xmlns::prod::streams::spl::operator_;

        auto_ptr<operatorModelType> model;
        if (!invalidOp) {
            string xsddir =
              CompilerConfiguration::instance().getStringValue(CompilerConfiguration::XsdDir);
            string schemaName((bf::path(xsddir) / "operatorModel.xsd").string());
            XMLDiag::XMLDiagPrinter errorPrinter(path.string());
            try {
                ::xml_schema::properties props;
                props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/operator",
                                      schemaName);
                model = operatorModel(istr, errorPrinter, 0, props);
            } catch (::xsd::cxx::exception& e) {
                SPLDBG("Unable to load Operator Model XML " << path.string() << ": " << e.what(),
                       Debug::Core);
                if (mt.verboseModelErrors()) {
                    SysOut::errorln(SPL_CORE_ERROR_LOADING_OPERATOR_MODEL_BRIEF(e.what()),
                                    SourceLocation(path.string(), 0, 0));
                    mt.runModelVerifier("operator-model", path.string());
                } else {
                    SysOut::errorln(SPL_CORE_ERROR_LOADING_OPERATOR_MODEL(e.what()),
                                    SourceLocation(path.string(), 0, 0));
                }
                invalidOp = true;
            }
        }

        // Now use ObjectModelImpl to check for errors now
        if (!invalidOp) {
            auto_ptr<Operator::OperatorModel> objModel;
            try {
                objModel.reset(
                  new Operator::OperatorModel(*model, path.parent_path().string(), true));
            } catch (SPLCompilerException& e) {
                SPLDBG("Unable to load Operator Model XML " << path.string() << ": "
                                                            << e.getExplanation(),
                       Debug::Core);
                SysOut::errorln(SPL_CORE_ERROR_CHECKING_OPERATOR_MODEL(e.getExplanation()),
                                SourceLocation(path.string(), 0, 0));
                invalidOp = true;
            }
        }

        // Now do some more
        if (invalidOp)
            prim->invalid(true);
        // try to populate as much as we can, if we got a model
        if (model.get()) {
            if (model->cppOperatorModel().present()) {
                prim->language(languageType::Cpp);
                populate(tkdir, path, *prim, model->cppOperatorModel().get(), false, n);
            } else {
                prim->language(languageType::Java);
                auto_ptr<opModelType> jmodel(
                  Operator::OperatorModel::convert(model->javaOperatorModel().get(), true));
                populate(tkdir, path, *prim, *jmodel, true, n);
            }
        }
        ns.primitiveOp().push_back(prim);
    }
}

void MakeToolkit::CompositeOperators::OperatorInformation::populate(toolkitNamespaceType& ns) const
{
    Utility::splDoc info;
    AstNode& hdr = AST::compositeDef::compositeHead(*_ast);
    AstNode& docAst = AST::compositeHead::splDoc(hdr);
    if (AST::splDoc::has_comment(docAst)) {
        AstNode& commentAst = AST::splDoc::comment(docAst);
        info.parse(astText(commentAst));
    }

    auto_ptr<compositeToolkitOperatorType> comp(new compositeToolkitOperatorType());
    if (!info.shortDescription().empty())
        comp->description(info.shortDescription());
    comp->name(_op.name());
    comp->line(_op.line());
    comp->column(_op.col());
    comp->uriIndex(_splFileIndex);
    AstNode& modsAst = AST::compositeHead::modifiers(hdr);
    for (int i = 0, n = AST::modifiers::modifierCount(modsAst); i < n; i++)
        if (AST::publicT::is(AST::modifiers::modifier(modsAst, i))) {
            comp->public_(true);
            break;
        }

    // Any non-default params?
    bool hasNonDefaultParams = false;
    AstNode& body = AST::compositeDef::compositeBody(*_ast);
    if (AST::compositeBody::has_formal(body)) {
        AstNode& formalsAst = AST::compositeBody::formal(body);
        for (int i = 0, n = AST::formalClause::compositeFormalCount(formalsAst); i < n; i++) {
            AstNode& formalAst = AST::formalClause::compositeFormal(formalsAst, i);
            AstNode& formalInitAst = AST::compositeFormal::compositeFormalInit(formalAst);
            if (!AST::compositeFormalInit::has_opActual(formalInitAst)) {
                hasNonDefaultParams = true;
                break;
            }
        }
    }

    comp->potentialMain(!hasNonDefaultParams &&
                        AST::noneT::is(AST::compositeHead::compositeIn(hdr)) &&
                        AST::noneT::is(AST::compositeHead::compositeOut(hdr)));

    // Ports
    AstNode& inputPorts = AST::compositeHead::compositeIn(hdr);
    const string* desc;
    if (!AST::noneT::is(inputPorts)) {
        for (uint32_t i = 0, n = AST::compositeIn::compositePortCount(inputPorts); i < n; i++) {
            AstNode& port = AST::compositeIn::compositePort(inputPorts, i);
            auto_ptr<compositeOpPortType> p(new compositeOpPortType());
            p->portIndex(i);
            p->name(astText(AST::compositePort::id(port)));
            desc = info.find("input " + p->name());
            if (desc)
                p->description(*desc);
            if (AST::compositePort::has_type(port)) {
                ostringstream os;
                prettyPrint(os, AST::compositePort::type(port));
                p->type(os.str());
            }
            comp->inputPort().push_back(p);
        }
    }
    AstNode& outputPorts = AST::compositeHead::compositeOut(hdr);
    if (!AST::noneT::is(outputPorts)) {
        for (uint32_t i = 0, n = AST::compositeOut::compositePortCount(outputPorts); i < n; i++) {
            AstNode& port = AST::compositeOut::compositePort(outputPorts, i);
            auto_ptr<compositeOpPortType> p(new compositeOpPortType());
            p->portIndex(i);
            p->name(astText(AST::compositePort::id(port)));
            desc = info.find("output " + p->name());
            if (desc)
                p->description(*desc);
            if (AST::compositePort::has_type(port)) {
                ostringstream os;
                prettyPrint(os, AST::compositePort::type(port));
                p->type(os.str());
            }
            comp->outputPort().push_back(p);
        }
    }
    // and parameters
    if (AST::compositeBody::has_formal(body)) {
        AstNode& formals = AST::compositeBody::formal(body);
        for (uint32_t i = 0, n = AST::formalClause::compositeFormalCount(formals); i < n; i++) {
            AstNode& f = AST::formalClause::compositeFormal(formals, i);
            auto_ptr<compositeOpParameterType> p(new compositeOpParameterType());
            // skip '$'
            p->name(astText(AST::compositeFormal::id(f)).substr(1));
            desc = info.find("param " + p->name());
            if (desc)
                p->description(*desc);
            AstNode& expressionMode = AST::compositeFormal::expressionMode(f);
            AstNode& name = AST::expressionMode::name(expressionMode);
            if (AST::typeT::is(name))
                p->metaType(compositeParamEnumType::Type);
            else if (AST::attributeT::is(name))
                p->metaType(compositeParamEnumType::Attribute);
            else if (AST::functionT::is(name))
                p->metaType(compositeParamEnumType::Function);
            else if (AST::expressionT::is(name))
                p->metaType(compositeParamEnumType::Expression);
            else {
                assert(AST::operatorT::is(name));
                p->metaType(compositeParamEnumType::Operator);
            }
            if (AST::expressionMode::has_typeArgs(expressionMode)) {
                ostringstream os;
                prettyPrint(os, AST::expressionMode::typeArgs(expressionMode));
                p->type(os.str());
            }
            AstNode& init = AST::compositeFormal::compositeFormalInit(f);
            if (AST::compositeFormalInit::has_opActual(init)) {
                p->optional(true);
                ostringstream os;
                prettyPrint(os, AST::compositeFormalInit::opActual(init));
                // remove trailing ';'
                string s = os.str();
                s.erase(s.size() - 1);
                p->defaultValue(s);
            } else
                p->optional(false);
            comp->parameter().push_back(p);
        }
    }

    // And types
    _types.populate(*comp);

    // Finally, the enums
    _enums.populate(*comp);

    ns.compositeOp().push_back(comp);
}

void MakeToolkit::CompositeOperators::populate(const bf::path& tkdir,
                                               toolkitNamespaceType& ns) const
{
    for (vector<OperatorInformation>::const_iterator it = _ops.begin(); it != _ops.end(); it++)
        it->populate(ns);
}

void MakeToolkit::NativeFunctions::populate(const bf::path& tkdir,
                                            toolkitNamespaceType& ns,
                                            NameSpaces& n,
                                            const MakeToolkit& mt) const
{
    const string& nsName = ns.name();
    for (uint32_t i = 0; i < _nativeFunctions.size(); i++) {
        // For each native function file, open it up, and extract the native functions from it
        // Put those into the toolkit XML
        string fileName = Utility::cleanPath(tkdir / _nativeFunctions[i]).string();
        ifstream istr(fileName.c_str(), fstream::in);
        if (!istr) {
            SysOut::errorln(SPL_CORE_XML_MISSING(fileName, tkdir.string()));
            continue;
        }
        int fileIndex = n.fileUriIndex(_nativeFunctions[i]);
        using namespace Function;
        using namespace Common;
        auto_ptr<functionModelType> rawModel;
        string xsddir =
          CompilerConfiguration::instance().getStringValue(CompilerConfiguration::XsdDir);
        string schemaName((bf::path(xsddir) / "functionModel.xsd").string());
        XMLDiag::XMLDiagPrinter errorPrinter(fileName);
        try {
            ::xml_schema::properties props;
            props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/function", schemaName);
            rawModel = functionModel(istr, errorPrinter, 0, props);
        } catch (::xsd::cxx::exception& e) {
            SPLDBG("Unable to load Function Model XML " << fileName << ": " << e.what(),
                   Debug::Core);
            if (mt.verboseModelErrors()) {
                SysOut::errorln(SPL_CORE_UNABLE_TO_LOAD_FUNCTION_MODEL_BRIEF(e.what()),
                                SourceLocation(fileName, 0, 0));
                mt.runModelVerifier("function-model", fileName);
            } else {
                SysOut::errorln(SPL_CORE_UNABLE_TO_LOAD_FUNCTION_MODEL(e.what()),
                                SourceLocation(fileName, 0, 0));
            }
            continue;
        }
        auto_ptr<FunctionModel> model;
        try {
            model.reset(new FunctionModel(*rawModel, bf::path(fileName).parent_path().string()));
        } catch (SPLCompilerException& e) {
            SPLDBG("Unable to load Function Model XML " << fileName << ": " << e.getExplanation(),
                   Debug::Core);
            SysOut::errorln(SPL_CORE_UNABLE_TO_LOAD_XML(fileName, schemaName, e.getExplanation()));
            continue;
        }

        // We now have the function model.  Load the information found there
        const vector<FunctionSetPtr>& funcSets = model->getFunctionSets();
        uint32_t numSets = funcSets.size();
        bf::path fName(fileName);
        for (uint32_t j = 0; j < numSets; j++) {
            FunctionSet& fs = *funcSets[j];
            // We have native functions
            std::vector<Function::CppNameLocation>& functions = fs.getFunctions();
            for (uint32_t k = 0, nk = functions.size(); k < nk; k++) {
                Function::CppNameLocation& nl = functions[k];
                NameLocation nameLoc(nl.name(), nl.line(), nl.col());
                if (!n.checkNameInNamespace(nsName, fileName, nameLoc))
                    continue;
                auto_ptr<toolkitFunctionType> fcn(new toolkitFunctionType());
                fcn->uriIndex(0);
                if (nl.line())
                    fcn->line(nl.line());
                if (nl.col())
                    fcn->column(nl.col());
                fcn->modelUriIndex(fileIndex);
                fcn->native(true);
                AstNode& fcnAst = getNativeFunctionInformation(*fcn, nl.name(), fName, true);
                if (!AST::functionPrototype::is(fcnAst))
                    continue;
                AstNode& modsAst = AST::functionPrototype::modifiers(fcnAst);
                for (int i1 = 0, n1 = AST::modifiers::modifierCount(modsAst); i1 < n1; i1++)
                    if (AST::publicT::is(AST::modifiers::modifier(modsAst, i1))) {
                        fcn->public_(true);
                        break;
                    }
                fcn->prototype(nl.name());
                if (nl.hasDescription())
                    fcn->description(nl.getDescription());
                ns.function().push_back(fcn);
            }
        }
    }
}

void MakeToolkit::SPLFunctions::populate(const bf::path& rootDir, toolkitNamespaceType& ns) const
{
    for (vector<FunctionInformation>::const_iterator it = _functions.begin();
         it != _functions.end(); it++)
        it->populate(rootDir, ns);
}

void MakeToolkit::SPLFunctions::FunctionInformation::populate(const bf::path& tkdir,
                                                              toolkitNamespaceType& ns) const
{
    Utility::splDoc info;
    AstNode& docAst = AST::functionHead::splDoc(*_ast);
    if (AST::splDoc::has_comment(docAst)) {
        AstNode& commentAst = AST::splDoc::comment(docAst);
        info.parse(astText(commentAst));
    }
    auto_ptr<toolkitFunctionType> fcn(new toolkitFunctionType());
    if (!info.shortDescription().empty())
        fcn->description(info.shortDescription());
    fcn->line(_fcn.line());
    fcn->column(_fcn.col());
    fcn->uriIndex(_splFileIndex);
    AstNode& modifiersAst = AST::functionHead::modifiers(*_ast);
    for (int i = 0, n = AST::modifiers::modifierCount(modifiersAst); i < n; i++) {
        AstNode& mod = AST::modifiers::modifier(modifiersAst, i);
        if (AST::statefulT::is(mod))
            fcn->stateful(true);
        if (AST::publicT::is(mod))
            fcn->public_(true);
    }
    populateBaseFunctionType(*fcn, *_ast);
    fcn->native(false);
    {
        ostringstream os;
        prettyPrint(os, *_ast);
        fcn->prototype(os.str());
    }

    // Fill in rest of information

    ns.function().push_back(fcn);
}

void MakeToolkit::SPLTypes::populate(toolkitNamespaceType& ns) const
{
    toolkitNamespaceType::type_sequence& t = ns.type();
    for (vector<TypeInformation>::const_iterator it = _types.begin(); it != _types.end(); it++) {
        auto_ptr<toolkitTypeType> type = it->populate();
        // Types in a namespace are static
        type->static_(true);
        t.push_back(type);
    }
}

auto_ptr<toolkitTypeType> MakeToolkit::SPLTypes::TypeInformation::populate() const
{
    Utility::splDoc info;
    AstNode& docAst = AST::typeDef::splDoc(*_ast);
    if (AST::splDoc::has_comment(docAst)) {
        AstNode& commentAst = AST::splDoc::comment(docAst);
        info.parse(astText(commentAst));
    }
    auto_ptr<toolkitTypeType> type(new toolkitTypeType());
    if (!info.shortDescription().empty())
        type->description(info.shortDescription());
    type->name(_type.name());
    type->line(_type.line());
    type->column(_type.col());
    type->uriIndex(_splFileIndex);

    // Fill in rest of information
    ostringstream os;
    prettyPrint(os, AST::typeDef::typeDefTail(*_ast));
    type->type(os.str());

    return type;
}

void MakeToolkit::SPLTypes::populate(compositeToolkitOperatorType& c) const
{
    compositeToolkitOperatorType::type_sequence& t = c.type();
    for (vector<TypeInformation>::const_iterator it = _types.begin(); it != _types.end(); it++) {
        auto_ptr<toolkitTypeType> type = it->populate();
        // Types in a composite might be static
        AstNode& modsAst = AST::typeDef::modifiers(it->ast());
        for (int i = 0, n = AST::modifiers::modifierCount(modsAst); i < n; i++)
            if (AST::staticT::is(AST::modifiers::modifier(modsAst, i))) {
                type->static_(true);
                break;
            }
        t.push_back(type);
    }
}

void MakeToolkit::SPLEnumLiterals::populate(toolkitNamespaceType& ns) const
{
    toolkitNamespaceType::enumLiterals_sequence& e = ns.enumLiterals();
    for (vector<EnumInformation>::const_iterator it = _enumLits.begin(); it != _enumLits.end();
         it++)
        e.push_back(it->populate());
}

auto_ptr<enumLiteralsType> MakeToolkit::SPLEnumLiterals::EnumInformation::populate() const
{
    auto_ptr<enumLiteralsType> e(new enumLiteralsType());
    e->line(_enumLit.line());
    e->column(_enumLit.col());
    e->uriIndex(_splFileIndex);

    for (int i = 0, n = AST::enumType::idCount(*_ast); i < n; i++) {
        AstNode& idAst = AST::enumType::id(*_ast, i);
        e->enum_().push_back(astText(idAst));
    }
    if (_typeDef) {
        // grab name and scope
        e->typeName(astText(AST::typeDef::id(*_typeDef)));
        AstNode& modsAst = AST::typeDef::modifiers(*_typeDef);
        for (int i = 0, n = AST::modifiers::modifierCount(modsAst); i < n; i++) {
            if (AST::staticT::is(AST::modifiers::modifier(modsAst, i))) {
                e->static_(true);
                break;
            }
        }
    } else
        e->static_(true); // assume static

    return e;
}

void MakeToolkit::SPLEnumLiterals::populate(compositeToolkitOperatorType& c) const
{
    compositeToolkitOperatorType::enumLiterals_sequence& e = c.enumLiterals();
    for (vector<EnumInformation>::const_iterator it = _enumLits.begin(); it != _enumLits.end();
         it++)
        e.push_back(it->populate());
}

void MakeToolkit::NameSpaces::setNamespace(const string& ns, const string& raw)
{
    _currentNamespace = ns;
    _rawNamespace = raw;
}

void MakeToolkit::NameSpaces::setNamespace(const string& ns, const SourceLocation& loc)
{
    // Check that the namespace in the file matches that of the pathname to the file
    if (ns != _currentNamespace) {
        SysOut::errorln(SPL_CORE_TOOLKIT_INVALID_OP_NAMESPACE(ns, _file.string(), _currentNamespace,
                                                              _rawNamespace),
                        loc);
    }
    _currentNamespace = ns;
}

void MakeToolkit::NameSpaces::setFile(const bf::path& file, const string& fileName)
{
    _file = file;
    _currentSPLFile = fileName;
    _currentSPLFileIndex = fileUriIndex(fileName);
}

void MakeToolkit::NameSpaces::addNativeFile(const string& ns, const string& functionModel)
{
    _map[ns]._nativeFunctions.add(functionModel);
}

void MakeToolkit::NameSpaces::addNamespace(const string& ns)
{
    // Will add an empty namespace if not already present
    _map[ns];
}

void MakeToolkit::NameSpaces::addPrimitiveOperator(const string& ns,
                                                   const string& opName,
                                                   const string& fileName)
{
    NameLocation nl(opName, 0, 0);
    if (checkNameInNamespace(ns, fileName, nl))
        _map[ns]._primitives.add(opName, fileName);
}

void MakeToolkit::NameSpaces::addFunction(AstNode& hdr)
{
    ostringstream os;
    prettyPrint(os, hdr); // generate the prototype
    AstNode* first = astNodeWithFirstLocation(hdr);
    assert(NULL != first);

    NameLocation nl(os.str(), astLine(*first), astColumn(*first));
    if (checkNameInNamespace(_currentNamespace, _currentSPLFile, nl))
        _map[_currentNamespace]._splFunctions.add(nl, _currentSPLFileIndex, hdr);
}

void MakeToolkit::NameSpaces::addComposite(const string& c, AstNode& def)
{
    // We have just seen a composite
    AstNode& header = AST::compositeDef::compositeHead(def);
    AstNode& id = AST::compositeHead::id(header);
    NameLocation nl(c, astLine(id), astColumn(id));
    if (checkNameInNamespace(_currentNamespace, _currentSPLFile, nl))
        _map[_currentNamespace]._composites.add(nl, _currentSPLFileIndex, def);
    _inComposite = true;
    _currentCompositeNames.clear();
}

void MakeToolkit::NameSpaces::finishComposite(const string& c, AstNode& def)
{
    _inComposite = false;
}

void MakeToolkit::NameSpaces::addType(const string& typeName, AstNode& ast)
{
    // We have just seen a type
    AstNode& id = AST::typeDef::id(ast);
    NameLocation nl(typeName, astLine(id), astColumn(id));
    NSInfo& ns = _map[_currentNamespace];
    SPLTypes& t = ns._splTypes;
    CompositeOperators& c = ns._composites;
    if (_inComposite) {
        if (checkName(_currentNamespace, _currentSPLFile, _currentCompositeNames, nl))
            c.addType(nl, _currentSPLFileIndex, ast);
    } else if (checkNameInNamespace(_currentNamespace, _currentSPLFile, nl))
        t.add(nl, _currentSPLFileIndex, ast);
    _currentTypedef = &ast;
}

void MakeToolkit::NameSpaces::finishType(const string& typeName, AstNode& ast)
{
    _currentTypedef = NULL;
}

void MakeToolkit::NameSpaces::addEnum(AstNode& ast)
{
    // We have just seen an enum
    bool success = true;
    NSInfo& ns = _map[_currentNamespace];
    SPLEnumLiterals& e = ns._splEnumLits;
    CompositeOperators& c = ns._composites;
    for (int i = 0, n = AST::enumType::idCount(ast); i < n; i++) {
        AstNode& idAst = AST::enumType::id(ast, i);
        NameLocation nl(astText(idAst), astLine(idAst), astColumn(idAst));
        if (_inComposite) {
            // Don't check names without a typedef (might be legally duplicated)
            if (_currentTypedef)
                success &=
                  checkName(_currentNamespace, _currentSPLFile, _currentCompositeNames, nl);
        } else
            success &= checkNameInNamespace(_currentNamespace, _currentSPLFile, nl);
    }
    if (success && AST::enumType::idCount(ast) > 0) {
        AstNode& idAst = AST::enumType::id(ast, 0);
        NameLocation nl(astText(idAst), astLine(idAst), astColumn(idAst));
        if (_inComposite)
            c.addEnum(nl, _currentSPLFileIndex, ast, _currentTypedef);
        else
            e.add(nl, _currentSPLFileIndex, ast, _currentTypedef);
    }
}

bool MakeToolkit::NameSpaces::nothingToDo() const
{
    return _map.empty() && !sawInfo;
}

void MakeToolkit::locateMixedModeFiles(const bf::path& tkdir)
{
    if (_config->isSet(CompilerConfiguration::NoMixedModePreprocessing))
        return;
    walkDirectoryTree(tkdir, tkdir, &MakeToolkit::recordMixedModeFile, "", false);
}

#define SPLMM_EXT ".splmm"
void MakeToolkit::recordMixedModeFile(const bf::path& rootDir,
                                      const bf::path& file,
                                      const string& insideReservedDirectory,
                                      const bool insidePrimitiveOperatorDirectory)
{
    string ext = bf::extension(file);
    if (ext != SPLMM_EXT)
        return;

    // Are we in an ignored directory tree?
    if (!insideReservedDirectory.empty()) {
        if (isNonSplReservedDirectory(insideReservedDirectory))
            SysOut::warnln(SPL_CORE_MAKETOOLKIT_IGNORING(file.string()));
        return;
    }

    if (insidePrimitiveOperatorDirectory &&
        !_config->getBooleanValue(CompilerConfiguration::Clean)) {
        SysOut::warnln(SPL_CORE_MAKETOOLKIT_IGNORING_PRIM(file.string()));
        return;
    }

    if (_config->isSet(CompilerConfiguration::MixedModeFile))
        _config->getStringVectorValue(CompilerConfiguration::MixedModeFile)
          .push_back(file.string());
    else
        _config->setStringVectorValue(CompilerConfiguration::MixedModeFile,
                                      vector<string>(1, file.string()));
}

void MakeToolkit::preprocessMixedModeFiles(const bf::path& tkdir)
{
    if (!_config->isSet(CompilerConfiguration::MixedModeFile) ||
        _config->isSet(CompilerConfiguration::NoMixedModePreprocessing))
        return;
    vector<string> const& filenmVector =
      _config->getStringVectorValue(CompilerConfiguration::MixedModeFile);
    vector<string>::const_iterator fit;
    for (fit = filenmVector.begin(); fit != filenmVector.end(); ++fit) {
        string const& filenm = *fit;

        bf::path scriptDir(_config->getStringValue(CompilerConfiguration::InternalScriptDir));
        bf::path mmpp = scriptDir / "spl-mixedmode-processor";
        vector<string> args;
        args.push_back("-r");
        args.push_back(tkdir.string());
        args.push_back("-o");
        args.push_back(tkdir.string());
        args.push_back("-t");
        args.push_back("Script");
        args.push_back(filenm);
        string plfilenm = filenm + ".pl";
        try {
            if (0 != ProcessLauncher::runProcess(mmpp.string(), args)) {
                if (_forceGeneration) {
                    SysOut::errorln(SPL_CORE_FAILED_TO_PREPROCESS_FILE(filenm));
                    continue;
                }
                SysOut::die(SPL_CORE_FAILED_TO_PREPROCESS_FILE(filenm));
            }
            ostringstream ostr;
            bf::path plexec(plfilenm);
            plexec = bf::system_complete(plexec);
            bf::path mmpDriver = scriptDir / "spl-mmp-driver";
            vector<string> mmpArgs;
            mmpArgs.push_back("-f");
            mmpArgs.push_back(plexec.string());
            vector<string>::const_iterator it;
            for (it = _preprocessorArgs.begin(); it != _preprocessorArgs.end(); ++it) {
                mmpArgs.push_back(*it);
            }
            if (0 !=
                ProcessLauncher::runProcess(mmpDriver.string(), mmpArgs, false, false, &ostr)) {
                try {
                    bf::remove(plfilenm);
                } catch (...) {
                }
                if (_forceGeneration) {
                    SysOut::errorln(SPL_CORE_FAILED_TO_PREPROCESS_FILE(filenm));
                    continue;
                }
                SysOut::die(SPL_CORE_FAILED_TO_PREPROCESS_FILE(filenm));
            }
            string pfilenm = bf::change_extension(bf::path(filenm), SPL_EXT).string();
            bf::path pfilept(pfilenm);
            mode_t mask = ::umask(0);
            ::umask(mask);
            mode_t perm = 0666 & ~mask;
            if (bf::exists(pfilept)) {
                if (::chmod(pfilenm.c_str(), perm) < 0) {
                    // We probably don't own the file.   Let us just try removing it instead
                    Utility::removeFile(pfilept, false);
                }
            }
            ofstream of(pfilenm.c_str());
            if (!of) {
                Utility::removeFile(plfilenm, false);
                if (_forceGeneration) {
                    SysOut::errorln(
                      SPL_CORE_CANNOT_CREATE_FILE(pfilenm, RuntimeUtility::getErrorNoStr()));
                    continue;
                }
                SysOut::die(SPL_CORE_CANNOT_CREATE_FILE(pfilenm, RuntimeUtility::getErrorNoStr()));
            }
            of << ostr.str();
            of.close();
            perm &= ~S_IWUSR & ~S_IWGRP & ~S_IWOTH;
            ::chmod(pfilenm.c_str(), perm);
        } catch (SPLCompilerException& e) {
            try {
                bf::remove(plfilenm);
            } catch (...) {
            }
            SysOut::errorln(SPL_CORE_FAILED_TO_PREPROCESS_FILE(filenm));
            Utility::removeFile(plfilenm, false);
            if (_forceGeneration) {
                SysOut::errorln(SPL_CORE_VERBATIM_ERROR(e.getExplanation()));
                continue;
            }
            SysOut::die(SPL_CORE_VERBATIM_ERROR(e.getExplanation()));
        }
        Utility::removeFile(plfilenm, false);
    } // for
}

void MakeToolkit::cleanupNLArtifacts(const bf::path& tkdir)
{
    bf::path nlDir = tkdir / "impl" / "nl";
    bf::path includeDir = nlDir / "include";
    if (bf::exists(includeDir)) {
        Utility::removeDirectory(includeDir, true, false);
    }
    bf::path tempDir = nlDir / "temp";
    if (bf::exists(tempDir)) {
        Utility::removeDirectory(tempDir, true, true);
    }
    if (bf::exists(nlDir)) {
        bf::directory_iterator it(nlDir);
        bf::directory_iterator itEnd;
        vector<bf::path> filesToRemove;
        for (; it != itEnd; ++it) {
            const bf::path& p = (*it).path();
            if (p.extension() == ".dat") {
                filesToRemove.push_back(p);
            }
        }
        for (vector<bf::path>::size_type i = 0, sz = filesToRemove.size(); i < sz; ++i) {
            Utility::removeFile(filesToRemove[i], true);
        }
    }
}

void MakeToolkit::cleanupPreprocessedMixedModeFiles()
{
    if (!_config->isSet(CompilerConfiguration::MixedModeFile) ||
        _config->isSet(CompilerConfiguration::NoMixedModePreprocessing))
        return;

    vector<string> const& filenmVector =
      _config->getStringVectorValue(CompilerConfiguration::MixedModeFile);
    vector<string>::const_iterator fit;
    for (fit = filenmVector.begin(); fit != filenmVector.end(); ++fit) {
        string const& mmfile = *fit;
        bf::path filept = bf::change_extension(bf::path(mmfile), SPL_EXT);
        Utility::removeFile(filept, true);
        // in case the generator is left over, clean it up
        bf::path genpt = mmfile + ".pl";
        Utility::removeFile(genpt, false);
    }
}

void MakeToolkit::addSourceCodeModel(compilationUnitType& m, const string& fileName)
{
    // Walk past the rootDir
    bf::path file(fileName);
    bf::path r;

    // Skip the initial part of the path
    bf::path::const_iterator it = file.begin();
    for (uint32_t i = 0; i < _rootDirCount; i++) {
        ++it;
    }

    // Copy the rest of the path
    while (it != file.end()) {
        r /= *it++;
    }

    sourceFileType* sf = new sourceFileType(m, r.string());
    _sourceCodeModels.push_back(sf);
}
