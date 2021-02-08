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

package PairCommon;
use strict;
use warnings;

sub verify($)
{
    my ($model) = @_;
    my $numInputPorts = $model->getNumberOfInputPorts();
    my $numParameters = $model->getNumberOfParameters();
    my @parameters = $model->getParameters();
    my $inputPort0 = $model->getInputPortAt(0);
    my $inTupleType = $inputPort0->getCppTupleType();
    for(my $i=1; $i<$numInputPorts; ++$i) {
        my $inputPort = $model->getInputPortAt($i);
        SPL::CodeGen::exitln(SPL::Msg::STDTK_INPUT_SCHEMAS_NOT_MATCHING(),
                             $inputPort->getSourceLocation())
            if($inTupleType ne $inputPort->getCppTupleType());
    }
    my $outputPort = $model->getOutputPortAt(0);
    my $outTupleType = $outputPort->getCppTupleType();
    SPL::CodeGen::exitln(SPL::Msg::STDTK_OUTPUT_SCHEMA_NOT_MATCHING_INPUT(0, 0),
                         $outputPort->getSourceLocation())
        if($inTupleType ne $outTupleType);

    # Are there any partitionByParameters?  If so, all inputs need them, and they have to match
    my $seenPartitionBy = 0;
    my @partitions;
    for (my $i = 0; $i < $numParameters; $i++) {
        my $p = $model->getParameterAt($i);
        my $pName = $p->getName();
        if ($pName =~ /^partitionByLHS$/) {
            $seenPartitionBy = 1;
            duplicateError (0, $p, $partitions[0]) if defined $partitions[0];
            $partitions[0] = $p;
        } elsif ($pName =~ /^partitionByRHS$/) {
            $seenPartitionBy = 1;
            duplicateError ($numInputPorts-1, $p, $partitions[$numInputPorts-1])
                if defined $partitions[$numInputPorts-1];
            $partitions[$numInputPorts-1] = $p;
        } elsif ($pName =~ /^partitionBy(\d+)$/) {
            $seenPartitionBy = 1;
            duplicateError ($1, $p, $partitions[$1]) if defined $partitions[$1];
            $partitions[$1] = $p;
        }
    }

    if ($seenPartitionBy) {
        # Check that all the expressions exist
        for (my $i = 0; $i < $numInputPorts; $i++) {
            SPL::CodeGen::exitln (SPL::Msg::STDTK_BARRIER_PAIR_MISSING_PARTITION_BY($i),
                                  $model->getContext()->getSourceLocation())
                unless defined $partitions[$i];
        }
        # Check that all the expressions match
        my $p = $partitions[0];
        my $numExpns = $p->getNumberOfValues();
        for (my $i = 1; $i < $numInputPorts; $i++) {
            my $g = $partitions[$i];
            SPL::CodeGen::exitln (
                SPL::Msg::STDTK_BARRIER_PAIR_PARTITION_NUMBER_EXPNS(
                        $g->getName(),
                        $g->getNumberOfValues(),
                        $numExpns),
                $g->getSourceLocation()) if $g->getNumberOfValues() != $numExpns;
            for (my $j = 0; $j < $numExpns; $j++) {
                my $l = $p->getValueAt($j);
                my $r = $g->getValueAt($j);
                SPL::CodeGen::exitln(
                    SPL::Msg::STDTK_BARRIER_PAIR_INVALID_PARTITION_EXPN(
                            $g->getName(), $l->getSPLType(), $r->getSPLType()),
                        $l->getSourceLocation())
                    if $l->getSPLType() ne $r->getSPLType();
            }
        }
    }

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($isInConsistentRegion) {
        if ($isInConsistentRegion->isStartOfRegion()) {
            SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
                $model->getContext()->getSourceLocation());
        }

        my $bufferSize = $model->getParameterByName("bufferSize");
        if ($bufferSize) {
            # Cannot support this parameter as tuple processing may wait for tuples come from other streams.
            # The wait works if it is assumed that no tuples will be lost, but that is not the case
            # in distributed topologies that can crash
            SPL::CodeGen::exitln(SPL::Msg::STDTK_PARAMETER_INVALID_IN_CR("bufferSize"),
                    $model->getParameterByName("bufferSize")->getSourceLocation());
        }
    }


}

sub duplicateError ($$)
{
    my ($num, $new, $old) = @_;
    SPL::CodeGen::exitln (
        SPL::Msg::STDTK_BARRIER_PAIR_DUPLICATE_PARTITIONBY($new->getName(), $num),
            $new->getSourceLocation());
}

1;
