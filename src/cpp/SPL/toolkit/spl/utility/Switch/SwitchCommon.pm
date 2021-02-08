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

package SwitchCommon;
use strict;
use warnings;

sub verify($)
{
    my ($model) = @_;
    my $dataInputPort = $model->getInputPortAt(0);
    my $dataOutputPort = $model->getOutputPortAt(0);

    SPL::CodeGen::exitln(SPL::Msg::STDTK_OUTPUT_SCHEMA_NOT_MATCHING_INPUT(0, 0),
        $dataOutputPort->getSourceLocation())
            if $dataInputPort->getCppTupleType() ne $dataOutputPort->getCppTupleType();

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($isInConsistentRegion) {
      SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_LIVE_IN_CC_REGION($model->getContext()->getKind()),
          $model->getContext()->getSourceLocation());
    }
}

1;
