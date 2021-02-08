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

package DynamicFilterCommon;
use strict;
use warnings;

sub verify($)
{
    my ($model) = @_;
    my $inputPort0 = $model->getInputPortAt(0);
    my $outputPort0 = $model->getOutputPortAt(0);
    my $inTupleType0 = $inputPort0->getCppTupleType();

    # Check port schemas
    SPL::CodeGen::exitln(SPL::Msg::STDTK_OUTPUT_SCHEMA_NOT_MATCHING_INPUT(0, 0),
                         $outputPort0->getSourceLocation())
        if($inTupleType0 ne $outputPort0->getCppTupleType());

    if ($model->getNumberOfOutputPorts() == 2) {
        my $outputPort1 = $model->getOutputPortAt(1);
        SPL::CodeGen::exitln(SPL::Msg::STDTK_OUTPUT_SCHEMA_NOT_MATCHING_INPUT(1, 0),
                             $outputPort1->getSourceLocation())
        if($inTupleType0 ne $outputPort1->getCppTupleType());
    }

    # Check parameter types
    my $keyParam = $model->getParameterByName("key");
    my $addKeyParam = $model->getParameterByName("addKey");
    my $removeKeyParam = $model->getParameterByName("removeKey");

    SPL::CodeGen::exitln(SPL::Msg::STDTK_DYNAMIC_FILTER_MISSING_REMOVE_KEY_WITH_INPUT_PORT(),
                         $model->getInputPortAt(2)->getSourceLocation())
        if ($model->getNumberOfInputPorts() == 3 && ! $removeKeyParam);

    if($removeKeyParam) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_DYNAMIC_FILTER_MISSING_INPUT_PORT_WITH_REMOVE_KEY(),
                             $removeKeyParam->getSourceLocation())
            if ($model->getNumberOfInputPorts() != 3);
        SPL::CodeGen::exitln(SPL::Msg::STDTK_DYNAMIC_FILTER_INVALID_NUMBER_EXPNS_FOR_PARAM('removeKey'),
                             $removeKeyParam->getSourceLocation())
            if ($removeKeyParam->getNumberOfValues() != $keyParam->getNumberOfValues());
    }

    SPL::CodeGen::exitln(SPL::Msg::STDTK_DYNAMIC_FILTER_INVALID_NUMBER_EXPNS_FOR_PARAM('addKey'),
                         $addKeyParam->getSourceLocation())
            if ($addKeyParam->getNumberOfValues() != $keyParam->getNumberOfValues());

    for (my $i = 0; $i < $keyParam->getNumberOfValues(); $i++) {
        my $k = $keyParam->getValueAt($i);
        my $ak = $addKeyParam->getValueAt($i);
        SPL::CodeGen::exitln(SPL::Msg::STDTK_DYNAMIC_FILTER_TYPE_MISMATCH_IN_EXPR_FOR_PARAM('addKey',
                             $i, $k->getSPLType(), $ak->getSPLType()), $ak->getSourceLocation())
            if ($k->getSPLType() ne $ak->getSPLType());
        if($removeKeyParam) {
            my $rk = $removeKeyParam->getValueAt($i);
            SPL::CodeGen::exitln(SPL::Msg::STDTK_DYNAMIC_FILTER_TYPE_MISMATCH_IN_EXPR_FOR_PARAM('removeKey',
                                 $i, $k->getSPLType(), $rk->getSPLType()), $rk->getSourceLocation())
                if ($k->getSPLType() ne $rk->getSPLType());
          }
    }

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($isInConsistentRegion && $isInConsistentRegion->isStartOfRegion()) {
        SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
            $model->getContext()->getSourceLocation());
    }
}

1;
