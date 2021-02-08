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
use File::Temp qw/ tempdir /;

use FindBin;
use lib ($FindBin::Bin);


my ($help,%opts);
my ($file);

####MAIN
my $sname = basename($0);
$help="usage: $sname -i <xsd-file>";

getopts('hi:',\%opts) or
    die("\n${help}\n");
$file = $opts{i};

if($opts{'h'})
{
  print $help, "\n";
  exit 0;
}

die "ERROR: Missing input file\n"
    unless($file);

# the version of xmllint we use cannot handle XMLSchema.xsd, so we use DTD
# DTD does not understand namespaces, so we need the following hack to
# remove certain lines from the document

my $tdir = tempdir(CLEANUP=>1);
my $tfile = "${tdir}/" . basename($file);
open(FILE, "<${file}")
  or die "ERROR: Cannot open file ${file} for reading\n";
open(TFILE, ">${tfile}")
  or die "ERROR: Cannot open file ${tfile} for writing\n";
while(<FILE>) {
  my ($ns) = m/xmlns:(\w*)\s*=\s*\"/;
  if($ns and $ns ne "xs" and $ns ne "xsd") {
    print TFILE "\n";
    next;
  }
  print TFILE $_;
}
close(FILE);
close(TFILE);

my $command = "cd ${tdir}; xmllint --noout --dtdvalid ".
    $FindBin::Bin . "/../dtd/XMLSchema.dtd ${tfile}";

system($command) == 0 or
    die "ERROR: system $command failed: $?\n";
