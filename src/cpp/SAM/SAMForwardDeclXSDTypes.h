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

#ifndef SAMFORWARDDECLXSDTYPES_H
#define SAMFORWARDDECLXSDTYPES_H

/// foward declarations
namespace xmlns {
namespace prod {
namespace streams {
namespace application {
class applicationSetType;
class applicationType;
class contextType;
class importedStreamsType;
class peType;
class streamPropertiesType;
class streamPropertyType;
class tupleTypeType;
namespace physical {
class physPEType;
class physOperInstanceType;
class physParallelRegionType;
class physApplicationSetType;
}
namespace augmented {
class augmentedApplicationSetType;
class augmentedPEInputPortType;
class augmentedPEOutputPortType;
class augmentedPEType;
class augmentedPEsType;
class connectionIdsType;
class connectionIdType;
class importIndexesType;
class jobHealthType;
class jobType;
class managedPEType;
class nameValuePairsType;
class peHealthType;
class peStateType;
class peStateReasonType;
class streamIdType;
class streamsType;
class streamType;
class streamViewType;
class streamViewsType;
namespace normalized {
class jobSpecRefType;
class peSpecRefType;
class portSpecRefType;
class portIdsType;
class portIdType;
class normalizedJobType;
class normalizedManagedPEType;
class normalizedAugmentedPEInputPortsType;
class normalizedAugmentedPEInputPortType;
class normalizedAugmentedPEOutputPortsType;
class normalizedAugmentedPEOutputPortType;
class inputPortToImportIndexMappingsType;
class inputPortToImportIndexMappingType;
class normalizedConnectionType;
class incomingDynamicConnectionsDetailsType;
class incomingDynamicConnectionDetailsType;
}
}
}
namespace runtime {
namespace sam {
class cancelParamsType;
class inputPortDynamicConnectionsInfoType;
class inputPortInfoType;
class jobInfoType;
class jobNotificationType;
class jobsInfoType;
class managedPEsType;
class outputPortInfoType;
class peInfoType;
class pesInfoType;
class peCollectionOperationStatusType;
class peStateInfoType;
class pesStateInfoType;
class peConfigStateInfoType;
class portConnectionsInfoType;
class samSummaryStateType;
class streamPropertyNameType;
class streamPropertyNamesType;
class submitParamsType;
class systemStateConstraintType;
class systemStateScopingType;
class systemStateType;
class systemTopologyScopingType;
class systemTopologyType;
class eTagType;
class excludeJobsType;
}
}
}
}
}

#endif
