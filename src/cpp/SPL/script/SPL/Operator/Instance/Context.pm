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

## @class SPL::Operator::Instance::Context
# \brief Class that represents the context of an operator instance.
package SPL::Operator::Instance::Context;

use strict;
use warnings;
use File::Spec;

use SPL::ModelHelper;
use SPL::Operator::Instance::Annotation;
use SPL::Operator::Instance::CatchContext;
use SPL::Operator::Instance::ConsistentRegionContext;
use SPL::Operator::Instance::EventTimeContext;
use SPL::Operator::Instance::RuntimeConstant;
use SPL::Operator::Instance::SourceLocation;
use SPL::Operator::Instance::StateVariable;
use SPL::Operator::Instance::Toolkit;

## @method string getProductVersion()
# Get the version of the product that was used to generate this operator instance.
# @return string with the version; the format of the version string is
# <tt>number [ '.' number [ '.' number [ '.' alphaNum ] ] ]</tt>.
sub getProductVersion
{
  my ($self) = @_;
  return $self->{xml_}->{productVersion}->[0];
}

## @method string getCheckpointingKind()
# Get the checkpoint kind that this operator is using
# @return string with value one of <tt>{ none, periodic, operatorDriven }</tt>
sub getCheckpointingKind
{
  my ($self) = @_;
  return $self->{xml_}->{checkpointKind}->[0];
}

## @method bool isCheckpointingEnabled()
# @return true if operator instance configures the checkpointing option, and false if the
# operator instance does not configure the checkpointing option
sub isCheckpointingEnabled()
{
  my ($self) = @_;
  return $self->{xml_}->{checkpointKind}->[0] ne 'none';
}

## @method SourceLocation getSourceLocation()
# Get the %SPL source location that corresponds to this operator instance.
# @return The SourceLocation object
sub getSourceLocation
{
  my ($self) = @_;
  return $self->{loc_};
}

## @method string getKind()
# Get the operator kind (e.g., \c spl.relational.Join)
# @return a string identifying the kind of operator this operator instance
# represents
sub getKind
{
  my ($self) = @_;
  return $self->{xml_}->{kind}->[0];
}

## @method string getClass()
# Get the C++ class of the operator (e.g., Bidder::MatchedBids)
# @return Returns a string identifying the name of the C++ class that implements this operator.
sub getClass
{
  my ($self) = @_;
  return $self->{xml_}->{class}->[0];
}

## @method string getOperatorDirectory()
# Get the directory where the operator model XML is located. This is an absolute path.
# @return a string giving the absolute path to the operator model directory
sub getOperatorDirectory
{
  my ($self) = @_;
  my $path = $self->{xml_}->{operatorModelDirectory}->[0];
  $path = File::Spec->catfile ($self->getApplicationDirectory(), $path)
      unless File::Spec->file_name_is_absolute($path);
  return $path;
}

## @method string getApplicationDirectory()
# Get the directory where the %SPL application is located. This is an absolute path.
# @return a string giving the absolute path to the SPL application directory
sub getApplicationDirectory
{
  my ($self) = @_;
  return $self->{xml_}->{applicationDirectory}->[0];
}

## @method string getDataDirectory()
# Get the directory where the data is located. This is an absolute path.
# @return a string giving the absolute path to the data directory
sub getDataDirectory
{
  my ($self) = @_;
  return $self->{xml_}->{dataDirectory}->[0];
}

## @method string getOutputDirectory()
# Get the directory where the output for the %SPL compilation is located (where
# the ADL file sits). This is an absolute path.
# @return a string giving the absolute path to the SPL compiler output directory
sub getOutputDirectory
{
  my ($self) = @_;
  return $self->{xml_}->{outputDirectory}->[0];
}

## @method unsigned isOptimizedModeOn()
# Check if the compiler is in optimized mode or not.
# @return 1 if the \c -a or \c --optimized-code-generation compiler option is
# specified, otherwise it returns 0
sub isOptimizedModeOn
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest(
      $self->{xml_}->{optimized}->[0]);
}

