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
# A class representing a processing element's Java virtual machine
package SPL::PE::JavaVM;

use strict;
use warnings;

use SPL::ModelHelper;

## @cmethod object new($xml)
#  Creates a new JavaVM object
#  @param xml The XML tree describing the JavaVM
#  @return Returns a new JavaVM object.
sub new {
  my ($class, $xml) = @_;
  my $self = {
    xml_ => $xml
  };
  return bless($self, $class);
}

## @method int vmArgsCount()
#  Returns the number of vmArgs
#  @return Returns the number of vmArgs
sub getNumberOfVMArgs {
  my ($self) = @_;
  return scalar(@{$self->{xml_}->{vmArgs}->[0]->{vmArg}})
      if defined($self->{xml_}->{vmArgs}->[0]->{vmArg});
  return 0;
}

## @method string getVMArgAt($index)
#  Obtain the vmArg string at a given index
#  @param index The index of the desired vmArg
#  @return Returns the desired vmArg string.
sub getVMArgAt {
  my ($self, $index) = @_;
  return $self->{xml_}->{vmArgs}->[0]->{vmArg}->[$index];
}

1;
