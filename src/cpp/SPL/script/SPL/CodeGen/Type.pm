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

## @class SPL::CodeGen::Type
# \brief Class that provides code generation helper functions dealing with %SPL type inspection.
#
# This is the %SPL %Operator Code Generation %Type API.  This API is used to extract information about
# %SPL types.  These types are representations of the SPL types, and are often extracted using
# SPL::Operator::Instance::Expression::getSPLType() from expressions and parameters.  The %SPL type
# is the same as written in %SPL, with whitespace removed, except between an attribute's type and associated
# name.
# For example, the %SPL type for the output port of this operator:\n
# <tt>stream<rstring a, int32 b> A = Beacon() {}</tt>\n
# is the string:\n
# <tt>"tuple<rstring a,int32 b>"</tt>\n
#
# The functions in this class can be used to test the kind of the type, and pull apart types into
# their component parts.
#
# For the above example, <tt>SPL::CodeGen::Type::isTuple($type)</tt> will return <tt>1</tt>.\n
# <tt>SPL::CodeGen::Type::getAttributeTypes($type)</tt> will return <tt>["rstring", "int32"]</tt>\n
# <tt>SPL::CodeGen::Type::getAttributeNames($type)</tt> will return <tt>["a", "b"]</tt>
package SPL::CodeGen::Type;

use strict;
use warnings;

our $verboseMode;
# Type inspection functions



## @fn bool isOptional ($type)
# Check if this type is an optional type
#
# @param type %SPL type
# @return true if this is an optional type, false otherwise
sub isOptional($)
{
    my ($type) = @_;
    return $type =~ /^optional<.*>$/;
}

## @fn bool getUnderlyingType ($type)
# Get the underlying type for an optional type
#
# @param type %SPL type
# @return the underlying type of an optional type, undef otherwise
sub getUnderlyingType($)
{
    my ($type) = @_;
    if ($type =~ /^optional<(.*)>$/) {
        return $1;
    }
    return undef;
}

## @fn bool isPrimitive ($type)
# Check if this type is a primitive type
#
# This is equivalent to <tt>!isComposite()</tt>
# @return true if this is a primitive type, false otherwise
sub isPrimitive($)
{
    my ($type) = @_;
    return !isComposite($type);
}

## @fn bool isComposite ($type)
# Check if this type is a composite type
# @param type %SPL type
#
# This is equivalent to <tt>isTuple() || isCollection()</tt>
# @return true if this is a composite type, false otherwise
sub isComposite($)
{
    my ($type) = @_;
	return isOptional($type) || isTuple($type) || isCollection($type);
}

## @fn bool isCollection ($type)
# Check if this type is a collection type
# @param type %SPL type
#
# This is equivalent to <tt>isSet() || isMap() || isList() || isBSet() || isBMap() || isBList()</tt>
# @return true if this is a collection type, false otherwise
sub isCollection($)
{
    my ($type) = @_;
    return isSet($type) || isMap($type) || isList($type) ||
           isBSet($type) || isBMap($type) || isBList($type);
}

## @fn bool isBounded ($type)
# Check if this type is a bounded type
# @param type %SPL type
#
# This is equivalent to <tt>isBSet() || isBMap() || isBList() || isBString()</tt>
# @return true if this is a bounded type, false otherwise
sub isBounded($)
{
    my ($type) = @_;
    return isBString() || isBSet($type) || isBMap($type) || isBList($type);
}

## @fn bool isString ($type)
# Check if this is a string type
# @param type %SPL type
#
# This is equivalent to <tt>isRString() || isBString() || isUString()</tt>
# @return true if this is a string type, false otherwise
sub isString($)
{
    my ($type) = @_;
    return isRString($type) || isBString($type) || isUString($type);
}

## @fn bool isOrdered ($type)
# Check if this is an ordered type
# @param type %SPL type
#
# This is equivalent to <tt>isIntegral() || isFloatingpoint() || isEnum() || isString() || isTimestamp()</tt>
# @return true if this is an ordered type, false otherwise
sub isOrdered($)
{
    my ($type) = @_;
    return isIntegral($type) || isFloatingpoint($type) ||
           isString($type) || isTimestamp($type) || isEnum($type);
}

## @fn bool isNumeric ($type)
# Check if this is a numeric type
# @param type %SPL type
#
# This is equivalent to <tt>isIntegral() || isFloatingpoint() || isComplex()</tt>
# @return true if this is a numeric type, false otherwise
sub isNumeric($)
{
    my ($type) = @_;
    return isIntegral($type) || isFloatingpoint($type) || isComplex($type);
}

