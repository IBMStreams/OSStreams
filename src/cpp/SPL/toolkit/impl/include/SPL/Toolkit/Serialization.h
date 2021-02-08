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

#ifndef SPL_TOOLKIT_SERIALIZATION_H_
#define SPL_TOOLKIT_SERIALIZATION_H_

/**
 * Serialization.h: public interface to aggregate functor serialization.
 */

/**
 * Intrusive serialization: each class declares and implements:
 *
 * template<typename Archive>
 * void save(Archive & ar) const { ar << member1 << member2 ...; }
 *
 * template<typename Archive>
 * load(Archive & ar) { ar >> member1 >> member2 ...; }
 */

/*
 * Implementation routes member access through the Access class.  This allows
 * us to grant access to private class member functions by specifying
 * friend class SPL::Serialization::Access
 */
#include <SPL/Toolkit/Access.h>

namespace SPL {
namespace Serialization {

/**
 * Default implementation is to call the class member function "save"
 * via the Access friend class.
 */
template<typename S, typename T>
inline void save(S & str, T const & t) { Access::save(str, t); }

/**
 * Default implementation is to call the member function "load".
 * via the Access friend class.
 */
template<typename S, typename T>
inline void load(S & str, T & t) { Access::load(str, t); }

} // namespace Serialization
} // namespace SPL

#endif // SPL_TOOLKIT_SERIALIZATION_H_
