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

# DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::RuntimeConstant
# \brief Class that represents a runtime constant.
package SPL::Operator::Instance::RuntimeConstant;

use strict;
use warnings;
use SPL::ModelHelper;

## @method string getName()
# Obtain the name of the runtime constant
# @return Returns the name of the runtime constant.
sub getName {
  my ($self) = @_;
  return $self->{xml_}->{name}->[0];
}

## @method string getCppType()
# Obtain the C++ type of the runtime constant
# @return Returns a string containing the C++ type name.
sub getCppType {
  my ($self) = @_;
  return $self->{xml_}->{cppType}->[0];
}

## @method string getSPLType()
# Obtain the type of the runtime constant, as it appears in spl source
# @return Returns a string containing the spl type name.
sub getSPLType {
  my ($self) = @_;
  return SPL::ModelHelper::cleanType($self->{xml_}->{splType}->[0]);
}

## @method string getValue()
# Get a string representation of the value of this runtime constant.
# @return Returns a string containing the value.
sub getValue {
  my ($self) = @_;
  return $self->{xml_}->{value}->[0];
}

# DOXYGEN_SKIP_FOR_USERS

## @method string hasCommandLineArg()
# Is there a command line arg field for this runtime constant?
# @return true if a command line arg is present
sub hasCommandLineArg {
  my ($self) = @_;
  return defined $self->{xml_}->{commandLineArg};
}

## @method string getCommandLineArg()
# Get a string representation of the command line argument of this runtime constant.
# @return Returns a string containing the commandLineArgument.
sub getCommandLineArg {
  my ($self) = @_;
  return $self->{xml_}->{commandLineArg}->[0];
}

sub new {
  my ($class, $xml) = @_;

  my $self = {
    xml_ => $xml
  };

  return bless($self, $class);
}

1;
