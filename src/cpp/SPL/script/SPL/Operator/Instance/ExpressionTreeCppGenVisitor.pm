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

## @class SPL::Operator::Instance::ExpressionTreeCppGenVisitor
# \brief Class that visits each node in a ExpressionTree tree, collecting the C++ code for the
# expression.
#
# To use %ExpressionTreeCppGenVisitor, the SPL expression tree must have been created with
# \<SPLExpressionTreeMode cppCode="true"\>.
# Create an instance of the class
# ($c = %SPL::Operator::Instance::ExpressionTreeCppGenVisitor->new())
# Visit the expression tree ($c->visit ($splExpn))
# Then $c->getCppCode() will return the C++ code for the SPL expression tree.
#
# If you wish to generate different C++ code for a given expression, you should derive from
# %ExpressionTreeCppGenVisitor, and define one or more visit_NAME methods.
# These methods should call $self->append ("string") to replace the tree with the string, or they
# should invoke $self->SUPER::visit_NAME to use the provided C++ code.
package SPL::Operator::Instance::ExpressionTreeCppGenVisitor;
use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTreeVisitor);

## @method string getCppCode()
# @return the C++ code corresponding to the SPL expression tree
sub getCppCode()
{
    my ($self) = @_;
    return $self->{code_};
}

## @method void append (string $str)
# @param str string to be appended to C++ code being generated
sub append ($$)
{
    my ($self, $str) = @_;
    $self->{code_} .= $str;
}

# DOXYGEN_SKIP_FOR_USERS
sub gen ($$)
{
    my ($self, $expn) = @_;
    my $cppCode = $expn->getCppCode();
    my $len = length ($cppCode);
    my $i = 0;
    while ($i < $len) {
        my $c = substr $cppCode, $i, 1;
        if ($c eq '{') {
            my $rest = substr ($cppCode, $i);
            if ($rest =~ /^\{([a-zA-Z]+)\}/) {
                $i += length ($&);
                my $name = $1;
                my $fcn = "get$name";
                my $subExpn = $self->new();
                $subExpn->visit($expn->$fcn());
                $self->append($subExpn->getCppCode());
                next;
            }
        }
        $self->append($c);
        $i++;
    }
}

sub visit_AttributeExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_BinaryExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_CallExpression
{
    my ($self, $expn) = @_;
    my $cppCode = $expn->getCppCode();
    my $len = length ($cppCode);
    my $i = 0;
    while ($i < $len) {
        my $c = substr $cppCode, $i, 1;
        if ($c eq '{') {
            my $rest = substr ($cppCode, $i);
            if ($rest =~ /^\{arg:(\d+)\}/) {
                $i += length ($&);
                my $arg = $1;
                my $subExpn = $self->new();
                $subExpn->visit($expn->getArgumentAt ($arg));
                $self->append($subExpn->getCppCode());
                next;
            }
            if ($rest =~ /^\{fcnName\?([^}]+)\}/) {
                $i += length ($&);
                my $fcnName = $1;
                $self->append($fcnName);
                next;
            }
        }
        $self->append($c);
        $i++;
    }
}

sub visit_CastExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_ConditionalExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_EnumExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_IsPresentExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_PostfixExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_PrefixExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_StreamHistoryExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_StreamExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_SubscriptExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_SymbolExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_UnaryExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_UnwrapExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_UnwrapOrElseExpression
{
    my ($self, $expn) = @_;
    $self->gen($expn);
}

sub visit_PrimitiveLiteral
{
    my ($self, $lit) = @_;
    $self->append($lit->getCppCode());
}

sub genSetListLit ($$)
{
    my ($self, $lit) = @_;
    my $cppCode = $lit->getCppCode();
    my $len = length ($cppCode);
    my $i = 0;
    while ($i < $len) {
        my $c = substr $cppCode, $i, 1;
        if ($c eq '{') {
            my $rest = substr ($cppCode, $i);
            if ($rest =~ /^\{value:(\d+)\}/) {
                $i += length ($&);
                my $arg = $1;
                my $subExpn = $self->new();
                $subExpn->visit($lit->getLiteralAt($arg));
                $self->append($subExpn->getCppCode());
                next;
            }
        }
        $self->append($c);
        $i++;
    }
}

sub visit_SetLiteral
{
    my ($self, $lit) = @_;
    $self->genSetListLit($lit);
}

sub visit_ListLiteral
{
    my ($self, $lit) = @_;
    $self->genSetListLit($lit);
}

sub visit_ExpressionLiteral
{
    my ($self, $lit) = @_;
    $self->visit($lit->getExpression());
}

sub visit_MapLiteral
{
    my ($self, $lit) = @_;
    my $cppCode = $lit->getCppCode();
    my $len = length ($cppCode);
    my $i = 0;
    while ($i < $len) {
        my $c = substr $cppCode, $i, 1;
        if ($c eq '{') {
            my $rest = substr ($cppCode, $i);
            if ($rest =~ /^\{((value)|(key)):(\d+)\}/) {
                $i += length ($&);
                my $arg = $4;
                my $fcn = $1 eq 'value' ? "getValueAt" : "getKeyAt";
                my $subExpn = $self->new();
                $subExpn->visit($lit->$fcn($arg));
                $self->append($subExpn->getCppCode());
                next;
            }
        }
        $self->append($c);
        $i++;
    }
}

sub visit_TupleLiteral
{
    my ($self, $lit) = @_;
    my $cppCode = $lit->getCppCode();
    my $len = length ($cppCode);
    my $i = 0;
    while ($i < $len) {
        my $c = substr $cppCode, $i, 1;
        if ($c eq '{') {
            my $rest = substr ($cppCode, $i);
            if ($rest =~ /^\{attr:(\d+)\}/) {
                $i += length ($&);
                my $arg = $1;
                my $subExpn = $self->new();
                $subExpn->visit($lit->getLiteralAt($arg));
                $self->append($subExpn->getCppCode());
                next;
            }
        }
        $self->append($c);
        $i++;
    }
}

sub new($$)
{
    my ($class, $expn) = @_;
    $class = ref($class) if ref($class);
    my $self = { code_ => ""};
    return bless($self, $class);
}
1;
# END_DOXYGEN_SKIP_FOR_USERS
