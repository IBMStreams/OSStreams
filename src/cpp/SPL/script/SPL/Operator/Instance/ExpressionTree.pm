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

## @class SPL::Operator::Instance::ExpressionTree
# \brief Class that represents the expression tree corresponding to an %SPL expression
#
# Important objects in an ExpressionTree:
# - The type
# - The kind
package SPL::Operator::Instance::ExpressionTree;

use strict;
use warnings;
use SPL::ModelHelper;

our $expressionKindMap = {
    'attribute'         => 'Attribute',
    'binary'            => 'Binary',
    'call'              => 'Call',
    'cast'              => 'Cast',
    'conditional'       => 'Conditional',
    'enum'              => 'Enum',
    'isPresent'         => 'IsPresent',
    'literal'           => 'Literal',
    'postfix'           => 'Postfix',
    'prefix'            => 'Prefix',
    'streamHistory'     => 'StreamHistory',
    'stream'            => 'Stream',
    'subscript'         => 'Subscript',
    'symbol'            => 'Symbol',
    'unary'             => 'Unary',
    'unwrap'            => 'Unwrap',
    'unwrapOrElse'      => 'UnwrapOrElse',
};

our $literalKindMap = {
    'primitive'   => 'Primitive',
    'expn'        => 'Expression',
    'set'         => 'Set',
    'list'        => 'List',
    'map'         => 'Map',
    'tuple'       => 'Tuple'
};

## @method string getKind()
# Return the expression kind, which is one of:
# attribute/binary/call/cast/conditional/enum/isPresent/literal/postfix/prefix/streamHistory/stream/symbol/unary/unwrap/unwrapOrElse
# @return the expression kind as a string
sub getKind
{
  my ($self) = @_;
  return $self->{kind_};
}

## @method string getType()
# Return the SPL type of the expression
# @return the SPL type as a string
sub getType
{
  my ($self) = @_;
  return $self->{type_};
}

