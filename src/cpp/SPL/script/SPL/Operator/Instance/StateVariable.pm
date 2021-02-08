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

## @class SPL::Operator::Instance::StateVariable
# \brief Class that represents a state variable within an operator instance.
package SPL::Operator::Instance::StateVariable;

use strict;
use warnings;
use SPL::ModelHelper;

use SPL::Operator::Instance::Expression;
use SPL::Operator::Instance::SourceLocation;

## @method SourceLocation getSourceLocation()
# Get the %SPL source location that corresponds to this variable
# @return the SourceLocation object
sub getSourceLocation
{
  my ($self) = @_;
  return $self->{loc_};
}

## @method string getName()
# Get the name of the state variable
# @return a string containing the name of the state variable
sub getName
{
  my ($self) = @_;
  return $self->{xml_}->{name}->[0];
}

## @method string getCppType()
# Get the C++ type of the state variable
# @return a string containing the C++ type name
sub getCppType
{
  my ($self) = @_;
  return $self->{xml_}->{cppType}->[0];
}

## @method string getSPLType()
# Get the %SPL type of the state variable
# @return a string containing the spl type name
sub getSPLType
{
  my ($self) = @_;
  return SPL::ModelHelper::cleanType($self->{xml_}->{splType}->[0]);
}

## @method unsigned hasValue()
# Check if the state variable has a value.
# @return 1 if the state variable has a value expression, 0 otherwise
sub hasValue
{
  my ($self) = @_;
  return 1
    if ($self->{xml_}->{value});
  return 0;

}

## @method string getValue()
# Get a string representation of the C++ value of the state variable.
# @return a string containing the C++ value
sub getValue
{
  my ($self) = @_;
  return $self->{value_};
}

# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml) = @_;

  my $self = {
    xml_       => $xml,
    value_     => undef,
    loc_       => undef
  };

  $self->{value_} = SPL::Operator::Instance::Expression->new($self->{xml_}->{value}->[0], undef)
    if($self->{xml_}->{value});

  $self->{loc_} = SPL::Operator::Instance::SourceLocation->new($self->{xml_}->{sourceLocation}->[0]);
  return bless($self, $class);
}

1;
