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
# A class that represents an XML type in the spl code.
package SPL::XML::XMLModel;

use strict;
use warnings;
use XML::Simple;
$XML::Simple::PREFERRED_PARSER = "XML::Parser";

use IO::File;

## @cmethod new object($model)
# Creates a new XMLModel object
# @param model The XML describing the XMLModel.  This is parsed
# into an XML tree.
sub new
{
   my ($class, $model) = @_;

   my $self = { xml_ => undef };
   my $xml = XML::Simple->new(ForceArray => 1, KeepRoot => 0);

   my $fh = IO::File->new($model);
   $self->{xml_} = $xml->XMLin($fh);
   $fh->close();
   return bless($self, $class);
}

## @method string getClass()
#  Obtain the name of the class for this xml type.
#  @return Returns a string containing the name of the class representing this xml type.
sub getClass
{
   my ($self) = @_;
   return $self->{xml_}->{class}->[0];
}

## @method string getSchema()
#  Obtain the name of the schema for this xml type.
#  @return Returns a string containing the name of the schema representing this xml type.
sub getSchema
{
   my ($self) = @_;
   if (!defined($self->{xml_}->{schema})) {
      return "";
   }
   return $self->{xml_}->{schema}->[0];
}
1;