## @method string getCppCode()
# Return the C++ code for this expression, or undef
# The C++ code has references to nested expression trees, and is not directly compilable as C++
# SPL::Operator::Instance::ExpressionTreeCppGenVisitor can be used to extract compilable C++
# @return the C++ code as a string, or undef if C++ code was not generated for this expression
sub getCppCode
{
  my ($self) = @_;
  return $self->{cppCode_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  die "toString not implemented in derived class: '" . $self->getKind() . "'!";
}

## @method bool isAttribute()
# Is this expression an Attribute %Expression?
# @return 1 if the expression is an Attribute %Expression, else 0.
sub isAttribute
{
  my ($self) = @_;
  return $self->{kind_} eq 'attribute';
}

## @method bool isBinary()
# Is this expression an Binary %Expression?
# @return 1 if the expression is an Binary %Expression, else 0.
sub isBinary
{
  my ($self) = @_;
  return $self->{kind_} eq 'binary';
}

## @method bool isCall()
# Is this expression an Call %Expression?
# @return 1 if the expression is an Call %Expression, else 0.
sub isCall
{
  my ($self) = @_;
  return $self->{kind_} eq 'call';
}

## @method bool isCast()
# Is this expression an Cast %Expression?
# @return 1 if the expression is an Cast %Expression, else 0.
sub isCast
{
  my ($self) = @_;
  return $self->{kind_} eq 'cast';
}

## @method bool isConditional()
# Is this expression an Conditional %Expression?
# @return 1 if the expression is an Conditional %Expression, else 0.
sub isConditional
{
  my ($self) = @_;
  return $self->{kind_} eq 'conditional';
}

## @method bool isEnum()
# Is this expression an Enum %Expression?
# @return 1 if the expression is an Enum %Expression, else 0.
sub isEnum
{
  my ($self) = @_;
  return $self->{kind_} eq 'enum';
}

## @method bool isIsPresent()
# Is this expression an IsPresent %Expression?
# @return 1 if the expression is an IsPresent %Expression, else 0.
sub isIsPresent
{
  my ($self) = @_;
  return $self->{kind_} eq 'isPresent';
}

## @method bool isLiteral()
# Is this expression an Literal %Expression?
# @return 1 if the expression is an Literal %Expression, else 0.
sub isLiteral
{
  my ($self) = @_;
  return $self->{kind_} eq 'literal';
}

## @method bool isPostfix()
# Is this expression an Postfix %Expression?
# @return 1 if the expression is an Postfix %Expression, else 0.
sub isPostfix
{
  my ($self) = @_;
  return $self->{kind_} eq 'postfix';
}

## @method bool isPrefix()
# Is this expression an Prefix %Expression?
# @return 1 if the expression is an Prefix %Expression, else 0.
sub isPrefix
{
  my ($self) = @_;
  return $self->{kind_} eq 'prefix';
}

## @method bool isStreamHistory()
# Is this expression an StreamHistory %Expression?
# @return 1 if the expression is an StreamHistory %Expression, else 0.
sub isStreamHistory
{
  my ($self) = @_;
  return $self->{kind_} eq 'streamHistory';
}

## @method bool isStream()
# Is this expression an Stream %Expression?
# @return 1 if the expression is an Stream %Expression, else 0.
sub isStream
{
  my ($self) = @_;
  return $self->{kind_} eq 'stream';
}

## @method bool isSubscript()
# Is this expression an Subscript %Expression?
# @return 1 if the expression is an Subscript %Expression, else 0.
sub isSubscript
{
  my ($self) = @_;
  return $self->{kind_} eq 'subscript';
}

## @method bool isSymbol()
# Is this expression an Symbol %Expression?
# @return 1 if the expression is an Symbol %Expression, else 0.
sub isSymbol
{
  my ($self) = @_;
  return $self->{kind_} eq 'symbol';
}

## @method bool isUnary()
# Is this expression an Unary %Expression?
# @return 1 if the expression is an Unary %Expression, else 0.
sub isUnary
{
  my ($self) = @_;
  return $self->{kind_} eq 'unary';
}

## @method bool isUnwrap()
# Is this expression an Unwrap %Expression?
# @return 1 if the expression is an Unwrap %Expression, else 0.
sub isUnwrap
{
  my ($self) = @_;
  return $self->{kind_} eq 'unwrap';
}

## @method bool isUnwrapOrElse()
# Is this expression an UnwrapOrElse %Expression?
# @return 1 if the expression is an UnwrapOrElse %Expression, else 0.
sub isUnwrapOrElse
{
  my ($self) = @_;
  return $self->{kind_} eq 'unwrapOrElse';
}

# DOXYGEN_SKIP_FOR_USERS
sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = { };

  my $kind;
  my $type = undef;
  my $cppCode = undef;

  foreach my $i (keys(%{$xml})) {
      if ($i eq 'type') {
          $type = $context->getType($xml->{type});
      } elsif ($i eq 'cppCode') {
          $cppCode = $xml->{cppCode};
      } else {
          $kind = $i;
      }
  }

  # Create the derived class
  my $d = "SPL::Operator::Instance::ExpressionTree::" . $expressionKindMap->{$kind};
  $self = $d->new ($xml->{$kind}->[0], $context);

  # add in the default fields
  $self->{kind_} = $kind;
  $self->{type_} = $type if !$self->{type_};
  $self->{cppCode_} = $cppCode unless defined  $self->{cppCode_};

  # $self is already blessed
  return $self;
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Attribute
# \brief Class that represents an Attribute expression
#
# Important objects in an Attribute:
# - The left hand side of the expression
# - The attribute name
package SPL::Operator::Instance::ExpressionTree::Attribute;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getLhs()
# Get the expression that is being 'dotted'
# @return the ExpressionTree object that is being 'fielded'
sub getLhs
{
  my ($self) = @_;
  return $self->{lhs_};
}


## @method getAttributeName()
# Get the name of the attribute
# @return the attribute name
sub getAttributeName
{
  my ($self) = @_;
  return $self->{attribute_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getLhs()->toString() . '.' . $self->getAttributeName();
}


# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                attribute_ => $xml->{attribute},
                lhs_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{lhs}->[0], $context),
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Binary
# \brief Class that represents a Binary expression
#
# Important objects in a Binary:
# - The operator for the expression
# - The left hand side of the expression
# - The right hand side of the expression
package SPL::Operator::Instance::ExpressionTree::Binary;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getLhs()
# Get the expression that is
# @return the ExpressionTree object that is being 'fielded'
sub getLhs
{
  my ($self) = @_;
  return $self->{lhs_};
}


## @method getRhs()
# Get the expression that is
# @return the ExpressionTree object that is being 'fielded'
sub getRhs
{
  my ($self) = @_;
  return $self->{rhs_};
}


## @method getOp()
# Get the operator for the expression
# @return the operator as a string
sub getOp
{
  my ($self) = @_;
  return $self->{op_};
}


## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getLhs()->toString() . $self->getOp() . $self->getRhs()->toString();
}

# DOXYGEN_SKIP_FOR_USERS
sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                  type_ => $context->getType($xml->{type}),
                  op_ => $xml->{op},
                  lhs_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{lhs}->[0], $context),
                  rhs_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{rhs}->[0], $context),
                  cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Call
# \brief Class that represents a Call expression
#
# Important objects in a Call:
# - The name of the function being called
# - The arguments to the function
package SPL::Operator::Instance::ExpressionTree::Call;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getNumberOfArguments()
# Get the number of arguments to the function
# @return the number of arguments
sub getNumberOfArguments
{
  my ($self) = @_;
  return scalar(@{$self->{args_}});
}

## @method getArguments()
# Get the arguments to the function call
# @return the list of ExpressionTree representing the arguments
sub getArguments
{
  my ($self) = @_;
  return $self->{args_};
}


## @method ExpressionTree getArgumentAt($index)
# Get the argument at the given index within the list of arguments
# @param index the index of the desired expression tree
# @return the desired expression tree object
sub getArgumentAt
{
  my ($self, $index) = @_;
  return $self->{args_}->[$index];
}


## @method getFunctionName()
# Get the name of the function being called
# @return the attribute name
sub getFunctionName
{
  my ($self) = @_;
  return $self->{fcn_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  my $ret = $self->getFunctionName() . '(';
  my $numArgs = $self->getNumberOfArguments();
  for (my $i = 0; $i < $numArgs; $i++) {
      $ret .= ", " if $i > 0;
      $ret .= $self->getArgumentAt($i)->toString();
  }
  return $ret . ')';
}


# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                fcn_ => $xml->{fcn},
                type_ => $context->getType($xml->{type}),
                args_ => [],
                cppCode_ => $xml->{cppCode}
             };

  foreach my $elem (@{$xml->{args}->[0]->{arg}}) {
      push @{$self->{args_}}, SPL::Operator::Instance::ExpressionTree->new ($elem, $context);
  }
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Cast
# \brief Class that represents a Cast expression
#
# Important objects in a Cast:
# - The expression that is being cast
package SPL::Operator::Instance::ExpressionTree::Cast;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getExpression()
# Get the expression that is being cast
# @return the ExpressionTree object that is being cast.
sub getExpression
{
  my ($self) = @_;
  return $self->{expn_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return '(' . $self->getType() . ')' . $self->getExpression()->toString();
}


# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);
  my $self = {
                type_ => $context->getType($xml->{type}),
                expn_ => SPL::Operator::Instance::ExpressionTree->new ($xml, $context),
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Conditional
# \brief Class that represents a Conditional expression
#
# Important objects in a Conditional:
# - The condition.
# - The left hand side of the expression
# - The right hand side of the expression
package SPL::Operator::Instance::ExpressionTree::Conditional;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getLhs()
# Get the expression that is used if the conditional evaluates to true
# @return the ExpressionTree object for the true case
sub getLhs
{
  my ($self) = @_;
  return $self->{lhs_};
}


## @method getRhs()
# Get the expression that is used if the conditional evaluates to false
# @return the ExpressionTree object for the false case
sub getRhs
{
  my ($self) = @_;
  return $self->{rhs_};
}


## @method getCondition()
# Get the expression that is used to control evaluation of the left and right sides
# @return the ExpressionTree object for the condition
sub getCondition
{
  my ($self) = @_;
  return $self->{condition_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getCondition()->toString() . ' ? ' . $self->getLhs()->toString() . ' : ' .
         $self->getRhs()->toString();
}


# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                condition_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{condition}->[0], $context),
                lhs_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{lhs}->[0], $context),
                rhs_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{rhs}->[0], $context),
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Enum
# \brief Class that represents an Enum expression
#
# Important objects in an Enum:
# - The name of the enum value
# - The numeric value of the enum (may return 'undef' if Custom Literal enum)
package SPL::Operator::Instance::ExpressionTree::Enum;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getName()
# Get the name of the enum value
# @return the name of the enumeration constant
sub getName
{
  my ($self) = @_;
  return $self->{id_};
}


## @method getValue()
# Get the numeric value of the enum, or undef for Custom Literal
# @return the value of the enum constant
sub getValue
{
  my ($self) = @_;
  return $self->{value_};
}


## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getName();
}

# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                value_ => $xml->{value},
                id_ => $xml->{id},
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS


## @class SPL::Operator::Instance::ExpressionTree::IsPresent
# \brief Class that represents an IsPresent expression
#
# Important objects in a IsPresent:
# - The operator
# - The associated expression
package SPL::Operator::Instance::ExpressionTree::IsPresent;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getExpression()
# Get the associated expression
# @return the ExpressionTree object for the expression
sub getExpression
{
  my ($self) = @_;
  return $self->{expn_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getExpression()->toString() . '!';
}

# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                expn_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{expn}->[0], $context),
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS


## @class SPL::Operator::Instance::ExpressionTree::Literal
# \brief Class that represents a Literal expression
#
# Important objects in a Literal:
# - Lots of stuff
package SPL::Operator::Instance::ExpressionTree::Literal;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method string getLiteralKind()
# Return the literal kind, which is one of:
# primitive/expn/set/list/map/tuple
# @return the literal kind as a string
sub getLiteralKind
{
  my ($self) = @_;
  return $self->{literalKind_};
}


## @method bool isPrimitiveLiteral()
# Is this expression a Primitive %Literal?
# @return 1 if the literal is a Primitive %Literal, else 0.
sub isPrimitiveLiteral
{
  my ($self) = @_;
  return $self->{literalKind_} eq 'primitive';
}

## @method bool isExpressionLiteral()
# Is this expression a Expression %Literal?
# @return 1 if the literal is a Expression %Literal, else 0.
sub isExpressionLiteral
{
  my ($self) = @_;
  return $self->{literalKind_} eq 'expn';
}

## @method bool isSetLiteral()
# Is this expression a Set %Literal?
# @return 1 if the literal is a Set %Literal, else 0.
sub isSetLiteral
{
  my ($self) = @_;
  return $self->{literalKind_} eq 'set';
}

## @method bool isListLiteral()
# Is this expression a List %Literal?
# @return 1 if the literal is a List %Literal, else 0.
sub isListLiteral
{
  my ($self) = @_;
  return $self->{literalKind_} eq 'list';
}

## @method bool isMapLiteral()
# Is this expression a Map %Literal?
# @return 1 if the literal is a Map %Literal, else 0.
sub isMapLiteral
{
  my ($self) = @_;
  return $self->{literalKind_} eq 'map';
}

## @method bool isTupleLiteral()
# Is this expression a Tuple %Literal?
# @return 1 if the literal is a Tuple %Literal, else 0.
sub isTupleLiteral
{
  my ($self) = @_;
  return $self->{literalKind_} eq 'tuple';
}

# DOXYGEN_SKIP_FOR_USERS
sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {};
  my $kind;
  my $type;
  my $cppCode = undef;

  foreach my $i (keys(%{$xml})) {
      if ($i eq 'content') {
          $kind = 'primitive';
      } elsif ($i eq 'cppCode') {
          $cppCode = $xml->{cppCode};
      } elsif ($i eq 'type') {
          $type = $context->getType($xml->{$i});
      } else {
          $kind = $i;
      }
  }

  my $l = "SPL::Operator::Instance::ExpressionTree::" . $literalKindMap->{$kind} . "Literal";
  if ($kind eq 'primitive') {
      $self = $l->new($xml, $context);
  } else {
      $self = $l->new($xml->{$kind}->[0], $context);
  }

  # set the default fields
  $self->{literalKind_} = $kind;
  $self->{kind_} = 'literal';
  $self->{type_} = $type if !$self->{type_};
  $self->{cppCode_} = $cppCode;

  # result is already blessed
  return $self;
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::ExpressionLiteral
# \brief Class that represents a literal with a runtime expression
#
# Important objects in an ExpressionLiteral:
# - The expression
package SPL::Operator::Instance::ExpressionTree::ExpressionLiteral;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree::Literal);

