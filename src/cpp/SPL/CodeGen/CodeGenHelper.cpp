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

#include <SPL/CodeGen/CodeGenHelper.h>

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/EnumerationModelImpl.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/OperatorInstanceModelImpl.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/StandaloneModelImpl.h>
#include <SPL/Core/TupleModelImpl.h>
#include <SPL/Core/TupleTypeModelImpl.h>
#include <SPL/Core/Visitor.h>
#include <SPL/Core/XMLModelImpl.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Utility/JNIUtils.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <SPL/Utility/Utility.h>
#include <UTILS/SupportFunctions.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/static_assert.hpp>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Enumeration_;
using namespace SPL::XML_;
using namespace SPL::Tuple_;
using namespace SPL::Standalone;
using namespace SPL::Operator::Instance;

namespace bf = boost::filesystem;

template class Singleton<CodeGenHelper>; // Implicitly instantiate the Singleton code here

static const char* scriptArgs[] = { "EnumerationModel", "OperatorInstanceModel",
                                    "Script",           "StandaloneModel",
                                    "TupleModel",       "XMLModel" };
BOOST_STATIC_ASSERT(CodeGenHelper::NumModels == sizeof(scriptArgs) / sizeof(const char*));

const char* CodeGenHelper::scriptArg(CodeGenHelper::Model model)
{
    return scriptArgs[model];
}

CodeGenHelper::CodeGenHelper()
  : Singleton<CodeGenHelper>(*this)
  , templateDir_(
      CompilerConfiguration::instance().getStringValue(CompilerConfiguration::TemplateDir))
  , scriptDir_(CompilerConfiguration::instance().getStringValue(CompilerConfiguration::ScriptDir))
  , internalScriptDir_(
      CompilerConfiguration::instance().getStringValue(CompilerConfiguration::InternalScriptDir))
{
    bf::path helper1 = scriptDir_ / "SPL" / "CodeGen.pm";
    bf::path helper2 = internalScriptDir_ / "SPL" / "CodeGenHelper.pm";

    // Check that the code gen helper modules exist.  Do this here so it's done once.
    if (!bf::exists(helper1)) {
        SysOut::die(SPL_CORE_CODE_GEN_SYS_TEMPLATE_MISSING(helper1.string()));
    }
    if (!bf::exists(helper2)) {
        SysOut::die(SPL_CORE_CODE_GEN_SYS_TEMPLATE_MISSING(helper2.string()));
    }
}

bool isOlderThan(bf::path const& gen, bf::path const& templt)
{
    if (bf::last_write_time(gen) < bf::last_write_time(templt)) {
        SPLDBG("Last write time for " << gen.string() << " is older than " << templt.string(),
               Debug::TraceCGDepCheck);
        return true;
    }

    SPLDBG("Last write time for " << gen.string() << " is newer than " << templt.string(),
           Debug::TraceCGDepCheck);
    return false;
}

void CodeGenHelper::checkIfGeneratorIsUpToDate(EnumerationModel const&) const
{
    static bool codeGeneratorChecked = false;
    if (codeGeneratorChecked) {
        return;
    }
    checkIfGeneratorIsUpToDate(Enum, (templateDir_ / "enum").string(), "");
    codeGeneratorChecked = true;
}

void CodeGenHelper::checkIfGeneratorIsUpToDate(XMLModel const&) const
{
    static bool codeGeneratorChecked = false;
    if (codeGeneratorChecked) {
        return;
    }
    checkIfGeneratorIsUpToDate(Enum, (templateDir_ / "xml").string(), "");
    codeGeneratorChecked = true;
}

void CodeGenHelper::checkIfGeneratorIsUpToDate(TupleModel const& tuple) const
{
    static bool codeGeneratorChecked = false;
    if (codeGeneratorChecked) {
        return;
    }
    checkIfGeneratorIsUpToDate(
      Tuple, (templateDir_ / (tuple.getFacade() ? "facade_tuple" : "tuple")).string(), "");
    codeGeneratorChecked = true;
}

void CodeGenHelper::checkIfGeneratorIsUpToDate(StandaloneModel const&) const
{
    static bool codeGeneratorChecked = false;
    if (codeGeneratorChecked) {
        return;
    }
    checkIfGeneratorIsUpToDate(Standalone, (templateDir_ / "standalone").string(), "");
    codeGeneratorChecked = true;
}

void CodeGenHelper::checkIfGeneratorIsUpToDate(OperatorInstanceModel const& model,
                                               bf::path const& resDir) const
{
    string kind = model.getContext().getKind();

    SeenSet::const_iterator it = seen_.find(kind);
    if (it != seen_.end()) {
        return;
    }

    // We haven't seen this one yet

    // Java operators never have resources, even if they appear to.  This can be caused by another
    // toolkit that has a java operator and also has resources.  The side effect is an attempt to
    // rebuild the code generator for the standard toolkit's JavaOp, which is bad.
    bool isJavaOp;
    string baseName1 = computeOperatorTemplateBaseName(model, isJavaOp);
    checkIfGeneratorIsUpToDate(OperatorInstance, baseName1, (isJavaOp) ? "" : resDir);
    seen_.insert(kind);
}

void CodeGenHelper::checkIfGeneratorIsUpToDate(CodeGenHelper::Model model,
                                               const string& inbaseName,
                                               bf::path const& resDir) const
{
    bf::path hGen = inbaseName + "_h.pm";
    bf::path cppGen = inbaseName + "_cpp.pm";

    bf::path hGenTemplate = inbaseName + "_h.cgt";
    bf::path cppGenTemplate = inbaseName + "_cpp.cgt";

    // Ensure all the templates exist, otherwise we can't continue
    if (!bf::exists(hGenTemplate)) {
        SysOut::die(SPL_CORE_CODE_GEN_TEMPLATE_MISSING(hGenTemplate.string()));
    }
    if (!bf::exists(cppGenTemplate)) {
        SysOut::die(SPL_CORE_CODE_GEN_TEMPLATE_MISSING(cppGenTemplate.string()));
    }

    // Check that the code generators exist and that they are newer than the templates, any helper
    // modules, and any included files
    if (!bf::exists(hGen) || !bf::exists(cppGen) || checkDependencies(hGen, hGenTemplate, resDir) ||
        checkDependencies(cppGen, cppGenTemplate, resDir)) {

        // We don't pass the .pm suffix...The code generator figures that out itself
        SysOut::verboseln(SPL_CORE_CREATE_CODE_GEN(inbaseName), cout);
        createCodeGenerator(model, hGen.string().substr(0, hGen.string().size() - 3), resDir);
        createCodeGenerator(model, cppGen.string().substr(0, cppGen.string().size() - 3), resDir);
    }
}

