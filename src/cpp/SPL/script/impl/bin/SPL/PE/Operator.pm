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
# A class that represents an operator associated with a processing element.
package SPL::PE::Operator;

use strict;
use warnings;

## @cmethod object new($xml)
#  Creates a new Operator object
#  @param xml The XML tree describing the operator.
#  @return Returns a new Operator object.
sub new {
  my ($class,$xml) = @_;

  my $self = {
    xml_ => $xml
  };

  return bless($self, $class);
}

## @method string getClass()
#  Obtain the operator class name
#  @return Returns a string containing the operator class name.
sub getClass {
  my ($self) = @_;
  return $self->{xml_}->{class}->[0];
}

1;
