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

#include <SPL/Core/CompilerApp.h>

#include <SPL/CodeGen/ADLGenerator.h>
#include <SPL/CodeGen/CodeGenHelper.h>
#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/FunctionGenerator.h>
#include <SPL/CodeGen/MakefileGenerator.h>
#include <SPL/CodeGen/OperatorGenerator.h>
#include <SPL/CodeGen/StandaloneGenerator.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/BaseMangler.h>
#include <SPL/Core/CatchAnnotationChecker.h>
#include <SPL/Core/CompilationState.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/ConsistentCut.h>
#include <SPL/Core/EventTimeRegionBuilder.h>
#include <SPL/Core/FeedbackLoopDetector.h>
#include <SPL/Core/MixedModeLineMapper.h>
#include <SPL/Core/MultipleConnectionChecker.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/ParallelRegionChecker.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/PoolPopulator.h>
#include <SPL/Core/PunctuationSemanticChecker.h>
#include <SPL/Core/StreamsUseDef.h>
#include <SPL/Core/TransportStats.h>
#include <SPL/Core/ViewAnnotationChecker.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/UsedCompositeDefinitions.h>
#include <SPL/FrontEnd/UsedOperatorInvocations.h>
#include <SPL/FrontEnd/UsedSPLFunctions.h>
#include <SPL/Optimizer/ConstraintChecker.h>
#include <SPL/Runtime/Function/BuiltinSPLFunctions.h>
#include <SPL/Runtime/Utility/MessageFormatter.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/OptimizeControl.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <SPL/Utility/Visitors.h>

#include <UTILS/RegEx.h>
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

namespace bf = boost::filesystem;

#define DPS_EXT ".dps"
#define DMM_EXT ".dmm"
#define SPL_EXT ".spl"

#ifdef BOOST_WINDOWS_API
#define SPLPATH_SEP ";"
#else
#define SPLPATH_SEP ":"
#endif

#define PHS_INFO Debug::instance().indebugCheck(Debug::PhsInfo)

#include <SPL/Runtime/Utility/Singleton.t>
template class Singleton<CoreMessageFormatter>;
template class Singleton<CompilerApp>;
template class Singleton<ModelDumper>;
template class Singleton<CompilationState>;
template class Singleton<ConsistentCutRegionBuilder>;

// An instance of the message formatter for
// core compiler messages
CoreMessageFormatter coreFormatter;

CompilerApp::CompilerApp()
  : Singleton<CompilerApp>(*this)
  , _interruptable(true)
  , _optionError(false)
  , _optimizedSelected(false)
  , _noOptimizedSelected(false)
{
    _config.reset(new CompilerConfiguration());

    // Construct these after the options have been processed
    if (_config->isSet(CompilerConfiguration::ModelFile)) {
        _modelDumper.reset(
          new ModelDumper(_config->getStringValue(CompilerConfiguration::ModelFile)));
    }
    // Note that the following initializations need to be done here because of SDB
    _pathSearch.reset(new PathSearch());
    _frontEnd.reset(new FrontEndDriver());

    _defaultTransport = Config::TCP;

    // Force an override
    char* overrideDefaultTransportation = getenv("STREAMS_USE_TRANSPORT");
    if (overrideDefaultTransportation)
        setTransport(NULL, overrideDefaultTransportation);
}

CompilerApp::~CompilerApp() {}

void CompilerApp::optionError(const FormattableMessage& msg)
{
    SysOut::errorln(msg);
    _optionError = true;
}

void CompilerApp::mySigIntHandler(int /*signum*/)
{
    if (!_interruptable)
        return;
    ostringstream ostr;
    ostr << "\n" << SPL_CORE_INTERRUPTED << "\n";
    async_safe_printf(1, ostr.str().c_str());
    exit(EXIT_FAILURE);
}

void CompilerApp::iceHandler(int sig)
{

    // We want better behaviour.   Print an ICE message
    SysOut::errorln(SPL_CORE_INTERNAL_COMPILER_ERROR(sig));
    exit(EXIT_FAILURE);
}

void CompilerApp::installFaultSignalHandlers()
{
    if (indebug(Debug::Traceback)) {
        // Use default behaviour
        DistilleryApplication::installFaultSignalHandlers();
        return;
    }

    // Install our default handler
    installSignalHandler(SIGABRT, mem_cb(this, &CompilerApp::iceHandler));
    installSignalHandler(SIGSEGV, mem_cb(this, &CompilerApp::iceHandler));
    installSignalHandler(SIGPIPE, mem_cb(this, &CompilerApp::iceHandler));
    installSignalHandler(SIGILL, mem_cb(this, &CompilerApp::iceHandler));
    installSignalHandler(SIGFPE, mem_cb(this, &CompilerApp::iceHandler));
    installSignalHandler(SIGBUS, mem_cb(this, &CompilerApp::iceHandler));
}

int CompilerApp::init()
{
    // run the base class init first....
    int r = DistilleryApplication::init();

    DistilleryApplication::getThisApp().removeSignalHandler(SIGINT);
    DistilleryApplication::getThisApp().installSignalHandler(
      SIGINT, mem_cb(this, &CompilerApp::mySigIntHandler));

    // Compute paths.
    computeCompilerPaths();

    // If JAVA_HOME is set, use its value, otherwise set JAVA_HOME to the internal java
    // If the value specified by JAVA_HOME is the install version, treat it as if it
    // wasn't specified.  This is done so that apps compiled against the install java
    // will run regardless of where the install lives.
    string instDir = _config->getStringValue(CompilerConfiguration::InstallDir);
    bf::path instJava(instDir);
    instJava /= "java";
    const char* javaHome = getenv("JAVA_HOME");
    if (NULL != javaHome) {
        _config->setStringValue(CompilerConfiguration::JavaHome, javaHome);
        _config->setBooleanValue(CompilerConfiguration::JavaHomeIsInstall, false);

        bf::path specifiedJava(javaHome);
        try {
            if (bf::equivalent(instJava, specifiedJava)) {
                _config->setBooleanValue(CompilerConfiguration::JavaHomeIsInstall, true);
            }
        } catch (...) {
            // If one doesn't exist then we don't want to die.
        }
    } else {
        setenv("JAVA_HOME", instJava.string().c_str(), 0);
        _config->setStringValue(CompilerConfiguration::JavaHome, instJava.string());
        _config->setBooleanValue(CompilerConfiguration::JavaHomeIsInstall, true);
    }

    return r;
}

bool CompilerApp::locateInputFile()
{
    // If we are running in a mode that requires an explicit input file, no need
    // to locate the input file, since the explicit one will be used
    if (_config->isSet(CompilerConfiguration::ExplicitInputFile))
        return true;

    // Find the file that contains the main composite
    string const& comp = _config->getStringValue(CompilerConfiguration::MainComposite);
    string opname, nmspace;
    size_t pos = comp.rfind("::");
    if (pos != string::npos) {
        nmspace = comp.substr(0, pos);
        opname = comp.substr(pos + 2);
    } else {
        opname = comp;
    }
    if (!_pathSearch->resolveToolkits(nmspace, opname))
        return false;

    const PathSearch::NonFunctionResult* res = _pathSearch->findNonFunction(nmspace, opname);
    if (res == NULL || res->isPrimitiveOperator())
        SysOut::die(SPL_CORE_CANNOT_LOCATE_MAIN_COMPOSITE_NO_SUCH_COMPOSITE(comp));

    // Remember the toolkit index for later
    FrontEndDriver::instance().setToolkitIndex(res->getToolkitIndex());
    _config->setStringValue(CompilerConfiguration::InputFile, res->getPath());
    return true;
}

