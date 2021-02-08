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

## @namespace SPL
# \brief Namespace that serves as the base for all %SPL code generation APIs.

## @mainpage SPL Operator Code Generation API Documentation
#
# @section intro_sec Introduction
#
# This is the %SPL Operator Code Generation API documentation. This API is used
# to develop generic %SPL primitive operators using code generator templates
# (.cgt files), in order to achieve a level of reusability and genericity that
# is not available to %SPL primitive operators written in pure C++ or Java.
#
# There are three main classes of interest in this API:
#   - SPL::CodeGen: Provides helper routines for common code generation tasks
#     (such as @link SPL::CodeGen::errorln errorln@endlink, @link
#     SPL::CodeGen::emitClass emitClass@endlink, or @link
#     SPL::CodeGen::getOutputTupleCppInitializer
#     getOutputTupleCppInitializer@endlink)
#   - SPL::CodeGen::Type: Provides helper routines for common type inspection
#     tasks (such as @link SPL::CodeGen::Type::isPrimitive isPrimitive@endlink or
#     @link SPL::CodeGen::Type::getElementType getElementType@endlink)
#   - SPL::Operator::Instance::OperatorInstance and its support classes (such as
#     @link SPL::Operator::Instance::Context Context@endlink, @link
#     SPL::Operator::Instance::Parameter Parameter@endlink, etc.): These are used
#     to access the <i>operator instance model</i>.
#
# @subsection oim_sec Operator Instance Model
#
# The operator instance model captures the configuration of the operator
# instance for which code will be generated. The configuration of the operator
# instance is based on its invocation in the %SPL source code after composite
# operator expansion is performed by the compiler.
#
# Additional details about usage of this API can be found in the "Developing
# operators" section of the "Streams Knowledge Center".

## @class SPL::CodeGen
# \brief Class that provides code generation helper utilities.
package SPL::CodeGen;

use strict;
use warnings;
use SPL::Msg;
use SPL::Helper;
use SPL::CodeGenHelper;

our $verboseMode;
our $OLDSTDOUT;
our $USER_ERROR = 10;  # needs to agree with the same define in Utility/Message.h
our $sawError = 0;

## @fn void headerPrologue($model, $includes)
#  Given an operator instance model, produces the prolog code for the
#  operator's header code generator template
#  @param model the operator instance model
#  @param includes optional lines (passed in as a list reference) to be
#  generated after the inclusion guards, but before any includes
sub headerPrologue($;$)
{
   my ($model, $includes) = @_;
   SPL::CodeGenHelper::headerPrologueImpl($model, 1, $includes);
}

## @fn void headerEpilogue($model)
#  Given an operator instance model, produces the epilog code for the operator's
#  header code generator template
#  @param model the operator instance model
sub headerEpilogue($)
{
   my ($model) = @_;
   SPL::CodeGenHelper::headerEpilogueImpl($model, 1);
}

## @fn void implementationPrologue($model)
#  Given an operator instnce model, produces the prolog code for the operator's
#  implementation code generator template
#  @param model the operator instance model
sub implementationPrologue($)
{
   my ($model) = @_;
   SPL::CodeGenHelper::implementationPrologueImpl($model, 1);
}

## @fn void implementationEpilogue($model)
#  Given an operator instance model, produces the epilog code for the operator's
#  implementation code generator template
#  @param model the operator instance model
sub implementationEpilogue($)
{
   my ($model) = @_;
   SPL::CodeGenHelper::implementationEpilogueImpl($model, 1);
}

## @fn bool hasSideEffectInAssignments($port)
#  Given an output port object, return true if any output assignment expression has
#  side effects, otherwise false. This can be used to pull output tuple initialization
#  out of a loop.
#  @param port the output port object
#  @return 1 if any output assignment has side effects, 0 otherwise
#
#  Example: For
#    - tuple type <tt>tuple\<int8 a, int8 b\></tt>, and
#    - %SPL output assignment fragment <tt>output S : a = x, b = y;</tt>
#  this function will return 0.
#
#  For
#    - %SPL output assignment fragment <tt>output S : a = x++, b = statefulFunction(y);</tt>
#  this function will return 1.
sub hasSideEffectInAssignments($)
{
    my ($outputPort) = @_;
    my $numAttrs = $outputPort->getNumberOfAttributes();
    for (my $i = 0; $i < $numAttrs; ++$i) {
        my $exp = $outputPort->getAttributeAt($i)->getAssignmentValue();
        return 1 if $exp && $exp->hasSideEffects();
    }
    return 0;
}

## @fn bool hasStreamAttributeInAssignments($port)
#  Given an output port object, return true if any output assignment expression contains an input
#  attribute within the expression.  This can be used to pull output tuple initialization  out of a loop
#  @param port the output port object
#  @return 1 if any output assignment contains an input attribute, 0 otherwise
#
#  Example: For
#    - tuple type <tt>tuple\<int8 a, int8 b\></tt>, and
#    - %SPL output assignment fragment <tt>output S : a = x, b = y;</tt>
#  this function will return 1 if x or y are input stream attributes, 0 if they are both
#  state variables.
sub hasStreamAttributeInAssignments($)
{
    my ($outputPort) = @_;
    my $numAttrs = $outputPort->getNumberOfAttributes();
    for (my $i = 0; $i < $numAttrs; ++$i) {
        my $exp = $outputPort->getAttributeAt($i)->getAssignmentValue();
        return 1 if $exp && $exp->hasStreamAttributes();
    }
    return 0;
}

## @fn string getOutputTupleCppInitializer($port)
#  Given an output port object, return the C++ expression which can be used to
#  initialize a tuple for the port with the output assignments.
#  @param port the output port object
#  @return a string containing the expression to be used for initializing the
#  output tuple
#
#  Example: For
#    - tuple type <tt>tuple\<int8 a, int8 b\></tt>, and
#    - %SPL output assignment fragment <tt>output S : a = x, b = y;</tt>
#
#  this function will produce the following:
#    - <tt>$iport0.get_x(), $iport0.get_y()</tt>
#
#  This can be used as:
# @code
#    my $assignments = SPL::CodeGen::getOutputTupleCppInitializer($outputPort);
#    emitALine "OPort${index}Type otuple($assignments);";
# @endcode
#
# If an output assignment uses a Custom Output Function, the result will contain a call
# to a function of the same name.  If this is not desired, the operator should walk the
# assignments one at a time, examining each one to generate the desired code.
sub getOutputTupleCppInitializer($)
{
    my ($outputPort) = @_;
    my $numAttrs = $outputPort->getNumberOfAttributes();
    my $assignments = "";
    for (my $i = 0; $i < $numAttrs; ++$i) {
        my $exp = $outputPort->getAttributeAt($i)->getAssignmentValue();
        if ($exp) {
            $assignments .= SPL::CodeGenHelper::ensureValue(
                                $outputPort->getAttributeAt($i)->getSPLType(),
                                $exp->getCppExpression());
            $assignments .= ", " if $i < $numAttrs-1;
        }
    }
    return $assignments;
}

## @fn string getOutputTupleCppAssignments($tupleName, $port)
#  Given a tuple name and an output port object, return a C++ code segment which
#  can be used to assign an already initialized tuple for the port with the
#  output attribute assignments captured in the output port object.
#  @param tupleName the name of the variable holding an initialized tuple
#  @param port the output port object
#  @return a string containing the statements to carry out the output
#  assignments
#
#  Example: For
#    - tuple type <tt>tuple\<int8 a, int8 b\></tt>, and
#    - %SPL output assignment fragment <tt>output S : a = x, b = y;</tt>
#
#  <tt>getOutputTupleCppAssignments('fred', $port)</tt> will produce the
#  following:
#    - <tt>fred.set_a($iport0.get_x()); fred.set_b($iport0.get_y());</tt>
sub getOutputTupleCppAssignments($$)
{
    my ($tupleName, $outputPort) = @_;
    my $numAttrs = $outputPort->getNumberOfAttributes();
    my $assignments = "";
    for (my $i = 0; $i < $numAttrs; ++$i) {
      my $attr = $outputPort->getAttributeAt($i);
      if($attr->hasAssignment()) {
        my $exp = $attr->getAssignmentValue();
        $assignments .= "$tupleName.set_" . $attr->getName() . "(" . $exp->getCppExpression() . "); ";
      }
    }
    return $assignments;
}

## @fn string getOutputTupleCppAssignmentsWithSideEffects($tupleName, $port, $sideEffects)
#  Given a tuple name and an output port object, return a C++ code segment which
#  can be used to assign an already initialized tuple for the port with the
#  output attribute assignments captured in the output port object.
#  This is the same as getOutputTupleCppAssignments except for the additional
#  sideEffects parameter which is used to specify whether we want assignments
#  which contain side effects on the RHS (or not).
#  @param tupleName the name of the variable holding an initialized tuple
#  @param port the output port object
#  @param sideEffects If true then we only want assignments with side effects
#  otherwise we only want assignments which have no side effects
#  @return a string containing the statements to carry out the output
#  assignments
#
#  Example: For
#    - tuple type <tt>tuple\<int8 a, int8 b\></tt>, and
#    - %SPL output assignment fragment <tt>output S : a = x, b = y++;</tt>
#    - we only want assignment fragments which contain side effects
#
#  <tt>getOutputTupleCppAssignmentsWithSideEffects('fred', $port, 1)</tt> will produce the
#  following:
#    - <tt>fred.set_b($iport0.get_y());</tt>

sub getOutputTupleCppAssignmentsWithSideEffects($$$)
{
    my ($tupleName, $outputPort, $sideEffects) = @_;
    my $numAttrs = $outputPort->getNumberOfAttributes();
    my $assignments = "";
    for (my $i = 0; $i < $numAttrs; ++$i) {
      my $attr = $outputPort->getAttributeAt($i);
      if($attr->hasAssignment()) {
        my $exp = $attr->getAssignmentValue();
        if ($exp->hasStreamAttributes() || $exp->hasSideEffects()) {
          if ($sideEffects) {
             $assignments .= "$tupleName.set_" . $attr->getName() . "(" . $exp->getCppExpression() . "); ";
           }
        }
        elsif (!$sideEffects) {
             $assignments .= "$tupleName.set_" . $attr->getName() . "(" . $exp->getCppExpression() . "); ";
        }
      }
    }
    return $assignments;
}


