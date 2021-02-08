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

macro(SplMakeNative BUILTIN_CPP NATIVE_HEADERS NATIVE_TOOLKITS)
  #
  # Path aliasing
  #
  set(SPL_MAKENATIVE_EXECUTABLE ${DST_REPO_ROOT}/tools/make-native)
  set(SPL_IMPLEM_CORE_GEN_DIR ${CMAKE_BINARY_DIR}/cpp/SPL/Core)
  set(SPL_HEADER_NATIVE_FUNC_SRC_DIR ${CMAKE_SOURCE_DIR}/cpp/SPL/Runtime/Function)
  set(SPL_HEADER_NATIVE_FUNC_GEN_DIR ${CMAKE_BINARY_DIR}/cpp/SPL/include/SPL/Runtime/Function)
  set(SPL_TOOLKIT_NATIVE_DIR ${CMAKE_BINARY_DIR}/cpp/SPL/toolkit/spl)
  #
  # make-native
  #
  add_custom_command(
    OUTPUT
    ${SPL_IMPLEM_CORE_GEN_DIR}/BuiltinExpressionEvaluator.cpp
    #
    # Native function headers
    #
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/SPLFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/CollectionFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/EventTimeFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/FileFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/MathFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/StringFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/TimeFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/UtilFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/XMLFunctions.h
    #
    # Native function toolkits
    #
    ${SPL_TOOLKIT_NATIVE_DIR}/collection/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/eventtime/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/file/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/math/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/string/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/time/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/utility/native.function/function.xml
    #
    COMMAND cmake -E make_directory ${SPL_TOOLKIT_NATIVE_DIR}/collection/native.function
    COMMAND cmake -E make_directory ${SPL_TOOLKIT_NATIVE_DIR}/eventtime/native.function
    COMMAND cmake -E make_directory ${SPL_TOOLKIT_NATIVE_DIR}/file/native.function
    COMMAND cmake -E make_directory ${SPL_TOOLKIT_NATIVE_DIR}/math/native.function
    COMMAND cmake -E make_directory ${SPL_TOOLKIT_NATIVE_DIR}/string/native.function
    COMMAND cmake -E make_directory ${SPL_TOOLKIT_NATIVE_DIR}/time/native.function
    COMMAND cmake -E make_directory ${SPL_TOOLKIT_NATIVE_DIR}/utility/native.function
    COMMAND cmake -E make_directory ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}
    #
    COMMAND
    ${SPL_MAKENATIVE_EXECUTABLE} -f
      -g ${SPL_IMPLEM_CORE_GEN_DIR}/BuiltinExpressionEvaluator.cpp
      -o ${SPL_TOOLKIT_NATIVE_DIR}
      ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinSPLFunctions.h         ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/SPLFunctions.h
	    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinCollectionFunctions.h  ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/CollectionFunctions.h
      ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinEventTimeFunctions.h   ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/EventTimeFunctions.h
	    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinFileFunctions.h        ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/FileFunctions.h
	    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinMathFunctions.h        ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/MathFunctions.h
	    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinStringFunctions.h      ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/StringFunctions.h
	    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinTimeFunctions.h        ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/TimeFunctions.h
	    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinUtilFunctions.h        ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/UtilFunctions.h
	    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinXMLFunctions.h         ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/XMLFunctions.h
    #
    DEPENDS
    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinSPLFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinCollectionFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinEventTimeFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinFileFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinMathFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinStringFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinTimeFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinUtilFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_SRC_DIR}/BuiltinXMLFunctions.h)
  #
  # Update source variable
  #
  list(APPEND ${BUILTIN_CPP}
    ${SPL_IMPLEM_CORE_GEN_DIR}/BuiltinExpressionEvaluator.cpp)
  list(APPEND ${NATIVE_HEADERS}
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/SPLFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/CollectionFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/EventTimeFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/FileFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/MathFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/StringFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/TimeFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/UtilFunctions.h
    ${SPL_HEADER_NATIVE_FUNC_GEN_DIR}/XMLFunctions.h)
  list(APPEND ${NATIVE_TOOLKITS}
    ${SPL_TOOLKIT_NATIVE_DIR}/collection/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/eventtime/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/file/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/math/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/string/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/time/native.function/function.xml
    ${SPL_TOOLKIT_NATIVE_DIR}/utility/native.function/function.xml)
