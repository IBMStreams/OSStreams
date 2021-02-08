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

package BeaconCommon;
use strict;
use warnings;

sub verify($)
{
    my ($model) = @_;

    my $iterations = $model->getParameterByName("iterations");
    if ($iterations) {
        my $t = $iterations->getValueAt(0)->getSPLType();
        SPL::CodeGen::exitln(
                SPL::Msg::STDTK_PARAMETER_WRONG_TYPE('iterations', $t, 'uint32/int32'),
                $iterations->getSourceLocation())
            unless SPL::CodeGen::Type::isUint32($t) || SPL::CodeGen::Type::isInt32($t);
    }

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    my $triggerCount = $model->getParameterByName("triggerCount");
    if ($isInConsistentRegion) {
       my $isTriggerOperator = $isInConsistentRegion->isTriggerOperator();

       if ($isTriggerOperator && !$triggerCount) {
           SPL::CodeGen::exitln(SPL::Msg::STDTK_BEACON_TRIGGER_MUST_BE_SET(),
               $model->getContext()->getSourceLocation());
       }
       if ($triggerCount && !$isTriggerOperator) {
           SPL::CodeGen::exitln(SPL::Msg::STDTK_BEACON_TRIGGER_ONLY_IN_OPDRIVEN(),
               $triggerCount->getSourceLocation());
       }
    } elsif ($triggerCount) {
       SPL::CodeGen::exitln(SPL::Msg::STDTK_BEACON_TRIGGER_ONLY_IN_OPDRIVEN(),
           $model->getParameterByName("triggerCount")->getSourceLocation());
    }

}

1;
