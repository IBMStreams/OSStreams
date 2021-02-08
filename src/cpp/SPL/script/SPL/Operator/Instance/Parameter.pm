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

## @class SPL::Operator::Instance::Parameter
# \brief Class that represents an operator parameter.
#
# A Parameter contains the following objects:
# - SourceLocation of the Parameter.
# - The name of the Parameter.
# - The number of expressions in the Parameter list.
# - The expressions in the Parameter list.
#
# Each Parameter contains one or more @link Expression Expressions@endlink.  These can be
# accessed as a Perl list of @link Expression Expressions@endlink using getValues(),
# or accessed one at a time using getValueAt().
#
# Example use of an optional Parameter that is a CustomLiteral with cardinality 1.\n
# <tt>
# my $format = $model->getParameterByName("format");\n
# // Since there is only one expression allowed, it is at index 0.\n
# // If the parameter was not provided, default to 'csv'\n
# $format = $format ? $format->getValueAt(0)->getSPLExpression() : "csv";\n
# ...\n
# // Later use\n
# <%%if ($format eq "binary") {%>\n
# &nbsp;&nbsp;// C++ code here\n
# <%}%>
# </tt>
#
# Example use of a manditory Parameter that is an rstring Expression with cardinality 1.
# <tt>
# my $fileName = $model->getParameterByName("fileName");\n
# // Since there is only one expression allowed, it is at index 0.\n
# $fileName = $fileName->getValueAt(0)->getCppExpression();\n
# ...\n
# // Later use\n
# FILE *f = fopen (<%%=$fileName%>, "r");
# </tt>
package SPL::Operator::Instance::Parameter;

use strict;
use warnings;

use SPL::Operator::Instance::Expression;
use SPL::Operator::Instance::SourceLocation;

## @method SourceLocation getSourceLocation()
# Get the %SPL source location that corresponds to this parmeter
# @return the SourceLocation object
sub getSourceLocation
{
  my ($self) = @_;
  return $self->{loc_};
}

## @method string getName()
# Get the name of the parameter
# @return a string containing the name of the parameter
sub getName
{
  my ($self) = @_;
  return $self->{xml_}->{name}->[0];
}

## @method unsigned getNumberOfValues()
# Get the number of values associated with this parameter.
# @return the number of value expressions for this parameter
sub getNumberOfValues
{
  my ($self) = @_;
  return scalar(@{$self->{values_}});
}

## @method Expression getValueAt($index)
# Get the expression object at a given index within the list of values
# asociated with this parameter
# @param index the index of the desired value expression
# @return the desired Expression object
sub getValueAt
{
  my ($self, $index) = @_;
  return $self->{values_}->[$index];
}

## @method \@Expression getValues()
# Get the list of expression objects asociated with this parameter.
# @return the possibly empty list of Expression objects
sub getValues
{
  my ($self) = @_;
  return $self->{values_};
}

# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class,$xml,$context) = @_;

  my $self = {
    xml_       => $xml,
    values_    => [],
    loc_       => undef
  };

  # extract values
  if ($self->{xml_}->{values}->[0]->{value}) {
    foreach my $velem (@{$self->{xml_}->{values}->[0]->{value}}) {
       my $value = SPL::Operator::Instance::Expression->new($velem, $context);
       push @{$self->{values_}}, $value;
    }
  }

  $self->{loc_} = SPL::Operator::Instance::SourceLocation->new($self->{xml_}->{sourceLocation}->[0]);
  return bless($self, $class);
}

1;
