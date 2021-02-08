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
# A class representing the model for a tuple.
package SPL::Tuple::TupleModel;

use strict;
use warnings;
use XML::Simple;
$XML::Simple::PREFERRED_PARSER = "XML::Parser";

use SPL::Tuple::Attribute;
use SPL::ModelHelper;
use IO::File;

## @cmethod object new($model)
#  Creates a new TupleModel object.
#  @param model The xml defining the tuple.  This is parsed into an XML tree.
#  @return Returns a new TupleModel object
sub new
   {
   my ($class, $model) = @_;

   my $self = { xml_ => undef, headers_ => [], attributes_ => [] };
   my $xml = XML::Simple->new(ForceArray => 1, KeepRoot => 0);

   my $fh = IO::File->new($model);
   $self->{xml_} = $xml->XMLin($fh);
   $fh->close();

   # extract headers
   foreach my $elem (@{$self->{xml_}->{headers}->[0]->{header}})
      {
      push @{$self->{headers_}}, $elem;
      }

   # extract attributes
   foreach my $elem (@{$self->{xml_}->{attributes}->[0]->{attribute}})
      {
      my $attr = SPL::Tuple::Attribute->new($elem);
      push @{$self->{attributes_}}, $attr;
      }

   return bless($self, $class);
   }

## @method string getClass()
#  Obtain the name of the C++ class for this tuple.
#  @return Returns a string containing the class name.
sub getClass
   {
   my ($self) = @_;
   return $self->{xml_}->{class};
   }

## @method string getSPLType()
#  Obtain the name of the SPL type for this tuple.
#  @return Returns a string containing the SPL type name.
sub getSPLType
   {
   my ($self) = @_;
   return $self->{xml_}->{splType};
   }

## @method unsigned getFacade()
# Obtain the facade for the tuple, as represented in the tuple model.
# @return Returns 1 if the tuple is a facade tuple, otherwise it returns 0.
sub getFacade
  {
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest($self->{xml_}->{facade});
  }

## @method unsigned getEqualitySupported()
# Obtain the equalitySupported for the tuple, as represented in the tuple model.
# @return Returns 1 if the tuple supports == and !=, otherwise return 0;
sub getEqualitySupported
  {
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest($self->{xml_}->{equalitySupported});
  }

## @method \@string getHeaders()
#  Obtain the list of header files that the C++ class needs to compile.
#  @return Returns a (possibly empty) list of headers.
sub getHeaders
   {
   my ($self) = @_;
   return $self->{headers_};
   }

## @method unsigned getNumberOfAttributes()
#  Obtain the number of attributes this tuple has.
#  @return Returns the number of attributes.
sub getNumberOfAttributes
   {
   my ($self) = @_;
   return scalar(@{$self->{attributes_}});
   }

## @method Attribute getAttributeAt($index)
#  Obtain the attribute at the specified index
#  @param index The zero-based index of the desired attribute.
#  @return Returns the desired attribute.
sub getAttributeAt
   {
   my ($self, $index) = @_;
   return $self->{attributes_}->[$index];
   }

## @method \@Attributes getAttributes()
#  Obtain the list of attributes for this tuple.
#  @returns Returns the list of Attribute objects.
sub getAttributes
   {
   my ($self) = @_;
   return $self->{attributes_};
   }

1;
