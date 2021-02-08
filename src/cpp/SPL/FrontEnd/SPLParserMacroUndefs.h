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

#ifndef _SPL_PARSER_MACRO_UNDEFS_
#define _SPL_PARSER_MACRO_UNDEFS_

/// The version of Antlr that creates C parsers has the unfortunate design
/// flaw of generating preprocessor definitions for all tokens, and making
/// these definitions visible in the header files of generated parsers. This
/// causes a problem when a file includes both a parser header and some
/// other header that uses the same names as identifiers. To avoid this
/// problem, the front-end keeps inclusion of parser headers to the minimum
/// necessary, and includes this file to undefine the most bothersome
/// definitions before including other files.

#undef HEX
#undef DIGIT
#undef LETTER
#undef SUBSCRIPT
#undef SL_COMMENT
#undef ML_COMMENT
#undef WHITE_SPACE
#undef NONE

#undef BOUNDSFORMAL
#undef TYPEFORMAL
#undef BOOLEAN
#undef ENUM
#undef VOID
#undef INT8
#undef INT16
#undef INT32
#undef INT64
#undef UINT8
#undef UINT16
#undef UINT32
#undef UINT64
#undef FLOAT32
#undef FLOAT64
#undef FLOAT128
#undef DECIMAL32
#undef DECIMAL64
#undef DECIMAL128
#undef COMPLEX32
#undef COMPLEX64
#undef COMPLEX128
#undef TIMESTAMP
#undef RSTRING
#undef BSTRING
#undef USTRING
#undef BLOB
#undef LIST
#undef BLIST
#undef SET
#undef BSET
#undef MAP
#undef BMAP
#undef TUPLE

#endif /*_SPL_PARSER_MACRO_UNDEFS_*/
