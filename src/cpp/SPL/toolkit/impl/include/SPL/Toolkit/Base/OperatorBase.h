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

#ifndef SPL_TOOLKIT_BASE_OPERATORBASE_H
#define SPL_TOOLKIT_BASE_OPERATORBASE_H

/**
 * @file OperatorBase.h
 * @brief Definition of the OperatorBase class.
 */

namespace SPL { namespace Toolkit { namespace Base {

/**
 * @brief %Base class for operator implementations.
 *
 * TODO add description of the implementation pattern i.e. how the
 * implementation is wrapped by generated SPL operator code.
 *
 * @tparam OP  the SPL operator wrapping this instance
 */
template<typename OP>
class OperatorBase
{
public:
  typedef OP OperatorType;   ///< Wrapper %SPL operator type

  /// Notify port readiness.
  virtual void allPortsReady() {}

  /// Notify pending shutdown.
  virtual void prepareToShutdown() {}

  /// Tuple processing for mutating ports.
  virtual void process(Tuple & tuple, uint32_t port) {}

  /// Tuple processing for non-mutating ports.
  virtual void process(Tuple const & tuple, uint32_t port) {}

  /// Processing in a separate thread.
  virtual void process(uint32_t idx) {}

protected:
  /**
   * @brief Constructs an @c OperatorBase instance.
   *
   * @param splOp %SPL operator wrapping this instance
   * @param splOpKind %SPL operator kind
   */
  OperatorBase(OperatorType& splOp, const std::string & splOpKind) :
    op_(splOp), kind_(splOpKind) {}

  virtual ~OperatorBase() {}

  /**
   * @brief Returns the kind of the SPL operator associated with this
   * operator instance.
   *
   * @return the trace categories
   */
  inline std::string const & kind() const
  { return kind_; }

  /**
   * @brief Returns the operator wrapping this instance.
   *
   * @return the operator wrapping this instance
   */
  inline OperatorType& op()
  { return op_; }

  /**
   * @brief Returns the operator wrapping this instance.
   *
   * @return the operator wrapping this instance
   */
  inline OperatorType const & op() const
  { return op_; }

private:
  OperatorType& op_;       // Reference to the wrapping operator instance
  const std::string kind_; // SPL Operator kind
};
}}}    // end SPL::Toolkit::Base
#endif // SPL_TOOLKIT_BASE_OPERATORBASE_H
