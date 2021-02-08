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

#ifndef SPL_RUNTIME_RUNTIME_ANNOTATIONS_H
#define SPL_RUNTIME_RUNTIME_ANNOTATIONS_H

#include <string>

// Operator instance annotations and keys
#define AUTONOMOUS_ANNOTATION "autonomous"
#define CC_REGION_ENTRY_ANNOTATION "consistentRegionEntry"
#define CC_IS_TRIGGER_OPERATOR "isTriggerOperator"
#define CC_IS_START_REGION "isStartOfRegion"
#define CC_IS_END_REGION "isEndOfRegion"
#define CC_AUTONOMOUS_INPUT_PORT "autonomousInputPort"
#define CC_REGION_INDEX "index"
#define CC_LOGICAL_INDEX "logicalIndex"
#define CC_DRAIN_TIMEOUT "drainTimeout"
#define CC_RESET_TIMEOUT "resetTimeout"

#define CATCH_ANNOTATION "catch"
#define CATCH_EXCEPTION "exception"
#define CATCH_TUPLE_TRACE "tupleTrace"
#define CATCH_STACK_TRACE "stackTrace"

const std::string THREADING_ANNOTATION = "threading";
const std::string THREADING_MODEL = "model";
const std::string THREADING_THREADS = "threads";
const std::string THREADING_ELASTIC = "elastic";

const std::string ET_CONTEXT_ANNOTATION = "eventTimeContext";
const std::string ET_CONTEXT_ATTRIBUTE = "attribute";
const std::string ET_CONTEXT_RESOLUTION = "resolution";
const std::string ET_CONTEXT_TYPE = "type";
const std::string ET_CONTEXT_LAG = "lag";
const std::string ET_CONTEXT_GAP = "minimumGap";
const std::string ET_CONTEXT_INPUT_CONNECTIONS = "inputConnections";
const std::string ET_CONTEXT_OUTPUT_PORTS = "outputPorts";

#endif
