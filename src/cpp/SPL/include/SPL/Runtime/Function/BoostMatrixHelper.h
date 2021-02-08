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

/*!
 * \file BoostMatrixHelper.h \brief Definitions of C++ helper functions for SPADE matrices (implemented as boost ublas matrices).
 */

#ifndef SPL_RUNTIME_FUNCTION_BOOST_MATRIX_HELPER_H
#define SPL_RUNTIME_FUNCTION_BOOST_MATRIX_HELPER_H

#include <SPL/Runtime/Type/SPLType.h>
#include <boost/numeric/ublas/matrix.hpp>

/// \namespace SPL::Functions::Math::Compat \brief C++ helper functions for
/// converting between SPADE (%SPL v1) matrices (based on boost:ublas::matrix)
/// and %SPL nested lists.

namespace SPL { namespace Functions { namespace Math { namespace Compat {

/// Convert from a nested %SPL list to a ublas matrix. The nested list must
/// represent a valid matrix: All sub-lists must have the same length.
/// @param to ublas matrix to convert to
/// @param from nested list to convert from
/// @throws SPLRuntimeTypeMismatchException if the list does not represent a
/// valid matrix
template <class T, class L, class A>
void convertFromNestedListToUblasMatrix(boost::numeric::ublas::matrix<T,L,A> & to, SPL::list<SPL::list<T> > const & from)
{
    size_t nrows = from.size();
    size_t ncols = (nrows==0) ? 0 : from[0].size();
    if (nrows != to.size1() || ncols != to.size2()) {
        to.resize(nrows, ncols, false);
    }
    for(size_t r=0; r<nrows; ++r) {
        SPL::list<T> const & row = from[r];
        if (row.size() != ncols) {
            SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR,
                                SPL_APPLICATION_RUNTIME_INVALID_NESTED_LIST_AS_MATRIX,
                                SPL_FUNC_DBG);
        }
        for (size_t c = 0; c < ncols; ++c) {
            to(r,c) = row[c];
        }
    }
}

/// Convert from a nested %SPL list to a ublas matrix
/// @param to nested list to convert to
/// @param from ublas matrix to convert from
template <class T, class L, class A>
void convertToNestedListFromUblasMatrix(SPL::list<SPL::list<T> > & to, boost::numeric::ublas::matrix<T,L,A> const & from)
{
    size_t nrows = from.size1();
    size_t ncols = from.size2();
    to.reserve(nrows);
    if (to.size() < nrows) {
        to.insert(to.end(), (nrows-to.size()), SPL::list<T>());
    } else if (to.size() > nrows) {
        to.erase(to.begin()+nrows, to.end());
    }
    for(size_t r=0; r<nrows; ++r) {
        SPL::list<T> & row = to[r];
        row.reserve(ncols);
        if (row.size() < ncols) {
            row.insert(row.end(), (ncols-row.size()), T());
        } else if (row.size() > ncols) {
            row.erase(row.begin()+ncols, row.end());
        }
        for (size_t c = 0; c < ncols; ++c) {
            row[c] = from(r,c);
        }
    }
}

}}}}
#endif /* SPL_RUNTIME_FUNCTION_BOOST_MATRIX_HELPER_H */
