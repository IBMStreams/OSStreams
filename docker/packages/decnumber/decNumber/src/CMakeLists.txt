#
# Copyright (c) IBM Corporation, 2021.  All rights reserved.
#
# This software is made available under the terms of the IBM
# alphaWorks License Agreement (distributed with this software as
# alphaWorks-License.txt).  Your use of this software indicates
# your acceptance of the terms and conditions of that Agreement.
#
# The description and User's Guide ("The decNumber C Library") for
# this software is included in the package as decNumber.pdf.  This
# document is also available in HTML, together with specifications,
# testcases, and Web links, on the General Decimal Arithmetic page.
#

set(CMAKE_POSITION_INDEPENDENT_CODE 1)

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.c)
list(REMOVE_ITEM SOURCES decBasic.c decCommon.c)

add_library(decNumber++ SHARED ${SOURCES})
install(TARGETS decNumber++ LIBRARY DESTINATION lib)

set_target_properties(
  decNumber++
  PROPERTIES
  VERSION ${LIB_VERSION_STRING}
  SOVERSION ${LIB_SOVERSION})
