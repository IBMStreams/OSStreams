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

#ifndef SPL_RUNTIME_OPERATOR_PORT_DELEGATE_H
#define SPL_RUNTIME_OPERATOR_PORT_DELEGATE_H

#include <SPL/Runtime/Operator/Operator.h>
#include <inttypes.h>

namespace SPL {
/// Class that represents a SPL port delegate
template<class DataType>
class PortDelegate
{
  public:
    /// Constructor
    PortDelegate() {}

    /// Create a port delegate
    /// @param objPtr object that holds the function to be wrapped
    /// @param callParam second paramter to the function to be wrapped
    /// @return resulting port delegate
    /// Example usage:
    ///    PortDelegate<MyTuple&>::createCall<MyOperClass,
    ///      &MyOperClass::myProcessFunction>(myOperObj, portIndex);
    template<class ObjectType, void (ObjectType::*MemberFunction)(DataType, uint32_t)>
    static PortDelegate createCall(ObjectType* objPtr, uint32_t callParam)
    {
        PortDelegate inputCaller;
        inputCaller.objectPointer = objPtr;
        inputCaller.boundParam = callParam;
        inputCaller.functionCaller = &FunctionCaller<ObjectType, MemberFunction>;
        return inputCaller;
    }

    /// Make a call through the port delegate
    /// @param data dt item to be passed to the wrapped function
    inline void operator()(DataType data) const
    {
        return (*functionCaller)(objectPointer, data, boundParam);
    }

  private:
    uint32_t boundParam;
    void* objectPointer;
    void (*functionCaller)(void*, DataType, uint32_t);

    template<class ObjectType, void (ObjectType::*MemberFunction)(DataType, uint32_t)>
    static void FunctionCaller(void* pointer, DataType data, uint32_t param)
    {
        ObjectType* optr = static_cast<ObjectType*>(pointer);
        return (optr->*MemberFunction)(data, param);
    }
};
};

#endif /* SPL_RUNTIME_OPERATOR_PORT_DELEGATE_H */