endmacro()

macro(SplMetricInfoPopulator VAR)
  #
  # Path aliasing
  #
  set(SPL_METRIC_INFO_GEN ${DST_REPO_ROOT}/tools/spl-metric-info-populator)
  set(SRM_METRIC_INFO_XML ${CMAKE_SOURCE_DIR}/cpp/SRM/config/SystemMetricsMetadata.xml)
  set(SPL_METRIC_INFO_HDR ${CMAKE_BINARY_DIR}/cpp/SPL/Runtime/Common/SystemMetricInfoPopulator.h)
  #
  # make-native
  #
  add_custom_command(
    OUTPUT ${SPL_METRIC_INFO_HDR}
    COMMAND ${SPL_METRIC_INFO_GEN} -i ${SRM_METRIC_INFO_XML} -o ${SPL_METRIC_INFO_HDR}
    MAIN_DEPENDENCY ${SRM_METRIC_INFO_XML})
  #
  # Update source variable
  #
  list(APPEND ${VAR} ${SPL_METRIC_INFO_HDR})
endmacro()

macro(SplDuplicateTupleTypeModelFiles VAR)
  #
  # Path aliasing
  #
  set(SPL_TUPLE_TYPE_MODEL_H_SRC ${CMAKE_SOURCE_DIR}/cpp/SPL/Core/Model/tupleTypeModel.h)
  set(SPL_TUPLE_TYPE_MODEL_H_DST ${CMAKE_BINARY_DIR}/cpp/SPL/Runtime/Toolkit/Model/tupleTypeModel.h)
  set(SPL_TUPLE_TYPE_MODEL_X_SRC ${CMAKE_SOURCE_DIR}/cpp/SPL/Core/Model/tupleTypeModel.ixx)
  set(SPL_TUPLE_TYPE_MODEL_X_DST ${CMAKE_BINARY_DIR}/cpp/SPL/Runtime/Toolkit/Model/tupleTypeModel.ixx)
  #
  # Copy the files
  #
  add_custom_command(
    OUTPUT
    ${SPL_TUPLE_TYPE_MODEL_H_DST}
    ${SPL_TUPLE_TYPE_MODEL_X_DST}
    COMMAND
    cmake -E copy ${SPL_TUPLE_TYPE_MODEL_H_SRC} ${SPL_TUPLE_TYPE_MODEL_H_DST}
    COMMAND
    cmake -E copy ${SPL_TUPLE_TYPE_MODEL_X_SRC} ${SPL_TUPLE_TYPE_MODEL_X_DST}
    DEPENDS
    ${SPL_TUPLE_TYPE_MODEL_H_SRC}
    ${SPL_TUPLE_TYPE_MODEL_X_SRC})
  #
  # Update source variable
  #
  list(APPEND ${VAR}
    ${SPL_TUPLE_TYPE_MODEL_H_DST}
    ${SPL_TUPLE_TYPE_MODEL_X_DST})
endmacro()

macro(SplMixedModePreprocessor DIRECTORY MODEL FILE VAR)
  #
  # Path Aliasing
  #
  set(SPL_MIXED_MODE_PREPROCESSOR ${CMAKE_SOURCE_DIR}/cpp/SPL/script/impl/bin/spl-mixedmode-processor)
  #
  # Call the preprocessor
  #
  add_custom_command(
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/${DIRECTORY}/${FILE}.pm
    WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
    cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${DIRECTORY}
    COMMAND
    cmake -E env PERL5LIB=${CMAKE_BINARY_DIR}/cpp/SPL/script/impl/bin
    ${SPL_MIXED_MODE_PREPROCESSOR} -t ${MODEL} ${DIRECTORY}/${FILE}
    COMMAND
    cmake -E copy ${DIRECTORY}/${FILE}.pm ${CMAKE_CURRENT_BINARY_DIR}/${DIRECTORY}/${FILE}.pm
    COMMAND
    cmake -E remove ${DIRECTORY}/${FILE}.pm
    DEPENDS
    ${DIRECTORY}/${FILE}.cgt
    ${CMAKE_SOURCE_DIR}/cpp/SPL/script/SPL/CodeGen.pm
    ${CMAKE_SOURCE_DIR}/cpp/SPL/script/SPL/CodeGen/Type.pm
    ${CMAKE_SOURCE_DIR}/cpp/SPL/script/impl/bin/SPL/CodeGenHelper.pm)
  #
  # Update source variable
  #
  list(APPEND ${VAR} ${CMAKE_CURRENT_BINARY_DIR}/${DIRECTORY}/${FILE}.pm)
