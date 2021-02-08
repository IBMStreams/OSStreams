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

package AdapterHelper;

use strict;
use warnings;
use SPL::CodeGen;

## @fn void verifyValidV1Tuple(@attributes)
# This function will do checks that this tuple is representable in V1 Streams
# @param port Input/Output port
# @param attribute Tuple attributes
# @param t SPL type
sub verifyScalarType($$$)
{
    my ($port, $attr, $t) = @_;

    # All intXs are supported
    return if SPL::CodeGen::Type::isSigned($t);

    # boolean is supported
    return if SPL::CodeGen::Type::isBoolean($t);

    # rstring is supported
    return if SPL::CodeGen::Type::isRString($t);

    # float32/float64 are supported
    return if SPL::CodeGen::Type::isFloat32($t) || SPL::CodeGen::Type::isFloat64($t);

    # That's all that the types that are supported.   Complain bitterly.
    SPL::CodeGen::errorln(
        SPL::Msg::STDTK_SOURCE_SINK_INVALID_V1_ATTRIBUTE($attr->getSPLType(), $attr->getName()),
        $port->getSourceLocation());
}

## @fn void verifyValidV1Tuple(@attributes)
# This function will do checks that this tuple is representable in V1 Streams
# @param port Input/Output port;
# @param attributes Input/Output Tuple attributes
sub verifyValidV1Tuple($)
{
    my ($port) = @_;

    foreach my $attr (@{$port->getAttributes()}) {
        my $t = $attr->getSPLType();
        if (SPL::CodeGen::Type::isList($t)) {
            $t = SPL::CodeGen::Type::getElementType($t);
        }
        verifyScalarType ($port, $attr, $t);
    }
    CORE::exit $SPL::CodeGen::USER_ERROR if ($SPL::CodeGen::sawError);
}

1;
