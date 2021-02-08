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
use Cwd qw(abs_path realpath);
use File::Basename ;
use Getopt::Long qw(:config no_ignore_case bundling) ;

sub usage
{
    print "Usage: " . basename(abs_path($0)) . " [--help] --dir dirname\n";
    print "\n";
    print "Author:  Paul Bye, pcbye\@us.ibm.com - ported to Linux/perl script from\n";
    print "\n";
    print "Fix generated doxygen files so that they are accessibility-compliant.\n";
    print "\n";
    print "Parameters\n";
    print "--dir dirname - scan starting in directory dirname\n";
    print"\n";
    exit;
}

################## MAIN ###############

# Constants
my $THISDIR = dirname(abs_path($0));

# Parameters
my $HELP = 0;
my $VERBOSE = 0;
my $DIRNAME;

my $result = GetOptions("h|help"    => \$HELP,
                        "v|verbose" => \$VERBOSE,
                        "d|dir:s"   => \$DIRNAME);

if (!$result || $HELP)
{
    usage() ;
    exit($result ? 0 : 1) ;
}

if (!$DIRNAME || !-d $DIRNAME)
{
    print STDERR "You must specify an exiting directory for the --dir parameter.\n";
    exit 1;
}

my $cmd;
my $output;
my $rc;
my $overallrc = 0;

print "*******************************\n";
print "Processing CSS files...\n";
print "*******************************\n";
$cmd = "python $THISDIR/fl.py  -s $THISDIR/doxycss.fls -d $DIRNAME -b -w *.css";
print "$cmd\n" if $VERBOSE;
print `$cmd`;
$rc = $? >> 8;
if ($rc)
{
    print "*****CSS Accessibility Generation failed with RC = $rc\n";
    $overallrc = 1;
}
print "*******************************\n";
print "Processing HTML files...\n";
print "*******************************\n";
$cmd = "python $THISDIR/fl.py  -s $THISDIR/doxygen.fls -d $DIRNAME -b";
print "$cmd\n" if $VERBOSE;
print `$cmd`;
$rc = $? >> 8;
if ($rc)
{
    print "*****HTML Accessibility Generation failed with RC = $rc\n";
    $overallrc = 1;
}

`rm -f $DIRNAME/*.bak`;

my @filelist = `find $DIRNAME -type f -name "*.htm*" -print `;
my $tidy = "/usr/bin/tidyp -q -m -b --force-output yes --add-xml-decl yes --indent-spaces 4 --wrap 0 --tab-size 4 --char-encoding utf8 --output-encoding utf8 --indent auto --output-xhtml yes --language en --numeric-entities no --new-empty-tags dt --enclose-text yes --tidy-mark no --logical-emphasis yes --quote-marks no --clean yes --drop-empty-paras yes --preserve-entities yes --wrap-attributes no --wrap-sections no --merge-divs no";

print "Processing files...\n";

for (my $i = 0; $i < @filelist; $i++)
{
    print "Processing file $filelist[$i]\n"; #if $VERBOSE;
    #print "." if !$VERBOSE;

    # Hack/fix to remove <u>S</u> so that tidy doesn't replace that with <span>...
    $cmd = "sed -i 's/<u>S<\\/u>/S/g' $filelist[$i]";
    print "$cmd\n" if $VERBOSE;
    print `$cmd`;

    $cmd = "$tidy $filelist[$i]";
    print "$cmd\n" if $VERBOSE;
    print `$tidy $filelist[$i]`;

    $cmd = 'sed -i "s@<div class=\"header\"@<div role=\"region\" aria-label=\"Header Label\" class=\"header\"@g"';
    print "$cmd\n" if $VERBOSE;
    print `$cmd $filelist[$i]`;

    $cmd = 'sed -i "s@<div class=\"contents\"@<div role=\"main\" class=\"contents\"@g"';
    print "$cmd\n" if $VERBOSE;
    print `$cmd $filelist[$i]`;

    # Fix @deprecated Doxygen annotation issue
    $cmd =  'sed -i "s-<dt><span class=\"strong\"><a class=\"el\" href=\"deprecated.html#_deprecated\(......\)\">Deprecated:</a></span></dt>-<dt><dd><span class=\"strong\"><a class=\"el\" href=\"deprecated.html#_deprecated\1\">Deprecated:</a></span></dd></dt>-g"';
    print "$cmd\n" if $VERBOSE;
    print `$cmd $filelist[$i]`;

    # Changes to support CHKPII
    # Remove <?xml line
    # Move <title> line in spl-builtins.html to before <link>
    $cmd = "sed -e '/^<.xml /d' -e '/<title>SPL Standard Toolkit Types and Functions<.title>/d' -e '/<link rel=\"STYLESHEET\" href=\"spl-builtins.css\"/i<title>SPL Standard Toolkit Types and Functions</title>' -i $filelist[$i]";
    print "$cmd\n" if $VERBOSE;
    print `$cmd`;
    # End: Changes to support CHKPII

    # Add headers to table for accessibility (defect 8403)
    if ($filelist[$i] =~ /\/annotated.html$/) {
        # Add in the proper headings
        $cmd = "sed -e '/<table>/a<tr><th align=\"left\" scope=\"col\">Class</th><th align=\"left\" scope=\"col\">Description</th></tr>' -i $filelist[$i]";
        print "$cmd\n" if $VERBOSE;
        print `$cmd`;
    } elsif ($filelist[$i] =~ /\/namespaces.html$/) {
        # Add in the proper headings
        $cmd = "sed -e '/<table>/a<tr><th align=\"left\" scope=\"col\">Namespace</th><th align=\"left\" scope=\"col\">Description</th></tr>' -i $filelist[$i]";
        print "$cmd\n" if $VERBOSE;
        print `$cmd`;
    } elsif ($filelist[$i] =~ /\/files.html$/) {
        # Add in the proper headings
        $cmd = "sed -e '/<table>/a<tr><th align=\"left\" scope=\"col\">Filename</th><th align=\"left\" scope=\"col\">Description</th></tr>' -i $filelist[$i]";
        print "$cmd\n" if $VERBOSE;
        print `$cmd`;
    } elsif ($filelist[$i] =~ /\/class_s_p_l_1_1_meta_1_1_base_type.html$/) {
        # Remove duplicate ids
        $cmd = "sed -i 's#id=\"a280486bbd28119c53a2359a54b84278d\"##g' $filelist[$i]";
        print "$cmd\n" if $VERBOSE;
        print `$cmd`;
    }

}

exit $overallrc;
