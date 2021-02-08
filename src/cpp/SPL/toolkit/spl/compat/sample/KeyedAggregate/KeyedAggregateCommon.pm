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

package KeyedAggregateCommon;
sub verify ($)
{
    my ($model) = @_;
    my $replacementKey = $model->getParameterByName("ReplacementKey");
    my $aggregationKey = $model->getParameterByName("AggregationKey");
    my %replacements;
    my %aggregates;

    # check for overlap between sets of attributes
    for (my $i = 0; $i < $replacementKey->getNumberOfValues(); $i++) {
        my $name = $replacementKey->getValueAt($i)->getSPLExpression();
        $name =~ s/.*\.//;
        $replacements{$name} = 1;
    }
    for (my $i = 0; $i < $aggregationKey->getNumberOfValues(); $i++) {
        my $name = $aggregationKey->getValueAt($i)->getSPLExpression();
        $name =~ s/.*\.//;
        if ($replacements{$name} == 1) {
            SPL::CodeGen::exitln ("'$name' may not be in both 'ReplacementKey' and 'AggregationKey'",
                $aggregationKey->getValueAt($i)->getSourceLocation());
            $aggregates{$name} = 1;
        }
    }

    # Check that input and output schemas match
    my $inputPort = $model->getInputPortAt(0);
    my $outputPort = $model->getOutputPortAt(0);
    foreach my $attr (@{$outputPort->getAttributes()}) {
        my $name = $attr->getName();

        # check the aggregate kind
        next if !$attr->hasAssignmentWithOutputFunction();
        my $aggr = $attr->getAssignmentOutputFunctionName();
        if ($aggregates{$name}) {
            SPL::CodeGen::exitln ("Output attribute '$name' must use aggregation 'Any'",
                $outputPort->getSourceLocation()) if $aggr ne "Any";
        } elsif ($replacements{$name}) {
            SPL::CodeGen::exitln ("Aggregation not supported on replacement key attribute '$name'",
                $outputPort->getSourceLocation()) if $aggr ne "None";
        }
    }
}

1;
