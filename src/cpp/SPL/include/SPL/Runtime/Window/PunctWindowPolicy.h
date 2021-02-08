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

#ifndef SPL_RUNTIME_WINDOW_PUNCT_WINDOW_POLICY_H
#define SPL_RUNTIME_WINDOW_PUNCT_WINDOW_POLICY_H

/*!
 * \file PunctWindowPolicy.h \brief Definition of the SPL::PunctWindowPolicy class.
 */

namespace SPL
{
    /// @ingroup Window
    /// @brief Class that represents a punctuation based window policy.
    ///
    /// - For a tumbling window, as an eviction policy
    ///
    /// A window is considered full when a punctuation is received.
    ///
    /// Operational semantics: When a tuple is received, it is inserted into the
    /// window. When a punctuation is received, the whole window flushes (mass
    /// eviction).
    ///
    /// Note: Sliding windows cannot have punctuation based eviction or trigger
    /// policies.
    class PunctWindowPolicy : public WindowPolicy
    {
    public:
        /// Constructor
        ///
        PunctWindowPolicy()
            : WindowPolicy(Punct) {}

        /// Destructor
        ///
        virtual ~PunctWindowPolicy() {}

        /// Clone
        /// @return a copy of this object
        WindowPolicy * clone () const
            { return new PunctWindowPolicy(*this); }
    };
};

#endif /* SPL_RUNTIME_WINDOW_PUNCT_WINDOW_POLICY_H */
