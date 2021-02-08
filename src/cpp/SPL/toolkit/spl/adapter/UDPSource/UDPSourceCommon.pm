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

package UDPSourceCommon;
use strict;
use warnings;

use File::Basename;

sub verify($)
{
    my ($model) = @_;
    unshift @INC, dirname($model->getContext()->getOperatorDirectory()) . "/Common";
    require AdapterHelper;
    AdapterHelper::verifySourceModel($model);

    # specific checks
    my $address = $model->getParameterByName("address");
    my $interface = $model->getParameterByName("interface");
    my $port = $model->getParameterByName("port");
    my $name = $model->getParameterByName("name");

    if ($port) {
        my $pt = $port->getValueAt(0)->getSPLType();
        SPL::CodeGen::exitln(SPL::Msg::STDTK_PORT_INVALID_TYPE($pt), $port->getSourceLocation())
            unless SPL::CodeGen::Type::isRString($pt) || SPL::CodeGen::Type::isUint32($pt);
    }

    # Must have a port or name
    if (!$name && !$port) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_NEITHER_PORT_NOR_NAME("UDPSource"), $model->getContext()->getSourceLocation());
    }

    # can't have address and name
    if ($address && $name) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_ADDRESS_AND_NAME(),
                           $address->getSourceLocation());
    }

    # can't have interface without address or name
    if ($interface && !($address || $name)) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_INTERFACE_NO_ADDRESS_OR_NAME(),
                           $interface->getSourceLocation());
    }

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($isInConsistentRegion) {
      SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_LIVE_IN_CC_REGION($model->getContext()->getKind()),
          $model->getContext()->getSourceLocation());
    }
}

1;
