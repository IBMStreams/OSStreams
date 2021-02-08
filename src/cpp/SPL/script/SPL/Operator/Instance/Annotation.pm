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

# DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::Annotation
# \brief Class that represents an Annotation
package SPL::Operator::Instance::Annotation;

use strict;
use warnings;

use SPL::Operator::Instance::TupleValue;

use Data::Dumper;

## @method string getName()
# Get the name of the annotation
# @return Returns the name of the annotation
sub getName
{
  my ($self) = @_;
  return $self->{name_};
}

## @method TupleValue getvalue()
# Get the value associated with this annotation.
# The value will be a TupleValue and will have one attribute per key/value pair
# in the annotation.
# @return Returns the value associated with this annotation
sub getValue
{
  my ($self) = @_;
  return $self->{value_};
}

sub new
{
  my ($class, $xml) = @_;
  my $self = {};
  $self->{name_} = $xml->{name};
  $self->{value_} = SPL::Operator::Instance::TupleValue->new($xml);
  return bless($self, $class);
}

1;
