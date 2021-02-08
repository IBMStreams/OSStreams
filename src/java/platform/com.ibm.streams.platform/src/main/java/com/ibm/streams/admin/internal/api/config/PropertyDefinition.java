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

package com.ibm.streams.admin.internal.api.config;

import com.ibm.streams.admin.internal.api.trace.TraceLogger;
import java.util.Hashtable;
import java.util.Map;

public class PropertyDefinition {

  /*
     Steps to add a new property:
     *** Best chance of getting it right is to copy a similar property

     CFGMGR
     ------
     * PropertyDefinition
       * add to PropertyID enum, if enumeration make sure enumeration class name passed on enum ctor too
       * add new enumeration if needed
     * PropertyInformation
       * add to PropertyDefinitionEnum
     * MsgOptions.properties
       * Add a place holder description so Domain Manager is not broken (ID will overwrite with the real description)
     * Config subcategory part  (e.g. DomainSwsProperties.java)
       * addPropID with new property
       * add get, set, get definition methods as necessary

     Internal API level
     ------------------
     * If top-level domain property, add get/set methods to StreamsDomain
     * If top-level instance property, add get/set methods to StreamsInstance
     * If a service property, add get/set methods to xxxService

     JMX
     ---
     * If it is a domain property, add to PropertyId enumeration in DomainMXBean
     * If it is an instance property, add to PropertyId enumeration in InstanceMXBean
     * If it is an instance property, add get/set implementation to xxxProxy part,
       implementation must include call to verifyCurrentVersionSupports
     * If a service property
       * update getPropertyIds list in xxxServiceBean
       * If need an enum, add it to DomainServiceMXBean or InstanceServiceMXBean
     * If not a service property
       * Add get/set methods to xxxMXBean part under com.ibm.streams.management.mx.api  (in alphabetical order)
       * Add enum to xxxMXBean if needed
       * Add get/set implementation to xxxBean part under com.ibm.streams.management.mx.impl
     * If adding a new enumeration value to an existing enumeration property ....
     * ApiEnum
       * If it is a domain property, add to _domainPropIdMap
       * If it is an instance property, add to _instancePropIdMap
       * If it is an enumeration, add an enum map
       * If it is an enumeration, add to _enumMap

     Design
     ------
     * Update the config RTC task for the release with the change and the updated spreadsheet
     * Update the JMX RTC task for the release with the interface change
  */

  private static final Map<PropertyID, PropertyDefinition> _propDefs =
      new Hashtable<PropertyID, PropertyDefinition>();
  private static final String CONFIG_PROTOBUF = "config.protobuf";

  private enum PropertyCategory {
    DOMAIN,
    INSTANCE
  }

  public enum PropertySubCategory {
    DOMAIN,
    AAS,
    SWS,
    JMX,
    DOMAIN_LOG,
    DOMAIN_TRACE,
    SECURITY,
    CONTROLLER,
    AUDIT_LOG,
    DOMAIN_NETWORK,
    INSTANCE,
    INSTANCE_TRACE,
    INSTANCE_NETWORK,
    SAM,
    SRM,
    SCH,
    HC,
    VIEW,
    APP,
    BUILD
  }

  private enum AddlAuthCheck {
    DOMAIN_ADMIN_ON_GET,
    DOMAIN_ADMIN_ON_SET
  }

  public enum InitialRelease {
    OLETA_42_OR_EARLIER,
    TIBER_421,
    FIRSTCLOUD_422,
    WDPWAMS_424,
    STREAMS_430
  } // If add new value, also update _propInitialReleaseMap and
  // verifyCurrentVersionSupports(PropertyId id) in
  // InstanceAbstractProxy

  public enum PropertyID {}

  public enum PropertyType {
    STRING,
    BOOLEAN,
    DOUBLE,
    INTEGER,
    LONG
  }