int CompilerApp::run(const vector<string>& remains)
{
    _transportStats.reset(new TransportStats);
    _streamsUseDef.reset(new StreamsUseDef);

    _preprocessorArgs = remains;
    _config->setBooleanValue(CompilerConfiguration::GenerateCode, true);

    // Emit an error if the user asked for translation
    if (_config->getBooleanValue(CompilerConfiguration::Translate)) {
        optionError(SPL_CORE_TRANSLATION_UNSUPPORTED);
        return EXIT_FAILURE;
    }

    // Set application directories
    setApplicationDirectories();

    // Network byte ordering implies no facades
#if __BYTE_ORDER == __LITTLE_ENDIAN
    if (_config->getBooleanValue(CompilerConfiguration::NetworkByteOrderEncoding))
        _config->setBooleanValue(CompilerConfiguration::PreferFacades, false);
#endif

    bool usageError = false;
    if ( // mode where -f is requred
      _config->getBooleanValue(CompilerConfiguration::PrintAST) ||
      _config->getBooleanValue(CompilerConfiguration::PrintSource) ||
      _config->getBooleanValue(CompilerConfiguration::PrintExpansion) ||
      _config->getBooleanValue(CompilerConfiguration::PrintTokens) ||
      _config->getBooleanValue(CompilerConfiguration::SyntaxCheckOnly) ||
      _config->getBooleanValue(CompilerConfiguration::Translate)) {
        _config->setBooleanValue(CompilerConfiguration::NoToolkitIndexing, true);
        if (!_config->isSet(CompilerConfiguration::MainComposite))
            _config->setStringValue(CompilerConfiguration::MainComposite, "Main");
        if (!_config->isSet(CompilerConfiguration::ExplicitInputFile) &&
            !_config->getBooleanValue(CompilerConfiguration::PrintExpansion)) {
            optionError(SPL_CORE_OPTION_REQUIRES_SOURCE_FILE);
            usageError = true;
        }
    } else { // regular launch where -M is mandatory and no -f is allowed
        usageError = !_config->isSet(CompilerConfiguration::MainComposite) ||
                     _config->isSet(CompilerConfiguration::ExplicitInputFile);
    }

    // Optimized code is now enabled by default (RTC 26959)
    if (_optimizedSelected && _noOptimizedSelected) {
        usageError = true;
    } else if (!_noOptimizedSelected) {
        _config->setBooleanValue(CompilerConfiguration::Optimized, true);
    }

    if (usageError)
        printUsage(cout, false);

    if (usageError || _optionError)
        return EXIT_FAILURE;

    if (PHS_INFO)
        _timer.print("Options");

    bool suppress = createMakefileIfNeeded();
    suppress = suppress | performCleanup();
    if (!suppress) {
        SPLDBG("Started compilation process...", Debug::Core);
        compile();
        if (PHS_INFO)
            _timer.total();
        SPLDBG("Ended compilation process...", Debug::Core);
    }
    return EXIT_SUCCESS;
}

void CompilerApp::compile()
{
    CompilationState state;

    // The line mapper needs to be created after splmm files have been created
    // but before the preprocessed spl file is processed.
    MixedModeLineMapper lineMapper;

    AutoErrorTrap aet(true, 50);

    // run only front end if needed
    if (_config->getBooleanValue(CompilerConfiguration::PrintAST) ||
        _config->getBooleanValue(CompilerConfiguration::PrintSource) ||
        _config->getBooleanValue(CompilerConfiguration::PrintExpansion) ||
        _config->getBooleanValue(CompilerConfiguration::PrintTokens) ||
        _config->getBooleanValue(CompilerConfiguration::SyntaxCheckOnly) ||
        _config->getBooleanValue(CompilerConfiguration::Translate)) {
        loadToolkits();
        if (PHS_INFO)
            _timer.print("Load toolkits");
        if (!locateInputFile())
            return;
        _frontEnd->run();
        if (PHS_INFO)
            _timer.print("Front End");
    }
    // run the complete compiler logic
    else {
        CodeGenHelper codeGenHelper; // TypeGenerator needs the CodeGenHelper instance to exist
        TypeGenerator typeGenerator; // CPP code generation requires TypeGenerator

        bool justADL = _config->isSet(CompilerConfiguration::JustTheAppModel);

        if (!justADL)
            createApplicationDirectories();

        loadToolkits();
        if (PHS_INFO)
            _timer.print("Load toolkits");
        if (!locateInputFile())
            return;
        _frontEnd->run();
        if (PHS_INFO)
            _timer.print("Front End");

        // Handle -g
        if (_config->getBooleanValue(CompilerConfiguration::SetWrapperSDB)) {
            const CompositeDefinition* main1 = _frontEnd->getMainComposite();
            if (main1) {
                // Does it have a config wrapper?
                if (!main1->find(Config::Wrapper)) {
                    // Add in "sdb"
                    const_cast<CompositeDefinition*>(main1)->addConfig(
                      Config::fromWrapper(SourceLocation::invalidSourceLocation, "sdb"));
                }
            }
        }

        // Figure out which types are used
        _frontEnd->collectUsedTypes();

        if (justADL)
            SPLDBG("Suppressing code generation...", Debug::Core);

        typeGenerator.buildModels();

        // **************************
        // optimizer entry point here
        // **************************
        if (!prodindebug(Debug::NoOptimizeStatement)) {
            // we do this early for now because the operator graph populate
            // generates the C++ for the SPL statements
            OperatorGenerator operatorGenerator;
            operatorGenerator.optimize(_frontEnd->getUsedOperatorInvocations());
            if (PHS_INFO)
                _timer.print("Optimizer");
        }

        OperatorGraph ograph; // singleton

        // populate the graph from the used operators table
        // The need to propagate annotations through to the OIM forced the splitting of the
        // building of the operator graph with the building of the operator instance models
        UsedOperatorInvocations& ops = _frontEnd->getUsedOperatorInvocations();
        ograph.populate(ops, _frontEnd->getMainComposite());

        if (PHS_INFO) {
            _timer.print("Operator Graph");
        }

        // Compute the consistent cut regions and validate them
        // This needs to be done before we build the OIMs as the
        // annotations synthesized as part of this processing need
        // to be propagated to the OIM
        ConsistentCutRegionBuilder ccrb(ograph);
        ccrb.build();

        if (PHS_INFO) {
            _timer.print("Consistent region builder");
        }

        {
            // Check and propagate catch annotations before building the OIM
            CatchAnnotationChecker cc;
            cc.check(ograph);
        }

        EventTimeRegionBuilder etrb(ograph);

        // Build the operator instance models
        ograph.populateOIModels(ops);

        { // Populate implicit pools
            PoolPopulator poolPopulator;
            poolPopulator.populate(ograph.getPoolCollection());
        }

        {
            // Check that no stream has multiple connections to the same input port
            MultipleConnectionChecker cc(ograph);
            cc.checkForMultipleStreamConnections();
            if (PHS_INFO) {
                _timer.print("multiple connection checker");
            }
        }
        { // Check punctuation semantics
            PunctuationSemanticChecker psc;
            psc.check(ograph);
            if (PHS_INFO) {
                _timer.print("Punctuation semantic checker");
            }
        }
        { // Check parallel regions
            ParallelRegionChecker prc;
            prc.check(ograph);
            if (PHS_INFO) {
                _timer.print("Parallel region checker");
            }
        }
        {
            ViewAnnotationChecker vc;
            vc.check(ograph);
            if (PHS_INFO) {
                _timer.print("View annotation checker");
            }
        }

        // Cannot proceed to code generation
        if (aet.getNumErrorsInCurrentTrap() != 0)
            return;

        SPLDBG(ograph, Debug::ShowOperatorGraph);
        if (ograph.numNodes() == 0) {
            const CompositeDefinition* mainComp = _frontEnd->getMainComposite();
            SysOut::warnln(SPL_CORE_EMPTY_MAIN_COMPOSITE(
                             _config->getStringValue(CompilerConfiguration::MainComposite)),
                           mainComp ? mainComp->getStartLocation()
                                    : SourceLocation::invalidSourceLocation);
        }

        {
            ConstraintChecker cola;
            cola.processConstraints(ograph);
        }

        // Cannot proceed with PE graph manipulation
        if (aet.getNumErrorsInCurrentTrap() != 0)
            return;

        SPLDBG(*_streamsUseDef, Debug::ShowStreams);

        { // Generate warnings about potential feedback loops
            FeedbackLoopDetector loopChecker(ograph);
            loopChecker.checkForLoops();
            if (PHS_INFO)
                _timer.print("Feedback loop checker");
        }

        // **************************
        // optimizer entry point here
        // **************************
        if (!prodindebug(Debug::NoOptimizeStatement)) {
            FunctionGenerator functionGenerator;
            functionGenerator.optimize();

            if (PHS_INFO)
                _timer.print("Optimizer");
        }

        // early exit for generating ADL only
        if (justADL) // Generate the ADL
        {
            ADLGenerator adl(&state);
            adl.generate();
            if (PHS_INFO)
                _timer.print("ADL generation");
            return; // early exit for generating ADL only
        }

        // *****
        // NOTE: from this point we are generating output .... not just ADL
        // *****

        // Generate code for types - from the models geneated above
        typeGenerator.generateCPPCode();
        if (PHS_INFO)
            _timer.print("Types");

        {
            // Generate code from functions
            FunctionGenerator functionGenerator;

            functionGenerator.generateCPPCode();
            if (PHS_INFO)
                _timer.print("Function code generation");
        }

        {
            // Generate code for operators
            OperatorGenerator operatorGenerator;
            operatorGenerator.generateCPPCode(ograph);
            if (PHS_INFO)
                _timer.print("Operator code generation");
        }

        {
            // Generate Standalone code
            StandaloneGenerator standaloneGenerator;
            standaloneGenerator.generateCPPCode(ograph);
            if (PHS_INFO)
                _timer.print("Standalone code generation");
        }

        // Cannot proceed with ADL generation
        if (aet.getNumErrorsInCurrentTrap() != 0)
            return;

        // Generate the ADL
        {
            ADLGenerator adl(&state);
            std::string adlName = adl.generate();
            if (PHS_INFO)
                _timer.print("ADL generation");

            // Generate a make file and compile it
            MakefileGenerator makefileGen(adlName);
            makefileGen.generate();
            if (PHS_INFO)
                _timer.print("Makefile");
            // Only run the make if there were no errors
            if (aet.getNumErrorsInCurrentTrap() == 0) {
                makefileGen.compile();
                if (PHS_INFO)
                    _timer.print("Execute Makefile");
            }
        }
    } // complete compiler logic
}

