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

package com.ibm.streams.admin.internal.core.api;

import com.ibm.icu.text.Collator;
import com.ibm.streams.admin.internal.api.trace.Trace;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class ApiProperties {
  public static enum PropertyType {
    // Constructor parameters:
    // 1. API string - this is part of the API definition and should not be changed
    // 2. Value type for comparisons
    // 3. Parent object parameter for an object member property
    // Note: the API strings need to be unique. If two different resources use the same
    //       name but different types, the ValueType must be VARIABLE and both the base
    //       property and the qualified property need to be in the supported properties
    //       list for the resource type. This does not apply to array property members because
    //       they cannot be sorted or filtered. For an example, see how ID, ID_STRING, and
    //       ID_BIGINT are used. VARIABLE type can also be used to return a value as one
    //       type but sort and filter as another - see PID and PID_BIGINT.
    REST_ID("restid", ValueType.STRING),
    RESOURCE_TYPE("resourceType", ValueType.STRING),
    SELF("self", ValueType.STRING),
    INSTALLATIONS("installations", ValueType.STRING),
    DOMAIN("domain", ValueType.STRING),
    DOMAINS("domains", ValueType.STRING),
    INSTANCE("instance", ValueType.STRING),
    INSTANCES("instances", ValueType.STRING),
    HOST("host", ValueType.STRING),
    HOSTS("hosts", ValueType.STRING),
    RESOURCE("resource", ValueType.STRING),
    RESOURCES("resources", ValueType.STRING),
    BUILD("build", ValueType.STRING),
    BUILDS("builds", ValueType.STRING),
    JOB("job", ValueType.STRING),
    JOBS("jobs", ValueType.STRING),
    PE("pe", ValueType.STRING),
    PES("pes", ValueType.STRING),
    OPERATOR("operator", ValueType.STRING),
    OPERATORS("operators", ValueType.STRING),
    CONNECTIONS("connections", ValueType.STRING),
    PE_CONNECTION("peConnection", ValueType.STRING),
    PE_CONNECTIONS("peConnections", ValueType.STRING),
    OPERATOR_CONNECTIONS("operatorConnections", ValueType.STRING),
    INPUT_PORTS("inputPorts", ValueType.STRING),
    OUTPUT_PORTS("outputPorts", ValueType.STRING),
    METRICS("metrics", ValueType.STRING),
    CONFIGURED_VIEW("configuredView", ValueType.STRING),
    CONFIGURED_VIEWS("configuredViews", ValueType.STRING),
    ACTIVE_VIEW("activeView", ValueType.STRING),
    ACTIVE_VIEWS("activeViews", ValueType.STRING),
    VIEW_ITEMS("viewItems", ValueType.STRING),
    TOTAL("total", ValueType.INTEGER),
    PREVIOUS("previous", ValueType.STRING),
    NEXT("next", ValueType.STRING),
    ID("id", ValueType.VARIABLE),
    ID_STRING(ValueType.STRING.toString() + "." + ID.toApiStr(), ValueType.STRING),
    ID_BIGINT(ValueType.BIGINTEGER.toString() + "." + ID.toApiStr(), ValueType.BIGINTEGER),
    NAME("name", ValueType.STRING),
    ARRAY_VALUE_STRING(
        "value",
        ValueType.STRING), // define duplicate array "value" first so top-level "value" gets in
    // _propTypeMap
    VALUE("value", ValueType.LONG),
    STREAM_NAME("streamName", ValueType.STRING),
    STREAM_VIEWABLE("streamViewable", ValueType.BOOLEAN),
    SUBMIT_TIME("submitTime", ValueType.LONG),
    STARTED_BY("startedBy", ValueType.STRING),
    STATUS("status", ValueType.STRING),
    HEALTH("health", ValueType.STRING),
    APPLICATION_VERSION("applicationVersion", ValueType.STRING),
    APPLICATION_PATH("applicationPath", ValueType.STRING),
    APPLICATION_NAME("applicationName", ValueType.STRING),
    DATA_PATH("dataPath", ValueType.STRING),
    OUTPUT_PATH("outputPath", ValueType.STRING),
    CHECKPOINT_PATH("checkpointPath", ValueType.STRING),
    ADL_FILE("adlFile", ValueType.STRING),
    IP_ADDRESS("ipAddress", ValueType.STRING),
    PROCESSOR_COUNT("processorCount", ValueType.LONG),
    SCHEDULER_STATE("schedulerState", ValueType.STRING),
    STATUS_REASON("statusReason", ValueType.STRING),
    STATUS_REASONS("statusReasons", ValueType.ARRAY),
    PID("processId", ValueType.VARIABLE),
    PID_BIGINT(ValueType.BIGINTEGER.toString() + "." + PID.toApiStr(), ValueType.BIGINTEGER),
    TRACING_LEVEL("tracingLevel", ValueType.STRING),
    PENDING_TRACING_LEVEL("pendingTracingLevel", ValueType.STRING),
    RELOCATABLE("relocatable", ValueType.BOOLEAN),
    RESTARTABLE("restartable", ValueType.BOOLEAN),
    RESTART_PRIORITY("restartPriority", ValueType.STRING),
    CURRENT_WORKING_PATH("currentWorkingPath", ValueType.STRING),
    LAUNCH_COUNT("launchCount", ValueType.LONG),
    INDEX_WITHIN_JOB("indexWithinJob", ValueType.LONG),
    INDEX_WITHIN_PE("indexWithinPE", ValueType.LONG),
    INDEX_WITHIN_OPERATOR("indexWithinOperator", ValueType.LONG),
    INDEX_WITHIN_REGION("indexWithinRegion", ValueType.LONG),
    OPERATOR_KIND("operatorKind", ValueType.STRING),
    OWNER("owner", ValueType.STRING),
    START_TIME("startTime", ValueType.LONG),
    ARCHITECTURE("architecture", ValueType.STRING),
    BUILD_VERSION("buildVersion", ValueType.STRING),
    INSTALL_PATH("installPath", ValueType.STRING),
    MINIMUM_OS_VERSION("minimumOSVersion", ValueType.STRING),
    OS_BASE_VERSION("minimumOSBaseVersion", ValueType.STRING),
    OS_PATCH_VERSION("minimumOSPatchVersion", ValueType.STRING),
    PRODUCT_NAME("productName", ValueType.STRING),
    PRODUCT_VERSION("productVersion", ValueType.STRING),
    FULL_PRODUCT_VERSION("fullProductVersion", ValueType.STRING),
    LAST_TIME_RETRIEVED("lastTimeRetrieved", ValueType.LONG),
    DESCRIPTION("description", ValueType.STRING),
    METRIC_TYPE("metricType", ValueType.STRING),
    OPERATOR_INPUT_PORTS("operatorInputPorts", ValueType.ARRAY),
    OPERATOR_OUTPUT_PORTS("operatorOutputPorts", ValueType.ARRAY),
    PE_INPUT_PORTS("peInputPorts", ValueType.ARRAY),
    PE_OUTPUT_PORTS("peOutputPorts", ValueType.ARRAY),
    OUTPUT_PORT("outputPort", ValueType.STRING),
    OPERATOR_OUTPUT_PORT("operatorOutputPort", ValueType.STRING),
    INPUT_PORT("inputPort", ValueType.STRING),
    OPERATOR_INPUT_PORT("operatorInputPort", ValueType.STRING),
    SOURCE_OBJECT("source", ValueType.OBJECT),
    SOURCE_JOB(JOB.toApiStr(), ValueType.STRING, SOURCE_OBJECT),
    SOURCE_PE(PE.toApiStr(), ValueType.STRING, SOURCE_OBJECT),
    SOURCE_OPERATOR(OPERATOR.toApiStr(), ValueType.STRING, SOURCE_OBJECT),
    SOURCE_OUTPUT_PORT(OUTPUT_PORT.toApiStr(), ValueType.STRING, SOURCE_OBJECT),
    TARGET_OBJECT("target", ValueType.OBJECT),
    TARGET_JOB(JOB.toApiStr(), ValueType.STRING, TARGET_OBJECT),
    TARGET_PE(PE.toApiStr(), ValueType.STRING, TARGET_OBJECT),
    TARGET_OPERATOR(OPERATOR.toApiStr(), ValueType.STRING, TARGET_OBJECT),
    TARGET_INPUT_PORT(INPUT_PORT.toApiStr(), ValueType.STRING, TARGET_OBJECT),
    SUBMIT_PARMS("submitParameters", ValueType.ARRAY),
    TRANSPORT_TYPE("transportType", ValueType.STRING),
    REQUIRED_CONNECTIONS("requiredConnections", ValueType.STRING),
    OPTIONAL_CONNECTIONS("optionalConnections", ValueType.STRING),
    EDITION_NAME("editionName", ValueType.STRING),
    HOT_FIX("hotFix", ValueType.STRING),
    SERVICES("services", ValueType.VARIABLE),
    SERVICES_ARRAY(ValueType.ARRAY.toString() + "." + SERVICES.toApiStr(), ValueType.ARRAY),
    SERVICES_STRING(ValueType.STRING.toString() + "." + SERVICES.toApiStr(), ValueType.STRING),
    APPLICATION_NETWORK("applicationNetwork", ValueType.STRING),
    REQUIRED("required", ValueType.BOOLEAN),
    METRIC_KIND("metricKind", ValueType.STRING),
    OPERATOR_NAME("operatorName", ValueType.STRING),
    PORT_INDEX("portIndex", ValueType.LONG),
    PORTS("ports", ValueType.STRING),
    ATTRIBUTES("attributes", ValueType.ARRAY),
    BUFFER_CAPACITY("bufferCapacity", ValueType.LONG),
    BUFFER_CAPACITY_UNITS("bufferCapacityUnits", ValueType.STRING),
    MAXIMUM_TUPLE_RATE("maximumTupleRate", ValueType.LONG),
    ACTIVATE_OPTION("activateOption", ValueType.STRING),
    FILTER_DATA("filterData", ValueType.BOOLEAN),
    FILTER_ATTRIBUTE("filterAttribute", ValueType.STRING),
    FILTER_EXPRESSSION("filterExpression", ValueType.STRING),
    COLLECTION_TIME("collectionTime", ValueType.LONG),
    DATA("data", ValueType.OBJECT),
    TYPE("type", ValueType.STRING),
    CHANNEL("channel", ValueType.LONG),
    CHANNELS("channels", ValueType.ARRAY),
    CREATE_TIME("creationTime", ValueType.LONG),
    CREATE_USER("creationUser", ValueType.STRING),
    LOGICAL_NAME("logicalName", ValueType.STRING),
    LOGICAL_OPERATOR_NAME("logicalOperatorName", ValueType.STRING),
    DEFAULT_VALUE("defaultValue", ValueType.STRING),
    COMPOSITE("composite", ValueType.STRING),
    PARAMETER_KIND("parameterKind", ValueType.STRING),
    JOB_GROUP("jobGroup", ValueType.STRING),
    APPLICATION_SCOPE("applicationScope", ValueType.STRING),
    IMPORT_OBJECT("import", ValueType.OBJECT),
    IMPORT_SUBSCRIPTION("subscription", ValueType.STRING, IMPORT_OBJECT),
    IMPORT_FILTER("filter", ValueType.STRING, IMPORT_OBJECT),
    IMPORT_OPERATOR(OPERATOR.toApiStr(), ValueType.STRING, IMPORT_OBJECT),
    IMPORT_APPLICATION_SCOPE(APPLICATION_SCOPE.toApiStr(), ValueType.STRING, IMPORT_OBJECT),
    IMPORT_TYPE(TYPE.toApiStr(), ValueType.STRING, IMPORT_OBJECT),
    IMPORT_NAMES("names", ValueType.ARRAY, IMPORT_OBJECT),
    EXPORT_OBJECT("export", ValueType.OBJECT),
    EXPORT_PROPERTIES("properties", ValueType.ARRAY, EXPORT_OBJECT),
    EXPORT_TYPE(TYPE.toApiStr(), ValueType.STRING, EXPORT_OBJECT),
    EXPORT_NAME(NAME.toApiStr(), ValueType.STRING, EXPORT_OBJECT),
    EXPORT_FILTER_ALLOWED("filterAllowed", ValueType.BOOLEAN, EXPORT_OBJECT),
    EXPORT_OPERATOR(OPERATOR.toApiStr(), ValueType.STRING, EXPORT_OBJECT),
    VALUES("values", ValueType.ARRAY),
    ZOO_KEEPER_CONNECTION_STRING("zooKeeperConnectionString", ValueType.STRING),
    BUFFER_CAPACITY_SECONDS("bufferCapacitySeconds", ValueType.LONG),
    BUFFER_CAPACITY_TUPLES("bufferCapacityTuples", ValueType.LONG),
    VIEWS("views", ValueType.STRING),
    VIEW("view", ValueType.STRING),
    RESOURCE_TAGS("resourceTags", ValueType.ARRAY),
    RESERVED("reserved", ValueType.BOOLEAN),
    TAGS("tags", ValueType.ARRAY),
    RESTRICTED_TAGS("restrictedTags", ValueType.ARRAY),
    EXPORTED_STREAMS("exportedStreams", ValueType.STRING),
    IMPORTED_STREAMS("importedStreams", ValueType.STRING),
    OS_CAPABILITIES("osCapabilities", ValueType.ARRAY),
    PROPERTIES_DEFINITION("propertiesDefinition", ValueType.OBJECT),
    DEFINITION_FMT_PROPS("definitionFormatProperties", ValueType.BOOLEAN),
    CUSTOM_DEFINITION("customDefinition", ValueType.STRING),
    DISPLAY_NAME("displayName", ValueType.STRING),
    ALLOCATIONS("allocations", ValueType.STRING),
    RESOURCE_ALLOCATIONS("resourceAllocations", ValueType.STRING),
    RESOURCE_ALLOCATION("resourceAllocation", ValueType.STRING),
    SCHEDULER_STATUS("schedulerStatus", ValueType.STRING),
    APPLICATION_RESOURCE("applicationResource", ValueType.BOOLEAN),
    RESOURCESPEC_OBJECT("resourceSpecification", ValueType.OBJECT),
    RESOURCESPEC_ID(ID.toApiStr(), ValueType.STRING, RESOURCESPEC_OBJECT),
    RESOURCESPEC_TAGS(TAGS.toApiStr(), ValueType.ARRAY, RESOURCESPEC_OBJECT),
    RESOURCESPEC_COUNT("resourceCount", ValueType.INTEGER, RESOURCESPEC_OBJECT),
    RESOURCESPEC_EXCLUSIVE("exclusive", ValueType.BOOLEAN, RESOURCESPEC_OBJECT),
    RESOURCESPEC_RESTRICTED_TAGS(RESTRICTED_TAGS.toApiStr(), ValueType.ARRAY, RESOURCESPEC_OBJECT),
    LEADER("leader", ValueType.BOOLEAN),
    VERSION_OBJECT("activeVersion", ValueType.OBJECT),
    VERSION_ARCHITECTURE(ARCHITECTURE.toApiStr(), ValueType.STRING, VERSION_OBJECT),
    VERSION_BUILD_VERSION(BUILD_VERSION.toApiStr(), ValueType.STRING, VERSION_OBJECT),
    VERSION_EDITION_NAME(EDITION_NAME.toApiStr(), ValueType.STRING, VERSION_OBJECT),
    VERSION_MINIMUM_OS_VERSION(MINIMUM_OS_VERSION.toApiStr(), ValueType.STRING, VERSION_OBJECT),
    VERSION_OS_BASE_VERSION(OS_BASE_VERSION.toApiStr(), ValueType.STRING, VERSION_OBJECT),
    VERSION_OS_PATCH_VERSION(OS_PATCH_VERSION.toApiStr(), ValueType.STRING, VERSION_OBJECT),
    VERSION_PRODUCT_NAME(PRODUCT_NAME.toApiStr(), ValueType.STRING, VERSION_OBJECT),
    VERSION_PRODUCT_VERSION(PRODUCT_VERSION.toApiStr(), ValueType.STRING, VERSION_OBJECT),
    VERSION_FULL_PRODUCT_VERSION(FULL_PRODUCT_VERSION.toApiStr(), ValueType.STRING, VERSION_OBJECT),
    VERSION_HOT_FIX(HOT_FIX.toApiStr(), ValueType.STRING, VERSION_OBJECT),
    NODES("nodes", ValueType.STRING),
    ZKENSEMBLES("zooKeeperEnsembles", ValueType.STRING),
    ZKENSEMBLE("zooKeeperEnsemble", ValueType.STRING),
    ENSEMBLE("ensemble", ValueType.STRING),
    ZKNODES("zooKeeperNodes", ValueType.STRING),
    MODE("mode", ValueType.STRING),
    AVG_REQUEST_LATENCY("averageRequestLatency", ValueType.LONG),
    CLIENT_PORT("clientPort", ValueType.INTEGER),
    CONNECTION_COUNT("connectionCount", ValueType.LONG),
    LAST_ZK_TRANSACION_ID("lastZooKeeperTransactionId", ValueType.STRING),
    MAX_REQUEST_LATENCY("maximumRequestLatency", ValueType.LONG),
    MIN_REQUEST_LATENCY("minimumRequestLatency", ValueType.LONG),
    OUTSTANDING_REQUESTS("outstandingRequestsCount", ValueType.LONG),
    PACKETS_RECEIVED("packetsReceivedCount", ValueType.LONG),
    PACKETS_SENT("packetsSentCount", ValueType.LONG),
    VERSION("version", ValueType.STRING),
    ZNODE_COUNT("znodeCount", ValueType.LONG),
    ACTIVE_SERVICES("activeServices", ValueType.STRING),
    SERVICE_STATUS("serviceStatus", ValueType.ARRAY),
    ARTIFACTS("artifacts", ValueType.ARRAY),
    EXPIRATION_TIMEOUT("expirationTimeout", ValueType.INTEGER),
    LAST_ACTIVITY_TIME("lastActivityTime", ValueType.LONG),
    VERIFICATION_ID("verificationId", ValueType.STRING),
    TUPLE_ATTRIBUTES("tupleAttributes", ValueType.ARRAY),
    SCHEMA_URI("schemaUri", ValueType.STRING),
    SPL_TYPE("splType", ValueType.STRING),
    BOUNDED_SIZE("boundedSize", ValueType.LONG),
    POSSIBLE_VALUES("possibleValues", ValueType.ARRAY),
    ELEMENT_OBJECT("elementType", ValueType.OBJECT),
    KEY_OBJECT("keyType", ValueType.OBJECT),
    VALUE_OBJECT("valueType", ValueType.OBJECT),
    UNDERLYING_OBJECT("underlyingType", ValueType.OBJECT),
    ELEMENT_TYPE(TYPE.toApiStr(), ValueType.STRING, ELEMENT_OBJECT),
    ELEMENT_POSSIBLE_VALUES(POSSIBLE_VALUES.toApiStr(), ValueType.STRING, ELEMENT_OBJECT),
    ELEMENT_BOUNDED_SIZE(BOUNDED_SIZE.toApiStr(), ValueType.LONG, ELEMENT_OBJECT),
    ELEMENT_SCHEMA_URI(SCHEMA_URI.toApiStr(), ValueType.STRING, ELEMENT_OBJECT),
    ELEMENT_TUPLE_ATTRIBUTES(TUPLE_ATTRIBUTES.toApiStr(), ValueType.STRING, ELEMENT_OBJECT),
    ELEMENT_ELEMENT(
        ELEMENT_OBJECT.toApiStr(),
        ValueType.STRING,
        ELEMENT_OBJECT), // String used here because not supporting filtering beyond first level
    ELEMENT_KEY(
        KEY_OBJECT.toApiStr(),
        ValueType.STRING,
        ELEMENT_OBJECT), // String used here because not supporting filtering beyond first level
    ELEMENT_VALUE(
        VALUE_OBJECT.toApiStr(),
        ValueType.STRING,
        ELEMENT_OBJECT), // String used here because not supporting filtering beyond first level
    ELEMENT_UNDERLYING(
        UNDERLYING_OBJECT.toApiStr(),
        ValueType.STRING,
        ELEMENT_OBJECT), // String used here because not supporting filtering beyond first level
    KEY_TYPE(TYPE.toApiStr(), ValueType.STRING, KEY_OBJECT),
    KEY_POSSIBLE_VALUES(POSSIBLE_VALUES.toApiStr(), ValueType.STRING, KEY_OBJECT),
    KEY_BOUNDED_SIZE(BOUNDED_SIZE.toApiStr(), ValueType.LONG, KEY_OBJECT),
    KEY_SCHEMA_URI(SCHEMA_URI.toApiStr(), ValueType.STRING, KEY_OBJECT),
    KEY_TUPLE_ATTRIBUTES(TUPLE_ATTRIBUTES.toApiStr(), ValueType.STRING, KEY_OBJECT),
    KEY_ELEMENT(
        ELEMENT_OBJECT.toApiStr(),
        ValueType.STRING,
        KEY_OBJECT), // String used here because not supporting filtering beyond first level
    KEY_KEY(
        KEY_OBJECT.toApiStr(),
        ValueType.STRING,
        KEY_OBJECT), // String used here because not supporting filtering beyond first level
    KEY_VALUE(
        VALUE_OBJECT.toApiStr(),
        ValueType.STRING,
        KEY_OBJECT), // String used here because not supporting filtering beyond first level
    KEY_UNDERLYING(
        UNDERLYING_OBJECT.toApiStr(),
        ValueType.STRING,
        KEY_OBJECT), // String used here because not supporting filtering beyond first level
    VALUE_TYPE(TYPE.toApiStr(), ValueType.STRING, VALUE_OBJECT),
    VALUE_POSSIBLE_VALUES(POSSIBLE_VALUES.toApiStr(), ValueType.STRING, VALUE_OBJECT),
    VALUE_BOUNDED_SIZE(BOUNDED_SIZE.toApiStr(), ValueType.LONG, VALUE_OBJECT),
    VALUE_SCHEMA_URI(SCHEMA_URI.toApiStr(), ValueType.STRING, VALUE_OBJECT),
    VALUE_TUPLE_ATTRIBUTES(TUPLE_ATTRIBUTES.toApiStr(), ValueType.STRING, VALUE_OBJECT),
    VALUE_ELEMENT(
        ELEMENT_OBJECT.toApiStr(),
        ValueType.STRING,
        VALUE_OBJECT), // String used here because not supporting filtering beyond first level
    VALUE_KEY(
        KEY_OBJECT.toApiStr(),
        ValueType.STRING,
        VALUE_OBJECT), // String used here because not supporting filtering beyond first level
    VALUE_VALUE(
        VALUE_OBJECT.toApiStr(),
        ValueType.STRING,
        VALUE_OBJECT), // String used here because not supporting filtering beyond first level
    VALUE_UNDERLYING(
        UNDERLYING_OBJECT.toApiStr(),
        ValueType.STRING,
        VALUE_OBJECT), // String used here because not supporting filtering beyond first level
    UNDERLYING_TYPE(TYPE.toApiStr(), ValueType.STRING, UNDERLYING_OBJECT),
    UNDERLYING_POSSIBLE_VALUES(POSSIBLE_VALUES.toApiStr(), ValueType.STRING, UNDERLYING_OBJECT),
    UNDERLYING_BOUNDED_SIZE(BOUNDED_SIZE.toApiStr(), ValueType.LONG, UNDERLYING_OBJECT),
    UNDERLYING_SCHEMA_URI(SCHEMA_URI.toApiStr(), ValueType.STRING, UNDERLYING_OBJECT),
    UNDERLYING_TUPLE_ATTRIBUTES(TUPLE_ATTRIBUTES.toApiStr(), ValueType.STRING, UNDERLYING_OBJECT),
    UNDERLYING_ELEMENT(
        ELEMENT_OBJECT.toApiStr(),
        ValueType.STRING,
        UNDERLYING_OBJECT), // String used here because not supporting filtering beyond first level
    UNDERLYING_KEY(
        KEY_OBJECT.toApiStr(),
        ValueType.STRING,
        UNDERLYING_OBJECT), // String used here because not supporting filtering beyond first level
    UNDERLYING_VALUE(
        VALUE_OBJECT.toApiStr(),
        ValueType.STRING,
        UNDERLYING_OBJECT), // String used here because not supporting filtering beyond first level
    // note: no direct nesting of optional types, so no UNDERLYING_UNDERLYING
    APPLICATION_TRACE("applicationTrace", ValueType.STRING),
    APPLICATION_LOG_TRACE("applicationLogTrace", ValueType.STRING),
    CONSOLE_LOG("consoleLog", ValueType.STRING),
    PRODUCT_LOG("productLog", ValueType.STRING),
    SNAPSHOT("snapshot", ValueType.STRING),
    METRICS_SNAPSHOT("metricsSnapshot", ValueType.STRING),
    JOBS_SNAPSHOT("jobsSnapshot", ValueType.STRING),
    JOBS_METRICS_SNAPSHOT("jobsMetricsSnapshot", ValueType.STRING),
    STARTED("started", ValueType.BOOLEAN),
    CONNECTION_URL("connectionUrl", ValueType.STRING),
    CONNECTION_URL_EXTERNAL("connectionUrlExternal", ValueType.STRING),
    ASPECT("aspect", ValueType.STRING),
    JOB_RESOURCE_SHARING("jobResourceSharing", ValueType.STRING),
    FUSION_SCHEME("fusionScheme", ValueType.STRING),
    THREADING_MODEL("threadingModel", ValueType.STRING),
    PROPOSED_OPERATORS_PER_RESOURCE("proposedOperatorsPerResource", ValueType.INTEGER),
    DYNAMIC_THREADING_THREAD_COUNT("dynamicThreadingThreadCount", ValueType.INTEGER),
    DYNAMIC_THREADING_ELASTIC("dynamicThreadingElastic", ValueType.BOOLEAN),
    TOPOLOGY_UPDATING("topologyUpdating", ValueType.BOOLEAN),
    GENERATION_ID("generationId", ValueType.LONG),

    UNKNOWN("", ValueType.UNKNOWN);

    private String _apiStr;
    private ValueType _valueType;
    private PropertyType _parentObject;

    private static Map<String, PropertyType> _propTypeMap;

    static {
      _propTypeMap = new Hashtable<String, PropertyType>();
      PropertyType[] types = values();
      for (int i = 0; i < types.length; ++i) {
        _propTypeMap.put(types[i].toApiStr(), types[i]);
      }
    }

    PropertyType(String apiStr, ValueType valueType) {
      this(apiStr, valueType, null);
    }

    PropertyType(String apiStr, ValueType valueType, PropertyType parentObject) {
      _valueType = valueType;
      _parentObject = parentObject;
      if (parentObject == null) {
        _apiStr = apiStr;
      } else {
        _apiStr = parentObject.toApiStr() + "." + apiStr;
      }
    }

    public String toApiStr() {
      return _apiStr;
    }

    public static PropertyType getPropType(String apiStr) {
      PropertyType mappedType = _propTypeMap.get(apiStr);
      if (mappedType == null) {
        mappedType = PropertyType.UNKNOWN;
      }
      return mappedType;
    }

    private PropertyType getComparableProp(ResourceType resourceType) {
      if (_valueType.equals(ValueType.VARIABLE)) {
        List<PropertyType> supportedProps = resourceType.getSupportedProps();
        List<ValueType> valueTypes = Arrays.asList(ValueType.values());
        Iterator<ValueType> it = valueTypes.iterator();
        while (it.hasNext()) {
          ValueType valueType = it.next();
          String valueTypeID = valueType.toString() + "." + toApiStr();
          PropertyType propType = PropertyType.getPropType(valueTypeID);
          if (supportedProps.contains(propType)) {
            return propType;
          }
        }
      }
      return this;
    }

    public ColumnType getCompareType(ResourceType resourceType) {
      return getComparableProp(resourceType)._valueType.getCompareType();
    }

    public boolean isComparable(ResourceType resourceType) {
      return !(getCompareType(resourceType).equals(ColumnType.NONCOMPARABLE));
    }

    public String getComparableColumnName(ResourceType resourceType) {
      return getComparableProp(resourceType).toApiStr();
    }

    public Object getComparableValue(String propValue, ResourceType resourceType) {
      PropertyType comparableProp = getComparableProp(resourceType);

      if (comparableProp._valueType.equals(ValueType.BIGINTEGER)) {
        return new BigInteger(propValue);
      }
      if (comparableProp._valueType.equals(ValueType.LONG)) {
        return Long.valueOf(propValue);
      }
      if (comparableProp._valueType.equals(ValueType.INTEGER)) {
        return Integer.valueOf(propValue);
      }
      if (comparableProp._valueType.equals(ValueType.BOOLEAN)) {
        return Boolean.valueOf(propValue);
      }

      return propValue;
    }

    public boolean isObjectMember() {
      return _parentObject != null;
    }

    public String getMemberApiStr() {
      if (isObjectMember()) {
        return _apiStr.substring(_parentObject.toApiStr().length() + 1);
      } else {
        return _apiStr;
      }
    }

    public PropertyType getParentObject() {
      return _parentObject;
    }

    public boolean isVariableType() {
      return _valueType.equals(ValueType.VARIABLE);
    }

    public boolean isObjectType() {
      return _valueType.equals(ValueType.OBJECT);
    }

    public boolean isInternalOnly() {
      if (isObjectMember()) {
        return true;
      }

      List<ValueType> valueTypes = Arrays.asList(ValueType.values());
      Iterator<ValueType> it = valueTypes.iterator();
      while (it.hasNext()) {
        ValueType valueType = it.next();
        if (toApiStr().startsWith(valueType.toString() + ".")) {
          return true;
        }
      }

      return false;
    }

    public boolean valuesEqual(
        Object propValue, String strValue, ResourceType resourceType, Collator collator) {
      if (propValue == null) {
        Trace.logDebug("STREAMSAPI propValue is null");
        return false;
      }

      Trace.logDebug("STREAMSAPI value type " + _valueType);
      Trace.logDebug("STREAMSAPI prop type " + propValue.getClass().getName());

      switch (_valueType) {
        case INTEGER:
          Integer int1 = (Integer) propValue;
          Integer int2 = Integer.valueOf(strValue);
          return int1.compareTo(int2) == 0;
        case LONG:
          Long long1 = (Long) propValue;
          Long long2 = Long.valueOf(strValue);
          return long1.compareTo(long2) == 0;
        case BOOLEAN:
          Boolean bool1 = (Boolean) propValue;
          Boolean bool2 = Boolean.valueOf(strValue);
          return bool1.compareTo(bool2) == 0;
        case BIGINTEGER:
          BigInteger bi1 = (BigInteger) propValue;
          BigInteger bi2 = new BigInteger(strValue);
          return bi1.compareTo(bi2) == 0;
        case VARIABLE:
          PropertyType comparableProp = getComparableProp(resourceType);
          Trace.logDebug("STREAMSAPI comparableProp " + comparableProp.toApiStr());
          return comparableProp.valuesEqual(propValue, strValue, resourceType, collator);
        case OBJECT:
        case ARRAY:
          return false;
        case STRING:
        default:
          return collator.compare((String) propValue, strValue) == 0;
      }
    }
  }

  public static enum ValueType {
    STRING(ColumnType.STRING),
    INTEGER(ColumnType.COMPARABLE),
    LONG(ColumnType.COMPARABLE),
    BIGINTEGER(ColumnType.COMPARABLE),
    BOOLEAN(ColumnType.COMPARABLE),
    OBJECT(ColumnType.NONCOMPARABLE),
    ARRAY(ColumnType.NONCOMPARABLE),
    VARIABLE(ColumnType.NONCOMPARABLE),
    UNKNOWN(ColumnType.NONCOMPARABLE);

    private ColumnType _compareType;

    ValueType(ColumnType compareType) {
      _compareType = compareType;
    }

    public ColumnType getCompareType() {
      return _compareType;
    }
  }

  protected static final String LIST = "List";

  public static enum ResourceType {
    // Constructor parameters:
    // 1. API string - this is part of the API definition and should not be changed
    // 2. Default sort column
    // 3. ID type
    // 4. Supported properties - these are used for row and column filtering
    INSTALLATION(
        "installation",
        null,
        null,
        new PropertyType[] {
          PropertyType.RESOURCE_TYPE,
          PropertyType.ARCHITECTURE,
          PropertyType.BUILD_VERSION,
          PropertyType.INSTALL_PATH,
          PropertyType.MINIMUM_OS_VERSION,
          PropertyType.OS_BASE_VERSION,
          PropertyType.OS_PATCH_VERSION,
          PropertyType.OWNER, // NA for resource installation
          PropertyType.PRODUCT_NAME,
          PropertyType.PRODUCT_VERSION,
          PropertyType.FULL_PRODUCT_VERSION,
          PropertyType.EDITION_NAME,
          PropertyType.HOT_FIX
        }),
    DOMAIN(
        "domain",
        PropertyType.ID_STRING,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.ID,
          PropertyType.START_TIME,
          PropertyType.CREATE_TIME,
          PropertyType.CREATE_USER,
          PropertyType.INSTANCES,
          PropertyType.SELF,
          PropertyType.STATUS,
          PropertyType.HOSTS,
          PropertyType.RESOURCES,
          PropertyType.RESOURCE_TAGS,
          PropertyType.ACTIVE_SERVICES,
          PropertyType.RESOURCE_ALLOCATIONS,
          PropertyType.ZOO_KEEPER_CONNECTION_STRING,
          PropertyType.ZKENSEMBLE,
          PropertyType.VERSION_OBJECT,
          PropertyType.PRODUCT_LOG,
          // Internal use only properties
          PropertyType.VERSION_ARCHITECTURE,
          PropertyType.VERSION_BUILD_VERSION,
          PropertyType.VERSION_EDITION_NAME,
          PropertyType.VERSION_FULL_PRODUCT_VERSION,
          PropertyType.VERSION_HOT_FIX,
          PropertyType.VERSION_MINIMUM_OS_VERSION,
          PropertyType.VERSION_OS_BASE_VERSION,
          PropertyType.VERSION_OS_PATCH_VERSION,
          PropertyType.VERSION_PRODUCT_NAME,
          PropertyType.VERSION_PRODUCT_VERSION,
        }),
    INSTANCE(
        "instance",
        PropertyType.ID_STRING,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.ID,
          PropertyType.OWNER,
          PropertyType.START_TIME,
          PropertyType.STATUS,
          PropertyType.HEALTH,
          PropertyType.STARTED_BY,
          PropertyType.APPLICATION_NETWORK,
          PropertyType.SELF,
          PropertyType.HOSTS,
          PropertyType.JOBS,
          PropertyType.PES,
          PropertyType.PE_CONNECTIONS,
          PropertyType.OPERATORS,
          PropertyType.OPERATOR_CONNECTIONS,
          PropertyType.CONFIGURED_VIEWS, // deprecated
          PropertyType.ACTIVE_VIEWS, // deprecated
          PropertyType.DOMAIN,
          PropertyType.CREATE_TIME,
          PropertyType.CREATE_USER,
          PropertyType.VIEWS,
          PropertyType.EXPORTED_STREAMS,
          PropertyType.IMPORTED_STREAMS,
          PropertyType.ACTIVE_SERVICES,
          PropertyType.RESOURCE_ALLOCATIONS,
          PropertyType.RESTRICTED_TAGS,
          PropertyType.JOBS_SNAPSHOT,
          PropertyType.JOBS_METRICS_SNAPSHOT,
          // Internal use only properties
          PropertyType.ID_STRING,
          PropertyType.VERSION_OBJECT,
          // Internal use only properties
          PropertyType.VERSION_ARCHITECTURE,
          PropertyType.VERSION_BUILD_VERSION,
          PropertyType.VERSION_EDITION_NAME,
          PropertyType.VERSION_FULL_PRODUCT_VERSION,
          PropertyType.VERSION_HOT_FIX,
          PropertyType.VERSION_MINIMUM_OS_VERSION,
          PropertyType.VERSION_OS_BASE_VERSION,
          PropertyType.VERSION_OS_PATCH_VERSION,
          PropertyType.VERSION_PRODUCT_NAME,
          PropertyType.VERSION_PRODUCT_VERSION,
        }),
    RESOURCE(
        "resource",
        PropertyType.ID,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.ID,
          PropertyType.DISPLAY_NAME,
          PropertyType.IP_ADDRESS,
          PropertyType.STATUS,
          PropertyType.TYPE,
          PropertyType.TAGS,
          PropertyType.RESTRICTED_TAGS,
          PropertyType.SELF,
          PropertyType.DOMAIN,
          PropertyType.METRICS,
          PropertyType.ACTIVE_SERVICES,
          PropertyType.ALLOCATIONS,
          PropertyType.INSTALLATIONS,
          PropertyType.SERVICE_STATUS,
          // Internal use only properties
          PropertyType.ID_STRING,
        }),
    RESOURCE_ALLOCATION(
        "resourceAllocation",
        PropertyType.SELF,
        ValueType.STRING,
        new PropertyType[] { // sorting by self so works for domain resource allocations list paging
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.STATUS,
          PropertyType.SCHEDULER_STATUS,
          PropertyType.APPLICATION_RESOURCE,
          PropertyType.SELF,
          PropertyType.DOMAIN,
          PropertyType.INSTANCE,
          PropertyType.RESOURCE,
          PropertyType.ACTIVE_SERVICES,
          PropertyType.JOBS,
          PropertyType.PES,
          PropertyType.RESOURCESPEC_OBJECT,
          PropertyType.SERVICE_STATUS,
          // Internal use only properties
          PropertyType.RESOURCESPEC_COUNT,
          PropertyType.RESOURCESPEC_EXCLUSIVE,
          PropertyType.RESOURCESPEC_ID,
          PropertyType.RESOURCESPEC_TAGS,
        }),
    ACTIVE_SERVICE(
        "activeService",
        PropertyType.TYPE,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.PID,
          PropertyType.TYPE,
          PropertyType.LEADER,
          PropertyType.STATUS,
          PropertyType.START_TIME,
          PropertyType.SELF,
          PropertyType.DOMAIN,
          PropertyType.RESOURCE, // NA for instance service
          PropertyType.RESOURCE_ALLOCATION, // NA for domain service
          PropertyType.INSTANCE, // NA for domain service
          PropertyType.CONNECTION_URL, // NA for instance service
          PropertyType.CONNECTION_URL_EXTERNAL, // NA for instance service
        }),
    HOST(
        "host",
        PropertyType.NAME,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.NAME,
          PropertyType.IP_ADDRESS,
          PropertyType.STATUS,
          PropertyType.PROCESSOR_COUNT,
          PropertyType.SCHEDULER_STATE, // NA for domain host
          PropertyType.SELF,
          PropertyType.INSTANCE, // NA for domain host
          PropertyType.JOBS, // NA for domain host
          PropertyType.PES, // NA for domain host
          PropertyType.METRICS,
          PropertyType.SERVICES,
          PropertyType.TAGS,
          PropertyType.RESTRICTED_TAGS,
          PropertyType.DOMAIN,
          // Internal use only properties
          PropertyType.SERVICES_ARRAY
        }),
    JOB(
        "job",
        PropertyType.ID_BIGINT,
        ValueType.BIGINTEGER,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.ID,
          PropertyType.NAME,
          PropertyType.SUBMIT_TIME,
          PropertyType.STARTED_BY,
          PropertyType.STATUS,
          PropertyType.HEALTH,
          PropertyType.APPLICATION_VERSION,
          PropertyType.APPLICATION_PATH,
          PropertyType.DATA_PATH,
          PropertyType.OUTPUT_PATH,
          PropertyType.CHECKPOINT_PATH,
          PropertyType.ADL_FILE,
          PropertyType.SELF,
          PropertyType.INSTANCE,
          PropertyType.HOSTS,
          PropertyType.RESOURCE_ALLOCATIONS,
          PropertyType.PES,
          PropertyType.OPERATORS,
          PropertyType.PE_CONNECTIONS,
          PropertyType.OPERATOR_CONNECTIONS,
          PropertyType.SUBMIT_PARMS,
          PropertyType.ACTIVE_VIEWS, // deprecated
          PropertyType.APPLICATION_NAME,
          PropertyType.APPLICATION_SCOPE,
          PropertyType.JOB_GROUP,
          PropertyType.DOMAIN,
          PropertyType.VIEWS,
          PropertyType.APPLICATION_LOG_TRACE,
          PropertyType.PRODUCT_LOG,
          PropertyType.JOB_RESOURCE_SHARING,
          PropertyType.FUSION_SCHEME,
          PropertyType.THREADING_MODEL,
          PropertyType.PROPOSED_OPERATORS_PER_RESOURCE,
          PropertyType.DYNAMIC_THREADING_THREAD_COUNT,
          PropertyType.DYNAMIC_THREADING_ELASTIC,
          PropertyType.PRODUCT_VERSION,
          PropertyType.TOPOLOGY_UPDATING,
          PropertyType.GENERATION_ID,
          PropertyType.SNAPSHOT,
          PropertyType.METRICS_SNAPSHOT,
          // Internal use only properties
          PropertyType.ID_BIGINT
        }),
    PE(
        "pe",
        PropertyType.ID_BIGINT,
        ValueType.BIGINTEGER,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.ID,
          PropertyType.INDEX_WITHIN_JOB,
          PropertyType.STATUS,
          PropertyType.STATUS_REASON,
          PropertyType.HEALTH,
          PropertyType.PID,
          PropertyType.TRACING_LEVEL,
          PropertyType.PENDING_TRACING_LEVEL,
          PropertyType.RELOCATABLE,
          PropertyType.RESTARTABLE,
          PropertyType.LAUNCH_COUNT,
          PropertyType.REQUIRED_CONNECTIONS,
          PropertyType.OPTIONAL_CONNECTIONS,
          PropertyType.RESTART_PRIORITY,
          PropertyType.CURRENT_WORKING_PATH,
          PropertyType.SELF,
          PropertyType.INSTANCE,
          PropertyType.HOST,
          PropertyType.RESOURCE_ALLOCATION,
          PropertyType.JOB,
          PropertyType.OPERATORS,
          PropertyType.INPUT_PORTS,
          PropertyType.OUTPUT_PORTS,
          PropertyType.CONNECTIONS,
          PropertyType.METRICS,
          PropertyType.DOMAIN,
          PropertyType.RESOURCE_TAGS,
          PropertyType.OS_CAPABILITIES,
          PropertyType.APPLICATION_TRACE,
          PropertyType.CONSOLE_LOG,
          PropertyType.PRODUCT_LOG,
          PropertyType.SNAPSHOT,
          PropertyType.METRICS_SNAPSHOT,
          // Internal use only properties
          PropertyType.ID_BIGINT,
          PropertyType.PID_BIGINT
        }),
    PE_INPUT_PORT(
        "peInputPort",
        PropertyType.INDEX_WITHIN_PE,
        ValueType.INTEGER,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.INDEX_WITHIN_PE,
          PropertyType.ID,
          PropertyType.TRANSPORT_TYPE,
          PropertyType.SELF,
          PropertyType.JOB,
          PropertyType.PE,
          PropertyType.CONNECTIONS,
          PropertyType.METRICS
        }),
    PE_OUTPUT_PORT(
        "peOutputPort",
        PropertyType.INDEX_WITHIN_PE,
        ValueType.INTEGER,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.INDEX_WITHIN_PE,
          PropertyType.ID,
          PropertyType.TRANSPORT_TYPE,
          PropertyType.SELF,
          PropertyType.JOB,
          PropertyType.PE,
          PropertyType.CONNECTIONS,
          PropertyType.METRICS
        }),
    PE_CONNECTION(
        "peConnection",
        null,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.STATUS,
          PropertyType.REQUIRED,
          PropertyType.SOURCE_OBJECT,
          PropertyType.TARGET_OBJECT,
          PropertyType.SELF,
          PropertyType.METRICS,
          PropertyType.ID,
          // Internal use only properties
          PropertyType.SOURCE_JOB,
          PropertyType.SOURCE_PE,
          PropertyType.SOURCE_OUTPUT_PORT,
          PropertyType.TARGET_JOB,
          PropertyType.TARGET_PE,
          PropertyType.TARGET_INPUT_PORT
        }),
    OPERATOR(
        "operator",
        PropertyType.NAME,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.NAME,
          PropertyType.INDEX_WITHIN_JOB,
          PropertyType.OPERATOR_KIND,
          PropertyType.CHANNEL,
          PropertyType.CHANNELS,
          PropertyType.SELF,
          PropertyType.INSTANCE,
          PropertyType.HOST,
          PropertyType.RESOURCE_ALLOCATION,
          PropertyType.JOB,
          PropertyType.PE,
          PropertyType.INPUT_PORTS,
          PropertyType.OUTPUT_PORTS,
          PropertyType.CONNECTIONS,
          PropertyType.METRICS,
          PropertyType.LOGICAL_NAME,
          PropertyType.DOMAIN,
          PropertyType.APPLICATION_TRACE,
          PropertyType.PRODUCT_LOG,
        }),
    OPERATOR_INPUT_PORT(
        "operatorInputPort",
        PropertyType.INDEX_WITHIN_OPERATOR,
        ValueType.INTEGER,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.INDEX_WITHIN_OPERATOR,
          PropertyType.NAME,
          PropertyType.SELF,
          PropertyType.JOB,
          PropertyType.PE,
          PropertyType.OPERATOR,
          PropertyType.CONNECTIONS,
          PropertyType.METRICS,
          PropertyType.PE_INPUT_PORTS,
          PropertyType.IMPORT_OBJECT,
          // Internal use only properties
          PropertyType.IMPORT_TYPE,
          PropertyType.IMPORT_NAMES,
          PropertyType.IMPORT_SUBSCRIPTION,
          PropertyType.IMPORT_FILTER,
          PropertyType.IMPORT_OPERATOR,
          PropertyType.IMPORT_APPLICATION_SCOPE,
        }),
    OPERATOR_OUTPUT_PORT(
        "operatorOutputPort",
        PropertyType.INDEX_WITHIN_OPERATOR,
        ValueType.INTEGER,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.INDEX_WITHIN_OPERATOR,
          PropertyType.NAME,
          PropertyType.STREAM_NAME,
          PropertyType.STREAM_VIEWABLE,
          PropertyType.SELF,
          PropertyType.JOB,
          PropertyType.PE,
          PropertyType.OPERATOR,
          PropertyType.CONNECTIONS,
          PropertyType.METRICS,
          PropertyType.PE_OUTPUT_PORTS,
          PropertyType.EXPORT_OBJECT,
          PropertyType.TUPLE_ATTRIBUTES,
          // Internal use only properties
          PropertyType.EXPORT_TYPE,
          PropertyType.EXPORT_NAME,
          PropertyType.EXPORT_PROPERTIES,
          PropertyType.EXPORT_FILTER_ALLOWED,
          PropertyType.EXPORT_OPERATOR,
        }),
    TUPLE_ATTRIBUTE(
        "tupleAttribute",
        null,
        null,
        new PropertyType[] {
          PropertyType.NAME,
          PropertyType.TYPE,
          PropertyType.SPL_TYPE,
          PropertyType.BOUNDED_SIZE,
          PropertyType.POSSIBLE_VALUES,
          PropertyType.SCHEMA_URI,
          PropertyType.TUPLE_ATTRIBUTES,
          PropertyType.ELEMENT_OBJECT,
          PropertyType.KEY_OBJECT,
          PropertyType.VALUE_OBJECT,
          PropertyType.UNDERLYING_OBJECT,
          // Internal use only properties
          PropertyType.ELEMENT_TYPE,
          PropertyType.ELEMENT_POSSIBLE_VALUES,
          PropertyType.ELEMENT_BOUNDED_SIZE,
          PropertyType.ELEMENT_SCHEMA_URI,
          PropertyType.ELEMENT_TUPLE_ATTRIBUTES,
          PropertyType.ELEMENT_ELEMENT,
          PropertyType.ELEMENT_KEY,
          PropertyType.ELEMENT_VALUE,
          PropertyType.ELEMENT_UNDERLYING,
          PropertyType.KEY_TYPE,
          PropertyType.KEY_POSSIBLE_VALUES,
          PropertyType.KEY_BOUNDED_SIZE,
          PropertyType.KEY_SCHEMA_URI,
          PropertyType.KEY_TUPLE_ATTRIBUTES,
          PropertyType.KEY_ELEMENT,
          PropertyType.KEY_KEY,
          PropertyType.KEY_VALUE,
          PropertyType.KEY_UNDERLYING,
          PropertyType.VALUE_TYPE,
          PropertyType.VALUE_POSSIBLE_VALUES,
          PropertyType.VALUE_BOUNDED_SIZE,
          PropertyType.VALUE_SCHEMA_URI,
          PropertyType.VALUE_TUPLE_ATTRIBUTES,
          PropertyType.VALUE_ELEMENT,
          PropertyType.VALUE_KEY,
          PropertyType.VALUE_VALUE,
          PropertyType.VALUE_UNDERLYING,
          PropertyType.UNDERLYING_TYPE,
          PropertyType.UNDERLYING_POSSIBLE_VALUES,
          PropertyType.UNDERLYING_BOUNDED_SIZE,
          PropertyType.UNDERLYING_SCHEMA_URI,
          PropertyType.UNDERLYING_TUPLE_ATTRIBUTES,
          PropertyType.UNDERLYING_ELEMENT,
          PropertyType.UNDERLYING_KEY,
          PropertyType.UNDERLYING_VALUE,
        }),
    OPERATOR_CONNECTION(
        "operatorConnection",
        null,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.SOURCE_OBJECT,
          PropertyType.TARGET_OBJECT,
          PropertyType.SELF,
          PropertyType.REQUIRED,
          PropertyType.PE_CONNECTION,
          PropertyType.ID,
          // Internal use only properties
          PropertyType.SOURCE_JOB,
          PropertyType.SOURCE_PE,
          PropertyType.SOURCE_OPERATOR,
          PropertyType.SOURCE_OUTPUT_PORT,
          PropertyType.TARGET_JOB,
          PropertyType.TARGET_PE,
          PropertyType.TARGET_OPERATOR,
          PropertyType.TARGET_INPUT_PORT
        }),
    METRIC(
        "metric",
        PropertyType.NAME,
        null,
        new PropertyType[] {
          PropertyType.RESOURCE_TYPE,
          PropertyType.NAME,
          PropertyType.METRIC_TYPE,
          PropertyType.VALUE,
          PropertyType.LAST_TIME_RETRIEVED,
          PropertyType.DESCRIPTION,
          PropertyType.METRIC_KIND,
        }),
    // deprecated
    CONFIGURED_VIEW(
        "configuredView",
        PropertyType.NAME,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.ID,
          PropertyType.NAME,
          PropertyType.DESCRIPTION,
          PropertyType.ADL_FILE, // RESTDESIGN - can I get this (might be able to get from the job)
          PropertyType
              .APPLICATION_NAME, // RESTDESIGN - can I get this (should be able to get from the job)
          PropertyType.OPERATOR_NAME,
          PropertyType.STREAM_NAME, // RESTDESIGN - can I get this
          PropertyType.PORT_INDEX,
          PropertyType.ATTRIBUTES,
          PropertyType.BUFFER_CAPACITY,
          PropertyType.BUFFER_CAPACITY_UNITS,
          PropertyType.MAXIMUM_TUPLE_RATE,
          PropertyType.ACTIVATE_OPTION,
          PropertyType.FILTER_DATA,
          PropertyType.FILTER_ATTRIBUTE,
          PropertyType.FILTER_EXPRESSSION,
          PropertyType.SELF,
          PropertyType.ACTIVE_VIEWS,
          // Internal use only properties
          PropertyType.ID_STRING,
        }),
    // deprecated
    ACTIVE_VIEW(
        "activeView",
        null,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.ID,
          PropertyType.SELF,
          PropertyType.CONFIGURED_VIEW,
          PropertyType.JOB,
          PropertyType.PORTS,
          PropertyType.VIEW_ITEMS,
          // Internal use only properties
          PropertyType.ID_STRING,
        }),
    // Note: setting default sort column to null, but will receive data sorted by collection time
    // By setting the default sort column to null, we will not try to resort the data
    VIEW_ITEM(
        "viewItem",
        null,
        null,
        new PropertyType[] {
          PropertyType.RESOURCE_TYPE,
          PropertyType.COLLECTION_TIME,
          PropertyType.CHANNEL,
          PropertyType.CHANNELS,
          PropertyType.DATA
        }),
    VIEW(
        "view",
        PropertyType.ID,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.ID,
          PropertyType.SELF,
          PropertyType.DOMAIN,
          PropertyType.INSTANCE,
          PropertyType.JOB,
          PropertyType.PORTS,
          PropertyType.VIEW_ITEMS,
          PropertyType.NAME,
          PropertyType.DESCRIPTION,
          PropertyType.ATTRIBUTES,
          PropertyType.BUFFER_CAPACITY_SECONDS,
          PropertyType.BUFFER_CAPACITY_TUPLES,
          PropertyType.BUFFER_CAPACITY_UNITS,
          PropertyType.MAXIMUM_TUPLE_RATE,
          PropertyType.ACTIVATE_OPTION,
          PropertyType.FILTER_DATA,
          PropertyType.FILTER_ATTRIBUTE,
          PropertyType.FILTER_EXPRESSSION,
          PropertyType.LOGICAL_OPERATOR_NAME,
          PropertyType.STARTED,
          // Internal use only properties
          PropertyType.ID_STRING,
        }),
    EXPORTED_STREAM(
        "exportedStream",
        null,
        null,
        new PropertyType[] {
          PropertyType.RESOURCE_TYPE,
          PropertyType.OPERATOR_OUTPUT_PORT,
          PropertyType.OPERATOR_CONNECTIONS
        }),
    IMPORTED_STREAM(
        "importedStream",
        null,
        null,
        new PropertyType[] {
          PropertyType.RESOURCE_TYPE,
          PropertyType.OPERATOR_INPUT_PORT,
          PropertyType.OPERATOR_CONNECTIONS
        }),
    ZKENSEMBLE(
        "zooKeeperEnsemble",
        PropertyType.NAME,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.NAME,
          PropertyType.STATUS,
          PropertyType.NODES,
          PropertyType.SELF,
          PropertyType.DOMAIN,
        }),
    ZKNODE(
        "zooKeeperNode",
        PropertyType.NAME,
        ValueType.STRING,
        new PropertyType[] {
          PropertyType.REST_ID,
          PropertyType.RESOURCE_TYPE,
          PropertyType.NAME,
          PropertyType.STATUS,
          PropertyType.MODE,
          PropertyType.AVG_REQUEST_LATENCY,
          PropertyType.CLIENT_PORT,
          PropertyType.HOST,
          PropertyType.CONNECTION_COUNT,
          PropertyType.LAST_ZK_TRANSACION_ID,
          PropertyType.MAX_REQUEST_LATENCY,
          PropertyType.MIN_REQUEST_LATENCY,
          PropertyType.OUTSTANDING_REQUESTS,
          PropertyType.PACKETS_RECEIVED,
          PropertyType.PACKETS_SENT,
          PropertyType.VERSION,
          PropertyType.ZNODE_COUNT,
          PropertyType.ENSEMBLE,
          PropertyType.SELF,
          PropertyType.DOMAIN,
        }),

    INSTALLATIONS(INSTALLATION.toApiStr() + LIST),
    DOMAINS(DOMAIN.toApiStr() + LIST),
    INSTANCES(INSTANCE.toApiStr() + LIST),
    RESOURCES(RESOURCE.toApiStr() + LIST),
    RESOURCE_ALLOCATIONS(RESOURCE_ALLOCATION.toApiStr() + LIST),
    ACTIVE_SERVICES(ACTIVE_SERVICE.toApiStr() + LIST),
    HOSTS(HOST.toApiStr() + LIST),
    JOBS(JOB.toApiStr() + LIST),
    PES(PE.toApiStr() + LIST),
    PE_INPUT_PORTS(PE_INPUT_PORT.toApiStr() + LIST),
    PE_OUTPUT_PORTS(PE_OUTPUT_PORT.toApiStr() + LIST),
    PE_CONNECTIONS(PE_CONNECTION.toApiStr() + LIST),
    OPERATORS(OPERATOR.toApiStr() + LIST),
    OPERATOR_INPUT_PORTS(OPERATOR_INPUT_PORT.toApiStr() + LIST),
    OPERATOR_OUTPUT_PORTS(OPERATOR_OUTPUT_PORT.toApiStr() + LIST),
    OPERATOR_CONNECTIONS(OPERATOR_CONNECTION.toApiStr() + LIST),
    TUPLE_ATTRIBUTES(TUPLE_ATTRIBUTE.toApiStr() + LIST),
    METRICS(METRIC.toApiStr() + LIST),
    CONFIGURED_VIEWS(CONFIGURED_VIEW.toApiStr() + LIST),
    ACTIVE_VIEWS(ACTIVE_VIEW.toApiStr() + LIST),
    VIEWS(VIEW.toApiStr() + LIST),
    VIEW_ITEMS(VIEW_ITEM.toApiStr() + LIST),
    EXPORTED_STREAMS(EXPORTED_STREAM.toApiStr() + LIST),
    IMPORTED_STREAMS(IMPORTED_STREAM.toApiStr() + LIST),
    ZKENSEMBLES(ZKENSEMBLE.toApiStr() + LIST),
    ZKNODES(ZKNODE.toApiStr() + LIST);

    private String _apiStr;
    private PropertyType _defaultSortColumn;
    private List<PropertyType> _supportedProps;
    private ValueType _idType;

    ResourceType(String apiStr) {
      this(apiStr, null, null, new PropertyType[] {});
    }

    ResourceType(
        String apiStr,
        PropertyType defaultSortColumn,
        ValueType idType,
        PropertyType[] supportedProps) {
      _apiStr = apiStr;
      _defaultSortColumn = defaultSortColumn;
      _supportedProps = new ArrayList<PropertyType>(Arrays.asList(supportedProps));
      _idType = (idType == null) ? ValueType.STRING : idType;
    }

    public String toApiStr() {
      return _apiStr;
    }

    public boolean isList() {
      return _apiStr.endsWith(LIST);
    }

    public PropertyType getDefaultSortColumn() {
      return _defaultSortColumn;
    }

    public ValueType getIdType() {
      return _idType;
    }

    public List<PropertyType> getSupportedProps() {
      List<PropertyType> supportedProps = new ArrayList<PropertyType>();
      supportedProps.addAll(_supportedProps);
      return supportedProps;
    }

    public List<PropertyType> getInternalOnlyProps() {
      List<PropertyType> internalOnlyProps = new ArrayList<PropertyType>();
      Iterator<PropertyType> it = _supportedProps.iterator();
      while (it.hasNext()) {
        PropertyType prop = it.next();
        if (prop.isInternalOnly()) {
          internalOnlyProps.add(prop);
        }
      }
      return internalOnlyProps;
    }

    public boolean isSupported(PropertyType property) {
      return _supportedProps.contains(property);
    }

    public String getIdParmName() {
      return toApiStr() + "." + PropertyType.REST_ID.toApiStr();
    }
  }
}
