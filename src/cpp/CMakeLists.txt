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
# Global includes
#

include_directories(${Boost_INCLUDE_DIRS})

#
# Build subdirectories
#

add_subdirectory(K8S)
add_subdirectory(MISC)
add_subdirectory(NAM)
add_subdirectory(SAM)
add_subdirectory(SPL)
add_subdirectory(TRANS)
add_subdirectory(TRC)
add_subdirectory(UTILS)

#
# streams-spl-compiler
#

add_library(streams-spl-compiler
  SHARED
  $<TARGET_OBJECTS:schema_spl_core>
  $<TARGET_OBJECTS:spl_codegen>
  $<TARGET_OBJECTS:spl_core>
  $<TARGET_OBJECTS:spl_frontend>
  $<TARGET_OBJECTS:spl_frontend_grammar>
  $<TARGET_OBJECTS:spl_optimizer>
  $<TARGET_OBJECTS:spl_utility>)

target_link_libraries(streams-spl-compiler
  -Wl,-z,defs
  ${LIBXML2_LIBRARIES}
  ${LibANTLR3C_LIBRARIES}
  streams-spl-runtime)

add_executable(sc
  $<TARGET_OBJECTS:spl_core_sc>)

target_link_libraries(sc
  -rdynamic
  streams-spl-compiler)

add_executable(spl-make-toolkit
  $<TARGET_OBJECTS:spl_core_mktk>)

target_link_libraries(spl-make-toolkit
  -rdynamic
  streams-spl-compiler)

add_executable(spl-schema-from-xml
  $<TARGET_OBJECTS:spl_core_xml>)

target_link_libraries(spl-schema-from-xml
  -rdynamic
  streams-spl-compiler)

add_executable(spl-toolkit-search
  $<TARGET_OBJECTS:spl_core_tks>)

target_link_libraries(spl-toolkit-search
  -rdynamic
  streams-spl-compiler)

add_executable(st-getmsg
  $<TARGET_OBJECTS:spl_core_msg>)

target_link_libraries(st-getmsg
  -rdynamic
  streams-spl-compiler)

#
# streams-stdtk-runtime
#

add_library(streams-stdtk-runtime
  SHARED
  $<TARGET_OBJECTS:spl_runtime_toolkit>)

target_link_libraries(streams-stdtk-runtime
  -Wl,-z,defs
  streams-spl-runtime
  ${XercesC_LIBRARIES})

add_library(streams-stdtk-javaop
  SHARED
  $<TARGET_OBJECTS:spl_runtime_toolkit_javaop>)

target_link_libraries(streams-stdtk-javaop
  -Wl,-z,defs
  streams-spl-runtime
  streams-stdtk-runtime)

add_library(streams-stdtk-xml
  SHARED
  $<TARGET_OBJECTS:spl_runtime_toolkit_xml>)

target_link_libraries(streams-stdtk-xml
  -Wl,-z,defs
  ${LIBXML2_LIBRARIES})

#
# streams-spl-runtime
#

add_library(streams-spl-runtime-nrgdb
  SHARED
  $<TARGET_OBJECTS:spl_runtime_pe_nrgdb>)

target_link_libraries(streams-spl-runtime-nrgdb
  # FIXME(xrg) There is a mutual dependency between this library and
  # streams-spl-runtime on the EventTimeContext symbol. Because of this we
  # currently disable the missing symbol check.
  # -Wl,-z,defs
  ${Boost_LIBRARIES})

add_library(streams-spl-runtime
  SHARED
  $<TARGET_OBJECTS:spl_debugger>
  $<TARGET_OBJECTS:spl_runtime_common>
  $<TARGET_OBJECTS:spl_runtime_function>
  $<TARGET_OBJECTS:spl_runtime_operator>
  $<TARGET_OBJECTS:spl_runtime_operator_eventtime>
  $<TARGET_OBJECTS:spl_runtime_operator_port>
  $<TARGET_OBJECTS:spl_runtime_operator_state>
  $<TARGET_OBJECTS:spl_runtime_operator_state_adapters_fs>
  $<TARGET_OBJECTS:spl_runtime_pe>
  $<TARGET_OBJECTS:spl_runtime_serialization>
  $<TARGET_OBJECTS:spl_runtime_type>
  $<TARGET_OBJECTS:spl_runtime_type_meta>
  $<TARGET_OBJECTS:spl_runtime_utility>
  $<TARGET_OBJECTS:spl_runtime_window>
  $<TARGET_OBJECTS:schema_spl_runtime>
  $<TARGET_OBJECTS:schema_spl_runtime_toolkit>)

target_link_libraries(streams-spl-runtime
  -Wl,-z,defs
  dl
  ${Boost_LIBRARIES}
  ${Decnumber_LIBRARIES}
  ${RocksDB_LIBRARIES}
  ${Xqilla_LIBRARY}
  streams-spl-runtime-nrgdb
  streams-runtime)

#
# PE executables
#

add_executable(streams-k8s-pe
  $<TARGET_OBJECTS:k8s>)

target_link_libraries(streams-k8s-pe
  ${CURL_LIBRARIES}
  ${PrometheusCpp_LIBRARIES}
  ${ZLIB_LIBRARIES}
  -rdynamic
  -Wl,-rpath -Wl,/foooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
  streams-spl-runtime)

#
# streams-spl-runtime-standalone
#

add_library(streams-spl-runtime-standalone
  SHARED
  $<TARGET_OBJECTS:spl_runtime_pe_standalone>)

target_link_libraries(streams-spl-runtime-standalone
  -Wl,-z,defs
  ${OPENSSL_LIBRARIES}
  streams-spl-runtime)

#
# Adapters
#

add_library(streams-spl-redis-store-adapter
  SHARED
  $<TARGET_OBJECTS:spl_runtime_operator_state_adapters_redis>)

target_link_libraries(streams-spl-redis-store-adapter
  -Wl,-z,defs
  ${Hiredis_LIBRARIES}
  streams-spl-runtime)

if(INCLUDE_OBJECTSTORAGE)
  add_library(streams-spl-s3client-adapter
    SHARED
    $<TARGET_OBJECTS:spl_runtime_operator_state_adapters_s3>)

  target_link_libraries(streams-spl-s3client-adapter
    -Wl,-z,defs
    ${AWSSDKCPP_LIBRARIES}
    streams-spl-runtime)

  add_library(streams-spl-objectstorage-adapter
    SHARED
    $<TARGET_OBJECTS:spl_runtime_operator_state_adapters_osa>)

  target_link_libraries(streams-spl-objectstorage-adapter
    -Wl,-z,defs
    streams-spl-s3client-adapter)
endif()

#
# streams-sam_ltop
#

add_library(streams-sam-ltop
  SHARED
  $<TARGET_OBJECTS:sam_ltop>)

target_link_libraries(streams-sam-ltop
  -Wl,-z,defs
  streams-spl-runtime)

#
# streams-apputils
#

add_library(streams-apputils
  SHARED
  $<TARGET_OBJECTS:apputils>
  $<TARGET_OBJECTS:schema_apputils>
  $<TARGET_OBJECTS:trc_apputils>)

