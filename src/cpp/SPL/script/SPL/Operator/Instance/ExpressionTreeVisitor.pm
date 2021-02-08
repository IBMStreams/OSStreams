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

## @class SPL::Operator::Instance::ExpressionTreeVisitor
# \brief Class that visits each node in a ExpressionTree tree
#
# To use %ExpressionTreeVisitor, derive a class from %ExpressionTreeVisitor, and override the
# visit_NAME methods to perform actions.    You should invoke $self->SUPER::visit_NAME to ensure that the
# tree walker continues to visit all the nodes
package SPL::Operator::Instance::ExpressionTreeVisitor;

use strict;
use warnings;
use SPL::ModelHelper;

## @method void visit(ExpressionTree $expn)
# @param expn expression tree to visit
sub visit
{
    my ($self, $expn) = @_;
    my $eKind = $SPL::Operator::Instance::ExpressionTree::expressionKindMap->{$expn->getKind()};
    my $visit;
    if ($eKind eq 'Literal') {
        # Further specialization
        my $lKind = $SPL::Operator::Instance::ExpressionTree::literalKindMap->{$expn->getLiteralKind()};
        $visit = 'visit_' . $lKind . 'Literal';
    } else {
        $visit = 'visit_' . $eKind . 'Expression';
    }
    # call the right method
    $self->$visit ($expn);
}

## @method void visit_AttributeExpression(Attribute $expn)
# @param expn attribute expression being visited
sub visit_AttributeExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getLhs());
}

## @method void visit_BinaryExpression(SPL::Operator::Instance::ExpressionTree Binary $expn)
# @param expn binary expression being visited
sub visit_BinaryExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getLhs());
    $self->visit ($expn->getRhs());
}

## @method void visit_CallExpression(Call $expn)
# @param expn call expression being visited
sub visit_CallExpression
{
    my ($self, $expn) = @_;
    my $numArgs = $expn->getNumberOfArguments();
    for (my $i = 0; $i < $numArgs; $i++) {
        $self->visit ($expn->getArgumentAt($i));
    }
}

## @method void visit_CastExpression(Cast $expn)
# @param expn cast expression being visited
sub visit_CastExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getExpression());
}

## @method void visit_ConditionalExpression(Conditional $expn)
# @param expn conditional expression being visited
sub visit_ConditionalExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getCondition());
    $self->visit ($expn->getLhs());
    $self->visit ($expn->getRhs());
}

## @method void visit_EnumExpression(Enum $expn)
# @param expn cast expression being visited
sub visit_EnumExpression
{
    my ($self, $expn) = @_;
}

## @method void visit_PostfixExpression(Postfix $expn)
# @param expn postfix expression being visited
sub visit_PostfixExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getExpression());
}

## @method void visit_PrefixExpression(Prefix $expn)
# @param expn postfix expression being visited
sub visit_PrefixExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getExpression());
}

## @method void visit_StreamHistoryExpression(StreamHistory $expn)
# @param expn stream history expression being visited
sub visit_StreamHistoryExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getStream());
}

## @method void visit_StreamExpression(Stream $expn)
# @param expn stream expression being visited
sub visit_StreamExpression
{
    my ($self, $expn) = @_;
}

## @method void visit_SubscriptExpression(Subscript $expn)
# @param expn subscript expression being visited
sub visit_SubscriptExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getExpression());
    $self->visit ($expn->getLower()) if ($expn->getLower());
    $self->visit ($expn->getUpper()) if ($expn->getUpper());
}

## @method void visit_SymbolExpression(Symbol $expn)
# @param expn symbol expression being visited
sub visit_SymbolExpression
{
    my ($self, $expn) = @_;
}

## @method void visit_UnaryExpression(Unary $expn)
# @param expn unary expression being visited
sub visit_UnaryExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getExpression());
}

## @method void visit_UnwrapExpression(Unwrap $expn)
# @param expn unwrap expression being visited
sub visit_UnwrapExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getExpression());
}

## @method void visit_UnwrapOrElseExpression(UnwrapOrElse $expn)
# @param expn unwrapOrElse expression being visited
sub visit_UnwrapOrElseExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getLhs());
    $self->visit ($expn->getRhs());
}

## @method void visit_IsPresentExpression(IsPresent $expn)
# @param expn isPresent expression being visited
sub visit_IsPresentExpression
{
    my ($self, $expn) = @_;
    $self->visit ($expn->getExpression());
}

## @method void visit_PrimitiveLiteral(PrimitiveLiteral $lit)
# @param lit primitive (scalar) literal being visited
sub visit_PrimitiveLiteral
{
    my ($self, $lit) = @_;
}

## @method void visit_SetLiteral(SetLiteral $lit)
# @param lit set literal being visited
sub visit_SetLiteral
{
    my ($self, $lit) = @_;
    my $numElements = $lit->getNumberOfElements();
    for (my $i = 0; $i < $numElements; $i++) {
        $self->visit ($lit->getLiteralAt($i));
    }
}

## @method void visit_ListLiteral(ListLiteral $lit)
# @param lit list literal being visited
sub visit_ListLiteral
{
    my ($self, $lit) = @_;
    my $numElements = $lit->getNumberOfElements();
    for (my $i = 0; $i < $numElements; $i++) {
        $self->visit ($lit->getLiteralAt($i));
    }
}

## @method void visit_ExpressionLiteral(ExpressionLiteral $lit)
# @param lit expression literal being visited
sub visit_ExpressionLiteral
{
    my ($self, $lit) = @_;
    $self->visit ($lit->getExpression());
}

## @method void visit_MapLiteral(MapLiteral $lit)
# @param lit map literal being visited
sub visit_MapLiteral
{
    my ($self, $lit) = @_;
    my $numElements = $lit->getNumberOfElements();
    for (my $i = 0; $i < $numElements; $i++) {
        $self->visit ($lit->getKeyAt($i));
        $self->visit ($lit->getValueAt($i));
    }
}

## @method void visit_TupleLiteral(TupleLiteral $lit)
# @param lit tuple literal being visited
sub visit_TupleLiteral
{
    my ($self, $lit) = @_;
    my $numElements = $lit->getNumberOfElements();
    for (my $i = 0; $i < $numElements; $i++) {
        # my $id = $lit->getAttributeAt($i)
        $self->visit ($lit->getLiteralAt($i));
    }
}

# DOXYGEN_SKIP_FOR_USERS
sub new {
  my ($class) = @_;

  $class = ref($class) if ref($class);
  my $self = {};
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS
