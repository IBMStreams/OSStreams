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

package FileSourceCommon;
use strict;
use warnings;

use File::Basename;

sub verify($)
{
    my ($model) = @_;
    unshift @INC, dirname($model->getContext()->getOperatorDirectory()) . "/Common";
    require AdapterHelper;
    AdapterHelper::verifySourceModel($model);

    my $moveFileToDirectory = $model->getParameterByName("moveFileToDirectory");
    my $deleteFile = $model->getParameterByName("deleteFile");
    $deleteFile = $deleteFile ? $deleteFile->getValueAt(0)->getSPLExpression() eq "true" : 0;
    my $ignoreOpenErrors = $model->getParameterByName("ignoreOpenErrors");

    # check for file and input port
    my $file = $model->getParameterByName("file");
    my $hotFile = $model->getParameterByName("hotFile");
    my $crContext = $model->getContext()->getOptionalContext("ConsistentRegion");
    my $isStart = $crContext ? $crContext->isStartOfRegion() : 0;

    if ($model->getNumberOfInputPorts() == 0) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_NO_FILE(),
                             $model->getContext()->getSourceLocation()) if !$file;
    } else {
        my $inputPort = $model->getInputPortAt(0);
        my $tupleType = $inputPort->getCppTupleType();

        SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_INPUT_NOT_RSTRING(),
                             $inputPort->getSourceLocation())
            if $inputPort->getNumberOfAttributes() != 1 ||
                 !SPL::CodeGen::Type::isRString($inputPort->getAttributeAt(0)->getSPLType());

        SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_INPUT_AND_PARAM("file"),
                             $inputPort->getSourceLocation()) if $file;
        SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_INPUT_AND_PARAM("hotFile"),
                             $inputPort->getSourceLocation()) if $hotFile;
    }

    # check optional secondary stream
    if ($model->getNumberOfOutputPorts() == 2) {
        my $op = $model->getOutputPortAt(1);
        SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_FILE_OUTPUT_INVALID(), $op->getSourceLocation())
            unless $op->getNumberOfAttributes() == 2;
        my $rstrings = 0;
        my $int32s = 0;
        for (my $i = 0; $i < 2; $i++) {
            my $t = $op->getAttributeAt($i)->getSPLType();
            $rstrings++ if SPL::CodeGen::Type::isRString($t);
            $int32s++ if SPL::CodeGen::Type::isInt32($t);
        }
        SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_FILE_OUTPUT_INVALID(), $op->getSourceLocation())
            unless $rstrings == 1 && $int32s == 1;
    }

    # hasHeaderLine is only valid with csv format
    my $format = $model->getParameterByName("format");
    $format = $format ? $format->getValueAt(0)->getSPLExpression() : "csv";
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

    # other option conflicts
    SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_OPTION_CONFLICT("hotFile", "deleteFile"),
                         $hotFile->getSourceLocation()) if $hotFile && $deleteFile;
    SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_OPTION_CONFLICT("hotFile", "moveFileToDirectory"),
                         $hotFile->getSourceLocation()) if $hotFile && $moveFileToDirectory;
    SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_OPTION_CONFLICT("deleteFile", "moveFileToDirectory"),
                         $moveFileToDirectory->getSourceLocation()) if $deleteFile && $moveFileToDirectory;
    SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_HOTFILE_BLOCK_WITHOUT_BLOCKSIZE())
                         if $hotFile && $format eq "block" && !$model->getParameterByName("blockSize");
    SPL::CodeGen::exitln(SPL::Msg::STDTK_PARAMETER_INVALID_IN_CR("deleteFile"),$model->getContext()->getSourceLocation()) if $crContext && $deleteFile;
    my $hotFileValue = $hotFile ? $hotFile->getValueAt(0)->getSPLExpression() eq "true" : 0;
    SPL::CodeGen::exitln(SPL::Msg::STDTK_PARAMETER_INVALID_IN_CR("hotFile"),$model->getContext()->getSourceLocation()) if $crContext && $hotFileValue;
    SPL::CodeGen::exitln(SPL::Msg::STDTK_PARAMETER_INVALID_IN_CR("moveFileToDirectory"),$model->getContext()->getSourceLocation()) if $crContext && $moveFileToDirectory;
    SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESOURCE_WITH_INPUT_PORT_CANNOT_BE_CR_START(),$model->getContext()->getSourceLocation()) if $isStart && $model->getNumberOfInputPorts() != 0;
}

1;
