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

package SPL::ModelHelper;

use strict;
use warnings;

sub booleanTest
{
    my $v = $_[0];
    return 1 if ( $v and $v ne "false");
    return 0;
}

sub cleanType
{
    my ($type) = @_;
    # SPL types may have embedded double quotes.  Escape them.
    $type =~ s/"/\\"/g;
    return $type;
}

1;
