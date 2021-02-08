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

package DirectoryScanCommon;
use strict;
use warnings;

use File::Basename;

sub verify($)
{
    my ($model) = @_;

    # Any attribute that doesn't have an output function must be of type rstring,
    # and will get the FullName
    my $outputPort = $model->getOutputPortAt(0);
    for (my $i = 0; $i < $outputPort->getNumberOfAttributes(); $i++) {
        my $attr = $outputPort->getAttributeAt($i);
        if (!$attr->hasAssignmentWithOutputFunction()) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_DIRSCAN_OUTPUT_NOT_RSTRING($attr->getName()),
                                 $outputPort->getSourceLocation())
                    if !SPL::CodeGen::Type::isRString($attr->getSPLType());
        }
    }

    # moveToDirectory not allowed in consistent region with periodic trigger
    my $moveToDirectory = $model->getParameterByName("moveToDirectory");
    my $crContext = $model->getContext()->getOptionalContext("ConsistentRegion");
    my $isInConsistentRegion = $crContext ? 1 : 0;
    my $isTrigger = $crContext ? $crContext->isTriggerOperator() : 0;

    SPL::CodeGen::exitln(SPL::Msg::STDTK_DIRECTORYSCAN_MOVE_IN_PERIODIC_REGION(),$model->getContext()->getSourceLocation()) if $crContext && $moveToDirectory && !$isTrigger;

}

1;
