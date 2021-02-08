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

package JoinCommon;
use strict;
use warnings;

sub verify($)
{
    my ($model) = @_;
    my $lhsInputPort = $model->getInputPortAt(0);
    my $rhsInputPort = $model->getInputPortAt(1);
    my $outputPort = $model->getOutputPortAt(0);
    my $lhsWindow = $lhsInputPort->getWindow();
    my $rhsWindow = $rhsInputPort->getWindow();

    # PUNCT eviction windows not allowed
    my $confs = SPL::CodeGen::createWindowConfigurations(
           { type    => $SPL::Operator::Instance::Window::SLIDING,
             eviction=> $SPL::Operator::Instance::Window::COUNT,
             trigger => $SPL::Operator::Instance::Window::COUNT},
           { type    => $SPL::Operator::Instance::Window::SLIDING,
             eviction=> $SPL::Operator::Instance::Window::TIME,
             trigger => $SPL::Operator::Instance::Window::COUNT},
           { type    => $SPL::Operator::Instance::Window::SLIDING,
             eviction=> $SPL::Operator::Instance::Window::DELTA,
             trigger => $SPL::Operator::Instance::Window::COUNT});
    SPL::CodeGen::validateWindowConfiguration($lhsInputPort, $confs);
    SPL::CodeGen::validateWindowConfiguration($rhsInputPort, $confs);
    if ($lhsInputPort->getWindow()->hasTriggerPolicy()) {
        my $tpv = $lhsInputPort->getWindow()->getTriggerPolicySize();
        my $tsz = $tpv->getSPLExpression();
        SPL::CodeGen::exitln(
            SPL::Msg::STDTK_INVALID_COUNT_BASED_TRIGGER_SIZE($tsz),
            $tpv->getSourceLocation())
            unless $tsz == 1;
    }
    if ($rhsInputPort->getWindow()->hasTriggerPolicy()) {
        my $tpv = $rhsInputPort->getWindow()->getTriggerPolicySize();
        my $tsz = $tpv->getSPLExpression();
        SPL::CodeGen::exitln(
            SPL::Msg::STDTK_INVALID_COUNT_BASED_TRIGGER_SIZE($tsz),
            $tpv->getSourceLocation())
            unless $tsz == 1;
    }
    # check number of outputs for each possible algorithm
    my $algorithm = $model->getParameterByName("algorithm");
    $algorithm = !defined ($algorithm) ? "inner" :
                        $algorithm->getValueAt(0)->getSPLExpression();
    my $numOutputs = $model->getNumberOfOutputPorts();
    my ($outputPort2, $outputPort3);
    if ($numOutputs > 1) {
        $outputPort2 = $model->getOutputPortAt(1);
        if ($algorithm eq "inner") {
            SPL::CodeGen::exitln(
                SPL::Msg::STDTK_JOIN_INVALID_NUMBER_OUTPUTS($numOutputs, "1", $algorithm),
                               $outputPort2->getSourceLocation());
       } elsif ($algorithm eq "leftOuter") {
            SPL::CodeGen::exitln(
                SPL::Msg::STDTK_JOIN_INVALID_NUMBER_OUTPUTS($numOutputs, "2", $algorithm),
                               $outputPort2->getSourceLocation()) if $numOutputs != 2;
            SPL::CodeGen::exitln(SPL::Msg::STDTK_JOIN_INVALID_LEFT_OUTER_SCHEMA(),
                               $outputPort2->getSourceLocation())
                   if $outputPort2->getCppTupleType() ne $lhsInputPort->getCppTupleType();
       } elsif ($algorithm eq "rightOuter") {
            SPL::CodeGen::exitln(
                SPL::Msg::STDTK_JOIN_INVALID_NUMBER_OUTPUTS($numOutputs, "2", $algorithm),
                               $outputPort2->getSourceLocation()) if $numOutputs != 2;
            SPL::CodeGen::exitln(SPL::Msg::STDTK_JOIN_INVALID_RIGHT_OUTER_SCHEMA(),
                               $outputPort2->getSourceLocation())
                   if $outputPort2->getCppTupleType() ne $rhsInputPort->getCppTupleType();
       } else {
            # must be outer
            SPL::CodeGen::exitln(
                SPL::Msg::STDTK_JOIN_INVALID_NUMBER_OUTPUTS($numOutputs, "3", $algorithm),
                               $outputPort2->getSourceLocation()) if $numOutputs != 3;
            SPL::CodeGen::exitln(SPL::Msg::STDTK_JOIN_INVALID_LEFT_OUTER_SCHEMA(),
                               $outputPort2->getSourceLocation())
                   if $outputPort2->getCppTupleType() ne $lhsInputPort->getCppTupleType();
            $outputPort3 = $model->getOutputPortAt(2);
            SPL::CodeGen::exitln(SPL::Msg::STDTK_JOIN_INVALID_RIGHT_OUTER_SCHEMA(),
                               $outputPort3->getSourceLocation())
                   if $outputPort3->getCppTupleType() ne $rhsInputPort->getCppTupleType();
       }
    }

    my $defaultTupleLHS = $model->getParameterByName("defaultTupleLHS");
    my $defaultTupleRHS = $model->getParameterByName("defaultTupleRHS");
    if ($defaultTupleLHS && $algorithm ne "rightOuter" && $algorithm ne "outer") {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_JOIN_UNEXPECTED_DEFAULT_TUPLE_SPECIFIED_FOR_JOIN("defaultTupleLHS", $algorithm),
                               $defaultTupleLHS->getSourceLocation())
    }
    if ($defaultTupleRHS && $algorithm ne "leftOuter" && $algorithm ne "outer") {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_JOIN_UNEXPECTED_DEFAULT_TUPLE_SPECIFIED_FOR_JOIN("defaultTupleRHS", $algorithm),
                               $defaultTupleRHS->getSourceLocation())
    }
    if ($numOutputs > 1 && $defaultTupleLHS) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_JOIN_UNEXPECTED_DEFAULT_TUPLE_SPECIFIED_FOR_MULTI_OUTPUT_JOIN("defaultTupleLHS"),
                               $defaultTupleLHS->getSourceLocation())
    }
    if ($numOutputs > 1 && $defaultTupleRHS) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_JOIN_UNEXPECTED_DEFAULT_TUPLE_SPECIFIED_FOR_MULTI_OUTPUT_JOIN("defaultTupleRHS"),
                               $defaultTupleRHS->getSourceLocation())
    }

    # make sure default tuples match respecitve ports
    if ($defaultTupleLHS) {
        my $tupleType = $defaultTupleLHS->getValueAt(0)->getCppType();
        SPL::CodeGen::exitln(
            SPL::Msg::STDTK_JOIN_DEFAULT_TUPLE_TYPE_ERROR("defaultTupleLHS", 0),
                                           $defaultTupleLHS->getSourceLocation())
            if $tupleType ne $lhsInputPort->getCppTupleType();
    }
    if ($defaultTupleRHS) {
        my $tupleType = $defaultTupleRHS->getValueAt(0)->getCppType();
        SPL::CodeGen::exitln(
            SPL::Msg::STDTK_JOIN_DEFAULT_TUPLE_TYPE_ERROR("defaultTupleRHS", 1),
                                           $defaultTupleRHS->getSourceLocation())
            if $tupleType ne $rhsInputPort->getCppTupleType();
    }


    # Make sure equality[RL]HS expressions match
    my $equalityLHS = $model->getParameterByName("equalityLHS");
    my $equalityRHS = $model->getParameterByName("equalityRHS");
    if (defined ($equalityLHS) || defined ($equalityRHS)) {
        my $eq = defined ($equalityLHS) ? $equalityLHS : $equalityRHS;
        SPL::CodeGen::exitln(SPL::Msg::STDTK_INVALID_NUMBER_EXPNS("equalityLHS",
                                                                  "equalityRHS"),
                $eq->getSourceLocation())
            if !defined ($equalityLHS) || !defined ($equalityRHS);
        SPL::CodeGen::exitln(SPL::Msg::STDTK_INVALID_NUMBER_EXPNS("equalityLHS",
                                                                  "equalityRHS"),
                $eq->getSourceLocation())
            if $equalityLHS->getNumberOfValues() != $equalityRHS->getNumberOfValues();
        for (my $i = 0; $i < $equalityLHS->getNumberOfValues(); $i++) {
            my $l = $equalityLHS->getValueAt($i);
            my $r = $equalityRHS->getValueAt($i);
            SPL::CodeGen::exitln(SPL::Msg::STDTK_JOIN_INVALID_EQUALITY_EXPNS($l->getSPLType(), $r->getSPLType()),
                    $l->getSourceLocation())
                if $l->getSPLType() ne $r->getSPLType();

        }
    }

    # check partitioning
    my $p = "partitionByLHS";
    my $partitionByParam = $model->getParameterByName($p);
    $confs = SPL::CodeGen::createWindowConfigurations({ partitioned => 1});
    my $isPartitioned = SPL::CodeGen::checkWindowConfiguration($lhsInputPort, $confs);
    if($partitionByParam) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_MISSING_PARTITIONED_WINDOW_MODIFIER("Join", $p),
                             $partitionByParam->getSourceLocation())
            if not $isPartitioned;
    } else {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_MISSING_PARTITION_BY_PARAMETER("Join", $p),
                             $lhsInputPort->getWindow()->getSourceLocation())
            if $isPartitioned;
    }
    $p = "partitionByRHS";
    $partitionByParam = $model->getParameterByName($p);
    $isPartitioned = SPL::CodeGen::checkWindowConfiguration($rhsInputPort, $confs);
    if($partitionByParam) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_MISSING_PARTITIONED_WINDOW_MODIFIER("Join", $p),
                             $partitionByParam->getSourceLocation())
            if not $isPartitioned;
    } else {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_MISSING_PARTITION_BY_PARAMETER("Join", $p),
                             $rhsInputPort->getWindow()->getSourceLocation())
            if $isPartitioned;
    }
    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($isInConsistentRegion && $isInConsistentRegion->isStartOfRegion()) {
        SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
            $model->getContext()->getSourceLocation());
    }
}

1;
