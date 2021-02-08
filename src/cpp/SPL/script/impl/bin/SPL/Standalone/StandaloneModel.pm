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
# A class representing the model of a standalone application
package SPL::Standalone::StandaloneModel;

use strict;
use warnings;
use XML::Simple;
$XML::Simple::PREFERRED_PARSER = "XML::Parser";

use SPL::ModelHelper;
use IO::File;

## @cmethod object new($model)
#  Creates a new StandaloneModel object
#  @param model The XML describing the processing element.  This is parsed into
#  an XML tree.
#  @return Returns a new StandaloneModel object.
sub new {
  my ($class, $model) = @_;

  my $self = {
    xml_              =>  undef
  };

  my $xml = XML::Simple->new (ForceArray => 1, KeepRoot => 0);
  my $fh = IO::File->new($model);
  $self->{xml_} = $xml->XMLin($fh);
  $fh->close();

  return bless($self, $class);
}

1;
