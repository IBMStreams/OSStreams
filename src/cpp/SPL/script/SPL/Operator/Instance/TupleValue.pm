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

# DOXYGEN_SKIP_FOR_USERS

## @class SPL::Operator::Instance::Value
# \brief Class that represents a value associated with an annotation, or value within an
# annotation.
package SPL::Operator::Instance::Value;
use strict;
use warnings;

our $typeMap = {
  'tuple'      => 'SPL::Operator::Instance::TupleValue',
  'list'       => 'SPL::Operator::Instance::ListValue',
  'set'        => 'SPL::Operator::Instance::SetValue',
  'map'        => 'SPL::Operator::Instance::MapValue'
};

## @method string getKind()
# Returns the kind of value a node represents.  It can be one of:
# tuple, tupleAttribute, list, set or map.
# @return the value kind
sub getKind
{
  my ($self) = @_;
  return $self->{kind_};
}

## @method string getType
# Returns the SPL type of the specific value
# @return the SPL type
sub getType
{
  my ($self) = @_;
  return $self->{type_};
}

## @method Value getValue
# Get the value associated with this node.  This could be a scalar Perl value
# or a reference to a SPL::Operator::Instance::Value if the value is not scalar.
# @return the value
sub getValue
{
  my ($self) = @_;
  return $self->{value_};
}

## @method int isTuple()
# Determine if the node represents a tuple value
# @return Returns 1 if this is a tuple value, otherwise 0.
sub isTuple
{
  return 0;
}

## @method int isList()
# Determine if the node represents a list value
# @return Returns 1 if this is a list value, otherwise 0.
sub isList
{
  return 0;
}

## @method int isSet()
# Determine if the node represents a set value
# @return Returns 1 if this is a set value, otherwise 0.
sub isSet
{
  return 0;
}

## @method int isMap()
# Determine if the node represents a map value
# @return Returns 1 if this is a map value, otherwise 0.
sub isMap
{
  return 0;
}

## @method int isScalar()
# Determine if the node represents a scalar value
# If a node represents a scalar value then the value returned by
# getValue can be used directly as a scalar value in Perl.
# @return Returns 1 if this is a scalar value, otherwise 0.
sub isScalar
{
  my ($self) = @_;
  return !ref($self->{value_});
}

sub valueKind
{
  my ($type) = @_;
  # Is it a primitive type
  return 'scalar' if (SPL::CodeGen::Type::isPrimitive($type));

  # We have a composite type
  return 'tuple' if (SPL::CodeGen::Type::isTuple($type));
  return 'list' if (SPL::CodeGen::Type::isList($type));
  return 'set' if (SPL::CodeGen::Type::isSet($type));
  return 'map' if (SPL::CodeGen::Type::isMap($type));
}

sub new
{
  my ($class, $kind, $type) = @_;
  my $self = {};
  $self->{kind_} = $kind;
  $self->{type_} = $type;
  return bless($self, $class);
}

# END_DOXYGEN_SKIP_FOR_USERS

1;

# DOXYGEN_SKIP_FOR_USERS

package SPL::Operator::Instance::ListOrSetValue;
our @ISA = qw(SPL::Operator::Instance::Value);
use strict;
use warnings;

## @method int getNumberOfElements()
# Gets the number of elements in the list or set value
# @return the number of elements
sub getNumberOfElements
{
  my ($self) = @_;
  return scalar @{$self->{value_}};
}

## @method list<elements> getElements()
# Get the element values for this list or set.
# The values could be scalar or instances of SPL::Operator::Instance::Value.
# @return the values.
sub getElements
{
  my ($self) = @_;
  return $self->{value_};
}

## @method string getElementType()
# Returns the SPL type of the value
# @return the SPL type
sub getElementType
{
  my ($self) = @_;
  return $self->{elementType_};
}

sub isScalar
{
  my ($self) = @_;
  return !ref($self->{value_}->[0]);
}

sub new
{
  my ($class, $kind, $xml, $type) = @_;
  my $self = $class->SUPER::new($kind, $type);
  $self->{value_} = [];
  $self->{elementType_} = $xml->{elementType};
  my $valueType = SPL::Operator::Instance::Value::valueKind($self->{elementType_});
  my $values = $xml->{value};
  foreach my $v (@{$values}) {
    if ($valueType eq 'scalar') {
      push @{$self->{value_}}, $v;
    } else {
      my $nv = $typeMap->{$valueType}->new($v, $self->{elementType_});
      push @{$self->{value_}}, $nv;
    }
  }
  return bless($self, $class);
}
# END_DOXYGEN_SKIP_FOR_USERS

1;

# DOXYGEN_SKIP_FOR_USERS
package SPL::Operator::Instance::SetValue;
our @ISA = qw(SPL::Operator::Instance::ListOrSetValue);
use strict;
use warnings;

sub isSet
{
  return 1;
}

sub new
{
  my ($class, $xml) = @_;
  my $setElem = $xml->{set}->[0];
  my $self = $class->SUPER::new("set", $setElem, $xml->{type});
  return bless($self, $class);
}
# END_DOXYGEN_SKIP_FOR_USERS
1;

