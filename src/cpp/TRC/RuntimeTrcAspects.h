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

//----------------------------------------------------------------------------
// This header file define some common user-defined aspects for tracing for
// Runtime components -- SAM, SRM, SCH, NAM, AAS, HC, PEC, Transport, Core.
//
// The purpose is that, with this common header, Runtime can share some
// common aspects so to help debugging across services.
//
// To benefit debugging, following are suggested grammar for Runtime aspect
// constants:
// <aspectPrefix><component_ID>.<action>
//     aspectPrefix = ":::", this will be useful for doing greps on trace files.
//     component_ID = defined set, e.g.  SAM, SCH, SRM, etc.
//     action = verb-noun phrase, i.e. startPE.
// Note: For some common actions across several Runtime components, such as
// StartPE/StopPE across SAM, HC, Runtime suggest to use same action words
// so to help across component debugging
//----------------------------------------------------------------------------

#ifndef RUNTIME_TRC_ASPECTS_H_
#define RUNTIME_TRC_ASPECTS_H_

#include <TRC/TRCTypes.h>

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

const static std::string ASPECT_PREFIX = ":::";

// AAS aspects
const static std::string AAS_ASP = ASPECT_PREFIX + "AAS";
const static std::string AAS_OBJECT_ASP = ASPECT_PREFIX + "AAS.Object";
const static std::string AAS_CLIENT_ASP = ASPECT_PREFIX + "AAS.Client";
const static std::string AAS_STARTUP_DAEMON = ASPECT_PREFIX + "AAS.StartupDaemon";
const static std::string AAS_SHUTDOWN_DAEMON = ASPECT_PREFIX + "AAS.ShutdownDaemon";
const static std::string AAS_CREATE_SESSION_BY_PWD = ASPECT_PREFIX + "AAS.CreateSessionByPwd";
const static std::string AAS_CREATE_SESSION_BY_KEY = ASPECT_PREFIX + "AAS.CreateSessionByKey";
const static std::string AAS_COMPLETE_CHALLENGE = ASPECT_PREFIX + "AAS.CompleteChallenge";
const static std::string AAS_CREATE_SESSION_BY_TRUSTED_CLIENT =
  ASPECT_PREFIX + "AAS.CreateSessionByTrustedClient";
const static std::string AAS_DELETE_SESSION = ASPECT_PREFIX + "AAS.DeleteSession";
const static std::string AAS_DELETE_ALL_SESSION = ASPECT_PREFIX + "AAS.DeleteAllSession";
const static std::string AAS_ADD_OBJECT = ASPECT_PREFIX + "AAS.AddObject";
const static std::string AAS_GET_OBJECT = ASPECT_PREFIX + "AAS.GetObject";
const static std::string AAS_LIST_OBJECTS = ASPECT_PREFIX + "AAS.ListObjects";
const static std::string AAS_DELETE_OBJECT = ASPECT_PREFIX + "AAS.DeleteObject";
const static std::string AAS_GET_USERNAME = ASPECT_PREFIX + "AAS.GetUsername";
const static std::string AAS_GET_INFO = ASPECT_PREFIX + "AAS.GetInfo";
const static std::string AAS_UPDATE_USERS = ASPECT_PREFIX + "AAS.UpdateUsers";
const static std::string AAS_REMOVE_INSTANCE = ASPECT_PREFIX + "AAS.RemoveInstance";
const static std::string AAS_CHECK_PERMISSION_SESSION =
  ASPECT_PREFIX + "AAS.CheckPermissionSession";
const static std::string AAS_CHECK_PERMISSION_USER = ASPECT_PREFIX + "AAS.CheckPermissionUser";
const static std::string AAS_SESSION_AGE = ASPECT_PREFIX + "AAS.SessionAge";
const static std::string AAS_LOG_ACTIVITY = ASPECT_PREFIX + "AAS.LogActivity";
const static std::string AAS_DUMP = ASPECT_PREFIX + "AAS.Dump";
const static std::string AAS_STOP = ASPECT_PREFIX + "AAS.Stop";
const static std::string AAS_OBJECT_ADD_USER_PERM = ASPECT_PREFIX + "AAS.Object.AddUserPerm";
const static std::string AAS_OBJECT_REMOVE_USER_PERM = ASPECT_PREFIX + "AAS.Object.RemoveUserPerm";
const static std::string AAS_OBJECT_ADD_GROUP_PERM = ASPECT_PREFIX + "AAS.Object.AddGroupPerm";
const static std::string AAS_OBJECT_REMOVE_GROUP_PERM =
  ASPECT_PREFIX + "AAS.Object.RemoveGroupPerm";
