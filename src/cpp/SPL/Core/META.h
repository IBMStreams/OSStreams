/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SPL_RUNTIME_TYPE_META_H
#define SPL_RUNTIME_TYPE_META_H

#define META_int8 INT8
#define META_int16 INT16
#define META_int32 INT32
#define META_int64 INT64
#define META_uint8 UINT8
#define META_uint16 UINT16
#define META_uint32 UINT32
#define META_uint64 UINT64
#define META_float32 FLOAT32
#define META_float64 FLOAT64
#define META_decimal32 DECIMAL32
#define META_decimal64 DECIMAL64
#define META_decimal128 DECIMAL128
#define META_complex32 COMPLEX32
#define META_complex64 COMPLEX64
#define META_rstring RSTRING
#define META_ustring USTRING
#define META_boolean BOOLEAN
#define META_timestamp TIMESTAMP
#define META_xml XML

#define ARITH_TYPES                                                                                \
    BOOST_PP_TUPLE_TO_LIST(15, (int8, int16, int32, int64, uint8, uint16, uint32, uint64, float32, \
                                float64, decimal32, decimal64, decimal128, complex32, complex64))  \
    /**/

#define ORDERED_TYPES                                                                              \
    BOOST_PP_TUPLE_TO_LIST(17, (int8, int16, int32, int64, uint8, uint16, uint32, uint64, float32, \
                                float64, decimal32, decimal64, decimal128, timestamp, boolean,     \
                                rstring, ustring))                                                 \
    /**/

#define FLOAT_TYPES BOOST_PP_TUPLE_TO_LIST(2, (float32, float64))

#define ALL_FLOAT_TYPES                                                                            \
    BOOST_PP_TUPLE_TO_LIST(                                                                        \
      7, (float32, float64, decimal32, decimal64, decimal128, complex32, complex64))               \
    /**/

#define INTEGER_TYPES                                                                              \
    BOOST_PP_TUPLE_TO_LIST(8, (int8, int16, int32, int64, uint8, uint16, uint32, uint64))

#define STRING_TYPES BOOST_PP_TUPLE_TO_LIST(2, (rstring, ustring))

#define TIMESTAMP_TYPES BOOST_PP_TUPLE_TO_LIST(1, (timestamp))

#define BOOL_TYPES BOOST_PP_TUPLE_TO_LIST(1, (boolean))

#define MISC_TYPES BOOST_PP_TUPLE_TO_LIST(3, (timestamp, boolean, xml))

#endif /* SPL_RUNTIME_TYPE_META_H */
