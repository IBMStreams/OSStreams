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

package CompressCommon;
use strict;
use warnings;

sub verify($)
{
    my ($model) = @_;
    my $compressionInput = $model->getParameterByName("compressionInput");
    if (!$compressionInput) {
        # input must be just one blob
        my $inputPort = $model->getInputPortAt(0);
        my $numAttrs = $inputPort->getNumberOfAttributes();
        SPL::CodeGen::exitln(SPL::Msg::STDTK_INPUT_INVALID('Compress', 'compressionInput'),
                                           $model->getContext()->getSourceLocation())
            if $numAttrs != 1 || !SPL::CodeGen::Type::isBlob($inputPort->getAttributeAt(0)->getSPLType());
    }

    # output must be one blob
    my $outputPort = $model->getOutputPortAt(0);
    my $numAttrs = $outputPort->getNumberOfAttributes();
    SPL::CodeGen::exitln(SPL::Msg::STDTK_COMPRESS_OUTPUT_INVALID('Compress'),
                                       $model->getContext()->getSourceLocation())
        if $numAttrs != 1 || !SPL::CodeGen::Type::isBlob($outputPort->getAttributeAt(0)->getSPLType());

}

1;