target_link_libraries(streams-apputils
  ${CMAKE_THREAD_LIBS_INIT}
  -Wl,-z,defs
  rt
  ${LIBBFD_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${ICU_LIBRARIES}
  ${XercesC_LIBRARIES})

#
# streams-runtime
#

add_library(streams-runtime
  SHARED
  $<TARGET_OBJECTS:nam>
  $<TARGET_OBJECTS:sam>
  $<TARGET_OBJECTS:schema_sam>
  $<TARGET_OBJECTS:schema_sam_v4200>
  $<TARGET_OBJECTS:schema_spl_core_ttm>
  $<TARGET_OBJECTS:schema_utils>
  $<TARGET_OBJECTS:trans>
  $<TARGET_OBJECTS:trc>
  $<TARGET_OBJECTS:utils>)

target_link_libraries(streams-runtime
  -Wl,-z,defs
  ${Boost_LIBRARIES}
  ${CURL_LIBRARIES}
  ${Libcap_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${Tecla_LIBRARIES}
  streams-apputils)

#
# Global formatting target
#
add_custom_target(streams_format
  DEPENDS
  k8s_format
  misc_format
  nam_format
  sam_format
  spl_format
  trans_format
  trc_format
  utils_format)

add_custom_target(streams_lint
  DEPENDS
  k8s_lint
  misc_lint
  nam_lint
  sam_lint
  spl_lint
  trans_lint
  trc_lint
  utils_lint)

#
# Library versioning
#

set(STREAMS_LIBRARIES
  streams-apputils
  streams-runtime
  streams-sam-ltop
  streams-spl-compiler
  streams-spl-runtime
  streams-spl-runtime-standalone
  streams-spl-redis-store-adapter
  streams-spl-runtime-nrgdb
  streams-stdtk-runtime
  streams-stdtk-javaop
  streams-stdtk-xml)

if(INCLUDE_OBJECTSTORAGE)
  list(APPEND STREAMS_LIBRARIES
    streams-spl-objectstorage-adapter
    streams-spl-s3client-adapter)
endif()

foreach(LIBRARY ${STREAMS_LIBRARIES})
  set_target_properties(
    ${LIBRARY}
    PROPERTIES
    VERSION ${LIB_VERSION_STRING}
    SOVERSION ${LIB_SOVERSION})
endforeach()

#
# RPath
#

set_property(
  TARGET
  #
  # bin/
  #
  sc
  spl-make-toolkit
  spl-schema-from-xml
  spl-toolkit-search
  #
  # lib/
  #
  streams-apputils
  streams-runtime
  streams-sam-ltop
  streams-spl-compiler
  streams-spl-runtime
  streams-spl-runtime-standalone
  #
  # the RPATHs
  #
  PROPERTY INSTALL_RPATH
  "$ORIGIN/../lib"
  "$ORIGIN/../system/impl/lib")

set_property(
  TARGET
  #
  # system/impl/bin
  #
  st-getmsg
  streams-k8s-pe
  #
  # system/impl/lib
  #
  streams-spl-redis-store-adapter
  streams-spl-runtime-nrgdb
  streams-stdtk-runtime
  streams-stdtk-javaop
  streams-stdtk-xml
  #
  # the RPATHs
  #
  PROPERTY INSTALL_RPATH
  "$ORIGIN/../lib"
  "$ORIGIN/../../../lib")

if(INCLUDE_OBJECTSTORAGE)
  set_property(
    TARGET
    #
    # system/impl/lib
    #
    streams-spl-objectstorage-adapter
    streams-spl-s3client-adapter
    #
    # the RPATHs
    #
    PROPERTY INSTALL_RPATH
    "$ORIGIN/../lib"
    "$ORIGIN/../../../lib")
endif()

#
# Installation commands
#

install(
  DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}/SPL/toolkit/impl/include/clean/SPL
  DESTINATION
  toolkits/spl/impl/include)

install(
  DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}/SPL/include/clean/SPL
  DESTINATION
  include)

install(
  DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}/SPL/clean/Runtime
  DESTINATION
  system/impl/include/SPL)

install(
  TARGETS
  sc
  spl-make-toolkit
  spl-schema-from-xml
  spl-toolkit-search
  RUNTIME
  DESTINATION bin)

install(
  TARGETS
  st-getmsg
  streams-k8s-pe
  RUNTIME
  DESTINATION system/impl/bin)

install(
  TARGETS
  streams-apputils
  streams-runtime
  streams-sam-ltop
  streams-spl-compiler
  streams-spl-runtime
  streams-spl-runtime-standalone
  LIBRARY
  DESTINATION lib)

install(
  TARGETS
  streams-spl-redis-store-adapter
  streams-spl-runtime-nrgdb
  streams-stdtk-runtime
  streams-stdtk-javaop
  streams-stdtk-xml
  LIBRARY
  DESTINATION system/impl/lib)

if(INCLUDE_OBJECTSTORAGE)
  install(
    TARGETS
    streams-spl-objectstorage-adapter
    streams-spl-s3client-adapter
    LIBRARY
    DESTINATION system/impl/lib)
endif()

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dst-config.h DESTINATION include)
