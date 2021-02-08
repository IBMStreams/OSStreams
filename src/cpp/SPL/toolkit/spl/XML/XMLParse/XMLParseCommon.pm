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

package XMLParseCommon;
use strict;
use warnings;

my $flatten;
my $attrsName;
my $textName;
sub verify($)
{
    my ($model) = @_;
    my $numOutputs = $model->getNumberOfOutputPorts();
    my $trigger = $model->getParameterByName("trigger");
    my $xmlInput = $model->getParameterByName("xmlInput");
    $flatten = $model->getParameterByName("flatten");
    $attrsName = $model->getParameterByName("attributesName");
    $textName = $model->getParameterByName("textName");

    $attrsName = $attrsName ? $attrsName->getValueAt(0)->getSPLExpression() : "_attrs";
    $textName = $textName ? $textName->getValueAt(0)->getSPLExpression() : "_text";
    $flatten = $flatten ? $flatten->getValueAt(0)->getSPLExpression() : "none";

    if ($numOutputs != $trigger->getNumberOfValues()) {
        SPL::CodeGen::exitln (SPL::Msg::STDTK_XMLPARSE_WRONG_NUMBER_TRIGGERS($numOutputs, $trigger->getNumberOfValues()), $trigger->getSourceLocation());
    }

    # we can only handle certain output types
    for (my $i = 0; $i < $numOutputs; $i++) {
        verifyOutputTuple ($model->getOutputPortAt($i));
    }

    # Check our input type
    my $inputType;
    if ($xmlInput) {
        $inputType = $xmlInput->getValueAt(0)->getSPLType();
    } else {
        my $inputPort = $model->getInputPortAt(0);
        my $numAttrs = $inputPort->getNumberOfAttributes();
        SPL::CodeGen::exitln (SPL::Msg::STDTK_XMLPARSE_XMLINPUT_MISSING(), $inputPort->getSourceLocation())
            unless $numAttrs == 1;
        $inputType = $inputPort->getAttributeAt(0)->getSPLType();
    }
    SPL::CodeGen::exitln (SPL::Msg::STDTK_XMLPARSE_UNSUPPORTED_INPUT_TYPE($inputType),
                          $trigger->getSourceLocation())
        unless SPL::CodeGen::Type::isRString($inputType) ||
               SPL::CodeGen::Type::isUString($inputType) ||
               SPL::CodeGen::Type::isBlob($inputType) ||
               SPL::CodeGen::Type::isXml($inputType);

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($isInConsistentRegion && $isInConsistentRegion->isStartOfRegion()) {
        SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
            $model->getContext()->getSourceLocation());
    }
}

sub verifyOutputTuple($)
{
    my ($outputPort) = @_;
    my $numAttrs = $outputPort->getNumberOfAttributes();
    for (my $i = 0; $i < $numAttrs; $i++) {
        my $attr = $outputPort->getAttributeAt($i);
        if ($attr->hasAssignmentWithOutputFunction()) {
            # Check the output assignment fcn
            my $fcn = $attr->getAssignmentOutputFunctionName();
            verifyOutputAttr ($attr, $outputPort, 1, $attr->getAssignmentSourceLocation()) unless $fcn eq "AsIs";
        } elsif (!$attr->hasAssignment()) {
            # No output function.  Is it the right type?
            verifyOutputAttr ($attr, $outputPort, 0, $outputPort->getSourceLocation());
        }
    }
}

sub checkBaseType($$$$$$);
sub verifyOutputAttr($$$)
{
    my ($attr, $outputPort, $allowAnyType, $loc) = @_;
    my $aType = $attr->getSPLType();
    my $n = $attr->getName();
    checkBaseType ($n, $aType, $aType, $outputPort, $allowAnyType ? -1 : 0, $loc);
}

sub isMapRStringRString($)
{
    my ($type) = @_;
    return 0 if !SPL::CodeGen::Type::isMap($type);
    my $k = SPL::CodeGen::Type::getKeyType($type);
    my $v = SPL::CodeGen::Type::getValueType($type);
    return SPL::CodeGen::Type::isRString($k) && SPL::CodeGen::Type::isRString($v);
}

sub checkBaseType($$$$$$)
{
    my ($aName, $type, $wholeType, $outputPort, $depth, $loc) = @_;

    # special cases for attrsName
    if ($flatten ne "attributes" && $aName eq $attrsName) {
        return if isMapRStringRString($type);
        SPL::CodeGen::exitln(SPL::Msg::STDTK_XMLPARSE_BAD_MAP_ATTRIBUTE($aName), $loc);
    }

    if ($flatten ne "elements" && $aName eq $textName) {
        return if SPL::CodeGen::Type::isRString($type);
        SPL::CodeGen::exitln(SPL::Msg::STDTK_XMLPARSE_BAD_TEXT_ATTRIBUTE($aName), $loc);
    }

    return if SPL::CodeGen::Type::isRString($type) ||
              SPL::CodeGen::Type::isUString($type) ||
              SPL::CodeGen::Type::isIntegral($type) ||
              SPL::CodeGen::Type::isFloatingpoint($type) ||
              isMapRStringRString($type);

    # Handle the list case recursively
    if ($depth <= 0 && (SPL::CodeGen::Type::isList($type) || SPL::CodeGen::Type::isBList($type))) {
        my $eType = SPL::CodeGen::Type::getElementType($type);
        checkBaseType ($aName, $eType, $wholeType, $outputPort, $depth == -1 ? -1 : $depth+1, $loc);
        if (SPL::CodeGen::Type::hasBound($type)) {
            my $bound = SPL::CodeGen::Type::getBound($type);
            SPL::CodeGen::exitln(SPL::Msg::STDTK_XMLPARSE_INVALID_BLIST ($bound, $type, $aName),
                $loc) unless $bound == 1;
        }
        return;
    }

    if (SPL::CodeGen::Type::isTuple($type)) {
        checkTupleType ($aName, $type, $wholeType, $outputPort, $loc);
        return;
    }

    # For now, anything else is bad
    SPL::CodeGen::exitln (SPL::Msg::STDTK_XMLPARSE_INVALID_OUTPUT_TYPE($wholeType, $aName), $loc);
}

sub checkTupleType($$$$$)
{
    my ($attr, $type, $wholeType, $outputPort, $loc) = @_;
    my @names = SPL::CodeGen::Type::getAttributeNames($type);
    my @types = SPL::CodeGen::Type::getAttributeTypes($type);
    for (my $i = 0; $i < scalar(@names); $i++) {
        checkBaseType ($attr . '.' . $names[$i], $types[$i], $wholeType, $outputPort, 0, $loc);
    }
}
1;