bool CodeGenHelper::checkDependencies(bf::path const& gen,
                                      bf::path const& templt,
                                      bf::path const& resDir) const
{
    SPLDBG("Checking dependencies for " << gen.string(), Debug::TraceCGDepCheck);
    // If the generator is older than the template we need to regenerate it
    if (isOlderThan(gen, templt)) {
        return true;
    }

    // If the message catalogue newer?
    if (!resDir.empty() && bf::exists(resDir)) {
        // Look for .pm files
        static bf::directory_iterator end_itr;
        for (bf::directory_iterator it(resDir); it != end_itr; it++) {
            if (!bf::exists(*it)) {
                continue;
            }
            if (bf::is_directory(*it)) {
                continue;
            }
            const string file = it->path().leaf().string();
            if (file.length() < 3) {
                continue;
            }
            if (file.substr(file.length() - 3) != ".pm") {
                continue;
            }

            // Is it newer than me?
            if (isOlderThan(gen, *it)) {
                return true;
            }
        }
    }

    // The generator can also have a list of dependencies at the beginning of the
    // file.  Check to see if any of those are newer than the generator.
    ifstream in(gen.string().c_str());
    string line;
    getline(in, line);
    const char str[] = "# SPL_CGT_INCLUDE: ";
    while (!line.compare(0, sizeof(str) - 1, str)) {
        bf::path fqn = gen.branch_path() / line.substr(sizeof(str) - 1);
        SPLDBG("Checking " << fqn.string(), Debug::TraceCGDepCheck);
        if (isOlderThan(gen, fqn)) {
            SPLDBG(fqn.string() << " is newer...rebuilding", Debug::TraceCGDepCheck);
            return true;
        }
        getline(in, line);
    }

    return false;
}

int CodeGenHelper::runPerlScript(string script, vector<string> const& args) const
{
    int rc = 0;
    try {
        rc = ProcessLauncher::runProcess(script, args);
    } catch (SPLCompilerException& e) {
        SysOut::die(SPL_CORE_CODE_GEN_FAILURE(e.getExplanation()), SourceLocation(script));
    }
    return rc;
}

void CodeGenHelper::createCodeGenerator(CodeGenHelper::Model model,
                                        string const& inbaseName,
                                        bf::path const& resDir) const
{
    // Do we need a path to the script here?
    bf::path sdir = internalScriptDir_ / "spl-mixedmode-processor";

    // Convert the model type into a string that the generator script will recongnize
    string type = scriptArg(model);

    vector<string> args;
    args.push_back("-t");
    args.push_back(type);
    args.push_back(inbaseName);
    if (!resDir.empty()) {
        args.push_back("--resdir");
        args.push_back(resDir.string());
    }

    int rc = runPerlScript(sdir.string(), args);
    if (rc != 0) {
        if (rc == 2) { // A return code of 2 indicates that an interrupt was suppressed during the
                       // generation...respect it now
            SysOut::errorln(SPL_CORE_INTERRUPTED, SourceLocation(), cerr);
            exit(EXIT_FAILURE);
        } else {
            SysOut::die(SPL_CORE_CODE_GENERATOR_FAILURE, SourceLocation(inbaseName + ".cgt"));
        }
    }
}

ParameterPtr CodeGenHelper::getParameterByName(const OperatorInstanceModel& oim,
                                               std::string paramName)
{
    SPLDBG("Getting parameter " << paramName, Debug::TraceOperatorVerify);
    const std::vector<ParameterPtr>& parameters = oim.getParameters();

    for (std::vector<ParameterPtr>::const_iterator it = parameters.begin(); it != parameters.end();
         it++) {
        if ((*it)->getName() == paramName) {
            return (*it);
        }
    }

    ParameterPtr p;
    return p;
}

