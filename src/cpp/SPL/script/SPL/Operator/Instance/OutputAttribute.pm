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

## @class SPL::Operator::Instance::OutputAttribute
# \brief Class that represents an output attribute of the operator instance.
#
# Important objects in an OutputAttribute:
# - The C++ Type for the Attribute
# - The %SPL Type for the Attribute (useful for error messages)
# - Does this attribute have an output assignment?
#   - The Expression for the output assignment
# - Does this attribute have an output assignment using an output function?
#   - The name of the output function
#   - The Expression parameters to the output function call
package SPL::Operator::Instance::OutputAttribute;

use strict;
use warnings;
use SPL::ModelHelper;

use SPL::Operator::Instance::Expression;

## @method string getName()
# Get the name of this attribute
# @return a string containing the name of this attribute
#
# Example use of %getName to work with an output attribute
# @code
# <%my $aName = $attr->getName();%>
# otuple.set_<%=$aName%>(55);
# @endcode
sub getName
{
  my ($self) = @_;
  return $self->{xml_}->{name}->[0];
}

## @method string getCppType()
# Get the C++ type of this attribute.
# @return a string containing the C++ type name
#
# Example use of %getCppType to work with an output attribute
# @code
# <%
# my $cppType = $attr->getCppType();
# my $aName = $attr->getName();
# %>
# // Create a reference to an output attribute
# <%=$cppType%>& oVar = otuple.get_<%=$aName%>();
# // Use the reference: oVar = 55;
# @endcode
sub getCppType
{
  my ($self) = @_;
  return $self->{xml_}->{cppType}->[0];
}

## @method string getSPLType()
# Get the %SPL type of this attribute.
# @return a string containing the %SPL type name
#
# Example use of %getSPLType for type checking
# @code
# <%
# my $aType = $attr->getSPLType();
# my $aName = $attr->getName();
# if (!SPL::CodeGen::Type::isNumeric($aType) {
#     errorln ("Attribute '$aName' has type '$aType', expected numeric type");
# }
# %>
# @endcode
sub getSPLType
{
  my ($self) = @_;
  return SPL::ModelHelper::cleanType($self->{xml_}->{splType}->[0]);
}

## @method unsigned hasAssignment().
# Check if this attribute has an assignment
# @return 1 if this output attribute has an assignment, either explicitly in an output clause, or due to a compiler-generated assignment, 0 otherwise
#
# Example use of %hasAssignment:
# @code
# <%
# if ($attr->hasAssignment()) {
#     # The output clause has an assignment for this attribute:  output A : b = 5;
#     # hasAssignmentWithOutputFunction can be called to see if the right-hand
#     # side is a custom output function, or a regular expression
# }
# %>
# @endcode
sub hasAssignment
{
  my ($self) = @_;
  return 1 if($self->{xml_}->{assignment});
  return 0;
}

## @method unsigned hasAssignmentWithOutputFunction()
# Check if this attribute has an assignment with a custom output function
# @return 1 if the assignment is an output function, 0 otherwise
#
# Example use of %hasAssignmentWithOutputFunction:
# @code
# <%
# if ($attr->hasAssignment()) {
#     # The output clause has an assignment for this attribute:  output A : b = f(x);
#     if ($attr->hasAssignmentWithOutputFunction()) {
#         # right hand side is a custom output function
#     } else {
#         # right hand side is an SPL expression without custom output functions
#     }
# }
# %>
# @endcode
sub hasAssignmentWithOutputFunction
{
  my ($self) = @_;
  return 1 if(
    $self->hasAssignment() and
    $self->{xml_}->{assignment}->[0]->{outputFunction});
  return 0;
}

## @method Expression getAssignmentValue()
# Get the assignment expression associated with this attribute.
# @pre hasAssignment() == 1
# @return the desired assignment Expression object
#
# Example use of %getAssignmentValue:
# @code
# <%
# if ($attr->hasAssignment()) {
#     # The output clause has an assignment for this attribute:  output A : b = f(x);
#     if (!$attr->hasAssignmentWithOutputFunction()) {
#         # right hand side is an SPL expression without custom output functions
#         my $expn = $attr->getAssignmentValue()->getCppExpression();
#         my $aName = $attr->getName();%>
#         otuple.set_<%=$aName%>(<%=$expn%>);
#     }
# <%}%>
# @endcode
sub getAssignmentValue
{
  my ($self,$index) = @_;
  return $self->{values_}->[0];
}

## @method Expression getAssignmentSourceLocation()
# Get the source location of the assignment expression associated with this attribute.
# @pre hasAssignment() == 1
# @return the source location for the assignment expression
#
sub getAssignmentSourceLocation
{
  my ($self,$index) = @_;
  return $self->{assignmentLocation_};
}

## @method string getAssignmentOutputFunctionName()
# Get the assignment output function name for this attribute.
# @pre hasAssignmentWithOutputFunction()==1
# @return the assignment output function name
#
# Example use of %getAssignmentOutputFunctionName:
# @code
# <%
# if ($attr->hasAssignmentWithOutputFunction())
#     my $fcnName = $attr->getAssignmentOutputFunctionName();
#     if ($fcnName eq "Max") {
#         # generate code for handling Max case
#     }
# }
# %>
# @endcode
sub getAssignmentOutputFunctionName
{
  my ($self) = @_;
  return undef unless $self->hasAssignmentWithOutputFunction();
  return $self->{xml_}->{assignment}->[0]->{outputFunction}->[0];
}

## @method \@Expression getAssignmentOutputFunctionParameterValues()
# Get the list of expressions associated with this attribute's assignment output
# function parameters.
# @pre hasAssignmentWithOutputFunction()==1
# @return the list of Expression objects
sub getAssignmentOutputFunctionParameterValues
{
  my ($self) = @_;
  return $self->{values_};
}

## @method Expression getAssignmentOutputFunctionParameterValueAt($index)
# Get the expression at a given index within the list of assignment expressions
# associated with this attribute's assignment output function parameters.
# @pre hasAssignmentWithOutputFunction()==1
# @param index the index of the desired expresion
# @return the desired Expression object
#
# Example use of %getAssignmentOutputFunctionParameterValueAt:
# @code
# <%
# if ($attr->hasAssignmentWithOutputFunction())
#     my $fcnName = $attr->getAssignmentOutputFunctionName();
#     if ($fcnName eq "twoArgFcn") {
#         my $arg0 = $attr->getAssignmentOutputFunctionParameterValueAt(0)->getCppExpression();
#         my $arg1 = $attr->getAssignmentOutputFunctionParameterValueAt(1)->getCppExpression();%>
#         callTwoArgFcn(<%=$arg0%>, <%=$arg1%>);
# <%}%>
# @endcode
sub getAssignmentOutputFunctionParameterValueAt
{
  my ($self,$index) = @_;
  return $self->{values_}->[$index];
}

# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  my $self = {
    xml_    => $xml,
    values_ => [],
    assignmentLocation_ => undef
  };

  if($self->{xml_}->{assignment}) {
    $self->{assignmentLocation_} =
        SPL::Operator::Instance::SourceLocation->new (
            $xml->{assignment}->[0]->{sourceLocation}->[0]);
    foreach my $velem (@{$self->{xml_}->{assignment}->[0]->{values}->[0]->{value}}) {
      my $value = SPL::Operator::Instance::Expression->new($velem,$context);
      push @{$self->{values_}}, $value;
    }
  }

  return bless($self, $class);
}

1;
