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

## @class
# A class representing a processing element's context.
package SPL::PE::Context;

use strict;
use warnings;

use SPL::ModelHelper;
use SPL::PE::JavaVM;
use SPL::PE::NLLocation;

## @cmethod object new($xml)
#  Creates a new Context object
#  @param xml The XML tree describing the context.
#  @return Returns a new Context object.
sub new {
  my ($class,$xml) = @_;

  my $self = {
    xml_ => $xml,
    nlLocations_ => undef,
    javaVM_ => undef
  };

  $self->{javaVM_} = SPL::PE::JavaVM->new($self->{xml_}->{javaVM}->[0])
    if $self->{xml_}->{javaVM};

  return bless($self, $class);
}

## @method string getClass()
#  Obtain the C++ class name of the processing element
#  @return Returns a string containing the name
sub getClass {
  my ($self) = @_;
  return $self->{xml_}->{class}->[0];
}

## @method string getAvoidRuntimeYield()
#  Obtain the setting for the 'avoid-runtime-yield' parameter
#  @return Returns a string containing the kind
sub getAvoidRuntimeYield {
  my ($self) = @_;
  return $self->{xml_}->{avoidRuntimeYield}->[0];
}

## @method unsigned hasJavaVM()
# Check if the PE has a Java virtual machine.
# @return 1 if the PE has a JVM, 0 otherwise
sub hasJavaVM {
  my ($self) = @_;
  return 1 if $self->{javaVM_};
  return 0;
}

## @method JavaVM getJavaVM()
# Get the JavaVM information for this PE.
# @return the JavaVM object
sub getJavaVM {
  my ($self) = @_;
  return $self->{javaVM_};
}

1;
