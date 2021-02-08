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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_SPL_APPLICATION_IMPL_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_SPL_APPLICATION_IMPL_H

#include <SPL/Runtime/ProcessingElement/PEForwardDeclarations.h>
#include <SPL/Runtime/ProcessingElement/PlatformAdapter.h>
#include <SPL/Runtime/Type/List.h>
#include <SPL/Runtime/Type/String.h>
#include <UTILS/DistilleryApplication.h>

#include <boost/filesystem/path.hpp>

namespace xmlns {
namespace prod {
namespace streams {
namespace application {
namespace v4200 {
class applicationSetType;
class compositeOperInstanceType;
}
}
}
}
}

namespace SPL {

class SplPE;

//! Run a standalone SPL application
class StandaloneApplicationImpl : public Distillery::DistilleryApplication
{
  public:
    /// Constructor
    StandaloneApplicationImpl();

    /// Destructor
    virtual ~StandaloneApplicationImpl();

  protected:
    /// Override this to perfrom pre-argument processing
    virtual int init();

    /// Override this to specify arguments and their functions
    virtual void getArguments(option_vector_t& options);

    /// Suppress default visible options
    virtual void getDistilleryApplicationVisibleOptions(option_vector_t& options) {}

    /// Override help to avoid printing to standard error
    void setHelp(const option_t* option, const char* value);

    /// Override this to switch the logger before the infrastructure does
    virtual void processOptions(int argc, char** argv, std::vector<std::string>& remains)
    {
        DistilleryApplication::processOptions(argc, argv, remains);
        setupLogger(); // needs to be done before this->run
    }
    void setupLogger();

    /// Print the application description
    /// @param o output stream
    /// @return the output stream
    virtual std::ostream& printDescription(std::ostream& o) const;

    /// Print usage information
    /// @param o output stream
    /// @param extended true if extended usage
    /// @return the output stream
    virtual std::ostream& printUsage(std::ostream& o, bool extended);

    /// The run method
    /// @param args command line arguments
    int run(std::vector<std::string> const& args);

    /// Create the instance of the PE and remember it
    void createPE();

    /// Perform PE processing
    /// Called from derived->run
    void process();

    /// Return the current Processing Element
    /// @return the current PE
    SplPE* getPE();

  protected:
    void getExecutableLocation(boost::filesystem::path& exeDirPath, std::string& exeFileName);
    void setSignalHandlers();
    void setArguments(std::vector<std::string> const& args);
    void loadADL();
    void createAADL();
    void initializeEnv();

    void setLogLevel(const option_t* option, int value);
    void setTraceLevel(const option_t* option, int value);
    void setConsole(const option_t* option, int value);
    void setShutdownTimer(const option_t* option, char const* value);
    void shutdownFromSignal(int sig);
    void setDataDirectory(const option_t* option, char const* value);

    /// SIGTERM signal handler
    /// @param sig the signal that triggered the violation
    /// @note: The SIGTERM signal is a generic signal used to cause program
    /// termination. Unlike SIGKILL, this signal can be blocked, handled,
    /// and ignored. It is the normal way to politely ask a program to
    /// terminate.
    ///
    /// The shell command kill generates SIGTERM by default.
    void mySigTermHandler(int sig);

    /// SIGINT signal handler
    /// @param sig the signal that triggered the violation
    /// @note: The SIGINT ("program interrupt") signal is sent when the user
    /// types the INTR character (normally C-c).
    void mySigIntHandler(int sig);

    /// SIGQUIT signal handler
    /// @param sig the signal that triggered the violation
    /// @note: The SIGQUIT signal is similar to SIGINT, except that it's
    /// controlled by a different key (the QUIT character, usually C-\) and
    /// produces a core dump when it terminates the process, just like a
    /// program error signal. You can think of this as a program error
    /// condition "detected" by the user.
    ///
    /// Certain kinds of cleanups are best omitted in handling SIGQUIT. For
    /// example, if the program creates temporary files, it should handle
    /// the other termination requests by deleting the temporary files. But
    /// it is better for SIGQUIT not to delete them, so that the user can
    /// examine them in conjunction with the core dump.
    void mySigQuitHandler(int sig);

  private:
    void checkForJVMArgs(
      const xmlns::prod::streams::application::v4200::compositeOperInstanceType& compositeInstance);
    std::string getADLFileName();

    int _logLevel;
    int _traceLevel;
    bool _logLevelSet;
    bool _traceLevelSet;
    bool _stderrLoggerSet;
    std::auto_ptr<PEModel> _theModel;
    SplPE* _thePE;
    std::auto_ptr<PlatformAdapter> _thePlatform;
    double _shutdownTimer;
    std::string _dataDirectory;
    Distillery::debug::Tracer* _appLogger;
    Distillery::debug::Logger* _logLogger;
    SPL::list<rstring> _rawArgs; // raw arguments that are specified
    std::tr1::unordered_set<std::string> _jvmArgs;
    std::vector<std::string> _finalVMArgs;
    std::tr1::unordered_set<std::string> _requiredArgs; // names of required args
    std::tr1::unordered_map<std::string, std::string>
      _optionalArgs; // names/values of optional args
    std::vector<std::pair<std::string, std::string> > _namedArgValues; // values of named args
    std::auto_ptr<xmlns::prod::streams::application::v4200::applicationSetType> _applicationSet;
};
}

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_SPL_APPLICATION_IMPL_H */
