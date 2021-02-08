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

#ifndef COMPILER_CONFIGURATION_H
#define COMPILER_CONFIGURATION_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>

#include <string>
#include <vector>

namespace SPL {
//! User Configuration Options for SPL compiles
class DLL_PUBLIC CompilerConfiguration : public Singleton<CompilerConfiguration>
{
  public:
    enum BooleanOption
    {
        Clean,                    /*!< clean */
        CleanAndSuppress,         /*!< clean and suppress compilation */
        CodeBeautification,       /*!< run astyle to re-indent code */
        SetWrapperSDB,            /*!< set main config wrapper as sdb */
        GenerateCode,             /*!< do Operator IR generation */
        GenMakefile,              /*!< generate makefile */
        HostValidation,           /*!< validate explicit host names */
        IgnoreTimestamps,         /*!< Don't check toolkit timestamps */
        JavaHomeIsInstall,        /*!< true if we set java home to point to the install version */
        JustTheAppModel,          /*!< create only the application model */
        NetworkByteOrderEncoding, /*!< Use network byte order encoding */
        NoMixedModePreprocessing, /*!< do not preprocess mixed-mode files */
        NoToolkitIndexing,        /*!< do not perform tookit indexing */
        Optimized,                /*!< generate optimized code */
        PrintAST,                 /*!< flag to print the ast */
        PrintSignature,           /*!< print compilation unit signature */
        PrintSource,              /*!< flag to print the source code */
        PrintExpansion,           /*!< flag to print the expansion of composites */
        PrintTokens,              /*!< flag to print the lexer tokens */
        PreferFacades,            /*!< try to use facade tuples if possible */
        RebuildToolkits,          /*!< auto-rebuild out of date toolkits */
        RelocatableDefault,       /*!< the default setting for operator relocatability */
        RestartableDefault,       /*!< the default setting for operator restartability */
        SyntaxCheckOnly, /*!< only perform syntax checking, no type checking or other semantic analysis */
        TolerateMissingToolkits, /*!< tolerate missing toolkits */
        ToolkitAugmented,        /*!< spl-make-toolkit should gen extra information for IDE */
        ToolkitMakeOperator,     /*!< spl-make-toolkit should make operators. */
        ToolkitSilent,           /*!< spl-make-toolkit should be quiet */
        Translate,               /*!< translate from spl 1 to 2 */
        Verbose,                 /*!< generate verbose output */
        numBooleanOptions
    };

    enum StringOption
    {
        ApplicationDir,    /*!< application directory */
        CheckpointDir,     /*!< checkpoint directory.  Either relative to workingDir or absolute */
        CxxFlags,          /*!< CXX flags to pass to make */
        DataDir,           /*!< output directory. */
        ExplicitInputFile, /*!< inout file specified via -f */
        InputFile,         /*!< file name to compile */
        InstallDir,        /*!< empty if running from source tree, otherwise root install dir */
        InternalScriptDir, /*!< script directory for internal scripts */
        JavaHome,          /*!< Setting of env var JAVA_HOME if it is set */
        LdFlags,           /*!< LD flags to pass to make */
        MainComposite,     /*!< main composite use */
        MakeToolkitDir,    /*!< spl-make-toolkit directory */
        ModelFile,         /*!< file into which internal data structure info is dumped */
        OutputDir,         /*!< output directory. */
        OutputSideEffectsBehavior, /*!< selects the behavior when an output clause contains expressions with side effects */
        ScriptDir,             /*!< script directory */
        SPLPath,               /*!< override of SPLPATH */
        StatsDir,              /*!< stat directory - performance stats */
        TemplateDir,           /*!< template directory */
        ToolkitDescription,    /*!< Description for spl-make-toolkit. */
        ToolkitDir,            /*!< default toolkit directory */
        ToolkitName,           /*!< Name for spl-make-toolkit. */
        StateVarScopeBehavior, /*!< selects the behavior when a state variable shadows an attribute */
        ToolkitProductVersion,        /*!< Product Version for spl-make-toolkit. */
        ToolkitVersion,               /*!< Version for spl-make-toolkit. */
        TransportStatDir,             /*!< directory for transport stat files. */
        XsdDir,                       /*!< xsd directory */
        WindowClauseOrderingBehavior, /*!< selects the behavior when a window clause is ut of order */
        numStringOptions
    };

    enum IntOption
    {
        DefaultPoolSize,       /*!< default pool size */
        DistccRemoteHostCount, /*!< number of remote distcc nodes */
        MakeThreadCount,       /*!< number of make threads */
        MaxFileNameLength, /*!< The maximum length that a filename can be (limited by system max) */
        numIntOptions
    };

    enum FloatOption
    {
        numFloatOptions
    };

    enum StringVectorOption
    {
        MixedModeFile, /*!< mixed-mode file */
        numStringVectorOptions
    };

    enum FusionMode
    {
        FusionDefault, //!< Default COLA fusion (default value too)
        FusionNone,    //!< Each operator has its own PE
        FusionAll,     //!< All operators on one PE
        FusionRandom,  //!< Random number of PEs and random PE placement
        FusionOptimize //!< COLA optimization  (needs profiling?)
    };

    enum ARYMode
    {
        ARYAuto, //!< Take the default action
        ARYYes,  //!< Avoid pthread_yield()
        ARYNo    //!< Use pthread_yield()
    };

    enum CxxLangLevel
    {
        LangLevelCxx98,
        LangLevelCxx11
    };

    enum OptionState
    {
        Unset = 0,
        Implied,
        Explicit
    };
    //! Constructor
    CompilerConfiguration();

