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

## @package SPL::Helper
# A set of utility functions for emitting messages and program termination.

package SPL::Helper;

use strict;
use warnings;
use SPL::Msg;
use Cwd qw(realpath abs_path getcwd);
use File::Basename;
use Getopt::Std;
use Getopt::Long qw(:config no_ignore_case bundling);

require Exporter;
our @ISA = 'Exporter';
our @EXPORT = ('SPLStdOut', 'SPLStdOutLn',
               'SPLStdErr', 'SPLStdErrLn',
               'SPLError', 'SPLErrorLn',
               'SPLWarn', 'SPLWarnLn',
               'SPLDie', 'SPLUsage',
               'SPLText',
               'SPLFormattedMessageWithResname',
               'SPLFormattedMessage',
               'SPLGetOptionsLong',
               'SPLCheckInstalled',
               'SPLGetJavaPath',
               'SPLGetIBMJavaPath',
               'SPLEscapePath');

## @fn void SPLStdOut($msg, @params)
#  Performs a print (or printf with parameters) of msg to STDOUT
#  @param msg A string containing text to be printed.  It may optionally
#  contain format chracters, in which case a set of parameters should follow the message.
#  @param params An optional list of parameters to satisfy the format chars in msg.
sub SPLStdOut
{
  my ($msg, @params) = @_;
  if (scalar(@params) > 0) {
    printf STDOUT $msg, @params;
  } else  {
    print STDOUT $msg;
  }
}

## @fn void SPLStdOutLn($msg, @params)
#  The same as SPLStdOut() with a newline added at the end.
#  @param msg A string containing text to be printed.  It may optionally
#  contain format chracters, in which case a set of parameters should follow the message.
#  @param params An optional list of parameters to satisfy the format chars in msg.
sub SPLStdOutLn
{
  SPLStdOut(@_);
  print STDOUT "\n";
}

## @fn void SPLStdErr($msg, @params)
#  The same as SPLStdOut() except output goes to STDERR.
#  @param msg A string containing text to be printed.  It may optionally
#  contain format chracters, in which case a set of parameters should follow the message.
#  @param params An optional list of parameters to satisfy the format chars in msg.
sub SPLStdErr
{
  my ($msg, @params) = @_;
  if (scalar(@params) > 0) {
    printf STDERR $msg, @params;
  } else {
    print STDERR $msg;
  }
}

## @fn void SPLStdErrLn($msg, @params)
#  The same as SPLStdOutLn() except output goes to STDERR.
#  @param msg A string containing text to be printed.  It may optionally
#  contain format chracters, in which case a set of parameters should follow the message.
#  @param params An optional list of parameters to satisfy the format chars in msg.
sub SPLStdErrLn
{
  SPLStdErr(@_);
  print STDERR "\n";
}

## @fn void SPLError($msg, @params)
#  The same as SPLStdErr() with the string 'ERROR' prepended to msg.
#  @param msg A string containing text to be printed.  It may optionally
#  contain format chracters, in which case a set of parameters should follow the message.
#  A leading CDISP[0-9][0-9][0-9][0-9]E will be moved to before the ERROR
#  @param params An optional list of parameters to satisfy the format chars in msg.
sub SPLError
{
  my ($msg, @params) = @_;
  my $sev = SPL::Msg::ERROR();
  my $msgId = "";
  if ($msg =~ /^(CDISP[0-9][0-9][0-9][0-9]E )(.*)/s) {
    $msgId = $1;
    $msg = $2;
  }
    print STDERR "${msgId}${sev}: ";
  if (scalar(@params) > 0) {
    printf STDERR $msg, @params;
  } else {
    print STDERR $msg;
  }
}

## @fn void SPLErrorLn($msg, @params)
#  The same as SPLError() with a new line added at the end.
#  @param msg A string containing text to be printed.  It may optionally
#  contain format chracters, in which case a set of parameters should follow the message.
#  @param params An optional list of parameters to satisfy the format chars in msg.
sub SPLErrorLn
{
  SPLError(@_);
  print STDERR "\n";
}

