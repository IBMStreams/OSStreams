#!/usr/bin/env bash

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

# must be using installation tree to use this script
if [ "$STREAMS_INSTALL" == "" ]; then
  echo  "STREAMS_INSTALL environment variable must be set"
  exit 1
fi

PKG_CONFIG_ALT_PATH=$STREAMS_INSTALL/etc/pkgconfig/cfg
distinst=$STREAMS_INSTALL

# read_pc_file --cflags|--libs pc_file_name_without_suffix
# is written to parse pc files that are located
# under $STREAMS_INSTALL/etc/pkgconfig/cfg only
#
# read_pc_file evaluates lines (expects these lines to be variable assignments)
# above the "Name:xxxx" lines, then based on the first parameter, looks for
# the respective "Cflags:xxxx" or "Libs:xxx" line, extracts and outputs flags from this line
# if there's a "Requires:xxx" line, read_pc_files will be called recursively to handle other required pc files
# the flags will be outputted in order depends on which line ("Cflags:"|"Libs:" or "Requires:") in the given pc file appears first
#
# this script is meant to be a simplified alternative to the linux pkg-config command
# it is written to resolve the problem with the $STREAMS_INSTALL path containing double byte characters (rtc35394)
# the linux pkg-config command (version 0.27.1 or higher) does not support non-ASCII character as they added
# a strdup_escape_shell function to handle file path (https://bugs.freedesktop.org/show_bug.cgi?id=86012)
read_pc_file() {
    is_done=false
    while read -r line
    do
        if [[ $line == Name:* ]]; then
            is_done=true
        fi
        if [[ $is_done == false ]]; then
            eval $line
        else
            if [[ $line == Cflags:* && "$1" == "--cflags" ]]; then
                pfix="Cflags: "
                outp=${line#$pfix}
                eval echo -n "$outp\ "
            elif [[ $line == Libs:* && "$1" == "--libs" ]]; then
                pfix="Libs: "
                outp=${line#$pfix}
                eval echo -n "$outp\ "
            elif [[ $line == Requires:* ]]; then
                pfix="Requires: "
                line=${line#$pfix}
                # process the required pc files in the same order as
                # what the "Requires:xxxx" line specifies
                for pcfile in $line
                do
                    outp="$(read_pc_file $1 $pcfile)"
                    eval echo -n "$outp\ "
                done
            fi
        fi
    done < "$PKG_CONFIG_ALT_PATH/$2.pc"
}

result=$(read_pc_file $1 $2)
echo "$result"