/*
 * Check the version.   Return 'true' if we need to force a clean
 */
bool CompilerApp::hasVersionChangedSinceLastCompile()
{
    bf::path outRoot = _config->getStringValue(CompilerConfiguration::OutputDir);
    if (!bf::exists(outRoot) || !bf::is_directory(outRoot))
        return false;

    // Check to see if we have source files
    if (!bf::exists(outRoot / "src"))
        return false;

    // Check that the version matches
    bf::path versionFile = outRoot / ".sc.outdir";
    if (bf::exists(versionFile)) {
        // Check that the version matches
        ifstream f(versionFile.string().c_str());
        if (!f)
            return true; // we couldn't open it - assume there is a problem
        string v;
        f >> v;
        if (v == "product_version=" STREAMS_VERSION)
            return false;
    }

    // Version mismatch - clean up first
    return true;
}

bool CompilerApp::performCleanup()
{
    bool clean = false;
    bool suppress = false;
    if (_config->isSet(CompilerConfiguration::Clean)) {
        clean = true;
    }
    if (_config->isSet(CompilerConfiguration::CleanAndSuppress)) {
        clean = suppress = true;
    }
    // Check version if we aren't already cleaning
    clean = clean || hasVersionChangedSinceLastCompile();
    if (clean)
        cleanupArtifacts();
    return suppress;
}

void CompilerApp::cleanupArtifacts()
{
    bf::path outRoot = _config->getStringValue(CompilerConfiguration::OutputDir);
    bf::path appRoot = _config->getStringValue(CompilerConfiguration::ApplicationDir);

    // remove the ADL
    string const& mainComp = _config->getStringValue(CompilerConfiguration::MainComposite);
    Utility::removeFile(outRoot / (Utility::replaceDoubleColonsWithDot(mainComp) + ".adl"), true);
    Utility::removeFile(outRoot / (Utility::replaceDoubleColonsWithDot(mainComp) + ".sab"), true);

    // TODO: should we be removing this?  This contains compile artifacts that should move to build
    Utility::removeDirectory("etc/config", true);
    Utility::removeDirectory("etc", false);

    Utility::removeDirectory(outRoot / "bin", true);
    Utility::removeDirectory(outRoot / "build", true);
    Utility::removeDirectory(outRoot / "etc", true);
    Utility::removeDirectory(outRoot / "src", true);
    Utility::removeDirectory(outRoot / "toolkits", true);
    Utility::removeDirectory(outRoot / "usr", true);
    Utility::removeFile(outRoot / ".sc.outdir", false);
    // if output root is not under application directory, do not remove it
    bf::path temp = outRoot;
    int nup = 0;
    if (Utility::makeRelativePath(temp, appRoot, nup) && nup != 0) {
        Utility::removeDirectory(outRoot, false);
        Utility::removeDirectory("output", false); // the default loc for output
    }

    // cleanup the application directory toolkit
    bf::path maketk = _config->getStringValue(CompilerConfiguration::InstallDir);
    maketk /= "bin";
    maketk /= "spl-make-toolkit";
    vector<string> args;
    if (_config->isSet(CompilerConfiguration::NoMixedModePreprocessing)) {
        args.push_back("--no-mixed-mode-preprocessing");
    }
    args.push_back("-c");
    args.push_back("-i");
    args.push_back(".");
    if (0 != ProcessLauncher::runProcess(maketk.string(), args)) {
        exit(EXIT_FAILURE); // spl-make-toolkit will print the proper message
    }
}

void CompilerApp::setApplicationDirectories()
{
    string appDir = bf::current_path().string();
    _config->setStringValue(CompilerConfiguration::ApplicationDir, appDir);
    SPLDBG("Application directory set to '" << appDir << "'", Debug::Core);
    BaseMangler::setMaxMangledNameSize(appDir);

    if (!_config->isSet(CompilerConfiguration::OutputDir)) {
        bf::path outputDir = appDir;
        outputDir /= "output";
        _config->setStringValue(CompilerConfiguration::OutputDir, outputDir.string());
    }

    if (!_config->isSet(CompilerConfiguration::CheckpointDir)) {
        bf::path dataDir;
        if (_config->isSet(CompilerConfiguration::DataDir)) {
            dataDir = _config->getStringValue(CompilerConfiguration::DataDir);
        } else {
            dataDir = appDir;
            dataDir /= "data";
        }
        bf::path ckptDir = dataDir;
        ckptDir /= "ckpt";
        _config->setStringValue(CompilerConfiguration::CheckpointDir, ckptDir.string());
    }
}

