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

#ifndef SPL_COMPILER_APP_H
#define SPL_COMPILER_APP_H

#include <dst-config.h>

#include <SPL/CodeGen/Config.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Utility/ModelDumper.h>
#include <SPL/Utility/PhaseTiming.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/HashMapHelpers.h>

#include <boost/filesystem/path.hpp>

namespace SPL {
class CompilerConfiguration;
class FormattableMessage;
class FrontEndDriver;
class OperatorGraph;
class PathSearch;
class StreamsUseDef;
class Toolkit;
class TransportStats;
class UsedOperatorInvocations;

class DLL_PUBLIC CompilerApp
  : public Distillery::DistilleryApplication
  , public Singleton<CompilerApp>
{
  public:
    CompilerApp();
    ~CompilerApp();

    void mySigIntHandler(int /*signum*/);
    void getArguments(option_vector_t& options);
    std::ostream& printDescription(std::ostream& o) const;
    virtual int run(const std::vector<std::string>& remains);

    static CompilerApp& getSPLCompiler() { return *static_cast<CompilerApp*>(_thisApp); }

    TransportStats& getTransportStats() { return *_transportStats; }

    StreamsUseDef& getStreamsUseDef() { return *_streamsUseDef; }

    FrontEndDriver& getFrontEndDriver() { return *_frontEnd; }

    Config::TransportKind getDefaultTransport() { return _defaultTransport; }

    // Map from name/version to filename for provtool
    typedef std::tr1::unordered_map<std::string, std::string> ProvisionMap;

    /// Return the toolkit/version to filename mapping
    /// @return name/version mapping to filename
    const ProvisionMap& provMap() const { return _provMap; }

    // Return the list of user supplied compile time arguments
    const std::vector<std::string>& getPreprocessorArgs() const { return _preprocessorArgs; }

    PhaseTiming& getPhaseTimer() { return _timer; }

  protected:
    virtual int init(); // override of base class

    void compile();
    void collectUsedTypes(const UsedOperatorInvocations& usedOps,
                          std::tr1::unordered_map<std::string, const RootTyp*>& usedTypes);
    void loadToolkits();
    bool locateInputFile();
    std::string checkStreamsInstallation();
    void cleanupArtifacts();
    bool performCleanup();
    bool hasVersionChangedSinceLastCompile();
    virtual void getDistilleryApplicationVisibleOptions(option_vector_t& options);

    bool getInterruptable() { return _interruptable; }
    void setInterruptable(bool enable) { _interruptable = enable; }

    // Option Processing routines
    void setHelp(const option_t* option, const char* value);
    void setAllowConvenienceFusionOptions(const option_t* option, const char* value);
    void setAvoidRuntimeYield(const option_t* option, const char* value);
    void setClean(const option_t* option, const char* value);
    void setCleanAndSuppress(const option_t* option, const char* value);
    void setCxxFlags(const option_t* option, const char* value);
    void setWrapperSDB(const option_t* option, const char* value);
    void setDefaultPoolSize(const option_t* option, int value);
    void setDistCleanAndSuppress(const option_t* option, const char* value);
    void setDistccRemoteHostCount(const option_t* option, int value);
    void setExplicitInputFile(const option_t* option, const char* value);
    void setFusionMode(const option_t* option, const char* value);
    void setIgnoreTimestamps(const option_t* option, const char* value);
    void setTolerateMissingToolkits(const option_t* option, const char* value);
    void setDoNotTolerateDepreciatedCommandLineUsage(const option_t* option, const char* value);
    void setJustTheApplicationModel(const option_t* option, const char* value);
    void setCheckpointDirectory(const option_t* option, const char* value);
    void setLdFlags(const option_t* option, const char* value);
    void setMakeThreadCount(const option_t* option, int value);
    void setMakefileGeneration(const option_t* option, const char* value);
    void setHostValidation(const option_t*, const char*);
    void setMainComposite(const option_t*, const char*);
    void setOptimizedCodeGeneration(const option_t* option, const char* value);
    void setNoOptimizedCodeGeneration(const option_t* option, const char* value);
    void setPreferFacades(const option_t* option, const char* value);
    void setPrintAST(const option_t* option, const char* value);
    void setPrintSource(const option_t* option, const char* value);
    void setPrintExpansion(const option_t* option, const char* value);
    void setPrintTokens(const option_t* option, const char* value);
    void setProfiling(const option_t* option, int value);
    void setProfilingSampleRate(const option_t* option, const char* value);
    void setMaximumPECpuFrac(const option_t* option, const char* value);
    void setRelaxFusionRelocatabilityRestartability(const option_t* option, const char* value);
    void setStaticLinkage(const option_t* option, const char* value);
    void setSPLPath(const option_t* option, const char* value);
    void setStandalone(const option_t* option, const char* value);
    void setSyntaxCheckOnly(const option_t* option, const char* value);
    void setDebugString(const option_t* option, const char* value);
    void setTranslate(const option_t* option, const char* value);
    void setTransport(const option_t* option, const char* value);
    void setVerboseMode(const option_t* option, const char* value);
    void setNetworkByteOrderEncoding(const option_t* option, const char* value);
    void setCodeBeautification(const option_t* option, const char* value);
    void setNoMixedModePreprocessing(const option_t* option, const char* value);
    void setRebuildToolkits(const option_t* option, const char* value);
    void setNoToolkitIndexing(const option_t* option, const char* value);
    void setOutputDirectory(const option_t* option, const char* value);
    void setDataDirectory(const option_t* option, const char* value);
    void setTransportStatDirectory(const option_t* option, const char* value);
    void setStateVariableScoping(const option_t* option, const char* value);
    void setWindowClauseOrdering(const option_t* option, const char* value);
    void setCxxLangLevel(const option_t* option, const char* value);
    void setOutputSideEffects(const option_t* option, const char* value);
    void optionError(const FormattableMessage&);
    void setModelFile(const option_t* option, const char* value);
    void setOptimizeControlString(const option_t* option, const char* value);
    void setDefaultRelocatable(const option_t* option, const char* value);
    void setDefaultRestartable(const option_t* option, const char* value);
    void setMaxFilenameLength(const option_t* option, const char* value);

  protected:
    void computeCompilerPaths();
    void setApplicationDirectories();
    void createApplicationDirectories();
    bool createMakefileIfNeeded();
    void createOldStyleMakefile();
    void readMetaFile(std::vector<std::string>& paths);
    void processDebugLevel(const option_t* option, const char* value);
    void installFaultSignalHandlers(void);
    void iceHandler(int sig);

    std::auto_ptr<CompilerConfiguration> _config;
    std::auto_ptr<ModelDumper> _modelDumper;
    std::auto_ptr<FrontEndDriver> _frontEnd;
    std::auto_ptr<PathSearch> _pathSearch;
    std::auto_ptr<TransportStats> _transportStats;
    std::auto_ptr<StreamsUseDef> _streamsUseDef;
    volatile bool _interruptable;
    Config::TransportKind _defaultTransport;
    std::vector<std::string> _preprocessorArgs;
    ProvisionMap _provMap;
    bool _optionError;
    PhaseTiming _timer;

  private:
    bool _optimizedSelected;
    bool _noOptimizedSelected;
};
};

#endif /* SPL_COMPILER_APP_H */