## @method unsigned isVerboseModeOn()
# Check if the compiler is in verbose mode or not. If in verbose mode,
# additional code-generation debug messages can be printed and are visible
# during the compilation process.
# @return 1 if the \c -v or \c --verbose-mode compiler option is specified,
# otherwise it returns 0
sub isVerboseModeOn
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest(
      $self->{xml_}->{verbose}->[0]);
}

## @method unsigned isCodeBeautificationOn()
# Check if code reformatting is on
# @return 1 if the \c -z or \c --code-beautification compiler option is
# specified, otherwise it returns 0
sub isCodeBeautificationOn
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest(
      $self->{xml_}->{codeBeautification}->[0]);
}

## @method \@StateVariable getStateVariables()
# Get the list of state variables
# @return a possibly empty list of StateVariable objects
sub getStateVariables
{
  my ($self) = @_;
  return $self->{stateVariables_};
}

## @method unsigned getNumberOfStateVariables()
# Get the number of state variables
# @return the number of state variables in this operator
sub getNumberOfStateVariables
{
  my ($self) = @_;
  return (defined $self->{stateVariables_}) ?
          scalar(@{$self->{stateVariables_}}) : 0;
}

## @method StateVariable getStateVariableAt($index)
# Get the state variable at a given index
# @return the StateVariable object at the given index
sub getStateVariableAt
{
  # add some check here
  my ($self, $index) = @_;
  return $self->{stateVariables_}->[$index];
}

## @method OptionalContext getOptionalContext($name)
# Get a blessed reference of an optional context associated to the provided
# string. If the optional context does not exist, a undefined reference
# is returned. When in a consistent region, an operator can obtain a
# ConsistentRegionContext using the "ConsistentRegion" string.
# @param name optional context name.
# @return a blessed reference of an available optional context or an undefined
# reference
sub getOptionalContext
{
  my ($self, $name) = @_;

  if (exists($self->{optContext_}->{$name})) {
    return $self->{optContext_}->{$name};
  }
  return undef;
}

## @method string getToolkitDirectory()
# Get the compile-time location of the toolkit containing the operator making this call
# @return Returns the absolute path to the toolkit directory
sub getToolkitDirectory
{
  my ($self) = @_;
  foreach my $tk (@{$self->{toolkits_}}) {
    if ($tk->getIsCurrentToolkit()) {
       return $tk->getRootDirectory();
    }
  }
  return undef;
}

# DOXYGEN_SKIP_FOR_USERS

## @method unsigned getNumberOfAnnotations()
# Get the number of annotations
# @return Returns the number of annotations
sub getNumberOfAnnotations
{
  my ($self) = @_;
  return scalar @{$self->{annotations_}};
}

## @method \@Annotation getAnnotations()
# Get the list of annotations.  The list may be empty.
# @return Returns a reference to a list of annotations
sub getAnnotations
{
  my ($self) = @_;
  return $self->{annotations_};
}

## @method \@Toolkit getToolkits()
# Get the list of toolkits.  The list may be empty.
# @return Returns a reference to a list of toolkits.
sub getToolkits
{
  my ($self) = @_;
  return $self->{toolkits_};
}

