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

class OutputFunctions;
class Context;
class Enumeration;
class CustomOutput;
class ExpressionMode;
class WindowExpressionMode;
class IncrementalCompilationStrategy;
class InputPorts;
class InputPortOpenSet;
class InputPortSet;
class OutputPorts;
class OutputPortOpenSet;
class OutputPortSet;
class Parameter;
class Parameters;
class SingleThreadedMode;
class TupleMode;
class WindowingMode;
class WindowPunctuationInputMode;
class WindowPunctuationOutputMode;
class JavaOpExecutionSettings;

typedef boost::shared_ptr<Context> ContextPtr;
typedef boost::shared_ptr<Enumeration> EnumerationPtr;
typedef boost::shared_ptr<CustomOutput> CustomOutputPtr;
typedef boost::shared_ptr<Common::Library> LibraryPtr;
typedef boost::shared_ptr<SingleThreadedMode> SingleThreadedModePtr;
typedef boost::shared_ptr<IncrementalCompilationStrategy> IncrementalCompilationStrategyPtr;
typedef boost::shared_ptr<Parameters> ParametersPtr;
typedef boost::shared_ptr<Parameter> ParameterPtr;
typedef boost::shared_ptr<ExpressionMode> ExpressionModePtr;
typedef boost::shared_ptr<WindowExpressionMode> WindowExpressionModePtr;
typedef boost::shared_ptr<InputPorts> InputPortsPtr;
typedef InputPortOpenSet InputPort;
typedef boost::shared_ptr<InputPort> InputPortPtr;
typedef boost::shared_ptr<InputPortOpenSet> InputPortOpenSetPtr;
typedef boost::shared_ptr<InputPortSet> InputPortSetPtr;
typedef boost::shared_ptr<TupleMode> TupleModePtr;
typedef boost::shared_ptr<WindowingMode> WindowingModePtr;
typedef boost::shared_ptr<WindowPunctuationInputMode> WindowPunctuationInputModePtr;
typedef boost::shared_ptr<OutputPorts> OutputPortsPtr;
typedef OutputPortOpenSet OutputPort;
typedef boost::shared_ptr<OutputPort> OutputPortPtr;
typedef boost::shared_ptr<OutputPortOpenSet> OutputPortOpenSetPtr;
typedef boost::shared_ptr<OutputPortSet> OutputPortSetPtr;
typedef boost::shared_ptr<OutputFunctions> OutputFunctionsPtr;
typedef boost::shared_ptr<WindowPunctuationOutputMode> WindowPunctuationOutputModePtr;
typedef boost::shared_ptr<JavaOpExecutionSettings> JavaOpExecutionSettingsPtr;
