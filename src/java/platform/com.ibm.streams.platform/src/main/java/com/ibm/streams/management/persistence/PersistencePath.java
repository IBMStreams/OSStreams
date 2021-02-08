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

package com.ibm.streams.management.persistence;

public class PersistencePath {
  // path constants
  public static final String STREAMS = "/com.ibm.im.streams";
  public static final String DOMAINS = "/com.ibm.im.streams/domains";
  public static final String MKDOMAIN_LOCK = "/com.ibm.im.streams/lock";
  public static final String STREAMS_RESOURCEMGR = "/com.ibm.im.streams.resourcemgr";

  public static final String EMBEDDEDZK = "/com.ibm.im.streams/embeddedzk";
  public static final String EMBEDDEDZK_CLIENTS = "/com.ibm.im.streams/embeddedzk/clients";
  public static final String EMBEDDEDZK_LOCK = "/com.ibm.im.streams/embeddedzk/lock";

  public static final String ZKPERFORMANCE = "/com.ibm.im.streams/zkperformance";

  // service id's
  public static final String AAS = "aas";
  public static final String APP = "app";
  public static final String HC = "hc";
  public static final String SAM = "sam";
  public static final String SCH = "sch";
  public static final String SRM = "srm";
  public static final String SWS = "sws";
  public static final String BUILDSERVICE = "build";
  public static final String NAMESERVICE = "nameservice";
  public static final String CONTROLLER = "controller";
  public static final String CONTAINER = "container";
  public static final String JMX = "jmx";
  public static final String AUDITLOG = "auditlog";
  public static final String VIEW = "view";
  public static final String PEC = "pec";
  public static final String DISTRIBUTED_PE = "distributed-pe";
  public static final String TRANS = "trans";