const static std::string AAS_OBJECT_LIST_PERMISSIONS = ASPECT_PREFIX + "AAS.Object.ListPermissions";
const static std::string AAS_OBJECT_LIST_CHILDREN = ASPECT_PREFIX + "AAS.Object.ListChildren";
const static std::string AAS_OBJECT_SET_PERSISTENT = ASPECT_PREFIX + "AAS.Object.SetPersistent";

// CFG aspects
const static std::string CFG_ASP = ASPECT_PREFIX + "CFG";

// NAM aspects
const static std::string NAM_GENERAL = ASPECT_PREFIX + "NAM";
const static std::string NAM_GET_INSTANCE = ASPECT_PREFIX + "NAM.GetInstance";
const static std::string NAM_STARTUP_DAEMON = ASPECT_PREFIX + "NAM.StartupDaemon";
const static std::string NAM_REGISTER_ENTRY = ASPECT_PREFIX + "NAM.RegisterEntry";
const static std::string NAM_UNREGISTER_ENTRY = ASPECT_PREFIX + "NAM.UnregisterEntry";
const static std::string NAM_LOOKUP_ENTRY = ASPECT_PREFIX + "NAM.LookupEntry";
const static std::string NAM_LIST_ENTRY = ASPECT_PREFIX + "NAM.ListEntry";
const static std::string NAM_SHUTDOWN_DAEMON = ASPECT_PREFIX + "NAM.ShutdownDaemon";
const static std::string NAM_UPDATE_DAEMON_PROP = ASPECT_PREFIX + "NAM.UpdateDaemonProperties";
const static std::string NAM_GET_DAEMON_PROP = ASPECT_PREFIX + "NAM.GetDaemonProperties";
const static std::string NAM_USER_SERVICE = ASPECT_PREFIX + "NAM.UserService";
const static std::string NAM_ZK_GENERAL = ASPECT_PREFIX + "NAM.ZooKeeper";

// HC aspects
const static std::string HC_GENERAL = ASPECT_PREFIX + "HC";
const static std::string HC_STARTUP_DAEMON = ASPECT_PREFIX + "HC.StartupDaemon";
const static std::string HC_SHUTDOWN_DAEMON = ASPECT_PREFIX + "HC.ShutdownDaemon";
const static std::string HC_START_PE = ASPECT_PREFIX + "HC.StartPE";
const static std::string HC_STOP_PE = ASPECT_PREFIX + "HC.StopPE";
const static std::string HC_SET_PE_STATE = ASPECT_PREFIX + "HC.SetPEState";
const static std::string HC_GET_PE_STATE = ASPECT_PREFIX + "HC.GetPEState";
const static std::string HC_SET_PE_CONFIGS = ASPECT_PREFIX + "HC.SetPEConfigs";
const static std::string HC_SET_PEC_PROPS = ASPECT_PREFIX + "HC.SetPECProperties";
const static std::string HC_MONITOR_PE_HEALTH = ASPECT_PREFIX + "HC.MonitorPeHealth";
const static std::string HC_GET_METRICS = ASPECT_PREFIX + "HC.GetMetrics";
const static std::string HC_DELETE_JOB_LOG = ASPECT_PREFIX + "HC.DeleteJobLog";
const static std::string HC_GET_MANAGEMENT_STATE = ASPECT_PREFIX + "HC.GetManagementState";
const static std::string HC_SET_CONN_STATE = ASPECT_PREFIX + "HC.SetConnectionState";
const static std::string HC_GET_CONN_STATE = ASPECT_PREFIX + "HC.GetConnectionState";
const static std::string HC_POST_RECOVER_NOTE = ASPECT_PREFIX + "HC.PostRecoveryNotification";

// K8S aspects
const static std::string K8S_GENERAL = ASPECT_PREFIX + "K8S";

