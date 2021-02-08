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

#include <SPL/Runtime/ProcessingElement/PEForwardDeclarations.h>

namespace Streams {
class SAMClient;
};

namespace Distillery {
class RPCRetryController;
};

namespace CORBA {
class ORB;
};

namespace SPL {
typedef xmlns::prod::streams::application::operInstanceType OPModel;
typedef xmlns::prod::streams::application::physical::physOperInstanceType PhysOPModel;
typedef xmlns::prod::streams::application::physical::physParallelRegionsType PhysParallelRegions;
typedef xmlns::prod::streams::application::physical::physParallelRegionType PhysParallelRegion;
typedef xmlns::prod::streams::application::augmented::augmentedPEType PEModel;
typedef xmlns::prod::streams::application::physical::physOperInstanceInputPortType
  PhysOPInputPortModel;

class ActiveQueue;
class BaseWindow;
class ConsistentRegionContextImpl;
class ConsistentRegionEventHandler;
class DebugHook;
class NetworkByteBuffer;
class Operator;
class OperatorContext;
class OperatorImpl;
class OperatorInputPortImpl;
class OperatorMetrics;
class OperatorMetricsImpl;
class OperatorOutputPortImpl;
class OperatorThread;
class PE;
class PECallbackRegistery;
class PECheckpointService;
class PEConsistentRegionService;
class PETransportIPortCollection;
class PETransportOPortCollection;
class PEDebugService;
class PEImpl;
class PEMetrics;
class PEMetricsImpl;
class PortSignal;
class ProcessSignal;
class ProcessingElement;
class Punctuation;
class StreamProperty;
class StreamPropertyCollection;
class SubmitSignal;
class SubscriptionExpression;
class Tuple;

class AutoCheckpointServices;
class AutoMetricsIntervalTimer;
class AutoCreateOperators;
class AutoDebugServices;
class AutoOperatorMetrics;
class AutoPeMetrics;
class AutoPePorts;
};
