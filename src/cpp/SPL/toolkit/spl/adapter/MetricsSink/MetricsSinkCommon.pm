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

package MetricsSinkCommon;
use strict;
use warnings;

sub verify($)
{
    my ($model) = @_;
    my $metrics = $model->getParameterByName("metrics");
    my $names = $model->getParameterByName("names");
    my $descriptions = $model->getParameterByName("descriptions");
    my $initialValues = $model->getParameterByName("initialValues");

    my $numValues = $metrics->getNumberOfValues();

    # check that all the number of expressions match
    SPL::CodeGen::exitln(SPL::Msg::STDTK_INVALID_NUMBER_EXPNS("metrics", "names"),
                                   $names->getSourceLocation())
                               if $names && $names->getNumberOfValues() != $numValues;

    SPL::CodeGen::exitln(SPL::Msg::STDTK_INVALID_NUMBER_EXPNS("metrics", "descriptions"),
                                   $descriptions->getSourceLocation())
                               if $descriptions && $descriptions->getNumberOfValues() != $numValues;

    SPL::CodeGen::exitln(SPL::Msg::STDTK_INVALID_NUMBER_EXPNS("metrics", "initialValues"),
                                   $initialValues->getSourceLocation())
                               if $initialValues && $initialValues->getNumberOfValues() != $numValues;

    my $isInConsistentRegion = $model->getContext()->getOptionalContext("ConsistentRegion");
    if ($isInConsistentRegion && $isInConsistentRegion->isStartOfRegion()) {
        SPL::CodeGen::exitln(SPL::Msg::OP_CANNOT_START_CR($model->getContext()->getKind()),
            $model->getContext()->getSourceLocation());
    }
}

1;