  public enum UpdateType {
    IMMEDIATE,
    AFTER_DOMAIN_RESTART,
    AFTER_INSTANCE_RESTART,
    AFTER_SWS_RESTART,
    AFTER_AUDITLOG_RESTART,
    AFTER_CONTROLLER_RESTART,
    AFTER_JMX_RESTART
  }

  public enum ConstraintType {
    NONE,
    ENUMERATION,
    RANGE,
    REGEX,
    COMMON_VALUES,
    LESS_THAN,
    GREATER_THAN,
    LOCALE,
    CUSTOM
  }

  public enum LogLevel {
    ERROR("error"),
    WARNING("warn"),
    INFORMATIONAL("info");

    private String _logLevelStr;
    private static Map<String, LogLevel> _logLevelMap;

    static {
      _logLevelMap = new Hashtable<String, LogLevel>();
      LogLevel[] logLevels = values();
      for (int i = 0; i < logLevels.length; ++i) {
        _logLevelMap.put(logLevels[i].toString(), logLevels[i]);
      }
    }

    LogLevel(String logLevel) {
      _logLevelStr = logLevel;
    }

    public String toString() {
      return _logLevelStr;
    }

    public static LogLevel getLevel(String logLevel) {
      return _logLevelMap.get(logLevel);
    }
  }

  public enum LogType {
    FILE("file"),
    SYSTEM_LOG("syslog");

    private String _logTypeStr;
    private static Map<String, LogType> _logTypeMap;

    static {
      _logTypeMap = new Hashtable<String, LogType>();
      LogType[] logTypes = values();
      for (int i = 0; i < logTypes.length; ++i) {
        _logTypeMap.put(logTypes[i].toString(), logTypes[i]);
      }
    }

    LogType(String logType) {
      _logTypeStr = logType;
    }

    public String toString() {
      return _logTypeStr;
    }

    public static LogType getType(String logType) {
      return _logTypeMap.get(logType);
    }
  }

  public enum CheckpointRepository {
    NOT_SPECIFIED("notSpecified"),
    FILE_SYSTEM("fileSystem"),
    REDIS("redis"),
    IBM_CLOUD_OBJECT_STORAGE("ibmCloudObjectStorage");

    private String _checkpointRepositoryStr;
    private static Map<String, CheckpointRepository> _checpointRepositoryMap;

    static {
      _checpointRepositoryMap = new Hashtable<String, CheckpointRepository>();
      CheckpointRepository[] repositoryTypes = values();
      for (int i = 0; i < repositoryTypes.length; ++i) {
        _checpointRepositoryMap.put(repositoryTypes[i].toString(), repositoryTypes[i]);
      }
    }

    CheckpointRepository(String repositoryType) {
      _checkpointRepositoryStr = repositoryType;
    }

    public String toString() {
      return _checkpointRepositoryStr;
    }

    public static CheckpointRepository getRepositoryType(String repositoryType) {
      return _checpointRepositoryMap.get(repositoryType);
    }
  }

  public enum SSLProtocol {
    USE_JAVA_SETTING("useJavaSetting"),
    SSL_TLS("SSL_TLS"),
    SSL_TLSv2("SSL_TLSv2"),
    TLSv1("TLSv1"),
    TLSv1_1("TLSv1.1"),
    TLSv1_2("TLSv1.2");

    private String _sslProtocolStr;
    private static Map<String, SSLProtocol> _sslProtocolMap;

    static {
      _sslProtocolMap = new Hashtable<String, SSLProtocol>();
      SSLProtocol[] sslProtocols = values();
      for (int i = 0; i < sslProtocols.length; ++i) {
        _sslProtocolMap.put(sslProtocols[i].toString(), sslProtocols[i]);
      }
    }

    SSLProtocol(String sslProtocol) {
      _sslProtocolStr = sslProtocol;
    }

    public String toString() {
      return _sslProtocolStr;
    }

    public static SSLProtocol getProtocol(String sslProtocol) {
      return _sslProtocolMap.get(sslProtocol);
    }
  }

