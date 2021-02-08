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

include_directories(
  ${Decnumber_INCLUDE_DIRS}
  ${JNI_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIR}
  ${LibANTLR3C_INCLUDE_DIRS}
  ${Xqilla_INCLUDE_DIR})

add_subdirectory(CodeGen)
add_subdirectory(Core)
add_subdirectory(Debugger)
add_subdirectory(FrontEnd)
add_subdirectory(include)
add_subdirectory(message)
add_subdirectory(Optimizer)
add_subdirectory(pkgconfigcfg)
add_subdirectory(Runtime)
add_subdirectory(template)
add_subdirectory(TestSrc)
add_subdirectory(toolkit)
add_subdirectory(Utility)


if(GENERATE_DOCUMENTATION)
  add_subdirectory(doc)
endif(GENERATE_DOCUMENTATION)

set(RUNTIME_SOURCE_HEADERS
  #
  # Window
  #
  Runtime/Window/AgeLRUPartitionEvictionImpl.h
  Runtime/Window/CountCountSlidingWindowImpl.h
  Runtime/Window/CountDeltaSlidingWindowImpl.h
  Runtime/Window/CountStarSlidingWindowImpl.h
  Runtime/Window/CountTimeSlidingWindowImpl.h
  Runtime/Window/CountTumblingWindowImpl.h
  Runtime/Window/DeltaCountSlidingWindowImpl.h
  Runtime/Window/DeltaDeltaSlidingWindowImpl.h
  Runtime/Window/DeltaStarSlidingWindowImpl.h
  Runtime/Window/DeltaTimeSlidingWindowImpl.h
  Runtime/Window/DeltaTumblingWindowImpl.h
  Runtime/Window/PartitionAgePartitionEvictionImpl.h
  Runtime/Window/PartitionCountLRUPartitionEvictionImpl.h
  Runtime/Window/PartitionCountPartitionEvictionImpl.h
  Runtime/Window/PartitionEvictionImpl.h
  Runtime/Window/PunctTumblingWindowImpl.h
  Runtime/Window/Serialization.h
  Runtime/Window/SlidingTypedefs.h
  Runtime/Window/SlidingWindowImpl.h
  Runtime/Window/StarCountSlidingWindowImpl.h
  Runtime/Window/StarDeltaSlidingWindowImpl.h
  Runtime/Window/StarTimeSlidingWindowImpl.h
  Runtime/Window/TimeCountSlidingWindowImpl.h
  Runtime/Window/TimeDeltaSlidingWindowImpl.h
  Runtime/Window/TimeIntervalWindowImpl.h
  Runtime/Window/TimeIntervalWindowPaneImpl.h
  Runtime/Window/TimeStarSlidingWindowImpl.h
  Runtime/Window/TimeTimeSlidingWindowImpl.h
  Runtime/Window/TimeTumblingWindowImpl.h
  Runtime/Window/TumblingTypedefs.h
  Runtime/Window/TumblingWindowImpl.h
  Runtime/Window/TumblingWindowSummarizerFactory.h
  Runtime/Window/TupleCountLRUPartitionEvictionImpl.h
  Runtime/Window/TupleCountPartitionEvictionImpl.h
  Runtime/Window/Typedefs.h
  Runtime/Window/WindowImpl.h
  Runtime/Window/WindowThread.h
  #
  # Common
  #
  Runtime/Common/BlockingQueue.h
  Runtime/Common/RuntimeDebugAspect.h
  Runtime/Common/RuntimeExceptionImpl.h
  #
  # Operator
  #
  Runtime/Operator/OperatorForwardDeclarations.h
  Runtime/Operator/EventTime/WatermarkHandler.h 
  Runtime/Operator/State/IncrementalCheckpointIndex.h
  #
  # Processing Element
  #
  Runtime/ProcessingElement/PE.h
  Runtime/ProcessingElement/PEForwardDeclarations.h
  Runtime/ProcessingElement/StandaloneApplication.h
  #
  # Serialization
  #
  Runtime/Serialization/StreamByteBuffer.h
  Runtime/Serialization/VirtualByteBuffer.h
  Runtime/Serialization/VirtualByteBufferHelpers.h
  #
  # Function
  #
  Runtime/Function/SPLCast.h
  Runtime/Function/SPLJavaFunction.h
  #
  # Utility
  #
  Runtime/Utility/JNIUtils.h
  Runtime/Utility/LogTraceMessage.h
  Runtime/Utility/Message.h
  Runtime/Utility/MessageFormatter.h
  Runtime/Utility/Payload.h
  Runtime/Utility/PayloadContainer.h
  Runtime/Utility/RuntimeUtility.h
  Runtime/Utility/Singleton.h
  Runtime/Utility/Visibility.h
  Runtime/Utility/WatermarkPunctPayload.h
  #
  # Type
  #
  Runtime/Type/TypeTMPHelper.h   
  Runtime/Type/TypeHelper.h)

foreach(HEADER ${RUNTIME_SOURCE_HEADERS})
  SplCleanupHeader(${CMAKE_CURRENT_SOURCE_DIR} ${HEADER} CLEANED_RUNTIME_HEADERS)
endforeach()

set(RUNTIME_BINARY_HEADERS
  Runtime/Common/ApplicationRuntimeMessage.h)

foreach(HEADER ${RUNTIME_BINARY_HEADERS})
  SplCleanupHeader(${CMAKE_CURRENT_BINARY_DIR} ${HEADER} CLEANED_RUNTIME_HEADERS)
endforeach()

add_custom_target(spl_runtime_cleaned_headers ALL DEPENDS ${CLEANED_RUNTIME_HEADERS})
add_dependencies(spl_runtime_cleaned_headers streams_messages)

#
# Documentation
#

add_custom_target(spl_doc_generate
  DEPENDS
  spl_dev_doc_generate
  spl_usr_doc_generate)

#
# Format
#
add_custom_target(spl_format
  DEPENDS
  spl_core_format
  spl_codegen_format
  spl_debugger_format
  spl_frontend_format
  spl_optimizer_format
  spl_runtime_format
  spl_testsrc_format
  spl_utility_format)

add_custom_target(spl_lint
  DEPENDS
  spl_codegen_lint
  spl_debugger_lint
  spl_frontend_lint
  spl_optimizer_lint
  spl_runtime_lint
  spl_testsrc_lint
  spl_utility_lint)

#
# Installation
#

install(
  PROGRAMS
  script/spl-app-info
  script/spl-make-doc
  script/spl-make-operator
  script/spl-verify-operator-model
  script/spl-verify-function-model
  script/spl-verify-toolkit-info
  script/spl-verify-toolkit-list
  DESTINATION
  bin)

install(
  PROGRAMS
  script/impl/bin/spl-bundle-builder
  script/impl/bin/spl-check-support
  script/impl/bin/spl-code-builder
  script/impl/bin/spl-code-gen-driver
  script/impl/bin/spl-format-code
  script/impl/bin/spl-truncate
  script/impl/bin/spl-mmp-driver
  script/impl/bin/spl-mixedmode-processor
  script/impl/bin/spl-make-resource
  script/impl/bin/spl-verification-driver
  DESTINATION
  system/impl/bin)

install(DIRECTORY script/SPL DESTINATION bin)
install(DIRECTORY script/impl/bin/SPL DESTINATION system/impl/bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/script/impl/bin/SPL/Msg.pm DESTINATION system/impl/bin/SPL)

install(
  DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}/include/SPL/Runtime/Function
  DESTINATION
  include/SPL/Runtime)

install(FILES etc/uncrustify.verbose.cfg DESTINATION etc/uncrustify)