bool CodeGenHelper::verify(OperatorInstanceModel const& model,
                           Operator::OperatorModel const& om,
                           bf::path const& operatorModelDirectory) const
{
    SPLDBG("Verifying operator " << operatorModelDirectory.c_str(), Debug::TraceOperatorVerify);

    // Were do we generate this?
    bf::path cdir =
      CompilerConfiguration::instance().getStringValue(CompilerConfiguration::OutputDir);
    bf::path targetDir = cdir / "src" / "operator";
    Utility::createDirectory(targetDir);

    // Where is the verifier?
    bf::path verifier = operatorModelDirectory;
    verifier /= om.getContext().getVerificationModule();

    // render the model in XML
    auto_ptr<xmlns::prod::streams::spl::operator_::instance::operatorInstanceModelType> modelXml =
      model.toXsdInstance();

    xml_schema::namespace_infomap map;
    map[""].name = "http://www.ibm.com/xmlns/prod/streams/spl/operator/instance";
    map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
    map["tt"].name = "http://www.ibm.com/xmlns/prod/streams/spl/tupleType";
    map["an"].name = "http://www.ibm.com/xmlns/prod/streams/spl/annotation";

    bf::path path = targetDir;
    Context const& context = model.getContext();
    string const& fileName = context.getClass();

    // className has the form [::]*.name.  Each '::' delimited segment except the last forms a
    // directory in the src hierarchy routed in src/operator
    size_t start = 0, end = fileName.find("::", 0);
    while (end != string::npos) {
        path = path / fileName.substr(start, end - start);
        Utility::createDirectory(path);
        start = end + 2;
        end = fileName.find("::", start);
    }
    string fileNameBase = fileName.substr(start, end - start);
    bf::path xml = path / (fileNameBase + ".xml");

    ofstream of(xml.string().c_str());
    if (of.fail()) {
        SysOut::die(SPL_CORE_CANNOT_CREATE_FILE(xml.string(), RuntimeUtility::getErrorNoStr()));
    }
    xmlns::prod::streams::spl::operator_::instance::operatorInstanceModel(of, *modelXml, map);
    of.close();

    if (!om.isJavaOperator()) {
        SPLDBG("Non Java operator.", Debug::TraceOperatorVerify);
        // Run the verifier
        bf::path sdir = internalScriptDir_ / "spl-verification-driver";
        vector<string> args;
        args.push_back("--verifier");
        args.push_back(verifier.string());
        args.push_back("--model");
        args.push_back(xml.string());
        int rc = runPerlScript(sdir.string(), args);
        if (rc != 0) {
            if (rc == 2) { // A return code of 2 indicates that an interrupt was suppressed during
                           // the generation...respect it now
                SysOut::errorln(SPL_CORE_INTERRUPTED, SourceLocation(), cerr);
                exit(EXIT_FAILURE);
                return false;
            } else if (rc == SPL_USER_ERROR) {
                // The verifier reported finding and reporting a user error.
                // Emit a message here and continue.
                // This can only be returned during the verification of an operator.
                SysOut::errorln(SPL_CORE_VERIFICATION_USER_ERROR(fileName));
                return false;
            } else {
                SysOut::die(SPL_CORE_VERIFICATION_FAILURE(verifier.string() + ".pm"));
                return false;
            }
        }

        return true;
    }

    else {
        SPLDBG("Java operator.", Debug::TraceOperatorVerify);
        // mimic what the verify module would have previously done, but
        // instead of invoking perl which will then invoke the JVM, replicate here what
        // the perl verify module for JavaOp would have done, and use a pooled JVM to
        // call OperatorCompileTime for this operator instance

        // my $outDir = $model->getContext()->getOutputDirectory();
        string outDir = context.getOutputDirectory();
        SPLDBG("outDir = " << outDir, Debug::TraceOperatorVerify);

        // my $unlinkFile = 1;
        int unlinkFile = 0;

        //    	 $unlinkFile = 0 if ($ENV{STREAMS_SAVE_JAVA_PARAM_FILE});
        SPLDBG("Getting STREAMS_SAVE_JAVA_PARAM_FILE environment variable.",
               Debug::TraceOperatorVerify);
        unlinkFile = !(Distillery::get_environment_variable("STREAMS_SAVE_JAVA_PARAM_FILE", 1));

        //    	 my $tmp = File::Temp->new( TEMPLATE => 'javaXXXXX',
        //    	                        DIR => $outDir,
        //    	                        UNLINK => $unlinkFile,
        //    	                        SUFFIX => '.txt');
        //    	  binmode($tmp, ":utf8");
        boost::filesystem::path tempPath =
          boost::filesystem::path(outDir) / boost::filesystem::unique_path("java%%%%%.txt");

        SPLDBG("Opening temporary file " << tempPath.string().c_str(), Debug::TraceOperatorVerify);
        ofstream tmp;
        tmp.open(tempPath.string().c_str());

        if (!tmp.is_open()) {
            SysOut::die(SPL_CORE_ERROR_OPENING_FILE(tempPath.string(), strerror(errno)));
        }

        //
        //    	  open $SPL::CodeGen::OLDSTDOUT, ">&", "STDOUT"
        //    	    or SPLDie(1, SPL::Msg::CANT_DUP_STDOUT());
        // above 2 lines not required since we are not invoking external processes

        //
        //    	  # Add verbose modeCodeGenHelper
        //    	  print $tmp 'verboseMode=' . ($model->getContext()->isVerboseModeOn() ? 'true' :
        //    'false')  . "\n";
        SPLDBG("Writing verbose mode", Debug::TraceOperatorVerify);
        tmp << "verboseMode=";
        checkWrite(tmp, tempPath.string(), unlinkFile);

        SPLDBG("Getting verbose mode", Debug::TraceOperatorVerify);
        if (context.getVerbose()) {
            SPLDBG("Writing true", Debug::TraceOperatorVerify);
            tmp << "true\n";
            checkWrite(tmp, tempPath.string(), unlinkFile);

        } else {
            SPLDBG("Writing false", Debug::TraceOperatorVerify);
            tmp << "false\n";
            checkWrite(tmp, tempPath.string(), unlinkFile);
        }

        //    	  # Add the generated flag
        //    	  my $generated = $model->getParameterByName("generated");
        //    	  $generated = $generated ? $generated->getValueAt(0)->getSPLExpression() : 'false';
        //    	  print $tmp 'generated=' . $generated . "\n";
        SPLDBG("Getting generated parameter", Debug::TraceOperatorVerify);
        ParameterPtr generated = getParameterByName(model, "generated");
        SPLDBG("testing generated parameter", Debug::TraceOperatorVerify);
        if (generated) {
            SPLDBG("Got generated parameter", Debug::TraceOperatorVerify);
            std::string splExpr = generated->getValues()[0]->getSPLExpression();
            SPLDBG("Writing generated", Debug::TraceOperatorVerify);
            tmp << "generated=" << (splExpr != "" ? splExpr : "false") << "\n";
            checkWrite(tmp, tempPath.string(), unlinkFile);
        }

        else {
            SPLDBG("Did not find generated parameter", Debug::TraceOperatorVerify);
            SPLDBG("Writing generated=false", Debug::TraceOperatorVerify);
            tmp << "generated=false\n";
            checkWrite(tmp, tempPath.string(), unlinkFile);
        }

        //    	  my $operclass = $model->getParameterByName("className")->getValueAt(0)->getSPLExpression();
        //    	  print $tmp 'className=' . $operclass . "\n";
        ParameterPtr className = getParameterByName(model, "className");
        std::string classNameExpr;
        if (className) {
            classNameExpr = className->getValues()[0]->getSPLExpression();
            SPLDBG("Writing class name", Debug::TraceOperatorVerify);
            tmp << "className=" << classNameExpr << "\n";
            checkWrite(tmp, tempPath.string(), unlinkFile);
        }

        else {
            SPLDBG("Did not find class name parameter", Debug::TraceOperatorVerify);
        }
        //
        //    	  # Add the classLibrary values.
        //    	  {
        //    	    my $clparam=$model->getParameterByName("classLibrary");

        SPLDBG("Processing classLibrary parameter", Debug::TraceOperatorVerify);
        ParameterPtr clParam = getParameterByName(model, "classLibrary");
        if (clParam) {
            std::vector<ExpressionPtr>& values = clParam->getValues();

            //    	    for (my $pi = 0; $pi < $clparam->getNumberOfValues(); $pi++) {

            for (size_t pi = 0; pi < values.size(); pi++) {

                //    	      my $clexpr = $clparam->getValueAt($pi);
                ExpressionPtr clexpr = values[pi];

                bool isComplex = false;

                //    	      my $cte = undef;
                //    	      $cte = SPL::CodeGen::compileTimeExpression($model, $clexpr) if ($clexpr->hasSPLExpressionTree());
                //    	      my $isComplex;

                if (clexpr) {

                    // get the compile time evaluated expression, if possible
                    SPLDBG("Getting compile time expression", Debug::TraceOperatorVerify);
                    SPLDBG("Expression = " << clexpr->getSPLExpression(),
                           Debug::TraceOperatorVerify);
                    std::string result = clexpr->getCompileTimeEvaluatedExpression();

                    boost::filesystem::path classLibPath =
                      boost::filesystem::path(result).normalize();

                    SPLDBG("result = " << classLibPath.c_str(), Debug::TraceOperatorVerify);
                    if (!result.empty()) {
                        tmp << "classLibrary." << pi << "="
                            << "\"" << classLibPath.c_str() << "\""
                            << "\n";
                        checkWrite(tmp, tempPath.string(), unlinkFile);
                    }

                    else {
                        // couldn't evaluate
                        //    	          print $tmp 'classLibrary.' . $pi . '=' . $clexpr->getSPLExpression() . "\n";
                        //    	          $isComplex = $clexpr->hasFunctions() || $clexpr->hasSideEffects() ? "true" : "false";
                        SPLDBG("Expression not evaluatable: " << clexpr->getSPLExpression(),
                               Debug::TraceOperatorVerify);
                        tmp << "classLibrary." << pi << "=" << clexpr->getSPLExpression() << "\n";
                        checkWrite(tmp, tempPath.string(), unlinkFile);
                        isComplex = clexpr->getHasFunctions() || clexpr->getHasSideEffects();
                    }
                }

                //    	      # Pass through if the SPL expression (Attribute free) has
                //    	      # functions or other complexities we won't resolve in Java.
                //    	      print $tmp 'classLibrary.' . $pi . '.complex=' . $isComplex . "\n";
                //    	    }
                //    	  }
                tmp << "classLibrary." << pi << ".complex=" << (isComplex ? "true" : "false")
                    << "\n";
                checkWrite(tmp, tempPath.string(), unlinkFile);
            }
        }

        else {
            SPLDBG("Did not find classLibrary parameter", Debug::TraceOperatorVerify);
        }
        //
        //    	  # All the parameters (names only).
        //    	  #foreach my $param (@{$model->getParameters()}) {
        //    	  #    print $tmp "parameter.". $param->getName()  . "\n";
        //    	  #}

        SPLDBG("Processing parameters", Debug::TraceOperatorVerify);
        vector<ParameterPtr> parameters = model.getParameters();
        for (vector<ParameterPtr>::iterator it = parameters.begin(); it != parameters.end(); it++) {
            tmp << "parameter." << (*it)->getName() << "\n";
            checkWrite(tmp, tempPath.string(), unlinkFile);
        }
        //
        //    	  # Description of the input ports
        //    	  print $tmp 'inputPortCount=' . $model->getNumberOfInputPorts() . "\n";
        SPLDBG("Processing input ports", Debug::TraceOperatorVerify);
        vector<InputPortPtr> inputPorts = model.getInputPorts();
        tmp << "inputPortCount=" << inputPorts.size() << "\n";
        checkWrite(tmp, tempPath.string(), unlinkFile);
        int iPortIndex = 0;

        //    	  foreach my $inport (@{$model->getInputPorts()}) {
        for (vector<InputPortPtr>::iterator it = inputPorts.begin(); it != inputPorts.end(); it++) {
            //    	    # Description of the port
            //    	    print $tmp 'inputPort.' . $inport->getIndex() . '=' .
            //    	         $inport->getNumberOfAttributes() . "\n";
            InputPortPtr inputPort = (*it);

            vector<InputAttributePtr> inputAttributes = inputPort->getAttributes();
            tmp << "inputPort." << iPortIndex << "=" << inputAttributes.size() << "\n";
            checkWrite(tmp, tempPath.string(), unlinkFile);
            int attribIndex = 0;

            for (vector<InputAttributePtr>::iterator attIt = inputAttributes.begin();
                 attIt != inputAttributes.end(); attIt++) {

                //    	    # Description of the port's attributes
                //    	    for (my $ai = 0; $ai < $inport->getNumberOfAttributes(); $ai++) {
                //    	      my $attr = $inport->getAttributeAt($ai);
                //
                //    	      print $tmp 'inputPort.' . $inport->getIndex() . '.' .
                //    	           $ai . '=' . $attr->getSPLType() . '-' . $attr->getName() . "\n";
                //    	    }
                InputAttributePtr attr = (*attIt);

                tmp << "inputPort." << iPortIndex << "." << attribIndex++ << "="
                    << attr->getSPLType() << "-" << attr->getName() << "\n";
                checkWrite(tmp, tempPath.string(), unlinkFile);
            }

            //    	    if ($inport->hasWindow()) {
            if (inputPort->hasWindow()) {

                //    	       my $param = 'window.' . $inport->getIndex();
                string param("window." + iPortIndex);
                //    	       my $win = $inport->getWindow();
                Window& window = inputPort->getWindow();

                //    	       my $wpi = 0;
                int wpi = 0;

                //    	       print $tmp $param . '.' . $wpi++ . '=' . $win->getWindowType() . "\n";
                tmp << param << "." << wpi++ << "="
                    << (window.hasEventTimePolicy() ? 3                             /* EVENT_TIME */
                                                    : window.hasTriggerPolicy() ? 2 /* SLIDING */
                                                                                : 1) /* TUMBLING */
                    << "\n";
                checkWrite(tmp, tempPath.string(), unlinkFile);
                // tmp << param << "." << wpi++ << "=" << window.getTriggerPolicy().getStyle().getValue() << "\n";

                //    	       print $tmp $param . '.' . $wpi++ . '=' . $win->isPartitioned() . "\n";
                tmp << param << "." << wpi++ << "=" << window.getPartitioned() << "\n";
                checkWrite(tmp, tempPath.string(), unlinkFile);

                //    	       my $evict = $win->getEvictionPolicyType();
                //    	       print $tmp $param . '.' . $wpi++ . '=' . $evict . "\n";

                if (window.hasEvictionPolicy()) {
                    tmp << param << "." << wpi++ << "="
                        << window.getEvictionPolicy().getStyle().getValue() << "\n";
                    checkWrite(tmp, tempPath.string(), unlinkFile);

                    //    	       print $tmp $param . '.' . $wpi++ . '=' . $win->getEvictionPolicyAttribute()->getSPLExpression() . "\n" if ($evict == $SPL::Operator::Instance::Window::DELTA);
                    if (window.getEvictionPolicy().getStyle().getValue() == WindowStyle::delta) {
                        tmp << param << "." << wpi++ << "="
                            << window.getEvictionPolicy().getAttribute().getSPLExpression() << "\n";
                        checkWrite(tmp, tempPath.string(), unlinkFile);
                    }

                    //    	       # eviction policy size hard coded for compile time
                    //    	       print $tmp $param . '.' . $wpi++ . '=' . "1\n" unless ($evict == $SPL::Operator::Instance::Window::PUNCT);
                    if (window.getEvictionPolicy().getStyle().getValue() != WindowStyle::punct) {
                        tmp << param << "." << wpi++ << "=1\n";
                        checkWrite(tmp, tempPath.string(), unlinkFile);
                    }
                }

                //
                //    	      if ($win->isSliding()) {
                if (window.hasTriggerPolicy()) { // SLIDING
                    //    	         my $trigger = $win->getTriggerPolicyType();
                    WindowStyle trigger = window.getTriggerPolicy().getStyle().getValue();

                    //    	         print $tmp $param . '.' . $wpi++ . '=' . $trigger . "\n";
                    tmp << param << "." << wpi++ << "=" << trigger << "\n";
                    checkWrite(tmp, tempPath.string(), unlinkFile);

                    //    	         print $tmp $param . '.' . $wpi++ . '=' . $win->getTriggerPolicyAttribute()->getSPLExpression() ."\n" if ($trigger == $SPL::Operator::Instance::Window::DELTA);
                    if (trigger == WindowStyle::delta) {
                        tmp << param << "." << wpi++ << "="
                            << window.getTriggerPolicy().getAttribute().getSPLExpression() << "\n";
                        checkWrite(tmp, tempPath.string(), unlinkFile);
                    }

                    //    	        # trigger policy size hard coded for compile time.
                    //    	         print $tmp $param . '.' . $wpi++ . "=0\n";
                    tmp << param << "." << wpi++ << "=0\n";
                    checkWrite(tmp, tempPath.string(), unlinkFile);

                    //    	      }
                }
                if (window.hasEventTimePolicy()) {
                    // TODO complete this.	Apparently this
                    // is needed for Java operators.
                    //
                    // tmp << param << "." << wpi++ << "=" << window.getEventTimePolicy().getAttribute().getSPLExpression() << "\n";
                    // checkWrite(tmp, tempPath.string(), unlinkFile);
                    //
                }
                //
                //    	      if ($win->hasPartitionEvictionPolicy()) {
                if (window.hasPartitionEvictionPolicy()) {
                    //    	         my $partitionEvictPolicy = $win->getPartitionEvictionPolicyKind();
                    //  	           print $tmp $param . '.' . $wpi++ . '=' . $partitionEvictPolicy . "\n";
                    tmp << param << "." << wpi++ << "="
                        << window.getPartitionEvictionPolicy()->getPartitionEvictionKind() << "\n";
                    checkWrite(tmp, tempPath.string(), unlinkFile);
                    //
                    //    	           # partition policy size hard coded for compile time
                    //    	           print $tmp $param . '.' . $wpi++ . "=1\n";
                    tmp << param << "." << wpi++ << "=1\n";
                    checkWrite(tmp, tempPath.string(), unlinkFile);
                    //    	         }
                }

                //    	    }
            }
            //    	  }
            iPortIndex++;
        }
        //
        //    	  # Description of the output ports
        //    	  print $tmp 'outputPortCount=' . $model->getNumberOfOutputPorts() . "\n";
        SPLDBG("Processing output ports", Debug::TraceOperatorVerify);
        vector<OutputPortPtr> outputPorts = model.getOutputPorts();
        tmp << "outputPortCount=" << outputPorts.size() << "\n";
        checkWrite(tmp, tempPath.string(), unlinkFile);
        int oPortIndex = 0;
        //
        //    	  foreach my $outport (@{$model->getOutputPorts()}) {
        for (vector<OutputPortPtr>::iterator it = outputPorts.begin(); it != outputPorts.end();
             it++) {
            //    	    # Description of the port
            //    	    print $tmp 'outputPort.' . $outport->getIndex() . '=' .
            //    	         $outport->getNumberOfAttributes() . "\n";
            OutputPortPtr outputPort = (*it);

            vector<OutputAttributePtr> outputAttributes = outputPort->getAttributes();
            tmp << "outputPort." << oPortIndex << "=" << outputAttributes.size() << "\n";
            checkWrite(tmp, tempPath.string(), unlinkFile);

            //    	    # Description of the port's attributes
            //    	    for (my $ai = 0; $ai < $outport->getNumberOfAttributes(); $ai++) {
            int attribIndex = 0;
            for (vector<OutputAttributePtr>::iterator attIt = outputAttributes.begin();
                 attIt != outputAttributes.end(); attIt++) {
                //    	      my $attr = $outport->getAttributeAt($ai);
                OutputAttributePtr attr = (*attIt);
                //
                //    	      print $tmp 'outputPort.' . $outport->getIndex() . '.' .
                //    	           $ai . '=' . $attr->getSPLType() . '-' . $attr->getName() . "\n";
                tmp << "outputPort." << oPortIndex << "." << attribIndex++ << "="
                    << attr->getSPLType() << "-" << attr->getName() << "\n";
                checkWrite(tmp, tempPath.string(), unlinkFile);
                //    	    }
            }

            //    	  }
            oPortIndex++;
        }
        //
        //    	  close($tmp);
        SPLDBG("Closing temporary file", Debug::TraceOperatorVerify);
        tmp.close();

        //
        //    	  my $javaHome = $ENV{'JAVA_HOME'};
        //    	  my $streamsInstall = $ENV{'STREAMS_INSTALL'};
        //    	  my $isIBMJava = 0;
        //    	  if ($javaHome =~ m/.*ibm.*/ || $javaHome =~ m/^$streamsInstall/) {
        //    	    $isIBMJava = 1;
        //    	  }
        SPLDBG("Getting JAVA_HOME and STREAMS_INSTALL env vars", Debug::TraceOperatorVerify);
        std::string javaHome = Distillery::get_environment_variable("JAVA_HOME", "");
        std::string streamsInstall = Distillery::get_environment_variable("STREAMS_INSTALL", "");

        std::vector<std::string> jvmArgs;
        //    	  my @jcga = ($javaHome . '/bin/java', '-cp');
        // above not needed as args... we'll startup a plain JVM and attach

        //    	  push(@jcga, $streamsInstall . '/lib/com.ibm.streams.operator.samples.jar');
        boost::filesystem::path verifyJarPath =
          boost::filesystem::path(streamsInstall) / "lib/com.ibm.streams.operator.samples.jar";
        jvmArgs.push_back(std::string("-Djava.class.path=") + verifyJarPath.normalize().c_str());

        //    	  push(@jcga, '-Dlog4j.configuration=com/ibm/streams/operator/internal/logtrace/log4j/log4j.properties');
        jvmArgs.push_back("-Dlog4j.configuration=com/ibm/streams/operator/internal/logtrace/log4j/"
                          "log4j.properties");

        //    	  push(@jcga, '-Xquickstart') if ($isIBMJava);
        //    	  push(@jcga, '-Xshareclasses:name=com.ibm.streams.spl.sc_%u,nonfatal,silent') if ($isIBMJava);
        if (SPL::JNI::JVMEnv::isIBMJVM()) {
            jvmArgs.push_back("-Xquickstart");
            if (SPL::JNI::JVMEnv::useJvmCache()) {
                jvmArgs.push_back("-Xshareclasses:name=com.ibm.streams.spl.sc_%u,nonfatal,silent");
            }
        }

        // launch JVM
        SPLDBG("Getting JVM Factory", Debug::TraceOperatorVerify);

        // this will intialize the singleton instance in case we are the first time the instance is
        // asked for
        SPL::JNI::JVMFactory throwAwayFactory(streamsInstall, &jvmArgs);

        // this will give us the one instance to rule them all, which might or might not be the one
        // we just instantiated above
        SPL::JNI::JVMFactory& jvmFactory = SPL::JNI::JVMFactory::instance();

        SPLDBG("Getting JVM from factory", Debug::TraceOperatorVerify);
        JavaVM* jvm = jvmFactory.getJVM();

        //
        //    	  push(@jcga, 'com.ibm.streams.operator.internal.compile.OperatorCompileTime');
        std::string classNameStr = "com/ibm/streams/operator/internal/compile/OperatorCompileTime";
        std::list<std::string> argsToMain;

        //
        //    	  if ($generated eq 'true') {
        //    	      SPL::CodeGen::printlnVerbose("Generating Java template code for %s", $operclass);
        //    	  }
        //
        if (generated) {
            SysOut::verboseln(SPL_CORE_GENERATING_JAVA_TEMPLATE_CODE(classNameExpr));
        }

        //    	  # Ensure output from compile time checks do not corrupt the
        //    	  # C++ template code.
        //    	  # dependency-scan: ignore the following line
        //    	  my $output = `@jcga`;

        // invoke the verifier via JNI
        SPL::JNI::JVMEnv jvmEnv(jvm);
        JNIEnv* env = NULL;
        jvmEnv.getJNIEnv(&env);

        SPLDBG("Looking for verifier class", Debug::TraceOperatorVerify);
        jclass verifierClass = env->FindClass(classNameStr.c_str());
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            SysOut::die(SPL_CORE_STREAMS_ERROR("JVM setup error"));
        }

        // setup arguments for the verifier's main method
        SPLDBG("Setting up verifier args", Debug::TraceOperatorVerify);
        jobjectArray stringArray;
        jstring tmpString;
        jclass clsString;
        jint size = 3;
        jint index = 0;
        clsString = env->FindClass("java/lang/String");
        stringArray = env->NewObjectArray(size, clsString, 0);
        //    	  # First argument is the name of the file with the properties
        //    	  push(@jcga, $tmp->filename);
        tmpString = env->NewStringUTF(tempPath.string().c_str());
        env->SetObjectArrayElement(stringArray, index++, tmpString);
        //    	  # Then information to find the operatorInstanceModel XML file
        //    	  push(@jcga, $outDir);
        tmpString = env->NewStringUTF(outDir.c_str());
        env->SetObjectArrayElement(stringArray, index++, tmpString);
        //    	  push(@jcga, $model->getContext()->getClass());
        tmpString = env->NewStringUTF(context.getClass().c_str());
        env->SetObjectArrayElement(stringArray, index++, tmpString);

        // call the verifier
        SPLDBG("Getting verify(String[]) method", Debug::TraceOperatorVerify);
        jmethodID method =
          env->GetStaticMethodID(verifierClass, "verify", "([Ljava/lang/String;)I");
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            SysOut::die(SPL_CORE_STREAMS_ERROR("Error getting verify method"));
        }

        SPLDBG("Calling verify method", Debug::TraceOperatorVerify);
        int rc = env->CallStaticIntMethod(verifierClass, method, stringArray);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            SysOut::die(SPL_CORE_STREAMS_ERROR("Error calling verify method"));
            if (unlinkFile) {
                SPLDBG("Removing temp file", Debug::TraceOperatorVerify);
                boost::filesystem::remove(tempPath);
            }
        }

        SPLDBG("return code =" << rc, Debug::TraceOperatorVerify);
        //    	    if ($rc == 1) {GetStaticMethodIDGetStaticMethodID
        if (rc == 1) {
            //    	      SPL::CodeGen::errorln("Error generating Java template code", $model->getContext()->getSourceLocation());
            //    	   }
            SysOut::errorln(SPL_CORE_ERROR_GENERATING_JAVA_TEMPLATE_CODE,
                            context.getSourceLocation());
        }

        else if (rc == 2) {
            //    	   if ($rc == 2) {
            //    	      SPL::CodeGen::errorln("Operator invocation error for Java primitive %s",
            //    	           $model->getContext()->getKind(),
            //    	           $model->getContext()->getSourceLocation());
            //    	   }
            //

            SysOut::errorln(SPL_CORE_JAVA_OPERATOR_INVOCATION_ERROR(context.getKind()),
                            context.getSourceLocation());
        }

        // remove temp file
        if (unlinkFile) {
            SPLDBG("Removing temp file", Debug::TraceOperatorVerify);
            boost::filesystem::remove(tempPath);
        }
    }
    SPLDBG("Exiting verify normally", Debug::TraceOperatorVerify);
    return true;
}

