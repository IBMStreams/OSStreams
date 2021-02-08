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

## @class SPL::Operator::Instance::ConsistentRegionContext
# \brief Class that represents the context of the operator regarding a consistent region. The
# ConsistentRegionContext is an OptionalContext that can be obtained with the "ConsistentRegion"
# string.
package SPL::Operator::Instance::ConsistentRegionContext;

use strict;
use warnings;
use SPL::Operator::Instance::OptionalContext;

our @ISA = qw(SPL::Operator::Instance::OptionalContext);


## @method bool isStartOfRegion()
# Check if the operator is the start of a consistent region.
# @return true if the operator is the start of a consistent region, false otherwise.
sub isStartOfRegion
{
  my ($self) = @_;
  return $self->{isStartOfRegion_};
}

## @method bool isEndOfRegion()
# Check if the operator is the end of a consistent region.
# @return true if the operator is the end of a consistent region, false otherwise.
sub isEndOfRegion
{
  my ($self) = @_;
  return $self->{isEndOfRegion_};
}

## @method bool isTriggerOperator()
# Check if the operator is the trigger of an operator-driven consistent region and must
# explicitly start the establishment of consistent states.
# @return true if the operator is the trigger operator of a consistent region, false otherwise.
sub isTriggerOperator
{
  my ($self) = @_;
  return $self->{isTriggerOperator_};
}

## @method bool isConsistentInputPort($index)
# Check if a given input port is consistent. An input port is consistent if
# it consumes streams from other operators in the same consistent region.
# @param $index input port index
# @return true if the input port is consistent, false otherwise
sub isConsistentInputPort
{
  my ($self, $index) = @_;
  return $self->{consistentIPByIndex_}->[$index];
}

## @method \@InputPort getInputPorts()
# Gets a list of all consistent input ports.
# @return list of consistent input ports
sub getInputPorts
{
  my ($self) = @_;
  return $self->{consistentIPArray_};
}

# DOXYGEN_SKIP_FOR_USERS

## @method bool isInRegion()
# @return true if the operator instance is in a consistent region, false otherwise.
sub isInRegion
{
  my ($self) = @_;
  return $self->{isInRegion_};
}

sub new
{
  my ($class, $opContext) = @_;

  my $self = {
    isInRegion_ => 0,
    isStartOfRegion_ => 0,
    isEndOfRegion_ => 0,
    isTriggerOperator_ => 0,
    consistentIPArray_    => [],
    consistentIPByIndex_ => [],
    autonomous_      => []
  };

  foreach my $annot (@{$opContext->getAnnotations()}) {
    if ($annot->getName() eq "consistentRegionEntry") {

      $self->{isInRegion_} = 1;

      my $value = $annot->getValue();
      foreach my $attribute (@{$annot->getValue()->getAttributes()}) {
        if ($attribute->getName() eq "isStartOfRegion") {
          if ($attribute->getValue() eq "true") {
            $self->{isStartOfRegion_} = 1;
          }
        }

        if ($attribute->getName() eq "isEndOfRegion") {
          if ($attribute->getValue() eq "true") {
            $self->{isEndOfRegion_} = 1;
          }
        }

        if ($attribute->getName() eq "isTriggerOperator") {
          if ($attribute->getValue() eq "true") {
            $self->{isTriggerOperator_} = 1;
          }
        }

        if ($attribute->getName() eq "autonomousInputPort") {
          push @{$self->{autonomous_}}, $attribute->getValue();
        }
      }
    }
  }

  return bless($self, $class);
}

sub setInputPorts
{
  my ($self, $inputPortsRef) = @_;

  # By default, all ports are not consistent
  my @inputPorts = @{$inputPortsRef};
  if (!$self->{isInRegion_} || $self->{isStartOfRegion_}) {
    my $i = 0;
    foreach (@inputPorts) {
      push @{$self->{consistentIPByIndex_}}, 0;
      $i++;
    }
  } else {
    # All are consistent, except the explicitly annotated ones
    my $i = 0;
    foreach (@inputPorts) {
      push @{$self->{consistentIPByIndex_}}, 1;
      $i++;
    }
    foreach my $autonomousPortIndex (@{$self->{autonomous_}}) {
      $self->{consistentIPByIndex_}->[$autonomousPortIndex] = 0;
    }
  }

  foreach my $iPort (@inputPorts) {
    if ($self->isConsistentInputPort($iPort->getIndex())) {
      push @{$self->{consistentIPArray_}}, $iPort;
    }
  }
}

1;
