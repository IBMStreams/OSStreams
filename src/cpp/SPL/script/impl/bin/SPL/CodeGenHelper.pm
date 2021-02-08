#
# Copyright 2021 IBM Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# A collection of internal utility functions to faciliate code generation.
package SPL::CodeGenHelper;

use strict;
use warnings;
use SPL::Msg;
use SPL::Helper;
use SPL::CodeGen::Type;
use SPL::Operator::Instance::SourceLocation;
use SPL::Operator::Instance::ExpressionTreeEvaluator;
use Scalar::Util 'blessed';
use Data::Dumper;
use File::Spec;
use XML::Simple;

require Exporter;
our @ISA = 'Exporter';
our @EXPORT = ('emitALine', 'className', 'macroName', 'operatorIncludes', 'optionalHeaderIncludes',
               'openNamespaceDirectives', 'operatorBaseClassDeclaration', 'closeNamespaceDirectives',
               'usingDeclaration', 'optionalImplementationIncludes', 'operatorBaseClassCtorDef',
               'processMethods', 'defineNonConstProcessRaw', 'definePunctProcessRaw',
               'defineConstProcessRaw', 'ensureValue', 'checkTypeMatches'
              );

my $rrc;  # a global which will refer to the callers return code value when interrupts are disabled

my $indentStr = "    ";
our $indent = 0;


## @fn void headerPrologueImpl($model, $generic, $includes, $pragmaLine)
#  Given an operator's model, produces the prolog code for the operator's header.
#  @param model the operator instance model.
#  @param generic Whether this is a generic operartor or not. If non-generic, the
#  base class will include helper routines for parameter access and also errors will
#  be reported on the code generator, rather than on the generated code.
#  @param includes optional lines (passed in as a list reference) to be
#  generated after the ifndef/define lines
#  @param $pragmaLine the line number on which this call appears (only used from #pragma)
sub headerPrologueImpl($$;$$)
{
    my ($model, $generic, $includes, $pragmaLine) = @_;
    my $context = $model->getContext();
    my $class = $context->getClass();
    my $cName = className($class);
    my $mName = macroName($class);
    my $hasWindow = 0;
    my $isInConsistentRegion = $context->getOptionalContext("ConsistentRegion");
    foreach my $iport (@{$model->getInputPorts()}) {
        if($iport->hasWindow()) {
            $hasWindow = 1;
            last;
        }
    }
    emitALine("");
    emitALine("#ifndef ${mName}H_");
    emitALine("#define ${mName}H_");
    emitALine("");
    if (defined $includes) {
        foreach my $inc (@{$includes}) {
            emitALine ($inc);
        }
    }
    emitALine("#include <SPL/Runtime/Operator/Operator.h>");
    emitALine("#include <SPL/Runtime/Operator/ParameterValue.h>");
    emitALine("#include <SPL/Runtime/Operator/OperatorContext.h>");
    emitALine("#include <SPL/Runtime/Operator/OperatorMetrics.h>");

    emitALine("#include <SPL/Runtime/Operator/Port/AutoPortMutex.h>");
    if ($isInConsistentRegion) {
      emitALine("#include <SPL/Runtime/Operator/Port/OperatorInputPort.h>");
      emitALine("#include <SPL/Runtime/Operator/State/ConsistentRegionContext.h>");
    }
    emitALine("#include <SPL/Runtime/Operator/State/StateHandler.h>");
    eventTimeIncludeHeaders($model);
    emitALine("#include <SPL/Runtime/ProcessingElement/PE.h>");
    emitALine("#include <SPL/Runtime/Type/SPLType.h>");
    emitALine("#include <SPL/Runtime/Window/Window.h>") if($hasWindow);
    emitALine("#include <SPL/Runtime/Utility/CV.h>");

    emitALine("using namespace UTILS_NAMESPACE;");
    emitALine("");
    operatorIncludes($context);
    emitALine("");
    optionalHeaderIncludes($model);
    emitALine("");
    emitALine("#define MY_OPERATOR $cName\$OP");
    emitALine("#define MY_BASE_OPERATOR $cName" . "_Base");
    emitALine("#define MY_OPERATOR_SCOPE " . cppNamespaceScope($context->getClass()));
    emitALine("");
    emitALine("namespace SPL {");
    SPL::CodeGen::emitAllGetEventTime($model);
    emitALine("namespace _Operator {");
    openNamespaceDirectives($class);
    emitALine("");
    operatorBaseClassDeclaration($model, $generic);

    if(not $generic) {
        $pragmaLine = 1 unless (defined $pragmaLine);
        my $kind = $model->getContext()->getKind();
        my ($dummy, $opname) = $kind =~ m/(.*::)?([^:]*)/;
        my $opdir = $model->getContext()->getOperatorDirectory();
        $opdir = File::Spec->abs2rel($opdir);
        my $file = "\"${opdir}/${opname}_h.cgt\"";
        emitALine("#line ".($pragmaLine+1)." ${file}");
    }
}

## @fn void headerEpilogueImpl($model, $generic)
#  Given an operator's model, produces the epilog code for the operator's header.
#  @param model The operator instance model.
#  @param generic Whether this is a generic operartor or not. If non-generic, the
#  base class will include helper routines for parameter access and also errors will
#  be reported on the code generator, rather than on the generated code.
sub headerEpilogueImpl($$)
{
   my ($model, $generic) = @_;
   my $class = $model->getContext()->getClass();
   my $mName = macroName($class);
   closeNamespaceDirectives($class);
   emitALine("} // namespace _Operator");
   emitALine("} // namespace SPL");
   emitALine("");
   emitALine("#undef MY_OPERATOR_SCOPE");
   emitALine("#undef MY_BASE_OPERATOR");
   emitALine("#undef MY_OPERATOR");
   emitALine("#endif // ${mName}H_");
}

## @fn void implementationPrologueImpl($model, $generic, $pragmaLine)
#  Given an operator's model, produces the prolog code for the operator's cpp file.
#  @param model The operator instance model.
#  @param generic Whether this is a generic operartor or not. If non-generic, the
#  base class will include helper routines for parameter access and also errors will
#  be reported on the code generator, rather than on the generated code.
#  @param $pragmaLine the line number on which this call appears (only used from #pragma)
sub implementationPrologueImpl($$;$)
{
   my ($model, $generic, $pragmaLine) = @_;
   my $context = $model->getContext();
   my $cName = className($context->getClass());
   emitALine("");
   emitALine('#include "./' . $cName . '.h"');
   usingDeclaration($context->getClass());
   emitALine("");
   emitALine("#include <SPL/Runtime/Function/SPLFunctions.h>");
   emitALine("#include <SPL/Runtime/Operator/Port/Punctuation.h>");
   emitALine("");
   optionalImplementationIncludes($model);
   emitALine("");
   emitALine("#define MY_OPERATOR_SCOPE " . cppNamespaceScope($context->getClass()));
   emitALine("#define MY_BASE_OPERATOR $cName" . "_Base");
   emitALine("#define MY_OPERATOR $cName\$OP");
   emitALine("");
   eventTimeDefineStatics($model);
   if(not $generic) {
       $pragmaLine = 1 unless (defined $pragmaLine);
       my $kind = $model->getContext()->getKind();
       my ($dummy, $opname) = $kind =~ m/(.*::)?([^:]*)/;
       my $opdir = $model->getContext()->getOperatorDirectory();
       $opdir = File::Spec->abs2rel($opdir);
       my $file = "\"${opdir}/${opname}_cpp.cgt\"";
       emitALine("#line ".($pragmaLine+1)." ${file}");
   }
}

## @fn void implementationEpilogueImpl($model,$generic)
#  Given an operator's model, produces the epilog code for the operator's cpp file.
#  @param model the operator instance model
#  @param generic Whether this is a generic operartor or not. If non-generic, the
#  base class will include helper routines for parameter access and also errors will
#  be reported on the code generator, rather than on the generated code.
sub implementationEpilogueImpl($$)
{
   my ($model,$generic) = @_;
   operatorBaseClassStaticIniter($model);
   emitALine("");
   operatorBaseClassCtorDef($model, $generic);
   emitALine("");
   processMethods($model, \&defineConstProcessRaw, \&defineNonConstProcessRaw, \&definePunctProcessRaw);
   emitALine("");
   parameterAccessMethodImpls($model) unless($generic);
   emitALine("");
   checkpointRestoreImpls($model);
   checkpointRestoreRawImpls($model);
}


## @fn string getOutputTupleCppInitializerWithOutputFunction($port)
#  Given an output port object, return the C++ expression which can be used to
#  initialize a tuple for the port with the output assignments with
#  output functions. Any output function field will be named
#  <tt>\<outputFunctionName\>$\<fieldName\>()</tt>.
#  @param port the output port object
#  @param outputFunctionPrefix String to prepend to the output function name. Ignored if undefined
#  @return a string containing the expression to be used for initializing the
#  output tuple
#  Example: For <tt>tuple\<int8 a, int8 b\></tt> and <tt>output S : a = Min(x),
#  b = Max(y);</tt>, this function will produce the following: <tt>Min$a(),
#  Max$b()</tt>
sub getOutputTupleCppInitializerWithOutputFunctions($;$)
{
    my ($outputPort, $outputFunctionPrefix) = @_;
    $outputFunctionPrefix = "" if !$outputFunctionPrefix;
    my $numAttrs = $outputPort->getNumberOfAttributes();
    my $assignments = "";
    for (my $i = 0; $i < $numAttrs; ++$i) {
        my $attr = $outputPort->getAttributeAt($i);
        if ($attr->hasAssignmentWithOutputFunction()) {
            $assignments .= $outputFunctionPrefix . $attr->getAssignmentOutputFunctionName() . '$' . $attr->getName() . "()";
        } else {
            my $exp = $attr->getAssignmentValue();
            $assignments .= ensureValue ($attr->getSPLType(), $exp->getCppExpression()) if $exp;
        }
        $assignments .= ", " if $i < $numAttrs-1;
    }
    return $assignments;
}

## @fn string inputPortTypeName($portIndex)
#  Computes the typedef'd type name for a given input port.
#  @param portIndex The index of the input port.
#  @return Returns the type name of the input port.
sub inputPortTypeName($)
{
   my ($portIndex) = @_;
   return "IPort${portIndex}Type";
}

## @fn string outputPortTypeName($portIndex)
#  Computes the typedef'd type name for a given output port.
#  @param portIndex The index of the output port.
#  @return Returns the type name of the output port.
sub outputPortTypeName($)
{
   my ($portIndex) = @_;
   return "OPort${portIndex}Type";
}

## @fn string portHistoryTypeName($portIndex)
#  Computes the type name of a history member variable for a given input port.
#  @param portIndex The index of the input port.
#  @return Returns the type name of the history member.
sub portHistoryTypeName($)
{
   my ($portIndex) = @_;
   return "IPort\$${portIndex}HistoryType";
}

## @fn string portHistoryMemberName($portIndex)
#  Computes the name of a history member variable for a given input port.
#  @param portIndex The index of the input port.
#  @return Returns the name of the history member.
sub portHistoryMemberName($)
{
   my ($portIndex) = @_;
   return "iport\$${portIndex}History";
}