bool CodeGenHelper::genCode(bf::path const& generator,
                            bf::path const& xmlModel,
                            string const& name,
                            string const& fileNameBase,
                            string const& signature,
                            bf::path const& target,
                            SourceLocation const& loc,
                            bool useIncrementalGen) const
{
    bf::path sdir = internalScriptDir_ / "spl-code-gen-driver";
    // At this point the generator has the .pm suffix on it.  That needs to be removed prior to
    // calling the perl code.
    string gen = generator.string();
    string genBase = gen.substr(0, gen.size() - 3);
    vector<string> args;
    if (useIncrementalGen) {
        args.push_back("--incremental");
    }
    args.push_back("--generator");
    args.push_back(genBase);
    args.push_back("--model");
    args.push_back(xmlModel.string());
    if (signature.size() < 4096) {
        args.push_back("--signature");
        args.push_back(signature);
    } else {
        args.push_back("--signature-file");
        bf::path sigfile = target.branch_path() / (fileNameBase + ".sig");
        ofstream sigf(sigfile.string().c_str());
        if (sigf.fail()) {
            SysOut::die(
              SPL_CORE_CANNOT_CREATE_FILE(sigfile.string(), RuntimeUtility::getErrorNoStr()));
        }
        sigf << signature << endl;
        args.push_back(sigfile.string());
    }
    args.push_back("--outfile");
    args.push_back(target.string());
    if (CompilerConfiguration::instance().getBooleanValue(
          CompilerConfiguration::CodeBeautification)) {
        args.push_back("-z");
        bf::path fmt = internalScriptDir_ / "spl-format-code";
        args.push_back(fmt.string());
    }

    // Since we are dealing with a mangled name the maximum we can extend the names length is 1
    // otherwise we risk exceeding the max filename length. See BaseMangler::setMaxMangledNameSize()
    bf::path backup(target.string() + "B");
    if (bf::exists(backup)) {
        try {
            bf::remove(backup);
        } catch (bf::filesystem_error const& e) {
            SysOut::errorln(SPL_CORE_CANNOT_REMOVE_FILE(backup.string(), e.what()));
        }
    }
    int rc = runPerlScript(sdir.string(), args);
    if (rc != 0) {
        try {
            bf::rename(target, backup);
        } catch (bf::filesystem_error const& e) {
            SysOut::errorln(
              SPL_CORE_CANNOT_RENAME_FILE(target.string(), backup.string(), e.what()));
        }
        if (rc == 2) { // A return code of 2 indicates that an interrupt was suppressed during the
                       // generation...respect it now
            SysOut::errorln(SPL_CORE_INTERRUPTED, SourceLocation(), cerr);
            exit(EXIT_FAILURE);
            return false;
        } else if (rc == SPL_USER_ERROR) {
            // The code generator reported finding and reporting a user error.
            // Emit a message here and continue.
            // This can only be returned during the generation of an operator.
            SysOut::errorln(SPL_CORE_CODE_GEN_USER_ERROR(name), loc);
            return false;
        } else {
            SysOut::die(SPL_CORE_CODE_GENERATOR_FAILURE, SourceLocation(genBase + ".cgt"));
            return false;
        }
    }
    return true;
}

