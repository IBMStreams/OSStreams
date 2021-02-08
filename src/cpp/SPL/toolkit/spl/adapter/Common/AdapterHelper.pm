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

## @package AdapterHelper
# A collection of utility functions to faciliate adapter code generation.
package AdapterHelper;

use strict;
use warnings;
use SPL::CodeGen;
use Encode qw(encode decode);

sub verifySeparator($$)
{
    my ($model, $isSource) = @_;

    my $separator = $model->getParameterByName("separator");
    return unless ($separator);

    my $format = $model->getParameterByName("format");
    my $origFileFormat = $format;
    my $formatKind = $format ? $format->getValueAt(0)->getCppExpression() : "csv";

    if ($formatKind ne "csv") {
        if ($format) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_SEPARATOR_NOT_CSV_FORMAT(), $format->getSourceLocation());
        } else {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_SEPARATOR_NOT_CSV_FORMAT(), $separator->getSourceLocation());
        }
        return;
    }

    $separator = $separator->getValueAt(0);
    my $sepType = $separator->getSPLType();
    if ($sepType eq "uint8" || $sepType eq "int8") {
        my $sepValue = $separator->getSPLExpression();
        if ($sepValue == 9) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_UNSUPPORTED_SEPARATOR_VALUE_TAB(), $separator->getSourceLocation()) if ($isSource);
        }
        elsif ($sepValue == 32) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_UNSUPPORTED_SEPARATOR_VALUE(" "), $separator->getSourceLocation()) if ($isSource);
        }
    } elsif ($sepType eq "rstring") {
        my $sepValue = $separator->getCppExpression();

        # There are 3 invalid patterns, tab, space, and multi-byte utf-8 characters
        if ($sepValue =~ /SPL::rstring\(\"\\x(..)\\x(..)\"\)/) {
            my $utf8L = hex($1);
            my $utf8R = hex($2);
            if (($utf8L == 194 || $utf8L == 195) && ($utf8R > 127 && $utf8R < 192)) {
                SPL::CodeGen::errorln(SPL::Msg::STDTK_SEPARATOR_ALTERNATIVE(), $separator->getSourceLocation());
            }
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_INVALID_SEPARATOR(), $separator->getSourceLocation());
        }
        elsif ($sepValue =~ /SPL::rstring\(\"\\t\"\)/) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_UNSUPPORTED_SEPARATOR_VALUE_TAB(), $separator->getSourceLocation()) if ($isSource);
        }
        elsif ($sepValue =~ /SPL::rstring\(\" \"\)/) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_UNSUPPORTED_SEPARATOR_VALUE(" "), $separator->getSourceLocation()) if ($isSource);
        }
    } else {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_PARAMETER_WRONG_TYPE("separator", $sepType, "int8/uint8/rstring"), $separator->getSourceLocation());
    }
}

## @fn void verifySourceModel($model)
# This function will do common source checks
# @param model Operator Instance Model
sub verifySourceModel($)
{
    my ($model) = @_;
    my $outputPort = $model->getOutputPortAt(0);
    my $format = $model->getParameterByName("format");
    my $options = $model->getParameterByName("option");
    my $eolMarker = $model->getParameterByName("eolMarker");
    my $blockSize = $model->getParameterByName("blockSize");
    my $compression = $model->getParameterByName("compression");
    my $hasDelayField = $model->getParameterByName("hasDelayField");
    my $parsing = $model->getParameterByName("parsing");
    my $defaultTuple = $model->getParameterByName("defaultTuple");
    my $encoding = $model->getParameterByName("encoding");
    my $readPunctuations = $model->getParameterByName("readPunctuations");

    verifySeparator($model, 1);

    # compute which attributes we will generate
    my $numAttrs = $outputPort->getNumberOfAttributes();
    my @genAttrs;
    for (my $i = 0; $i < $numAttrs; $i++) {
        my $attr = $outputPort->getAttributeAt($i);
        push @genAttrs, $attr if !$attr->hasAssignment();
    }

    # this is how many are left
    $numAttrs = scalar(@genAttrs);

    SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_NO_ATTRIBUTES($model->getContext()->getKind()), $outputPort->getSourceLocation()) if $numAttrs == 0;

    # defaults
    my $origFileFormat = $format;
    $format = $format ? $format->getValueAt(0)->getCppExpression() : "csv";

    my $origFileParsing = $parsing;
    $parsing = $parsing->getValueAt(0)->getCppExpression() if $parsing;

    # check for combinations that are not legal
    if ($encoding && ($format eq "bin" || $format eq "block")) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_ENCODING_BAD_FORMAT($format), $origFileFormat->getSourceLocation());
    }

    SPL::CodeGen::exitln(SPL::Msg::STDTK_SINK_INVALID_READPUNCT(), $readPunctuations->getSourceLocation()) if $readPunctuations && $format ne 'bin';

    if ($eolMarker && $format ne "line") {
        if ($origFileFormat) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_EOLMARKER_NOT_LINE_FORMAT(), $origFileFormat->getSourceLocation());
        } else {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_EOLMARKER_NOT_LINE_FORMAT(), $eolMarker->getSourceLocation());
        }
    }

    if ($defaultTuple) {
        if($format ne "csv") {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_DEFAULT_TUPLE_NOT_CSV_FORMAT(), $defaultTuple->getSourceLocation());
        }
        if($defaultTuple->getValueAt(0)->getCppType() ne $outputPort->getCppTupleType()) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_DEFAULT_TUPLE_NOT_MATCHING_OUTPUT_SCHEMA(), $defaultTuple->getSourceLocation());
        }
    }

    SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_BLOCKSIZE_NOT_BLOCK_FORMAT(), $blockSize->getSourceLocation()) if $blockSize && $format ne "block";

    my $opKind = $model->getContext()->getKind();
    if ($opKind ne "spl.adapter::UDPSource" && $opKind ne "spl.adapter::FileSource") {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_MISSING_BLOCKSIZE(), $origFileFormat->getSourceLocation())
            if $format eq "block" && !defined $blockSize;
    }

    SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_INVALID_PACED(),
                       $hasDelayField->getSourceLocation())
        if $hasDelayField && !($format eq "csv" || $format eq "bin" || $format eq "txt");

    SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_INVALID_PARSING(), $origFileParsing->getSourceLocation())
        if $parsing && $parsing eq "permissive" && !($format eq "bin" || $format eq "csv" || $format eq "txt");

    if ($format eq "line") {
        # output must be one rstring attribute
        if ($numAttrs != 1 || !SPL::CodeGen::Type::isRString($genAttrs[0]->getSPLType())) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_LINE_ATTRIBUTE_INVALID(), $model->getContext()->getSourceLocation());
        }
    } elsif ($format eq "block") {
        # output must be one blob attribute
        if ($numAttrs != 1 || !SPL::CodeGen::Type::isBlob($genAttrs[0]->getSPLType())) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_BLOCK_ATTRIBUTE_INVALID(), $model->getContext()->getSourceLocation());
        }
    }

    # Check eolMarker
    my ($eolChar1, $eolChar2);
    ($eolChar1, $eolChar2) = extractEolChars ($eolMarker) if $eolMarker;
}