void CompilerApp::createApplicationDirectories()
{
    bf::path appRoot = _config->getStringValue(CompilerConfiguration::ApplicationDir);
    bf::path outRoot = _config->getStringValue(CompilerConfiguration::OutputDir);

    if (!bf::exists(outRoot)) { // output directory
        Utility::createDirectory(outRoot);
    }
    bf::path suppressFile = outRoot / ".sc.outdir";
    if (!bf::exists(suppressFile)) {
        // will create the file if not already present
        ofstream dummy(suppressFile.string().c_str());
        // Save the compiled version so that we know if it changes
        dummy << "product_version=" STREAMS_VERSION << endl;
    }

    Utility::createDirectory(outRoot / "src");            // generated code
    Utility::createDirectory(outRoot / "build");          // generated objects
    Utility::createDirectory(outRoot / "build" / "temp"); // working directory
    Utility::createDirectory(outRoot / "bin");            // binaries
    Utility::createDirectory(outRoot / "etc");            // Streams artifacts
    Utility::createDirectory(outRoot / "toolkits");       // Toolkit copy location for app bundles
    bf::path usrImplDir = outRoot / "usr" / "impl";
    bf::path usrTempDir = outRoot / "usr" / "tmp";
    Utility::createDirectory(usrImplDir); // User area to go into bundle
    Utility::createDirectory(usrTempDir); // User temp area
}

string CompilerApp::checkStreamsInstallation()
{
    string instDir;
    try {
        instDir = getInstallationDirectory();
    } catch (const DistilleryException& e) {
        SysOut::die(SPL_CORE_STREAMS_ERROR(e.getExplanation()));
    }
    return instDir;
}

void CompilerApp::computeCompilerPaths()
{
    // Figure out the default pathnames to be used
    bf::path installDir = checkStreamsInstallation();
    bf::path xsdDir = installDir / "etc" / "xsd" / "SPL";
    bf::path scriptDir = installDir / "bin";
    bf::path internalScriptDir = installDir / "system" / "impl" / "bin";
    bf::path statDir = installDir / "/etc" / "stat";
    bf::path toolkitDir = installDir / "toolkits" / "spl";
    bf::path templateDir = installDir / "system" / "impl" / "template";
    _config->setStringValue(CompilerConfiguration::InstallDir, installDir.string());
    _config->setStringValue(CompilerConfiguration::XsdDir, xsdDir.string());
    _config->setStringValue(CompilerConfiguration::StatsDir, statDir.string());
    _config->setStringValue(CompilerConfiguration::InternalScriptDir, internalScriptDir.string());
    _config->setStringValue(CompilerConfiguration::ScriptDir, scriptDir.string());
    _config->setStringValue(CompilerConfiguration::ToolkitDir, toolkitDir.string());
    setenv("STREAMS_TOOLKIT_DIR", toolkitDir.string().c_str(), 0);
    _config->setStringValue(CompilerConfiguration::TemplateDir, templateDir.string());
}

void CompilerApp::loadToolkits()
{
    if (!_config->isSet(CompilerConfiguration::NoToolkitIndexing)) {
        bf::path maketk = _config->getStringValue(CompilerConfiguration::InstallDir);
        maketk /= "bin";
        maketk /= "spl-make-toolkit";
        // We need to index the application directory first
        vector<string> args;
        if (_config->isSet(CompilerConfiguration::NoMixedModePreprocessing)) {
            args.push_back("--no-mixed-mode-preprocessing");
        }
        args.push_back("-i");
        args.push_back(".");
        args.insert(args.end(), _preprocessorArgs.begin(), _preprocessorArgs.end());
        if (0 != ProcessLauncher::runProcess(maketk.string(), args)) {
            exit(EXIT_FAILURE); // spl-make-toolkit will print the proper message
        }
        if (PHS_INFO) {
            _timer.print("spl-make-toolkit");
        }
    }

    // Setup all toolkit paths
    string const& toolkitDir = _config->getStringValue(CompilerConfiguration::ToolkitDir);
    vector<string> allPaths;

    if (!_config->isSet(CompilerConfiguration::ExplicitInputFile)) {
        allPaths.push_back(".");
    }
    // Do we have a user specified option?
    if (_config->isSet(CompilerConfiguration::SPLPath)) {
        string userPaths = _config->getStringValue(CompilerConfiguration::SPLPath);
        // Walk through the string, separating paths at SPLPATH_SEP
        vector<string> paths;
        Distillery::tokenize(userPaths, paths, SPLPATH_SEP, false);
        for (vector<string>::const_iterator it = paths.begin(), itEnd = paths.end(); it != itEnd;
             ++it) {
            allPaths.push_back(Utility::cleanPath(*it).string());
        }
    }
    // The default path for the spl toolkit
    allPaths.push_back(toolkitDir);
    // Grab the toolkits from the Meta-file
    readMetaFile(allPaths);
    SPLPRODDBG("Lookup paths for 'use's: ", Debug::Core);
    for (int i = 0; i < (int)allPaths.size(); i++) {
        SPLPRODDBG("  [" << i << "]\t" << allPaths[i], Debug::Core);
    }
    // Set it for path search
    _pathSearch->setPaths(allPaths);
    if (_config->isSet(CompilerConfiguration::ExplicitInputFile)) {
        _pathSearch->loadToolkits();
    } else {
        _pathSearch->locateToolkits();
    }
}

void CompilerApp::readMetaFile(vector<string>& paths)
{
    static Distillery::RegEx pcRegex("(.*)_(.*)\\.pc");

    bf::path metaFile = _config->getStringValue(CompilerConfiguration::ApplicationDir);
    metaFile /= "META-INF";
    metaFile /= "SC.MF";

    ifstream infile;

    infile.open(metaFile.string().c_str(), ifstream::in);
    if (!infile)
        return; // no file to work with

    // Read the file line by line
    string l;
    while (getline(infile, l)) {
        // Ignore lines with leading #
        if (l.empty() || l[0] == '#')
            continue;
        // look for XXX=YYY
        size_t eq = l.find('=');
        if (eq == string::npos)
            continue;
        string word = l.substr(0, eq);
        if (word == "toolkit")
            paths.push_back(l.substr(eq + 1));
        else if (word == "pcfile") {
            // figure out the name/version information and store in _provmap
            l = l.substr(eq + 1);
            vector<string> matches;
            pcRegex.match(l, matches);
            if (matches.size() == 3) {
                string name = matches[1];
                string version = matches[2];
                size_t p = name.find_last_of("/");
                if (p != string::npos)
                    name = name.substr(p + 1);
                _provMap[name + '/' + version] = l;
            }
        }
    }
}

void CompilerApp::getDistilleryApplicationVisibleOptions(option_vector_t& options)
{
    // We don't want any of the default visible options
}