endmacro()

macro(SplModelCleaner FILE VAR)
  #
  # Path aliasing
  #
  set(SPL_MODEL_CLEANER ${DST_REPO_ROOT}/tools/spl-model-cleaner)
  #
  # Call the preprocessor
  #
  add_custom_command(
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE}/${FILE}-cleaned.xml
    COMMAND
    cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${FILE}
    COMMAND
    ${SPL_MODEL_CLEANER} ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}/${FILE}.xml
      > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}/${FILE}-cleaned.xml
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}/${FILE}.xml
    )
  #
  # Update source variable
  #
  list(APPEND ${VAR} ${CMAKE_CURRENT_BINARY_DIR}/${FILE}/${FILE}-cleaned.xml)
endmacro()

macro(SplPkgConfigCfgCopy NAME VAR)
  #
  # Call the preprocessor
  #
  add_custom_command(
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pc
    COMMAND
    cmake -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.pc.tmpl ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pc
    )
  #
  # Update source variable
  #
  list(APPEND ${VAR} ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pc)
endmacro()

macro(SplPkgConfigCfgHelper NAME VAR)
  #
  # Path aliasing
  #
  set(SPL_PKGCONFIG_HELPER ${DST_REPO_ROOT}/tools/pkgconfighelper)
  #
  # Call the preprocessor
  #
  add_custom_command(
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pc
    COMMAND
    ${SPL_PKGCONFIG_HELPER} ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pc.tmpl ${USE_AT} ${DISABLE_NEW_DTAGS} ${MACRO_VER} ${MACRO_REL} ${MACRO_MOD} ${MACRO_FIX}
    COMMAND
    cmake -E rename ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pc.tmpl ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pc
    )
  #
  # Update source variable
  #
  list(APPEND ${VAR} ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.pc)
endmacro()

macro(SplCleanupHeader DIRECTORY NAME VAR)
  #
  # Path aliasing
  #
  set(SPL_CLEANUP_HEADER ${DST_REPO_ROOT}/tools/spl-header-cleaner)
  #
  # Call the preprocessor
  #
  add_custom_command(
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/clean/${NAME}
    COMMAND
    cmake -E copy ${DIRECTORY}/${NAME} ${CMAKE_CURRENT_BINARY_DIR}/clean/${NAME}
    COMMAND
    ${SPL_CLEANUP_HEADER} ${CMAKE_CURRENT_BINARY_DIR}/clean/${NAME}
    DEPENDS
    ${DIRECTORY}/${NAME}
    )
  #
  # Update source variable
  #
  list(APPEND ${VAR} ${CMAKE_CURRENT_BINARY_DIR}/clean/${NAME})
endmacro()

macro(BuildProductFile VAR)
  #
  # Path aliasing
  #
  set(BUILD_PRODFILE ${DST_TOOLS}/build-prodfile)
  #
  # Call the preprocessor
  #
  add_custom_command(
    OUTPUT
    ${CMAKE_BINARY_DIR}/.product
    COMMAND
    ${BUILD_PRODFILE} --outputfile ${CMAKE_BINARY_DIR}/.product)
  #
  # Update source variable
  #
  list(APPEND ${VAR} ${CMAKE_BINARY_DIR}/.product)
endmacro()
