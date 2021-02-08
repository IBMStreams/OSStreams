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

## @class SPL::Operator::Instance::OptionalContext
# \brief Class that represents an optional operator context.
package SPL::Operator::Instance::OptionalContext;

use strict;
use warnings;

#DOXYGEN_SKIP_FOR_USERS

sub new {
  my ($class) = @_;
  my $self = {};
  return bless($self, $class);
}

1;