bool CodeGenHelper::needsToBeGenerated(bf::path const& generator,
                                       bf::path const& fileName,
                                       string const& signature) const
{
    // If the file doesn't exist then, obviously, we need to generate it
    if (!exists(fileName)) {
        return true;
    }
    // If the file exists, is the signature correct?
    // The first line of the file contains the signature in the form // signature
    ifstream in(fileName.string().c_str());

    // const char* introducer = "// ";
    const unsigned extraBytes = 3; // for the introducer

    string sig;
    getline(in, sig);
    if (sig.size() != extraBytes + signature.size()) {
        return true;
    }
    if (strncmp(sig.data() + extraBytes, signature.data(), signature.size()) != 0) {
        return true;
    }

    // Finally, check the timestamps of generator vs. the generated code.  If the generator is newer
    // that the generated file, it needs to be generated.
    if (bf::last_write_time(generator) > bf::last_write_time(fileName)) {
        return true;
    }

    return false;
}

template<class M,
         class ModelType,
         void (*parseFn)(ostream&,
                         ModelType const&,
                         ::xml_schema::namespace_infomap const&,
                         string const&,
                         xml_schema::flags)>
void CodeGenHelper::genCodeIfNeeded(M const& model,
                                    const char* uri,
                                    string const& name,
                                    string const& fileNameBase,
                                    string const& signature,
                                    string const& generatorBase,
                                    bf::path const& headerGenerator,
                                    bf::path const& cppGenerator,
                                    bf::path const& headerName,
                                    bf::path const& cppName,
                                    bf::path const& xml,
                                    SourceLocation const& loc,
                                    bool useIncrementalGen) const
{

    // see if we need to generate anything
    bool headerNeedsToBeGenerated =
      (useIncrementalGen) ? true : needsToBeGenerated(headerGenerator, headerName, signature);
    bool cppNeedsToBeGenerated =
      (useIncrementalGen) ? true : needsToBeGenerated(cppGenerator, cppName, signature);

    if (!headerNeedsToBeGenerated) {
        SysOut::verboseln(SPL_CORE_SKIPPING_GEN_H(generatorBase, name), cout);
    }

    if (!cppNeedsToBeGenerated) {
        SysOut::verboseln(SPL_CORE_SKIPPING_GEN_CPP(generatorBase, name), cout);
        if (!headerNeedsToBeGenerated) {
            return;
        }
    }

    // render the model in XML
    auto_ptr<ModelType> modelXml = model.toXsdInstance();

    xml_schema::namespace_infomap map;
    map[""].name = uri;
    map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
    if (generatorBase == "operator") {
        map["tt"].name = "http://www.ibm.com/xmlns/prod/streams/spl/tupleType";
        map["an"].name = "http://www.ibm.com/xmlns/prod/streams/spl/annotation";
    }

    ofstream of(xml.string().c_str());
    if (of.fail()) {
        SysOut::die(SPL_CORE_CANNOT_CREATE_FILE(xml.string(), RuntimeUtility::getErrorNoStr()));
    }
    (*parseFn)(of, *modelXml, map, "", 0);
    of.close();

    // Gen the header file
    bool hGenned = true;
    if (headerNeedsToBeGenerated) {
        SysOut::verboseln(SPL_CORE_GEN_CODE_H(generatorBase, name), cout);
        hGenned = genCode(headerGenerator, xml, name, fileNameBase, signature, headerName, loc,
                          useIncrementalGen);
    }

    // Gen the cpp file (suppress if hGen failed)
    if (hGenned && cppNeedsToBeGenerated) {
        SysOut::verboseln(SPL_CORE_GEN_CODE_CPP(generatorBase, name), cout);
        genCode(cppGenerator, xml, name, fileNameBase, signature, cppName, loc, useIncrementalGen);
    }
}

