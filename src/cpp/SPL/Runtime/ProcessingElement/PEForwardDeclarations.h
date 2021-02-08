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

namespace xmlns {
namespace prod {
namespace streams {
namespace application {
class applicationSetType;
class peType;
class operInstanceType;
class operInstanceOutgoingConnectionType;
class customMetricType;
class operDefinitionType;
class envVarType;
class envVarsType;
class splitterType;
class parallelChannelType;
class partitioningAttributesType;
class tupleTypeType;
namespace augmented {
class augmentedPEType;
class augmentedPEsType;
}
namespace physical {
class physParallelRegionType;
class physParallelRegionsType;
class physOperInstanceType;
class physOperInstanceInputPortType;
class toolkitsMapType;
class toolkitMapType;
}
}
}
}
}

namespace SPL {
typedef xmlns::prod::streams::application::envVarsType EnvVars;
typedef xmlns::prod::streams::application::envVarType EnvVar;
typedef xmlns::prod::streams::application::splitterType SplitterType;
typedef xmlns::prod::streams::application::parallelChannelType ParallelChannelType;
typedef xmlns::prod::streams::application::partitioningAttributesType PartitioningAttributesType;
typedef xmlns::prod::streams::application::tupleTypeType TupleTypeType;
typedef xmlns::prod::streams::application::augmented::augmentedPEType PEModel;
typedef xmlns::prod::streams::application::physical::toolkitsMapType ToolkitsMapType;
typedef xmlns::prod::streams::application::physical::toolkitMapType ToolkitMapType;
class Punctuation;
class Tuple;
class PEImpl;
class PE;
}