## @method getExpression()
# Get the ExpressionTree object representing the literal value
# @return the ExpressionTree for the literal value
sub getExpression
{
  my ($self) = @_;
  return $self->{expn_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getExpression()->toString();
}


# DOXYGEN_SKIP_FOR_USERS
sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                expn_ => SPL::Operator::Instance::ExpressionTree->new ($xml, $context),
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::ListOrSetLiteral
# \brief Class that represents a literal list or set
#
# Important objects in a ListOrSetLiteral:
# - The number of elements in the list or set
# - The literals in the list or set
package SPL::Operator::Instance::ExpressionTree::ListOrSetLiteral;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree::Literal);

## @method getNumberOfElements()
# Get the number of elements in the list or set
# @return the number of elements
sub getNumberOfElements()
{
  my ($self) = @_;
  return scalar(@{$self->{items_}});
}

## @method Literal getLiteralAt($index)
# Get the literal at the given index within the list or set
# @param index the index of the desired Literal
# @return the desired expression tree object
sub getLiteralAt
{
  my ($self, $index) = @_;
  return $self->{items_}->[$index];
}

## @method getLiterals()
# Get all the literals in the list or set
# @return the list of Literal(s) representing the literals
sub getLiterals
{
  my ($self) = @_;
  return $self->{items_};
}

# DOXYGEN_SKIP_FOR_USERS
sub valuesToString()
{
  my ($self) = @_;
  my $numElements = $self->getNumberOfElements();
  my $ret = "";
  for (my $i = 0; $i < $numElements; $i++) {
     $ret .= ", " if $i > 0;
     $ret .= $self->getLiteralAt($i)->toString();
  }
  return $ret;
}

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                items_ => [],
                cppCode_ => $xml->{cppCode}
             };

  foreach my $elem (@{$xml->{value}}) {
      push @{$self->{items_}},
          SPL::Operator::Instance::ExpressionTree::Literal->new ($elem, $context);
  }
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::ListLiteral
# \brief Class that represents a literal list
#
# Important objects in a ListLiteral:
# - The number of elements in the list
# - The literals in the list
package SPL::Operator::Instance::ExpressionTree::ListLiteral;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree::ListOrSetLiteral);

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return '[' . $self->valuesToString() . ']';
}

1;

