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

## @class SPL::Operator::Instance::InputAttribute
# \brief Class that represents an attribute of an input port.
#
# Important objects in an InputAttribute:
# - The name of the attribute
# - The %SPL Type of the attribute
# - The C++ Type of the attribute
package SPL::Operator::Instance::InputAttribute;

use strict;
use warnings;
use SPL::ModelHelper;

## @method string getName()
# Get the name of this attribute.
# @return a string containing the name of the attribute
#
# Example use of %getName to work with an input attribute
# @code
# <%my $aName = $attr->getName();%>
# doWork (ituple.get_<%=$aName%>());
# @endcode
sub getName
{
  my ($self) = @_;
  return $self->{xml_}->{name}->[0];
}

## @method string getCppType()
# Get the C++ type of this attribute.
# @return a string containing the C++ type name of the attribute
#
# Example use of %getCppType to work with an input attribute
# @code
# <%
# my $cppType = $attr->getCppType();
# my $aName = $attr->getName();
# %>
# // Create a reference to an input attribute
# const <%=$cppType%>& iVar = ituple.get_<%=$aName%>();
# @endcode
sub getCppType
{
  my ($self) = @_;
  return $self->{xml_}->{cppType}->[0];
}

## @method string getSPLType()
# Get the %SPL type of this attribute.
# @return a string containing the %SPL type name of the attribute
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

# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml) = @_;

  my $self = {
    xml_ => $xml
  };

  return bless($self, $class);
}

1;
