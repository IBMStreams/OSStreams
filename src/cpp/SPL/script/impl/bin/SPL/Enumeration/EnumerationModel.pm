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
# A class that represents an enumeation in the spl code.
package SPL::Enumeration::EnumerationModel;

use strict;
use warnings;
use XML::Simple;
$XML::Simple::PREFERRED_PARSER = "XML::Parser";

use SPL::Enumeration::Enumerator;
use IO::File;

## @cmethod new object($model)
# Creates a new EnumerationModel object
# @param model The XML describing the enumeration.  This is parsed
# into an XML tree.
sub new
   {
   my ($class, $model) = @_;

   my $self = { xml_ => undef, enumerators_ => [] };
   my $xml = XML::Simple->new(ForceArray => 1, KeepRoot => 0);

   my $fh = IO::File->new($model);
   $self->{xml_} = $xml->XMLin($fh);
   $fh->close();

   # extract enumerators
   foreach my $elem (@{$self->{xml_}->{enumerators}->[0]->{enumerator}})
      {
      my $enum = SPL::Enumeration::Enumerator->new($elem);
      push @{$self->{enumerators_}}, $enum;
      }

   return bless($self, $class);
   }

## @method string getClass()
#  Obtain the name of the class for this enum.
#  @return Returns a string containing the name of the class representing this enumeration.
sub getClass
   {
   my ($self) = @_;
   return $self->{xml_}->{class}->[0];
   }


## @method unsigned getNumberOfEnumerators()
#  Obtain the number of enumerators this enum has.
#  @return Returns the number of enumerators.
sub getNumberOfEnumerators
   {
   my ($self) = @_;
   return scalar(@{$self->{enumerators_}});
   }

## @method Enumerator getEnumeratorAt($index)
#  Obtain the enumerator at the specified index
#  @param index The index of the desired enumerator
#  @return Returns the desired enumerator.
sub getEnumeratorAt
   {
   my ($self, $index) = @_;
   return $self->{enumerators_}->[$index];
   }

## @method \@Enumerator getEnumerators()
#  Obtain the list of enumerators for this enum.
#  @return Returns the list of enumerators.
sub getEnumerators
   {
   my ($self) = @_;
   return bless( $self->{enumerators_}, "SPL::Enumeration::Enumerator" );
   }

1;