## @fn string finalPunctMutexName()
#  Returns the name to use for the final punctuation mutex variable.
#  @return Returns a string containing the variable name.
sub finalPunctMutexName()
{
   return "\$fpMutex";
}

## @fn string ccContextName()
# Returns the name of the variable that points to the ConsistentRegionContext
# @return Returns a string containing the variable name.
sub ccContextName()
{
  return "\$ccContext";
}

## @fn string stateVarMutexName()
#  Returns the name to use for the state variable mutex variable.
#  @return Returns a string containing the variable name.
sub stateVarMutexName()
{
   return "\$svMutex";
}

## @fn string autoPortMutexName()
#  Returns the name to use for the auto port mutex variable
#  @return Returns a string containing the variable name.
sub autoPortMutexName()
{
   return "\$apm";
}

## @fn string bitsetMemberName()
#  Given the number of an output port, generate a unique
#  name for the final punctuation bitset.
sub bitsetMemberName()
{
   return "\$oportBitset";
}

## @fn void emitALine(@params)
#  Prints the given parameters to STDOUT followed by a new line
#  after indenting by $indent times the number of spaces in $indentStr.
#  @param params The list of parameters to print
sub emitALine
{
   my (@params) = @_;
   if ($indent > 0) {
      for (my $i = 0; $i < $indent; ++$i) {
         print $indentStr;
      }
   }
   print @params, "\n";
}

## @fn void headerIncludes($model)
#  Emits the fixed set of headers required to compile an operator.
#  @param $model The operator model
sub headerIncludes($)
{
   my ($model) = @_;
   my $class = $model->getClass();

   emitALine "#include <SPL/Runtime/Type/SPLType.h>";
   emitALine "#include <SPL/Runtime/Common/RuntimeException.h>";
}

## @fn void namespaceName($class)
#  Given an operator's class name, produces a namespace name in the form of a::b::c
#  @param class The operator's fully-qualified class name.
sub namespaceName($)
{
   my ($class) = @_;
   my @bits = splitName($class);
   my $ns = "";
   for (my $i = 0; $i < scalar(@bits)-1; ++$i) {
      if (length($ns) > 0) { $ns = $ns . "::"; }
      $ns = $ns . $bits[$i];
   }
   return $ns;
}

## @fn void openNamespaceDirectives($class)
# Given an operator's name, say a::b::c
# create a series of namespace declarations
# like namespace a { namespace b {
# The last segment in the name is the actual class name
# @param class The fully-qualified name of an operator for which
# we are generating open namespace directives.
sub openNamespaceDirectives($)
{
   my ($class) = @_;
   my @bits = splitName($class);
   for (my $i = 0; $i < scalar(@bits)-1; ++$i) {
      emitALine "namespace " . $bits[$i] . " {";
   }
}

## @fn void closeNamespaceDirectives($class)
#  Given an operator's name, produces the appropriate closings for
#  its enclosing namespace
#  @param class The operator's fully-qualified class name.
sub closeNamespaceDirectives($)
{
   my ($class) = @_;
   my @bits = splitName($class);
   for (my $i = 0; $i < scalar(@bits)-1; ++$i) {
      emitALine "} // namespace " . $bits[$i];
   }
}

