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

#include <SAM/SAMTypes.h>

SAM_NAMESPACE_USE
SPC_NAMESPACE_USE
UTILS_NAMESPACE_USE
using namespace std;

IMPL_EXCEPTION(SAM, SAM, UTILS_NAMESPACE::Distillery);
IMPL_EXCEPTION(SAM, CannotContactServer, SAM);
IMPL_EXCEPTION(SAM, GDG, SAM);
IMPL_EXCEPTION(SAM, InstanceShuttingDown, SAM);
IMPL_EXCEPTION(SAM, InvalidAttribute, SAM);
IMPL_EXCEPTION(SAM, InvalidProtocol, SAM);
IMPL_EXCEPTION(SAM, MalformedJob, SAM);
IMPL_EXCEPTION(SAM, MalformedXMLObject, SAM);
IMPL_EXCEPTION(SAM, SAMRoutingNotCurrent, SAM);
IMPL_EXCEPTION(SAM, SAMRoutingNotValid, SAM);
IMPL_EXCEPTION(SAM, UndefinedAAS, SAM);
IMPL_EXCEPTION(SAM, UndefinedSAM, SAM);
IMPL_EXCEPTION(SAM, UnknownEvent, SAM);
IMPL_EXCEPTION(SAM, ConnectionNotFound, GDG);
IMPL_EXCEPTION(SAM, GDGInternalError, GDG);
IMPL_EXCEPTION(SAM, IncompleteJobInformation, GDG);
IMPL_EXCEPTION(SAM, InvalidAppSetID, GDG);
IMPL_EXCEPTION(SAM, InvalidInputPortID, GDG);
IMPL_EXCEPTION(SAM, InvalidJobID, GDG);
IMPL_EXCEPTION(SAM, InvalidOutputPortID, GDG);
IMPL_EXCEPTION(SAM, InvalidPEType, GDG);
IMPL_EXCEPTION(SAM, InvalidQuery, GDG);
IMPL_EXCEPTION(SAM, InvalidStateTransition, GDG);
IMPL_EXCEPTION(SAM, InvalidScope, GDG);
IMPL_EXCEPTION(SAM, InvalidStreamID, GDG);
IMPL_EXCEPTION(SAM, JobCannotUnregister, GDG);
IMPL_EXCEPTION(SAM, PENotRunningYet, GDG);
IMPL_EXCEPTION(SAM, PENotTerminated, GDG);
IMPL_EXCEPTION(SAM, PECannotRestart, GDG);
IMPL_EXCEPTION(SAM, PEAlreadyTerminating, GDG);
IMPL_EXCEPTION(SAM, PECannotUnregister, GDG);
IMPL_EXCEPTION(SAM, PropertyAlreadyExists, GDG);
IMPL_EXCEPTION(SAM, StreamNotActive, GDG);
IMPL_EXCEPTION(SAM, InvalidPropertyType, GDG);

IMPL_EXCEPTION(SAM, SAMStreamNotFound, SAM);
IMPL_EXCEPTION(SAM, SAMDuplicateStream, SAM);
IMPL_EXCEPTION(SAM, SAMPropertyNotFound, SAM);
IMPL_EXCEPTION(SAM, SAMDuplicateProperty, SAM);
IMPL_EXCEPTION(SAM, SAMInvalidSubscriptionExpression, SAM);
IMPL_EXCEPTION(SAM, SAMCannotUpdateNameBasedSubscription, SAM);

IMPL_EXCEPTION(SAM, SAMJobNoPesStoppable, SAM);

IMPL_EXCEPTION(SAM, SAMInvalidStreamId, SAM);
IMPL_EXCEPTION(SAM, SAMInvalidStreamViewId, SAM);
IMPL_EXCEPTION(SAM, InvalidJobName, SAM);
IMPL_EXCEPTION(SAM, LogicalToPhysical, SAM);

IMPL_EXCEPTION(SAM_NAMESPACE, PB, SAM);
IMPL_EXCEPTION(SAM_NAMESPACE, Deserialization, PB);
IMPL_EXCEPTION(SAM_NAMESPACE, IncompleteMessage, PB);