    /// Set a boolean option value
    /// @param b Boolean Option
    /// @param value value to be set
    void setBooleanValue(BooleanOption b, bool value)
    {
        _boolOptions[b] = value;
        _isSetBool[b] = Explicit;
    }

    /// Imply a boolean option value.  Implied options are considered set
    /// but not by the direct occurance of that option on the command line
    /// @param b Boolean Option
    /// @param value value to be set
    void implyBooleanValue(BooleanOption b, bool value)
    {
        _boolOptions[b] = value;
        _isSetBool[b] = Implied;
    }

    void clearBooleanValue(BooleanOption b) { _isSetBool[b] = Unset; }

    /// Return true if setBooleanValue has been invoked
    /// @return true iff setBooleanValue has been called
    bool isImplied(BooleanOption b) const { return _isSetBool[b] == Implied; }

    /// Return true if setBooleanValue or implyBooleanValue have been invoked
    /// @return true iff setBooleanValue or implyBooleanValue have been called
    bool isSet(BooleanOption b) const { return _isSetBool[b] != Unset; }

    /// Return value of a boolean option
    /// @return HostPlacement
    bool getBooleanValue(BooleanOption b) const { return _boolOptions[b]; }

    /// Set a string option value
    /// @param s String Option
    /// @param value value to be set
    void setStringValue(StringOption s, const std::string& value)
    {
        _stringOptions[s] = value;
        _isSetString[s] = true;
    }

    /// Return true if setStringValue has been invoked
    /// @param s String Option
    /// @return true iff setStringValue has been called
    bool isSet(StringOption s) const { return _isSetString[s]; }

    /// Return value of a boolean option
    /// @param s String Option
    /// @return HostPlacement
    const std::string& getStringValue(StringOption s) const { return _stringOptions[s]; }

    /// Set an int option value
    /// @param i Integer Option
    /// @param value value to be set
    void setIntValue(IntOption i, int value)
    {
        _intOptions[i] = value;
        _isSetInt[i] = true;
    }

    /// Return true if setIntValue has been invoked
    /// @param i Integer Option
    /// @return true iff setIntValue has been called
    bool isSet(IntOption i) const { return _isSetInt[i]; }

    /// Return value of an int option
    /// @param i Integer Option
    /// @return HostPlacement
    int getIntValue(IntOption i) const { return _intOptions[i]; }

    /// Set a float option value
    /// @param f Float Option
    /// @param value value to be set
    void setFloatValue(FloatOption f, double value)
    {
        _floatOptions[f] = value;
        _isSetFloat[f] = true;
    }

    /// Return true if setFloatValue has been invoked
    /// @param f Float Option
    /// @return true iff setFloatValue has been called
    bool isSet(FloatOption f) const { return _isSetFloat[f]; }

    /// Return value of an float option
    /// @param f Float Option
    /// @return HostPlacement
    double getFloatValue(FloatOption f) const { return _floatOptions[f]; }

    /// Return true if setFloatValue has been invoked
    /// @param f Float Option
    /// @return true iff setFloatValue has been called
    bool isSet(StringVectorOption vs) const { return _isSetStringVector[vs]; }

    /// Set a vector string option
    /// @param vs Vector string Option
    /// @param value value toset
    void setStringVectorValue(StringVectorOption vs, const std::vector<std::string>& value)
    {
        _stringVectorOptions[vs] = value;
        _isSetStringVector[vs] = true;
    }

    /// Return value of an float option
    /// @param f Float Option
    /// @return HostPlacement
    std::vector<std::string>& getStringVectorValue(StringVectorOption vs)
    {
        return _stringVectorOptions[vs];
    }

    /// Return value of an float option
    /// @param f Float Option
    /// @return HostPlacement
    std::vector<std::string> const& getStringVectorValue(StringVectorOption vs) const
    {
        return _stringVectorOptions[vs];
    }

    /// Return the list of pathnames to lookup SPL names
    /// Usually set to a default, or from SPLPATH environment variable
    /// @return The vector of paths
    std::vector<std::string>& getSPLPaths() { return _splPaths; }

#if 0
        /// Set the fusion mode
        /// @param mode Fusion mode
        void setFusionMode(FusionMode mode)
        {
            _fusionMode = mode;
        }

        /// Return the current Fusion mode
        /// @return fusion mode
        FusionMode getFusionMode() const
        {
            return _fusionMode;
        }
#endif

    /// Set the 'Avoid Runtime Yield' mode
    /// @param mode 'avoid runtime yield' mode
    void setARYMode(ARYMode mode) { _aryMode = mode; }

    /// Return the current Fusion mode
    /// @return 'avoid runtime yield' mode
    ARYMode getARYMode() const { return _aryMode; }

    /// Set the C++ language level to use when compiling generated code
    /// @param ll The language level.
    void setCxxLangLevel(CxxLangLevel ll) { _cxxLangLevel = ll; }

    /// Return the C++ language level to use
    /// @return Return the language level
    CxxLangLevel getCxxLangLevel() const { return _cxxLangLevel; }

  private:
    bool _boolOptions[numBooleanOptions];
    std::string _stringOptions[numStringOptions];
    int _intOptions[numIntOptions];
    double _floatOptions[numFloatOptions];
    std::vector<std::string> _stringVectorOptions[numStringVectorOptions];
    OptionState _isSetBool[numBooleanOptions];
    bool _isSetString[numStringOptions];
    bool _isSetInt[numIntOptions];
    bool _isSetFloat[numFloatOptions];
    bool _isSetStringVector[numStringVectorOptions];
    FusionMode _fusionMode;
    ARYMode _aryMode;
    CxxLangLevel _cxxLangLevel;
    std::vector<std::string> _splPaths;
};
};

#endif /* SPL_CONFIGURATION_H */