## @fn string className($class)
#  Given an operator's fully-qualified name, returns the operator's class name
#  @param class The operator's fully-qualified class name.
#  @return Returns the class name with all qualifiers removed.
sub className($)
{
   my ($class) = @_;
   my @bits = splitName($class);
   return $bits[$#bits];
}

## @fn \@string splitName($class)
#  This function splits a fully-qualified class name like a::b::c:::d into its component parts
#  @return Returns a list of the component parts of the name.
sub splitName($)
{
   my ($class) = @_;
   return split (/::/, $class);
}

## @fn string macroName($class)
#  Given an operator's name, produces the head of a header guard macro name
#  @return Returns a string containing the macro name.
sub macroName($)
{
   my ($class) = @_;
   my @bits = splitName($class);
   my $name = "";
   foreach my $comp (@bits) {
      $name = $name . uc($comp) . "_";
   }
   return "SPL_OPER_INSTANCE_" . $name;
}

## @fn void operatorIncludes($context)
#  Given an operator model's context, this method emits a '\#include'
#  statement for each header.
#  @note The header in name in the context should already have the '<>' in it.
#  @param context The operator model's context.
sub operatorIncludes($)
{
  my ($context) = @_;
   for ( my $i = 0; $i < $context->getNumberOfIncludes(); ++$i) {
      my $include = $context->getIncludeAt($i);
      emitALine "#include " . $include;
   }
}

## @fn void optionalHeaderIncludes($model)
#  Given the operator's model, this function emits any optional headers that are required
#  in the operator's header file in order for it to compile.
#  @param model The operator's model.
sub optionalHeaderIncludes($)
{
   my ($model) = @_;

   if ($model->getNumberOfInputPorts() > 0) {
      my $iports = $model->getInputPorts();
      foreach my $iport (@$iports) {
         if ($iport->hasHistory()) {
         emitALine "#include <deque>";
         last;
         }
      }
      # Add header for final punctuation handling bitset
      my $numOputputPorts = $model->getNumberOfOutputPorts();
      if ($numOputputPorts > 0) {
         emitALine "#include <bitset>";
         if ($numOputputPorts > 1) {
            emitALine "#include <vector>";
         }
      }
   }
}

## @fn void optionalImplementationIncludes($model)
#  Given the operator's model, this function emits any optional headers that are required
#  in the operator's source file in order for it to compile.
#  @param model The operator's model.
sub optionalImplementationIncludes($)
{
   my ($model) = @_;

   if ($model->getNumberOfInputPorts() > 0) {
      # Look for final punctuation forwarding (won't be any if there are no input ports)
      if ($model->getNumberOfOutputPorts() > 0) {
         emitALine "#include <string>";
      }
   }
}

## @fn void declareConstProcessRaw()
#  This function emits a declaration for 'void processRaw(Tuple const & tuple, uint32_t port)'
#  and the corresponding tupleLogic.
sub declareConstProcessRaw($)
{
   emitALine "inline void tupleLogic(Tuple const & tuple, uint32_t port);";
   emitALine "void processRaw(Tuple const & tuple, uint32_t port);";
}

## @fn void declareNonConstProcessRaw()
#  This function emits a declaration for 'void processRaw(Tuple & tuple, uint32_t port)'
#  and the corresponding tupleLogic.
sub declareNonConstProcessRaw($)
{
   emitALine "inline void tupleLogic(Tuple & tuple, uint32_t port);";
   emitALine "void processRaw(Tuple & tuple, uint32_t port);";
}

## @fn void declarePunctProcessRaw()
#  This function emits a declaration for 'void processRaw(Punctuation const & punct, uint32_t port)' and the corresponding punctLogic.
sub declarePunctProcessRaw($)
{
   my ($model) = @_;
   my $context = $model->getContext();
   my $isInConsistentRegion = $context->getOptionalContext("ConsistentRegion");

   emitALine "inline void punctLogic(Punctuation const & punct, uint32_t port);";
   emitALine "void processRaw(Punctuation const & punct, uint32_t port);";
   emitALine "void punctPermitProcessRaw(Punctuation const & punct, uint32_t port);";
   emitALine "void punctNoPermitProcessRaw(Punctuation const & punct, uint32_t port);";

# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
#   emitALine "inline void watermarkLogic(Watermark const & wm, uint32_t port);";

   if (isJavaOperator($context))
   {
       emitALine "virtual void javaopInternalAction(Punctuation const & punct, uint32_t port) = 0;";
   }
}

## @fn void declareNonConstSubmit()
#  This function emits a declaration for 'void submit(Tuple & tuple, uint32_t port)'.
sub declareNonConstSubmit($)
{
   emitALine "inline void submit(Tuple & tuple, uint32_t port)";
   emitALine "{";
   ++$indent;
   emitALine "Operator::submit(tuple, port);";
   --$indent;
   emitALine "}";
}

## @fn void declareConstSubmit()
#  This function emits a declaration for 'void submit(Tuple const & tuple, uint32_t port)'.
sub declareConstSubmit($)
{
   emitALine "inline void submit(Tuple const & tuple, uint32_t port)";
   emitALine "{";
   ++$indent;
   emitALine "Operator::submit(tuple, port);";
   --$indent;
   emitALine "}";
}

## @fn void declarePunctSubmit()
#  This function emits a declaration for 'void submit(Punctuation const & punct, uint32_t port)'.
sub declarePunctSubmit($)
{
   my ($model) = @_;
   emitALine "inline void submit(Punctuation const & punct, uint32_t port)";
   emitALine "{";
   ++$indent;
   emitALine "Operator::submit(punct, port);";
   --$indent;
   emitALine "}";
}

## @fn void operatorBaseClassDeclaration($model, $generic)
#  Given the operator's model, produces the declaration
#  for the operator's base class.
#  @param model The operator's model.
#  @param generic Whether this is a generic operator or not, in case it is
#  non-generic helper methods for parameter access will be generated
sub operatorBaseClassDeclaration($$)
{
   my ($model, $generic) = @_;
   emitALine "class MY_BASE_OPERATOR : public Operator";
   emitALine "{";
   emitALine "public:";
   ++$indent;
   emitALine "";
   portTypedefs($model);
   emitALine "";
   emitALine "MY_BASE_OPERATOR();";
   emitALine "";
   emitALine "~MY_BASE_OPERATOR();";
   emitALine "";
   processMethods($model, \&declareConstProcessRaw, \&declareNonConstProcessRaw, \&declarePunctProcessRaw);
   emitALine "";
   submitMethods($model, \&declareNonConstSubmit, \&declareConstSubmit, \&declarePunctSubmit);
   emitALine "";
   parameterAccessMethodDecls($model) unless($generic);
   emitALine "";
   runtimeConstantDeclarations($model);
   emitALine "";
   memberDeclarations($model);
   emitALine "";
   --$indent;
   emitALine "protected:";
   ++$indent;
   emitALine "Mutex " . stateVarMutexName() . ";";
   historyMemberDeclarations($model);
   finalPunctuationBitsets($model);
   consistentContextDeclaration($model);
   parameterMemberDeclarations($model);
   checkpointDeclarations($model);
   eventTimeDeclarations($model);
   --$indent;
   emitALine "";
   emitALine "private:";
   ++$indent;
   emitALine("static bool globalInit_;");
   emitALine("static bool globalIniter();");
   emitALine "ParameterMapType paramValues_;";
   if (mustEmitExceptionBlock($model)) {
       emitALine "Metric* metrics_[" . $model->getNumberOfInputPorts() . "];";
   }
   emitALine "ParameterMapType& getParameters() { return paramValues_;}";
   emitALine "void addParameterValue(std::string const & param, ConstValueHandle const& value)";
   emitALine "{";
   ++$indent;
   emitALine "ParameterMapType::iterator it = paramValues_.find(param);";
   emitALine "if (it == paramValues_.end())";
   ++$indent;
   emitALine "it = paramValues_.insert (std::make_pair (param, ParameterValueListType())).first;";
   --$indent;
   emitALine "it->second.push_back(&ParameterValue::create(value));";
   --$indent;
   emitALine "}";
   emitALine "void addParameterValue(std::string const & param)";
   emitALine "{";
   ++$indent;
   emitALine "ParameterMapType::iterator it = paramValues_.find(param);";
   emitALine "if (it == paramValues_.end())";
   ++$indent;
   emitALine "it = paramValues_.insert (std::make_pair (param, ParameterValueListType())).first;";
   --$indent;
   emitALine "it->second.push_back(&ParameterValue::create());";
   --$indent;
   emitALine "}";
   --$indent;
   emitALine "};";
}

## @fn bool isCustomLiteralParameter($param)
#  given a parameter, check if it is a custom literal
sub isCustomLiteralParameter($)
{
   my ($param) = @_;
   my $value = $param->getValueAt(0);
   return !($value->getCppType() =~ /^SPL::/);
}

## @fn bool isRuntimeConstant($expn)
#  given an expression, check if it is a runtime literal
sub isRuntimeConstant($)
{
   my ($expn) = @_;
   return $expn->getCppExpression() =~ m/^lit\$[0-9]*$/;
}

## @fn bool isParamWithNoStreamAttributes($param)
#  given a parameter, check if it has no stream attributes
sub isParamWithNoStreamAttributes($)
{
   my ($param) = @_;
   for (my $i = 0; $i < $param->getNumberOfValues(); $i++) {
       my $value = $param->getValueAt($i);
       return 0 if $value->hasStreamAttributes();
   }
   return 1;
}

## @fn void parameterMemberDeclarations($model)
#  Given the operator model, create parameter member variables for non-generic operators.
sub parameterMemberDeclarations($)
{
    my ($model) = @_;
    my $params = $model->getParameters();
    foreach my $param (@{$params}) {
        for (my $i = 0; $i < $param->getNumberOfValues(); $i++) {
            next if not isParamWithNoStreamAttributes($param);
            my $value = $param->getValueAt($i);
            next if isRuntimeConstant($value);
            my $type = isCustomLiteralParameter($param) ? "SPL::rstring" : $value->getCppType();
            emitALine "$type param\$" . $param->getName() . '$' . $i . ';';
        }
    }
}

## @fn void parameterAccessMethodDecls($model)
#  Given the operator model, create parameter access method declerations for
#  non-generic operators.
sub parameterAccessMethodDecls($)
{
   my ($model) = @_;
   my $params = $model->getParameters();
   foreach my $param (@{$params}) {
       next if(not isParamWithNoStreamAttributes($param));
       my $value = $param->getValueAt(0);
       my $type = isCustomLiteralParameter($param) ? "SPL::rstring" : $value->getCppType();
       emitALine "$type const & getParameter_", $param->getName(), "() const;"
   }
   emitALine "bool hasParameter(std::string const & param) const;";
   emitALine "SPL::ConstValueHandle getParameter(std::string const & param) const;";
}

## @fn void parameterValueInitList($model)
#  Given the operator model, populate parameter member variables for non-generic operators.
sub parameterValueInitList($)
{
    my ($model) = @_;
    my $params = $model->getParameters();
    foreach my $param (@{$params}) {
        my $pname = $param->getName();
        for (my $i = 0; $i < $param->getNumberOfValues(); $i++) {
            my $qname = '"' . $pname . '"';
            my $value = $param->getValueAt($i);
            my $pmode = $value->getParameterExpressionMode();
            if (not isParamWithNoStreamAttributes($param)) {
                emitALine "addParameterValue ($qname);";
            } else {
                my $p;
                if (isRuntimeConstant($value)) {
                    $p = $value->getCppExpression();
                } else {
                    $p = 'param$' . $pname . '$' . $i;
                    if (isCustomLiteralParameter($param)) {
                        emitALine $p . ' = "' . $value->getSPLExpression() . '";';
                    } else {
                        emitALine "$p = " . $value->getCppExpression() . ";";
                    }
                }
                emitALine "addParameterValue ($qname, SPL::ConstValueHandle($p));";
            }
        }
    }
    emitALine "(void) getParameters(); // ensure thread safety by initializing here";
}

## @fn void parameterAccessMethodImpls($model)
#  Given the operator model, create parameter access methods implementations for
#  non-generic operators.
sub parameterAccessMethodImpls($)
{
   my ($model) = @_;
   my $params = $model->getParameters();
   foreach my $param (@{$params}) {
       next if not isParamWithNoStreamAttributes($param);
       my $value = $param->getValueAt(0);
       my $pname = $param->getName();
       my $type = isCustomLiteralParameter($param) ? "SPL::rstring" : $value->getCppType();
       emitALine "$type const & MY_BASE_OPERATOR::getParameter_${pname}() const {";
       ++$indent;
       if (isRuntimeConstant($value)) {
           emitALine "return " . $value->getCppExpression() . ';';
       } else {
           emitALine "return param\$$pname\$0;";
       }
       --$indent;
       emitALine "}";
       emitALine "";
   }

   emitALine "bool MY_BASE_OPERATOR::hasParameter(std::string const & param) const {";
   ++$indent;
   emitALine "return getParameterNames().count(param);";
   --$indent;
   emitALine "}";

   emitALine "SPL::ConstValueHandle MY_BASE_OPERATOR::getParameter(std::string const & param) const {";
   ++$indent;
   emitALine "assert(getParameterNames().count(param));";
   emitALine "const ParameterValueListType& values = getParameterValues(param);";
   emitALine "return values[0]->getValue();";
   --$indent;
   emitALine "}";
}

## @fn void finalPunctuationBitsets($model)
#  Given an operator's model it emits a declaration for a final punctuation bitset
#  @param model The operator's model
sub finalPunctuationBitsets($)
{
   my ($model) = @_;

   my $numInputPorts = $model->getNumberOfInputPorts();
   if ($numInputPorts > 0) {
      my $numOutputPorts = $model->getNumberOfOutputPorts();
      if ($numOutputPorts > 0) {
         # For each output port
         # we need a bitset to track if the input ports
         # have signalled
         my $numBits = $numInputPorts+1;
         my $bitsetName = bitsetMemberName();
         emitALine "typedef std::bitset<${numBits}> OPortBitsetType;";
         if ($numOutputPorts > 1) {
            emitALine "std::vector<OPortBitsetType> ${bitsetName};";
         } else {
            emitALine "OPortBitsetType ${bitsetName};";
         }
         emitALine "Mutex " . finalPunctMutexName() . ";";
      }
   }
}

## @fn void consistentContextDeclaration($model)
# Given an operator's model, it emits a declaration for the consistent annotation context
# @param model The operator's model
sub consistentContextDeclaration($)
{
   my ($model) = @_;

   if ($model->getContext()->getOptionalContext("ConsistentRegion")) {
      emitALine "ConsistentRegionContext * " . ccContextName() . ";";
   }
}

## @fn void runtimeConstantDeclarations($model)
# If the operator given by $model has any runtime constants,
# declares member variables for them.
#  @param model The operator's model
sub runtimeConstantDeclarations($)
{
   my ($model) = @_;
   my $context = $model->getContext();
   for (my $i = 0; $i < $context->getNumberOfRuntimeConstants(); ++$i ) {
      my $constant = $context->getRuntimeConstantAt($i);
      emitALine $constant->getCppType() . " " .  $constant->getName() . ";";
   }
}

## @fn void historyMemberDeclarations($model)
#  If any input ports have history then emit the history members
#  @param model The operator's model
sub historyMemberDeclarations($)
{
   my ($model) = @_;
   if ($model->getNumberOfInputPorts() > 0) {
      my $ports = $model->getInputPorts();
      foreach my $port (@$ports) {
         if ($port->hasHistory()) {
            my $index = $port->getIndex();
            my $typeName = portHistoryTypeName($index);
            my $memberName = portHistoryMemberName($index);
            my $iPortTypeName = inputPortTypeName($index);
            emitALine "typedef std::deque<${iPortTypeName}> ${typeName};";
            emitALine "${typeName} ${memberName};";
         }
      }
   }
}

## @fn void eventTimeIncludeHeaders()
#  Emits the event time related include statements.
sub eventTimeIncludeHeaders($)
{
   my ($model) = @_;
   if (hasEventTimeContext($model)) {
       emitALine "#include <SPL/Runtime/Operator/EventTime/EventTimeContext.h>";
   }
}

## @fn void eventTimeDeclarations()
#  Emits the event time related operator members and functions.
#  @param model The operator's model
sub eventTimeDeclarations($)
{
    my ($model) = @_;
    if (hasEventTimeContext($model)) {
        emitALine "EventTimeContext& eventTimeContext_;";
    }
}

## @fn void eventTimeDefineStatics($model)
#  Given an operator's model, emits the event-time related static member
#  definitions.
#  @param model The operator's model
sub eventTimeDefineStatics($)
{
   my ($model) = @_;
   # nothing to do.
}

sub hasEventTimeContext($) {
    my ($model) = @_;
    SPL::CodeGen::hasEventTimeContext($model);
}

## @fn void eventTimeInitialize()
#  Emits the event-time related class member initialization code.
sub eventTimeInitialize($)
{
    my ($model) = @_;
    if (hasEventTimeContext($model)) {
        return ", eventTimeContext_(*EventTimeContext::getContext(*this))";
    }
    else {
        return "";
    }
}

## @fn void eventTimeHandleWatermark($model)
#  Emits the event time related code for handling Watermark punctuations.
#  @param indent [Input/Output] The indentation level used by the caller.
sub eventTimeHandleWatermark($)
{
   my ($model) = @_;
   my $context = $model->getContext();

   if (hasEventTimeContext($model)) {
       if (isJavaOperator($context))
       {
           emitALine "javaopInternalAction(punct, port);";
       }
   }
}

## @fn void portTypedefs($model)
#  Given an operator's model, emits a set of typedefs for
#  its input and output ports.
#  @param model The operator's model
sub portTypedefs($)
{
   my ($model) = @_;
   for (my $i = 0; $i < $model->getNumberOfInputPorts(); ++$i) {
      my $port = $model->getInputPortAt($i);
      my $tupleType = $port->getCppTupleType();
      my $portTypeName = inputPortTypeName($i);
      emitALine "typedef ${tupleType} ${portTypeName};";
   }
   for (my $j = 0; $j < $model->getNumberOfOutputPorts(); ++$j) {
      my $port = $model->getOutputPortAt($j);
      my $tupleType = $port->getCppTupleType();
      my $portTypeName = outputPortTypeName($j);
      emitALine "typedef ${tupleType} ${portTypeName};";
   }
}

## @fn void memberDeclarations($model)
#  Given an operator's model, emits  a set of member declarations.
#  @param model The operator's model
sub memberDeclarations($)
{
   my ($model) = @_;
   my $context = $model->getContext();
   for (my $i = 0; $i < $context->getNumberOfStateVariables(); ++$i) {
      my $var = $context->getStateVariableAt($i);
      emitALine $var->getCppType() . " " .  $var->getName() . ";";
   }
}

## @fn boolean mustEmitExceptionBlock($model)
# Given an operator's model, checks if an exception block should be emitted
# @param model The operator's model
# @return 1 if the exception block must be emitted, 0 otherwise
sub mustEmitExceptionBlock($)
{
   my ($model) = @_;
   my $context = $model->getContext();
   my $catchContext = $context->getOptionalContext("Catch");
   if ($catchContext && $catchContext->mustCatchExceptions()) {
      return 1;
   }
   return 0;
}

## @fn void emitCatchExceptionBegin($model)
# Given an operator's model, emit the beginning
# of a try/catch block, if necessary
# @param model The operator's model
sub emitCatchExceptionBegin($)
{
   my ($model) = @_;
   if (!mustEmitExceptionBlock($model)) {
       return;
   }
   $indent++;
   emitALine "try {";
   $indent++;
}

## @fn void emitTupleTrace($model)
# Given an operator's model, emit the code for doing
# tuple logging when handling an exception
# @param model The operator's catch context
sub emitTupleTrace($)
{
   my ($catchContext) = @_;
   # Print tuples
   emitALine "SPLAPPTRC(L_ERROR, \"Exception in operator \" << getContext().getName()";
   emitALine "    << \" in port \" << port, SPL_OPER_DBG);";
   if ($catchContext->mustTraceTuples()) {
      emitALine "SPLAPPTRC(L_ERROR, \"Processed tuple: \" << tuple, SPL_OPER_DBG);";
   }
}

## @fn void emitStackTrace()
# Given an operator's model, emit the code for printing
# the exception stack trace
# @param model The operator's catch context
# Emit the code for emitting an exception stack trace
sub emitStackTrace($)
{
   my ($catchContext) = @_;
   if ($catchContext->mustPrintStackTrace()) {
       emitALine "std::stringstream backtrace;";
       emitALine "SPL::BacktraceDumper::dump(backtrace);";
       emitALine "SPLAPPTRC(L_ERROR, \"Exception: \" << backtrace.str(), SPL_OPER_DBG);";
   }
}

## @fn void emitMetricIncrement
# emit code for incrementing the
# nExceptionsCaughtPortX metric
# for the port
sub emitMetricIncrement($)
{
    my ($catchContext) = @_;
    emitALine "metrics_[port]->incrementValue();";
}

## @fn void emitCatchExceptionEnd($model)
# Given an operator's model, emit the end
# of a try/catch block, if necessary
# @param model The operator's model
sub emitCatchExceptionEnd($)
{
   my ($model) = @_;
   if (!mustEmitExceptionBlock($model)) {
       return;
   }
   my $catchContext = $model->getContext()->getOptionalContext("Catch");
   --$indent;
   if ($catchContext->mustCatchStreamsExceptions() ||
       $catchContext->mustCatchStandardExceptions() ||
       $catchContext->mustCatchAllExceptions()) {
       emitALine "} catch (SPL::SPLRuntimeException const & e) {";
       $indent++;
       emitALine "if (getContext().getPE().mustRethrowException()) {";
       $indent++;
       emitALine "throw e;";
       $indent--;
       emitALine "}";
       emitTupleTrace($catchContext);
       if ($catchContext->mustPrintStackTrace()) {
          emitALine "SPLAPPTRC(L_ERROR, \"Exception: \" << e, SPL_OPER_DBG);";
       }
       emitMetricIncrement($catchContext);
       $indent--;
       emitALine "}";
   }
   if ($catchContext->mustCatchStandardExceptions() ||
       $catchContext->mustCatchAllExceptions()) {
       emitALine "catch (std::exception const & e) {";
       $indent++;
       emitALine "if (getContext().getPE().mustRethrowException()) {";
       $indent++;
       emitALine "throw e;";
       $indent--;
       emitALine "}";
       emitTupleTrace($catchContext);
       if ($catchContext->mustPrintStackTrace()) {
           emitALine "SPLAPPTRC(L_ERROR, \"Exception identifier: \" << e.what(), SPL_OPER_DBG);";
           emitStackTrace($catchContext);
       }
       emitMetricIncrement($catchContext);
       $indent--;
       emitALine "}";
   }
   if ($catchContext->mustCatchAllExceptions()) {
       emitALine "catch (...) {";
       $indent++;
       emitALine "if (getContext().getPE().mustRethrowException()) {";
       $indent++;
       emitALine "throw;";
       $indent--;
       emitALine "}";
       emitTupleTrace($catchContext);
       emitStackTrace($catchContext);
       emitMetricIncrement($catchContext);
       $indent--;
       emitALine "}";
   }
   $indent--;
}

## @fn void emitStateVariableCheckpoint($model,$bufferName)
#  Given an operator's model and the buffer name,
#  emits the checkpoint routines for the operator.
#  @param model The operator's model
#  @param bufferName name of the buffer to checkpoint the state to
sub emitStateVariableCheckpoint($$)
{
   my ($model, $bufferName) = @_;
   my $context = $model->getContext();
   for (my $i = 0; $i < $context->getNumberOfStateVariables(); ++$i) {
      my $var = $context->getStateVariableAt($i);
      my $varType = $var->getSPLType();
      emitALine "$bufferName << " .  $var->getName() . ";";
   }
}

## @fn void emitStreamHistoryCheckpoint($model, $bufferName)
# Given an operator's model and the buffer name,
# emits the checkpoint routines for the operator stream
# history.
# @param model The operator's model
# @param bufferName name of the buffer to checkpoint the state to
sub emitStreamHistoryCheckpoint($$)
{
   my ($model, $bufferName) = @_;
   for (my $i = 0; $i < $model->getNumberOfInputPorts(); ++$i) {
      my $inputPort = $model->getInputPortAt($i);
      if ($inputPort->hasHistory()) {
         my $memberName = portHistoryMemberName($i);
         emitALine "$bufferName << " . $memberName . ";";
      }
   }
}


## @fn void emitStateVariableRestore($model,$bufferName)
#  Given an operator's model and the buffer name,
#  emits the restore routines for the operator.
#  @param model The operator's model
#  @param bufferName name of the buffer to restore the state from
sub emitStateVariableRestore($$)
{
   my ($model, $bufferName) = @_;
   my $context = $model->getContext();
   for (my $i = 0; $i < $context->getNumberOfStateVariables(); ++$i) {
      my $var = $context->getStateVariableAt($i);
      emitALine "$bufferName >> " .  $var->getName() . ";";
   }
}

## @fn void emitStreamHistoryRestore($model, $bufferName)
# Given an operator's model and the buffer name,
# emits the restore routines for the operator stream
# history.
# @param model The operator's model
# @param bufferName name of the buffer to restore the state from
sub emitStreamHistoryRestore($$)
{
   my ($model, $bufferName) = @_;
   for (my $i = 0; $i < $model->getNumberOfInputPorts(); ++$i) {
      my $inputPort = $model->getInputPortAt($i);
      if ($inputPort->hasHistory()) {
         my $memberName = portHistoryMemberName($i);
         emitALine "${memberName}.clear();";
         emitALine "$bufferName >> " . $memberName . ";";
      }
   }
}

## @fn void checkpointDeclarations($model)
#  Given an operator's model, emits the checkpoint save/restore
#  method declarations for the operator.
#  @param model The operator's model
sub checkpointDeclarations($)
{
   my ($model) = @_;
   my $context = $model->getContext();
   emitALine "void checkpointStateVariables(NetworkByteBuffer & opstate) const;";
   emitALine "void restoreStateVariables(NetworkByteBuffer & opstate);";
   emitALine "void checkpointStateVariables(Checkpoint & ckpt);";
   emitALine "void resetStateVariables(Checkpoint & ckpt);";
   emitALine "void resetStateVariablesToInitialState();";
   emitALine "bool hasStateVariables() const;";
   emitALine "void resetToInitialStateRaw();";
   emitALine "void checkpointRaw(Checkpoint & ckpt);";
   emitALine "void resetRaw(Checkpoint & ckpt);";
}

## @fn void generateHasStateVariables($model)
# Given an operator's model, emits the hasStateVariable method
# This method has to return true if there is any state in the operator
# that must be persisted. This includes stream history and
# state variables.
# @param model The operator's model
sub generateHasStateVariables($)
{
   my ($model) = @_;

   emitALine "bool MY_BASE_OPERATOR::hasStateVariables() const {";
   ++$indent;
   if ($model->getContext()->getNumberOfStateVariables() > 0) {
      emitALine "return true;";
   } else {
      my $hasHistory = 0;
      for (my $i = 0; $i < $model->getNumberOfInputPorts(); ++$i) {
         my $inputPort = $model->getInputPortAt($i);
         if ($inputPort->hasHistory()) {
            $hasHistory = 1;
            last;
         }
      }
      if ($hasHistory) {
         emitALine "return true;";
      } else {
         emitALine "return false;";
      }
   }
   --$indent;
   emitALine "}";
}

## @fn void emitClearStreamHistory($model)
# Given an operator's model, emit the method calls
# to clear up the stream history
# @param model The operator's model
sub emitClearStreamHistory($)
{
   my ($model) = @_;

   for (my $i = 0; $i < $model->getNumberOfInputPorts(); ++$i) {
      my $inputPort = $model->getInputPortAt($i);
      if ($inputPort->hasHistory()) {
         my $memberName = portHistoryMemberName($i);
         emitALine "${memberName}.clear();";
      }
   }

   initHistory($model);
}

## @fn void checkpointRestoreImpls($model)
#  Given an operator's model, emits the checkpoint save/restore
#  method implementations for the operator.
#  @param model The operator's model
sub checkpointRestoreImpls($)
{
   my ($model) = @_;
   my $context = $model->getContext();
   emitALine "void MY_BASE_OPERATOR::checkpointStateVariables(NetworkByteBuffer & opstate) const {";
   ++$indent;
   emitStateVariableCheckpoint($model, "opstate");
   --$indent;
   emitALine "}";
   emitALine "";
   emitALine "void MY_BASE_OPERATOR::restoreStateVariables(NetworkByteBuffer & opstate) {";
   ++$indent;
   emitStateVariableRestore($model, "opstate");
   --$indent;
   emitALine "}";
   emitALine "";
   emitALine "void MY_BASE_OPERATOR::checkpointStateVariables(Checkpoint & ckpt) {";
   ++$indent;
   emitStateVariableCheckpoint($model, "ckpt");
   emitStreamHistoryCheckpoint($model, "ckpt");
   --$indent;
   emitALine "}";
   emitALine "";
   emitALine "void MY_BASE_OPERATOR::resetStateVariables(Checkpoint & ckpt) {";
   ++$indent;
   emitStateVariableRestore($model, "ckpt");
   emitStreamHistoryRestore($model, "ckpt");
   --$indent;
   emitALine "}";
   emitALine "";
   emitALine "void MY_BASE_OPERATOR::resetStateVariablesToInitialState() {";
   ++$indent;
   stateVarInitList($model, 1); # emit the default value assignment
   emitClearStreamHistory($model);
   --$indent;
   emitALine "}";
   emitALine "";
   generateHasStateVariables($model);
   emitALine "";
}

sub checkpointRestoreRawImpls($)
{
   my ($model) = @_;

   emitALine "void MY_BASE_OPERATOR::resetToInitialStateRaw() {";
   ++$indent;
   emitALine "AutoMutex " . autoPortMutexName() . "(" . stateVarMutexName(). ");";
   emitALine "StateHandler *sh = getContext().getStateHandler();";
   emitALine "if (sh != NULL) {";
   ++$indent;
   emitALine "sh->resetToInitialState();";
   --$indent;
   emitALine "}";
   emitALine "resetStateVariablesToInitialState();";
   --$indent;
   emitALine "}";
   emitALine "";
   emitALine "void MY_BASE_OPERATOR::checkpointRaw(Checkpoint & ckpt) {";
   ++$indent;
   emitALine "AutoMutex " . autoPortMutexName() . "(" . stateVarMutexName(). ");";
   emitALine "StateHandler *sh = getContext().getStateHandler();";
   emitALine "if (sh != NULL) {";
   ++$indent;
   emitALine "sh->checkpoint(ckpt);";
   --$indent;
   emitALine "}";
   # State variables are checkpointed after calling user code, so that the
   # state variable's values are not overwritten by user code
   emitALine "checkpointStateVariables(ckpt);";
   --$indent;
   emitALine "}";
   emitALine "";
   emitALine "void MY_BASE_OPERATOR::resetRaw(Checkpoint & ckpt) {";
   ++$indent;
   emitALine "AutoMutex " . autoPortMutexName() . "(" . stateVarMutexName(). ");";
   emitALine "StateHandler *sh = getContext().getStateHandler();";
   emitALine "if (sh != NULL) {";
   ++$indent;
   emitALine "sh->reset(ckpt);";
   --$indent;
   emitALine "}";
   # State variables are reset after calling user code, so that the
   # state variable's values is not affected by user code
   emitALine "resetStateVariables(ckpt);";
   --$indent;
   emitALine "}";
   emitALine "";


}

## @fn unsigned tupleNeedsMutexAcquire($inputPorts, $cnst)
#  For tuple processing, check if auto port mutex is needed. If any port for the
#  set we are processing has tuple logic then we need a port mutex, also if any
#  port for the set we are processing has history then we need a port mutex
#  @param inputPorts The list of input ports
#  @param cnst A string which is either 'const' or ''.
#  @return Returns 1 if a mutex is needed, otherwise it returns 0.
sub tupleNeedsMutexAcquire($$)
{
    my ($inputPorts, $cnst) = @_;
    my $isConst = (length($cnst) > 0) ? 1 : 0;
    foreach my $port (@$inputPorts) {
        if ($port->hasHistory()) { return 1; }
        next if ($port->getMutationAllowed() && $isConst);
        next if (!$port->getMutationAllowed() && !$isConst);
        if ($port->hasTupleLogic() && $port->generateTupleLogic()) { return 1; }
    }
    return 0;
}

## @fn void tupleMutexAcquire($inputPorts, $cnst)
#  For tuple processing, emits a declaration for an AutoPortMutex if it is needed.
#  @param inputPorts The list of input ports.
#  @param cnst A string which is either 'const' or ''.
sub tupleMutexAcquire($$)
{
    my ($inputPorts, $cnst) = @_;
    if (tupleNeedsMutexAcquire($inputPorts, $cnst)) {
        emitALine "AutoPortMutex " . autoPortMutexName() . "(" . stateVarMutexName() . ", *this);";
    }
}

## @fn void tupleLogic($inputPort)
#  Given an input port, emits the statement for the tuple logic if it has any
#  @param inputPort The input port.
sub tupleLogic($)
{
   my ($inputPort) = @_;
   if ($inputPort->hasTupleLogic() && $inputPort->generateTupleLogic()) {
      emitALine "\n" . $inputPort->getTupleLogic();
   }
}

## @fn void punctLogic($inputPort)
#  Given an input port, emits the statement for the punct logic if it has any
#  @param inputPort The input port.
sub punctLogic($)
{
   my ($inputPort) = @_;
   if ($inputPort->hasPunctLogic()) {
       emitALine "\n" . $inputPort->getPunctLogic();
   }
}

# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
# # @fn void watermarkLogic($inputPort)
#  Given an input port, emits the statement for the watermark logic if it has any
#  @param inputPort The input port.

# sub watermarkLogic($)
# {
#    my ($inputPort) = @_;
#    if ($inputPort->hasWatermarkLogic()) {
#        emitALine "\n" . $inputPort->getWatermarkLogic();
#    }
# }

## @fn void processMethods($model, $constGen, $nonConstGen, $punctGen)
#  Given a set of pointers to functions that declare the various process???
#  methods, call the appropriate ones based on the given model.
#  @param model The operator's model
#  @param constGen The function that generates code for a const tuple
#  @param nonConstGen The function that generates code for a non-const tuple
#  @param punctGen The function that generates code for punctuation.
sub processMethods($$$$)
{
   my ($model, $constGen, $nonConstGen, $punctGen) = @_;
   my $needToGenConst = 1;
   my $needToGenNonConst = 1;
   # we need to generate one or two process methods, depending on the number of
   # ports and whether or not they are mutatable
   for (my $i = 0; $i < $model->getNumberOfInputPorts() &&
        ($needToGenConst || $needToGenNonConst); ++$i) {
      my $port = $model->getInputPortAt($i);
      if ($needToGenConst && ! $port->getMutationAllowed()) {
         &$constGen($model);
         $needToGenConst = 0;
      }
      if ($needToGenNonConst && $port->getMutationAllowed()) {
         &$nonConstGen($model);
         $needToGenNonConst = 0;
      }
   }

   # If we have input ports then we need a processRaw for punctuation
   if ($model->getNumberOfInputPorts() > 0) {
      emitALine "";
      &$punctGen($model);
   }
}

## @fn void submitMethods($model, $nonConstGen, $constGen, $punctGen)
#  Given a set of pointers to functions that declare the various submit functions,
#  call the generator to generate the mutating tuple submit if the operator has
#  a mutating output port, and the generators to generate the non-mutating
#  tuple submit if the operator has a non-mutating output port.  The goal is
#  to mask the unused method to catch unintended calls at compile time.
#
#  a submit method for punctuations is also declared.
#
#  @param model The operator's model
#  @param nonConstGen The function that generates code for a non-const submit
#  @param constGen The function that generates code for a const submit
#  @param punctGen The function that generates code for a punctuation submit
sub submitMethods($$$$)
{
   my ($model, $nonConstGen, $constGen, $punctGen) = @_;
   my $hasConst = 0;
   my $hasNonConst = 0;
   my $outputPortCount = $model->getNumberOfOutputPorts();
   for (my $i = 0; $i < $outputPortCount &&
        (! $hasConst || ! $hasNonConst); ++$i) {
       my $port = $model->getOutputPortAt($i);
       $hasConst = 1 if (! $port->getMutationAllowed());
       $hasNonConst = 1 if ($port->getMutationAllowed());
   }
   if (!$hasConst && !$hasNonConst) {
       $hasConst = 1;
       $hasNonConst = 1;
   }
   if ($hasNonConst) {
       &$nonConstGen($model)
   }
   if ($hasConst) {
       &$constGen($model)
   }

   &$punctGen($model);
   emitALine "";
}

## @fn string cppNamespaceScope($class)
#  Given an operator's name, produces the complete namespace used by that operator
#  @param class The operator's name.
#  @return The operator's namepace scope
sub cppNamespaceScope($)
{
   my ($class) = @_;
   my $ns = namespaceName($class);
   my $dir = "SPL::_Operator";
   $dir .= "::$ns" if length($ns) > 0;
   return $dir;
}

## @fn void usingDeclaration($class)
#  Given an operator's name, produces a using declaration in the form of using SPL::_Operator::a::b;
#  @param class The operator's name.
sub usingDeclaration($)
{
   my ($class) = @_;
   emitALine "using namespace " . cppNamespaceScope($class) . ';';
}

## @fn void operatorBaseClassStaticIniter($model)
#  Given an operator's model, emits the operator's base class static initers.
#  @param model The operator's model.
sub operatorBaseClassStaticIniter($)
{
   my ($model) = @_;
   my $className = $model->getContext()->getClass();
   emitALine "static SPL::Operator * initer() { return new MY_OPERATOR_SCOPE::MY_OPERATOR(); }";
   emitALine "bool MY_BASE_OPERATOR::globalInit_ = MY_BASE_OPERATOR::globalIniter();";
   emitALine "bool MY_BASE_OPERATOR::globalIniter() {";
   emitALine "    instantiators_.insert(std::make_pair(\"$className\",&initer));";
   emitALine "    return true;";
   emitALine "}";
}

## @fn void operatorBaseClassCtorDef($model, $generic)
#  Given an operator's model, emits the operator's base class constructor definition.
#  @param model The operator's model.
#  @param generic Whether this is a generic operator or not. If non-generic, the
#  ctor will include initialization code for parameter access helpers
sub operatorBaseClassCtorDef($$)
{
   my ($model, $generic) = @_;
   # emit template function for runtime constant initialization
   emitALine "template<class T> static void initRTC (SPL::Operator& o, T& v, const char * n) {";
   emitALine "    SPL::ValueHandle vh = v;";
   emitALine "    o.getContext().getRuntimeConstantValue(vh, n);";
   emitALine "}";
   emitALine "";
   emitALine "MY_BASE_OPERATOR::MY_BASE_OPERATOR()";
   my $eventTimeInitializers = eventTimeInitialize($model);
   emitALine " : Operator() $eventTimeInitializers {";
   ++$indent;
   emitALine "uint32_t index = getIndex();";
   consistentRegionInitList($model);
   runtimeConstantInitList($model);
   stateVarInitList($model, 0); # do not emit the default value assignment
   parameterValueInitList($model);
   initHistory($model);
   initBitsets($model);
   initCustomMetrics($model);
   --$indent;
   emitALine  "}";
   emitALine "MY_BASE_OPERATOR::~MY_BASE_OPERATOR()";
   emitALine "{";
   ++$indent;
   emitALine "for (ParameterMapType::const_iterator it = paramValues_.begin(); it != paramValues_.end(); it++) {";
   ++$indent;
   emitALine "const ParameterValueListType& pvl = it->second;";
   emitALine "for (ParameterValueListType::const_iterator it2 = pvl.begin(); it2 != pvl.end(); it2++) {";
   ++$indent;
   emitALine  "delete *it2;";
   --$indent;
   emitALine  "}";
   --$indent;
   emitALine  "}";
   --$indent;
   emitALine  "}";
}

## @fn void initBitsets($model)
#  Given an operator's model, emits the initializers for the final punctuation bitset if needed.
#  @param model The operator's model.
sub initBitsets($)
{
   my ($model) = @_;
   my $numInputPorts = $model->getNumberOfInputPorts();
   if ($numInputPorts > 0) {
      my $bitsetName = bitsetMemberName();
      my $numBits = $numInputPorts+1;
      # For each output port we need to init the defined bitset
      my $numOutputPorts = $model->getNumberOfOutputPorts();
      for (my $i = 0; $i < $numOutputPorts; ++$i) {
         my $oport = $model->getOutputPortAt($i);
         # Build the initialization string
         my $init = "";
         for (my $j = 0; $j < $numInputPorts; ++$j) {
            $init = (($oport->getFinalPunctuationPortScopes()->{$j}) ? "1": "0") . $init;
         }
         if ($oport->getNumberOfFinalPunctuationPorts()==0) {
             $init = "1" . $init;
         } else {
             $init = "0" . $init;
         }
         if ($numOutputPorts > 1) {
            emitALine "${bitsetName}.push_back(OPortBitsetType(std::string(\"${init}\")));";
         } else {
            emitALine "${bitsetName} = OPortBitsetType(std::string(\"${init}\"));";
         }
      }
   }
}

## @fn void initHistory($model)
#  Given the operator's model, emits an initializer for the history member if needed.
#  @param model The operator's model.
sub initHistory($)
{
   my ($model) = @_;

   # For each input port that has history, initialize the history queue
   for (my $i = 0; $i < $model->getNumberOfInputPorts(); ++$i) {
      my $inputPort = $model->getInputPortAt($i);
      if ($inputPort->hasHistory()) {
          emitALine "";
          my $size = $inputPort->getHistory();
          emitALine "{";
          ++$indent;
          my $portType = inputPortTypeName($i);
          emitALine "${portType} v;";
          emitALine "v.clear();";
          my $typeName = portHistoryTypeName($i);
          emitALine "for (${typeName}::size_type i = 0; i <= $size; ++i) {";
          ++$indent;
          my $memberName = portHistoryMemberName($i);
          emitALine "${memberName}.push_front(v);";
          --$indent;
          emitALine "}";
          --$indent;
          emitALine "}";
      }
   }
}

## @fn void initCustomMetrics($model)
#  Given the operator's model, emits code to create custom metrics for
#  all input ports with exception handlers.
#  @param model The operator's model
sub initCustomMetrics($)
{
    my ($model) = @_;

    if (mustEmitExceptionBlock($model)) {
        emitALine "OperatorMetrics& om = getContext().getMetrics();";
        # For each input port, create a custom metric.
        for (my $portIndex = 0; $portIndex < $model->getNumberOfInputPorts(); ++$portIndex) {
            my $inputPort = $model->getInputPortAt($portIndex);
            emitALine "metrics_[$portIndex] = &(om.createCustomMetric(\"nExceptionsCaughtPort$portIndex\", \"Number of exceptions caught on port $portIndex\", Metric::Counter));";
        }
    }
}

## @fn genTupleClear($model)
# Generate the code to do a clear efficiently()
# @param model The tuple's model.
sub genTupleClear($)
{
   my ($model) = @_;

   ++$indent;
   for (my $i = 0, my $count = $model->getNumberOfAttributes(); $i < $count; ++$i) {
      my $attr = $model->getAttributeAt($i);
      my $name =  $attr->getName();
      $name = "get_$name()";
      my $cppType =  $attr->getCppType();
      if ($cppType =~ /^SPL::u?int(8|16|32|64)$/) {
          emitALine "$name = 0;";
      } elsif ($cppType =~ /^SPL::(float|decimal|complex)(32|64|128)$/) {
          emitALine "$name = 0;";
      } elsif ($cppType =~ /^SPL::blob$/) {
          emitALine "$name.clear();";
      } elsif ($cppType =~ /^SPL::timestamp$/) {
          emitALine "$name = SPL::timestamp(0);";
      } elsif ($cppType =~ /^SPL::ustring$/) {
          emitALine "$name.remove();";
      } elsif ($cppType =~ /^SPL::rstring$/) {
          # This is what is commonly called the C++ "swap trick". Calling std::string::clear()
          # does not actually deallocate the memory in the string. It just resets the internal
          # pointers. The code below instantiates a temporary, and swaps the tuple attribute
          # with that temporary. When the temporary goes out of scope, the data gets deleted,
          # and the tuple attribute contains an empty string.
          emitALine "$cppType().swap($name);";
      } elsif ($cppType =~ /^SPL::bstring</) {
          emitALine "$name = \"\";";
      } elsif ($cppType =~ /^SPL::boolean/) {
          emitALine "$name = false;";
      } elsif ($cppType =~ /^SPL::list</) {
          # For std::list, the swap trick is generally more expensive than calling
          # std::list::clear(). We're trying to mitigate this performance hit by calling clear()
          # on "small" lists, but doing the swap trick on "large" lists. This keeps a (hopefully)
          # reasonable upper bound on memory use without hurting performance.
          emitALine "if ($name.size() < 1024) {";
          ++$indent;
          emitALine "$name.clear();";
          --$indent;
          emitALine "}";
          emitALine "else {";
          ++$indent;
          emitALine "$cppType().swap($name);";
          --$indent;
          emitALine "}";
      } elsif ($cppType =~ /^SPL::blist</) {
          # Note: no swap trick.
          emitALine "$name.clear();";
      } elsif ($cppType =~ /^SPL::b?(set|map)</) {
          emitALine "$name.clear();";
      } else {
          # all that is left is enums, tuples and optionals
          emitALine "$name.clear();";
      }
   }
   --$indent;
}

## @fn void consistentRegionInitList($model)
# Given an operator's model, emits an initializer for each variable required to
# control Drain and Reset punctuation forwarding
# @param model The operator's model.
sub consistentRegionInitList($)
{
   my ($model) = @_;
   if ($model->getContext()->getOptionalContext("ConsistentRegion")) {
      emitALine ccContextName() .
         " = static_cast<ConsistentRegionContext *>(getContext().getOptionalContext(CONSISTENT_REGION));";
   }
}

## @fn void runtimeConstantInitList($model)
#  Given an operator's model, emits an initializer for each runtime constant member if needed.
#  @param model The operator's model.
sub runtimeConstantInitList($)
{
   my ($model) = @_;
   my $context = $model->getContext();
   for (my $i = 0; $i < $context->getNumberOfRuntimeConstants(); ++$i ) {
      my $constant = $context->getRuntimeConstantAt($i);
      emitALine "initRTC(*this, " . $constant->getName() . ', "' . $constant->getName() . '");';
   }
}

## @gn void emitDefaultInitValue($var)
# Given a StateVariable, emits the variable assignment to its
# default value.
# @param var StateVariable
sub emitVarDefaultAssignment($)
{
   my ($var) = @_;

   my $type = $var->getSPLType();
   if ($type eq "boolean") {
      emitALine $var->getName() . " = false;";
   } elsif ($type eq "int8" || $type eq "int16" ||
            $type eq "int32" || $type eq "int64" ||
            $type eq "uint8" || $type eq "uint16" ||
            $type eq "uint32" || $type eq "uint64") {
      emitALine $var->getName() . " = 0;";
   } elsif ($type eq "float32" || $type eq "float64") {
      emitALine $var->getName() . " = 0.0;";
   } else {
      emitALine $var->getName() . " = " . $var->getCppType() . "();";
   }
}

## @fn void stateVarInitList($model)
#  Given an operator's model, emits the initializer list for its attribute ctor.
#  @param model The operator's model.
#  @param emitDefault emit the default value assignment
sub stateVarInitList($$)
{
   my ($model,$emitDefault) = @_;
   my $context = $model->getContext();
   for (my $i = 0; $i < $context->getNumberOfStateVariables(); ++$i) {
      my $var = $context->getStateVariableAt($i);
      if ($var->hasValue()) {
         emitALine $var->getName() . " = " . $var->getValue()->getCppExpression() . ";";
         emitALine "SPLAPPTRC(L_DEBUG, \"Variable: " . $var->getName() . " Value: \" << " . $var->getName() . ",SPL_OPER_DBG);";
      } else {
         if ($emitDefault) {
            emitVarDefaultAssignment($var);
         }
      }
   }
}

## @fn void defineConstProcessRaw($model)
#  Given an operator's model, emits the definition of the constProcessRaw method.
#  @param model The operator's model.
sub defineConstProcessRaw($)
{
   my ($model) = @_;
   defineProcessRaw($model, "const ");
}

## @fn void defineNonConstProcessRaw($model)
#  Given an operator's model, emits the definition of the constProcessRaw method.
#  @param model The operator's model.
sub defineNonConstProcessRaw($)
{
   my ($model) = @_;
   defineProcessRaw($model, "");
}

## @fn void historyUpdate($inputPort)
#  Given an input port, emits the code to update the history member variable if needed.
#  @param inputPort The input port.
sub historyUpdate($)
{
   my ($inputPort) = @_;
   if ($inputPort->hasHistory()) {
      my $memberName = portHistoryMemberName($inputPort->getIndex());
      emitALine "${memberName}.push_front(" . $inputPort->getCppTupleName() . ");";
      my $maxSize = $inputPort->getHistory() + 1;
      emitALine "if (${memberName}.size() > ${maxSize}) { ${memberName}.pop_back(); }";
   }
}

## @fn void tuplePortProcessing($inputPort, $cnst, $allInputPorts)
#  Emits the port processing logic for a given port.
#  @param inputPort The input port
#  @param cnst A string that is either 'const' or ''.
#  @param allInputPorts All the input ports.
sub tuplePortProcessing($$$)
{
   my ($inputPort, $cnst, $allInputPorts) = @_;
   my $cast = "static_cast<$cnst MY_OPERATOR_SCOPE::MY_OPERATOR*>(this)->MY_OPERATOR::";
   my $index = $inputPort->getIndex();
   if (($inputPort->hasTupleLogic() && $inputPort->generateTupleLogic()) || $inputPort->hasHistory()) {
       my $portTypeName = inputPortTypeName($index);
       my $tupleType = $inputPort->getCppTupleName();
       emitALine "${portTypeName} $cnst& ${tupleType} = static_cast<${portTypeName} $cnst &>(tuple);";
       tupleMutexAcquire($allInputPorts, $cnst);
       historyUpdate($inputPort);
       tupleLogic($inputPort);
   }
}

## @fn void punctPortProcessing($inputPort)
#  Given an input port, emits the logic to process punctuation.
#  @param inputPort The input port.
sub punctPortProcessing($)
{
   my ($inputPort) = @_;
   emitALine "AutoPortMutex " . autoPortMutexName() . "(" . stateVarMutexName() . ", *this);";
   punctLogic($inputPort);
}

# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
# # @fn void watermarkPortProcessing($inputPort)
#  Given an input port, emits the logic to process watermark
#  @param inputPort The input port.

# sub watermarkPortProcessing($)
# {
#    my ($inputPort) = @_;
#    emitALine "AutoPortMutex " . autoPortMutexName() . "(" . stateVarMutexName() . ", *this);";
#    watermarkLogic($inputPort);
# }

## @fn void tupleCaseStatementBody($inputPort, $cnst, $allInputPorts)
#  Emits the case statement body for the processing logic of an input port
#  @param inputPort The input port.
#  @param cnst A string that is either 'const' or ''.
#  @param allInputPorts All the input ports.
sub tupleCaseStatementBody($$$)
{
   my ($inputPort, $cnst, $allInputPorts) = @_;
   my $index = $inputPort->getIndex();
   if (($inputPort->hasTupleLogic() && $inputPort->generateTupleLogic()) || $inputPort->hasHistory()) {
       emitALine "case $index:";
       emitALine "{";
       ++$indent;
       tuplePortProcessing($inputPort, $cnst, $allInputPorts);
       --$indent;
       emitALine "}";
       emitALine "break;";
   }
}

## @fn void punctCaseStatementBody($inputPort)
#  Emits the case statement body for the punctuation processing of an input port
#  @param inputPort The input port.
sub punctCaseStatementBody($)
{
   my ($inputPort) = @_;
   my $index = $inputPort->getIndex();
   if($inputPort->hasPunctLogic()) {
       emitALine "case $index:";
       emitALine "{";
       ++$indent;
       punctPortProcessing($inputPort);
       --$indent;
       emitALine "}";
       emitALine "break;";
   }
}

# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
# # @fn void watermarkCaseStatementBody($inputPort)
#  Emits the case statement body for the watermark processing of an input port
#  @param inputPort The input port.

# sub watermarkCaseStatementBody($)
# {
#    my ($inputPort) = @_;
#    my $index = $inputPort->getIndex();
#    if($inputPort->hasWatermarkLogic()) {
#        emitALine "case $index:";
#        emitALine "{";
#        ++$indent;
#        watermarkPortProcessing($inputPort);
#        --$indent;
#        emitALine "}";
#        emitALine "break;";
#    }
# }

## @fn void tupleCaseStatement($inputPort, $cnst, $allInputPorts)
#  Emits a case statement for tuple processing in the processRaw method.
#  @param inputPort The input port.
#  @param cnst A string that is either 'const' or ''.
#  @param allInputPorts All the input ports.
sub tupleCaseStatement($$$)
{
   my ($inputPort, $cnst, $allInputPorts) = @_;
   my $isConst = (length($cnst) > 0) ? 1 : 0;

   # We generate a case statement body when the input port is mutating and
   # we are generating the non-const processRaw.
   if ($inputPort->getMutationAllowed() && !$isConst) {
      tupleCaseStatementBody($inputPort, $cnst, $allInputPorts);
   }
   # or we are a non-mutating port and we are generating the const processRaw
   elsif (!$inputPort->getMutationAllowed() && $isConst) {
      tupleCaseStatementBody($inputPort, $cnst, $allInputPorts);
   }
}

## @fn void tupleSwitchStatement($inputPorts, $cnst, $allInputPorts)
#  Given a list of input ports, emits the switch statement for input port tuple processing.
#  @param inputPorts The list of input ports.
#  @param cnst A string that is either 'const' or ''.
#  @param allInputPorts The list of all input ports.
sub tupleSwitchStatement($$$)
{
   my ($inputPorts, $cnst, $allInputPorts) = @_;
   emitALine "switch(port) {";
   ++$indent;
   foreach my $port (@$inputPorts) {
      tupleCaseStatement($port, $cnst, $allInputPorts);
   }
   --$indent;
   emitALine "}";
}

## @fn void punctSwitchStatement($inputPorts)
#  Given a list of input ports, emits the switch statement for input port punctuation processing.
#  @param inputPorts The input ports list.
sub punctSwitchStatement($)
{
   my ($inputPorts) = @_;
   emitALine "switch(port) {";
   ++$indent;
   foreach my $port (@$inputPorts) {
      punctCaseStatementBody($port);
   }
   --$indent;
   emitALine "}";
}

## @fn void watermarkSwitchStatement($inputPorts)
#  Given a list of input ports, emits the switch statement for input port watermark processing.
#  @param inputPorts The input ports list.
sub watermarkSwitchStatement($)
{
   my ($inputPorts) = @_;
   emitALine "switch(port) {";
   ++$indent;
   foreach my $port (@$inputPorts) {
      watermarkCaseStatementBody($port);
   }
   --$indent;
   emitALine "}";
}

## @fn void handlePunctLogic($inputPorts)
#  For punct processing, adds the switch statements that contains the punct logic
#  and produces a declaration for an AutoPortMutex before that if it is needed
#  @param inputPorts The input ports list.
sub handlePunctLogic($)
{
    my ($inputPorts) = @_;
    my @ports = ();
    foreach my $port (@$inputPorts) {
# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
#        if ($port->hasPunctLogic() || $port->hasWatermarkLogic()) { $ports[$#ports+1] = $port; }
        if ($port->hasPunctLogic()) { $ports[$#ports+1] = $port; }
    }

   emitALine "void MY_BASE_OPERATOR::punctLogic(Punctuation const & punct, uint32_t port) {";
   ++$indent;
    if (scalar(@ports)>0) {
        if(scalar(@$inputPorts)>1) {
            punctSwitchStatement(\@ports);
        } else {
            punctPortProcessing($ports[0]);
        }
    }
   --$indent;
   emitALine "}\n";

# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
   # emitALine "void MY_BASE_OPERATOR::watermarkLogic(Watermark const & wm, uint32_t port) {";
   # ++$indent;
   #  if (scalar(@ports)>0) {
   #      if(scalar(@$inputPorts)>1) {
   #          watermarkSwitchStatement(\@ports);
   #      } else {
   #          watermarkPortProcessing($ports[0]);
   #      }
   #  }
   # --$indent;
   # emitALine "}\n";
}

## @fn void definePunctNoPermitProcess($model)
#  Given an operator's model, emits the definition of the processRaw for punctuation handling for
#  consistent ports.
#  @param model The operator's model.
sub definePunctNoPermitProcess($)
{
  my ($model) = @_;
  emitALine "void MY_BASE_OPERATOR::punctNoPermitProcessRaw(Punctuation const & punct, uint32_t port) {";
  ++$indent;
  emitALine "switch(punct) {";
  emitALine "case Punctuation::WindowMarker:";
  ++$indent;
  emitALine "punctLogic(punct, port);";
  emitALine "process(punct, port);";
  emitALine "break;";
  --$indent;
  emitALine "case Punctuation::FinalMarker:";
  ++$indent;
  emitALine "punctLogic(punct, port);";
  handleFinalPunctuation($model);
  --$indent;
  emitALine "case Punctuation::DrainMarker:";
  emitALine "case Punctuation::ResetMarker:";
  emitALine "case Punctuation::ResumeMarker:";
  ++$indent;
  handleConsistentPunctuations($model);
  emitALine "break;";
  --$indent;
  emitALine "case Punctuation::SwitchMarker:";
  ++$indent;
  handleSwitchPunctuation($model);
  emitALine "break;";
  --$indent;

  emitALine "case Punctuation::WatermarkMarker:";
  ++$indent;
  eventTimeHandleWatermark($model);
  emitALine "break;";
  --$indent;

  emitALine "default:";
  ++$indent;
  emitALine "break;";
  --$indent;
  emitALine "}";
  --$indent;
  emitALine "}";
  emitALine "";
}

## @fn void definePunctPermitProcess($model)
#  Given an operator's model, emits the definition of the processRaw for punctuation handling for
#  consistent ports.
#  @param model The operator's model.
sub definePunctPermitProcess($)
{
  my ($model) = @_;
  emitALine "void MY_BASE_OPERATOR::punctPermitProcessRaw(Punctuation const & punct, uint32_t port) {";

  ++$indent;
  emitALine "{";
  ++$indent;
  if ($model->getContext()->getOptionalContext("ConsistentRegion")) {
    emitALine "ConsistentRegionPermit cct(" . ccContextName() . ");";
  }
  emitALine "punctNoPermitProcessRaw(punct, port);";
  --$indent;
  emitALine "}";
  --$indent;
  emitALine "}";
  emitALine "";
}

## @fn void definePunctProcessRaw($model)
#  Given an operator's model, emits the definition of the processRaw for punctuation handling.
#  @param model The operator's model.
sub definePunctProcessRaw($)
{
   my ($model) = @_;
   my $context = $model->getContext();
   my $inputPorts = $model->getInputPorts();
   handlePunctLogic($inputPorts);
   definePunctPermitProcess($model);
   definePunctNoPermitProcess($model);
   emitALine "void MY_BASE_OPERATOR::processRaw(Punctuation const & punct, uint32_t port) {";

   ++$indent;
   emitALine "switch(port) {";
   foreach my $port (@$inputPorts) {
      my $portIndex = $port->getIndex();
      emitALine "case $portIndex:";
      ++$indent;
      my $caContext = $model->getContext()->getOptionalContext("ConsistentRegion");
      my $isInRegion = 1 if $caContext;
      if (!$isInRegion || ($isInRegion && $caContext->isConsistentInputPort($portIndex)) || isJavaOperator($context)) {
          emitALine "punctNoPermitProcessRaw(punct, port);";
      } else {
          emitALine "punctPermitProcessRaw(punct, port);";
      }
      emitALine "break;";
      --$indent;
   }
   emitALine "}";
   --$indent;
   emitALine "}\n";
}

# Return if the operator is a Java primitive or an
# invocation of JavaOp.
# @param context The operators invocation context
sub isJavaOperator($)
{
   my ($context) = @_;
   # If it's a Java operator than pass the punct into
   # the operator.
   return ( ( $context->getKind() eq 'spl.utility::JavaOp' )
       ||
       ( defined $context->{xml_}->{javaOperatorModelDirectory} )
      );
}

## @fn void handleSwitchPunctuation($mode)
# Given an operator's model, emits the code to handle the Switch punctuation.
# @param Model the operator's model
sub handleSwitchPunctuation($)
{
   my ($model) = @_;
   my $context = $model->getContext();

   # If it's a Java operator then pass the punct into
   # the operator.
   if (isJavaOperator($context))
   {
       emitALine "javaopInternalAction(punct, port);";
   }
}

## @fn void handleConsistentPunctuation($model)
# Given an operator's model, emits the code to manage Drain/Reset/Resume punctuation.
# @param model The operator's model
sub handleConsistentPunctuations($)
{
   my ($model) = @_;
   my $context = $model->getContext();

   my $ccContext = $context->getOptionalContext("ConsistentRegion");
   my $isInRegion = 1 if $ccContext;
   if (!$isInRegion || $ccContext->isStartOfRegion()) {
      return;
   }
   my $numOutputPorts = $model->getNumberOfOutputPorts();

   emitALine "APPTRC(L_DEBUG, \"Pre-processing \" << punct << \" at port \" << port, SPL_OPER_DBG);";
   emitALine "if (" . ccContextName() . "->mustForwardMarker(punct, port)) {";
   ++$indent;
   emitALine "APPTRC(L_DEBUG, \"Going to forward\", SPL_OPER_DBG);";
   emitALine "for (uint32_t i = 0; i < $numOutputPorts; i++)";
   ++$indent;
   emitALine "autoSubmit(punct, i);";
   --$indent;
   emitALine ccContextName() . "->forwardedMarker(punct, port);";
   --$indent;
   emitALine "}";
   emitALine "APPTRC(L_DEBUG, \"Post processing\", SPL_OPER_DBG);";
   emitALine ccContextName() ."->postMarkerProcessing(punct, port);";

   # If it's a Java operator than pass the punct into
   # the operator.
   if (isJavaOperator($context))
   {
       emitALine "javaopInternalAction(punct, port);";
   }

}

## @fn void handleFinalPunctuation($model)
#  Given an operator's model, emits the code to manage final punctuation.
#  @param model The operator's model.
sub handleFinalPunctuation($)
{
   my ($model) = @_;

   my $numOutputPorts = $model->getNumberOfOutputPorts();
   if ($numOutputPorts == 0) {
       emitALine "process(punct, port);";
       emitALine "break;";
       return;
   }
   emitALine  "{";
   ++$indent;
   emitALine "process(punct, port);";

   # If it is not a java operator, and it has an event-time context,
   # submit a watermark with the max value to trigger any remaining
   # timeInterval windows.
   if (!isJavaOperator($model->getContext()) && hasEventTimeContext($model)) {
       emitALine "// Set the watermark to the max watermark value to trigger any remaining timeInterval windows";
       emitALine "eventTimeContext_.submit(std::numeric_limits<SPL::Watermark>::max());"
   }

   if ($numOutputPorts > 1) {
       emitALine "for (uint32_t i = 0; i < ${numOutputPorts}; ++i) {";
       ++$indent;
   }
   my $memberName = bitsetMemberName();
   my $index = ($numOutputPorts > 1) ? "[i]": "";
   my $oportNum = ($numOutputPorts > 1) ? "i": "0";
   emitALine "bool forward = false;";
   emitALine "{";
   ++$indent;
   emitALine "AutoPortMutex " . autoPortMutexName() . "(" . finalPunctMutexName() . ", *this);";
   emitALine "${memberName}${index}.reset(port);";
   emitALine "if (${memberName}${index}.none()) {";
   ++$indent;
   emitALine "${memberName}${index}.set(" . $model->getNumberOfInputPorts() . ");";
   emitALine "forward=true;";
   --$indent;
   emitALine "}";
   --$indent;
   emitALine "}";
   emitALine "if(forward)";
   ++$indent;
   emitALine "submit(punct, ${oportNum});";
   --$indent;
   if ($numOutputPorts > 1) {
       --$indent;
       emitALine "}";
   }
   emitALine "return;";
   --$indent;
   emitALine "}";
}

## @fn void tupleProcessRawSwitchStatement($model, $cnst)
#  Emits a definition for a processRaw method for tuple processing
sub tupleProcessRawSwitchStatement($)
{
   my ($model) = @_;
   my $context = $model->getContext();
   ++$indent;
   emitALine "switch(port) {";
   my $inputPorts = $model->getInputPorts();
   foreach my $port (@$inputPorts) {
      my $portIndex = $port->getIndex();
      emitALine "case $portIndex:";
      ++$indent;
      # For Java operators permits are managed in Java.
      my $needPermit = !isJavaOperator($context) &&
               !$model->getContext()->getOptionalContext("ConsistentRegion")->isConsistentInputPort($portIndex);

      if ($needPermit) {
         emitALine "{";
         ++$indent;
         emitALine "ConsistentRegionPermit cct(" . ccContextName() . ");";
      }
      emitALine "tupleLogic (tuple, port);";
      emitALine "static_cast<MY_OPERATOR_SCOPE::MY_OPERATOR*>(this)->MY_OPERATOR::process(tuple, port);";
      if ($needPermit) {
         --$indent;
         emitALine "}";
      }
      emitALine "break;";
      --$indent;
   }
   --$indent;
   emitALine "}";
}

## @fn void defineProcessRaw($model, $cnst)
#  Emits a definition for a processRaw method for tuple processing
#  @param model The operator's model.
#  @param cnst A string that is either 'const' or ''.
sub defineProcessRaw($$)
{
   my ($model, $cnst) = @_;
   my $isConst = (length($cnst) > 0) ? 1 : 0;

   my $inputPorts = $model->getInputPorts();

   # figure out how many case statements would be generated.  If only one
   # then don't generate a switch
   my @ports = ();
   foreach my $port (@$inputPorts) {
      if ($port->getMutationAllowed() && !$isConst) { $ports[$#ports+1] = $port; }
      elsif (!$port->getMutationAllowed() && $isConst) { $ports[$#ports+1] = $port; }
   }

   emitALine "void MY_BASE_OPERATOR::tupleLogic(Tuple " . $cnst . "& tuple, uint32_t port) {";
   ++$indent;

   if ($#ports > 0) {
      tupleSwitchStatement(\@ports, $cnst, $inputPorts);
   } else {
      tuplePortProcessing($ports[0], $cnst, $inputPorts);
   }
   --$indent;
   emitALine "}\n";

   emitALine "";
   emitALine "void MY_BASE_OPERATOR::processRaw(Tuple " . $cnst . "& tuple, uint32_t port) {";

   emitCatchExceptionBegin($model);

   my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
   ++$indent;
   if ($isInConsistentRegion) {
      tupleProcessRawSwitchStatement($model);
   } else {
      emitALine "tupleLogic (tuple, port);";
      emitALine "static_cast<MY_OPERATOR_SCOPE::MY_OPERATOR*>(this)->MY_OPERATOR::process(tuple, port);";
   }
   --$indent;

   emitCatchExceptionEnd($model);

   emitALine "}\n";
}

## @fn string location(@params)
#  Returns the source location as a formatted string ready
#  to print if the last argument is a source location.
#  @param params A set of parameters the last of which can be a source location
#  @return Returns the formatted location or an empty string.
sub location
{
   my ($params) = @_; # a ref to the arg list of the caller
   my $sloc = @$params[scalar(@$params)-1];
   my $location = "";
   if (isSourceLocation($sloc) && $sloc->isValid()) {
      $location = sprintf("%s:%d:%d: ", $sloc->getFileName(), $sloc->getLine(), $sloc->getColumn());
      pop @$params;
   }
   return $location;
}

## @fn unsigned isSourceLocation($sloc)
#  @param sloc The potential source location
#  @return Returns 1 if the argument is a source location, otherwise it
#  returns 0.
sub isSourceLocation($)
{
    my ($sloc) = @_;
    if (blessed($sloc) && $sloc->isa('SPL::Operator::Instance::SourceLocation')) {
        return 1;
    }
    return 0;
}

## @fn void codeGenSignalHandler()
#  A function that handles signals during code generation
sub codeGenSignalHandler {
  $$rrc = 2;
  SPL::Helper::SPLStdErrLn(SPL::Msg::DISABLED_INTERRUPTS("$0"));
}

my %ORIGSIG;

## @fn void disableInterruptRequests
#  This function disables interrupts by installing a signal handler
sub disableInterruptRequests($) {

  ($rrc) = @_;

  $ORIGSIG{INT} = $SIG{INT};
  $ORIGSIG{QUIT} = $SIG{QUIT};
  $ORIGSIG{TERM} = $SIG{TERM};

  $SIG{INT} = \&codeGenSignalHandler;
  $SIG{QUIT} = \&codeGenSignalHandler;
  $SIG{TERM} = \&codeGenSignalHandler;
}

## @fn void enableInterruptRequests()
#  This function re-enables interrupts by restoring the original signal handlers.
sub enableInterruptRequests {
   $SIG{INT} = $ORIGSIG{INT} if defined $ORIGSIG{INT};
   $SIG{QUIT} = $ORIGSIG{QUIT} if defined $ORIGSIG{QUIT};
   $SIG{TERM} = $ORIGSIG{TERM} if defined $ORIGSIG{TERM};
}

## @fn string ensureValue($type, $expn)
#  This function ensures that boolean expressions are casted to SPL::boolean
#  @param type Expression type
#  @param expn Expression to be handled
sub ensureValue ($$) {
    my ($type, $expn) = @_;
    if (SPL::CodeGen::Type::isBoolean($type)) {
        return "static_cast<SPL::boolean>($expn)";
    }
    return $expn;
}

## @fn unsigned checkTypeMatches($concreteType, $type, $possibleTypes)
#  This function checks to see if the actual type is one of the valid types
#  @param concreteType Actual type to be checked
#  @param type SPL type or array of SPL types to check against
#  @param possibleTypes set to string of actual types (for error messages)
#  @return 1 if the type matches, else 0.
sub checkTypeMatches ($$$)
{
    my ($concreteType, $type, $possibleTypes) = @_;
    if (ref($type) eq 'ARRAY') {
        foreach my $t (@$type) {
            return 1 if $concreteType eq $t;
            if (!defined($$possibleTypes)) {
                $$possibleTypes = "'$t'";
            } else {
                $$possibleTypes .= "/'$t'";
            }
        }
        return 0;
    }
    $$possibleTypes = "'$type'";
    return $concreteType eq $type;
}

## @fn void warnPartitionEvictionPolicyIgnored ($window)
#  This function will generate a nice warning if a partition eviction policy is used,
#  and the operator isn't using it
#  @param window window with a partition eviction policy
sub warnPartitionEvictionPolicyIgnored ($)
{
    my ($window) = @_;
    my $kind = $window->getPartitionEvictionPolicyKind();
    my $expn = $kind eq 'partitionAge' ? $window->getPartitionEvictionPolicyAge()
                                       : $window->getPartitionEvictionPolicyCount();
    SPL::CodeGen::warnln (SPL::Msg::PARTITION_EVICTION_POLICY_IGNORED(
                                $kind), $expn->getSourceLocation());
}

sub runtimePathImpl($$$)
{
    my ($model, $path, $varName) = @_;
    # Walk the path of each toolkit and see if the path we are given is in one of them
    my $context = $model->getContext();
    my $toolkits = $context->getToolkits();
    foreach my $toolkit (@{$toolkits}) {
        my $rootDir = $toolkit->getRootDirectory();
        my $rootDirLen = length($rootDir);
        if (length($path) > $rootDirLen && substr($path, 0, $rootDirLen) eq $rootDir) {
            # We found a toolkit that contains this path
            my $relativePath = substr($path, $rootDirLen);
            my $toolkitName = $toolkit->getName();
            emitALine("   $varName = getPE().getOutputDirectory();");
            emitALine("   $varName += \"/toolkits\";");
            emitALine("   $varName += \"/$toolkitName\";");
            emitALine("   $varName += \"$relativePath\";");
            return;
        }
    }
    emitALine("  $varName = \"$path\";");
}

sub evaluateExpression($$)
{
    my ($model, $expr) = @_;
    return undef unless ($expr->getType() eq 'rstring');
    my $evaluator = SPL::Operator::Instance::ExpressionTreeEvaluator->new($model);
    return $evaluator->evaluateExpression($expr);
}

1;