// PEC aspects
const static std::string PEC_GENERAL = ASPECT_PREFIX + "PEC";
const static std::string PEC_STARTUP_DAEMON = ASPECT_PREFIX + "PEC.StartupDaemon";
const static std::string PEC_SHUTDOWN = ASPECT_PREFIX + "PEC.Shutdown";
const static std::string PEC_START_PE = ASPECT_PREFIX + "PEC.StartPE";
const static std::string PEC_MONITOR_PE_HEALTH = ASPECT_PREFIX + "PEC.MonitorPeHealth";
const static std::string PEC_GET_METRICS = ASPECT_PREFIX + "PEC.GetMetrics";
const static std::string PEC_GET_SNAPSHOT = ASPECT_PREFIX + "PEC.GetStateSnapshot";
const static std::string PEC_ROUTING_NOTE = ASPECT_PREFIX + "PEC.NotifyRoutingInfo";
const static std::string PEC_CONN_STATE = ASPECT_PREFIX + "PEC.ConnectionState";
const static std::string PEC_POST_RECOVER_NOTE = ASPECT_PREFIX + "PEC.PostRecoveryNotification";
const static std::string PEC_SET_PE_CONFIGS = ASPECT_PREFIX + "PEC.SetPEConfigs";
const static std::string PEC_STREAM_VIEW_NOTE = ASPECT_PREFIX + "PEC.NotifyStreamView";

// SAM aspects
const static std::string SAM_ASP = ASPECT_PREFIX + "SAM";
const static std::string SAM_CLIENT_ASP = ASPECT_PREFIX + "SAM.Client";
const static std::string SAM_STARTUP_DAEMON = ASPECT_PREFIX + "SAM.StartupDaemon";
const static std::string SAM_SHUTDOWN_DAEMON = ASPECT_PREFIX + "SAM.ShutdownDaemon";
const static std::string SAM_SUBMIT_JOB = ASPECT_PREFIX + "SAM.SubmitJob";
const static std::string SAM_CANCEL_JOB = ASPECT_PREFIX + "SAM.CancelJob";
const static std::string SAM_STOP_PES_IN_JOB = ASPECT_PREFIX + "SAM.StopPEsInJob";
const static std::string SAM_RESTART_PE = ASPECT_PREFIX + "SAM.RestartPE";
const static std::string SAM_RESTART_PE_ON_HOST = ASPECT_PREFIX + "SAM.RestartPEOnHost";
const static std::string SAM_RESTART_PES_IN_JOB = ASPECT_PREFIX + "SAM.RestartPEsInJob";
const static std::string SAM_UPDATE_PE_CONFIG = ASPECT_PREFIX + "SAM.UpdatePEConfig";
const static std::string SAM_UPDATE_PE_CONFIGS_IN_JOB = ASPECT_PREFIX + "SAM.UpdatePEConfigsInJob";
const static std::string SAM_SET_PE_CONFIG_STATE = ASPECT_PREFIX + "SAM.SetPEConfigState";
const static std::string SAM_SET_PE_STATE = ASPECT_PREFIX + "SAM.SetPEState";
const static std::string SAM_SET_CONNECTION_STATE = ASPECT_PREFIX + "SAM.SetConnectionState";
const static std::string SAM_GET_APPLICATION_STATE = ASPECT_PREFIX + "SAM.GetApplicationState";
const static std::string SAM_GET_COMPOSITE_JOB_SCHEDULE =
  ASPECT_PREFIX + "SAM.GetCompositeJobSchedule";
const static std::string SAM_GET_SYSTEM_STATE = ASPECT_PREFIX + "SAM.GetSystemState";
const static std::string SAM_GET_SUMMARY_STATE = ASPECT_PREFIX + "SAM.GetSummaryState";
const static std::string SAM_ADD_STREAM_PROPERTY = ASPECT_PREFIX + "SAM.AddStreamProperty";
const static std::string SAM_ADD_STREAM_PROPERTIES = ASPECT_PREFIX + "SAM.AddStreamProperties";
const static std::string SAM_SET_STREAM_PROPERTY = ASPECT_PREFIX + "SAM.SetStreamProperty";
const static std::string SAM_SET_STREAM_PROPERTIES = ASPECT_PREFIX + "SAM.SetStreamProperties";
const static std::string SAM_GET_STREAM_PROPERTY = ASPECT_PREFIX + "SAM.GetStreamProperty";
const static std::string SAM_GET_STREAM_PROPERTIES = ASPECT_PREFIX + "SAM.GetStreamProperties";
const static std::string SAM_REMOVE_STREAM_PROPERTY = ASPECT_PREFIX + "SAM.RemoveStreamProperty";
const static std::string SAM_REMOVE_STREAM_PROPERTIES =
  ASPECT_PREFIX + "SAM.RemoveStreamProperties";
const static std::string SAM_SET_SUBSCRIPTION = ASPECT_PREFIX + "SAM.SetSubscription";
const static std::string SAM_GET_SUBSCRIPTION = ASPECT_PREFIX + "SAM.GetSubscription";
const static std::string SAM_SET_FILTER = ASPECT_PREFIX + "SAM.SetFilter";
const static std::string SAM_GET_FILTER = ASPECT_PREFIX + "SAM.GetFilter";
const static std::string SAM_HIBERNATE = ASPECT_PREFIX + "SAM.Hibernate";
const static std::string SAM_STOP = ASPECT_PREFIX + "SAM.Stop";
const static std::string SAM_GET_VIEWABLE_STREAMS = ASPECT_PREFIX + "SAM.GetViewableStreams";
const static std::string SAM_GET_VIEWABLE_STREAMS_METADATA =
  ASPECT_PREFIX + "SAM.GetViewableStreamsMetadata";
const static std::string SAM_OPEN_STREAM_VIEW = ASPECT_PREFIX + "SAM.OpenStreamView";
const static std::string SAM_CLOSE_STREAM_VIEW = ASPECT_PREFIX + "SAM.CloseStreamView";
const static std::string SAM_SET_STREAM_VIEW_PROPERTIES =
  ASPECT_PREFIX + "SAM.SetStreamViewProperties";
const static std::string SAM_GET_STREAM_VIEW_PROPERTIES =
  ASPECT_PREFIX + "SAM.GetStreamViewProperties";
const static std::string SAM_GET_OPEN_STREAM_VIEWS = ASPECT_PREFIX + "SAM.GetOpenStreamViews";
const static std::string SAM_SHUTDOWN = ASPECT_PREFIX + "SAM.Shutdown";
const static std::string SAM_QUIESCE = ASPECT_PREFIX + "SAM.Quiesce";
const static std::string SAM_START_PE = ASPECT_PREFIX + "SAM.StartPE";
const static std::string SAM_STOP_PE = ASPECT_PREFIX + "SAM.StopPE";

// SCH aspects
const static std::string SCH_INTERNAL = ASPECT_PREFIX + "SCH.Internal";
const static std::string SCH_GATHER_PE_METRICS = ASPECT_PREFIX + "SCH.GatherPeMetrics";
const static std::string SCH_COMPUTE_PLACEMENT = ASPECT_PREFIX + "SCH.ComputePlacement";
const static std::string SCH_COMPUTE_PLACEMENT_FOR_JOB =
  ASPECT_PREFIX + "SCH.ComputePlacementForJob";
const static std::string SCH_COMPUTE_PLACEMENT_FOR_PE = ASPECT_PREFIX + "SCH.ComputePlacementForPe";
const static std::string SCH_COMPUTE_PLACEMENT_FOR_PE_ON_HOST =
  ASPECT_PREFIX + "SCH.ComputePlacementForPeOnHost";
const static std::string SCH_COMPUTE_PE_RESTART_RECOMMENDATIONS =
  ASPECT_PREFIX + "SCH.ComputePeRestartRecommendations";
const static std::string SCH_COMPUTE_PE_RESTART_RECOMMENDATIONS_FOR_INSTANCE =
  ASPECT_PREFIX + "SCH.ComputePeRestartRecommendationsForInstance";
const static std::string SCH_COMPUTE_PE_RESTART_RECOMMENDATIONS_FOR_HOST =
  ASPECT_PREFIX + "SCH.ComputePeRestartRecommendationsForHost";
const static std::string SCH_COMPUTE_PE_RESTART_RECOMMENDATIONS_FOR_JOB =
  ASPECT_PREFIX + "SCH.ComputePeRestartRecommendationsForJob";