## @class SPL::Operator::Instance::ExpressionTree::SetLiteral
# \brief Class that represents a literal set
#
# Important objects in a SetLiteral:
# - The number of elements in the set
# - The literals in the set
package SPL::Operator::Instance::ExpressionTree::SetLiteral;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree::ListOrSetLiteral);

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return '{' . $self->valuesToString() . '}';
}

1;

# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::MapLiteral
# \brief Class that represents a literal map
#
# Important objects in a MapLiteral:
# - The number of elements in the map
# - The literals for the keys and values in the map
package SPL::Operator::Instance::ExpressionTree::MapLiteral;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree::Literal);

## @method getNumberOfElements()
# Get the number of elements in the map
# @return the number of elements
sub getNumberOfElements()
{
  my ($self) = @_;
  return scalar(@{$self->{keys_}});
}

## @method Literal getKeyAt($index)
# Get the literal key at the given index within the map
# @param index the index of the desired key
# @return the desired literal object
sub getKeyAt
{
  my ($self, $index) = @_;
  return $self->{keys_}->[$index];
}

## @method getKeys()
# Get all the keys in the map
# @return the list of Literal(s) representing the keya
sub getKeys
{
  my ($self) = @_;
  return $self->{keys_};
}

## @method Literal getValueAt($index)
# Get the literal value at the given index within the map
# @param index the index of the desired value
# @return the desired literal object
sub getValueAt
{
  my ($self, $index) = @_;
  return $self->{values_}->[$index];
}

## @method getValues()
# Get all the values in the map
# @return the list of Literal(s) representing the values
sub getValues
{
  my ($self) = @_;
  return $self->{values_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  my $kind = $self->getKind();
  my $ret = '{';
  my $numElements = $self->getNumberOfElements();
  for (my $i = 0; $i < $numElements; $i++) {
     $ret .= ", " if $i > 0;
     $ret .= $self->getKeyAt($i)->toString();
     $ret .= ':';
     $ret .= $self->getValueAt($i)->toString();
  }
  return $ret . '}';
}

# DOXYGEN_SKIP_FOR_USERS
sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                keys_ => [],
                values_ => [],
                cppCode_ => $xml->{cppCode}
             };

  foreach my $elem (@{$xml->{value}}) {
      push @{$self->{values_}},
          SPL::Operator::Instance::ExpressionTree::Literal->new ($elem->{value}->[0], $context);
      push @{$self->{keys_}},
          SPL::Operator::Instance::ExpressionTree::Literal->new ($elem->{key}->[0], $context);
  }
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::PrimitiveLiteral
# \brief Class that represents a scalar literal
#
# Important objects in a PrimitiveLiteral:
# - The value
package SPL::Operator::Instance::ExpressionTree::PrimitiveLiteral;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree::Literal);


