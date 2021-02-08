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

## @class SPL::Operator::Instance::OutputPort
# \brief Class that represents an output port of an operator instance.
#
# Important objects in an OutputPort:
# - The C++ Tuple Type
# - The C++ Tuple Name
# - The attributes in the tuple (can return by index or name)
# - The Window (if any) for the OutputPort
package SPL::Operator::Instance::OutputPort;

use strict;
use warnings;
use XML::Simple;
$XML::Simple::PREFERRED_PARSER = "XML::Parser";

use SPL::Operator::Instance::OutputAttribute;
use SPL::Operator::Instance::SourceLocation;
use SPL::Operator::Instance::Window;
use SPL::ModelHelper;

## @method SourceLocation getSourceLocation()
# Get the %SPL source location that corresponds to this output port
# @return the SourceLocation object
sub getSourceLocation
{
  my ($self) = @_;
  return $self->{loc_};
}

## @method unsigned getIndex()
# Get the zero-based index of the output port
# @return the index of this port
sub getIndex
{
  my ($self) = @_;
  return $self->{xml_}->{index}->[0];
}

## @method string getCppTupleName()
# Get the name used for the tuples from this port.
# @return a string containing the C++ name of the tuple
#
# Example use of %getCppTupleName for an output port
# @code
# <%
# my $outputPort = $model->getOutputPortAt(0);
# my $outTupleName = $outputPort->getCppTupleName();
# %>
# ...
# void MY_OPERATOR::process(Punctuation const & punct, uint32_t port)
# {
#    // Send a known tuple to port 2
#    OPort0Type <%=$outTupleName%> (5, 6, "hi");
#    submit (<%=$outTupleName%>, 2);
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
# The name OPortXType where X is the port number is a typedef to the C++ tuple type for port X
#
# Example use of %getCppTupleType for an output port
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
# Example use of %getSPLTupleType for an output port
# @code
# <%
# my $port0 = $model->getOutputPortAt(0);
# my $port1 = $model->getOutputPortAt(0);
# if ($port0->getCppTupleName() ne $port1->getCppTupleName()) {
#     # Error: output ports have different types
#     my $port0Type = $port0->getSPLTupleType();
#     my $port1Type = $port1->getSPLTupleType();
#     SPL::CodeGen::exitln ("Output ports 0 and 1 must have the same type.  Actual types are '$port1Type' and '$port1Type'", $port0->getSourceLocation());
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

## @method OutputAttribute getAttributeAt($index)
# Get the OutputAttribute object for the given index.
# @param index positional order of the desired OutputAttribute
# @return the OutputAttribute object at the given index
sub getAttributeAt
{
  my ($self, $index) = @_;
  return $self->{attributes_}->[$index];
}

## @method OutputAttribute getAttributeByName($name)
# Get the OutputAttribute object for the given name.
# @param name the name of the attribute
# @return the OutputAttribute object for the given name
sub getAttributeByName
{
  my ($self, $name) = @_;
  return $self->{attributeMap_}->{$name};
}

## @method \@OutputAttribute getAttributes
# Get the list of OutputAttribute objects associated with this output port.
# @return a list of OutputAttribute objects
sub getAttributes
{
  my ($self) = @_;
  return $self->{attributes_};
}

## @method string getSchema($rootName)
# Get the schema for the tuple associated with this output port.
# @param rootName top level XML name
# @return the XML corresponding to the tuple definition for this output port
sub getSchema($$)
{
  my ($self, $rootName) = @_;
  if ($self->{xml_}->{tuple}) {
      my $xml = XML::Simple->new (ForceArray => 1, KeepRoot => 0, RootName => $rootName, keyattr => []);
      return $xml->XMLout ($self->{xml_}->{tuple}->[0]);
  }
  return undef;

}

# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  my $self = {
    xml_                     => $xml,
    attributes_              => [],
    attributeMap_            => {},
    window_                  => undef,
    # For all all output ports specified, the hash holds an integer
    # that represents which port in a set this one is.  Note that they use a 1-based
    # index. For example, if there are three output ports specified, 1, 3, and 5, the
    # mapping will be 1 ==> 1, 3 ==> 2, 5 ==> 3
    finalPunctuationPortScopes_ => {},
    loc_ => undef
};

  if($self->{xml_}->{attributes}->[0]->{attribute}){
    foreach my $aelem (@{$self->{xml_}->{attributes}->[0]->{attribute}}){
      my $attrb = SPL::Operator::Instance::OutputAttribute->new($aelem, $context);
      push @{$self->{attributes_}}, $attrb;
      $self->{attributeMap_}->{$attrb->getName()} = $attrb;
    }
  }

  if($self->{xml_}->{window}) {
    $self->{window_} = SPL::Operator::Instance::Window->new($self->{xml_}->{window}->[0]);
  }

  if ($self->{xml_}->{finalPunctuationPortScope}) {
    my $i = 1;
    foreach my $elem (@{$self->{xml_}->{finalPunctuationPortScope}->[0]->{port}}) {
      $self->{finalPunctuationPortScopes_}->{$elem} = $i;
      ++$i;
    }
  }

  $self->{loc_} = SPL::Operator::Instance::SourceLocation->new($self->{xml_}->{sourceLocation}->[0]);

  return bless($self, $class);
}

sub getNumberOfFinalPunctuationPorts {
  my ($self) = @_;
  return keys(%{$self->{finalPunctuationPortScopes_}});
}

sub getFinalPunctuationPortScopes {
  my ($self) = @_;
  return $self->{finalPunctuationPortScopes_};
}


1;
