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

// IBM
//
// Class Description:
//
// This file contains the declaration for all SAM-related internal types

#ifndef SAMTYPES_H
#define SAMTYPES_H

#include <SAM/SAMNamespaceDefs.h>
#include <UTILS/DistilleryException.h>

#include <limits.h>

SAM_NAMESPACE_BEGIN

/// IBM Streams runtime identifiers
typedef uint64_t ID;
typedef uint64_t INDEX;

/// ID limits.
enum
{
    ID_MIN = 0,
    ID_MAX = ULLONG_MAX - 1,
    // use, for example, when an ID must be set, but its value is not known yet
    ID_INVALID = ID_MAX + 1
};

/// The system-assigned ID for a PE
typedef ID PEID;
/// The system-assigned ID for a stream
typedef ID StreamID;
/// The system-assigned ID for a job
typedef ID JobID;
/// The system-assigned ID for a port
typedef ID PortID;
/// The system-assigned ID for a stream view
typedef ID StreamViewID;

/// Route notification types
typedef enum
{
    /// Adding a new route
    ADD = 0,
    /// Remove a new route
    DEL = 1,
    /// Update a new route
    UPDATE = 2
} RoutingNotificationType;

/// Stream view state types
/// Note:  this is used for serialization
typedef enum
{
    INITED = 0,
    FAILED = 1,
    CLOSED = 2,
    CLOSING = 3,
    OPENED = 4,
    OPENING = 5
} StreamViewStateType;

// Declaring *all* related exceptions
DECL_EXCEPTION(SAM_NAMESPACE, SAM, UTILS_NAMESPACE::Distillery);
DECL_EXCEPTION(SAM_NAMESPACE, CannotContactServer, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, GDG, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, InstanceShuttingDown, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidAttribute, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidProtocol, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, MalformedJob, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, MalformedXMLObject, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, SAMRoutingNotCurrent, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, SAMRoutingNotValid, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, UndefinedAAS, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, UndefinedSAM, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, UnknownEvent, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, ConnectionNotFound, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, GDGInternalError, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, IncompleteJobInformation, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidAppSetID, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidInputPortID, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidJobID, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidOutputPortID, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidPEType, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidQuery, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidStateTransition, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidScope, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidStreamID, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, JobCannotUnregister, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, PENotRunningYet, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, PENotTerminated, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, PECannotRestart, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, PEAlreadyTerminating, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, PECannotUnregister, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, PropertyAlreadyExists, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, StreamNotActive, GDG);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidPropertyType, GDG);

DECL_EXCEPTION(SAM_NAMESPACE, SAMStreamNotFound, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, SAMDuplicateStream, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, SAMPropertyNotFound, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, SAMDuplicateProperty, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, SAMInvalidSubscriptionExpression, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, SAMCannotUpdateNameBasedSubscription, SAM);

DECL_EXCEPTION(SAM_NAMESPACE, SAMJobNoPesStoppable, SAM);

DECL_EXCEPTION(SAM_NAMESPACE, SAMInvalidStreamId, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, SAMInvalidStreamViewId, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, InvalidJobName, SAM);

DECL_EXCEPTION(SAM_NAMESPACE, LogicalToPhysical, SAM);

DECL_EXCEPTION(SAM_NAMESPACE, PB, SAM);
DECL_EXCEPTION(SAM_NAMESPACE, Deserialization, PB);
DECL_EXCEPTION(SAM_NAMESPACE, IncompleteMessage, PB);

// exception strings
#define CANNOT_CONTACT_SCH_STR "Cannot contact scheduler (streams-sch)"
#define CANNOT_CONTACT_AAS_STR "Cannot contact security server (streams-aas)"
#define CANNOT_CONTACT_SRM_STR "Cannot contact streams resource manager (streams-srm)"

SAM_NAMESPACE_END

#endif
