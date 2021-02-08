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

#ifndef SPL_RUNTIME_OPERATOR_STREAM_PROPERTY_COLLECTION_H
#define SPL_RUNTIME_OPERATOR_STREAM_PROPERTY_COLLECTION_H

/*!
 * \file StreamPropertyCollection.h \brief Definition of the SPL::StreamPropertyCollection class.
 */

#include <SPL/Runtime/Operator/Port/StreamProperty.h>

namespace SPL
{
    /// \brief Class that represents a stream property collection.
    ///
    /// It extends from std::vector<StreamProperty>.
    class StreamPropertyCollection : public std::vector<StreamProperty>
    {
    public:
        /// Empty constructor
        ///
        explicit StreamPropertyCollection()
            : std::vector<StreamProperty>() {}

        /// Construct using a value
        /// @param n number of elements
        /// @param v value to use
        explicit StreamPropertyCollection(
            std::vector<StreamProperty>::size_type n,
            StreamProperty const & v = StreamProperty())
            : std::vector<StreamProperty>(n,v) {}

        /// Construct from an stl vector
        /// @param x stl vector to use
        StreamPropertyCollection(const std::vector<StreamProperty> &x)
            : std::vector<StreamProperty>(x) {}

        /// Copy constructor
        /// @param x list to construct from
        StreamPropertyCollection(const StreamPropertyCollection &x)
            : std::vector<StreamProperty>(x) {}

        /// Construct from a sequence
        /// @param first begin iterator, inclusive
        /// @param last end iterator, exclusive
        template <class InputIterator>
        StreamPropertyCollection(InputIterator first, InputIterator last)
            : std::vector<StreamProperty>(first,last) {}
    };

    inline std::ostream & operator<<(std::ostream & ostr, StreamPropertyCollection const & spc)
    {
        ostr << "[";
        for(size_t i=0, iu=spc.size(); i<iu; ++i) {
            if (i) {
                ostr << ",";
            }
            ostr << spc[i];
        }
        ostr << "]";
        return ostr;
    }
}

#endif /* SPL_RUNTIME_OPERATOR_STREAM_PROPERTY_COLLECTION_H */