void CompilerApp::getArguments(option_vector_t& options)
{
    setKeepRemainingArguments();
    option_t args[] = {
        { 'h', "help", 0, "", SPL_CORE_OPTION_HELP, STR_OPT(CompilerApp::setHelp), 0, 0 },
        { 'C', "clean-and-suppress", 0, "", SPL_CORE_OPTION_CLEAN_AND_SUPPRESS,
          STR_OPT(CompilerApp::setCleanAndSuppress), 0, 0 },
        { 'D', "debug", ARG, "", "", STR_OPT(CompilerApp::processDebugLevel), 1,
          0 }, // only used for testing
        { 'F', "use-transport", ARG, "USE_TRANSPORT", SPL_CORE_OPTION_USE_TRANSPORT,
          STR_OPT(CompilerApp::setTransport), 0, 0 },
        { 'J', "suppress-all-but-the-application-model", 0, "",
          SPL_CORE_OPTION_SUPPRESS_ALL_BUT_ADL, STR_OPT(CompilerApp::setJustTheApplicationModel), 0,
          0 },
        { 'K', "checkpoint-directory", ARG, "", SPL_CORE_OPTION_CHECKPOINT_DIRECTORY,
          STR_OPT(CompilerApp::setCheckpointDirectory), 0, 0 },
        { 'M', "main-composite", ARG, "", SPL_CORE_OPTION_MAIN_COMPOSITE,
          STR_OPT(CompilerApp::setMainComposite), 0, 0 },
        { 'O', "set-relax-fusion-relocatability-restartability", 0, "",
          SPL_CORE_OPTION_RELAX_FUSION,
          STR_OPT(CompilerApp::setRelaxFusionRelocatabilityRestartability), 0, 0 },
        { 'P', "profile-all-operators", ARG, "", SPL_CORE_OPTION_PROFILE_ALL_OPERATORS,
          INT_OPT(CompilerApp::setProfiling), 0, 0 },
        { 'S', "profiling-sampling-rate", ARG, "", SPL_CORE_OPTION_PROFILING_SAMPLING_RATE,
          STR_OPT(CompilerApp::setProfilingSampleRate), 0, 0 },
        { 'T', "standalone-application", 0, "", SPL_CORE_OPTION_STANDALONE,
          STR_OPT(CompilerApp::setStandalone), 0, 0 },
        { 'Z', "max-pe-cpu-fraction", ARG, "", SPL_CORE_OPTION_MAXIMUM_PE_CPU_FRAC,
          STR_OPT(CompilerApp::setMaximumPECpuFrac), 0, 0 },
        { 'a', "optimized-code-generation", 0, "", SPL_CORE_OPTION_OPTIMIZED_CODE_GEN,
          STR_OPT(CompilerApp::setOptimizedCodeGeneration), 0, 0 },
        { 'c', "clean", 0, "", SPL_CORE_OPTION_CLEAN, STR_OPT(CompilerApp::setClean), 0, 0 },
        { 'd', "default-pool-size", ARG, "", SPL_CORE_OPTION_DEFAULT_POOL_SIZE,
          INT_OPT(CompilerApp::setDefaultPoolSize), 0, 0 },
        { 'g', "debug", 0, "", SPL_CORE_OPTION_DEBUG, STR_OPT(CompilerApp::setWrapperSDB), 0, 0 },
        { 'j', "num-make-threads", ARG, "", SPL_CORE_OPTION_NUM_MAKE_THREADS,
          INT_OPT(CompilerApp::setMakeThreadCount), 0, 0 },
        { 'k', "prefer-facade-tuples", 0, "", SPL_CORE_OPTION_PREFER_FACADE_TUPLES,
          STR_OPT(CompilerApp::setPreferFacades), 0, 0 },
        { 'm', "makefile-generation", 0, "", SPL_CORE_OPTION_MAKEFILE_GENERATION,
          STR_OPT(CompilerApp::setMakefileGeneration), 0, 0 },
        { 'p', "part-mode", ARG, "", SPL_CORE_OPTION_PART_MODE, STR_OPT(CompilerApp::setFusionMode),
          0, 0 },
        { 'r', "num-distcc-remote-hosts", ARG, "", SPL_CORE_OPTION_NUM_DISTCC_REMOTE_HOSTS,
          INT_OPT(CompilerApp::setDistccRemoteHostCount), 0, 0 },
        { 's', "static-link", 0, "", SPL_CORE_OPTION_STATIC_LINK,
          STR_OPT(CompilerApp::setStaticLinkage), 0, 0 },
        { 't', "spl-path", ARG, "SPLPATH", SPL_CORE_OPTION_SPL_PATH(string(SPLPATH_SEP)),
          STR_OPT(CompilerApp::setSPLPath), 0, 0 },
        // we only support homogeneous clusters
        //    { 'u', "use-network-byte-order-encoding", 0, "", SPL_CORE_OPTION_USE_NETWORK_BYTE_ORDER_ENCODING, STR_OPT(CompilerApp::setNetworkByteOrderEncoding), 0, 0},
        { 'v', "verbose-mode", 0, "", SPL_CORE_OPTION_VERBOSE_MODE,
          STR_OPT(CompilerApp::setVerboseMode), 0, 0 },
        { 'w', "ld-flags", ARG, "", SPL_CORE_OPTION_LD_FLAGS, STR_OPT(CompilerApp::setLdFlags), 0,
          0 },
        { 'x', "cxx-flags", ARG, "", SPL_CORE_OPTION_CXX_FLAGS, STR_OPT(CompilerApp::setCxxFlags),
          0, 0 },
        { 'z', "code-beautification", 0, "", SPL_CORE_OPTION_CODE_BEAUTIFICATION,
          STR_OPT(CompilerApp::setCodeBeautification), 0, 0 }, // long-only options
        { 0, "allow-convenience-fusion-options", 0, "", SPL_CORE_ALLOW_CONVENIENCE_FUSION_OPTIONS,
          STR_OPT(CompilerApp::setAllowConvenienceFusionOptions), 0, 0 },
        { 0, "avoid-runtime-yield", ARG, "AVOID_RUNTIME_YIELD", SPL_CORE_AVOID_RUNTIME_YIELD,
          STR_OPT(CompilerApp::setAvoidRuntimeYield), 0, 0 },
        { 0, "c++std", ARG, "", SPL_CORE_OPTION_CXX_LANG_LEVEL,
          STR_OPT(CompilerApp::setCxxLangLevel), 0, 0 },
        { 0, "data-directory", ARG, "", SPL_CORE_OPTION_DATA_DIRECTORY,
          STR_OPT(CompilerApp::setDataDirectory), 0, 0 },
        { 0, "default-relocatable", ARG, "", SPL_CORE_OPTION_DEFAULT_RELOCATABLE,
          STR_OPT(CompilerApp::setDefaultRelocatable), 0, 0 },
        { 0, "default-restartable", ARG, "", SPL_CORE_OPTION_DEFAULT_RESTARTABLE,
          STR_OPT(CompilerApp::setDefaultRestartable), 0, 0 },
        { 0, "max-filename-length", ARG, "", SPL_CORE_OPTION_MAX_FILENAME_LENGTH,
          STR_OPT(CompilerApp::setMaxFilenameLength), 0, 0 },
        { 0, "no-mixed-mode-preprocessing", 0, "", SPL_CORE_OPTION_NO_MIXED_MODE_PREPROCESSING,
          STR_OPT(CompilerApp::setNoMixedModePreprocessing), 0, 0 },
        { 0, "no-optimized-code-generation", 0, "", SPL_CORE_OPTION_NO_OPTIMIZED_CODE_GEN,
          STR_OPT(CompilerApp::setNoOptimizedCodeGeneration), 0, 0 },
        { 0, "no-toolkit-indexing", 0, "", SPL_CORE_OPTION_NO_TOOLKIT_INDEXING,
          STR_OPT(CompilerApp::setNoToolkitIndexing), 0, 0 },
        { 0, "output-directory", ARG, "", SPL_CORE_OPTION_OUTPUT_DIRECTORY,
          STR_OPT(CompilerApp::setOutputDirectory), 0, 0 },
        { 0, "output-side-effects-behavior", ARG, "", SPL_CORE_OPTION_OUTPUT_SIDE_EFFECTS,
          STR_OPT(CompilerApp::setOutputSideEffects), 0, 0 },
        { 0, "rebuild-toolkits", 0, "", SPL_CORE_OPTION_REBUILD_TOOLKITS,
          STR_OPT(CompilerApp::setRebuildToolkits), 0, 0 },
        { 0, "state-variable-scoping-behavior", ARG, "", SPL_CORE_OPTION_STATE_VARIABLE_SCOPING,
          STR_OPT(CompilerApp::setStateVariableScoping), 0, 0 },
        { 0, "transport-stat-dir", ARG, "", SPL_CORE_OPTION_TRANSPORT_STAT_DIRECTORY,
          STR_OPT(CompilerApp::setTransportStatDirectory), 0, 0 },
        { 0, "validate-hosts", 0, "", SPL_CORE_OPTION_VALIDATE_HOSTS,
          STR_OPT(CompilerApp::setHostValidation), 0, 0 },
        { 0, "window-clause-ordering", ARG, "", SPL_CORE_OPTION_WINDOW_CLAUSE_ORDERING,
          STR_OPT(CompilerApp::setWindowClauseOrdering), 0, 0 },

        // non-visible options
        { 'f', "input-file", ARG, "", SPL_CORE_OPTION_INPUT_FILE,
          STR_OPT(CompilerApp::setExplicitInputFile), 1, 0 },
        { 0, "ignore-timestamps", 0, "", SPL_CORE_OPTION_IGNORE_TIMESTAMPS,
          STR_OPT(CompilerApp::setIgnoreTimestamps), 1, 0 },
        { 0, "print-ast", 0, "", SPL_CORE_OPTION_PRINT_AST, STR_OPT(CompilerApp::setPrintAST), 1,
          0 },
        { 0, "print-source", 0, "", SPL_CORE_OPTION_PRINT_SOURCE,
          STR_OPT(CompilerApp::setPrintSource), 1, 0 },
        { 0, "print-expansion", 0, "", SPL_CORE_OPTION_PRINT_EXPANSION,
          STR_OPT(CompilerApp::setPrintExpansion), 1, 0 },
        { 0, "print-tokens", 0, "", SPL_CORE_OPTION_PRINT_TOKENS,
          STR_OPT(CompilerApp::setPrintTokens), 1, 0 },
        { 0, "spl-dump", ARG, "SPL_DUMP", SPL_CORE_OPTION_STREAMS_SPL_DUMP,
          STR_OPT(CompilerApp::setDebugString), 1, 0 },
        { 0, "spl-opt-ctl", ARG, "SPL_OPT_CTL", SPL_CORE_OPTION_STREAMS_SPL_DUMP,
          STR_OPT(CompilerApp::setOptimizeControlString), 1, 0 },
        { 0, "syntax-check-only", 0, "", SPL_CORE_OPTION_SYNTAX_CHECK_ONLY,
          STR_OPT(CompilerApp::setSyntaxCheckOnly), 1, 0 },
        { 0, "tolerate-missing-toolkits", 0, "", SPL_CORE_OPTION_TOLERATE_MISSING_TOOLKITS,
          STR_OPT(CompilerApp::setTolerateMissingToolkits), 1, 0 },
        { 0, "translate", 0, "", SPL_CORE_OPTION_TRANSLATE, STR_OPT(CompilerApp::setTranslate), 1,
          0 }
    };
    APPEND_OPTIONS(options, args);
}

