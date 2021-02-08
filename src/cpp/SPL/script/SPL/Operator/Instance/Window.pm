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

## @class SPL::Operator::Instance::Window
# \brief Class that represents a window on an input port.
package SPL::Operator::Instance::Window;

use strict;
use warnings;

use SPL::ModelHelper;
use SPL::Operator::Instance::Expression;
use SPL::Operator::Instance::SourceLocation;

## @var unsigned TUMBLING An integer value representing a tumbling window.
our $TUMBLING = 1;
## @var unsigned SLIDING An integer value representing a sliding window.
our $SLIDING  = 2;
## @var unsigned TIME_INTERVAL An integer value represending a timeInterval window.
our $TIME_INTERVAL = 3;

## @var unsigned TIME An integer value representing a window policy of time.
our $TIME   = 1;
## @var unsigned COUNT An integer value representing a window policy of count.
our $COUNT  = 2;
## @var unsigned PUNCT An integer value representing a window policy of punctuation.
our $PUNCT  = 3;
## @var unsigned DELTA An integer value representing a window policy of attribute delta.
our $DELTA = 4;
## @var unsigned INVALID An integer value representing a window without an eviction or trigger policy.
our $INVALID = 5;

## @method SourceLocation getSourceLocation()
# Get the %SPL source location that corresponds to this window.
# @return the SourceLocation object
sub getSourceLocation
{
  my ($self) = @_;
  return $self->{loc_};
}

## @method unsigned getPortIndex()
# Get the port index for the window object.
# @return the port index
sub getPortIndex
{
  my ($self) = @_;
  return $self->{portIndex_};
}

## @method unsigned isPartitioned()
# Check if the window is a partitioned window.
# @return 1 if the window is partitioned, 0 otherwise
sub isPartitioned
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest(
      $self->{xml_}->{partitioned}->[0]);
}

## @method unsigned hasPartitionEvictionPolicy()
# Check if the window (which must be partitioned) has a partition eviction policy
# @return 1 if the window has a partition eviction policy, 0 otherwise
sub hasPartitionEvictionPolicy
{
  my ($self) = @_;
  return SPL::ModelHelper::booleanTest(
      $self->{xml_}->{partitionEviction}->[0]);
}

## @method string getPartitionEvictionPolicyKind()
# Return the partition eviction policy kind of the window (which must be partitioned)
# @return one of: partitionAge, partitionCount, tupleCount
sub getPartitionEvictionPolicyKind
{
  my ($self) = @_;
  return $self->{xml_}->{partitionEviction}->[0]->{kind}->[0];
}

## @method Expression getPartitionEvictionPolicyAge()
# Return the number of seconds that a partition must be idle before it is evicted
# @pre <tt>hasPartitionEvictionPolicy()</tt>
# @return expression corresponding to the age
sub getPartitionEvictionPolicyAge
{
  my ($self) = @_;
  return $self->{partitionEvictionAge_};
}

## @method Expression getPartitionEvictionPolicyCount()
# Return the number of tuples/partitions that must be presented before partition eviction is done
# @pre <tt>hasPartitionEvictionPolicy()</tt>
# @return expression corresponding to the count
sub getPartitionEvictionPolicyCount
{
  my ($self) = @_;
  return $self->{partitionEvictionCount_};
}

## @method unsigned isSliding()
# Check if the window is a sliding window.
# @return 1 if the window is sliding, 0 otherwise
sub isSliding
{
  my ($self) = @_;
  my $isSliding = (! ($self->isTimeInterval())) &&  $self->hasTriggerPolicy();
  return $isSliding;
}

## @method unsigned isTumbling()
# Check if the window is a tumbling window.
# @return 1 if the window is tumbling, 0 otherwise
sub isTumbling
{
  my ($self) = @_;
  my $isTumbling = (! ($self->isTimeInterval())) && (! ($self->hasTriggerPolicy()));
  return $isTumbling;
}

## @method unsigned isTimeInterval()
# Check if the window is a time interval window.
# @return 1 if the window is time interval, 0 otherwise
sub isTimeInterval
{
  my ($self) = @_;
  my $isTimeInterval = defined($self->{intervalDuration_});
  return $isTimeInterval;
}

## @method unsigned hasIntervalDuration
# Check if the window has an interval duration.  This applies only to
# a timeInterval window.
# @return 1 if the window has an interval duration, 0 otherwise.
sub hasIntervalDuration
{
  my ($self) = @_;
  return defined($self->{intervalDuration_});
}

## @method unsigned hasCreationPeriod
# Check if the window has a creation period.  This applies only to
# a timeInterval window.
# @return 1 if the window has a creation period, 0 otherwise.
sub hasCreationPeriod
{
  my ($self) = @_;
  return defined($self->{creationPeriod_});
}

## @method unsigned hasIntervalOffset
# Check if the window has an interval offset.  This applies only to
# a timeInterval window.
# @return 1 if the window has an interval offset, 0 otherwise.
sub hasIntervalOffset
{
  my ($self) = @_;
  return defined($self->{intervalOffset_});
}

