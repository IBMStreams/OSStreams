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

#
# Runtime messages
#
GenerateMessages(runtime-gen-header StreamsRuntimeMessages UTILS RuntimeMessages       h STREAMS_MESSAGES)
GenerateMessages(runtime-gen-header StreamsRuntimeMessages NAM   NAMMessages           h STREAMS_MESSAGES)
GenerateMessages(runtime-gen-header StreamsRuntimeMessages SAM   SAMMessages           h STREAMS_MESSAGES)
GenerateMessages(runtime-gen-header StreamsRuntimeMessages SEC   AASMessages           h STREAMS_MESSAGES)
GenerateMessages(runtime-gen-header StreamsRuntimeMessages SCH   SCHMessages           h STREAMS_MESSAGES) 
GenerateMessages(runtime-gen-header StreamsRuntimeMessages SRM   SRMMessages           h STREAMS_MESSAGES)
GenerateMessages(runtime-gen-header StreamsRuntimeMessages UTILS RuntimeCommonMessages h STREAMS_MESSAGES)
GenerateMessages(runtime-gen-header StreamsRuntimeMessages UTILS UTILSMessages         h STREAMS_MESSAGES)
#
# Installable message data files
#
GenerateMessageData(StreamsRuntimeMessages MESSAGE_DATA)
#
# Installable message script files
#
GenerateMessages(gen-perl StreamsRuntimeMessages . StreamsRuntimeMessages pm MESSAGE_SCRIPTS)
#
# Custom target
#
add_custom_target(streams_messages DEPENDS ${STREAMS_MESSAGES})
add_dependencies(streams_messages streams_spl_messages)

add_custom_target(streams_messages_data ALL DEPENDS ${MESSAGE_DATA} ${MESSAGE_SCRIPTS})

install(FILES ${MESSAGE_DATA} DESTINATION messages)
install(FILES ${MESSAGE_SCRIPTS} DESTINATION system/impl/bin)
