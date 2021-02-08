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

#ifndef SPL_CODEGEN_HELPER_H
#define SPL_CODEGEN_HELPER_H

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/commonModel.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Utility/SysOut.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <cstring>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <time.h>
#include <vector>

namespace SPL {

namespace Enumeration_ {
class EnumerationModel;
}

namespace Tuple_ {
class TupleModel;
}

namespace XML_ {
class XMLModel;
}

namespace Operator {
class OperatorModel;
namespace Instance {
class OperatorInstanceModel;
class Parameter;
}
}

namespace Standalone {
class StandaloneModel;
}

class CompilerConfiguration;
class SourceLocation;

///! Helper functions for code generation purposes
class CodeGenHelper : public SPL::Singleton<CodeGenHelper>
{
    typedef Enumeration_::EnumerationModel EnumerationModel;
    typedef Tuple_::TupleModel TupleModel;
    typedef Operator::Instance::OperatorInstanceModel OperatorInstanceModel;
    typedef Standalone::StandaloneModel StandaloneModel;
    typedef XML_::XMLModel XMLModel;

  public:
    /// Constructor
    CodeGenHelper();

    enum Model
    {
        Enum,
        OperatorInstance,
        Script,
        Standalone,
        Tuple,
        XML,
        NumModels
    };

    /// Generate code for the given tuple if it has not yet been generated or is out-of-date.
    /// @param tuple The tuple we are generating code for
    /// @param name Name of the artifact to be generated
    /// @param signature The tuple's signature; used to determine if previously generated code is
    /// up-to-date.
    /// @param targetDir The directoy into which the code should be generated
    void genCodeIfNeeded(TupleModel const& tuple,
                         std::string const& name,
                         std::string const& signature,
                         boost::filesystem::path const& targetDir) const;

    /// Generate code for the given enum if it has not yet been generated or is out-of-date.
    /// @param enumr The enum we are generating code for
    /// @param name Name of the artifact to be generated
    /// @param signature The enum's signature; used to determine if previously generated code is
    /// up-to-date.
    /// @param targetDir The directoy into which the code should be generated
    void genCodeIfNeeded(EnumerationModel const& enumr,
                         std::string const& name,
                         std::string const& signature,
                         boost::filesystem::path const& targetDir) const;

    /// Generate code for the given xml type if it has not yet been generated or is out-of-date.
    /// @param xml The xml we are generating code for
    /// @param name Name of the artifact to be generated
    /// @param signature The xml's signature; used to determine if previously generated code is
    /// up-to-date.
    /// @param targetDir The directoy into which the code should be generated
    void genCodeIfNeeded(XMLModel const& xml,
                         std::string const& name,
                         std::string const& signature,
                         boost::filesystem::path const& targetDir) const;

    /// Generate code for the given operator instance
    /// @param inst The operator instance
    /// @param name Name of the artifact to be generated
    /// @param signature The model signature used to validate the generated source
    /// @param targetDir The root of the heriarchy in which operator code is generated in
    /// @param useIncrementalGen Only update the operator code if it differs from the
    /// @param resDir A path to a directory containing a perl module to be included, or an empty
    /// path if there is none code generated previously, or if no operator code exists.
    void genCodeIfNeeded(OperatorInstanceModel const& inst,
                         std::string const& name,
                         std::string const& signature,
                         boost::filesystem::path const& targetDir,
                         bool useIncrementalGen,
                         boost::filesystem::path const& resDir) const;

    /// Generate code for the standalone model
    /// @param model The standalone model
    /// @param name Name of the artifact to be generated
    /// @param signature The model signature used to validate the generated source
    /// @param targetDir The directory into which the standalone code should be generated.
    void genCodeIfNeeded(StandaloneModel const& model,
                         std::string const& name,
                         std::string const& signature,
                         boost::filesystem::path const& targetDir) const;

    /// Given the fully qualified name of an operator, compute the base name of the code generators
    /// @param inst The operator for which we are computing the name
    /// @param isJavaOp true is returned if this is the java operaotr, otherwise false
    /// @return Returns the base name for the given operator
    std::string computeOperatorTemplateBaseName(OperatorInstanceModel const& inst,
                                                bool& isJavaOp) const;

    /// Verify a given Operator Instance invocation
    /// @param model Operator Instance Mode to be used
    /// @param om Operator Model being checked
    /// @param operatorModelDirectory The directory where the operator model XML resides.  Used to
    /// find the verification module.
    /// @return 'true' if code was successfully verified
    bool verify(OperatorInstanceModel const& model,
                Operator::OperatorModel const& om,
                boost::filesystem::path const& operatorModelDirectory) const;

  private:
    /// Check if a code generator is up-to-date
    /// @param model The model being code gen'd
    /// @param baseName The name of the code generator (minus the _h.pm and _cpp.pm)
    /// @param resDir A path to a directory containing a perl module to be included, or an empty
    /// path if there is none
    void checkIfGeneratorIsUpToDate(Model model,
                                    std::string const& baseName,
                                    boost::filesystem::path const& resDir) const;

    /// Check if the code generator for a tuple model is up to date
    /// @param model An instance of a model for which we want to check the generator
    void checkIfGeneratorIsUpToDate(TupleModel const& model) const;

    /// Check if the code generator for a enumeration model is up to date
    /// @param model An instance of a model for which we want to check the generator
    void checkIfGeneratorIsUpToDate(EnumerationModel const& model) const;

    /// Check if the code generator for a enumeration model is up to date
    /// @param model An instance of a model for which we want to check the generator
    void checkIfGeneratorIsUpToDate(XMLModel const& model) const;

    /// Check if the code generator for a standalone model is up to date
    /// @param model An instance of a model for which we want to check the generator
    void checkIfGeneratorIsUpToDate(StandaloneModel const& model) const;

    /// Check if the code generator for an operator instance model is up to date
    /// @param model An instance of a model for which we want to check the generator
    /// @param resDir A path to a directory containing a perl module to be included, or an empty
    /// path if there is none
    void checkIfGeneratorIsUpToDate(OperatorInstanceModel const& model,
                                    boost::filesystem::path const& resDir) const;

    /// Creates a code generator for the specified model
    /// @param model The type of code generator we are creating
    /// @param baseName The base filename of the code generator we are creating.
    /// @param resDir A path to a directory containing a perl module to be included, or an empty
    /// path if there is none
    void createCodeGenerator(Model model,
                             std::string const& baseName,
                             boost::filesystem::path const& resDir) const;

    /// Generate code for the given XMLized model
    /// @param generator The fully qualified name of the code generator (w/o the .pm suffix) to run
    /// @param xmlModel The name of a file into which to generate the xml
    /// @param name Name of the artifact that is being generated
    /// @param baseFileName base file name of the target file
    /// @param signature The model signature that is written into the generated code to verify  that
    /// it is up-to-date
    /// @param target The file into which the code should be generated
    /// @param loc The source location of the operator we are generating code for, or 0 if there is
    /// no location.
    /// @param useIncrementalGen Only update the operator code if it differs from the
    /// code generated previously, or if no operator code exists.
    /// @return 'true' if code was successfully generated
    bool genCode(boost::filesystem::path const& generator,
                 boost::filesystem::path const& xmlModel,
                 std::string const& name,
                 std::string const& baseFileName,
                 std::string const& signature,
                 boost::filesystem::path const& target,
                 SourceLocation const& loc,
                 bool useIncrementalGen) const;

    /// Generate code for the given model
    /// @param model The model for which we are generating code
    /// @param uri The URI to be embedded in the XML
    /// @param name The name of the object we are generating code for
    /// @param baseFileName base file name of the target file
    /// @param signature The unique signature string to be written into the file
    /// @param generatorBase A string which identifies the type of generator being run
    /// @param headerGenerator The header file generator
    /// @param cppGenerator The cpp file generator
    /// @param headerName The name of the header file we are generating
    /// @param cppName The name of the cpp file we are generating
    /// @param xml The name of the xml file for the model
    /// @param loc A source location
    /// @param useIncrementalGen If true the code generate the code and update the source
    /// code generated previously, or if no operator code exists.
    template<class M,
             class ModelType,
             void (*parseFn)(std::ostream&,
                             ModelType const&,
                             ::xml_schema::namespace_infomap const&,
                             std::string const&,
                             xml_schema::flags)>
    inline void genCodeIfNeeded(M const& model,
                                const char* uri,
                                std::string const& name,
                                std::string const& baseFileName,
                                std::string const& signature,
                                std::string const& generatorBase,
                                boost::filesystem::path const& headerGenerator,
                                boost::filesystem::path const& cppGenerator,
                                boost::filesystem::path const& headerName,
                                boost::filesystem::path const& cppName,
                                boost::filesystem::path const& xml,
                                SourceLocation const& loc,
                                bool useIncrementalGen) const;

    /// Determines if a given file needs to be generated
    /// @param generator The fully qualified name of the code generator to run
    /// @param fileName The name of the file we are generating code into.
    /// @param signature The signature for the code being generated.
    /// @return Returns true if the file needs to be generated, otherwise false.
    bool needsToBeGenerated(boost::filesystem::path const& generator,
                            boost::filesystem::path const& fileName,
                            std::string const& signature) const;

    /// Deterines if a give code generator needs to be regenerated
    /// @param gen The fully qualified name of the code generator to check
    /// @param templt The template for the code generator.
    /// @param resDir A path to a directory containing a perl module to be included, or an empty
    /// path if there is none
    /// @return Returns true if the generator needs to be generated, otherwise false.
    bool checkDependencies(boost::filesystem::path const& gen,
                           boost::filesystem::path const& templt,
                           boost::filesystem::path const& resDir) const;

    /// Utility routine for running a perl script and handling the return values or exceptions.
    int runPerlScript(std::string script, std::vector<std::string> const& args) const;

    /// Returns the base name of a generator or source file for the given model
    /// @param model The model for which we want the base name
    /// @return Returns the base name of a generator or source file for the given model
    static const char* baseName(Model model);

    /// Returns the string that needs to be passed to the spl-mixedmode-processor script to
    /// specify which model we are creating a code generator for.
    /// @param model The model for which we want the string
    /// @return Returns the string that needs to be passed to the spl-mixedmode-processor.
    static const char* scriptArg(Model model);

    static boost::shared_ptr<SPL::Operator::Instance::Parameter> getParameterByName(
      const OperatorInstanceModel& oim,
      std::string paramName);

    static void checkWrite(std::ofstream& s, const std::string& filename, bool deleteOnError)
    {
        s << std::flush;
        if (!s) {
            if (deleteOnError) {
                boost::filesystem::remove(filename);
            }
            SysOut::die(SPL_CORE_ERROR_WRITING_TO_FILE(filename, strerror(errno)));
        }
    }

    boost::filesystem::path templateDir_;
    boost::filesystem::path scriptDir_;
    boost::filesystem::path internalScriptDir_;
    typedef std::tr1::unordered_set<std::string> SeenSet;
    mutable SeenSet seen_;
};
};

#endif /* SPL_CODEGEN_HELPER_H */
