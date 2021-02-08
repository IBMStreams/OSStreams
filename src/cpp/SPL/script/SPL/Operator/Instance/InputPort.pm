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

## @class SPL::Operator::Instance::InputPort
# \brief Class that represents an input port of an operator instance.
#
# Important objects in an InputPort:
# - The C++ Tuple Type
# - The C++ Tuple Name
# - The attributes in the tuple (can return by index or name)
# - The Window (if any) for the InputPort
package SPL::Operator::Instance::InputPort;

use strict;
use warnings;
use XML::Simple;
$XML::Simple::PREFERRED_PARSER = "XML::Parser";

use SPL::Operator::Instance::InputAttribute;
use SPL::Operator::Instance::SourceLocation;
use SPL::Operator::Instance::Window;
use SPL::ModelHelper;

## @method SourceLocation getSourceLocation()
# Get the %SPL source location that corresponds to this input port
# @return the SourceLocation object
sub getSourceLocation
{
  my ($self) = @_;
  return $self->{loc_};
}

## @method unsigned getIndex()
# Get the zero-based index of the input port
# @return the index of this port
sub getIndex
{
  my ($self) = @_;
  return $self->{xml_}->{index}->[0];
}

## @method string getCppTupleName()
# Get the name used for the tuples from this port. This name is used in
# C++ expressions obtained from parameters and output assignments.
# @return a string containing the C++ name of the tuple
#
# Example use of %getCppTupleName for an input port
# @code
# <%
# my $inputPort = $model->getInputPortAt(0);
# my $inTupleName = $inputPort->getCppTupleName();
# %>
# ...
# void MY_OPERATOR::process(Tuple const & tuple, uint32_t port)
# {
#    IPort0Type const & <%=$inTupleName%> = static_cast<IPort0Type const&>(tuple);
#    // C++ expressions referencing input attributes will reference $inTupleName
#    ...
# }
# @endcode
sub getCppTupleName
{
  my ($self) = @_;
  return $self->{xml_}->{tupleCppName}->[0];
}

## @method string getCppTupleType()
# Get the tuple type for this port. This type could be used in the generated
# C++ code to declare new tuples.
# @return a string containing the C++ representation of the tuple type
#
# The name IPortXType where X is the port number is a typedef to the C++ tuple type for port X
#
# Example use of %getCppTupleType for an input port
# @code
# <%
# # check that the input port schema matches the output port schema
# my $inputPort = $model->getInputPortAt(0);
# my $outputPort = $model->getOutputPortAt(0);
# if ($inputPort->getCppTupleType() ne $outputPort->getCppTupleType()) {
#     exitln ("Input and Output ports must have same schema", $inputPort->getSourceLocation());
# }
# %>
# @endcode
sub getCppTupleType
{
  my ($self) = @_;
  return $self->{xml_}->{tupleCppType}->[0];
}

## @method string getSPLTupleType()
# Get the %SPL tuple type for this port.
# @return a string containing the %SPL representation of the tuple type
#
# Example use of %getSPLTupleType for an input port
# @code
# <%
# my $port0 = $model->getInputPortAt(0);
# my $port1 = $model->getInputPortAt(0);
# if ($port0->getCppTupleType() ne $port1->getCppTupleType()) {
#     # Error: input ports have different types
#     my $port0Type = $port0->getSPLTupleType();
#     my $port1Type = $port1->getSPLTupleType();
#     SPL::CodeGen::exitln ("Input ports 0 and 1 must have the same type.  Actual types are '$port1Type' and '$port1Type'", $port0->getSourceLocation());
# }
# %>
# @endcode
sub getSPLTupleType
{
  my ($self) = @_;
  return SPL::ModelHelper::cleanType($self->{xml_}->{tupleSPLType}->[0]);
}

## @method unsigned getMutationAllowed()
# Get the \c mutationAllowed attribute for this port, as represented in the
# operator model for this operator instance.
# @return 1 if mutation is allowed, 0 otherwise
sub getMutationAllowed
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest($self->{xml_}->{mutationAllowed}->[0]);
}

## @method unsigned getFacade()
# Get the facade status for the tuples on this port.
# @return 1 if the tuples on this port are implemented as facade tuples, 0 otherwise
sub getFacade
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest($self->{xml_}->{facade}->[0]);
}

## @method unsigned getNumberOfAttributes()
# Get the number of attributes carried by tuples from this port.
# @return the number of attributes
sub getNumberOfAttributes
{
  my ($self) = @_;
  return scalar(@{$self->{attributes_}});
}

## @method InputAttribute getAttributeAt($index)
# Get the InputAttribute object for the given index.
# @param index positional order of the desired InputAttribute
# @return the InputAttribute object at the given index
sub getAttributeAt
{
  my ($self, $index) = @_;
  return $self->{attributes_}->[$index];
}

## @method InputAttribute getAttributeByName($name)
# Get the InputAttribute object for the given name.
# @param name the name of the attribute
# @return the InputAttribute object for the given name
sub getAttributeByName
{
  my ($self, $name) = @_;
  return $self->{attributeMap_}->{$name};
}

