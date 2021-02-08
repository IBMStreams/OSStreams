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

package AggregateCommon;
use strict;
use warnings;

sub verify($)
{
    my ($model) = @_;
    my $inputPort = $model->getInputPortAt(0);
    my $outputPort = $model->getOutputPortAt(0);
    my $inTupleType = $inputPort->getCppTupleType();
    my $outTupleType = $outputPort->getCppTupleType();
    my $window = $inputPort->getWindow();


    my $partitionByParam = $model->getParameterByName("partitionBy");
    my $isPartitioned = $window->isPartitioned();
    if($partitionByParam) {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_MISSING_PARTITIONED_WINDOW_MODIFIER("Aggregate", "partitionBy"),
                           $partitionByParam->getSourceLocation())
            if (not $isPartitioned);
    } else {
        SPL::CodeGen::exitln(SPL::Msg::STDTK_MISSING_PARTITION_BY_PARAMETER("Aggregate", "partitionBy"),
                           $window->getSourceLocation())
            if ($isPartitioned);
    }

    my $aggregateIncompleteWindows = $model->getParameterByName("aggregateIncompleteWindows");
    my $incompleteValue  = $aggregateIncompleteWindows ?
            $aggregateIncompleteWindows->getValueAt(0)->getSPLExpression() : "false";
    if ($incompleteValue eq "true" && $window->isTumbling()) {
        $aggregateIncompleteWindows = $model->getParameterByName("aggregateIncompleteWindows");
        SPL::CodeGen::exitln(SPL::Msg::STDTK_AGGREGATE_TUMBLING_AND_INCOMPLETE(),
                   $aggregateIncompleteWindows->getSourceLocation());
    }

    my $aggregateEvictedPartitions = $model->getParameterByName("aggregateEvictedPartitions");
    $incompleteValue  = $aggregateEvictedPartitions ?
            $aggregateEvictedPartitions->getValueAt(0)->getSPLExpression() : "false";
    if ($incompleteValue eq "true" && !$window->isTumbling()) {
        $aggregateEvictedPartitions = $model->getParameterByName("aggregateEvictedPartitions");
        SPL::CodeGen::exitln(SPL::Msg::STDTK_AGGREGATE_SLIDING_AND_EVICT(),
                   $aggregateEvictedPartitions->getSourceLocation());
    }

    my $hasEvictionPolicy = $window->isPartitioned() and $window->hasPartitionEvictionPolicy();
    if ($aggregateEvictedPartitions && !$hasEvictionPolicy) {
        $aggregateEvictedPartitions = $model->getParameterByName("aggregateEvictedPartitions");
        SPL::CodeGen::exitln(SPL::Msg::STDTK_AGGREGATE_NO_PARTITION_AND_EVICT(),
                   $aggregateEvictedPartitions->getSourceLocation());
    }

    # Check that Custom arguments are calls
    for (my $i = 0; $i < $outputPort->getNumberOfAttributes(); $i++) {
        my $attr = $outputPort->getAttributeAt($i);
        next if !$attr->hasAssignmentWithOutputFunction() ||
                 $attr->getAssignmentOutputFunctionName() ne 'Custom';

        for (my $j = 0; $j < 3; $j++) {
            my $expn = $attr->getAssignmentOutputFunctionParameterValueAt($j);
            my $splExpn = $expn->getSPLExpressionTree();
            SPL::CodeGen::errorln(
                SPL::Msg::STDTK_AGGREGATE_CUSTOM_PARAMETER_NOT_CALL($j, $splExpn->toString()),
                                   $expn->getSourceLocation()) if !$splExpn->isCall();
            SPL::CodeGen::errorln(
                SPL::Msg::STDTK_AGGREGATE_CUSTOM_PARAMETER_NOT_ATTRIBUTE_FREE($j, $splExpn->toString()),
                                   $expn->getSourceLocation()) if $j != 1 && $expn->hasStreamAttributes();
        }
    }

}

sub hasCustom($)
{
    my ($outputPort) = @_;
    for (my $i = 0; $i < $outputPort->getNumberOfAttributes(); $i++) {
        my $attr = $outputPort->getAttributeAt($i);
        return 1 if $attr->hasAssignmentWithOutputFunction() &&
                    $attr->getAssignmentOutputFunctionName() eq 'Custom';
    }
    return 0;
}

sub scalarAggregate ($)
{
    my ($attr) = @_;
    my $agg = $attr->getAssignmentOutputFunctionName();
    return 1 if $agg eq "Any" ||  $agg eq "First" || $agg eq "Last";
    return 0 if $agg eq "Custom";
    my $paramValues = $attr->getAssignmentOutputFunctionParameterValues();
    return defined ($paramValues->[0]) ? 0 : 1;
}

