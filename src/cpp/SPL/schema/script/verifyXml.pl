#!/usr/bin/perl

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

use strict;
use warnings;
use Getopt::Std;
use File::Basename;

my ($help,%opts);
my ($file,$schema);

####MAIN
my $sname = basename($0);
$help="usage: $sname -i <xml-input-file> -s <xsd-schema-file>";

getopts('hi:s:',\%opts) or
    die("\n${help}\n");
$file = $opts{i};
$schema = $opts{s};

if($opts{'h'})
{
  print $help, "\n";
  exit 0;
}

die "ERROR: Missing xml input file\n"
    unless($file);
die "ERROR: Missing xsd schema file\n"
    unless($schema);

my $command = "xmllint --noout --schema $schema $file";

system($command) == 0
    or die "ERROR: system $command failed: $?\n";