## @fn bool isIntegral ($type)
# Check if this is an integral numeric type
# @param type %SPL type
#
# This is equivalent to <tt>isSigned() || isUnsigned()</tt>
# @return true if this is an integral numeric type, false otherwise
sub isIntegral($)
{
    my ($type) = @_;
    return isSigned($type) || isUnsigned($type);
}

## @fn bool isFloatingpoint ($type)
# Check if this is a floating point type
# @param type %SPL type
#
# This is equivalent to <tt>isFloat() || isDecimal()</tt>
# @return true if this is a floating point type, false otherwise
sub isFloatingpoint($)
{
    my ($type) = @_;
    return isFloat($type) || isDecimal($type);
}

## @fn bool isFloat ($type)
# Check if this is a binary float type
# @param type %SPL type
#
# This is equivalent to <tt>isFloat32() || isFloat64()</tt>
# @return true if this is a binary float type, false otherwise
sub isFloat($)
{
    my ($type) = @_;
    return isFloat32($type) || isFloat64($type);
}

## @fn bool isDecimal ($type)
# Check if this is a decimal float type
# @param type %SPL type
#
# This is equivalent to <tt>isDecimal32() || isDecimal64() || isDecimal128()</tt>
# @return true if this is a decimal float type, false otherwise
sub isDecimal($)
{
    my ($type) = @_;
    return isDecimal32($type) || isDecimal64($type) || isDecimal128($type);
}

## @fn bool isComplex ($type)
# Check if this is a complex type
# @param type %SPL type
#
# This is equivalent to <tt>isComplex32() || isComplex64()</tt>
# @return true if this is a complex type, false otherwise
sub isComplex($)
{
    my ($type) = @_;
    return isComplex32($type) || isComplex64($type);
}

## @fn bool isSigned ($type)
# Check if this is a signed integral type
# @param type %SPL type
#
# This is equivalent to <tt>isInt8() || isInt16() || isInt32() || isInt64()</tt>
# @return true if this is a signed integral type, false otherwise
sub isSigned($)
{
    my ($type) = @_;
    return isInt8($type) || isInt16($type) || isInt32($type) || isInt64($type);
}

## @fn bool isUnsigned ($type)
# Check if this is an unsigned integral type
# @param type %SPL type
#
# This is equivalent to <tt>isUint8() || isUint16() || isUint32() || isUint64()</tt>
# @return true if this is an unsigned integral type, false otherwise
sub isUnsigned($)
{
    my ($type) = @_;
    return isUint8($type) || isUint16($type) || isUint32($type) || isUint64($type);
}

# Individual types

## @fn bool isTuple ($type)
# Check if this is a tuple type
# @param type %SPL type
# @return true if this is a tuple type, false otherwise
sub isTuple($)
{
    my ($type) = @_;
    return $type =~ /^tuple<.*>$/;
}

## @fn bool isList ($type)
# Check if this is a list type
# @param type %SPL type
# @return true if this is a list type, false otherwise
sub isList($)
{
    my ($type) = @_;
    return $type =~ /^list<.*>$/;
}

## @fn bool isSet ($type)
# Check if this is a set type
# @param type %SPL type
# @return true if this is a set type, false otherwise
sub isSet($)
{
    my ($type) = @_;
    return $type =~ /^set<.*>$/;
}

## @fn bool isMap ($type)
# Check if this is a map type
# @param type %SPL type
# @return true if this is a map type, false otherwise
sub isMap($)
{
    my ($type) = @_;
    return $type =~ /^map<.*>$/;
}

## @fn bool isBList ($type)
# Check if this is a bounded list type
# @param type %SPL type
# @return true if this is a bounded list type, false otherwise
sub isBList($)
{
    my ($type) = @_;
    return $type =~ /^list<.*>\[\d+\]$/;
}

## @fn bool isBSet ($type)
# Check if this is a bounded set type
# @param type %SPL type
# @return true if this is a bounded set type, false otherwise
sub isBSet($)
{
    my ($type) = @_;
    return $type =~ /^set<.*>\[\d+\]$/;
}

## @fn bool isBMap ($type)
# Check if this is a map type
# @param type %SPL type
# @return true if this is a map type, false otherwise
sub isBMap($)
{
    my ($type) = @_;
    return $type =~ /^map<.*>\[\d+\]$/;
}

## @fn bool isEnum ($type)
# Check if this is a enum type
# @param type %SPL type
# @return true if this is a enum type, false otherwise
sub isEnum($)
{
    my ($type) = @_;
    return $type =~ /^enum{.*}$/;
}

