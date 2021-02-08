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
# The attribute class represents a tuple's attributes.
package SPL::Tuple::Attribute;

use strict;
use warnings;

## @cmethod object new($xml)
#  Creates a new Attribute object
#  @param xml The XML tree describing the attribute.
#  @return Returns a new Attribute object.
sub new
{
   my ($class, $xml) = @_;

   my $self = {xml_ => $xml};

   return bless($self, $class);
}

## @method string getName()
#  Obtain the attribute's name.
#  @return Returns ia string containing the name of the attribute.
sub getName
{
   my ($self) = @_;
   return $self->{xml_}->{name}->[0];
}

## @method string getCppType()
#  Obtain the attribute's C++ type
#  @return Returns a string containing the C++ type.
sub getCppType
{
   my ($self) = @_;
   return $self->{xml_}->{cppType}->[0];
}

## @method string getSPLType()
#  Obtain the attribute's SPL type
#  @return Returns a string containing the SPL type.
sub getSPLType
{
   my ($self) = @_;
   return $self->{xml_}->{splType}->[0];
}

## @method unsigned hasFixedSize()
#  Check if there is a fixedSize for this attribute
#  @return Returns 1 if this attribute has a fixedSize, otherwise it returns 0.
sub hasFixedSize
{
  my ($self) = @_;
  return (defined $self->{xml_}->{fixedSize}) ? 1 : 0;
}

## @method FixedSize getFixedSize()
#  Obtain the fixedSize for this attribute
#  @note Note that you should verify there is a fixedSize using the hasFixedSize()
#  method prior to calling this method.
#  @return Returns the FixedSize
sub getFixedSize
{
  my ($self) = @_;
  return $self->{xml_}->{fixedSize}->[0]
}

1;
