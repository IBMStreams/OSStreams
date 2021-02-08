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
# A class representing a NL location
package SPL::PE::NLLocation;

use strict;
use warnings;

use SPL::ModelHelper;

## @cmethod object new($xml)
#  Creates a new NLLocation object
#  @param xml The XML tree describing the NLLocation
#  @return Returns a new NLLocation object.
sub new
{
    my ($class, $xml) = @_;
    my $self = {
        location_ => $xml->{location},
        toolkitName_ => $xml->{toolkitName},
        path_ => $xml->{path}
    };
    return bless($self, $class);
}

## @method string getLocation()
#  Determine the reletive location of the resource
#  @return Returns one of Local, External, Install
sub getLocation
{
    my ($self) = @_;
    return $self->{location_};
}

## @method string getToolkitName()
#  Determine the name of the toolkit containing the resource
#  @return Returns the toolkit name
sub getToolkitName
{
    my ($self) = @_;
    return $self->{toolkitName_};
}

## @method string getPath()
#  Determine the path to the toolkit containing the resource
#  @return Returns the toolkit path
sub getPath
{
    my ($self) = @_;
    return $self->{path_};
}
1;