  public enum SSLOption {
    SSL_TLS("SSL_TLS"),
    SSL_TLSv2("SSL_TLSv2"),
    TLSv1("TLSv1"),
    TLSv1_1("TLSv1.1"),
    TLSv1_2("TLSv1.2"),
    NONE("none");

    private String _sslOptionStr;
    private static Map<String, SSLOption> _sslOptionMap;

    static {
      _sslOptionMap = new Hashtable<String, SSLOption>();
      SSLOption[] sslOptions = values();
      for (int i = 0; i < sslOptions.length; ++i) {
        _sslOptionMap.put(sslOptions[i].toString(), sslOptions[i]);
      }
    }

    SSLOption(String sslOption) {
      _sslOptionStr = sslOption;
    }

    public String toString() {
      return _sslOptionStr;
    }

    public static SSLOption getOption(String sslOption) {
      return _sslOptionMap.get(sslOption);
    }
  }

  public enum TransportSecurityType {
    SSLv3("SSLv3"),
    TLSv1("TLSv1"),
    TLSv1_1("TLSv1.1"),
    TLSv1_2("TLSv1.2"),
    NONE("none");

    private String _transportSecurityTypeStr;
    private static Map<String, TransportSecurityType> _transportSecurityTypeMap;

    static {
      _transportSecurityTypeMap = new Hashtable<String, TransportSecurityType>();
      TransportSecurityType[] transportSecurityTypes = values();
      for (int i = 0; i < transportSecurityTypes.length; ++i) {
        _transportSecurityTypeMap.put(
            transportSecurityTypes[i].toString(), transportSecurityTypes[i]);
      }
    }

    TransportSecurityType(String transportSecurityType) {
      _transportSecurityTypeStr = transportSecurityType;
    }

    public String toString() {
      return _transportSecurityTypeStr;
    }

    public static TransportSecurityType getOption(String transportSecurityType) {
      return _transportSecurityTypeMap.get(transportSecurityType);
    }
  }

  public enum AuthenticationType {
    PAM("pam"),
    LDAP("ldap");

    private String _authenticationTypeStr;
    private static Map<String, AuthenticationType> _authenticationTypeMap;

    static {
      _authenticationTypeMap = new Hashtable<String, AuthenticationType>();
      AuthenticationType[] authenticationTypes = values();
      for (int i = 0; i < authenticationTypes.length; ++i) {
        _authenticationTypeMap.put(authenticationTypes[i].toString(), authenticationTypes[i]);
      }
    }

    AuthenticationType(String authenticationType) {
      _authenticationTypeStr = authenticationType;
    }

    public String toString() {
      return _authenticationTypeStr;
    }

    public static AuthenticationType getType(String authenticationType) {
      return _authenticationTypeMap.get(authenticationType);
    }
  }

  public enum ApplicationResourceAllocationMode {
    JOB("job"),
    INSTANCE("instance");

    private String _applicationResourceAllocationModeStr;
    private static Map<String, ApplicationResourceAllocationMode>
        _applicationResourceAllocationModeMap;

    static {
      _applicationResourceAllocationModeMap =
          new Hashtable<String, ApplicationResourceAllocationMode>();
      ApplicationResourceAllocationMode[] applicationResourceAllocationModes = values();
      for (int i = 0; i < applicationResourceAllocationModes.length; ++i) {
        _applicationResourceAllocationModeMap.put(
            applicationResourceAllocationModes[i].toString(),
            applicationResourceAllocationModes[i]);
      }
    }

    ApplicationResourceAllocationMode(String applicationResourceAllocationMode) {
      _applicationResourceAllocationModeStr = applicationResourceAllocationMode;
    }

    public String toString() {
      return _applicationResourceAllocationModeStr;
    }

    public static ApplicationResourceAllocationMode getMode(
        String applicationResourceAllocationMode) {
      return _applicationResourceAllocationModeMap.get(applicationResourceAllocationMode);
    }
  }