## @fn void SPLWarn($msg, @params)
#  The same as SPLStdErr() with the string 'WARNING' prepended to msg.
#  @param msg A string containing text to be printed.  It may optionally
#  contain format chracters, in which case a set of parameters should follow the message.
#  A leading CDISP[0-9][0-9][0-9][0-9][EW] will be moved to before the WARNING
#  @param params An optional list of parameters to satisfy the format chars in msg.
sub SPLWarn
{
  my ($msg, @params) = @_;
  my $sev = SPL::Msg::WARNING();
  my $msgId = "";
  if ($msg =~ /^(CDISP[0-9][0-9][0-9][0-9][EW] )(.*)/s) {
      $msgId = $1;
      $msg = $2;
  }
  print STDERR "${msgId}${sev}: ";
  if (scalar(@params) > 0) {
    printf STDERR $msg, @params;
  } else {
    print STDERR $msg;
  }
}

## @fn void SPLWarnLn($msg, @params)
#  The same as SPLWarn() with a new line added at the end.
#  @param msg A string containing text to be printed.  It may optionally
#  contain format chracters, in which case a set of parameters should follow the message.
#  @param params An optional list of parameters to satisfy the format chars in msg.
sub SPLWarnLn
{
  SPLWarn(@_);
  print STDERR "\n";
}

## @fn unsigned SPLDie($ecode, $msg, @params)
#  Exits the program with the specified return code after printing the
#  given msg.
#  @param ecode The value to be returned from the program.
#  @param msg A string containing text to be printed.  It may optionally
#  contain format chracters, in which case a set of parameters should follow the message.
#  @param params An optional list of parameters to satisfy the format chars in msg.
sub SPLDie
{
  my ($ecode, $msg, @params) = @_;
  SPLErrorLn($msg, @params);
  exit $ecode;
}

sub SPLMessageUtil()
{
  my $cwd = dirname(__FILE__);
  my $installDir = $ENV{STREAMS_INSTALL};
  return "${installDir}/system/impl/bin/st-getmsg";
}

my $messageEmitted = 0;

sub SPLFormattedMessageWithResname
{
  my $text = SPLFormattedMessage(@_);

  my $resName = $_[4];

  return "$resName: $text";
}

sub SPLFormattedMessage
{
  my ($toolkitDir, $toolkitName, $bundleName, $defaultLang, $resName, $defaultText, @argList) = @_;

  my $dt = $${defaultText};
  chomp $dt;
  # Escape any double quotes and '$'
  $dt =~ s/"/\\"/g;
  $dt =~ s/\$/\\\$/g;

  my $args = "";
  for (my $i = 0; $i < scalar @argList; ++$i) {
    my $arg = $argList[$i];
    my $argType = "s";
    # See if we can better determine what the arg type is
    if ($arg =~ /^[-+]?\d+$/) {
        $argType = "i";
    } elsif ($arg =~ /^[-+]?\d+\.\d+$/) {
        $argType = "f";
    }
    if ($argType eq "s") {
        # Escape any double quotes and '$'
        $arg =~ s/"/\\"/g;
        $arg =~ s/\$/\\\$/g;
        $arg = "\"" . $arg . "\"";
    }
    $args .= " ${argType}:" . $arg;
  }
  my $stgmsg = SPLMessageUtil();
  my $cmd = "${stgmsg} -u -i ${toolkitDir} -t ${toolkitName} -b ${bundleName} -m ${resName} -x \"" . ${dt} . "\" -d ${defaultLang} -n -p " . ${args};
  # dependency-scan: ignore the following line
  my  $text = `$cmd`;

  return $text;
}

