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

#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/PersistenceException.h>

/// Copy char array to string and free char array.
//#define ASSIGN_PTR_TO_STR_AND_FREE(ptr,str_val)   \
//  do {                                \
//    if(ptr) {                         \
//      str_val.assign(ptr);            \
//      CORBA::string_free(ptr);        \
//    }                                 \
//  }while(0);

/// check for exception xml, throw, and free the exceptionPtr
//#define PROCESSING_EXCEPTION(exceptionPtr)                \
//  do {                                                    \
//    std::string e_out(DistilleryException::NO_EXCEPTION); \
//    ASSIGN_PTR_TO_STR_AND_FREE(exceptionPtr,e_out);             \
//    DistilleryExceptionInstantiator::instantiateAndThrow(e_out); \
//  }while(0);

/// Process unknown exception and catch DistilleryException.
#define RUNTIME_TO_EX_NEW(strval, varex)                                                           \
    EX_LOCATION(exss);                                                                             \
    DistilleryException de(exss.str(), varex);                                                     \
    strval = de.toXMLString();

/// Catch a specific distillery exception.
#define RUNTIME_CATCH_EX(strvar, eType, lev, aspect)                                               \
    catch (const eType& e)                                                                         \
    {                                                                                              \
        SPCDBG(lev,                                                                                \
               "operation " << __FUNCTION__ << " failed -- exception: " << e.getExplanation(),     \
               aspect);                                                                            \
        strvar = e.toXMLString();                                                                  \
    }

/// Catch a specific non-distillery exception, which not support e.toXMLString().
#define RUNTIME_CATCH_NON_DIST_EX(strvar, eType, lev, aspect)                                      \
    catch (const eType& e)                                                                         \
    {                                                                                              \
        stringstream __my_ex_strm;                                                                 \
        __my_ex_strm << e;                                                                         \
        RUNTIME_TO_EX_NEW(strvar, __my_ex_strm.str().c_str());                                     \
        SPCDBG(lev, "operation " << __FUNCTION__ << " failed -- exception: " << strvar, aspect);   \
    }

/// Catch DistilleryException and unknown exceptions.
#define RUNTIME_CATCH_COMMON(strvar, aspect)                                                       \
    RUNTIME_CATCH_EX(strvar, DistilleryException, L_ERROR, aspect)                                 \
    catch (const PersistenceException& pe)                                                         \
    {                                                                                              \
        SPCDBG(L_ERROR, "operation " << __FUNCTION__ << " failed -- exception: " << pe.toString(), \
               aspect);                                                                            \
        RUNTIME_TO_EX_NEW(strvar, "operation " + string(__FUNCTION__) +                            \
                                    " failed -- exception: " + pe.toString());                     \
    }                                                                                              \
    catch (...)                                                                                    \
    {                                                                                              \
        SPCDBG(L_ERROR, "operation " << __FUNCTION__ << " failed -- unknown exception: ", aspect); \
        RUNTIME_TO_EX_NEW(strvar,                                                                  \
                          "operation " + string(__FUNCTION__) + " failed -- unknown exception");   \
    }