void CompilerApp::setMaxFilenameLength(const option_t* option, const char* value)
{
    istringstream istr(value);
    int i;
    if (!(istr >> i) || i < 128) {
        optionError(SPL_CORE_BAD_MAX_FILENAME_LENGTH);
    } else {
        _config->setIntValue(CompilerConfiguration::MaxFileNameLength, i);
    }
}

void CompilerApp::setDefaultRelocatable(const option_t* option, const char* value)
{
    string v(value);
    if ("true" == v) {
        _config->setBooleanValue(CompilerConfiguration::RelocatableDefault, true);
        // Relocatable implies restartable
        if (!_config->isSet(CompilerConfiguration::RestartableDefault) ||
            _config->isImplied(CompilerConfiguration::RestartableDefault)) {
            _config->implyBooleanValue(CompilerConfiguration::RestartableDefault, true);
        } else {
            // The restartable default has already been set.  Is it compatible?
            bool bRestartable = _config->getBooleanValue(CompilerConfiguration::RestartableDefault);
            if (!bRestartable) {
                // Complain
                optionError(SPL_CORE_INVALID_RELOC_RESTART);
            }
        }
    } else if ("false" == v) {
        _config->setBooleanValue(CompilerConfiguration::RelocatableDefault, false);
        if (_config->isImplied(CompilerConfiguration::RestartableDefault)) {
            // we must have seen a true before and done the implication...unset it
            _config->clearBooleanValue(CompilerConfiguration::RestartableDefault);
        }
    } else {
        optionError(SPL_CORE_UNKNOWN_RELOC_RESTART(value, "default-relocatable"));
        return;
    }
}

void CompilerApp::setDefaultRestartable(const option_t* option, const char* value)
{
    string v(value);
    if ("true" == v) {
        _config->setBooleanValue(CompilerConfiguration::RestartableDefault, true);
        if (_config->isImplied(CompilerConfiguration::RelocatableDefault)) {
            // If the restartable default is implied then we must have see a previous instance
            // of this option.  True here has no implication so clear it
            _config->clearBooleanValue(CompilerConfiguration::RelocatableDefault);
        }
    } else if ("false" == v) {
        _config->setBooleanValue(CompilerConfiguration::RestartableDefault, false);
        // A false for restartable implies false for relocatable
        if (!_config->isSet(CompilerConfiguration::RelocatableDefault) ||
            _config->isImplied(CompilerConfiguration::RelocatableDefault)) {
            _config->implyBooleanValue(CompilerConfiguration::RelocatableDefault, false);
        } else {
            // The relocatable default must already be set.  Is it compatible?
            bool relocatable = _config->getBooleanValue(CompilerConfiguration::RelocatableDefault);
            if (relocatable) {
                optionError(SPL_CORE_INVALID_RELOC_RESTART);
            }
        }
    } else {
        optionError(SPL_CORE_UNKNOWN_RELOC_RESTART(value, "default-restartable"));
    }
}

void CompilerApp::setCxxLangLevel(const option_t* option, const char* value)
{
    string v(value);
    if ("c++98" == v) {
        _config->setCxxLangLevel(CompilerConfiguration::LangLevelCxx98);
    } else if ("c++11" == v) {
        bf::path script = _config->getStringValue(CompilerConfiguration::InternalScriptDir);
        script /= "spl-check-support";
        vector<string> args;
        args.push_back("LangLevelCxx11");
        if (0 == ProcessLauncher::runProcess(script.string(), args)) {
            SysOut::errorln(SPL_CORE_UNSUPPORTED_OPTION_IGNORED("--c++std=c++11"));
        } else {
            _config->setCxxLangLevel(CompilerConfiguration::LangLevelCxx11);
        }
    } else {
        optionError(SPL_CORE_UNKNOWN_CXX_LANG_LEVEL(value));
    }
}

void CompilerApp::setOutputSideEffects(const option_t* option, const char* value)
{
    string v(value);
    if ("ignore" == v || "warn" == v || "error" == v) {
        _config->setStringValue(CompilerConfiguration::OutputSideEffectsBehavior, value);
    } else {
        optionError(SPL_CORE_INVALID_OUTPUT_SIDE_EFFECTS(value));
    }
}