  public enum JobResourceSharing {
    SAME_JOB("sameJob"),
    SAME_USER("sameUser"),
    SAME_INSTANCE("sameInstance");

    private String _jobResourceSharingStr;
    private static Map<String, JobResourceSharing> _jobResourceSharingMap;

    static {
      _jobResourceSharingMap = new Hashtable<String, JobResourceSharing>();
      JobResourceSharing[] sharingConstraints = values();
      for (int i = 0; i < sharingConstraints.length; ++i) {
        _jobResourceSharingMap.put(sharingConstraints[i].toString(), sharingConstraints[i]);
      }
    }

    JobResourceSharing(String sharingConstraint) {
      _jobResourceSharingStr = sharingConstraint;
    }

    public String toString() {
      return _jobResourceSharingStr;
    }

    public static JobResourceSharing getType(String sharingConstraint) {
      return _jobResourceSharingMap.get(sharingConstraint);
    }
  }

  public enum AuditLogLevel {
    OFF("off"),
    STANDARD("standard");

    private String _auditLogLevelStr;
    private static Map<String, AuditLogLevel> _auditLogLevelMap;

    static {
      _auditLogLevelMap = new Hashtable<String, AuditLogLevel>();
      AuditLogLevel[] auditLogLevels = values();
      for (int i = 0; i < auditLogLevels.length; ++i) {
        _auditLogLevelMap.put(auditLogLevels[i].toString(), auditLogLevels[i]);
      }
    }

    AuditLogLevel(String auditLogLevel) {
      _auditLogLevelStr = auditLogLevel;
    }

    public String toString() {
      return _auditLogLevelStr;
    }

    public static AuditLogLevel getType(String auditLogLevel) {
      return _auditLogLevelMap.get(auditLogLevel);
    }
  }

  public enum TraceLevel {
    DEFAULT("default"), // $$$ see if we can get rid of this one
    OFF("off"),
    ERROR("error"),
    WARNING("warn"),
    INFORMATIONAL("info"),
    DEBUG("debug"),
    TRACE("trace");

    private String _traceLevelStr;
    private static Map<String, TraceLevel> _traceLevelMap;

    static {
      _traceLevelMap = new Hashtable<String, TraceLevel>();
      TraceLevel[] traceLevels = values();
      for (int i = 0; i < traceLevels.length; ++i) {
        _traceLevelMap.put(traceLevels[i].toString(), traceLevels[i]);
      }
    }

    TraceLevel(String traceLevel) {
      _traceLevelStr = traceLevel;
    }

    public String toString() {
      return _traceLevelStr;
    }

    public static TraceLevel getLevel(String traceLevel) {
      return _traceLevelMap.get(traceLevel);
    }

    public static TraceLogger.Level translate(TraceLevel traceLevel) {
      TraceLogger.Level level = TraceLogger.Level.ERROR;
      switch (traceLevel) {
        case OFF:
          level = TraceLogger.Level.OFF;
          break;
        case WARNING:
          level = TraceLogger.Level.WARN;
          break;
        case INFORMATIONAL:
          level = TraceLogger.Level.INFO;
          break;
        case DEBUG:
          level = TraceLogger.Level.DEBUG;
          break;
        case TRACE:
          level = TraceLogger.Level.TRACE;
          break;
        case DEFAULT:
        case ERROR:
          level = TraceLogger.Level.ERROR;
      }
      return level;
    }
  }

  public enum RpcRetryMode {
    BOUNDED("bounded"),
    UNBOUNDED("unbounded");

    private String _rpcRetryModeStr;
    private static Map<String, RpcRetryMode> _rpcRetryModeMap;

    static {
      _rpcRetryModeMap = new Hashtable<String, RpcRetryMode>();
      RpcRetryMode[] retryModes = values();
      for (int i = 0; i < retryModes.length; ++i) {
        _rpcRetryModeMap.put(retryModes[i].toString(), retryModes[i]);
      }
    }

