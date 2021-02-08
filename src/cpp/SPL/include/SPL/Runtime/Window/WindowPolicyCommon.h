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

#ifndef SPL_RUNTIME_WINDOW_WINDOW_POLICY_COMMON_H
#define SPL_RUNTIME_WINDOW_WINDOW_POLICY_COMMON_H

#include <SPL/Runtime/Utility/Visibility.h>

/**
 * @file WindowPolicyCommon.h @brief Definition of the SPL::WindowPolicy class.
 */
namespace SPL {

/**
 * @ingroup Window
 * @brief Class that represents the eviction policies or the trigger
 * policies of a window.
 */
class WindowPolicy
{
public:
  /// @brief Type of the window policy.
  enum Type
  {
    Count,    //!< Count based window policy
    Delta,    //!< Delta based window policy
    Punct,    //!< %Punctuation based window policy
    Time,     //!< Time based window policy
    EventTime //!< Event-time based window policy
  };

  /// @brief Constructor
  WindowPolicy(Type type) : type_(type) {}

  /// @brief Destructor
  virtual ~WindowPolicy() {}

  /**
   * @brief Returns the type of the window policy.
   * @return type of the window policy
   */
  Type getType() const
  { return type_; }

  /**
   * @brief Clone this instance.
   * @return a copy of this object
   */
  virtual WindowPolicy * clone() const = 0;

  /**
   * @brief Returns a string representation for the given WindowPolicy type.
   * @return WindowPolicy type value as a string
   */
	const char* typeToString() const DLL_PUBLIC;

private:
  Type type_;
};
}

#endif /* SPL_RUNTIME_WINDOW_WINDOW_POLICY_COMMON_H */