## @fn list getParameterCppTypes($param)
#  Given a parameter object, return a list of C++ types corresponding to the
#  expressions contained within the parameter. The resulting list can be used as
#  the \a types parameter in an @link SPL::CodeGen::emitClass emitClass @endlink call.
#  @param param parameter object
#  @return list of types
sub getParameterCppTypes($)
{
    my ($param) = @_;
    return undef if not defined ($param);
    my @types;
    for (my $i = 0; $i < $param->getNumberOfValues(); $i++) {
        push @types, $param->getValueAt($i)->getCppType();
    }
    return @types;
}

## @fn string getParameterCppInitializer($param)
#  Given a parameter object, return a C++ expresion that can be used as the
#  constructor initializer for an object of type returned by an @link
#  SPL::CodeGen::emitClass emitClass @endlink call.
#  @param param parameter object
#  @return string to be used as a constructor initializer or \a undef if param
#  is not defined
#
#  Example: For
#    - %SPL parameter assignment fragment <tt>param key: a, sqrt(b);</tt>
#
#  this function will produce the following:
#    - <tt>$iport0.get_a(),SPL:Functions::Math::sqrt($iport0.get_b())</tt>
sub getParameterCppInitializer($)
{
    my ($param) = @_;
    return undef if not defined ($param);
    my $res = "";
    for (my $i = 0; $i < $param->getNumberOfValues(); $i++) {
        my $value = $param->getValueAt($i);
        if ($value) {
            $res .= $value->getCppExpression();
            $res .= ", " if $i < $param->getNumberOfValues()-1;
        }
    }
    return $res;
}