## @method \@InputAttribute getAttributes
# Get the list of InputAttribute objects associated with this input port.
# @return a list of InputAttribute objects
sub getAttributes
{
  my ($self) = @_;
  return $self->{attributes_};
}

## @method unsigned hasTupleLogic()
# Check if this input port has tuple logic defined.
# @return 1 if this input port has tuple logic, 0 otherwise
sub hasTupleLogic
{
  my ($self) = @_;
  return 1
    if($self->{tupleLogic_});
  return 0;
}

## @method unsigned hasPunctLogic()
# Check if this input port has punctuation logic defined.
# @return 1 if this input port has punctuation logic, 0 otherwise
sub hasPunctLogic
{
  my ($self) = @_;
  return 1
    if($self->{punctLogic_});
  return 0;
}

# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
# # @method unsigned hasWatermarkLogic()
# Check if this input port has watermark logic defined.
# @return 1 if this input port has watermark logic, 0 otherwise

# sub hasWatermarkLogic
# {
#   my ($self) = @_;
#   return 1
#     if($self->{watermarkLogic_});
#   return 0;
# }

## @method unsigned hasHistory()
# Check if this input port has history.
# @return 1 if this input port has history, 0 otherwise
sub hasHistory
{
  my ($self) = @_;
  return 1
    if($self->{xml_}->{history});
  return 0;
}

## @method unsigned getHistory()
# Get the largest history access index for this port
# @pre hasHistory() == 1
# @return the largest history access index
sub getHistory
{
  my ($self) = @_;
  return $self->{xml_}->{history}->[0];
}

## @method unsigned hasWindow()
# Check if this input port has a window defined on it.
# @return 1 if this input port has a window, 0 otherwise
sub hasWindow
{
  my ($self) = @_;
  return 1
    if($self->{window_});
  return 0;
}

## @method Window getWindow()
# Get the window associated with this input port.
# @pre hasWindow() == 1
# @return the Window object
sub getWindow
{
  my ($self) = @_;
  return $self->{window_};
}

## @method string getSchema($rootName)
# Get the schema for the tuple associated with this input port.
# @param rootName top level XML name
# @return the XML corresponding to the tuple definition for this input port
sub getSchema($$)
{
  my ($self, $rootName) = @_;
  if ($self->{xml_}->{tuple}) {
      my $xml = XML::Simple->new (ForceArray => 1, KeepRoot => 0, RootName => $rootName, keyattr => []);
      return $xml->XMLout ($self->{xml_}->{tuple}->[0]);
  }
  return undef;

}

## @method unsigned isControl()
# Determine if this input port is a control port
# @return Returns 1 if this input port is a control port, otherwise false
sub isControl()
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest($self->{xml_}->{isControlPort});
}


# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $opContext) = @_;

  my $self = {
    xml_                => $xml,
    attributes_         => [],
    attributeMap_       => {},
    tupleLogic_         => undef,
    punctLogic_         => undef,
# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
#    watermarkLogic_     => undef,
    window_             => undef,
    loc_                => undef,
    generateTupleLogic_ => 1
  };

  if($self->{xml_}->{attributes}->[0]->{attribute}){
    foreach my $aelem (@{$self->{xml_}->{attributes}->[0]->{attribute}}){
      my $attrb = SPL::Operator::Instance::InputAttribute->new($aelem);
      push @{$self->{attributes_}}, $attrb;
      $self->{attributeMap_}->{$attrb->getName()} = $attrb;
    }
  }

  if($self->{xml_}->{tupleLogic}) {
    $self->{tupleLogic_} = $self->{xml_}->{tupleLogic}->[0];
  }

  if($self->{xml_}->{punctLogic}) {
    $self->{punctLogic_} = $self->{xml_}->{punctLogic}->[0];
  }

# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
  # if($self->{xml_}->{watermarkLogic}) {
  #   $self->{watermarkLogic_} = $self->{xml_}->{watermarkLogic}->[0];
  # }

  if($self->{xml_}->{window}) {
    $self->{window_} = SPL::Operator::Instance::Window->new($self->{xml_}->{window}->[0],
                                                            $self->{xml_}->{index}->[0]);
  }

  $self->{loc_} = SPL::Operator::Instance::SourceLocation->new($self->{xml_}->{sourceLocation}->[0]);

  return bless($self, $class);
}

sub suppressTupleLogicGeneration {
  my ($self) = @_;
  $self->{generateTupleLogic_} = 0;
}

sub generateTupleLogic {
  my ($self) = @_;
  return $self->{generateTupleLogic_};
}

sub getTupleLogic {
  my ($self) = @_;
  return $self->{tupleLogic_};
}

sub getPunctLogic {
  my ($self) = @_;
  return $self->{punctLogic_};
}

# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
# sub getWatermarkLogic {
#   my ($self) = @_;
#   return $self->{watermarkLogic_};
# }

1;