void CodeGenHelper::genCodeIfNeeded(TupleModel const& model,
                                    string const& name,
                                    string const& signature,
                                    bf::path const& targetDir) const
{
    string const& fileNameBase = model.getClass();
    bf::path header = targetDir / (fileNameBase + ".h");
    bf::path src = targetDir / (fileNameBase + ".cpp");
    bf::path xml = targetDir / (fileNameBase + ".xml");

    // Compute the generator names
    string gen = model.getFacade() ? "facade_tuple" : "tuple";
    bf::path hGen = templateDir_ / (gen + "_h.pm");
    bf::path cppGen = templateDir_ / (gen + "_cpp.pm");

    genCodeIfNeeded<TupleModel, tupleModelType, xmlns::prod::streams::spl::tuple::tupleModel>(
      model, "http://www.ibm.com/xmlns/prod/streams/spl/tuple", name, fileNameBase, signature, gen,
      hGen, cppGen, header, src, xml, SourceLocation(), false);
}

void CodeGenHelper::genCodeIfNeeded(EnumerationModel const& model,
                                    string const& name,
                                    string const& signature,
                                    bf::path const& targetDir) const
{
    string const& fileNameBase = model.getClass();
    bf::path header = targetDir / (fileNameBase + ".h");
    bf::path src = targetDir / (fileNameBase + ".cpp");
    bf::path xml = targetDir / (fileNameBase + ".xml");

    // Compute the generator names
    bf::path hGen = templateDir_ / "enum_h.pm";
    bf::path cppGen = templateDir_ / "enum_cpp.pm";

    genCodeIfNeeded<EnumerationModel, enumerationModelType,
                    xmlns::prod::streams::spl::enumeration::enumerationModel>(
      model, "http://www.ibm.com/xmlns/prod/streams/spl/enumeration", name, fileNameBase, signature,
      "enum", hGen, cppGen, header, src, xml, SourceLocation(), false);
}

