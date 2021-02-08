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

set(CMAKE_POSITION_INDEPENDENT_CODE 1)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Tecla_INCLUDE_DIRS}
  ${XercesC_INCLUDE_DIR})

set(SKIP_SOURCES
  main.cpp
  perfect_hash.cpp
  DistilleryServiceClient.cpp
  DistilleryServiceClient.h
  ResourceManagementUtils.cpp
  ResourceManagementUtils.h
  TSharedQueueImpl.h)

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)
list(REMOVE_ITEM SOURCES ${SKIP_SOURCES})

add_format_target(utils_format SOURCES)
add_lint_target(utils_lint gnu++03 SOURCES)

set(APPUTILS_SOURCES
  BacktraceDumper.cpp BacktraceDumper.h
  Base64.cpp Base64.h
  CV.cpp CV.h
  Directory.cpp Directory.h
  DistilleryApplicationInstantiator.cpp
  DistilleryException.cpp DistilleryException.h
  DistilleryExceptionCode.cpp DistilleryExceptionCode.h
  Formatter.cpp Formatter.h
  Mutex.cpp Mutex.h
  RWMutex.cpp RWMutex.h
  ResourceBundleHelper.cpp ResourceBundleHelper.h
  SerializationBuffer.cpp SerializationBuffer.h
  SpinLock.cpp SpinLock.h
  SupportFunctions.cpp SupportFunctions.h
  Thread.cpp Thread.h)

if(${ENABLE_DEBUG_INSTRUMENTATION})
  list(APPEND APPUTILS_SOURCES
    HashStream.cpp HashStream.h
    RegEx.cpp RegEx.h
    SymbolReader.cpp SymbolReader.h)
endif()

add_library(apputils OBJECT ${APPUTILS_SOURCES})
add_dependencies(apputils schema_xsd streams_messages)

list(REMOVE_ITEM SOURCES ${APPUTILS_SOURCES})
add_library(utils OBJECT ${SOURCES})
add_dependencies(utils schema_xsd streams_messages)

set(UTILS_HEADERS
  CV.h
  DSTAutoConf.h
  DistilleryException.h
  DistilleryExceptionCode.h
  HashMapHelpers.h
  HostToNetwork.h
  Mutex.h
  RWLock.h
  SpinLock.h
  Thread.h
  UTILSTypes.h)

install(
  FILES
  ${UTILS_HEADERS}
  DESTINATION
  system/impl/include/UTILS)