// SRM aspects
const static std::string SRM_ASP = ASPECT_PREFIX + "SRM";
const static std::string SRM_RETRIEVE_METRICS = ASPECT_PREFIX + "SRM.RetrieveMetrics";
const static std::string SRM_GET_METRICS_DATA = ASPECT_PREFIX + "SRM.GetMetricsData";
const static std::string SRM_METRICS = ASPECT_PREFIX + "SRM.Metrics";
const static std::string SRM_PINGER = ASPECT_PREFIX + "SRM.Pinger";
const static std::string SRM_STOP_INSTANCE = ASPECT_PREFIX + "SRM.StopInstance";
const static std::string SRM_ADD_HOST = ASPECT_PREFIX + "SRM.AddHost";
const static std::string SRM_REMOVE_HOST = ASPECT_PREFIX + "SRM.RemoveHost";
const static std::string SRM_ADD_HOST_PROPERTIES = ASPECT_PREFIX + "SRM.AddHostProperties";
const static std::string SRM_WAKEUP_HOST = ASPECT_PREFIX + "SRM.WakeupHost";
const static std::string SRM_QUIESCE_HOST = ASPECT_PREFIX + "SRM.QuiesceHost";
const static std::string SRM_START_SERVICE = ASPECT_PREFIX + "SRM.StartService";
const static std::string SRM_STOP_SERVICE = ASPECT_PREFIX + "SRM.StopService";
const static std::string SRM_RESTART_SERVICE = ASPECT_PREFIX + "SRM.RestartService";
const static std::string SRM_GET_INFRASTRUCTURE_STATE =
  ASPECT_PREFIX + "SRM.GetInfrastructureState";
const static std::string SRM_GET_INSTANCE_STATE = ASPECT_PREFIX + "SRM.GetInstanceState";
const static std::string SRM_SET_SERVICE_PROPERTIES = ASPECT_PREFIX + "SRM.SetServiceProperties";
const static std::string SRM_HOST_CHANGE_NOTIFICATION =
  ASPECT_PREFIX + "SRM.HostChangeNotification";
const static std::string SRM_INSTANCE_CHANGE_NOTIFICATION =
  ASPECT_PREFIX + "SRM.InstanceChangeNotification";
const static std::string SRM_REMOVE_HOST_COMPLETE = ASPECT_PREFIX + "SRM.RemoveHostComplete";
const static std::string SRM_QUIESCE_HOST_COMPLETE = ASPECT_PREFIX + "SRM.QuiesceHostComplete";
const static std::string SRM_STOP_SERVICE_COMPLETE = ASPECT_PREFIX + "SRM.StopServiceComplete";
const static std::string SRM_BULK_LOAD_INSTANCE_CONFIGURATION =
  ASPECT_PREFIX + "SRM.BulkLoadInstanceConfiguration";
const static std::string SRM_UPDATE_LIVENESS = ASPECT_PREFIX + "SRM.UpdateLiveness";
const static std::string SRM_INSTANCE_FSM = ASPECT_PREFIX + "SRM.InstanceFsm";
const static std::string SRM_DAEMON_FSM = ASPECT_PREFIX + "SRM.DaemonFsm";
const static std::string SRM_NODE_FSM = ASPECT_PREFIX + "SRM.NodeFsm";

// Transport aspects
const static std::string CORE_TRANS = ASPECT_PREFIX + "Core.Transport";
const static std::string CORE_TRANS_HS = ASPECT_PREFIX + "Core.Transport.Handshake";
const static std::string CORE_TRANS_TCP = ASPECT_PREFIX + "Core.Transport.Tcp";
const static std::string CORE_TRANS_TCP_SECURE = ASPECT_PREFIX + "Core.Transport.Tcp.Secure";

// Utils/Core aspects
const static std::string CORE_UTILS = ASPECT_PREFIX + "Core.Utils";
const static std::string FSM_ASP = ASPECT_PREFIX + "Core.Fsm";
const static std::string CORE_CORBA = ASPECT_PREFIX + "Core.Corba";
const static std::string CORE_CORBA_MSG = ASPECT_PREFIX + "Core.Corba.Message";
const static std::string CORE_DB = ASPECT_PREFIX + "Core.Database";
const static std::string CORE_DAPP = ASPECT_PREFIX + "Core.DistilleryApp";
const static std::string CORE_DCORBA = ASPECT_PREFIX + "Core.DistilleryCorba";
const static std::string CORE_PC = ASPECT_PREFIX + "Core.PerfCounter";
const static std::string CORE_RPC = ASPECT_PREFIX + "Core.Rpc";

// define macro for method begin/end trace statements
#define SPCDBG_BEGIN(aspect) SPCDBG(L_DEBUG, "Enter: ", aspect)
#define SPCDBG_EXIT(aspect) SPCDBG(L_DEBUG, "Exit: ", aspect)

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif
