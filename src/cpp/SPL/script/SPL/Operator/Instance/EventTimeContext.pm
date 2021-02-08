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

## @class SPL::Operator::Instance::EventTimeContext
# \brief Class that represents the context of the operator regarding event-time. The
# EventTimeContext is an OptionalContext that can be obtained with the "EventTime"
# string.
package SPL::Operator::Instance::EventTimeContext;

use strict;
use warnings;
use SPL::Operator::Instance::OptionalContext;

use constant NAME => "EventTime";

our @ISA = qw(SPL::Operator::Instance::OptionalContext);


## @method bool isInRegion()
# Check if the operator is within an event-time enabled graph
# @return true if the operator is within an event-time enabled graph, false otherwise.
sub isInRegion
{
  my ($self) = @_;
  defined($self->{attribute_});
}

## @method string getAttribute()
# Gets the name of the event-time attribute.
# @return name of the event-time attribute
sub getAttribute
{
  my ($self) = @_;
  $self->{attribute_};
}

## @method string getResolution()
# Gets the name of the event-time attribute resolution.
# @return the resolution
sub getResolution
{
  my ($self) = @_;
  $self->{resolution_};
}

## @method string getType()
# Gets the type of the event-time attribute
# @return the event-time attribute type
sub getType
{
  my ($self) = @_;
  $self->{type_};
}

# DOXYGEN_SKIP_FOR_USERS

## @method getInputConnections()
# Gets a list of all event-time input connections.
# @return list of event-time input connections
sub getInputConnections
{
  my ($self) = @_;
  $self->{inputConnections_};
}

## @method \@OutputPort getOutputPorts()
# Gets a list of all event-time output ports
# @return list of event-time output ports
sub getOutputPorts
{
  my ($self) = @_;
  $self->{outputPorts_};
}

sub new
{
  my ($class, $opContext) = @_;

  my $self = {
    attribute_ => undef,
    resolution_ => undef,
    type_ => undef,
    inputConnections_ => [],
    outputPorts_ => []
  };

  foreach my $annot (@{$opContext->getAnnotations()}) {
    if ($annot->getName() eq "eventTimeContext") {
      my $value = $annot->getValue();

      foreach my $attribute (@{$annot->getValue()->getAttributes()}) {
        if ($attribute->getName() eq "attribute") {
            $self->{attribute_} = $attribute->getValue();
        }
        if ($attribute->getName() eq "resolution") {
          $self->{resolution_} = $attribute->getValue();
        }
        if ($attribute->getName() eq "type") {
          $self->{type_} = $attribute->getValue();
        }
        if ($attribute->getName() eq "inputConnections") {
          # value is comma-separated list beginning and ending with
          # square brackets.  Remove the brackets.
          push @{$self->{inputConnections_}}, substr($attribute->getValue(), 1, -1);
        }
        if ($attribute->getName() eq "outputPorts") {
          push @{$self->{outputPorts_}}, $attribute->getValue();
        }
      }
    }
  }

  bless($self, $class);
}

1;