## @method unsigned hasDiscardAge
# Check if the window has a discard age.  This applies only to
# a timeInterval window.
# @return 1 if the window has a discard age, 0 otherwise.
sub hasDiscardAge
{
  my ($self) = @_;
  return defined($self->{discardAge_});
}

## @method Expression getIntervalDuration
# Return the duration, in seconds, of each interval of a time interval window.
# @pre <tt>hasIntervalDuration()</tt>
# @return expression corresponding to the interval duration
sub getIntervalDuration
{
  my ($self) = @_;
  return $self->{intervalDuration_};
}

## @method Expression getCreationPeriod
# Return the period, in seconds, of the creation of a new time interval.
# @pre <tt>hasCreationPeriod()</tt>
# @return expression corresponding to the creation period
sub getCreationPeriod
{
  my ($self) = @_;
  return $self->{creationPeriod_};
}

## @method Expression getIntervalOffset
# Return the interval offset, in seconds, of a time interval window.
# @pre <tt>hasIntervalOffset()</tt>
# @return expression corresponding to the interval offset
sub getIntervalOffset
{
  my ($self) = @_;
  return $self->{intervalOffset_};
}

## @method Expression getDiscardAge
# Return the discard age, in seconds, of the time interval window.
# @pre <tt>hasDiscardAge()</tt>
# @return expression corresponding to the discard age
sub getDiscardAge
{
  my ($self) = @_;
  return $self->{discardAge_};
}

## @method unsigned getWindowType()
# Get the type of the window, one of:
#   - SPL::Operator::Instance::Window::TUMBLING,
#   - SPL::Operator::Instance::Window::SLIDING,
#   - SPL::Operator::Instance::Window::TIME_INTERVAL;
#
# @return the window's type
sub getWindowType {
  my ($self) = @_;
  return $SLIDING
    if($self->isSliding());
  return $TUMBLING
    if ($self->isTumbling());
  return $TIME_INTERVAL
    if ($self->isTimeInterval());
}

## @method unsigned getEvictionPolicyType()
# Get the type eviction policy, one of:
#   - SPL::Operator::Instance::Window::TIME,
#   - SPL::Operator::Instance::Window::COUNT,
#   - SPL::Operator::Instance::Window::PUNCT,
#   - SPL::Operator::Instance::Window::DELTA,
#   - SPL::Operator::Instance::Window::INVALID.
# @return the window evitction policy
sub getEvictionPolicyType
{
  my ($self) = @_;
  if ($self->hasEvictionPolicy()) {
      my $type = $self->{xml_}->{evictionPolicy}->[0]->{type}->[0];
      return $self->obtainWindowType_($type);
  }
  return $INVALID;
}

## @method unsigned getEvictionPolicySize()
# Get the size for the eviction policy.
# @pre <tt>getEvictionPolicyType()!=$SPL::Operator::Instance::Window::PUNCT</tt>
# @return Returns the policy size.
sub getEvictionPolicySize
{
  my ($self) = @_;
  return $self->{evictionPolicySize_};
}

## @method string getEvictionPolicyAttribute()
# Get the attribute of the eviction policy.
# @pre <tt>getEvictionPolicyType()==$SPL::Operator::Instance::Window::DELTA</tt>
# @return an expression identifying the attribute
#
# Example, if the eviction policy is delta (iAttr, 10), getEvictionPolicyAttribute() will return the Expression for iAttr
sub getEvictionPolicyAttribute
{
  my ($self, $index) = @_;
  return $self->{evictionPolicyAttribute_};
}

## @method unsigned hasEvictionPolicy()
# Check if the window has an eviction policy.
# @return 1 if the window has an eviction policy, 0 otherwise
sub hasEvictionPolicy
{
  my ($self) = @_;
  return 1
    if ($self->{xml_}->{evictionPolicy});
  return 0;
}

## @method unsigned hasTriggerPolicy()
# Check if the window has a trigger policy.
# @return 1 if the window has a trigger policy, 0 otherwise
sub hasTriggerPolicy
{
  my ($self) = @_;
  return 1
    if ($self->{xml_}->{triggerPolicy});
  return 0;
}

## @method unsigned getTriggerPolicyType()
# Get the trigger policy type, one of:
#   - SPL::Operator::Instance::Window::TIME,
#   - SPL::Operator::Instance::Window::COUNT,
#   - SPL::Operator::Instance::Window::DELTA,
#   - SPL::Operator::Instance::Window::INVALID;
# @pre <tt>hasTriggerPolicy()==1</tt>
# @return the trigger policy
sub getTriggerPolicyType
{
  my ($self, $index) = @_;
  if ($self->hasTriggerPolicy()) {
      my $type = $self->{xml_}->{triggerPolicy}->[0]->{type}->[0];
      return $self->obtainWindowType_($type);
  }
  return $INVALID;
}