sub checkSideEffects($)
{
  my ($exprClass) = @_;

  if ($exprClass->hasSideEffects()) {
    SPL::CodeGen::errorln(
                SPL::Msg::STDTK_AGGREGATE_SIDEEFFECTS($exprClass->getSPLExpression),
                $exprClass->getSourceLocation());
  }
}

sub insertExpr($$;$)
{
  my ($dbref,$exprClass,$choke) = @_;

  $choke = 0 unless defined($choke);

  my $expr = $exprClass->getCppExpression();
  my $type = $exprClass->getCppType();

  # check for side effects
  checkSideEffects($exprClass) if $choke;

  $dbref->{$expr} = $type if $exprClass->hasStreamAttributes();
}

# collect all the expressions: (1) from the group-by's, (2) from output, (3) delta attributes
sub collectExprs($)
{
  my ($model) = @_;
  my $groupByParam = $model->getParameterByName("groupBy");
  my $outputPort = $model->getOutputPortAt(0);
  my $inputPort = $model->getInputPortAt(0);
  my $window = $inputPort->getWindow();

  ## collected expressions
  my %allExprs = ();

  ## collect expressions from the group-by's
  if ($groupByParam) {
    for (my $i=0;$i<$groupByParam->getNumberOfValues();$i++) {
      insertExpr(\%allExprs, $groupByParam->getValueAt($i), 1);
    }
  }

  ## collect expressions related to output:
  # for each output port tuple attribute 'attr' {
  #   if (attr has assignment with output fn) {
  #     allExprs += param value(s)
  #   }
  # }
  for (my $i = 0; $i < $outputPort->getNumberOfAttributes(); $i++) {
    my $attr = $outputPort->getAttributeAt($i);
    next if !$attr->hasAssignmentWithOutputFunction();

    my $paramValues = $attr->getAssignmentOutputFunctionParameterValues();
    insertExpr(\%allExprs, $paramValues->[0], 1) if defined ($paramValues->[0]);
    insertExpr(\%allExprs, $paramValues->[1], 1) if (scalar(@{$paramValues}) == 2);
  }

  ## delta attributes
  # eviction
  insertExpr(\%allExprs, $window->getEvictionPolicyAttribute()) if ($window->hasEvictionPolicy() && $window->getEvictionPolicyType()==$SPL::Operator::Instance::Window::DELTA);

  # trigger
  insertExpr(\%allExprs, $window->getTriggerPolicyAttribute()) if ($window->hasTriggerPolicy() && $window->getTriggerPolicyType()==$SPL::Operator::Instance::Window::DELTA);

  return %allExprs;
}

sub numberExprs($)
{
  my $param = shift;
  my %allExprs = %$param;
  my @exprs;
  my @types;
  foreach my $key (sort keys %allExprs) {
    push (@exprs, $key);
    push (@types, $allExprs{$key});
  }
  return \@exprs, \@types;
}

sub getCppParams($)
{
  my ($param) = @_;
  return undef if not defined ($param);
  my @res;
  for (my $i = 0; $i < $param->getNumberOfValues(); $i++) {
    my $value = $param->getValueAt($i);
    if ($value) {
      push(@res, $value->getCppExpression());
    }
  }
  return \@res;
}

sub translateExprs($$$;$)
{
  my ($name, $dbref, $exprsref,$derefby) = @_;
  my %db = %$dbref;
  if (defined($exprsref)) {
    my @exprs = @$exprsref;
    return "" unless (scalar(@exprs) > 0);
    $derefby = "." unless defined ($derefby);

    return join(", ", map {
        my $e = $_;
        if (defined($db{$e})) {
          $name.$derefby."get_".$db{$e}{name}."()"
        }
        else {
          $e;
        }
      } @exprs);
  }
  else {
    return "";
  }
}

sub getWindowCppInitializer($$)
{
  my ($model,$dbref) = @_;
  my $inputPort = $model->getInputPortAt(0);
  my $window = $inputPort->getWindow();

  # eviction
  my $getterEvict = ($window->getEvictionPolicyType()==$SPL::Operator::Instance::Window::DELTA) ?
     ("get_".$dbref->{$window->getEvictionPolicyAttribute()->getCppExpression()}{name}) : "";

  # trigger
  my $getterTrigger = ($window->getTriggerPolicyType()==$SPL::Operator::Instance::Window::DELTA) ?
     ("get_".$dbref->{$window->getTriggerPolicyAttribute()->getCppExpression()}{name}) : "";

  return $getterEvict, $getterTrigger;
}

1;