## @method getValue()
# Get the string representing the literal value
# @return the value of a primitive literal
sub getValue
{
  my ($self) = @_;
  return $self->{value_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getValue();
}


# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                value_ => $xml->{content},
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::StreamHistory
# \brief Class that represents a StreamHistory expression
#
# Important objects in a StreamHistory:
# - The associatated stream
# - The history depth
package SPL::Operator::Instance::ExpressionTree::StreamHistory;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getStream()
# Get the stream expression for this reference
# @return the associated stream expression
sub getStream
{
  my ($self) = @_;
  return $self->{stream_};
}


## @method int getDepth()
# Get the depth of the stream history reference
# @return the history depth
sub getDepth
{
  my ($self) = @_;
  return $self->{depth_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getStream()->toString() . '['. $self->getDepth() . ']';
}

# DOXYGEN_SKIP_FOR_USERS
sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                depth_ => $xml->{depth},
                stream_ => SPL::Operator::Instance::ExpressionTree::Stream->new ($xml->{stream}->[0], $context),
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Stream
# \brief Class that represents a Stream expression
#
# Important objects in a Stream:
# - The port number
# - The stream name
package SPL::Operator::Instance::ExpressionTree::Stream;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getPort()
# Get the number of the associated port for this stream
# @return the port number
sub getPort
{
  my ($self) = @_;
  return $self->{port_};
}

## @method getName()
# Get the name of the associated port for this stream
# @return the port name
sub getName
{
  my ($self) = @_;
  return $self->{name_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getName();
}

# DOXYGEN_SKIP_FOR_USERS
sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                port_ => $xml->{port},
                name_ => $xml->{name},
                kind_ => 'stream',
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Subscript
# \brief Class that represents a Subscript expression
#
# Important objects in a Subscript:
# - The expression being subscripted
# - The lower bound
# - The upper bound
# - Is this a slice?
package SPL::Operator::Instance::ExpressionTree::Subscript;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method ExpressionTree getExpression()
# Get the expression that is being subscripted
# @return the ExpressionTree object that is being 'subscripted'
sub getExpression
{
  my ($self) = @_;
  return $self->{expn_};
}


## @method ExpressionTree getLower()
# Get the name lower bound for the subscript or undef if not defined
# @return the lower bound
sub getLower
{
  my ($self) = @_;
  return $self->{lower_};
}


## @method ExpressionTree getUpper()
# Get the name upper bound for the subscript or undef if not defined
# @return the upper bound
sub getUpper
{
  my ($self) = @_;
  return $self->{upper_};
}


## @method isSlice()
# Is this a slice?
# @return 1 if this is a slice
sub isSlice
{
  my ($self) = @_;
  return $self->{slice_};
}


# DOXYGEN_SKIP_FOR_USERS

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  my $ret = $self->getExpression()->toString();
  my $lower = $self->getLower();
  return $ret if !$lower;
  $ret .= '[' . $lower->toString();
  my $upper = $self->getUpper();
  $ret .= ':' . $upper->toString() if $upper;
  return $ret . ']';
}

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                expn_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{expn}->[0], $context),
                lower_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{lower}->[0], $context),
                slice_ => SPL::ModelHelper::booleanTest($xml->{slice}),
                cppCode_ => $xml->{cppCode}
             };
  $self->{upper_} = ($self->{slice_}) ? SPL::Operator::Instance::ExpressionTree->new ($xml->{upper}->[0], $context) : undef;
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Symbol
# \brief Class that represents a Symbol expression
#
# Important objects in a Symbol:
# - The symbol name
package SPL::Operator::Instance::ExpressionTree::Symbol;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getName()
# Get the name of the symbol
# @return the symbol name
sub getSymbol
{
  my ($self) = @_;
  return $self->{symbol_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getSymbol();
}


# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                symbol_ => $xml->{id},
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::TupleLiteral
# \brief Class that represents a literal tuple
#
# Important objects in a TupleLiteral:
# - The number of attributes in the tuple
# - The attribute names and literal values in the tuple
package SPL::Operator::Instance::ExpressionTree::TupleLiteral;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree::Literal);

## @method getNumberOfElements()
# Get the number of elements in the tuple
# @return the number of elements
sub getNumberOfElements()
{
  my ($self) = @_;
  return scalar(@{$self->{values_}});
}

## @method Literal getLiteralAt($index)
# Get the literal at the given index within the tuple
# @param index the index of the desired Literal
# @return the desired expression tree object
sub getLiteralAt
{
  my ($self, $index) = @_;
  return $self->{values_}->[$index];
}

## @method getLiterals()
# Get all the literals in the tuple
# @return the list of Literal(s) representing the literals
sub getLiterals
{
  my ($self) = @_;
  return $self->{values_};
}

## @method string getAttributeAt($index)
# Get the attribute name at the given index within the tuple
# @param index the index of the desired attribute
# @return the desired attribute name
sub getAttributeAt
{
  my ($self, $index) = @_;
  return $self->{ids_}->[$index];
}