## @method unsigned getTriggerPolicySize()
# Get the size of the trigger policy
# @pre <tt>hasTriggerPolicy()==1</tt>
# @return Returns the policy size.
sub getTriggerPolicySize
{
  my ($self, $index) = @_;
  return $self->{triggerPolicySize_};
}

## @method string getTriggerPolicyAttribute()
# Get the attribute of the trigger policy.
# @pre <tt>getTriggerPolicyType()==$SPL::Operator::Instance::Window::DELTA</tt>
# @return an expression identifying the attribute
#
# Example, if the trigger policy is delta (iAttr, 10), getTriggerPolicyAttribute() will return the Expression for iAttr
sub getTriggerPolicyAttribute
{
  my ($self, $index) = @_;
  return $self->{triggerPolicyAttribute_};
}

# DOXYGEN_SKIP_FOR_USERS

## @fn private unsigned obtainWindowType_()
sub obtainWindowType_() {
  my ($self, $type) = @_;
  if($type eq "count"){
    return $COUNT;
  } elsif ($type eq "time"){
    return $TIME;
  } elsif ($type eq "punct"){
    return $PUNCT;
  } elsif ($type eq "delta"){
    return $DELTA;
  } else {
    die "ERROR: Unknown window component type '${type}'\n";
  }
}

sub new {
  my ($class, $xml, $index) = @_;

  my $self = {
    xml_        => $xml,
    evictionPolicySize_  => undef,
    evictionPolicyAttribute_ => undef,
    triggerPolicySize_  => undef,
    triggerPolicyAttribute_ => undef,
    intervalDuration_ => undef,
    intervalOffset_ => undef,
    creationPeriod_ => undef,
    discardAge_ => undef,
    portIndex_ => $index,
    loc_ => undef
  };

  if($self->{xml_}->{evictionPolicy}) {
      if($self->{xml_}->{evictionPolicy}->[0]->{size}) {
          $self->{evictionPolicySize_} = SPL::Operator::Instance::Expression->new(
            $self->{xml_}->{evictionPolicy}->[0]->{size}->[0], undef);
      }
      if($self->{xml_}->{evictionPolicy}->[0]->{attribute}) {
          $self->{evictionPolicyAttribute_} = SPL::Operator::Instance::Expression->new(
              $self->{xml_}->{evictionPolicy}->[0]->{attribute}->[0], undef);
      }
  }
  if($self->{xml_}->{triggerPolicy}) {
      if($self->{xml_}->{triggerPolicy}->[0]->{size}) {
          $self->{triggerPolicySize_} = SPL::Operator::Instance::Expression->new(
              $self->{xml_}->{triggerPolicy}->[0]->{size}->[0], undef);
      }
      if($self->{xml_}->{triggerPolicy}->[0]->{attribute}) {
          $self->{triggerPolicyAttribute_} = SPL::Operator::Instance::Expression->new(
              $self->{xml_}->{triggerPolicy}->[0]->{attribute}->[0], undef);
      }
  }
  if($self->{xml_}->{partitionEviction}) {
      if($self->{xml_}->{partitionEviction}->[0]->{age}) {
          $self->{partitionEvictionAge_} = SPL::Operator::Instance::Expression->new(
              $self->{xml_}->{partitionEviction}->[0]->{age}->[0]);
      }
      if($self->{xml_}->{partitionEviction}->[0]->{count}) {
          $self->{partitionEvictionCount_} = SPL::Operator::Instance::Expression->new(
              $self->{xml_}->{partitionEviction}->[0]->{count}->[0]);
      }
  }

  if($self->{xml_}->{eventTimePolicy}) {
      if($self->{xml_}->{eventTimePolicy}->[0]->{intervalDuration}) {
          $self->{intervalDuration_} = SPL::Operator::Instance::Expression->new(
              $self->{xml_}->{eventTimePolicy}->[0]->{intervalDuration}->[0]);
      }
      if($self->{xml_}->{eventTimePolicy}->[0]->{creationPeriod}) {
          $self->{creationPeriod_} = SPL::Operator::Instance::Expression->new(
              $self->{xml_}->{eventTimePolicy}->[0]->{creationPeriod}->[0]);
      }
      if($self->{xml_}->{eventTimePolicy}->[0]->{discardAge}) {
          $self->{discardAge_} = SPL::Operator::Instance::Expression->new(
              $self->{xml_}->{eventTimePolicy}->[0]->{discardAge}->[0]);
      }
      if($self->{xml_}->{eventTimePolicy}->[0]->{intervalOffset}) {
          $self->{intervalOffset_} = SPL::Operator::Instance::Expression->new(
              $self->{xml_}->{eventTimePolicy}->[0]->{intervalOffset}->[0]);
      }
  }

  $self->{loc_} = SPL::Operator::Instance::SourceLocation->new($self->{xml_}->{sourceLocation}->[0]);

  return bless($self, $class);
}


1;
