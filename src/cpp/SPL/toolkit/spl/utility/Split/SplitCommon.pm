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

package SplitCommon;
use strict;
use warnings;

sub verify($)
{
    my ($model) = @_;
    my $numOutputPorts = $model->getNumberOfOutputPorts();
    my $inputPort = $model->getInputPortAt(0);
    my $index = $model->getParameterByName("index");
    my $file = $model->getParameterByName("file");
    my $key = $model->getParameterByName("key");

    # legality checks
    SPL::CodeGen::exitln(SPL::Msg::STDTK_SPLIT_FILE_AND_INDEX(),
                         $file->getSourceLocation()) if ($file && $index);
    SPL::CodeGen::exitln(SPL::Msg::STDTK_SPLIT_FILE_NO_KEY(),
                         $file->getSourceLocation()) if ($file && !$key);
    SPL::CodeGen::exitln(SPL::Msg::STDTK_SPLIT_KEY_NO_FILE(),
                         $key->getSourceLocation()) if ($key && !$file);

    # need file or index!
    SPL::CodeGen::exitln(SPL::Msg::STDTK_SPLIT_NEED_FILE_OR_INDEX(),
                         $model->getContext()->getSourceLocation())
        if (!$file && !$index);

    # check matching schemas
    my $inputTupleType = $inputPort->getCppTupleType();
    for (my $i = 0; $i < $numOutputPorts; $i++) {
        my $op = $model->getOutputPortAt($i);
        SPL::CodeGen::exitln(SPL::Msg::STDTK_OUTPUT_SCHEMA_NOT_MATCHING_INPUT($i, 0),
                           $op->getSourceLocation())
            if $inputTupleType ne $op->getCppTupleType();
    }

    # Check index type (if present)
    if ($index) {
        my $iType = $index->getValueAt(0)->getSPLType();
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SPLIT_INVALID_INDEX_TYPE($iType),
                             $index->getValueAt(0)->getSourceLocation())
            unless $iType =~ /^u?int(32|64)$/ || $iType =~ /^list<u?int(32|64)>$/;
    }

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($isInConsistentRegion && $isInConsistentRegion->isStartOfRegion()) {
        SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
            $model->getContext()->getSourceLocation());
    }
}

1;