## @fn bool isBoolean ($type)
# Check if this is a boolean type
# @param type %SPL type
# @return true if this is a boolean type, false otherwise
sub isBoolean($)
{
    my ($type) = @_;
    return $type eq "boolean";
}

## @fn bool isTimestamp ($type)
# Check if this is a timestamp type
# @param type %SPL type
# @return true if this is a timestamp type, false otherwise
sub isTimestamp($)
{
    my ($type) = @_;
    return $type eq "timestamp";
}

## @fn bool isBlob ($type)
# Check if this is a blob type
# @param type %SPL type
# @return true if this is a blob type, false otherwise
sub isBlob($)
{
    my ($type) = @_;
    return $type eq "blob";
}

## @fn bool isRString ($type)
# Check if this is a rstring type
# @param type %SPL type
# @return true if this is a rstring type, false otherwise
sub isRString($)
{
    my ($type) = @_;
    return $type eq "rstring";
}

## @fn bool isBString ($type)
# Check if this is a bounded rstring type
# @param type %SPL type
# @return true if this is a bounded rstring type, false otherwise
sub isBString($)
{
    my ($type) = @_;
    return $type =~ /^rstring\[\d+\]$/;
}

## @fn bool isUString ($type)
# Check if this is a ustring type
# @param type %SPL type
# @return true if this is a ustring type, false otherwise
sub isUString($)
{
    my ($type) = @_;
    return $type eq "ustring";
}

## @fn bool isInt8 ($type)
# Check if this is a int8 type
# @param type %SPL type
# @return true if this is a int8 type, false otherwise
sub isInt8($)
{
    my ($type) = @_;
    return $type eq "int8";
}

## @fn bool isInt16 ($type)
# Check if this is a int16 type
# @param type %SPL type
# @return true if this is a int16 type, false otherwise
sub isInt16($)
{
    my ($type) = @_;
    return $type eq "int16";
}

## @fn bool isInt32 ($type)
# Check if this is a int32 type
# @param type %SPL type
# @return true if this is a int32 type, false otherwise
sub isInt32($)
{
    my ($type) = @_;
    return $type eq "int32";
}

## @fn bool isInt64 ($type)
# Check if this is a int64 type
# @param type %SPL type
# @return true if this is a int64 type, false otherwise
sub isInt64($)
{
    my ($type) = @_;
    return $type eq "int64";
}

## @fn bool isUint8 ($type)
# Check if this is a uint8 type
# @param type %SPL type
# @return true if this is a uint8 type, false otherwise
sub isUint8($)
{
    my ($type) = @_;
    return $type eq "uint8";
}

## @fn bool isUint16 ($type)
# Check if this is a uint16 type
# @param type %SPL type
# @return true if this is a uint16 type, false otherwise
sub isUint16($)
{
    my ($type) = @_;
    return $type eq "uint16";
}

## @fn bool isUint32 ($type)
# Check if this is a uint32 type
# @param type %SPL type
# @return true if this is a uint32 type, false otherwise
sub isUint32($)
{
    my ($type) = @_;
    return $type eq "uint32";
}

## @fn bool isUint64 ($type)
# Check if this is a uint64 type
# @param type %SPL type
# @return true if this is a uint64 type, false otherwise
sub isUint64($)
{
    my ($type) = @_;
    return $type eq "uint64";
}

## @fn bool isFloat32 ($type)
# Check if this is a float32 type
# @param type %SPL type
# @return true if this is a float32 type, false otherwise
sub isFloat32($)
{
    my ($type) = @_;
    return $type eq "float32";
}

## @fn bool isFloat64 ($type)
# Check if this is a float64 type
# @param type %SPL type
# @return true if this is a float64 type, false otherwise
sub isFloat64($)
{
    my ($type) = @_;
    return $type eq "float64";
}

## @fn bool isDecimal32 ($type)
# Check if this is a decimal32 type
# @param type %SPL type
# @return true if this is a decimal32 type, false otherwise
sub isDecimal32($)
{
    my ($type) = @_;
    return $type eq "decimal32";
}

## @fn bool isDecimal64 ($type)
# Check if this is a decimal64 type
# @param type %SPL type
# @return true if this is a decima64 type, false otherwise
sub isDecimal64($)
{
    my ($type) = @_;
    return $type eq "decimal64";
}

## @fn bool isDecimal128 ($type)
# Check if this is a decimal128 type
# @param type %SPL type
# @return true if this is a decima128 type, false otherwise
sub isDecimal128($)
{
    my ($type) = @_;
    return $type eq "decimal128";
}

## @fn bool isComplex32 ($type)
# Check if this is a complex32 type
# @param type %SPL type
# @return true if this is a complex32 type, false otherwise
sub isComplex32($)
{
    my ($type) = @_;
    return $type eq "complex32";
}

## @fn bool isComplex64 ($type)
# Check if this is a complex64 type
# @param type %SPL type
# @return true if this is a complex64 type, false otherwise
sub isComplex64($)
{
    my ($type) = @_;
    return $type eq "complex64";
}

## @fn bool isXml ($type)
# Check if this is an xml type
# @param type %SPL type
# @return true if this is an xml type, false otherwise
sub isXml($)
{
    my ($type) = @_;
    return $type =~ /^xml(<.*>)?/
}

## @fn bool getSchema ($type)
# If this is an xml type, get the schema
# @param type %SPL type
# @return If this is an xml type with a schema then the schema is returned, otherwise undef is returned.
sub getSchema($)
{
    my ($type) = @_;
    if (isXml($type)) {
        $type =~ /\<\\"(.*)\\"\>/;
        return $1 if (defined($1));
    }
    return undef;
}

## @fn string getElementType ($type)
# Get the element type for a list/set or bounded list/set
# @param type %SPL type
# @return the element type of a list/set or \a undef
#
# Example: if $t contains the value <tt>list<set<int32>></tt>
# <tt>SPL::CodeGen::getElementType($t)</tt> will return <tt>set<int32></tt>
#
sub getElementType($)
{
    my ($type) = @_;
    if ($type =~ /^list<(.*)>(\[\d+\])?$/) {
        return $1;
    }
    if ($type =~ /^set<(.*)>(\[\d+\])?$/) {
        return $1;
    }
    return undef;
}

## @fn string getKeyType ($type)
# Get the key type for a map or bounded map
# @param type %SPL type
# @return key type of a map or \a undef
#
# Example: if $t contains the value <tt>map<set<int32>,list<int32>></tt>
# <tt>SPL::CodeGen::getKeyType($t)</tt> will return <tt>set<int32></tt>
#
sub getKeyType($)
{
    my ($type) = @_;
    if ($type =~ /^map<(.*)>(\[\d+\])?$/) {
        $type = $1;
        # need to walk through to find first ',' not in <...>
        my $count = 0;
        for (my $i = 0; $i < length($type); $i++) {
            my $c = substr $type, $i, 1;
            if ($c eq '<' || $c eq '{') {
                $count++;
            } elsif ($c eq '>' || $c eq '}') {
                $count--;
            } elsif ($c eq ',' && $count == 0) {
                return substr $type, 0, $i;
            }
        }
    }
    return undef;
}

## @fn string getValueType ($type)
# Get the value type for a map or bounded map
# @param type %SPL type
# @return value type of a map or \a undef
#
# Example: if $t contains the value <tt>map<set<int32>,list<int32>></tt>
# <tt>SPL::CodeGen::getValueType($t)</tt> will return <tt>list<int32></tt>
#
sub getValueType($)
{
    my ($type) = @_;
    if ($type =~ /^map<(.*)>(\[\d+\])?$/) {
        $type = $1;
        # need to walk through to find first ',' not in <...>
        my $count = 0;
        for (my $i = 0; $i < length($type); $i++) {
            my $c = substr $type, $i, 1;
            if ($c eq '<' || $c eq '{') {
                $count++;
            } elsif ($c eq '>' || $c eq '}') {
                $count--;
            } elsif ($c eq ',' && $count == 0) {
                return substr $type, $i+1;
            }
        }
    }
    return undef;
}

## @fn bool hasBound ($type)
# Check if this is a bounded type. Bounded types may be formed from
# list/set/map/rstring.
# @param type %SPL type
# @return true if this type has a bound, fase otherwise
sub hasBound($)
{
    my ($type) = @_;
    if ($type =~ /^(list|set|map)<.*>\[(\d+)\]$/) {
        return 1;
    }
    if ($type =~ /^rstring\[(\d+)\]$/) {
        return 1;
    }
    return 0;
}

## @fn unsigned getBound ($type)
# Get the bound for a bounded type. Bounded types may be formed from
# list/set/map/rstring.
# @param type %SPL type
# @return bound of a bounded list/set/map/rstring/ustring or \a undef
sub getBound($)
{
    my ($type) = @_;
    if ($type =~ /^(list|set|map)<.*>\[(\d+)\]$/) {
        return $2;
    }
    if ($type =~ /^rstring\[(\d+)\]$/) {
        return $1;
    }
    return undef;
}

## @fn list getAttributeTypes ($type)
# Get a list of the attribute types for a tuple type
# @param type %SPL type
# @return list of types of attributes in a tuple or \a undef
# For example, the %SPL type for the output port of this operator:\n
#
# Example: if $i contains <tt>"tuple<rstring a,int32 b>"</tt>
#
# <tt>SPL::CodeGen::Type::getAttributeTypes($type)</tt> will return <tt>["rstring", "int32"]</tt>
sub getAttributeTypes($)
{
    my ($type) = @_;
    if ($type =~ /^tuple<(.*)>$/) {
        $type = $1;
        my @types;
        while (index ($type, ',') >= 0) {
            # need to walk through to find first ',' not in <...>
            my $count = 0;
            my $found = 0;
            for (my $i = 0; $i < length($type); $i++) {
                my $c = substr $type, $i, 1;
                if ($c eq '<' || $c eq '{') {
                    $count++;
                } elsif ($c eq '>' || $c eq '}') {
                    $count--;
                } elsif ($c eq ',' && $count == 0) {
                    my $comp = substr $type, 0, $i;
                    push (@types, substr ($comp, 0, rindex ($comp, ' ')));
                    $type = substr $type, $i+1;
                    $found = 1;
                    last;
                }
            }
            last if !$found; # the must have been a comma in a nested map
        }
        push (@types, substr ($type, 0, rindex ($type, ' ')));
        return @types;
    }
    return undef;
}

## @fn list getAttributeNames ($type)
# Return a list of the attribute names for a tuple type
# @param type %SPL type
# @return list of names of attributes in a tuple or \a undef
#
# Example: if $i contains <tt>"tuple<rstring a,int32 b>"</tt>
#
# <tt>SPL::CodeGen::Type::getAttributeNames($type)</tt> will return <tt>["a", "b"]</tt>
sub getAttributeNames($)
{
    my ($type) = @_;
    if ($type =~ /^tuple<(.*)>$/) {
        $type = $1;
        my @names;
        while (index ($type, ',') >= 0) {
            # need to walk through to find first ',' not in <...>
            my $count = 0;
            my $found = 0;
            for (my $i = 0; $i < length($type); $i++) {
                my $c = substr $type, $i, 1;
                if ($c eq '<' || $c eq '{') {
                    $count++;
                } elsif ($c eq '>' || $c eq '}') {
                    $count--;
                } elsif ($c eq ',' && $count == 0) {
                    my $comp = substr $type, 0, $i;
                    push (@names, substr ($comp, rindex($comp, ' ')+1));
                    $type = substr $type, $i+1;
                    $found = 1;
                    last;
                }
            }
            last if !$found; # the must have been a comma in a nested map
        }
        push (@names, substr ($type, rindex($type, ' ')+1));
        return @names;
    }
    return undef;
}

## @fn list getEnumValues ($type)
# Return a list of the value for an enum type
# @param type %SPL type
# @return list of values in an enum or \a undef
#
# Example: if $t contains the value <tt>enum{A,B,C}</tt>
# <tt>SPL::CodeGen::getEnumValues($t)</tt> will return <tt>[ 'A', 'B', 'C']</tt>.
#
sub getEnumValues($)
{
    my ($type) = @_;
    if ($type =~ /^enum{(.*)}$/) {
        $type = $1;
        my @names;
        my $i = index $type, ',';
        while ($i >= 0) {
            push @names, substr $type, 0, $i;
            $type = substr $type, $i+1;
            $i = index $type, ',';
        }
        push @names, $type;
        return @names;
    }
    return undef;
}

## @fn bool hasBoundedSetOrMap ($type)
# Check if this type contains a bounded set or map type
# @param type %SPL type
# @return true if this type contains a bounded set of map type, false otherwise
sub hasBoundedSetOrMap
{
    my ($type) = @_;
    return 1 if(isBSet($type) || isBMap($type));
    if(isList($type) || isBList($type) || isSet($type)) {
        return hasBoundedSetOrMap(getElementType($type));
    }
    if (isMap($type)) {
        return hasBoundedSetOrMap(getKeyType($type)) ||
               hasBoundedSetOrMap(getValueType($type));
    }
    if (isTuple($type)) {
        my @atypes = getAttributeTypes($type);
        foreach my $atype (@atypes) {
            return 1 if(hasBoundedSetOrMap($atype));
        }
    }
    return 0;
}

1;
