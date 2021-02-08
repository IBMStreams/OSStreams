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

# DOXIGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::CatchContext
# \brief Class that represents the context of the operator regarding a catch annotation. The
# CatchContext is an OptionalContext that can be obtained with the "Catch"
# string.
package SPL::Operator::Instance::CatchContext;

use strict;
use warnings;
use SPL::Operator::Instance::OptionalContext;

our @ISA = qw(SPL::Operator::Instance::OptionalContext);


## @method bool mustCatchException()
# Check if the operator must catch exceptions
# @return true if the operator must catch exceptions, false otherwise.
sub mustCatchExceptions()
{
  my ($self) = @_;
  return $self->{mustCatchException_};
}

## @method bool mustCatchStreamsExceptions()
# Check if the operator must catch Streams Exceptions only.
# @return true if the operator must catch only Streams exceptions, false otherwise.
sub mustCatchStreamsExceptions
{
  my ($self) = @_;
  if ($self->{exceptionKind_} eq "streams") {
    return 1;
  }
  return 0;
}

## @method bool mustCatchStandardExceptions()
# Check if the operator must catch Standard Exceptions, which also includes Streams exceptions.
# @return true if the operator must catch std exceptions, false otherwise.
sub mustCatchStandardExceptions
{
  my ($self) = @_;
  if ($self->{exceptionKind_} eq "std") {
    return 1;
  }
  return 0;
}

## @method bool mustCatchAllExceptions()
# Check if the operator must catch all Exceptions, which also includes standard exceptions.
# @return true if the operator must catch all exceptions, false otherwise.
sub mustCatchAllExceptions
{
  my ($self) = @_;
  if ($self->{exceptionKind_} eq "all") {
    return 1;
  }
  return 0;
}

## @method bool mustTraceTuples()
# Check if the operator must trace tuples that caused an exception to be triggered.
# @return true if tuples must be traced, false otherwise
sub mustTraceTuples
{
  my ($self) = @_;
  return $self->{tupleTrace_};
}

## @method bool mustPrintStackTrace()
# Check if the operator must print the stack trace that caused an exception to be triggered.
# @return true if must print the stack trace, false otherwise
sub mustPrintStackTrace
{
  my ($self) = @_;
  return $self->{stackTrace_};
}

sub new
{
  my ($class, $opContext) = @_;

  my $self = {
    mustCatchException_ => 0,
    exceptionKind_ => "",
    tupleTrace_ => 0,
    stackTrace_ => 1
  };

  foreach my $annot (@{$opContext->getAnnotations()}) {
    if ($annot->getName() eq "catch") {

      my $value = $annot->getValue();
      foreach my $attribute (@{$annot->getValue()->getAttributes()}) {
        if ($attribute->getName() eq "exception") {
          my $noQuotes = $attribute->getValue();
          $noQuotes =~ s/\"//;
          $noQuotes =~ s/\"//;
          $self->{exceptionKind_} = $noQuotes;
          $self->{mustCatchException_} = ($self->{exceptionKind_} eq "none") ? 0 : 1;
        } elsif ($attribute->getName() eq "tupleTrace") {
          if ($attribute->getValue() eq "true") {
            $self->{tupleTrace_} = 1;
          }
        } elsif ($attribute->getName() eq "stackTrace") {
          if ($attribute->getValue() eq "false") {
            $self->{stackTrace_} = 0;
          }
        }
      }
    }
  }

  return bless($self, $class);
}


1;
