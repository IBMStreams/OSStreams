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

package UnionCommon;
sub verify ($)
{
    my ($model) = @_;
    my $outputPort = $model->getOutputPortAt(0);
    my $outTupleType = $outputPort->getCppTupleType();
    my $numInputPorts = $model->getNumberOfInputPorts();
    my $numAttributes = $outputPort->getNumberOfAttributes();

    # check that all the attributes match
    for (my $i = 0; $i < $numInputPorts; $i++) {
        my $inputPort = $model->getInputPortAt($i);
        my $inTupleType = $inputPort->getCppTupleType();
        my $inTupleName = $inputPort->getCppTupleName();

        # easy case
        next if $inTupleName eq $outTupleType;

        # ensure each attribute in the output is found in the input
        for (my $j = 0; $j < $numAttributes; $j++) {
            my $attr = $outputPort->getAttributeAt($j);
            # Walk through input port to find a match
            my $found = 0;
            for ($k = 0; $k < $inputPort->getNumberOfAttributes(); $k++) {
                my $inAttr = $inputPort->getAttributeAt($k);
                if ($attr->getName() eq $inAttr->getName()) {
                    # check the type
                    if ($attr->getSPLType() eq $inAttr->getSPLType()) {
                        $found = 1;
                        last;
                    }
                    # oops
                    SPL::CodeGen::exitln(SPL::Msg::STDTK_UNION_TYPE_MISMATCH (
                                            $attr->getName(), $i, $attr->getSPLType(), $inAttr->getSPLType()),
                                        $inputPort->getSourceLocation());
                }
            }
            SPL::CodeGen::exitln(SPL::Msg::STDTK_UNION_MISSING_ATTRIBUTE ($attr->getName(), $i),
                                $inputPort->getSourceLocation()) if !$found;
        }
    }

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($isInConsistentRegion && $isInConsistentRegion->isStartOfRegion()) {
        SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
            $model->getContext()->getSourceLocation());
    }

}

1;
