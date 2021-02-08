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

package SortCommon;
use strict;
use warnings;

sub verify($)
{
    my ($model) = @_;
    my $inputPort = $model->getInputPortAt(0);
    my $outputPort = $model->getOutputPortAt(0);
    my $inTupleType = $inputPort->getCppTupleType();
    my $outTupleType = $outputPort->getCppTupleType();


    my $window = $inputPort->getWindow();

    SPL::CodeGen::exitln(SPL::Msg::STDTK_OUTPUT_SCHEMA_NOT_MATCHING_INPUT(0, 0),
                       $outputPort->getSourceLocation())
        if($inTupleType ne $outTupleType);
   my $confs = SPL::CodeGen::createWindowConfigurations(
          { type     => $SPL::Operator::Instance::Window::TUMBLING },
          { type     => $SPL::Operator::Instance::Window::SLIDING,
            eviction => $SPL::Operator::Instance::Window::COUNT,
            trigger  => $SPL::Operator::Instance::Window::COUNT},
          { type     => $SPL::Operator::Instance::Window::TIME_INTERVAL});
    SPL::CodeGen::validateWindowConfiguration($inputPort, $confs);
    if($inputPort->getWindow()->hasTriggerPolicy()) {
        my $tpv = $inputPort->getWindow()->getTriggerPolicySize();
        my $tsz = $tpv->getSPLExpression();
        SPL::CodeGen::exitln(SPL::Msg::STDTK_INVALID_COUNT_BASED_TRIGGER_SIZE($tsz),
                           $tpv->getSourceLocation())
              unless($tsz == 1);
    }
    # Make sure sortBy expressions have ordered types
    my $sortByParam = $model->getParameterByName("sortBy");
    for my $value (@{$sortByParam->getValues()}) {
        my $type = $value->getSPLType();
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SORT_UNORDERED_SORTBY(
                $value->getSPLExpression(), $type), $value->getSourceLocation()) unless
            SPL::CodeGen::Type::isOrdered($type);
    }
    my $partitionByParam = $model->getParameterByName("partitionBy");
    $confs = SPL::CodeGen::createWindowConfigurations({ partitioned => 1});
    my $isPartitioned = SPL::CodeGen::checkWindowConfiguration($inputPort, $confs);
    if($partitionByParam) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_MISSING_PARTITIONED_WINDOW_MODIFIER("Sort", "partitionBy"),
                           $partitionByParam->getSourceLocation())
            if(not $isPartitioned);
    } else {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_MISSING_PARTITION_BY_PARAMETER("Sort", "partitionBy"),
                           $inputPort->getWindow()->getSourceLocation())
            if($isPartitioned);
    }
    my $orderParam = $model->getParameterByName("order");
    if($orderParam) {
        my $numOrderValues = $orderParam->getNumberOfValues();
        my $numSortByValues = $sortByParam->getNumberOfValues();
        if($numOrderValues!=1) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_INVALID_NUMBER_OF_ORDER_VALUES(
                    $numSortByValues, $numOrderValues), $orderParam->getSourceLocation())
                if($numOrderValues != $numSortByValues);
        }
    }
    my $consistentRegionContext = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($consistentRegionContext && $consistentRegionContext->isStartOfRegion()) {
            SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
                    $model->getContext()->getSourceLocation());
    }
}

1;
