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

package V1TCPSinkCommon;
use strict;
use warnings;

use File::Basename;

sub verify($)
{
    my ($model) = @_;
    unshift @INC, dirname($model->getContext()->getOperatorDirectory()) . "/Common";
    require AdapterHelper;
    AdapterHelper::verifyValidV1Tuple($model->getInputPortAt(0));

    # specific checks
    my $address = $model->getParameterByName("address");
    my $port = $model->getParameterByName("port");
    my $role = $model->getParameterByName("role");
    my $reconnectionPolicy = $model->getParameterByName("reconnectionPolicy");
    my $reconnectionBound = $model->getParameterByName("reconnectionBound");

    $role = $role->getValueAt(0)->getSPLExpression() if defined $role;

    if (defined $port) {
        my $pt = $port->getValueAt(0)->getSPLType();
        SPL::CodeGen::exitln(SPL::Msg::STDTK_PORT_INVALID_TYPE($pt), $port->getSourceLocation())
            unless SPL::CodeGen::Type::isRString($pt) || SPL::CodeGen::Type::isUint32($pt);
    }

    # Must have a address/port for clients
    if ($role eq "client") {
        if (!defined $address || !defined $port) {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_NEITHER_ADDRESS_NOR_NAME(),
                               $model->getContext()->getSourceLocation());
        }
    }

    if ($role eq "server" && defined $address) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_SERVER_ADDRESS(),
                           $address->getSourceLocation());
    }

    # reconnectionBound is only allowed with role client and reconnectionPolicy Bounded
    if (defined $reconnectionBound) {
        if (!defined $reconnectionPolicy ||
                $reconnectionPolicy->getValueAt(0)->getSPLExpression() ne "BoundedRetry") {
            SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_INVALID_RECONNECT_BOUND(),
                               $reconnectionBound->getSourceLocation());
        }
    }

    # need a reconnectionBound with BoundedRetry
    if (defined $reconnectionPolicy && $reconnectionPolicy->getValueAt(0)->getSPLExpression() eq "BoundedRetry"
            && !defined $reconnectionBound) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_SOURCE_SINK_INVALID_BOUNDED_RECONNECT_NEEDS_BOUND(),
                           $reconnectionPolicy->getSourceLocation());
    }

}

1;
