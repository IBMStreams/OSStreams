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

#ifndef SPL_RUNTIME_OPERATOR_INPUT_PORT_DELEGATE_H
#define SPL_RUNTIME_OPERATOR_INPUT_PORT_DELEGATE_H

#include <SPL/Runtime/Operator/Operator.h>
#include <inttypes.h>

namespace SPL {
template<class DataType>
class OpInputPortDelegate
{
  public:
    OpInputPortDelegate() {}
    OpInputPortDelegate(Operator* oper, int32_t port)
      : oper_(oper)
      , port_(port)
    {}

    inline void operator()(DataType data) const { return oper_->processRaw(data, port_); }

  private:
    Operator* oper_;
    int32_t port_;
};
};

#endif /* SPL_RUNTIME_OPERATOR_INPUT_PORT_DELEGATE_H */