void CompilerApp::setStateVariableScoping(const option_t* option, const char* value)
{
    string v(value);
    if ("old" == v || "new" == v) {
        _config->setStringValue(CompilerConfiguration::StateVarScopeBehavior, value);
    } else {
        optionError(SPL_CORE_UNKNOWN_OLD_NEW_BEHAVIOR(value, "state-variable-scoping-behavior"));
    }
}

void CompilerApp::setWindowClauseOrdering(const option_t* option, const char* value)
{
    string v(value);
    if ("any" == v || "strict" == v) {
        _config->setStringValue(CompilerConfiguration::WindowClauseOrderingBehavior, value);
    } else {
        optionError(SPL_CORE_UNKNOWN_WINDOW_CLAUSE_ORDERING_BEHAVIOR(value));
    }
}

void CompilerApp::processDebugLevel(const option_t* option, const char* value)
{
    // just send it up
    DistilleryApplication::processDebugLevel(option, value);
}

void CompilerApp::setHelp(const option_t* option, const char* value)
{
    printUsage(cout, false);
    cout << endl;
    Distillery::DistAppTerminate(0);
}

void CompilerApp::setAllowConvenienceFusionOptions(const option_t* option, const char* value)
{
    SysOut::writeln(SPL_CORE_OPTION_DEPRECATED_AND_IGNORED("--allow-convenience-fusion-options"));
}

ostream& CompilerApp::printDescription(ostream& o) const
{
    o << SPL_CORE_COMPILER_USAGE << endl;
    o << "SPL - Streams Compiler";
    return o;
}

void CompilerApp::setOutputDirectory(const option_t* option, const char* value)
{
    // If the directory specified is relative, compute the complete path and set it
    // Note that if a relative path is given, the complete path is relative to cwd.
    bf::path p(value);
    string dir;
    if (p.is_complete()) {
        dir = value;
    } else {
        p = bf::system_complete(p);
        dir = p.string();
    }
    _config->setStringValue(CompilerConfiguration::OutputDir, dir);
    if (bf::exists(p) && !bf::is_directory(p))
        SysOut::die(SPL_CORE_OUTPUT_DIRECTORY_IS_NOT_A_DIRECTORY(string(value)));
}

void CompilerApp::setDataDirectory(const option_t* option, const char* value)
{
    // If the directory specified is relative, compute the complete path and set it
    // Note that if a relative path is given, the complete path is relative to cwd.
    bf::path p(value);
    string dir;
    if (p.is_complete()) {
        dir = value;
    } else {
        p = bf::system_complete(p);
        dir = p.string();
    }
    _config->setStringValue(CompilerConfiguration::DataDir, dir);
    if (bf::exists(p) && !bf::is_directory(p)) {
        SysOut::die(SPL_CORE_DATA_DIRECTORY_IS_NOT_A_DIRECTORY(string(value)));
    }
}

void CompilerApp::setModelFile(const option_t* option, const char* value)
{
    _config->setStringValue(CompilerConfiguration::ModelFile, value);
}

void CompilerApp::setCheckpointDirectory(const option_t*, const char* value)
{
    SysOut::warnln(SPL_CORE_CKPT_DIR_OPTION_WARNING, SourceLocation::invalidSourceLocation);
    // If the directory specified is relative, compute the complete path and set it
    // Note that if a relative path is given, the complete path is relative to cwd.
    bf::path p(value);
    string dir;
    if (p.is_complete()) {
        dir = value;
    } else {
        p = bf::system_complete(p);
        dir = p.string();
    }
    _config->setStringValue(CompilerConfiguration::CheckpointDir, dir);
    if (bf::exists(p) && !bf::is_directory(p))
        SysOut::die(SPL_CORE_CKPT_DIRECTORY_IS_NOT_A_DIRECTORY(string(value)));
}

void CompilerApp::setTransportStatDirectory(const option_t* option, const char* value)
{
    // If the directory specified is relative, compute the complete path and set it
    // Note that if a relative path is given, the complete path is relative to cwd.
    bf::path p(value);
    string dir;
    if (p.is_complete()) {
        dir = value;
    } else {
        p = bf::system_complete(p);
        dir = p.string();
    }
    _config->setStringValue(CompilerConfiguration::TransportStatDir, dir);
    if (bf::exists(p) && !bf::is_directory(p))
        SysOut::die(SPL_CORE_TRANSPORT_STAT_DIRECTORY_IS_NOT_A_DIRECTORY(string(value)));
}

void CompilerApp::setNoMixedModePreprocessing(const option_t*, const char* value)
{
    _config->setBooleanValue(CompilerConfiguration::NoMixedModePreprocessing, true);
}

void CompilerApp::setNoToolkitIndexing(const option_t*, const char* value)
{
    _config->setBooleanValue(CompilerConfiguration::NoToolkitIndexing, true);
}

void CompilerApp::setIgnoreTimestamps(const option_t* /*option*/, const char* value)
{
    _config->setBooleanValue(CompilerConfiguration::IgnoreTimestamps, true);
}

void CompilerApp::setTolerateMissingToolkits(const option_t* /*option*/, const char* value)
{
    _config->setBooleanValue(CompilerConfiguration::TolerateMissingToolkits, true);
}

void CompilerApp::setLdFlags(const option_t* /*option*/, const char* value)
{
    _config->setStringValue(CompilerConfiguration::LdFlags, value);
}

void CompilerApp::setCxxFlags(const option_t* /*option*/, const char* value)
{
    _config->setStringValue(CompilerConfiguration::CxxFlags, value);
}

void CompilerApp::setMakeThreadCount(const option_t* /*option*/, int value)
{
    if (value < 0)
        SysOut::die(SPL_CORE_INVALID_THREAD_COUNT_SPECIFIED(value));
    _config->setIntValue(CompilerConfiguration::MakeThreadCount, value);
}

void CompilerApp::setDistccRemoteHostCount(const option_t* /*option*/, int value)
{
    if (value <= 0)
        SysOut::die(SPL_CORE_INVALID_DISTCC_NODE_COUNT_SPECIFIED(value));
    _config->setIntValue(CompilerConfiguration::DistccRemoteHostCount, value);
}

void CompilerApp::setCodeBeautification(const option_t* /*option*/, const char* /*value*/)
{
    _config->setBooleanValue(CompilerConfiguration::CodeBeautification, true);
}

void CompilerApp::setDefaultPoolSize(const option_t* /*option*/, int value)
{
    if (value <= 0)
        SysOut::die(SPL_CORE_INVALID_DEFAULT_POOL_SIZE_SPECIFIED(value));
    _config->setIntValue(CompilerConfiguration::DefaultPoolSize, value);
}

void CompilerApp::setAvoidRuntimeYield(const option_t* option, const char* value)
{
    const std::string v = value;
    if (v == "auto" || v == "AUTO")
        _config->setARYMode(CompilerConfiguration::ARYAuto);
    else if (v == "yes" || v == "YES")
        _config->setARYMode(CompilerConfiguration::ARYYes);
    else if (v == "no" || v == "NO")
        _config->setARYMode(CompilerConfiguration::ARYNo);
    else
        optionError(SPL_CORE_UNKNOWN_AVOID_RUNTIME_YIELD(v));
}

void CompilerApp::setTransport(const option_t* option, const char* value)
{
    const std::string trans = value;
    if (trans == "tcp" || trans == "TCP")
        _defaultTransport = Config::TCP;
    else
        optionError(SPL_CORE_UNKNOWN_TRANSPORT(trans));
}