## @method getAttributes()
# Get all the attribute names in the tuple
# @return the list of attribute names in the tuple
sub getAttributes
{
  my ($self) = @_;
  return $self->{ids_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  my $kind = $self->getKind();
  my $ret = '{';
  my $numElements = $self->getNumberOfElements();
  for (my $i = 0; $i < $numElements; $i++) {
     $ret .= ", " if $i > 0;
     $ret .= $self->getAttributeAt($i);
     $ret .= '=';
     $ret .= $self->getLiteralAt($i)->toString();
  }
  return $ret . '}';
}


# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                ids_ => [],
                values_ => [],
                cppCode_ => $xml->{cppCode}
             };
  foreach my $elem (@{$xml->{attr}}) {
      push @{$self->{ids_}}, $elem->{id};
      push @{$self->{values_}},
          SPL::Operator::Instance::ExpressionTree::Literal->new (@{$elem->{value}}, $context);
  }

  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Unary
# \brief Class that represents a Unary expression
#
# Important objects in a Unary:
# - The operator
# - The associated expression
package SPL::Operator::Instance::ExpressionTree::Unary;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getExpression()
# Get the associated expression
# @return the ExpressionTree object for the expression
sub getExpression
{
  my ($self) = @_;
  return $self->{expn_};
}

## @method getOp()
# Get the operator for the expression
# @return the operator as a string
sub getOp
{
  my ($self) = @_;
  return $self->{op_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getOp() . $self->getExpression()->toString();
}

# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                op_ => $xml->{op},
                expn_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{expn}->[0], $context),
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Unwrap
# \brief Class that represents an Unwrap expression
#
# Important objects in a Unwrap:
# - The operator
# - The associated expression
package SPL::Operator::Instance::ExpressionTree::Unwrap;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getExpression()
# Get the associated expression
# @return the ExpressionTree object for the expression
sub getExpression
{
  my ($self) = @_;
  return $self->{expn_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getExpression()->toString() . '!';
}

# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                expn_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{expn}->[0], $context),
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::UnwrapOrElse
# \brief Class that represents an UnwrapOrElse expression
#
# Important objects in a UnwrapOrElse:
# - The operator
# - The associated left hand size and right hand side expressions
package SPL::Operator::Instance::ExpressionTree::UnwrapOrElse;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree);

## @method getLhs()
# Get the associated left hand side expression
# @return the ExpressionTree object for the expression
sub getLhs
{
  my ($self) = @_;
  return $self->{lhs_};
}

## @method getRhs()
# Get the associated left hand side expression
# @return the ExpressionTree object for the expression
sub getRhs
{
  my ($self) = @_;
  return $self->{rhs_};
}

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getLhs()->toString() . '?: ' . $self->getRhs()->toString();
}


# DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class, $xml, $context) = @_;

  $class = ref($class) if ref($class);

  my $self = {
                type_ => $context->getType($xml->{type}),
                lhs_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{lhs}->[0], $context),
                rhs_ => SPL::Operator::Instance::ExpressionTree->new ($xml->{rhs}->[0], $context),
                cppCode_ => $xml->{cppCode}
             };
  return bless($self, $class);
}

1;
# END_DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::ExpressionTree::Prefix
# \brief Class that represents a Prefix expression
#
# Important objects in a Prefix:
# - The operator
# - The associated expression
package SPL::Operator::Instance::ExpressionTree::Prefix;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree::Unary);
1;


## @class SPL::Operator::Instance::ExpressionTree::Postfix
# \brief Class that represents a Postfix expression
#
# Important objects in a Postfix:
# - The operator
# - The associated expression
package SPL::Operator::Instance::ExpressionTree::Postfix;

use strict;
use warnings;
use base qw(SPL::Operator::Instance::ExpressionTree::Unary);

## @method string toString()
# Return a printable form of the expression
# @return the expression formatted as a string
sub toString
{
  my ($self) = @_;
  return $self->getExpression()->toString() . $self->getOp();
}

1;
