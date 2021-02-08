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

package DeDuplicateCommon;
use strict;
use warnings;

sub verify($)
{
    my ($model) = @_;
    my $inputPort = $model->getInputPortAt(0);
    my $inTupleType = $inputPort->getCppTupleType();

    if ($model->getNumberOfOutputPorts() == 2) {
        my $outputPort = $model->getOutputPortAt(1);
        my $outTupleType = $outputPort->getCppTupleType();
        SPL::CodeGen::exitln(SPL::Msg::STDTK_OUTPUT_SCHEMA_NOT_MATCHING_INPUT(1, 0),
            $outputPort->getSourceLocation()) if $inTupleType ne $outTupleType;
    }

    my $timeOut = $model->getParameterByName("timeOut");
    my $count = $model->getParameterByName("count");
    my $deltaAttribute = $model->getParameterByName("deltaAttribute");
    my $delta = $model->getParameterByName("delta");

    # check for conflicts
    SPL::CodeGen::exitln (SPL::Msg::STDTK_PARAM_CONFLICT('timeOut', 'count'),
                          $timeOut->getSourceLocation()) if $timeOut && $count;
    SPL::CodeGen::exitln (SPL::Msg::STDTK_PARAM_CONFLICT('deltaAttribute', 'count'),
                          $deltaAttribute->getSourceLocation()) if $deltaAttribute && $count;
    SPL::CodeGen::exitln (SPL::Msg::STDTK_PARAM_CONFLICT('deltaAttribute', 'timeOut'),
                          $deltaAttribute->getSourceLocation()) if $deltaAttribute && $timeOut;

    SPL::CodeGen::exitln (SPL::Msg::STDTK_DEDUPLICATE_DELTA_MISSING(),
                           $deltaAttribute ? $deltaAttribute->getSourceLocation() : $delta->getSourceLocation())
        if ($deltaAttribute && !$delta) || (!$deltaAttribute && $delta);

    if ($deltaAttribute) {
        SPL::CodeGen::exitln (SPL::Msg::STDTK_DEDUPLICATE_DELTA_DIFFERENT_TYPES(), $deltaAttribute->getSourceLocation())
            if $deltaAttribute->getValueAt(0)->getCppType() ne $delta->getValueAt(0)->getCppType();

        my $splType = $deltaAttribute->getValueAt(0)->getSPLType();
        SPL::CodeGen::exitln (SPL::Msg::STDTK_DEDUPLICATE_BAD_DELTA_TYPE(),  $deltaAttribute->getSourceLocation())
            if !(SPL::CodeGen::Type::isIntegral($splType) || SPL::CodeGen::Type::isFloatingpoint($splType) ||
                 SPL::CodeGen::Type::isTimestamp($splType) || SPL::CodeGen::Type::isEnum($splType));
    }

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($isInConsistentRegion && $isInConsistentRegion->isStartOfRegion()) {
        SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
            $model->getContext()->getSourceLocation());
    }
}

sub isNanType($) {
    my ($type) = @_;
    return SPL::CodeGen::Type::isFloatingpoint($type) ||
        SPL::CodeGen::Type::isComplex($type);
}

sub hasNanType(@);

sub hasNanType(@) {
    my @types = @_;
    my $hasNan = 0;

  TYPES:
    foreach my $type(@types) {
        my $refType = ref($type);
        if (!$refType) {
            if (SPL::CodeGen::Type::isPrimitive($type)) {
                $hasNan = isNanType($type);
                last TYPES if $hasNan;
            }
            elsif (SPL::CodeGen::Type::isTuple($type)) {
                my @attributeTypes = SPL::CodeGen::Type::getAttributeTypes($type);
                $hasNan = hasNanType(@attributeTypes);
                last TYPES if $hasNan;
            }
            elsif (SPL::CodeGen::Type::isList($type) ||
                   SPL::CodeGen::Type::isBList($type) ||
                   SPL::CodeGen::Type::isSet($type) ||
                   SPL::CodeGen::Type::isBSet($type)) {
                my $elementType = SPL::CodeGen::Type::getElementType($type);
                $hasNan = hasNanType($elementType);
                last TYPES if $hasNan;
            }
            elsif (SPL::CodeGen::Type::isMap($type) ||
                   SPL::CodeGen::Type::isBMap($type)) {
                my $keyType = SPL::CodeGen::Type::getKeyType($type);
                $hasNan = hasNanType($keyType);
                last TYPES if $hasNan;
                my $valueType = SPL::CodeGen::Type::getValueType($type);
                $hasNan = hasNanType($valueType);
                last TYPES if $hasNan;
            }
            else {
                $hasNan = isNanType ($type);
                last TYPES if $hasNan;
            }
        }
        elsif ($refType eq 'ARRAY') {
            $hasNan = hasNanType (@$type);
            last TYPES if $hasNan;
        }
        elsif ($refType eq 'SPL::Operator::Instance::InputAttribute') {
            my $splType = $type->getSPLType();
            $hasNan = hasNanType($splType);
            last TYPES if $hasNan;
        }
    }

    return $hasNan;
}


1;