sub new {
  my ($class,$xml) = @_;

  my $self = {
    xml_ => $xml,
    includes_ => [],
    stateVariables_ => [],
    runtimeConstants_ => [],
    loc_ => undef,
    types_ => [],
    process_ => undef,
# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
#    watermark_ => undef,
    annotations_ => [],
    cContext_ => undef,
    toolkits_ => [],
  };

  foreach my $include (@{$self->{xml_}->{includes}->[0]->{include}}) {
      push @{$self->{includes_}}, $include;
  }

  foreach my $selem (@{$self->{xml_}->{stateVariables}->[0]->{stateVariable}}) {
      my $svar = SPL::Operator::Instance::StateVariable->new($selem);
      push @{$self->{stateVariables_}}, $svar;
  }

  foreach my $relem (@{$self->{xml_}->{runtimeConstants}->[0]->{runtimeConstant}}) {
      my $rconst = SPL::Operator::Instance::RuntimeConstant->new($relem);
      push @{$self->{runtimeConstants_}}, $rconst;
  }

  foreach my $telem (@{$self->{xml_}->{types}->[0]->{type}}) {
      $self->{types_}[$telem->{index}] = $telem->{type};
  }

  foreach my $annot (@{$xml->{annotations}->[0]->{annotation}}) {
    push @{$self->{annotations_}}, SPL::Operator::Instance::Annotation->new($annot);
  }

  foreach my $toolkit (@{$xml->{toolkits}->[0]->{toolkit}}) {
      push @{$self->{toolkits_}}, SPL::Operator::Instance::Toolkit->new($toolkit);
  }

  my $blessedSelf = bless($self, $class);
  my $cContext = SPL::Operator::Instance::ConsistentRegionContext->new($blessedSelf);
  if ($cContext->isInRegion()) {
    $self->{optContext_}->{"ConsistentRegion"} = $cContext;
  }

  my $catchContext = SPL::Operator::Instance::CatchContext->new($blessedSelf);
  if ($catchContext->mustCatchExceptions()) {
    $self->{optContext_}->{"Catch"} = $catchContext;
  }

  my $eventTimeContext = SPL::Operator::Instance::EventTimeContext->new($blessedSelf);
  if ($eventTimeContext->isInRegion()) {
    $self->{optContext_}->{SPL::Operator::Instance::EventTimeContext::NAME} = $eventTimeContext;
  }

  $self->{loc_} = SPL::Operator::Instance::SourceLocation->new($self->{xml_}->{sourceLocation}->[0]);
  $self->{process_} = $self->{xml_}->{processLogic}->[0];
# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
#  $self->{watermark_} = $self->{xml_}->{watermarkLogic}->[0];

  # set global verboseMode
  $SPL::CodeGen::verboseMode = isVerboseModeOn ($self);
  return $blessedSelf;
}

sub getIncludes {
  my ($self) = @_;
  return $self->{includes_};
}

sub getNumberOfIncludes {
  my ($self) = @_;
  return scalar(@{$self->{includes_}});
}

sub getType {
  my ($self, $index) = @_;
  return $self->{types_}[$index];
}

sub getIncludeAt {
  # add some check here
  my ($self, $index) = @_;
  return $self->{includes_}->[$index];
}

sub getRuntimeConstants {
  my ($self) = @_;
  return $self->{runtimeConstants_};
}
sub getNumberOfRuntimeConstants {
  my ($self) = @_;
  return (defined $self->{runtimeConstants_}) ?
          scalar(@{$self->{runtimeConstants_}}) : 0;
}
sub getRuntimeConstantAt {
  my ($self, $index) = @_;
  return $self->{runtimeConstants_}->[$index];
}
sub getProcessLogic {
  my ($self) = @_;
  return $self->{process_};
}
# // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
# sub getWatermarkLogic {
#   my ($self) = @_;
#   return $self->{watermark_};
# }
sub getJavaOperatorDirectory
{
  my ($self) = @_;
  return undef if !$self->{xml_}->{javaOperatorModelDirectory};
  my $path = $self->{xml_}->{javaOperatorModelDirectory}->[0];
  $path = File::Spec->catfile ($self->getApplicationDirectory(), $path)
      unless File::Spec->file_name_is_absolute($path);
  return $path;
}

sub setInputPorts {
  my ($self, $inputPortsRef) = @_;

  if ($self->{optContext_}->{"ConsistentRegion"}) {
    $self->{optContext_}->{"ConsistentRegion"}->setInputPorts($inputPortsRef);
  }
}

1;
