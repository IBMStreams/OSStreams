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

## @class SPL::Operator::Instance::ExpressionTreeEvaluator
# \brief Class that evaluates an expression tree and returns a compile-time usable expression, or undef
#
# DOXYGEN_SKIP_FOR_USERS

package SPL::Operator::Instance::ExpressionTreeEvaluator;

use strict;
use warnings;
use SPL::ModelHelper;

## @method void evaluate(ExpressionTree $expn)
#  @param expn expression tree to evaluate
sub evaluateExpression
{
    my ($self, $expn) = @_;
    my $evaluatedExpr = $self->evaluate($expn);
    return undef unless defined($evaluatedExpr);
    return "\"" . $evaluatedExpr . "\"";
}

sub evaluate
{
    my ($self, $expn) = @_;

    my $eKind = $SPL::Operator::Instance::ExpressionTree::expressionKindMap->{$expn->getKind()};
    my $evaluate;
    if ($eKind eq 'Literal') {
        # Further specialization
        my $lKind = $SPL::Operator::Instance::ExpressionTree::literalKindMap->{$expn->getLiteralKind()};
        $evaluate = 'evaluate_' . $lKind . 'Literal';
    } else {
        $evaluate = 'evaluate_' . $eKind . 'Expression';
    }
    # call the right method
    my $result = $self->$evaluate ($expn);
    return $result;
}

sub evaluate_AttributeExpression
{
    return undef;
}

sub evaluate_BinaryExpression
{
    my ($self, $expn) = @_;

    my $lhs = $expn->getLhs();
    my $lhsType = $lhs->getType();

    my $rhs = $expn->getRhs();
    my $rhsType = $rhs->getType();

    my $op = $expn->getOp();

    my $lhsResult = $self->evaluate($lhs);
    my $rhsResult = $self->evaluate($rhs);
    return undef unless (defined($lhsResult) && defined($rhsResult));;
    if ($op eq '+') {
        return "${lhsResult}${rhsResult}";
    }
    return undef;
}

sub evaluate_CallExpression
{
    my ($self, $expn) = @_;
    return undef unless ($expn->getType eq 'rstring');

    my $functionName = $expn->getFunctionName();
    if ($functionName eq 'getToolkitDirectory') {
        my $toolkitName = $self->evaluate ($expn->getArgumentAt(0));
        my $context = $self->{model}->getContext();
        my $toolkits = $context->getToolkits();
        foreach my $toolkit (@{$toolkits}) {
            if ($toolkit->getName() eq $toolkitName) {
                return $toolkit->getRootDirectory();
            }
        }
    }
    return undef;
}

sub evaluate_CastExpression
{
    my ($self, $expn) = @_;
    return $self->evaluate($expn->getExpression());
}

sub evaluate_ConditionalExpression
{
    return undef;
}

sub evaluate_EnumExpression
{
    return undef;
}

sub evaluate_PostfixExpression
{
    return undef;
}

sub evaluate_PrefixExpression
{
    return undef;
}

sub evaluate_StreamHistoryExpression
{
    return undef;
}

sub evaluate_StreamExpression
{
    return undef;
}

sub evaluate_SubscriptExpression
{
    return undef;
}

sub evaluate_SymbolExpression
{
    return undef;
}

sub evaluate_UnaryExpression
{
    return undef;
}

sub evaluate_PrimitiveLiteral
{
    my ($self, $lit) = @_;
    my $type = $lit->getType();
    my $value = $lit->getValue();
    if ($type eq 'rstring') {
        $value = substr($value, 1, length($value)-2);
    }
    return $value;
}

sub evaluate_SetLiteral
{
    return undef;
}

sub evaluate_ListLiteral
{
    return undef;
}

sub evaluate_ExpressionLiteral
{
    return undef;
}

sub evaluate_MapLiteral
{
    return undef;
}

sub evaluate_TupleLiteral
{
    return undef;
}

sub new {
  my ($class, $model) = @_;

  $class = ref($class) if ref($class);
  my $self = {};
  $self->{model} = $model;
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS
