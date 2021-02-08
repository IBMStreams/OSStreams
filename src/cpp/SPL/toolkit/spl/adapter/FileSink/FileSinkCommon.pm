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

package FileSinkCommon;
use strict;
use warnings;

use File::Basename;

sub checkCloseMode ($$$$) {
    my ($model, $mode, $check, $needed) = @_;

    my $p = $model->getParameterByName($needed);
    if (!$mode) {
        SPL::CodeGen::exitln (SPL::Msg::STDTK_FILESINK_CLOSEMODE_UNNEEDED_PARAM($needed, 'never'),
            $p->getSourceLocation()) if $p;
        return;
    }

    my $origMode = $mode;
    $mode = $mode->getValueAt(0)->getCppExpression();
    if ($mode eq $check) {
        SPL::CodeGen::exitln (SPL::Msg::STDTK_FILESINK_CLOSEMODE_NEEDS_PARAM($mode, $needed),
            $origMode->getSourceLocation()) if !$p;
        if ($mode ne "time") {
            # need to check the type as well
            my $typ = $p->getValueAt(0)->getSPLType();
            SPL::CodeGen::exitln (SPL::Msg::STDTK_FILESINK_PARAM_TYPE($needed, $typ),
                $p->getSourceLocation())
            if !(SPL::CodeGen::Type::isUint32($typ) ||
                SPL::CodeGen::Type::isUint64($typ));
        }
    } else {
        SPL::CodeGen::exitln (SPL::Msg::STDTK_FILESINK_CLOSEMODE_UNNEEDED_PARAM($needed, $mode),
            $p->getSourceLocation()) if $p;
    }
}

sub verify($)
{
    my ($model) = @_;
    unshift @INC, dirname($model->getContext()->getOperatorDirectory()) . "/Common";
    require AdapterHelper;
    AdapterHelper::verifySinkModel($model);

    # Check the output port, if present
    if ($model->getNumberOfOutputPorts() == 1) {
        my $outputPort = $model->getOutputPortAt(0);
        my $tupleType = $outputPort->getCppTupleType();

        SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESINK_OUTPUT_NOT_RSTRING(),
            $outputPort->getSourceLocation())
        if $outputPort->getNumberOfAttributes() != 1 ||
        !SPL::CodeGen::Type::isRString($outputPort->getAttributeAt(0)->getSPLType());
    }

    # check closeMode
    my $closeMode = $model->getParameterByName("closeMode");

    checkCloseMode ($model, $closeMode, "count", "tuplesPerFile");
    checkCloseMode ($model, $closeMode, "time", "timePerFile");
    checkCloseMode ($model, $closeMode, "size", "bytesPerFile");

    $closeMode = $closeMode ? $closeMode->getValueAt(0)->getCppExpression() : "never";

    # Consistent region checks
    my $writeStateHandlerCallbacks = $model->getParameterByName("writeStateHandlerCallbacks");
    $writeStateHandlerCallbacks = $writeStateHandlerCallbacks ? ($writeStateHandlerCallbacks->getValueAt(0)->getSPLExpression() eq "true") : 0;
    if ($writeStateHandlerCallbacks) {
      my $format = $model->getParameterByName("format");
      $format = $format ? $format->getValueAt(0)->getSPLExpression() : "csv";
      if ($format ne "csv") {
          SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESINK_WRITESTATEHANDLER_ILLEGAL_FORMAT($format),
              $model->getParameterByName("writeStateHandlerCallbacks")->getSourceLocation());
      }
    }

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    my $truncateOnReset = $model->getParameterByName("truncateOnReset");
    if ($truncateOnReset && !$isInConsistentRegion) {
      if ($truncateOnReset->getValueAt(0)->getSPLExpression eq "true") {
          SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESINK_TRUNCATEONRESET_OUTSIDE_CR(),
              $truncateOnReset->getSourceLocation());
      }
    }

    if ($isInConsistentRegion) {
        # truncateOnReset true by default
        $truncateOnReset = $truncateOnReset ? ($truncateOnReset->getValueAt(0)->getSPLExpression eq "true") : 1;

        my $append = $model->getParameterByName("append");
        if ($append) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_PARAMETER_INVALID_IN_CR("append"),
                $append->getSourceLocation());
        }

        my $compression = $model->getParameterByName("compression");
        if ($compression) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_PARAMETER_INVALID_IN_CR("compression"),
                $compression->getSourceLocation());
        }
        if ($closeMode ne "never") {
            if ($truncateOnReset && ($closeMode eq "dynamic" || $closeMode eq "time")) {
                # Cannot guarantee 'dynamic' as multiple files can be modified between cuts, and we only record one cutOffset.
                SPL::CodeGen::exitln(SPL::Msg::STDTK_FILESINK_TRUNCATEONRESET_ILLEGAL_CLOSEMODE("'$closeMode'"),
                    $model->getParameterByName("closeMode")->getSourceLocation());
            }
        }

        if ($isInConsistentRegion->isStartOfRegion()) {
            SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
                    $model->getContext()->getSourceLocation());
        }
    }

}

1;
