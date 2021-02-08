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

## @namespace SPL::Operator
# \brief Namespace for classes that provide access to operator level
# information.
#
# @namespace SPL::Operator::Instance
# \brief Namespace for classes that provide access to operator instance level
# information.

## @class SPL::Operator::Instance::OperatorInstance
# \brief Class that represents the instance of an operator.
#
# This class provides methods to access all the information
# necessary to generate code for the given operator.
#
# For a given %Operator %Instance, you can access the
# @link SPL::Operator::Instance::Context Context@endlink,
# the @link SPL::Operator::Instance::Parameter Parameters@endlink,
# the @link SPL::Operator::Instance::InputPort Input Ports@endlink,
# and the @link SPL::Operator::Instance::OutputPort Output Ports@endlink.
package SPL::Operator::Instance::OperatorInstance;

use strict;
use warnings;
use XML::Simple;
$XML::Simple::PREFERRED_PARSER = "XML::Parser";

use SPL::Operator::Instance::Context;
use SPL::Operator::Instance::Parameter;
use SPL::Operator::Instance::Expression;
use SPL::Operator::Instance::InputAttribute;
use SPL::Operator::Instance::InputPort;
use SPL::Operator::Instance::Window;
use SPL::Operator::Instance::OutputAttribute;
use SPL::Operator::Instance::OutputPort;
use IO::File;

## @method Context getContext()
# Get the Context object
# @return the Context object for this operator instance
sub getContext
{
  my ($self) = @_;
  return $self->{context_};
}

## @method unsigned getNumberOfParameters()
# Get the number of parameters associated with this operator instance.
# @return the number of parameters
sub getNumberOfParameters
{
  my ($self) = @_;
  return (defined $self->{parameters_}) ? scalar(@{$self->{parameters_}}) : 0;
}

## @method Parameter getParameterAt($index)
# Get the Parameter object at a given index
# @param index the index of the desired parameter
# @return the desired Parameter object
sub getParameterAt
{
  my ($self, $index) = @_;
  return $self->{parameters_}->[$index];
}

## @method \@Parameter getParameters()
# Get the set of parameters.
# @return a possibly empty list of Parameter objects
sub getParameters
{
  my ($self) = @_;
  return $self->{parameters_};
}

## @method Parameter getParameterByName($name)
# Get the parameter with the given name.
# @param name the name of the desired parameter
# @return the desired Parameter object
sub getParameterByName
{
  my ($self, $name) = @_;
  return $self->{parametersMap_}->{$name};
}

## @method unsigned getNumberOfInputPorts()
# Get the number of input ports defined for this operator
# @return the number of input ports
sub getNumberOfInputPorts
{
  my ($self) = @_;
  return scalar(@{$self->{inputPorts_}});
}

## @method InputPort getInputPortAt($index)
# Get the input port at a given index.
# @param index the index of the desired input port
# @return the desired InputPort object
sub getInputPortAt
{
  my ($self, $index) = @_;
  return $self->{inputPorts_}->[$index];
}

## @method \@InputPort getInputPorts()
# Get the list of input ports.
# @return the list of InputPort objects for this operator instance
sub getInputPorts
{
  my ($self) = @_;
  return $self->{inputPorts_};
}

## @method unsigned getNumberOfOutputPorts()
# Get the number of output ports defined for this operator.
# @return the number of output ports
sub getNumberOfOutputPorts
{
  my ($self) = @_;
  return scalar(@{$self->{outputPorts_}});
}

## @method OutputPort getOutputPortAt($index)
# Get the output port for the given index.
# @param index the index of the desired output port
# @return the desired OutputPort object
sub getOutputPortAt
{
  # add some check here
  my ($self, $index) = @_;
  return $self->{outputPorts_}->[$index];
}

## @method \@OutputPort getOutputPorts()
# Get the list of output ports associated with this opertor instance.
# @return the list of OutputPort objects
sub getOutputPorts
{
  my ($self) = @_;
  return $self->{outputPorts_};
}

# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $model) = @_;

  my $self = {
    xml_              =>  undef,
    context_          =>  undef,
    parameters_       =>  [],
    parametersMap_    =>  {},
    inputPorts_       =>  [],
    outputPorts_      =>  []
  };

  my $xml = XML::Simple->new (ForceArray => 1, KeepRoot => 0, keyattr => []);
  my $fh = IO::File->new($model);
  $self->{xml_} = $xml->XMLin($fh);
  $fh->close();

  $self->{context_} = SPL::Operator::Instance::Context->new($self->{xml_}->{context}->[0]);

  # extract parameters
  if ($self->{xml_}->{parameters}->[0]->{parameter}) {
    foreach my $pelem (@{$self->{xml_}->{parameters}->[0]->{parameter}}) {
       my $param = SPL::Operator::Instance::Parameter->new($pelem,$self->{context_});
       push @{$self->{parameters_}}, $param;
       $self->{parametersMap_}->{$param->getName()} = $param;
    }
  }

  # extract input port information
  if ($self->{xml_}->{inputPorts}->[0]->{inputPort}){
    foreach my $ielem (@{$self->{xml_}->{inputPorts}->[0]->{inputPort}}) {
      my $iport = SPL::Operator::Instance::InputPort->new($ielem,$self->{context_});
      push @{$self->{inputPorts_}}, $iport;
    }
  }

  # extract output port information
  if ($self->{xml_}->{outputPorts}->[0]->{outputPort}){
    foreach my $oelem (@{$self->{xml_}->{outputPorts}->[0]->{outputPort}}) {
      my $oport = SPL::Operator::Instance::OutputPort->new($oelem,$self->{context_});
      push @{$self->{outputPorts_}}, $oport;
    }
  }

  # pass input port information to the Context
  $self->{context_}->setInputPorts(\@{$self->{inputPorts_}});

  bless($self, $class);

  return $self;
}

1;
