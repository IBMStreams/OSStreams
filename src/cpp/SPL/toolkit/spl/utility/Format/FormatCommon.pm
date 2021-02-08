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

package FormatCommon;
use strict;
use warnings;

use File::Basename;

sub verify($)
{
    my ($model) = @_;
    # Format is like a Sink for parameter checking
    unshift @INC, dirname(dirname($model->getContext()->getOperatorDirectory())) . "/adapter/Common";
    require AdapterHelper;
    AdapterHelper::verifySinkModel($model);

    # Check that no input attributes are referenced if writePuncuations : true;
    my $writePunctuations = $model->getParameterByName("writePunctuations");
    $writePunctuations = $writePunctuations ?
         $writePunctuations->getValueAt(0)->getCppExpression() eq "true" : 0;

    if ($writePunctuations) {
        my $outputPort = $model->getOutputPortAt(0);
        my $numAttrs = $outputPort->getNumberOfAttributes();
        for (my $i = 0; $i < $numAttrs; $i++) {
            my $attr = $outputPort->getAttributeAt($i);
            if ($attr->hasAssignment()) {
                my $aName = $attr->getName();
                my $expn;
                if ($attr->hasAssignmentWithOutputFunction()) {
                    next if $attr->getAssignmentOutputFunctionName() ne "AsIs";
                    $expn = $attr->getAssignmentValue();
                } else {
                    $expn = $attr->getAssignmentOutputFunctionParameterValues()->[0];
                }
                SPL::CodeGen::exitln(SPL::Msg::STDTK_FORMAT_BAD_STREAM_ATTR_REF($aName),
                                     $expn->getSourceLocation()) if $expn->hasStreamAttributes();
            }
        }
    }

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($isInConsistentRegion && $isInConsistentRegion->isStartOfRegion()) {
        SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
            $model->getContext()->getSourceLocation());
    }
}

1;
