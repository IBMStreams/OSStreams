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

## @class SPL::Operator::Instance::Expression
# \brief Class that represents an expression within an operator instance.
#
# An Expression contains the following objects:
# - The SourceLocation of the Expression.
# - The C++ type of the Expression.
# - The %SPL type of the Expression.
# - The C++ expression that will evaluate the Expression.
# - The %SPL expression that corresponds to the original %SPL expression.
# - A boolean (hasFunctions) which will be <tt>1</tt> if the Expression contains subroutine calls.
# - A boolean (hasSideEffects) which will be <tt>1</tt> if the Expression has a side effect such as <tt>i++</tt>.
# - A boolean (readsState) which will be <tt>1</tt> if the Expression reads logic state data.
# - A boolean (hasNestedCustomOutputFunction) which will be <tt>1</tt> if the Expression calls a custom output function within a nested expression.
# - A boolean (hasStreamAttributes) which will be <tt>1</tt> if the Expression has a reference to an input stream attribute.
#
# Example use of an Expression in a Code Generation Template:
# @code
# <%
# my $cppType = $expn->getCppType();
# my $cppExpn = $expn->getCppExpression();
# %>
# // Ensure expression is evaluated only one time
# <%=$cppType%> tempVar = <%=$cppExpn%>;
# @endcode
package SPL::Operator::Instance::Expression;

use strict;
use warnings;

use SPL::ModelHelper;
use SPL::Operator::Instance::SourceLocation;
use SPL::Operator::Instance::ExpressionTree;

## @method SourceLocation getSourceLocation()
#  Get the %SPL source location that corresponds to this expression.
#  @return the SourceLocation object
sub getSourceLocation
{
  my ($self) = @_;
  return $self->{loc_};
}

## @method string getCppType()
# Get the C++ type of the expression.
# @return a string with the name of the C++ type of this expression
#
# Example use of %getCppType to declare a temporary and assign the C++ value of the expression:
#
# @code
# <%
# my $cppType = $expn->getCppType();
# my $cppExpn = $expn->getCppExpression();
# %>
# // Ensure expression is evaluated only one time
# <%=$cppType%> tempVar = <%=$cppExpn%>;
# @endcode
sub getCppType
{
  my ($self) = @_;
  return $self->{xml_}->{cppType}->[0];
}

## @method string getSPLType()
# Get the %SPL type of the expression.
# @return a string with the name of the %SPL type
#
# Example use of %getSPLType() in an error message
# @code
# <%
# my $expnType = $expn->getSPLType();
# if (!SPL::CodeGen::Type::isNumeric ($expnType)) {
#     SPL::CodeGen::errorln ("Expected numeric type, found '$expnType'",
#                            $expn->getSourceLocation())
# }
# %>
# @endcode
sub getSPLType
{
  my ($self) = @_;
  return SPL::ModelHelper::cleanType($self->{xml_}->{splType}->[0]);
}

## @method string getCppExpression()
# Get the C++ representation of the expression.
# @return a string with the C++ expression
# @note The value returned by %getCppExpression() contains a C++ expression that corresponds
# to the %SPL expression. If expression rewrite is allowed for the expression, the compiler
# may perform folding and literal replacement on this expression. If the expression is to be
# embedded into the generated code, then whether the compiler performed expression rewrite
# or not is of no consequence from the operator developer's perspective. However, if the
# expression is to be inspected at compile-time, the developer needs to be wary of expression rewrite.
#
# Example use of %getCppExpression to access the C++ value of the expression:
#
# @code
# <%
# my $cppExpn = $expn->getCppExpression();
# %>
# // assumes C++ expression can be assigned to an int
# int tempVar = <%=$cppExpn%>;
# @endcode
sub getCppExpression
{
  my ($self) = @_;
  return $self->{xml_}->{cppExpression}->[0];
}

