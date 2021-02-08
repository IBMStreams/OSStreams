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

package SPL::Operator::Instance::Toolkit;

use strict;
use warnings;

# DOXYGEN_SKIP_FOR_USERS

## @method string getName()
# Get the name of this toolkit
# @return string with the name of the toolkit
sub getName
{
    my ($self) = @_;
    return $self->{name_};
}

## @method string getRootDirectory()
# Get the root directory for this toolkit
# @return string with the root direcrtory of the toolkit
sub getRootDirectory
{
    my ($self) = @_;
    return $self->{rootDirectory_};
}

## @method int getIndex()
# Get the index of this toolkit
# @return returns the index of the toolkit
sub getIndex
{
    my ($self) = @_;
    return $self->{index_};
}

## @method int getIsCurrentToolkit()
# Determine if this is the current (application) toolkit
# @return returns 1 if this is the current toolkit, otherwise 0
sub getIsCurrentToolkit
{
    my ($self) = @_;
    return $self->{isCurrentToolkit_};
}

sub new
{
    my ($class, $xml) = @_;
    my $self = { name_ => $xml->{name}->[0],
                 rootDirectory_ => $xml->{rootDirectory}->[0],
                 index_ => $xml->{index}->[0],
                 isCurrentToolkit_ => ($xml->{isCurrentToolkit} eq 'true') ? 1 : 0
               };
    return bless($self, $class);
}

1;