void CompilerApp::setMainComposite(const option_t* option, const char* value)
{
    static Distillery::RegEx main1(
      "^"                                                               // start of line
      "((([_[:alpha:]][_[:alnum:]]*)\\.)*[_[:alpha:]][_[:alnum:]]*::)?" //[name.space::]
      "[_[:alpha:]][_[:alnum:]]*"                                       // composite
      "$");                                                             // end of line
    string name = value;
    if (!main1.match(name))
        SysOut::die(SPL_CORE_MAIN_COMPOSITE_NOT_A_VALID_IDENTIFIER(name));
    _config->setStringValue(CompilerConfiguration::MainComposite, name);
}

void CompilerApp::setFusionMode(const option_t* option, const char* value)
{
    SysOut::writeln(SPL_CORE_OPTION_DEPRECATED_AND_IGNORED("-p, --part-mode"));
}

void CompilerApp::setExplicitInputFile(const option_t*, const char* value)
{
    string const filenm = string(value);
    bf::path filept(filenm);
    if (!bf::exists(filept))
        SysOut::die(SPL_CORE_PATH_DOES_NOT_EXIST(filenm));
    if (bf::is_directory(filept))
        SysOut::die(SPL_CORE_PATH_IS_A_DIRECTORY(filenm));
    string ext = bf::extension(filept);
    if (ext != DPS_EXT && ext != SPL_EXT)
        SysOut::die(SPL_CORE_FILE_HAS_INVALID_EXT(filenm, ext, ".spl"));
    _config->setStringValue(CompilerConfiguration::ExplicitInputFile, filept.string());
}

void CompilerApp::setRelaxFusionRelocatabilityRestartability(const option_t*, const char* value)
{
    SysOut::writeln(SPL_CORE_OPTION_DEPRECATED_AND_IGNORED(
      "-O, --set-relax-fusion-relocatability-restartability"));
}

void CompilerApp::setStaticLinkage(const option_t*, const char* value)
{
    SysOut::writeln(SPL_CORE_OPTION_DEPRECATED_AND_IGNORED("-s, --static-link"));
}

void CompilerApp::setSPLPath(const option_t*, const char* value)
{
    // remember this for later use in ::run
    if (_config->isSet(CompilerConfiguration::SPLPath)) {
        string app1 = _config->getStringValue(CompilerConfiguration::SPLPath) + SPLPATH_SEP + value;
        _config->setStringValue(CompilerConfiguration::SPLPath, app1);
    } else {
        _config->setStringValue(CompilerConfiguration::SPLPath, value);
    }
}

void CompilerApp::setMakefileGeneration(const option_t* /*option*/, const char* /*value*/)
{
    _config->setBooleanValue(CompilerConfiguration::GenMakefile, true);
}

void CompilerApp::setOptimizedCodeGeneration(const option_t* /*option*/, const char* /*value*/)
{
    _optimizedSelected = true;
}

void CompilerApp::setNoOptimizedCodeGeneration(const option_t* /*option*/, const char* /*value*/)
{
    _noOptimizedSelected = true;
}

void CompilerApp::setClean(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::Clean, true);
}

void CompilerApp::setCleanAndSuppress(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::CleanAndSuppress, true);
}

void CompilerApp::setVerboseMode(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::Verbose, true);
}

void CompilerApp::setNetworkByteOrderEncoding(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::NetworkByteOrderEncoding, true);
}

void CompilerApp::setWrapperSDB(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::SetWrapperSDB, true);
}

void CompilerApp::setStandalone(const option_t*, const char*)
{
    SysOut::writeln(SPL_CORE_OPTION_DEPRECATED_AND_IGNORED("-T, --standalone-application"));
}

void CompilerApp::setDebugString(const option_t*, const char* value)
{
    Debug::instance().addDebugString(value);
}

void CompilerApp::setOptimizeControlString(const option_t*, const char* value)
{
    OptimizeControl::instance().addOptimizeControlString(value);
}

void CompilerApp::setPreferFacades(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::PreferFacades, true);
}

void CompilerApp::setPrintAST(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::PrintAST, true);
}

void CompilerApp::setPrintSource(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::PrintSource, true);
}

void CompilerApp::setPrintExpansion(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::PrintExpansion, true);
}

void CompilerApp::setPrintTokens(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::PrintTokens, true);
}

void CompilerApp::setSyntaxCheckOnly(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::SyntaxCheckOnly, true);
}

void CompilerApp::setTranslate(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::Translate, true);
}

void CompilerApp::setHostValidation(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::HostValidation, true);
}

void CompilerApp::setRebuildToolkits(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::RebuildToolkits, true);
}

void CompilerApp::setProfiling(const option_t*, int value)
{
    SysOut::writeln(SPL_CORE_OPTION_DEPRECATED_AND_IGNORED("-P, --profile-all-operators"));
}

void CompilerApp::setProfilingSampleRate(const option_t*, const char* value)
{
    SysOut::writeln(SPL_CORE_OPTION_DEPRECATED_AND_IGNORED("-S, --profiling-sampling-rate"));
}

void CompilerApp::setMaximumPECpuFrac(const option_t*, const char* value)
{
    SysOut::writeln(SPL_CORE_OPTION_DEPRECATED_AND_IGNORED("-Z, --max-pe-cpu-fraction"));
}

void CompilerApp::setJustTheApplicationModel(const option_t*, const char*)
{
    _config->setBooleanValue(CompilerConfiguration::JustTheAppModel, true);
}

bool CompilerApp::createMakefileIfNeeded()
{
    if (!_config->isSet(CompilerConfiguration::GenMakefile)) {
        return false;
    }

    string const& main1 = _config->getStringValue(CompilerConfiguration::MainComposite);
    bf::path appDir = _config->getStringValue(CompilerConfiguration::ApplicationDir);
    bf::path outDir = _config->getStringValue(CompilerConfiguration::OutputDir);
    bf::path dataDir = _config->getStringValue(CompilerConfiguration::DataDir);

    bf::path mkfile = appDir / "Makefile";
    ofstream of(mkfile.string().c_str());
    if (!of) {
        SysOut::die(SPL_CORE_CANNOT_CREATE_FILE(mkfile.string(), RuntimeUtility::getErrorNoStr()));
    }
    SysOut::writeln(SPL_CORE_CREATING_SPL_MAKEFILE, cout);

    bool useOutDir = (outDir != appDir / "output");
    bool useDataDir = _config->isSet(CompilerConfiguration::DataDir);

    string outDirOpt;
    if (useOutDir) {
        outDirOpt = "--output-directory ";
        outDirOpt += outDir.string();
        outDirOpt += " ";
    }
    string dataDirOpt;
    if (useDataDir) {
        dataDirOpt = "--data-directory ";
        dataDirOpt += dataDir.string();
        dataDirOpt += " ";
    }
    of << ".PHONY: all distributed clean \n\n";
    of << "SPLC_FLAGS ?= " << outDirOpt << dataDirOpt << "\n";
    of << "SPLC = $(STREAMS_INSTALL)/bin/sc\n";
    of << "SPL_CMD_ARGS ?= \n";
    of << "SPL_MAIN_COMPOSITE = " << main1;
    of << "\n\n";
    of << "all: distributed\n\n";
    of << "distributed:\n";
    of << "\t$(SPLC) $(SPLC_FLAGS) -M $(SPL_MAIN_COMPOSITE) $(SPL_CMD_ARGS)\n\n";
    of << "clean: \n";
    of << "\t$(SPLC) $(SPLC_FLAGS) -C -M $(SPL_MAIN_COMPOSITE)\n\n";
    of.close();
    return true;
}
