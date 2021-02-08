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
# SPL runtime messages
#
GenerateMessages(gen-header StreamsSPLRuntimeMessages SPL/Runtime/Common ApplicationRuntimeMessage h STREAMS_SPL_MESSAGES)
GenerateMessages(gen-header StreamsSPLRuntimeMessages SPL/Runtime/Common RuntimeMessage            h STREAMS_SPL_MESSAGES)
#
# SPL compiler messages
#
GenerateMessages(gen-header StreamsSPLCompilerMessages SPL/Core                CompilerError   h  STREAMS_SPL_MESSAGES)
GenerateMessages(gen-header StreamsSPLCompilerMessages SPL/Core                CompilerMessage h  STREAMS_SPL_MESSAGES)
GenerateMessages(gen-header StreamsSPLCompilerMessages SPL/FrontEnd            FrontEndErrors  h  STREAMS_SPL_MESSAGES)
GenerateMessages(gen-header StreamsSPLCompilerMessages SPL/Optimizer           OptimizerError  h  STREAMS_SPL_MESSAGES)
GenerateMessages(gen-perl   StreamsSPLCompilerMessages SPL/script/impl/bin/SPL Msg             pm STREAMS_SPL_MESSAGES)
#
# Installable message data files
#
GenerateMessageData(StreamsSPLCompilerMessages SPL_MESSAGE_DATA)
GenerateMessageData(StreamsSPLRuntimeMessages  SPL_MESSAGE_DATA)
#
# Installable message script files
#
GenerateMessages(gen-perl StreamsSPLCompilerMessages . StreamsSPLCompilerMessages pm SPL_MESSAGE_SCRIPTS)
GenerateMessages(gen-perl StreamsSPLRuntimeMessages  . StreamsSPLRuntimeMessages  pm SPL_MESSAGE_SCRIPTS)
#
# Custom target
#
add_custom_target(streams_spl_messages DEPENDS ${STREAMS_SPL_MESSAGES})
add_custom_target(streams_spl_messages_data ALL DEPENDS ${STREAMS_SPL_MESSAGES} ${SPL_MESSAGE_DATA} ${SPL_MESSAGE_SCRIPTS})

install(FILES ${SPL_MESSAGE_DATA} DESTINATION messages)
install(FILES ${SPL_MESSAGE_SCRIPTS} DESTINATION system/impl/bin)