## @fn void extractEolChars($eolMarker)
# This function extract1 or two chars from the eolString or complain if there are not 1 or 2
# @param eolMarker parameter from the model
sub extractEolChars($)
{
    my ($eolMarker) = @_;
    my $v = $eolMarker->getValueAt(0)->getSPLExpression();
    my $bareString;
    my ($char1, $char2);
    if ($v =~ /^"(.+)"$/) {
        $bareString = $1;
    } else {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_EOLMARKER_INVALID(),
                           $eolMarker->getSourceLocation());
        return (undef, undef);
    }

    # is the first character a special one?
    if ($bareString =~ /^(\\[abfnrtv0"])/) {
        $char1 = $1;
        $bareString = substr $bareString, 2;
    } elsif ($bareString =~ /^\\u([0-9a-fA-F]{2})([0-9a-fA-F]{2})/) {
        $char1 = "\\x$2";
        $bareString = substr $bareString, 6;
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_EOLMARKER_INVALID(),
                             $eolMarker->getSourceLocation())
                         unless $1 eq '00';
    } else {
        $char1 = substr $bareString, 0, 1;
        $bareString = substr $bareString, 1;
    }

    if (length $bareString == 0) {
        # only 1 character
        return ($char1, undef);
    }

    # How about the secondOne?
    if ($bareString =~ /^(\\[abfnrtv0"])/) {
        $char2 = $1;
        $bareString = substr $bareString, 2;
    } elsif ($bareString =~ /^\\u([0-9a-fA-F]{2})([0-9a-fA-F]{2})/) {
        $char2 = "\\x$2";
        $bareString = substr $bareString, 6;
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_EOLMARKER_INVALID(),
                             $eolMarker->getSourceLocation())
                         unless $1 eq '00';
    } else {
        $char2 = substr $bareString, 0, 1;
        $bareString = substr $bareString, 1;
    }

    # anything left?
    if (length $bareString != 0) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_EOLMARKER_INVALID(),
                           $eolMarker->getSourceLocation());
        return (undef, undef);
    }

    return ($char1, $char2);
}

## @fn void verifySinkModel($model)
# This function will do common sink checks
# @param model Operator Instance Model
sub verifySinkModel($)
{
    my ($model) = @_;

    my $inputPort = $model->getInputPortAt(0);
    my $format = $model->getParameterByName("format");
    my $compression = $model->getParameterByName("compression");
    my $eolMarker = $model->getParameterByName("eolMarker");
    my $hasDelayField = $model->getParameterByName("hasDelayField");
    my $writePunctuations = $model->getParameterByName("writePunctuations");
    my $flush = $model->getParameterByName("flush");
    my $quoteStrings = $model->getParameterByName("quoteStrings");
    my $separator = $model->getParameterByName("separator");
    my $suppress = $model->getParameterByName("suppress");
    my $origSeparator = $separator;
    my %suppressedAttrs;
    my $numAttrs = $inputPort->getNumberOfAttributes();
    if ($suppress) {
        my $numValues = $suppress->getNumberOfValues();
        for (my $i = 0; $i < $numValues; $i++) {
            my $e = $suppress->getValueAt($i);
            my $a = $e->getSPLExpression();
            # remove the stream name
            $a =~ s/^.*\.//;
            $suppressedAttrs{$a} = 1;
        }
    }
    # compute the attributes we will generate
    my @genAttrs;
    for (my $i = 0; $i < $numAttrs; ++$i) {
        my $attr = $inputPort->getAttributeAt($i);
        push @genAttrs, $attr if !$suppressedAttrs{$attr->getName()};
    }
    $numAttrs = scalar(@genAttrs);

    # defaults
    my $origFileFormat = $format;
    $format = $format ? $format->getValueAt(0)->getCppExpression() : "csv";

    $separator = $separator->getValueAt(0)->getCppExpression() if $separator;

    # check for combinations that are not legal
    if ($eolMarker && $format ne "line") {
        if ($origFileFormat) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_EOLMARKER_NOT_LINE_FORMAT(), $origFileFormat->getSourceLocation());
        } else {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_EOLMARKER_NOT_LINE_FORMAT(), $eolMarker->getSourceLocation());
        }
    }

    if ($separator && $format ne "csv") {
        if ($origFileFormat) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_SEPARATOR_NOT_CSV_FORMAT(), $origFileFormat->getSourceLocation());
        } else {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_SEPARATOR_NOT_CSV_FORMAT(), $origSeparator->getSourceLocation());
        }
    }
    SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_QUOTESTRINGS_NOT_CSV_FORMAT(), $quoteStrings->getSourceLocation()) if $quoteStrings && $format ne "csv";

    if ($separator) {
        # Multi-byte utf-8 characters are not supported
        if ($separator =~ /SPL::rstring\(\"\\x..\\x..\"\)/) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_INVALID_SEPARATOR(), $origSeparator->getSourceLocation());
        }
    }

    SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_INVALID_PACED(), $hasDelayField->getSourceLocation())
        if $hasDelayField && !($format eq "csv" || $format eq "bin" || $format eq "txt");

    SPL::CodeGen::exitln(SPL::Msg::STDTK_SINK_INVALID_WRITEPUNCT(), $writePunctuations->getSourceLocation())
        if $writePunctuations && !($format eq "csv" || $format eq "txt" || $format eq 'bin');

    if ($format eq "line") {
        # input must be one rstring attribute
        if ($numAttrs != 1 || !SPL::CodeGen::Type::isRString($genAttrs[0]->getSPLType())) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_LINE_ATTRIBUTE_INVALID(), $model->getContext()->getSourceLocation());
        }
    } elsif ($format eq "block") {
        # input must be one blob attribute
        if ($numAttrs != 1 || !SPL::CodeGen::Type::isBlob($genAttrs[0]->getSPLType())) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_BLOCK_ATTRIBUTE_INVALID(), $model->getContext()->getSourceLocation());
        }
    }

    # Check eolMarker
    my ($eolChar1, $eolChar2);
    ($eolChar1, $eolChar2) = extractEolChars ($eolMarker) if $eolMarker;;
}