## @fn string adaptCppExpression($cppExpr, @tuples)
# Obtain the C++ expression adapted for the given tuple context, where the
# references to input tuples are replaced with the names given by the \a tuples
# parameter.
# @param cppExpr C++ expression to adapt to the tuple context
# @param tuples list of tuple variable names for the input ports
# @return a string with the adapted C++ expression
#
# Example: Consider a \c Filter operator with a \c filter parameter that defines
# a predicate over the input tuple attributes. Here is how one can use the
# filter expression from within the code generator template:
#
# @code
#  <%
#    my $filterParam = $model->getParameterByName("filter");
#    my $filterExpr = $filterParam->getValueAt(0)->getCppExpression();
#  %>
#
#  void MY_OPERATOR::process(Tuple & tuple, uint32_t port) {
#    if (<%=SPL::CodeGen:adaptCppExpression($filterExpr, "tuple")%>) {
#      ...
#    }
#  }
# @endcode
sub adaptCppExpression($@)
{
    my ($cppExpr, @tuples) = @_;
    my %quotes;
    my @chars = split(//, $cppExpr);
    my $inQuotes = 0;
    my $opening = 0;
    my $ending = 0;
    my $numSlashes = 0;
    for(my $i=0; $i<scalar(@chars); ++$i)
    {
        if($inQuotes) {
            if($chars[$i] eq "\\") {
                ++$numSlashes;
            } else {
                if(($chars[$i] eq "\"") &&
                   ($numSlashes % 2 == 0)) {
                    $ending = $i;
                    $quotes{$opening} = $ending;
                    $inQuotes = 0;
                }
                $numSlashes = 0;
            }
        } else {
            if($chars[$i] eq '"') {
                $opening = $i;
                $quotes{$opening} = undef;
                $inQuotes = 1;
            }
        }
    }

    my $newString;
    my $lastPos=0;
    while($cppExpr =~ m/(iport\$\d+)/g) {
        my $p = length($`);
        my $l = length($&);
        $inQuotes = 0;
        foreach my $b (keys %quotes) {
            my $e = $quotes{$b};
            if($p>$b && $p<$e)
            {
                $inQuotes = 1;
                last;
            }
        }
        unless($inQuotes) {
            $newString .= substr($cppExpr, $lastPos, $p-$lastPos);
            my $token = $&;
            my ($index) = ($& =~ m/(\d+)$/);
            if($index >= scalar(@tuples)) {
                SPL::CodeGen::errorln(SPL::Msg::INCORRECT_NUMBER_OF_TUPLE_VARIABLE_NAMES);
                return;
            }
            $newString .= "SPL::port_cast<IPort${index}Type>(" . $tuples[$index] . ")";
            $lastPos=$p+$l;
        }
    }
    $newString .= substr($cppExpr, $lastPos, length($cppExpr)-$lastPos);

    return $newString;
}

## @fn string prefixLiteralsAndStatesInCppExpressions($cppExpr, $prefix)
# Obtain the C++ expression with all references to lit$[0-9]*, state$
# and iport$[0-9]+History prefixed by the \a prefix parameter.
# @param cppExpr C++ expression to prefix literals and state references in
# @param prefix prefix to be prepended to the 'lit$x' and 'state$' term
# @return a string with the prefixed C++ expression
#
# Example: Consider operator with a helper class that needs to handle general C++ expressions.  The
# helper class will need to ensure that literals and state variable s(which are in the base operator class)
# can be compiled in C++.   If the helper class has a reference to the operator class in variable _oper,
# then we can prefix "_oper." to all literals, and the C++ expression will be valid.
# a predicate over the input tuple attributes. Here is how one can use the
# C++ expression from a Helper class:
#
# @code
#  <%
#    my $param = $model->getParameterByName("param");
#    my $expr = $param->getValueAt(0)->getCppExpression();
#  %>
#
#  class HelperClass {
#      MY_OPERATOR_SCOPE::MY_OPERATOR& _oper;
#    public:
#      HelperClass(MY_OPERATOR_SCOPE::MY_OPERATOR& oper) : _oper(oper) {}
#      void doSomething() {
#          int value = <%=SPL::CodeGen::prefixLiteralsAndStatesInCppExpressions($expr, "_oper.")%>;
#      }
#  }
# @endcode
sub prefixLiteralsAndStatesInCppExpressions($$)
{
    my ($cppExpr, $prefix) = @_;
    my %quotes;
    my @chars = split(//, $cppExpr);
    my $inQuotes = 0;
    my $opening = 0;
    my $ending = 0;
    my $numSlashes = 0;
    for(my $i=0; $i<scalar(@chars); ++$i)
    {
        if($inQuotes) {
            if($chars[$i] eq "\\") {
                ++$numSlashes;
            } else {
                if(($chars[$i] eq "\"") &&
                   ($numSlashes % 2 == 0)) {
                    $ending = $i;
                    $quotes{$opening} = $ending;
                    $inQuotes = 0;
                }
                $numSlashes = 0;
            }
        } else {
            if($chars[$i] eq '"') {
                $opening = $i;
                $quotes{$opening} = undef;
                $inQuotes = 1;
            }
        }
    }

    my $newString;
    my $lastPos=0;
    while($cppExpr =~ m/(state\$)|(lit\$\d+)|(iport\$\d+History)/g) {
        my $p = length($`);
        my $l = length($&);
        $inQuotes = 0;
        foreach my $b (keys %quotes) {
            my $e = $quotes{$b};
            if($p>$b && $p<$e)
            {
                $inQuotes = 1;
                last;
            }
        }
        unless($inQuotes) {
            $newString .= substr($cppExpr, $lastPos, $p-$lastPos);
            my $token = $&;
            $newString .= "$prefix$token";
            $lastPos=$p+$l;
        }
    }
    $newString .= substr($cppExpr, $lastPos, length($cppExpr)-$lastPos);

    return $newString;
}

## @fn string extractPerlValue($cppExpr, $splType)
# Obtain the Perl value from a C++ expression of a given SPL type.
# @pre the C++ expression must contain a C++ literal
# @pre the type must belong to one of the following categories: integral, float,
# decimal, string, boolean
# @param cppExpr C++ expression to convert to a Perl value
# @param splType %SPL type of the C++ expression
# @return a Perl value, or undef if unsuccessful
#
# Example: Consider a \c Filter operator with a \c filter parameter that defines
# a predicate over the input tuple attributes. Here is how one can use the
# filter expression from within the code generator template:
#
# @code
#  # Assume that the 'count' parameter is a Constant expression and expression
#  # rewrite is turned off for the parameter.
#  <%
#    my $count = $model->getParameterByName("count")->getValueAt(0);
#    my $c = SPL::CodeGen::extractPerlValue($count->getCppExpression(), $count->getSPLType());
#    if($c>4) { ... }
#  %>
# @endcode
sub extractPerlValue($$)
{
    my ($cppExpr, $splType) = @_;

    # if we start with a '(' then we are an expression, not a literal
    return undef if $cppExpr =~ m/^\(/;

    my $value = $cppExpr;
    $value =~ s/SPL::[^\(]*\(//;
    $value =~ s/\)$//;

    # Check for valid literals values, else return undef
    my $fp="([-+]?((\\d*\\.?\\d+([eE][-+]?\\d+)?f?)|(NAN)|(INFINITY)))";
    if(SPL::CodeGen::Type::isIntegral($splType)) {
        if ($value =~ m/^\s*([-+]?\d+)(LL)?U?\s*$/) {
            $value = $1;
        } else {
            return undef;
        }
    } elsif(SPL::CodeGen::Type::isFloat($splType)) {
        if ($value =~ m/^\s*$fp\s*$/) {
            $value = $1;
        } else {
            return undef;
        }
    } elsif(SPL::CodeGen::Type::isDecimal($splType)) {
        if ($value =~ m/^\s*"$fp"\s*$/) {
            $value = $1;
        } else {
            return undef;
        }
    } elsif(SPL::CodeGen::Type::isRString($splType) or
            SPL::CodeGen::Type::isUString($splType)) {
        # already caught many invalid cases above
        return undef if $value =~ m/^lit\$/;
        # Handle embedded NULs
        $value =~ s/^SPL::rstring\((.*)\)$/$1/;
        if ($value =~ m/^(.*), \d+$/) {
            # There are NULs in the string
            $value = $1;
            $value =~ s/\\0" "/\\0"."/g;
        }
        # Handle embedded @ and $
        $value =~ s/\@/\\\@/g;
        $value =~ s/\$/\\\$/g;
        $value = eval($value);
    } elsif(SPL::CodeGen::Type::isBoolean($splType)) {
        # already caught invalid cases above
        $value = ($value eq 'true') ? 1 : 0;
    } else {
        return undef;
    }
    return $value;
}

## @fn void emitSubmitOutputTuple($outputPort, $inputPort)
# Generate code to submit a tuple to a given output port. If the output
# attribute assignments indicate that the tuple is simply forwarded from the
# given input port, then the generated code will not create a separate tuple
# and will just forward the input one, as long as the port mutability settings
# allow it.
# @param outputPort The output port that we want to submit the tuple to
# @param inputPort The input port from which a tuple is potentially forwarded
#
# Example: <tt>emitSubmitOutputTuple($iport, $oport)</tt> will produce the
# following code <tt>submit($iport0, 0);</tt> if the output port index is 0, the
# output assignments indicate a simple forwarding, and the port mutability
# settings allow submitting the input tuple directly to the output
# port. Otherwise, it will generate code like:
# <tt>OPort0Type otuple($iport0.get_x()+5); submit(otuple, 0);</tt>.
sub emitSubmitOutputTuple($$)
{
    my ($outputPort, $inputPort) = @_;

    my $index = $outputPort->getIndex();
    if (isInputTupleForwarded ($outputPort, $inputPort, 0)) {
        # There is one potential problem:  the output tuple is mutating, and the input
        # one isn't.   Generate a copy ctr instead in this case
        my $cppName = $inputPort->getCppTupleName();
        if ($outputPort->getMutationAllowed() && !$inputPort->getMutationAllowed()) {
            # we have to make a copy, or this will fail to compile
            emitALine "OPort${index}Type otuple($cppName); submit (otuple, $index);";
        } else {
            # we are directly submitting the input tuple
            emitALine "submit ($cppName, $index);";
        }
    } else {
        # we have to do this the hard way
        my $assignments = SPL::CodeGen::getOutputTupleCppInitializer($outputPort);
        emitALine "OPort${index}Type otuple($assignments); submit (otuple, $index);";
    }
}

## @fn void emitSubmitCachedOutputTuple($cachedTupleName, $outputPort, $inputPort)
# Generate code to submit a cached tuple to a given output port. If the output
# attribute assignments indicate that the tuple is simply forwarded from the
# given input port, then the generated code will not populate the cached tuple
# and will just forward the input one, as long as the port mutability settings
# allow it.
# @param cachedTupleName Name for the output tuple
# @param outputPort The output port that we want to submit the tuple to
# @param inputPort The input port from which a tuple is potentially forwarded
#
# Example: <tt>emitSubmitCachedOutputTuple('fred', $iport, $oport)</tt> will
# produce the following code <tt>submit($iport0, 0);</tt> if the output port
# index is 0, the output assignments indicate a simple forwarding, and the port
# mutability settings allow submitting the input tuple directly to the output
# port. Otherwise, it will generate code like: <tt>fred.set_x($iport0.get_x()+5);
# submit(fred, 0);</tt>.
sub emitSubmitCachedOutputTuple($$$)
{
    my ($cachedTupleName, $outputPort, $inputPort) = @_;

    my $index = $outputPort->getIndex();
    if (isInputTupleForwarded ($outputPort, $inputPort, 0)) {
        # There is one potential problem:  the output tuple is mutating, and the input
        # one isn't.   Generate a copy ctr instead in this case
        my $cppName = $inputPort->getCppTupleName();
        if ($outputPort->getMutationAllowed() && !$inputPort->getMutationAllowed()) {
            # we have to make a copy, or this will fail to compile
            emitALine "$cachedTupleName = $cppName; submit ($cachedTupleName, $index);";
        } else {
            # we are directly submitting the input tuple
            emitALine "submit ($cppName, $index);";
        }
    } else {
        # we have to do this the hard way
        my $assignments = SPL::CodeGen::getOutputTupleCppAssignments($cachedTupleName, $outputPort);
        emitALine "$assignments submit ($cachedTupleName, $index);";
    }
}

## @fn string emitClass($model, $name, @types)
#  Given a name and a list of types, generates a C++ class that is hashable and
#  contains member fields with the specified types. The class will have public
#  members with the names 'field\<index\>_', one for each element in the $types
#  list. The class will also have empty, copy, and member initializer
#  constructors, assignment operator, as well as equality and non-equality
#  comparison operators.
#  @param model the operator instance model
#  @param name local name for the class (must uniqiely identify the class
#  within the operator)
#  @param types a list reference that holds the C++ types of the members
#  @return the name of the class, which is different than the parameter \a
#  name.
#  NOTE: emitClass() call should come before the headerPrologue() call.
#
#  Example usage from within a code generator template:
#  @code
#  // This is an <opname>_h.cgt file
#  <%
#    my $keyParam = $model->getParameterByName("key");
#    my @keyTypes = SPL::CodeGen::getParameterCppTypes($keyParam);
#    my $keyClass = SPL::CodeGen::emitClass($model, "MyKey", @keyTypes);
#    ...
#    SPL::CodeGen::headerPrologue($model);
#  %>
#  @endcode
sub emitClass($$@)
{
    my ($model, $name, @types) = @_;
    return if(scalar(@types)==0);

    my $context = $model->getContext();
    my $class = $context->getClass();
    $name = "${class}\$${name}";
    $name =~ s/::/\$/g;
    emitALine "#ifndef ${name}_H";
    emitALine "#define ${name}_H";
    emitALine "#include <SPL/Runtime/Type/SPLType.h>";
    emitALine "#include <SPL/Runtime/Serialization/ByteBuffer.h>";
    emitALine "#include <SPL/Runtime/Utility/TimeUtils.h>";
    SPL::CodeGenHelper::operatorIncludes($context);
    emitALine "";
    emitALine "class ${name} {";
    emitALine "public:";

    # empty and copy constructor
    emitALine "  ${name}() {}\n\n";
    emitALine "  ${name}(const ${name} & o) :";
    foreach(my $i=0; $i<scalar(@types); ++$i) {
       my $type = $types[$i];
       my $line = "field${i}_(o.field${i}_)";
       $line .= "," if($i!=scalar(@types)-1);
       emitALine "    ${line}";
    }
    emitALine "  {}";
    emitALine "";

    # constructor
    emitALine "  ${name}(";
    foreach(my $i=0; $i<scalar(@types); ++$i) {
        my $type = $types[$i];
        my $line = "const ${type} & field${i}";
        $line .= "," if($i!=scalar(@types)-1);
        emitALine "    ${line}";
    }
    emitALine "  ) :";
    foreach(my $i=0; $i<scalar(@types); ++$i) {
        my $line = "field${i}_(field${i})";
        $line .= "," if($i!=scalar(@types)-1);
        emitALine "    ${line}";
    }
    emitALine "  {}";
    emitALine "";

    # getters
    foreach(my $i=0; $i<scalar(@types); ++$i) {
       my $type = $types[$i];
       emitALine "  ${type} & get_field${i}() {";
       emitALine "    return field${i}_;";
       emitALine "  }";
       emitALine "";
       emitALine "  ${type} const & get_field${i}() const {";
       emitALine "    return field${i}_;";
       emitALine "  }";
       emitALine "";
    }
    # assignment
    emitALine "  const ${name} & operator=(${name} const & o) {";
    emitALine "    if(this==&o) return *this;";
    foreach(my $i=0; $i<scalar(@types); ++$i) {
        emitALine "    field${i}_ = o.field${i}_;";
    }
    emitALine "    return *this;";
    emitALine "  }";
    emitALine "";

    # equality
    emitALine "  bool operator==(${name} const & o) const {";
    foreach(my $i=0; $i<scalar(@types); ++$i) {
        emitALine "    if(field${i}_ != o.field${i}_) return false;";
    }
    emitALine "    return true;";
    emitALine "  }";
    emitALine "";

    # inequality
    emitALine "  bool operator!=(${name} const & o) const {";
    emitALine "    return !(*this==o);";
    emitALine "  }";
    emitALine "";

    # hash function
    emitALine "  size_t hashCode() const {";
    emitALine "    size_t r = 17;";
    emitALine "    size_t c;";
    foreach(my $i=0; $i<scalar(@types); ++$i) {
        my $type = $types[$i];
        emitALine "    c = std::tr1::hash<${type} >()(field${i}_);";
        emitALine "    r = 37 * r + c;";
    }
    emitALine "    return r;";
    emitALine "  }";
    emitALine "";

    # Checkpoint
    emitALine "  template <class S>";
    emitALine "  void serialize(SPL::ByteBuffer<S> & buf) const {";
    foreach(my $i=0; $i<scalar(@types); ++$i) {
        my $type = $types[$i];
        emitALine "    buf << field${i}_;";
    }
    emitALine "  }";
    emitALine "";

    # Reset
    emitALine "  template <class S>";
    emitALine "  void deserialize(SPL::ByteBuffer<S> & buf) {";
    foreach(my $i=0; $i<scalar(@types); ++$i) {
        my $type = $types[$i];
        emitALine "    buf >> field${i}_;";
    }
    emitALine "  }";
    emitALine "";

    # variables
    foreach(my $i=0; $i<scalar(@types); ++$i) {
        my $type = $types[$i];
        emitALine "  ${type} field${i}_;";
    }
    emitALine "};";

    # overload operator<<() for checkpointing
    emitALine "template <class S>";
    emitALine "inline SPL::ByteBuffer<S> & operator<<(SPL::ByteBuffer<S> & buf, const ${name} & v)";
    emitALine "{";
    emitALine "    v.serialize(buf); return buf;";
    emitALine "}";

    # overload operator>>() for restoring
    emitALine "template <class S>";
    emitALine "inline SPL::ByteBuffer<S> & operator>>(SPL::ByteBuffer<S> & buf, ${name} & v)";
    emitALine "{";
    emitALine "    v.deserialize(buf); return buf;";
    emitALine "}";

    emitALine "";

    # functional hash
    emitALine "namespace std {";
    emitALine "  namespace tr1 {";
    emitALine "    template <>";
    emitALine "    struct hash<${name} > {";
    emitALine "      size_t operator()(${name} const & val) const {";
    emitALine "        return val.hashCode();";
    emitALine "      }";
    emitALine "    };";
    emitALine "  }";
    emitALine "}";
    emitALine "";

    # FormattedOutputFunction and checkpoint streaming operators
    emitALine "namespace SPL {";
    emitALine "  std::ostream & operator<<(std::ostream & ostr, ${name} const & x) {";
    foreach(my $i=0; $i<scalar(@types); ++$i) {
        emitALine "    ostr << x.get_field${i}();";
    }
    emitALine "    return ostr;";
    emitALine "  }";
    emitALine "";
    emitALine "}";
    emitALine "";

    emitALine "#endif /* ${name}_H */";
    emitALine "";
    return $name;
}

## @fn string getWindowCppType($window, $tupleType, $partitionType, $dataType, $storageType)
#  Given a window object from the operator instance model, returns the C++ type
#  for the window.
#  @param window window object from the operator instance model
#  @param tupleType C++ type of the tuple to be stored in the window
#  @param partitionType optional C++ type of the partitions within the window; when subsequent
#  parameter ($dataType or $storageType) need to be specified, set $partitionType to '' for unpartitioned window
#  @param $dataType optional parameter for the C++ data type for the window, can be either'std::deque'
#  (default) or 'SPL::IncrDeque'
#  @param $storageType optional parameter for the C++ storage type for the window, can be either
#  'std::tr1::unordered_map' (default) or 'SPL::IncrUnorderedMap'
#
#  Example 1: For a partitioned sliding window with a count-based eviction
#  policy and a delta-based trigger policy,
#    - <tt>getWindowCppType($window, "MyTupleT", "MyPartitionT")</tt>
#
#  will produce the following:
#    - <tt>SPL::SlidingWindow<MyTupleT, MyPartitionT></tt>
#
#  Example 2: For an un-partitioned tumbling window with a count-based eviction
#  and a count-based trigger policy,
#    - <tt>getWindowCppType($window, "MyTupleT")</tt>
#
#  will produce the following:
#    - <tt>SPL::TumblingWindow<MyTupleT></tt>
#
#  Example 3: For an un-partitioned sliding window with a count-based eviction
#  and a count-based trigger policy and incremental checkpointing,
#    - <tt>getWindowCppType($window, "MyTupleT", '', 'SPL::IncrDeque', 'SPL::IncrUnorderedMap')</tt>
#
#  will produce the following:
#    - <tt>SPL::SlidingWindow<MyTupleT, int32_t, SPL::IncrDeque<MyTupleT>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<MyTupleT> > ></tt>
sub getWindowCppType($$;$$$$)
{
    my ($window, $tupleType, $partitionType, $dataType, $storageType) = @_;

    my $windowCppType =
      ($window->isTimeInterval()) ?
        'SPL::TimeIntervalWindow' :
      ($window->isTumbling()) ?
        'SPL::TumblingWindow' :
        'SPL::SlidingWindow';

    if(!defined($partitionType)) {
        return "$windowCppType<$tupleType >";
    }
    my $partitionCppType = ($partitionType eq '') ? 'int32_t' : $partitionType;
    if(!defined($dataType)) {
        return "$windowCppType<$tupleType, $partitionCppType >";
    }
    my $dataCppType = "std::deque<$tupleType >";
    if($dataType eq 'SPL::IncrDeque') {
        $dataCppType = "SPL::IncrDeque<$tupleType >";
    }
    my $storageCppType = "std::tr1::unordered_map<$partitionCppType, $dataCppType >";
    if(defined($storageType) and ($storageType eq 'SPL::IncrUnorderedMap')) {
        $storageCppType = "SPL::IncrUnorderedMap<$partitionCppType, $dataCppType >";
    }
    return "$windowCppType<$tupleType, $partitionCppType, $dataCppType, $storageCppType >";
}

## @fn string getWindowCppInitializer($window, $tupleType, $attrbGetterE, $attrbGetterT)
#  Given a window object from the operator instance model, returns the C++ type
#  initializer for the window.
#  @param window window object from the operator instance model
#  @param tupleType C++ type of the tuple to be stored in the window
#  @param attrbGetterE optional name of the member function within the
#  $tupleType, which is used to access the delta-based eviction policy
#  attribute. This parameter is not used for window policies other than
#  delta-based ones. If not specified, get_<delta-attribute-name> will be used
#  @param attrbGetterT optional name of the member function within the
#  $tupleType, which is used to access the delta-based trigger policy
#  attribute. This parameter is not used for window policies other than
#  delta-based ones. If not specified, get_<delta-attribute-name> will be used
#
#  @note This routine will NOT handle partition eviction policies
#
#  Example: For a sliding window with a count-based eviction policy of size 10
#  and a delta-based trigger policy on an \c int32 attribute with a delta of 15, the call
#    - <tt>getWindowCppInitializer($window, "MyTupleT", "", "get_age")</tt>
#
# will produce the following:
#    - <tt>*this, 0, SPL::CountWindowPolicy(10), SPL::DeltaWindowPolicy<MyTupleT,uint32,&MyTupleT::get_age>(15)</tt>
sub getWindowCppInitializer($$;$$)
{
    my ($window, $tupleType, $attrbGetterE, $attrbGetterT) = @_;
    my $port = $window->getPortIndex();
    my $epolInit = getWindowPolicyCppObject($window, 1, $tupleType, $attrbGetterE);
    # warn the user that we are not using the partition eviction policy
    SPL::CodeGenHelper::warnPartitionEvictionPolicyIgnored ($window)
        if ($window->hasPartitionEvictionPolicy());
    if($window->isTumbling() || $window->isTimeInterval()) {
        return "*this, ${port}, ${epolInit}";
    } else { # sliding
        my $tpolInit = getWindowPolicyCppObject($window, 0, $tupleType, $attrbGetterT);
        return "*this, ${port}, ${epolInit}, ${tpolInit}";
    }
}

## @fn string getPartitionedWindowCppInitializer($window, $tupleType, $selectionType, $attrbGetterE, $attrbGetterT)
#  Given a window object for a partitioned window from the operator instance model,
#  returns the C++ type initializer for the window.
#  @param window window object from the operator instance model
#  @param tupleType C++ type of the tuple to be stored in the window
#  @param selectionType Algorithm to use for partion selection.  Supported algorithms are
#  "LRU" and "OperatorDefined".
#  @param attrbGetterE optional name of the member function within the
#  $tupleType, which is used to access the delta-based eviction policy
#  attribute. This parameter is not used for window policies other than
#  delta-based ones. If not specified, get_<delta-attribute-name> will be used
#  @param attrbGetterT optional name of the member function within the
#  $tupleType, which is used to access the delta-based trigger policy
#  attribute. This parameter is not used for window policies other than
#  delta-based ones. If not specified, get_<delta-attribute-name> will be used
#
#  Example: For a sliding window with a count-based eviction policy of size 10
#  and a delta-based trigger policy on an \c int32 attribute with a delta of 15, and
#  partitioned(tupleCount(100)), the call
#    - <tt>getPartitionedWindowCppInitializer($window, "MyTupleT", "LRU", "", "get_age")</tt>
#
# will produce the following:
#    - <tt>*this, 0, SPL::CountWindowPolicy(10), SPL::DeltaWindowPolicy<MyTupleT,uint32,&MyTupleT::get_age>(15), ::SPL::TupleCountPartitionEvictionPolicy(100, ::SPL::PartitionEvictionPolicy::LRU)</tt>
sub getPartitionedWindowCppInitializer($$$;$$)
{
    my ($window, $tupleType, $selectionType, $attrbGetterE, $attrbGetterT) = @_;
    my $port = $window->getPortIndex();
    my $epolInit = getWindowPolicyCppObject($window, 1, $tupleType, $attrbGetterE);
    my $partitionEvictionPolicy =
        getWindowPartitionEvictionPolicyInitializer($window, $selectionType);
    SPL::CodeGen::errorln (SPL::Msg::INVALID_PARTITION_EVICTION_SELECTOR($selectionType))
        unless $selectionType eq 'LRU' || $selectionType eq 'OperatorDefined';
    if($window->isTumbling() || $window->isTimeInterval()) {
        # no partition eviction policy for event time window
        return "*this, ${port}, ${epolInit}${partitionEvictionPolicy}";
    } else { # sliding
        my $tpolInit = getWindowPolicyCppObject($window, 0, $tupleType, $attrbGetterT);
        return "*this, ${port}, ${epolInit}, ${tpolInit}${partitionEvictionPolicy}";
    }
}

## @fn string getWindowPolicyCppObject($window, $eviction, $tupleType, $attrbGetter)
#  Given a window object from the operator instance model and a window policy,
#  returns the code that creates a C++ object for the policy
#  @param window window object from the operator instance model
#  @param eviction 1 if eviction policy, 0 if trigger policy
#  @param tupleType C++ type of the tuple to be stored in the window
#  @param attrbGetter optional name of the member function within the
#  $tupleType, which is used to access the delta-based window policy
#  attribute. This parameter is not used for window policies other than
#  delta-based ones. If not specified, get_<delta-attribute-name> will be used
#
#  Example 1: For a sliding window with a count-based eviction policy of size 10, the call
#    - <tt>getWindowPolicyCppObject($window, 1, "MyTupleT")</tt>
#
#  will produce the following:
#    - <tt>SPL::CountWindowPolicy(10)</tt>
#
#  Example 2: For a sliding window with a delta-based trigger policy on an \c int32
#  attribute with a delta of 15, the call
#    - <tt>getWindowPolicyCppObject($window, 0, "MyTupleT", "get_age")</tt>
#
#  will produce the following:
#    - <tt>SPL::DeltaWindowPolicy<MyTupleT,uint32,&MyTupleT::get_age>(15)</tt>
#
#  Example 3: For a sliding window with a delta-based trigger policy on an \c int32
#  attribute with a delta of 15, where the window stores pointers, the call
#    - <tt>getWindowPolicyCppObject($window, 0, "MyTupleT*", "get_age")</tt>
#
#  will produce the following:
#    - <tt>SPL::DeltaWindowPolicy<MyTupleT*,uint32,&MyTupleT::get_age>(15)</tt>
sub getWindowPolicyCppObject($$;$)
{
    my ($window, $eviction, $tupleType, $attrbGetter) = @_;
    my ($policyType, $deltaType, $deltaAttrbGetter, $size);
    if ($window->isTimeInterval()) {

        my ($intervalDuration, $creationPeriod, $discardAge, $intervalOffset, $resolution);

        $intervalDuration = $window->getIntervalDuration()->getCppExpression();
        if ($window->hasCreationPeriod()) {
            $creationPeriod = $window->getCreationPeriod()->getCppExpression();
        }
        else {
            $creationPeriod = $intervalDuration;
        }
        if ($window->hasDiscardAge()) {
            $discardAge = $window->getDiscardAge()->getCppExpression();
        }
        else {
            $discardAge = "0.0";
        }
        if ($window->hasIntervalOffset()) {
            $intervalOffset = $window->getIntervalOffset()->getCppExpression();
        }
        else {
            $intervalOffset = "0.0";
        }

        my $policy = "::SPL::TimeIntervalWindowOptions().setIntervalDuration(timestamp($intervalDuration)).setCreationPeriod(timestamp($creationPeriod)).setDiscardAge(timestamp($discardAge)).setIntervalOffset(timestamp($intervalOffset))";

        return $policy;
    }
    if ($eviction) {
        $policyType = $window->getEvictionPolicyType();
        $size = $window->getEvictionPolicySize()->getCppExpression()
            if ($policyType!=$SPL::Operator::Instance::Window::PUNCT);
        if ($policyType==$SPL::Operator::Instance::Window::DELTA) {
            $deltaType = $window->getEvictionPolicyAttribute()->getCppType();
            $deltaAttrbGetter = $window->getEvictionPolicyAttribute()->getCppExpression()
                if(not $attrbGetter);
        }
    } else {
        $policyType = $window->getTriggerPolicyType();
        $size = $window->getTriggerPolicySize()->getCppExpression()
            if ($policyType!=$SPL::Operator::Instance::Window::PUNCT);
        if ($policyType==$SPL::Operator::Instance::Window::DELTA) {
            $deltaType = $window->getTriggerPolicyAttribute()->getCppType();
            $deltaAttrbGetter = $window->getTriggerPolicyAttribute()->getCppExpression()
                if(not $attrbGetter);
        }
    }
    if((not $attrbGetter) and $deltaAttrbGetter) {
        $attrbGetter = $deltaAttrbGetter;
        $attrbGetter =~ s/^[^\.]*\.//; # iport0.get_age() -> get_age()
        $attrbGetter =~ s/\s*\(\s*\)\s*$//; # get_age() -> get_age
    }

    my $ret = "";
    if ($policyType==$SPL::Operator::Instance::Window::COUNT) {
        $ret = "::SPL::CountWindowPolicy(${size})";
    } elsif ($policyType==$SPL::Operator::Instance::Window::DELTA) {
        my $baseTupleType = $tupleType;
        $baseTupleType =~ s/\*\s*$//; # T * -> T
        $baseTupleType =~ s/^const //; # const T -> T
        $ret = "::SPL::DeltaWindowPolicy<${tupleType},${deltaType},&" .
                                          "${baseTupleType}::${attrbGetter}>(${size})";
    } elsif ($policyType==$SPL::Operator::Instance::Window::TIME) {
        $ret = "::SPL::TimeWindowPolicy(${size})";
    } elsif ($policyType==$SPL::Operator::Instance::Window::PUNCT) {
        $ret = "::SPL::PunctWindowPolicy()";
    }
    return $ret;
}

## @fn string getWindowPartitionEvictionPolicyInitializer($window, $selectionType)
#  Given a window object from the operator instance model, returns the C++ initializer
#  that can be appended to the window initializer to set the partition evicition policy.
#  If there is no partition eviction policy, return an empty string
#  @param window window object from the operator instance model
#  @param selectionType Algorithm to use for partion selection.  Supported algorithms are
#  "LRU" and "OperatorDefined".
#
#  Example: For a partition eviction policy of partitionAge(value), return
#    - <tt>, ::SPL::PartitionEvictionPolicy::PartitionAge(value)</tt>
sub getWindowPartitionEvictionPolicyInitializer($$)
{
    my ($window, $selectionType) = @_;
    return "" if (!$window->hasPartitionEvictionPolicy());
    my ($kind, $value);
    my $pKind = $window->getPartitionEvictionPolicyKind();
    my $selector;
    if ($pKind eq "partitionAge") {
        $kind = "PartitionAge";
        $value = $window->getPartitionEvictionPolicyAge();
    } elsif ($pKind eq "partitionCount") {
        $kind = "PartitionCount";
        $value = $window->getPartitionEvictionPolicyCount();
        $selector = $selectionType;
    } else {
        $kind = "TupleCount";
        $value = $window->getPartitionEvictionPolicyCount();
        $selector = $selectionType;
    }
    $selector = $selector ? ", ::SPL::PartitionEvictionPolicy::$selector" : "";
    $value = $value->getCppExpression();
    return ", ::SPL::${kind}PartitionEvictionPolicy($value$selector)";
}

## @fn string getWindowEventCppType($window, $tupleType, $partitionType, $dataType, $storageType)
#  Given a window object from the operator instance model, returns the C++ type
#  for the WindowEvent class that is used to handle events from the window.
#  @param window window object from the operator instance model
#  @param tupleType C++ type of the tuple to be stored in the window
#  @param partitionType optional C++ type of the partitions within the window; when subsequent
#  parameter ($dataType or $storageType) need to be specified, set $partitionType to '' for unpartitioned window
#  @param $dataType optional parameter for the C++ data type for the window, can be either'std::deque'
#  (default) or 'SPL::IncrDeque'
#  @param $storageType optional parameter for the C++ storage type for the window, can be either
#  'std::tr1::unordered_map' (default) or 'SPL::IncrUnorderedMap'
#
#  Example 1: For a partitioned sliding window with a count-based eviction
#  policy and a delta-based trigger policy,
#    - <tt>getWindowEventCppType($window, "MyTupleT", "MyPartitionT")</tt>
#
#  will produce the following:
#    - <tt>SPL::WindowEvent<MyTupleT, MyPartitionT></tt>
#
#  Example 2: For an un-partitioned sliding window with a count-based eviction
#  and a count-based trigger policy,
#    - <tt>getWindowEventCppType($window, "MyTupleT")</tt>
#
#  will produce the following:
#    - <tt>SPL::WindowEvent<MyTupleT></tt>
#
#  Example 3: For an un-partitioned sliding window with a count-based eviction
#  and a count-based trigger policy and incremental checkpointing,
#    - <tt>getWindowEventCppType($window, "MyTupleT", '', 'SPL::IncrDeque', 'SPL::IncrUnorderedMap')</tt>
#
#  will produce the following:
#    - <tt>SPL::WindowEvent<MyTupleT, int32_t, SPL::IncrDeque<MyTupleT>, SPL::IncrUnorderedMap<int32_t, SPL::IncrDeque<MyTupleT> > ></tt>
sub getWindowEventCppType($$;$$$$)
{
    my ($window, $tupleType, $partitionType, $dataType, $storageType) = @_;
    if(!defined($partitionType)) {
        return "SPL::WindowEvent<$tupleType >";
    }
    my $partitionCppType = ($partitionType eq '') ? 'int32_t' : $partitionType;
    if(!defined($dataType)) {
        return "SPL::WindowEvent<$tupleType, $partitionCppType >";
    }
    my $dataCppType = "std::deque<$tupleType >";
    if($dataType eq 'SPL::IncrDeque') {
        $dataCppType = "SPL::IncrDeque<$tupleType >";
    }
    my $storageCppType = "std::tr1::unordered_map<$partitionCppType, $dataCppType >";
    if(defined($storageType) and ($storageType eq 'SPL::IncrUnorderedMap')) {
        $storageCppType = "SPL::IncrUnorderedMap<$partitionCppType, $dataCppType >";
    }
    return "SPL::WindowEvent<$tupleType, $partitionCppType, $dataCppType, $storageCppType >";
}

## @fn list createWindowConfigurations(@confs)
#  Given a list of window configurations, verify that the configurations are
#  valid and return a reference to a list of configurations that can be used as
#  input to @link SPL::CodeGen::validateWindowConfiguration
#  validateWindowConfiguration@endlink and @link
#  SPL::CodeGen::checkWindowConfiguration checkWindowConfiguration@endlink
#  functions. The input list contains one hash reference for each
#  configuration. A hash that represents a window configuration contains one or
#  more of the following mappings:
#  - type => \<window-type\>, where the window type is one of
#    $SPL::Operator::Instance::Window::TUMBLING and SLIDING.
#  - eviction => \<eviction-policy\>, where the eviction policy is one of
#   $SPL::Operator::Instance::Window::COUNT, DELTA, TIME, and PUNCT.
#  - trigger => \<trigger-policy\>, where the trigger policy is one of
#   $SPL::Operator::Instance::Window::COUNT, DELTA, and TIME.
#  - partitioned => \<partition-status\>, where the partition status is 0 or 1.
#  These mappings are treated as conjunctions.
#  @param confs list of valid window configurations
#  @param confs reference to the list of valid window configurations
#  @return a reference to the list of configurations
#
#  Here is an example use from the Sort operator, which creates two
#  configurations, one that accepts any tumbling window, another one that
#  accepts sliding windows with count based trigger and eviction policies:
#  \code
#  my $confs = SPL::CodeGen::createWindowConfigurations(
#         { type     => $SPL::Operator::Instance::Window::TUMBLING },
#         { type     => $SPL::Operator::Instance::Window::SLIDING,
#           eviction => $SPL::Operator::Instance::Window::COUNT,
#           trigger  => $SPL::Operator::Instance::Window::COUNT});
#  \endcode
sub createWindowConfigurations
{
    my (@confs) = @_;
    foreach my $conf (@confs) {
        my $type = $conf->{type};
        my $partitioned = $conf->{partitioned};
        my $eviction = $conf->{eviction};
        my $trigger = $conf->{trigger};
        if(defined($type)) {
            SPL::CodeGen::exitln(SPL::Msg::UNKNOWN_WINDOW_PROPERTY_VALUE($type, "type"))
                if($type ne "$SPL::Operator::Instance::Window::TUMBLING" &&
                   $type ne "$SPL::Operator::Instance::Window::SLIDING" &&
                   $type ne "$SPL::Operator::Instance::Window::TIME_INTERVAL");
        }
        if(defined($partitioned)) {
            SPL::CodeGen::exitln(SPL::Msg::UNKNOWN_WINDOW_PROPERTY_VALUE($partitioned, "partitioned"))
                if($partitioned ne "0" && $partitioned ne "1");
        }
        if(defined($eviction)) {
            SPL::CodeGen::exitln(SPL::Msg::UNKNOWN_WINDOW_PROPERTY_VALUE($eviction, "eviction"))
                if($eviction ne "$SPL::Operator::Instance::Window::COUNT" &&
                   $eviction ne "$SPL::Operator::Instance::Window::DELTA" &&
                   $eviction ne "$SPL::Operator::Instance::Window::TIME" &&
                   $eviction ne "$SPL::Operator::Instance::Window::PUNCT");
        }
        if(defined($trigger)) {
            SPL::CodeGen::exitln(SPL::Msg::UNKNOWN_WINDOW_PROPERTY_VALUE($trigger, "trigger"))
                if($trigger ne "$SPL::Operator::Instance::Window::COUNT" &&
                   $trigger ne "$SPL::Operator::Instance::Window::DELTA" &&
                   $trigger ne "$SPL::Operator::Instance::Window::TIME" &&
                   $trigger ne "$SPL::Operator::Instance::Window::PUNCT");
        }
        my %validKeys = ("type"=>1, "partitioned"=>1, "eviction"=>1, "trigger"=>1);
        foreach my $key (keys %{$conf}) {
            SPL::CodeGen::exitln(SPL::Msg::UNKNOWN_WINDOW_PROPERTY_KEY($key))
                if(not $validKeys{$key});
        }
    }
    return \@confs;
}

## @fn void validateWindowConfiguration($iport, $confs)
#  Given an input port and a list of window configurations (created via @link
#  SPL::CodeGen::createWindowConfigurations createWindowConfigurations@endlink),
#  verifies that the input port has a valid window configuration. Prints an
#  error message and exits if the window configuration of the input port is not
#  among the provided configurations.
#  @param iport the input port object from the operator instance model
#  @param confs a list of valid window configurations, see
#  createWindowConfigurations function for details.
#
#  Example use from the \c Sort operator:
#  \code
#  my $confs = SPL::CodeGen::createWindowConfigurations(
#         { type     => $SPL::Operator::Instance::Window::TUMBLING },
#         { type     => $SPL::Operator::Instance::Window::SLIDING,
#           eviction => $SPL::Operator::Instance::Window::COUNT,
#           trigger  => $SPL::Operator::Instance::Window::COUNT});
#  my $iport = $model->getInputPortAt(0);
#  SPL::CodeGen::validateWindowConfiguration($iport, $confs);
#  \endcode
sub validateWindowConfiguration($$)
{
    my ($iport, $confs) = @_;
    return if(1==checkWindowConfiguration($iport,$confs));
    if($iport->hasWindow()) {
       SPL::CodeGen::exitln(SPL::Msg::STDTK_UNSUPPORTED_WINDOW_CONFIGURATION($iport->getIndex()),
                          $iport->getWindow()->getSourceLocation());
   } else {
       SPL::CodeGen::exitln(SPL::Msg::STDTK_UNSUPPORTED_WINDOW_CONFIGURATION($iport->getIndex()),
                          $iport->getSourceLocation());
   }
}

## @fn bool checkWindowConfiguration($iport, $confs)
#  Given an input port and a list of window configurations (created via @link
#  SPL::CodeGen::createWindowConfigurations createWindowConfigurations@endlink),
#  checks if the input port has a valid window configuration. Returns 0 if the
#  window configuration of the input port is not among the provided
#  configurations, and 1 otherwise.
#  @param iport the input port object from the operator instance model
#  @param confs a list of valid window configurations, see
#  createWindowConfigurations function for details.
#  @return 1 if the configuration is valid, 0 otherwise
#
#  Example use from the \c Sort operator:
#  \code
#  my $confs = SPL::CodeGen::createWindowConfigurations(
#         { type     => $SPL::Operator::Instance::Window::TUMBLING },
#         { type     => $SPL::Operator::Instance::Window::SLIDING,
#           eviction => $SPL::Operator::Instance::Window::COUNT,
#           trigger  => $SPL::Operator::Instance::Window::COUNT});
#  my $iport = $model->getInputPortAt(0);
#  my $res = SPL::CodeGen::checkWindowConfiguration($iport, $confs);
#  \endcode
sub checkWindowConfiguration($$)
{
    my ($iport, $confs) = @_;
    if(not $iport->hasWindow()) {
        if(scalar(@{$confs})) {
            return 0;
        } else {
            return 1;
        }
    }

    my $match = 0;
    my $window = $iport->getWindow();
    foreach my $conf (@{$confs}) {
        my $type = $conf->{type};
        my $partitioned = $conf->{partitioned};
        my $eviction = $conf->{eviction};
        my $trigger = $conf->{trigger};
        if(defined($type)) {
            next if($type!=$window->getWindowType());
        }
        if(defined($partitioned)) {
            next if($partitioned!=$window->isPartitioned());
        }
        if(defined($eviction)) {
            next if($eviction!=$window->getEvictionPolicyType());
        }
        if(defined($trigger)) {
            next if($trigger!=$window->getTriggerPolicyType());
        }
        $match = 1;
        last;
    }
    return $match;
}

## @fn bool isInputTupleForwarded($outputPort, $inputPort, $checkMutation)
# Check if the output tuple to be created is an exact copy of the input tuple
# @param outputPort the output port that we want to submit a tuple to
# @param inputPort the input port from which the tuples are potentially forwarded
# @param checkMutation if 1, check that the port mutation settings allow
# forwarding the input tuple directly, without creating a copy
# @return 1 if the input tuple is used directly to generate the output tuple
sub isInputTupleForwarded($$$)
{
    my ($outputPort, $inputPort, $checkMutation) = @_;
    # easy case: different types
    return 0 if $inputPort->getCppTupleType() ne $outputPort->getCppTupleType();

    # another one to check.   Are the mutation settings okay?   In other words,
    # we can't pass a non-mutating input to a mutating output
    return 0 if $checkMutation &&
                $outputPort->getMutationAllowed() && !$inputPort->getMutationAllowed();

    # check all the assignment expressions to see if they match
    my $numAttrs = $outputPort->getNumberOfAttributes();
    my $iportName = $inputPort->getCppTupleName();
    for (my $i = 0; $i < $numAttrs; ++$i) {
        my $attr = $outputPort->getAttributeAt($i);
        my $name = $attr->getName();
        my $exp = $attr->getAssignmentValue();
        # is there a matching output expression?
        return 0 if !$exp;
        my $cppExpn = $exp->getCppExpression();

        # look for iport$N.get_ATTRIBUTE()
        return 0 if $cppExpn ne ($iportName . ".get_" . $name . "()");
    }

    # all matched
    return 1;
}

## @fn void checkMinimalSchema($port, @schema)
# Given an input or output port, and a list of schema restrictions, ensure that the tuple that
# the port is consuming/producing contains all of the desired schema name/types.
# If there are attributes/types missing from the tuple, generate an error.
# For each restriction, there must be at least one attribute that satisfies it.
# @param port input or output port object whose schema is to be checked
# @param schema list of name/type pairs that must be present in the input/output tuple
#
# \a schema is a list of hash references that represent schema restrictions using one or
# more of the following mappings:
# - <tt> name => "attributeName" </tt>
# - <tt> type => "splType" </tt> or <tt> type => [ "splType", "splType" ... ] </tt>
#
# NOTE: The only blanks allowed in the %SPL type string are those that separate a
# tuple attribute type from the name (\c "int32 attr"). All types are 'unwound' to the
# expanded %SPL type. User defined type names are not allowed.
#
# Here is an example use, which is looking for an attribute \a foo with type \c rstring or \c int32,
# any attribute with type \c tuple<int32 x, rstring y>, and an attribute \a bar with any type.
#  @code
#  SPL::CodeGen::checkMinimalSchema ($model->getInputPortAt(0),
#         { name => "foo", type => ["rstring", "int32] },
#         { type => "tuple<int32 x,rstring y>" },
#         { name => "bar" });
#  @endcode
sub checkMinimalSchema($@)
{
    my ($port, @schema) = @_;
    foreach my $s (@schema) {
        my $name = $s->{name};
        my $type = $s->{type};
        if (defined($name)) {
            my $a = $port->getAttributeByName($name);
            if (!defined ($a)) {
                SPL::CodeGen::errorln (SPL::Msg::NAME_NOT_FOUND_IN_SCHEMA($port->getIndex(), $name),
                    $port->getSourceLocation());
            } elsif (defined ($type)) {
                # does $a have the right type?
                my $possibleTypes;
                if (!SPL::CodeGenHelper::checkTypeMatches ($a->getSPLType(), $type, \$possibleTypes)) {
                    SPL::CodeGen::errorln (SPL::Msg::NAME_WRONG_TYPE_IN_SCHEMA($port->getIndex(),
                            $name, $a->getSPLType(), $possibleTypes), $port->getSourceLocation());
                }
            }
        } elsif (defined($type)) {
            # look for any attribute with given type
            my $seen = 0;
            for (my $i = 0; $i < $port->getNumberOfAttributes(); $i++) {
                my $possibleTypes;
                if (SPL::CodeGenHelper::checkTypeMatches ($port->getAttributeAt($i)->getSPLType(), $type, \$possibleTypes)) {
                    $seen = 1;
                    last;
                }
            }
            if (!$seen) {
                SPL::CodeGen::errorln (SPL::Msg::TYPE_NOT_FOUND_IN_SCHEMA($port->getIndex(),
                        join ('/', $type)), $port->getSourceLocation());
            }
        }
    }
    CORE::exit $SPL::CodeGen::USER_ERROR if ($SPL::CodeGen::sawError);
}

## @fn void checkMaximalSchema($port, @schema)
# Given an input or output port, and a list of schema restrictions, ensure that the tuple that
# the port is consuming/producing only contains attributes with the desired schema name/types.
# If there are attributes or types in the schema that are not supported, generate an error.
# For each attribute, there must be at least one restriction that permits it.
# @param port  input or output port object whose schema is to be checked
# @param schema list of name/type pairs that are valid in the input/output tuple
#
# \a schema is a list of hash references that represent schema restrictions using one or
# more of the following mappings:
# - <tt> name => "attributeName" </tt>
# - <tt> type => "splType" </tt> or <tt> type => [ "splType", "splType" ... ] </tt>
#
# NOTE: The only blanks allowed in the %SPL type string are those that separate a
# tuple attribute type from the name (\c "int32 attr"). All types are 'unwound' to the
# expanded %SPL type. User defined type names are not allowed.
#
# Here is an example use: If attribute \a foo is present, it must have type \c
# rstring or \c ustring, any attribute with type \c tuple<int32 x, rstring y> is
# acceptable, and an attribute with name \c bar' is allowed.
#  @code
#  SPL::CodeGen::checkMaximalSchema ($model->getInputPortAt(0),
#         { name     => "foo", type => [ "rstring", "ustring" ] },
#         { type => "tuple<int32 x,rstring y>" },
#         { name => "bar" });
#  @endcode
sub checkMaximalSchema($@)
{
    my ($port, @schema) = @_;
    # walk the attributes;
    for (my $i = 0; $i < $port->getNumberOfAttributes(); $i++) {
        my $a = $port->getAttributeAt($i);
        my $aName = $a->getName();
        my $aType = $a->getSPLType();

        # is there a match for this name?
        my $seen = 0;
        my $possibleTypes;
        foreach my $s (@schema) {
            my $name = $s->{name};
            my $type = $s->{type};
            if (defined ($name) && $aName eq $name) {
                # This is a name match. Does the type (if present) match too?
                if (!defined ($type) || SPL::CodeGenHelper::checkTypeMatches($aType, $type, \$possibleTypes)) {
                    $seen = 1;
                    last;
                }
            } elsif (defined($type)) {
                my $possibleTypes;
                if (SPL::CodeGenHelper::checkTypeMatches($aType, $type, \$possibleTypes)) {
                    # type match
                    $seen = 1;
                    last;
                }
            }
        }
        # saw an attribute that didn't match; complain
        if (!$seen) {
            if (defined ($possibleTypes)) {
                SPL::CodeGen::errorln (SPL::Msg::ATTRIBUTE_NOT_ALLOWED_IN_SCHEMA_VALID_TYPES($port->getIndex(), $aName, $aType, $possibleTypes),
                    $port->getSourceLocation());
            } else {
                SPL::CodeGen::errorln (SPL::Msg::ATTRIBUTE_NOT_ALLOWED_IN_SCHEMA($port->getIndex(), $aName, $aType),
                    $port->getSourceLocation());
            }
        }
    }
    CORE::exit $SPL::CodeGen::USER_ERROR if ($SPL::CodeGen::sawError);
}

## @fn void checkAnySchema($port, @schema)
# Given an input or output port, and a list of schema restrictions, ensure that the tuple that
# the port is consuming/producing contains attributes of the right names/types.
# If an attribute in the tuple is in the list of restrictions, it must have the right type.
# Any attributes NOT in the schema restrictions are accepted without errors.
# For each attribute, there must be at least one restriction with a matching name that is satisfied.
# @param port   input or output port from the operator model to be checked
# @param schema list of name/type pairs that must be present in the input/output tuple
#
# \a schema is a list of hash references that represent schema restrictions using one or
# more of the following mappings:
# - <tt> name => "attributeName" </tt>
# - <tt> type => "splType" </tt> or <tt> type => [ "splType", "splType" ... ] </tt>
#
# NOTE: The only blanks allowed in the %SPL type string are those that separate a
# tuple attribute type from the name (\c "int32 attr"). All types are 'unwound' to the
# expanded %SPL type. User defined type names are not allowed.
#
# Here is an example use: If an attribute \a foo is present, it must have type
# \c rstring or \c int32,
# @code
#  SPL::CodeGen::checkAnySchema ($model->getInputPortAt(0),
#         { name => "foo", type => [ "rstring", "int32" ]});
# @endcode
sub checkAnySchema($@)
{
    my ($port, @schema) = @_;
    foreach my $s (@schema) {
        my $name = $s->{name};
        my $type = $s->{type};

        # ignore records that don't have both name and type
        next if !defined($type) || !defined($name);

        my $a = $port->getAttributeByName($name);

        # if we don't have a name in the tuple schema that matches, we are fine
        next if !defined ($a);

        # does $a have the right type?
        my $possibleTypes;
        if (!SPL::CodeGenHelper::checkTypeMatches ($a->getSPLType(), $type, \$possibleTypes)) {
            SPL::CodeGen::errorln (SPL::Msg::NAME_WRONG_TYPE_IN_SCHEMA($port->getIndex(), $name,
                        $a->getSPLType(), $possibleTypes),
                $port->getSourceLocation());
        }
    }
    CORE::exit $SPL::CodeGen::USER_ERROR if ($SPL::CodeGen::sawError);
}

## @fn void print($msg, @params)
#  This function allows printing to the console during code generation.  During
#  code generation STDOUT is redirected. Attempts to print to it will result in
#  corruption of the code generator.  Note that if no optional params are passed
#  the Perl \c print function will be called.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message
#  @param params an optional list of parameters to satisfy the format chars in msg
sub print
{
    my ($msg, @params) = @_;
    if (scalar(@params) > 0) {
        printf $OLDSTDOUT $msg, @params;
    } else {
        print $OLDSTDOUT $msg;
    }
}

## @fn void println($msg, @params)
#  This function is similar to the print() method with the addition of a new
#  line added at the end.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message
#  @param params an optional list of parameters to satisfy the format chars in msg
sub println
{
    SPL::CodeGen::print(@_);
    print $OLDSTDOUT "\n";
}

## @fn void printVerbose($msg, @params)
#  This function is similar to the print() method except that it only prints if the \c -v or
#  \c --verbose-mode  compiler option has been specified.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message
#  @param params an optional list of parameters to satisfy the format chars in msg
sub printVerbose
{
    if ($verboseMode) {
        SPL::CodeGen::print(@_);
    }
}

## @fn void printlnVerbose($msg, @params)
#  This function is similar to the println() method except that it only prints if the \c -v or
#  \c --verbose-mode compiler option has been specified.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message
#  @param params an optional list of parameters to satisfy the format chars in msg
sub printlnVerbose
{
    if ($verboseMode) {
        SPL::CodeGen::println(@_);
    }
}

## @fn void warn($msg, @params, $sloc)
#  Prints \c 'WARNING' plus the contents of the format string.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message.  A leading CDISP[0-9][0-9][0-9][0-9][EW] will be moved to
#  before the WARNING
#  @param params an optional list of parameters to satisfy the format chars in msg
#  @param sloc an optional reference to a @link SPL::Operator::Instance::SourceLocation SourceLocation@endlink object
sub warn
{
    my $loc = SPL::CodeGenHelper::location(\@_);
    my ($msg, @params) = @_;
    my $msgId = "";
    my $sev = "";
    if ($msg =~ /^(CDISP[0-9][0-9][0-9][0-9][EW] )(.*)/s) {
        $msgId = $1;
        $msg = $2;
        $sev = SPL::Msg::WARNING();
    } else {
        $sev = SPL::Msg::WARNING_WITH_MESSAGE_ID();
    }
    print STDERR "${loc}${msgId}${sev}: ";
    if (scalar(@params) > 0) {
        printf STDERR $msg, @params;
    } else {
        print STDERR $msg;
    }
}

## @fn void warnln($msg, @params, $sloc)
#  This function is the same as warn() with the addition of a newline at the end.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message
#  @param params an optional list of parameters to satisfy the format chars in msg
#  @param sloc an optional reference to a @link SPL::Operator::Instance::SourceLocation SourceLocation@endlink object
sub warnln
{
    SPL::CodeGen::warn(@_);
    print STDERR "\n";
}

## @fn void warnVerbose($msg, @params, $sloc)
#  This function is the same as warn() except that it only prints if the \c -v
#  or \c --verbose-mode compiler option has been specified.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message
#  @param params an optional list of parameters to satisfy the format chars in msg
#  @param sloc an optional reference to a @link SPL::Operator::Instance::SourceLocation SourceLocation@endlink object
sub warnVerbose
{
    if ($verboseMode) {
        SPL::CodeGen::warn(@_);
    }
}

## @fn void warnlnVerbose($msg, @params, $sloc)
#  This function is the same as warnVerbose() with the addition of a new line at the end.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message
#  @param params an optional list of parameters to satisfy the format chars in msg
#  @param sloc an optional reference to a @link SPL::Operator::Instance::SourceLocation SourceLocation@endlink object
sub warnlnVerbose
{
    if ($verboseMode) {
       SPL::CodeGen::warn(@_);
       print STDERR "\n";
    }
}

## @fn void error($msg, @params, $sloc)
#  Prints \c 'ERROR ' plus the format string to STDERR.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message.  A leading CDISP[0-9][0-9][0-9][0-9]E will be moved to
#  before the ERROR
#  @param params an optional list of parameters to satisfy the format chars in msg
#  @param sloc an optional reference to a @link SPL::Operator::Instance::SourceLocation SourceLocation@endlink object
sub error
{
    my $loc = SPL::CodeGenHelper::location(\@_);
    my ($msg, @params) = @_;
    my $msgId = "";
    my $sev = "";
    if ($msg =~ /^(CDISP[0-9][0-9][0-9][0-9]E )(.*)/s) {
        $msgId = $1;
        $msg = $2;
        $sev = SPL::Msg::ERROR();
    } else {
        $sev = SPL::Msg::ERROR_WITH_MESSAGE_ID();
    }
    print STDERR "${loc}${msgId}${sev}: ";
    if (scalar(@params) > 0) {
        printf STDERR $msg, @params;
    } else {
        print STDERR $msg;
    }
    $SPL::CodeGen::sawError = 1;
}

## @fn void errorln($msg, @params, $sloc)
#  This function is the same as error() with the addition of a newline at the end.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message
#  @param params an optional list of parameters to satisfy the format chars in msg
#  @param sloc an optional reference to a @link SPL::Operator::Instance::SourceLocation SourceLocation@endlink object
sub errorln
{
    SPL::CodeGen::error(@_);
    print STDERR "\n";
}

## @fn void exit($msg, @params, $sloc)
#  This function enables the printing of the format string to STDERR prior to
#  exiting.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message
#  @param params an optional list of parameters to satisfy the format chars in msg
#  @param sloc an optional reference to a @link SPL::Operator::Instance::SourceLocation SourceLocation@endlink object
sub exit
{
    error(@_);
    exit $USER_ERROR;
}

## @fn void exitln($msg, @params, $sloc)
#  This method provides the same functionality as exit with the addition of a
#  new line at the end.
#  @param msg a string containing text to be printed, which may optionally contain
#  format characters (see \c printf function of Perl), in which case a set of
#  parameters should follow the message
#  @param params an optional list of parameters to satisfy the format chars in msg
#  @param sloc an optional reference to a @link SPL::Operator::Instance::SourceLocation SourceLocation@endlink object
sub exitln
{
    errorln(@_);
    CORE::exit $USER_ERROR;
}

## @fn void runtimePath($model, $path, $varName)
# This method takes a compile-time path and converts it to a runtime path.  If the compile-time path
# points into a toolkit, then that path will reflect the runtime location of that toolkit, otherwise
# the original path will be used.
# @param model the operator instance model
# @param path the path to be converted
# @param varName the variable name to assign the runtime path to
sub runtimePath($$$)
{
    my ($model, $path, $varName) = @_;
    SPL::CodeGenHelper::runtimePathImpl($model, $path, $varName);
}

## @fn void compileTimeExpression($model, $paramValue)
# This method takes a paramenter value representing a run-time path and converts it to an expression that
# can be used at compile time to access resources. undef is returned if the parameter value cannot be evaluated.
# The operator model must specify <splExpressionTree param="true"/> for this to be used.
# @param model the operator instance model
# @param paramValue The parameter value to be converted
sub compileTimeExpression($$)
{
    my ($model, $paramValue) = @_;
    if (!$paramValue->hasSPLExpressionTree()) {
        SPL::CodeGen::errorln(SPL::Msg::MISSING_SPL_EXPRESSION_TREE);
        return undef;
    }
    my $expr = $paramValue->getSPLExpressionTree();
    return SPL::CodeGenHelper::evaluateExpression($model, $expr);
}

## @param model the operator instance model
## @param inputPort the input port
sub validateTimeIntervalAttribute($$) {
    my ($model, $inputPort) = @_;
    # If this input port has a timeInterval window, then it must also
    # have an event-time attribute, and the attribute must be one
    # of the attributes in the tuple
    my $isTimeInterval = 0;
    my $window;
    if ($inputPort->hasWindow()) {
        $window = $inputPort->getWindow();
        $isTimeInterval = $window->isTimeInterval();
    }

    if ($isTimeInterval) {
        my $eventTimeContext = $model->getContext()->getOptionalContext("EventTime");
        my $eventTimeAttribute;
        if (defined($eventTimeContext)) {
            $eventTimeAttribute = $eventTimeContext->getAttribute();
        }
        if (!$eventTimeAttribute) {
            SPL::CodeGen::errorln(SPL::Msg::MISSING_EVENTTIME_ANNOTATION, $window->getSourceLocation());
        }
        else {
            # find the attribute in any nested types, including the path to it.

            my $eventTimeAttributeFound = 1;
            my $tupleType = $inputPort->getSPLTupleType;
            my @attributeParts = split(/\./, $eventTimeAttribute);

            # $primitiveAttribute will be the innermost, primitive attribute
            # name.  The remaining elements of @attributeParts will be
            # any containing tuples.
            my $primitiveAttribute = pop @attributeParts;

            while ($eventTimeAttributeFound && @attributeParts) {
                my $attributePart = shift @attributeParts;
                my @attributeNames = SPL::CodeGen::Type::getAttributeNames($tupleType);

                my @attributeTypes = SPL::CodeGen::Type::getAttributeTypes($tupleType);
                my $attributeCount = @attributeNames;
                for (my $attributeIndex = 0; $attributeIndex < $attributeCount; ++$attributeIndex) {
                    my $attributeName = $attributeNames[$attributeIndex];
                    my $attributeType = $attributeTypes[$attributeIndex];
                    if ($attributeName eq $attributePart) {
                        # if it is a tuple, continue,
                        if (SPL::CodeGen::Type::isTuple($attributeType)) {
                            $tupleType = $attributeType;
                            last;
                        }
                        # otherwise, bail
                        else {
                            $eventTimeAttributeFound = 0;
                            last;
                        }
                    }
                }
            }
            if ($eventTimeAttributeFound) {
                $eventTimeAttributeFound = 0;
                my @attributeNames = SPL::CodeGen::Type::getAttributeNames($tupleType);
                my @attributeTypes = SPL::CodeGen::Type::getAttributeTypes($tupleType);
                my $attributeCount = @attributeNames;
                for (my $attributeIndex = 0; $attributeIndex < $attributeCount; ++$attributeIndex) {
                    my $attributeName = $attributeNames[$attributeIndex];
                    my $attributeType = $attributeTypes[$attributeIndex];

                    if ($attributeName eq $primitiveAttribute) {
                        $eventTimeAttributeFound = 1;
                        last;
                    }
                }
            }

            if (!$eventTimeAttributeFound) {
                SPL::CodeGen::errorln (SPL::Msg::MISMATCH_ATTRIBUTE_EVENTTIME_ANNOTATION,
                    $window->getSourceLocation());
            }
        }
    }
}

## @param model the operator instance model
sub emitInputPortGetEventTime($$$)
{
    my ($model, $port, $handledAttributeTuplePairs) = @_;

    my $eventTimeContext = $model->getContext()->getOptionalContext("EventTime");
    if (defined($eventTimeContext)) {
        my $inputConnections = $eventTimeContext->getInputConnections();
        # @$inputConnections is now a list of triples:
        # x,y,z , where x is the port index, y is the connected
        # operator index, and z is the connected port index.  We
        # only need the port index here.

        my $portIndex = $port->getIndex();
        foreach my $inputConnection (@$inputConnections) {

            my ($inputPort, $connectedOperator, $connectedPort) = $inputConnection =~ /([0-9]+),([0-9]+),([0-9]+)/s;

            if ($inputPort == $portIndex) {
                emitGetEventTime($model, $port, $handledAttributeTuplePairs);
                last;
            }
        }
    }
}

sub emitOutputPortGetEventTime($$$)
{
    my ($model, $port, $handledAttributeTuplePairs) = @_;
    my $eventTimeContext = $model->getContext()->getOptionalContext("EventTime");
    if (defined($eventTimeContext)) {
        my $outputPorts = $eventTimeContext->getOutputPorts();
        my $portIndex = $port->getIndex();
        foreach my $outputPort (@$outputPorts) {
            if ($outputPort == $portIndex) {
                emitGetEventTime($model, $port, $handledAttributeTuplePairs);
                last;
            }
        }
    }
}

sub emitGetEventTime($$$)
{
    my ($model, $port, $handledAttributeTuplePairs) = @_;

    my $context = $model->getContext();
    my $eventTimeContext = $context->getOptionalContext("EventTime");
    if (defined($eventTimeContext)) {
        my $eventTimeAttribute = $eventTimeContext->getAttribute();
        my $eventTimeType = $eventTimeContext->getType();
        my $eventTimeResolution = $eventTimeContext->getResolution();
        my $tupleType = $port->getCppTupleType();

        # check if this attribute/tuple pair has already been handled.
        my $matchFound = 0;
        my $count = scalar @${handledAttributeTuplePairs};
        for (my $index = 0; $index < $count; $index += 2) {
            if (($handledAttributeTuplePairs->[$index] eq $eventTimeAttribute)
                && ($handledAttributeTuplePairs->[$index + 1] eq $tupleType)) {
                $matchFound = 1;
                last;
            }
        }

        if (!$matchFound) {
            push (@{$handledAttributeTuplePairs}, $eventTimeAttribute, $tupleType);
            emitEventTimeClass($port->getCppTupleType(), $eventTimeAttribute, $eventTimeType, $eventTimeResolution, 1);

        }
    }
}

sub emitAllGetEventTime($)
{
    my ($model) = @_;

    # Iterate through the input ports and output ports, and find each one with
    # an event-time context.  For each, emit a getEventTime method.

    # Track the attribute names and tuple types, and do not emit
    # duplicate definitions.

    my @handledAttributeTuplePairs = ();

    my $inputPortCount = $model->getNumberOfInputPorts;
    for (my $inputPortIndex = 0; $inputPortIndex < $inputPortCount; ++$inputPortIndex) {
        my $inputPort = $model->getInputPortAt($inputPortIndex);
        # perhaps there is a better place for this validation
        validateTimeIntervalAttribute($model, $inputPort);
        emitInputPortGetEventTime($model, $inputPort, \@handledAttributeTuplePairs);
    }

    my $outputPortCount = $model->getNumberOfOutputPorts;
    for (my $outputPortIndex = 0; $outputPortIndex < $outputPortCount; ++$outputPortIndex) {
        my $outputPort = $model->getOutputPortAt($outputPortIndex);
        emitOutputPortGetEventTime($model, $outputPort, \@handledAttributeTuplePairs);
    }
}

sub emitEventTimeClass($$$$$)
{
    my ($tupleType, $eventTimeAttribute, $eventTimeType, $eventTimeResolution, $includeSetter) = @_;
    $eventTimeType =~ s/SPL:://;

    my @path = split /\./, $eventTimeAttribute;
    $eventTimeAttribute = pop @path;
    my $callPath = "";
    if (@path) {
        $callPath = "get_" . join("().get_", @path) . "().";
    }

    emitALine "  template <>";
    emitALine "  struct EventTime<" . $tupleType . " > {";
    emitALine "    typedef " . $tupleType . " TupleType;";
    emitALine "    typedef " . $eventTimeType . " EventTimeType; // The type of the event time attribute";

    emitALine "    static EventTimeType get (TupleType const & t)";
    emitALine "    { return t." . $callPath . "get_" . $eventTimeAttribute . "(); }";

    emitALine "    static EventTimeType fromTimestamp(timestamp const & ts)";
    if ($eventTimeType eq "timestamp") {
        emitALine "    { return ts; }";
    }
    else {
        emitALine "    { return SPL::Functions::EventTime::" . $eventTimeType . "TicksFromTimestamp(ts, SPL::Functions::EventTime::" . $eventTimeResolution . "); }";
    }

    emitALine "    static timestamp toTimestamp(TupleType const & t)";
    emitALine "    { return toTimestamp(get(t)); }";

    emitALine "    static timestamp toTimestamp(EventTimeType t) ";
    if ($eventTimeType eq "timestamp") {
        emitALine "    { return t; }";
    }
    else {
        emitALine "    { return SPL::Functions::EventTime::toTimestamp" . "(t, SPL::Functions::EventTime::" . $eventTimeResolution . "); }";
    }

    # setEventTime is always passed a timestamp.  The generated
    # code should convert it to the event time type, taking into
    # consideration the resolution in use.
    if ($includeSetter) {
        emitALine "    static void setEventTime(TupleType & tuple, timestamp const & ts)";
        emitALine "    { tuple." . $callPath . "set_" . $eventTimeAttribute . "(fromTimestamp(ts)); }";
    }

    emitALine "  };";
}

sub hasEventTimeContext($) {
    my ($model) = @_;
    my $eventTimeContext = $model->getContext()->getOptionalContext("EventTime");
    defined($eventTimeContext);
}

1;