    RpcRetryMode(String retryMode) {
      _rpcRetryModeStr = retryMode;
    }

    public String toString() {
      return _rpcRetryModeStr;
    }

    public static RpcRetryMode getMode(String retryMode) {
      return _rpcRetryModeMap.get(retryMode);
    }
  }

  public enum FusionScheme {
    LEGACY("legacy"),
    AUTOMATIC("automatic"),
    MANUAL("manual");

    private String _fusionSchemeStr;
    private static Map<String, FusionScheme> _fusionSchemeMap;

    static {
      _fusionSchemeMap = new Hashtable<String, FusionScheme>();
      FusionScheme[] fusionSchemes = values();
      for (int i = 0; i < fusionSchemes.length; ++i) {
        _fusionSchemeMap.put(fusionSchemes[i].toString(), fusionSchemes[i]);
      }
    }

    FusionScheme(String fusionScheme) {
      _fusionSchemeStr = fusionScheme;
    }

    public String toString() {
      return _fusionSchemeStr;
    }

    public static FusionScheme getScheme(String fusionScheme) {
      return _fusionSchemeMap.get(fusionScheme);
    }
  }

  public enum PlacementScheme {
    LEGACY("legacy"),
    BALANCED_JOB("balancedJob"),
    BALANCED_INSTANCE("balancedInstance");

    private String _plcmtSchemeStr;
    private static Map<String, PlacementScheme> _plcmtSchemeMap;

    static {
      _plcmtSchemeMap = new Hashtable<>();
      PlacementScheme[] plcmtSchemes = values();
      for (int i = 0; i < plcmtSchemes.length; ++i) {
        _plcmtSchemeMap.put(plcmtSchemes[i].toString(), plcmtSchemes[i]);
      }
    }

    PlacementScheme(String plcmtScheme) {
      _plcmtSchemeStr = plcmtScheme;
    }

    public String toString() {
      return _plcmtSchemeStr;
    }

    public static PlacementScheme getScheme(String plcmtScheme) {
      return _plcmtSchemeMap.get(plcmtScheme);
    }
  }

  public enum ThreadingModel {
    NOT_SPECIFIED("notSpecified"),
    AUTOMATIC("automatic"),
    MANUAL("manual"),
    DEDICATED("dedicated"),
    DYNAMIC("dynamic");

    private String _threadingModelStr;
    private static Map<String, ThreadingModel> _threadingModelMap;

    static {
      _threadingModelMap = new Hashtable<String, ThreadingModel>();
      ThreadingModel[] threadingModels = values();
      for (int i = 0; i < threadingModels.length; ++i) {
        _threadingModelMap.put(threadingModels[i].toString(), threadingModels[i]);
      }
    }

    ThreadingModel(String threadingModel) {
      _threadingModelStr = threadingModel;
    }

    public String toString() {
      return _threadingModelStr;
    }

    public static ThreadingModel getModel(String threadingModel) {
      return _threadingModelMap.get(threadingModel);
    }
  }

  public enum RevocationMethod {
    AUTOMATIC("automatic"),
    CRL("crl"),
    OCSP("ocsp"),
    NONE("none");

    private String _revocationMethodStr;
    private static Map<String, RevocationMethod> _revocationMethodMap;

    static {
      _revocationMethodMap = new Hashtable<String, RevocationMethod>();
      RevocationMethod[] revocationMethods = values();
      for (int i = 0; i < revocationMethods.length; ++i) {
        _revocationMethodMap.put(revocationMethods[i].toString(), revocationMethods[i]);
      }
    }

    RevocationMethod(String revocationMethod) {
      _revocationMethodStr = revocationMethod;
    }

    public String toString() {
      return _revocationMethodStr;
    }

    public static RevocationMethod getMethod(String revocationMethod) {
      return _revocationMethodMap.get(revocationMethod);
    }
  }
}