## @method string getSPLExpression()
# Get the expression as it appears in %SPL source
# @return a string with the %SPL expression
# @note The value returned by %getSPLExpression() contains the %SPL expression as it appears
# in the source code for the operator instance in question. However, if expression rewrite is
# allowed for the expression, then the compiler is free to share code among different instances
# of the same kind of operator. This means that code won't be generated for some of the instances.
# This works because the compiler rewrites the expressions to contain runtime literals whose
# values are loaded at runtime, making sure that different instances of the same kind of
# operator are initialized differently despite sharing their generated source code. An important
# consequence of this is that, <b>%getSPLExpression() should never be used to generate code when
# expression rewrite is allowed</b>. Use getCppExpression() for generating code.
# %getSPLExpression() can still be used for compile-time error reporting.
#
# Example use of %getSPLExpression() in an error message
# @code
# <%
# my $expnType = $expn->getSPLType();
# if (!SPL::CodeGen::Type::isNumeric ($expnType)) {
#     my $splExpn = $expn->getSPLExpression();
#     SPL::CodeGen::errorln ("Expected numeric expression, found '$splExpn'",
#                            $expn->getSourceLocation())
# }
# %>
# @endcode
sub getSPLExpression
{
  my ($self) = @_;
  return $self->{xml_}->{splExpression}->[0];
}

## @method bool hasFunctions()
# Check if the expression contains functions.
# @return 1 if this expression contains a function call, 0 otherwise
sub hasFunctions
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest(
      $self->{xml_}->{hasFunctions}->[0])
}

## @method bool hasSideEffects()
# Check if the expression contains side effects (increment/decrement, calls to stateful functions,
# calls to functions with mutable parameters, etc.)
# @return 1 if this function can change the state of the program, 0 otherwise.
sub hasSideEffects
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest($self->{xml_}->{hasSideEffects}->[0]);
}

## @method bool readsState()
# Check if the expression reads SPL logic state data
# @return 1 if state is read, otherwise 0
sub readsState
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest($self->{xml_}->{readsState}->[0]);
}

## @method bool hasNestedCustomOutputFunction()
# Check if the expression contains a call to a custom output function that isn't at the 'root'
# @return 1 if this function contains a call to a nested custom output function, 0 otherwise.
sub hasNestedCustomOutputFunction
{
  my ($self) = @_;
  return 0 if !$self->{xml_}->{hasNestedCustomOutputFunction};
  return SPL::ModelHelper::booleanTest(
      $self->{xml_}->{hasNestedCustomOutputFunction}->[0]);
}

## @method bool hasStreamAttributes()
# Check if the expression contains stream attributes.
# @return 1 if this expression contains a reference to a stream attribute, 0 otherwise
sub hasStreamAttributes
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest(
      $self->{xml_}->{hasStreamAttributes}->[0]);
  return 0;
}

## @method string getParameterExpressionMode()
# Get the mode of the expression in a parameter
# @return a string containing the mode of the expression:
# Attribute/AttributeFree/Constant/CustomLiteral/Expression
# Only valid for expressions from parameters
sub getParameterExpressionMode
{
  my ($self) = @_;
  return $self->{xml_}->{parameterExpressionMode}->[0];
}

## @method ExpressionTree getSPLExpressionTree()
#  Get the %SPL expression tree that corresponds to this expression or undef if not present.
#  @return the ExpressionTree object or undef
sub getSPLExpressionTree
{
  my ($self) = @_;
  return $self->{tree_};
}

## @method bool hasSPLExpressionTree()
# Check if the expression contains an SPL expression tree.
# @return 1 if this expression contains an SPL expression tree, 0 otherwise
sub hasSPLExpressionTree
{
  my ($self) = @_;
  return $self->{tree_} ? 1 : 0;
}

# DOXYGEN_SKIP_FOR_USERS

## @method unsigned hasRuntimeConstants()
# Check if the expression has a runtime constant
# @return Returns 1 if this expression contains any runtime constants,
# otherwise it returns 0.
sub hasRuntimeConstants {
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest(
      $self->{xml_}->{hasRuntimeConstant}->[0]);
}

sub new {
  my ($class, $xml, $context) = @_;

  my $self = {
    xml_    => $xml,
    loc_    => undef,
    tree_   => undef
  };

  $self->{loc_} = SPL::Operator::Instance::SourceLocation->new($self->{xml_}->{sourceLocation}->[0]);
  my $tree = $self->{xml_}->{expressionTree}->[0];
  $self->{tree_} = new SPL::Operator::Instance::ExpressionTree($tree, $context) if ($tree);

  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS
