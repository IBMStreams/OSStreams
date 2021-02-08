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

package ParseCommon;
use strict;
use warnings;

use File::Basename;

sub verify($)
{
    my ($model) = @_;
    # Parse is like a Source for parameter checking
    unshift @INC, dirname(dirname($model->getContext()->getOperatorDirectory())) . "/adapter/Common";
    require AdapterHelper;
    AdapterHelper::verifySourceModel($model);

    my $parseInput = $model->getParameterByName("parseInput");
    if (!$parseInput) {
        # input must be just one blob
        my $inputPort = $model->getInputPortAt(0);
        my $numAttrs = $inputPort->getNumberOfAttributes();
        SPL::CodeGen::exitln(SPL::Msg::STDTK_INPUT_INVALID('Parse', 'parseInput'),
                                           $model->getContext()->getSourceLocation())
            if $numAttrs != 1 || !SPL::CodeGen::Type::isBlob($inputPort->getAttributeAt(0)->getSPLType());
    }

    # hasHeaderLine is only valid with csv format
    my $format = $model->getParameterByName("format");
    $format = $format ? $format->getValueAt(0)->getCppExpression() : "csv";
    my $hasHeaderLine = $model->getParameterByName("hasHeaderLine");
    SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_INVALID_HAS_HEADER_LINE(),
                         $hasHeaderLine->getSourceLocation())
         if $hasHeaderLine && $format ne "csv";
    if ($hasHeaderLine) {
        # hasHeaderLine must be boolean or uint32
        my $typ = $hasHeaderLine->getValueAt(0)->getSPLType();
        SPL::CodeGen::exitln (SPL::Msg::STDTK_FILESOURCE_HAS_HEADER_LINE_WRONG_TYPE($typ),
                              $hasHeaderLine->getSourceLocation())
                if !(SPL::CodeGen::Type::isUint32($typ) || SPL::CodeGen::Type::isBoolean($typ));
    }
    my $consistentRegionContext = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($consistentRegionContext && $format eq "block") {
	SPL::CodeGen::exitln(SPL::Msg::STDTK_PARSE_BLOCK_IN_CONSISTENT_REGION(), $model->getParameterByName('format')->getSourceLocation());
    }
    if ($consistentRegionContext && $consistentRegionContext->isStartOfRegion()) {
            SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
                    $model->getContext()->getSourceLocation());
        }
}

1;