## @fn unsigned SPLText($resname, $defaultText, $argList)
#  Returns the text for the message from the appropriate resource bundle
#  @param resname The id of the message to be loaded.
#  @param defaultText Text to use in an english locale or if we can't
#  load the message.
#  @param argList The list of substitution arguments
sub SPLText($$@)
{
  my ($resname, $defaultText, @argList) = @_;
  my $dt = $${defaultText};
  chomp $dt;
  # Escape any double quotes and '$'
  $dt =~ s/"/\\"/g;
  $dt =~ s/\$/\\\$/g;

  my $args = "";
  for (my $i = 0; $i < scalar @argList; ++$i) {
    my $arg = $argList[$i];
    my $argType = "s";
    # See if we can better determine what the arg type is
    if ($arg =~ /^[-+]?\d+$/) {
        $argType = "i";
    } elsif ($arg =~ /^[-+]?\d+\.\d+$/) {
        $argType = "f";
    }
    if ($argType eq "s") {
        # Escape any double quotes and '$'
        $arg =~ s/"/\\"/g;
        $arg =~ s/\$/\\\$/g;
        $arg = "\"" . $arg . "\"";
    }
    $args .= " ${argType}:" . $arg;
  }
  my $stgmsg = SPLMessageUtil();
  my $cmd = "$stgmsg -m $resname -p -x\"" . $dt ."\" -d en " . $args . " 2>/dev/null";
  # dependency-scan: ignore the following line
  my $text = `$cmd`;
  chomp $text;

  my $msgPrefix = ($resname =~ /.{9}T/) ? "" : "$resname ";
  return "${msgPrefix}${text}";
}

my @optionWarnings = ();
sub optionWarningHandler($)
{
   my ($msg) = @_;
   if ($msg =~ m/Unknown option: (.*)/) {
      my $text = SPL::Msg::UNKNOWN_OPTION($1);
      push(@optionWarnings, $text);
   } elsif ($msg =~ m/Value \"([^\"]+)\" invalid for option (\S+) .*/) {
      my $text = SPL::Msg::INVALID_OPTION_VALUE($1, $2);
      push(@optionWarnings, $text);
   }
   return 1;
}

## @fn void SPLUsage(string $usageString, unsigned ecode)
#  Calls the given function to display "usage" info and then
#  exits with the given return code.
#  @param usageString A pointer to the function that will emit
#  the usage.
#  @param ecode The return code to be used for exit.
sub SPLUsage($$)
{
   my ($usageString, $ecode) = @_;
   my $sname = basename($0);

   if ($ecode == 0) {
      SPLStdOutLn($usageString->($sname));
   } else {
      SPLStdErrLn($usageString->($sname));
   }
   exit($ecode);
}

## @fn void SPLGetOptionsLong(string $usageString, %opts, @optionList)
#  This function manages the parsing of options and the
#  display of option error messages and usage information
#  in the context of globalization.
#  @param usageString The address of a "usage" function
#  that is called with the name of the executable if there
#  is a problem with the options.
#  @param opts A reference to the hash to be populated
#  @param optionList The options that are expected
sub SPLGetOptionsLong
{
   my ($usageString, $opts, @optionList) = @_;
   my %orig;
   $orig{__WARN__} = $SIG{__WARN__};
   $SIG{__WARN__} = \&optionWarningHandler;
   my $rc = GetOptions($opts, @optionList);
   $SIG{__WARN__} = $orig{__WARN__};

   foreach my $msg (@optionWarnings) {
      SPLStdErrLn($msg);
   }

   SPLUsage($usageString, 1) unless $rc;
   SPLUsage($usageString, 0) if ($$opts{help});
}

## @fn bool SPLCheckInstalled(string $executable)
#  This function checks if the given executable is installed and runnable
#  on the system.
#  @param executable The executable to check.
#  @return Returns 1 if the executable is installed, otherwise 0.
sub SPLCheckInstalled($)
{
   my ($exe) = @_;
   my $rc = system("which ${exe} > /dev/null 2>&1");
   return ($rc == 0) ? 1 : 0;
}

# deprecated
sub SPLGetIBMJavaPath()
{
    my $install = $ENV{'JAVA_HOME'};
    return "${install}/bin";
}

## @fn string SPLGetJavaPath()
#  Get the path to the java in the Streams install
#  @return Returns the java path
sub SPLGetJavaPath()
{
    my $install = $ENV{'JAVA_HOME'};
    return "${install}/bin";
}

## @fn string SPLEscapePath(string $path)
#  Escapes any characterst hat must be escaped when passed through to the shell
#  via the system or other calls.
#  @param path The path to be escaped
#  @return Returns a path with space, $, <, > and & escaped
sub SPLEscapePath($)
{
    my ($path) = @_;
    $path =~ s/\$/\\\$/g;
    $path =~ s/\</\\\</g;
    $path =~ s/\>/\\\>/g;
    $path =~ s/&/\\&/g;
    $path =~ s/ /\\ /g;
    return $path;
}

1;
