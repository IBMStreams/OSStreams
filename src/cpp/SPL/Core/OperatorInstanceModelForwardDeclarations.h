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

class Annotation;
class AnnotationAttribute;
class AnnotationTuple;
class Assignment;
class Context;
class EventTimeContext;
class EventTimePolicy;
class Expression;
class InputAttribute;
class InputPort;
class OutputAttribute;
class OutputPort;
class Parameter;
class Port;
class PartitionEvictionPolicy;
class RuntimeConstant;
class StateVariable;
class Toolkit;
class Window;
class WindowPolicy;

typedef WindowPolicy EvictionPolicy;
typedef WindowPolicy TriggerPolicy;

typedef boost::shared_ptr<Annotation> AnnotationPtr;
typedef boost::shared_ptr<AnnotationAttribute> AnnotationAttributePtr;
typedef boost::shared_ptr<AnnotationTuple> AnnotationTuplePtr;
typedef boost::shared_ptr<Assignment> AssignmentPtr;
typedef boost::shared_ptr<Context> ContextPtr;
typedef boost::shared_ptr<EventTimeContext> EventTimeContextPtr;
typedef boost::shared_ptr<EventTimePolicy> EventTimePolicyPtr;
typedef boost::shared_ptr<EvictionPolicy> EvictionPolicyPtr;
typedef boost::shared_ptr<Expression> ExpressionPtr;
typedef boost::shared_ptr<InputAttribute> InputAttributePtr;
typedef boost::shared_ptr<InputPort> InputPortPtr;
typedef boost::shared_ptr<OutputAttribute> OutputAttributePtr;
typedef boost::shared_ptr<OutputPort> OutputPortPtr;
typedef boost::shared_ptr<Parameter> ParameterPtr;
typedef boost::shared_ptr<PartitionEvictionPolicy> PartitionEvictionPolicyPtr;
typedef boost::shared_ptr<Port> PortPtr;
typedef boost::shared_ptr<RuntimeConstant> RuntimeConstantPtr;
typedef boost::shared_ptr<StateVariable> StateVariablePtr;
typedef boost::shared_ptr<Toolkit> ToolkitPtr;
typedef boost::shared_ptr<TriggerPolicy> TriggerPolicyPtr;
typedef boost::shared_ptr<Window> WindowPtr;
typedef boost::shared_ptr<WindowPolicy> WindowPolicyPtr;
