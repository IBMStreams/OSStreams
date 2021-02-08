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
# A class representing the model of a processing element
package SPL::PE::PEModel;

use strict;
use warnings;
use XML::Simple;
$XML::Simple::PREFERRED_PARSER = "XML::Parser";

use SPL::PE::Context;
use SPL::PE::Operator;
use IO::File;

## @cmethod object new($model)
#  Creates a new PEModel object
#  @param model The XML describing the processing element.  This is parsed into
#  an XML tree.
#  @return Returns a new PEModel object.
sub new {
  my ($class, $model) = @_;

  my $self = {
    xml_              =>  undef,
    context_          =>  undef,
    operators_        =>  []
  };

  my $xml = XML::Simple->new (ForceArray => 1, KeepRoot => 0);
  my $fh = IO::File->new($model);
  $self->{xml_} = $xml->XMLin($fh);
  $fh->close();
  $self->{context_} = SPL::PE::Context->new($self->{xml_}->{context}->[0]);

  # extract operator information
  if ($self->{xml_}->{operators}->[0]->{operator}){
    foreach my $oelem (@{$self->{xml_}->{operators}->[0]->{operator}}) {
      my $oper = SPL::PE::Operator->new($oelem);
      push @{$self->{operators_}}, $oper;
    }
  }

  return bless($self, $class);
}

## @method Context getContext()
#  Obtain the context object
#  @return Returns the Context object.
sub getContext {
  my ($self) = @_;
  return $self->{context_};
}

## @method unsigned getNumberOfOperators()
#  Obtain the number of operators associated with this processing element.
#  @return Returns the number of operators.
sub getNumberOfOperators {
  my ($self) = @_;
  return scalar(@{$self->{operators_}});
}

## @method Operator getOperatorAt($index)
#  Obtain the operator object at a given index
#  @param index The index of the desired operator.
#  @return Returns the desired Operator object.
sub getOperatorAt {
  my ($self, $index) = @_;
  return $self->{operators_}->[$index];
}

## @method \@Operator getOperators()
#  Obtain the set of operator objects associated with the processing element.
#  @return Returns the list of Operator objects.
sub getOperators {
  my ($self) = @_;
  return $self->{operators_};
}

1;