# DOXYGEN_SKIP_FOR_USERS
package SPL::Operator::Instance::MapValue;
our @ISA = qw(SPL::Operator::Instance::Value);
use strict;
use warnings;

sub isMap
{
  return 1;
}

sub new
{
  my ($class, $xml) = @_;
  my $self = $class->SUPER::new("map", $xml->{type});
  $self->{value_} = {};
  my $mapElem = $xml->{map}->[0];
  $self->{keyType_} = $mapElem->{keyType};
  $self->{valueType_} = $mapElem->{valueType};
  my $keyValueType = SPL::Operator::Instance::Value::valueKind($self->{keyType_});
  my $valueValueType = SPL::Operator::Instance::Value::valueKind($self->{valueType_});
  for (my $i = 0; $i < scalar @{$mapElem->{key}}; ++$i) {
    my $key = @{$mapElem->{key}}[$i];
    my $kv;
    if ($keyValueType eq 'scalar') {
      $kv = $key;
    } else {
      $kv = $typeMap->{$keyValueType}->new($key, $self->{keyType_});
    }
    my $value = $mapElem->{value}->[$i];
    my $vv;
    if ($valueValueType eq 'scalar') {
      $vv = $value;
    } else {
      $vv = $typeMap->{$valueValueType}->new($value, $self->{valueType_});
    }
    $self->{value_}->{$kv} = $vv;
  }
  return bless($self, $class);
}
# END_DOXYGEN_SKIP_FOR_USERS
1;

# DOXYGEN_SKIP_FOR_USERS
package SPL::Operator::Instance::ListValue;
our @ISA = qw(SPL::Operator::Instance::ListOrSetValue);
use strict;
use warnings;

## @method element getElementAt(index)
# Gets the element at the given index, or undef if the index is not valid.
# @return the element or undef
sub getElementAt
{
  my ($self, $index) = @_;
  return $self->{value_}->[$index];
}

sub isList
{
  return 1;
}

sub new
{
  my ($class, $xml) = @_;
  my $listElem = $xml->{list}->[0];
  my $self = $class->SUPER::new("list", $listElem, $xml->{type});
  return bless($self, $class);
}
# END_DOXYGEN_SKIP_FOR_USERS

1;

# DOXYGEN_SKIP_FOR_USERS
package SPL::Operator::Instance::TupleAttributeValue;
our @ISA = qw(SPL::Operator::Instance::Value);
use strict;
use warnings;

## @method string getName()
# Gets the name of this tuple attribute
# @return the attribute name
sub getName
{
  my ($self) = @_;
  return $self->{name_};
}

sub new
{
  my ($class, $xml) = @_;
  my $self = $class->SUPER::new("tupleAttribute", $xml->{type});
  $self->{name_} = $xml->{name};
  my $valueType = SPL::Operator::Instance::Value::valueKind($xml->{type});

  if ($valueType eq 'scalar') {
    $self->{value_} = $xml->{content};
  } else {
    $self->{value_} = $typeMap->{$valueType}->new($xml, $xml->{type});
  }
  return bless($self, $class);
}
# END_DOXYGEN_SKIP_FOR_USERS

1;

# DOXYGEN_SKIP_FOR_USERS
## @class SPL::Operator::Instance::TupleValue
# \brief Class that represents a Tuple value
package SPL::Operator::Instance::TupleValue;
our @ISA = qw(SPL::Operator::Instance::Value);

use strict;
use warnings;

## @method int getNumberOfAttributes()
# Returns the number of attribute values in this tuple value.
# @return Returns the number of attribute values.
sub getNumberOfAttributes
{
  my ($self) = @_;
  return scalar @{$self->{attributes_}};
}

## @method list<SPL::Operator::Instance::TupleAttributeValue> getAttributes()
# Returns a list of the tuple attribute values of this tuple value
# @return Returns a list of the tuple attribute values of this tuple value
sub getAttributes
{
  my ($self, $index) = @_;
  return $self->{attributes_};
}

## @method SPL::Operator::Instance::TupleAttributeValue getAttributeAt(index)
# Gets the tuple attribute value for a given attribute
# @return Returns the tuple attribute value or undef if index is not valid.
sub getAttributeAt
{
  my ($self, $index) = @_;
  return $self->{attributes_}->[$index];
}

## @method SPL::Operator::Instance::TupleAttributeValue getAttributeByName(name)
# Get the attribute value associated witht he named attribute
# @return Returns the attribute value or undef if an attribute with the specified name is not found.
sub getAttributeByName
{
  my ($self, $name) = @_;
  foreach my $attr (@{$self->{attributes_}}) {
    return $attr if ($attr->getName() eq $name);
  }
  return undef;
}


sub new
{
  my ($class, $xml) = @_;
  my $tuple = $xml->{'an:tuple'}->[0];
  my $self = $class->SUPER::new("tuple", $tuple->{type});
  $self->{attributes_} = [];
  my $attributes = $tuple->{'an:attr'};
  foreach my $attr (@{$attributes}) {
    my $a = SPL::Operator::Instance::TupleAttributeValue->new($attr);
    push @{$self->{attributes_}}, $a;
  }
  return bless($self, $class);
}

# END_DOXYGEN_SKIP_FOR_USERS

1;
