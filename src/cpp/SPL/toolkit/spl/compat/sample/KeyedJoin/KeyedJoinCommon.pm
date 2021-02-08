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

package KeyedJoinCommon;
sub verify ($)
{
    my ($model) = @_;
    my $replacementKey1 = $model->getParameterByName("ReplacementKey1");
    my $replacementKey2 = $model->getParameterByName("ReplacementKey2");
    my $equalityCondition1 = $model->getParameterByName("EqualityCondition1");
    my $equalityCondition2 = $model->getParameterByName("EqualityCondition2");

    # Check that the types and numbers match
    SPL::CodeGen::exitln(
            "'EqualityCondition1' and 'EqualityCondition2' must have the same number of expressions",
            $equalityCondition1->getSourceLocation())
        if $equalityCondition1->getNumberOfValues() != $equalityCondition2->getNumberOfValues();
    for (my $i = 0; $i < $equalityCondition1->getNumberOfValues(); $i++) {
        SPL::CodeGen::exitln("Expression type mismatch in 'EqualityCondition1' and 'EqualityCondition2' parameter lists: ('$lT' and '$rT')",
                $lT->getSourceLocation()) if $lT ne $rT;

    }

    SPL::CodeGen::exitln(
            "'ReplacementKey1' and 'ReplacementKey2' must have the same number of expressions",
            $equalityCondition1->getSourceLocation())
        if $equalityCondition1->getNumberOfValues() != $equalityCondition2->getNumberOfValues();
    for (my $i = 0; $i < $equalityCondition1->getNumberOfValues(); $i++) {
        my $lT = $equalityCondition1->getValueAt($i)->getSPLType();
        my $rT = $equalityCondition2->getValueAt($i)->getSPLType();
        SPL::CodeGen::exitln("Expression type mismatch in 'ReplacementKey1' and 'ReplacementKey2' parameter lists: ('$lT' and '$rT')",
                $Tl->getSourceLocation()) if $lT ne $rT;

    }

}

1;