## @fn void verifySecureModel($model)
# This function will do common SSL / TLS checks
# @param model Operator Instance Model
sub verifySecureModel($)
{
    my ($model) = @_;

    my $sslProtocol = $model->getParameterByName("sslProtocol");
    my $sslConfigName = $model->getParameterByName("sslConfigName");
    if (defined $sslProtocol || defined $sslConfigName) {
        # Checks on parameters when SSL is enabled
        my $role = $model->getParameterByName("role");
        my $sslAuthenticateClient = $model->getParameterByName("sslAuthenticateClient");
        $role = $role->getValueAt(0)->getSPLExpression() if defined $role;
        # sslAuthenticated not used with client
        if (defined $sslAuthenticateClient && $role eq "client") {
            SPL::CodeGen::warnln(SPL::Msg::STDTK_SSL_PARAM_IGNORED_BY_ROLE("sslAuthenticateClient", "client"), $sslAuthenticateClient->getSourceLocation());
        }
    } else {
        # Warn about SSL options if SSL is not enabled
        my $sslParam;

        $sslParam = $model->getParameterByName("sslCertificateFile");
        if (defined $sslParam) {
            SPL::CodeGen::warnln(SPL::Msg::STDTK_SSL_PARAM_IGNORED_NO_SSL("sslCertificateFile"), $sslParam->getSourceLocation());
        }

        $sslParam = $model->getParameterByName("password");
        if (defined $sslParam) {
            SPL::CodeGen::warnln(SPL::Msg::STDTK_SSL_PARAM_IGNORED_NO_SSL("password"), $sslParam->getSourceLocation());
        }

        $sslParam = $model->getParameterByName("sslCertificateAuthorityFile");
        if (defined $sslParam) {
            SPL::CodeGen::warnln(SPL::Msg::STDTK_SSL_PARAM_IGNORED_NO_SSL("sslCertificateAuthorityFile"), $sslParam->getSourceLocation());
        }

        $sslParam = $model->getParameterByName("sslAuthenticateClient");
        if (defined $sslParam) {
            SPL::CodeGen::warnln(SPL::Msg::STDTK_SSL_PARAM_IGNORED_NO_SSL("sslAuthenticateClient"), $sslParam->getSourceLocation());
        }

        $sslParam = $model->getParameterByName("sslCipherSuites");
        if (defined $sslParam) {
            SPL::CodeGen::warnln(SPL::Msg::STDTK_SSL_PARAM_IGNORED_NO_SSL("sslCipherSuites"), $sslParam->getSourceLocation());
        }
    }
}

1;