  // property id's
  public static final String USER = "user";
  public static final String LOCALE = "locale";
  public static final String TIME = "time";
  public static final String CREATE_TIME = "createTime";
  public static final String RESOURCE = "resource";
  public static final String RESOURCEMGR = "resourcemgr";
  public static final String ID = "id";
  public static final String NAME = "name";
  public static final String HOSTNAME = "hostname";
  public static final String TYPE = "type";
  public static final String VALUE = "value";
  public static final String DESCRIPTION = "description";
  public static final String KIND = "kind";
  public static final String IP_ADDR = "ipAddr";
  public static final String PREFERRED_IP_ADDR = "preferredIpAddr";
  public static final String LEADER = "leader";
  public static final String PORT = "port";
  public static final String LOCAL_PORT = "localport";
  public static final String HTTP_PORT = "httpport";
  public static final String URL = "url";
  public static final String REST = "rest";
  public static final String PID = "pid";
  public static final String SERVICE = "service";
  public static final String REGISTERED = "registered";
  public static final String ROOT = "root";
  public static final String MASTER = "master";
  public static final String REPLACE = "replace";
  public static final String RESTART = "restart";
  public static final String REBOOT = "reboot";
  public static final String UPDATE = "update";
  public static final String CLEAN = "clean";
  public static final String RELEASED = "released";
  public static final String SCH_RELEASED = "schedulerReleased";
  public static final String REVOKED = "revoked";
  public static final String RESTRICTED = "restricted";
  public static final String UNREGISTER = "unregister";
  public static final String TIMEOUT = "timeout";
  public static final String COUNT = "count";
  public static final String TAGS = "tags";
  public static final String RESTRICTED_TAGS = "restrictedTags";
  public static final String RESOURCE_SPEC = "resourcespec";
  public static final String DYNAMIC = "dynamic";
  public static final String PENDING_ID = "pendingId";
  public static final String EXCLUSIVE = "exclusive";
  public static final String INSTANCE = "instance";
  public static final String CPU_COUNT = "cpuCount";
  public static final String CPU_COUNT_ACTUAL = "cpuCountActual";
  public static final String CPU_SPEED = "cpuSpeed";
  public static final String CPU_LOAD_AVERAGE = "cpuLoadAvg";
  public static final String CPU_LOAD_AVERAGE_CPUS = "cpuLoadAvgCpus";
  public static final String CPU_UTIL = "cpuUtil";
  public static final String CPU_UTIL_SHAPED = "cpuUtil_shaped";
  public static final String MEM_TOTAL = "memoryTotal";
  public static final String MEM_FREE = "memoryFree";
  public static final String MEM_UTIL = "memoryUtil";
  public static final String MEM_UTIL_SHAPED = "memoryUtil_shaped";
  public static final String NETWORK_SPEED = "networkSpeed";
  public static final String NETWORK_UTIL = "networkUtil";
  public static final String NETWORK_UTIL_SHAPED = "networkUtil_shaped";
  public static final String OS = "OS";
  public static final String OSVERSION = "OS.VERSION";
  public static final String OSPATCH = "OS.PATCH";
  public static final String OSUSER = "OS.USER";
  public static final String ARCHITECTURE = "Architecture";
  public static final String INSTALL = "install";
  public static final String VERSION = "Version";
  public static final String PRODUCT_NAME = "ProductName";
  public static final String BUILD = "Build";
  public static final String BUNDLE = "bundle";
  public static final String EDITION_NAME = "EditionName";
  public static final String EDITION_SHORT_NAME = "Edition";
  public static final String HOTFIX = "HotFix";
  public static final String UUID = "uuid";
  public static final String REPOSITORY = "repository";
  public static final String BUNDLES = "bundles";
  public static final String APPS = "applications";
  public static final String APPCACHE = "appcache";
  public static final String BUILDS = "builds";
  public static final String BUILDARCHIVES = "buildarchives";
  public static final String CHARTS = "charts";
  public static final String VIEWS = "views";
  public static final String EXIT_CODE = "exitCode";
  public static final String EXIT_CODE_DATA_1 = "exit_code_data_1";
  public static final String EXIT_CODE_DATA_2 = "exit_code_data_2";
  public static final String EXIT_CODE_DATA_3 = "exit_code_data_3";
  public static final String EXIT_CODE_NUM_OF_UNAVAIL_RES = "numOfUnavailRes";
  public static final String MSG_ID = "msgId";
  public static final String MSG = "msg";
  public static final String LOG_LEVEL = "logLevel";
  public static final String PING_TIMESTAMP = "pingTimestamp"; // ping timestamp
  public static final String STATUS = "status"; // some detail status of a service
  public static final String STATUS_REASON =
      "statusReason"; // list of reasons related to service status
  public static final String STATE = "state";
  public static final String STATE_REASON = "stateReason";
  public static final String CLIENT_CONNECTION =
      "clientConnection"; // client connection to the service (boolean)
  public static final String PE_COUNT = "peCount"; // how many PEs running with the resource
  public static final String PE_RELOCATED =
      "peRelocated"; // indicates if sam has relocated pes (boolean)
  public static final String FILENAME = "fileName";
  public static final String COMMAND = "command";
  public static final String DOWNLD_AND_INSTALL_CMD = "dnLoadAndInstallCmd";
  public static final String INSTALL_CMD = "installCmd";
  public static final String UNINSTALL_CMD = "unInstallCmd";
  public static final String PREPARE_CWD_CMD = "prepareCwdCmd";
  public static final String INSTANCE_ID = "instanceId";
  public static final String JOB_ID = "jobId";
  public static final String PATH = "path";
  public static final String CWD_PATH = "currentWorkingDir";
  public static final String CHECKSUM = "checkSum";
  public static final String STALENESS_COUNT = "stalenessCount";
  public static final String ZK_DISCONNECT = "zkDisconnect";
  public static final String ZK_RECONNECT = "zkReconnect";
  public static final String JMX_CONNECT = "jmx.connect";
  public static final String JMX_HTTP = "jmx.http";
  public static final String JMX_APPREPOS_HTTP = "jmx.apprepos.http";
  public static final String PREPARE_PINVD_CMD = "preparePinvdCmd";
  public static final String PINVD_PATH = "peInvocationDir";
  public static final String PE_ID = "peId";
  public static final String MOVING = "moving";
  public static final String PRIOR_VERSION = "priorVersion";
  public static final String DUPLICATE = "duplicate";
  public static final String IS_REDUNDANT = "isRedundant";
}