void CodeGenHelper::genCodeIfNeeded(XMLModel const& model,
                                    string const& name,
                                    string const& signature,
                                    bf::path const& targetDir) const
{
    string const& fileNameBase = model.getClass();
    bf::path header = targetDir / (fileNameBase + ".h");
    bf::path src = targetDir / (fileNameBase + ".cpp");
    bf::path xml = targetDir / (fileNameBase + ".xml");

    // Compute the generator names
    bf::path hGen = templateDir_ / "xml_h.pm";
    bf::path cppGen = templateDir_ / "xml_cpp.pm";

    genCodeIfNeeded<XMLModel, xmlModelType, xmlns::prod::streams::spl::xml::xmlModel>(
      model, "http://www.ibm.com/xmlns/prod/streams/spl/xml", name, fileNameBase, signature, "xml",
      hGen, cppGen, header, src, xml, SourceLocation(), false);
}

string CodeGenHelper::computeOperatorTemplateBaseName(OperatorInstanceModel const& inst,
                                                      bool& isJavaOp) const
{
    PathSearch& ps = PathSearch::instance();

    string fullyQualifiedOperatorName = inst.getContext().getKind();

    // We need to split the fully qualified name into a namespace name, operator name and compute
    // the basename.
    size_t colon = fullyQualifiedOperatorName.find("::", 0);
    string oper;
    if (colon == string::npos) {
        oper = fullyQualifiedOperatorName;
    } else {
        oper = fullyQualifiedOperatorName.substr(colon + 2, string::npos);
    }
    string directory;
    Operator::OperatorModel const* om =
      ps.findOperatorModel(fullyQualifiedOperatorName, &directory);
    if (!om) {
        SysOut::die(SPL_CORE_OPERATOR_NOT_FOUND(fullyQualifiedOperatorName));
    }
    if (om->isJavaOperator()) {
        om = ps.findOperatorModel("spl.utility::JavaOp", &directory);
        assert(om);
        isJavaOp = true;
        return (bf::path(directory) / "JavaOp").string();
    } else {
        isJavaOp = false;
        return (bf::path(directory) / oper).string();
    }
}

void CodeGenHelper::genCodeIfNeeded(OperatorInstanceModel const& model,
                                    string const& name,
                                    string const& signature,
                                    bf::path const& targetDir,
                                    bool useIncrementalGen,
                                    bf::path const& resDir) const
{
    checkIfGeneratorIsUpToDate(model, resDir);
    Context const& context = model.getContext();
    string const& fileName = context.getClass();
    bf::path path = targetDir;

    // className has the form [::]*.name.  Each '::' delimited segment except the last forms a
    // directory in the src hierarchy routed in src/operator
    size_t start = 0, end = fileName.find("::", 0);
    while (end != string::npos) {
        path = path / fileName.substr(start, end - start);
        Utility::createDirectory(path);
        start = end + 2;
        end = fileName.find("::", start);
    }
    string fileNameBase = fileName.substr(start, end - start);
    bf::path header = path / (fileNameBase + ".h");
    bf::path src = path / (fileNameBase + ".cpp");
    bf::path xml = path / (fileNameBase + ".xml");

    // Find the path to the code generators
    bool isJavaOp;
    string generatorBaseName = computeOperatorTemplateBaseName(model, isJavaOp);

    // compute the paths of the code generators
    bf::path hGen = generatorBaseName + "_h.pm";
    bf::path cppGen = generatorBaseName + "_cpp.pm";

    // Get a source location for use in diagnostics.
    SourceLocation const& loc = model.getContext().getSourceLocation();

    genCodeIfNeeded<OperatorInstanceModel, opimodel::operatorInstanceModelType,
                    xmlns::prod::streams::spl::operator_::instance::operatorInstanceModel>(
      model, "http://www.ibm.com/xmlns/prod/streams/spl/operator/instance", name, fileNameBase,
      signature, "operator", hGen, cppGen, header, src, xml, loc, useIncrementalGen);
}

void CodeGenHelper::genCodeIfNeeded(StandaloneModel const& model,
                                    string const& name,
                                    string const& signature,
                                    bf::path const& targetDir) const
{
    string fileNameBase = "standalone";
    bf::path header = targetDir / (fileNameBase + ".h");
    bf::path src = targetDir / (fileNameBase + ".cpp");
    bf::path xml = targetDir / (fileNameBase + ".xml");

    string generatorBaseName = (templateDir_ / "standalone").string();

    // compute the paths of the code generators
    bf::path hGen = generatorBaseName + "_h.pm";
    bf::path cppGen = generatorBaseName + "_cpp.pm";

    genCodeIfNeeded<StandaloneModel, xmlns::prod::streams::spl::standalone::standaloneModelType,
                    xmlns::prod::streams::spl::standalone::standaloneModel>(
      model, "http://www.ibm.com/xmlns/prod/streams/spl/standalone", name, fileNameBase, signature,
      "standalone", hGen, cppGen, header, src, xml, SourceLocation(), false);
}
