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

## @class SPL::Operator::Instance::SourceLocation
# \brief Class that represents a location in an %SPL source file.
package SPL::Operator::Instance::SourceLocation;

use strict;
use warnings;

## @method string getFileName()
#  Get the file name associated with this source location.
#  @return a string containing the name of the spl source file
sub getFileName
{
  my ($self) = @_;
  return $self->{xml_}->{file}->[0];
}

## @method integer getLine()
#  Get the line number associated with this source location.
#  @pre isValid()==1
#  @return the line number
sub getLine
{
  my ($self) = @_;
  return $self->{xml_}->{line}->[0];
}

## @method integer getColumn()
#  Get the column number associated with this source location.
#  @pre isValid()==1
#  @return the column number
sub getColumn
{
  my ($self) = @_;
  return $self->{xml_}->{column}->[0];
}

## @method bool isValid()
#  Determine if this object represents a valid source location.
#  @return 1 if this is a valid source location, 0 otherwise
sub isValid
{
  my ($self) = @_;
  return 1 if ($self->getLine() >= 0);
  return 0;
}

# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml) = @_;
  my $self = {
    xml_  => $xml
  };
  return bless($self, $class);
}

1;
