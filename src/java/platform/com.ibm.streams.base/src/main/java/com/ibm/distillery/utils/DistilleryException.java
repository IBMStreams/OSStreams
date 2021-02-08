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

package com.ibm.distillery.utils;

import com.ibm.distillery.utils.exc.HashStreamException;
import com.ibm.distillery.utils.exc.SymbolReaderException;
import com.ibm.icu.util.StringTokenizer;
import com.ibm.streams.platform.services.AdditionalExceptionDataType;
import com.ibm.streams.platform.services.ExceptionType;
import com.ibm.streams.platform.services.MessageElementType;
import com.ibm.streams.platform.services.ObjectFactory;
import java.io.ByteArrayOutputStream;
import java.io.PrintWriter;
import java.io.StringReader;
import java.io.StringWriter;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.Vector;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.transform.stream.StreamSource;

public class DistilleryException extends Exception {
  public static final long serialVersionUID = 1;
  public static final String NO_EXCEPTION = "noexception";

  // Exception location -- the location where the exception
  // occurred
  private String location;

  // Exception stack backtrace information -- maybe be brought in all
  // the way from the from C++ server
  private String backtrace;

  /// Exception related exception code
  private DistilleryExceptionCode exceptionCode;

  /// Substitution text related to the exception code
  private Vector<String> substitutionText;

  /// XML string containing the optional additional data
  private String additionalData;

  private static JAXBContext jc = null;
  private static Unmarshaller unmarshaller = null;
  private static Marshaller marshaller = null;

  /** Default construct */
  public DistilleryException() {
    super();

    location = new String();
    backtrace = new String();
    exceptionCode = new DistilleryExceptionCode();
    substitutionText = new Vector<String>();
    additionalData = new String();
  }

  /**
   * Construct a new exception with the specified detail message
   *
   * @param msg the exception related message
   */
  public DistilleryException(String msg) {
    this(msg, new DistilleryExceptionCode(), new Vector<String>());
  }

  /**
   * Construct a new exception with the specified detail message
   *
   * @param msg the exception related message
   * @param myExceptionCode the exception related exception code
   * @param mySubstitutionText the exception code related substitution text
   */
  public DistilleryException(
      String msg, DistilleryExceptionCode myExceptionCode, Vector<String> mySubstitutionText) {
    super(msg);
    location = new String();
    backtrace = new String();
    exceptionCode = myExceptionCode;
    substitutionText = mySubstitutionText;
    additionalData = new String();
  }

  /**
   * Constructs a new exception with the specified detail message and cause
   *
   * @param msg the exception related message
   * @param cause the exception cause
   */
  public DistilleryException(String msg, Throwable cause) {
    this(msg, cause, new DistilleryExceptionCode(), new Vector<String>());
  }

  /**
   * Constructs a new exception with the specified detail message and cause
   *
   * @param msg the exception related message
   * @param cause the exception cause
   * @param myExceptionCode the exception related exception code
   * @param mySubstitutionText the exception code related substitution text
   */
  public DistilleryException(
      String msg,
      Throwable cause,
      DistilleryExceptionCode myExceptionCode,
      Vector<String> mySubstitutionText) {
    super(msg, cause);
    location = new String();
    backtrace = new String();
    exceptionCode = myExceptionCode;
    substitutionText = mySubstitutionText;
    additionalData = new String();
  }

  /**
   * Constructs a new exception with the specified detail message and cause
   *
   * @param msg the exception related message
   * @param myExceptionCode the exception related exception code
   * @param mySubstitutionText the exception code related substitution text
   * @param myAdditionalData the additional data related to this exception code
   */
  public DistilleryException(
      String msg,
      DistilleryExceptionCode myExceptionCode,
      Vector<String> mySubstitutionText,
      String myAdditionalData) {
    super(msg);
    location = new String();
    backtrace = new String();
    exceptionCode = myExceptionCode;
    substitutionText = mySubstitutionText;
    additionalData = myAdditionalData;
  }

  /**
   * Constructs a new exception with the specified detail message and cause
   *
   * @param msg the exception related message
   * @param cause the exception cause
   * @param myExceptionCode the exception related exception code
   * @param mySubstitutionText the exception code related substitution text
   * @param myAdditionalData the additional data related to this exception code
   */
  public DistilleryException(
      String msg,
      Throwable cause,
      DistilleryExceptionCode myExceptionCode,
      Vector<String> mySubstitutionText,
      String myAdditionalData) {
    super(msg, cause);
    location = new String();
    backtrace = new String();
    exceptionCode = myExceptionCode;
    substitutionText = mySubstitutionText;
    additionalData = myAdditionalData;
  }

  /**
   * Constructs a new exception with the specified detail message and cause
   *
   * @param msg the exception related message
   * @param cause the exception cause
   * @param myExceptionCode the exception related exception code
   * @param mySubstitutionText the exception code related substitution text
   * @param myAdditionalData the additional data related to this exception code
   */
  public DistilleryException(
      String msg,
      Throwable cause,
      DistilleryExceptionCode myExceptionCode,
      Vector<String> mySubstitutionText,
      AdditionalExceptionDataType myAdditionalData)
      throws DistilleryException {
    super(msg, cause);
    location = new String();
    backtrace = new String();
    exceptionCode = myExceptionCode;
    substitutionText = mySubstitutionText;

    // marshall the additional data
    try {
      additionalData = toString(myAdditionalData);
    } catch (DistilleryException e) {
      throw e;
    }
  }

  /**
   * Constructs a new exception with the specified detail message and cause
   *
   * @param msg the exception related message
   * @param myExceptionCode the exception related exception code
   * @param mySubstitutionText the exception code related substitution text
   * @param myAdditionalData the additional data related to this exception code
   */
  public DistilleryException(
      String msg,
      DistilleryExceptionCode myExceptionCode,
      Vector<String> mySubstitutionText,
      AdditionalExceptionDataType myAdditionalData)
      throws DistilleryException {
    super(msg);
    location = new String();
    backtrace = new String();
    exceptionCode = myExceptionCode;
    substitutionText = mySubstitutionText;

    // marshall the additional data
    try {
      additionalData = toString(myAdditionalData);
    } catch (DistilleryException e) {
      throw e;
    }
  }

  /**
   * Construct a new exception with the specified cause and a detail message of (cause==null ? null
   * : cause.toString()) (which typically contains the class and detail message of cause)
   *
   * @param cause the exception cause
   */
  public DistilleryException(Throwable cause) {
    this(cause, new DistilleryExceptionCode(), new Vector<String>());
  }

  /**
   * Construct a new exception with the specified cause and a detail message of (cause==null ? null
   * : cause.toString()) (which typically contains the class and detail message of cause)
   *
   * @param cause the exception cause
   * @param myExceptionCode the exception related exception code
   * @param mySubstitutionText the exception code related substitution text
   */
  public DistilleryException(
      Throwable cause, DistilleryExceptionCode myExceptionCode, Vector<String> mySubstitutionText) {
    super(cause);
    location = new String();
    backtrace = new String();
    exceptionCode = myExceptionCode;
    substitutionText = mySubstitutionText;
    additionalData = new String();
  }

  /**
   * Obtain the location (source code information) of where an exception occurred
   *
   * @return the location where an exception occurred
   */
  public String getLocation() {
    return location;
  }

  /**
   * Obtain the backtrace (i.e., the call stack) of where an exception occurred
   *
   * @return the exception backtrace
   */
  public String getBacktrace() {
    return backtrace;
  }

  /**
   * Get the exception code related to the exception
   *
   * @return The exception code
   */
  public DistilleryExceptionCode getExceptionCode() {
    return exceptionCode;
  }

  /**
   * Get the substitution texts related to the exception code
   *
   * @return The vector of substitution texts
   */
  public Vector<String> getSubstitutionText() {
    return substitutionText;
  }

  /**
   * Check to see if the exception has additional data
   *
   * @return Boolean indicating if the exception has additional data
   */
  public Boolean hasAdditionalData() {
    return (additionalData.length() > 0);
  }

  /**
   * Get the exception's additional data
   *
   * @return The AdditionalExceptionDataType for the exception's additional data
   */
  public AdditionalExceptionDataType getAdditionalData() throws DistilleryException {
    AdditionalExceptionDataType addData = new AdditionalExceptionDataType();
    // marshall the additional data
    try {
      addData = getAdditionalDataObject(additionalData);
    } catch (DistilleryException e) {
      throw e;
    }
    return addData;
  }

  /**
   * Set the exception's additional data
   *
   * @param The AdditionalExceptionDataType to be set in the exception's additional data
   */
  public void setAdditionalData(AdditionalExceptionDataType myAdditionalData)
      throws DistilleryException {
    String addData = new String();
    try {
      addData = toString(myAdditionalData);
    } catch (DistilleryException e) {
      throw e;
    }
    additionalData = addData;
  }

  public String toString() {
    String ret = new String(super.toString());
    Throwable causeEx = super.getCause();
    if (causeEx != null) {
      ret = ret.concat(" -- CAUSE EXCEPTION ");
      ret = ret.concat(causeEx.toString());
      ret = ret.concat(" -- END CAUSE EXCEPTION ");
    }
    ret = ret.concat(",  location=" + location + ",  backtrace=" + backtrace);
    ret =
        ret.concat(
            ",  exceptionCode="
                + exceptionCode
                + ",  number of substitution text="
                + substitutionText.size());
    for (int i = 0; i < substitutionText.size(); i++) {
      ret = ret.concat(",  ");
      ret = ret.concat(substitutionText.get(i));
    }
    if (additionalData.length() > 0) {
      ret = ret.concat(", additionalData=" + additionalData);
    }
    return ret;
  }

  /**
   * Create an object
   *
   * @param constructor the constructor to be used
   * @param arguments the constructor parameter to be used
   * @return the newly instantiated object
   */
  private static Object createObject(Constructor constructor, Object[] arguments) {
    Object object = null;
    try {
      object = constructor.newInstance(arguments);
      return object;
    } catch (InstantiationException e) {
      throw new DistilleryRuntimeException(
          "Attempting to create an object '" + constructor.toString() + "'", e);
    } catch (IllegalAccessException e) {
      throw new DistilleryRuntimeException(
          "Attempting to create an object '" + constructor.toString() + "'", e);
    } catch (IllegalArgumentException e) {
      throw new DistilleryRuntimeException(
          "Attempting to create an object '" + constructor.toString() + "'", e);
    } catch (InvocationTargetException e) {
      throw new DistilleryRuntimeException(
          "Attempting to create an object '" + constructor.toString() + "'", e);
    }
  }

  private static synchronized JAXBContext initJc() throws JAXBException {
    if (jc == null) {
      jc = JAXBContext.newInstance(com.ibm.streams.platform.services.ObjectFactory.class);
      unmarshaller = jc.createUnmarshaller();
      marshaller = jc.createMarshaller();
      marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, new Boolean(true));
    }
    return jc;
  }

  /**
   * Instantiate and throw an exception
   *
   * @param eType exception type
   * @param eMsg exception message
   * @param eLocation exception location
   * @param eBacktrace exception backtrace information
   */
  public static void instantiateAndThrow(
      String eType, String eMsg, String eLocation, String eBacktrace) throws DistilleryException {
    instantiateAndThrow(
        eType,
        eMsg,
        eLocation,
        eBacktrace,
        new DistilleryExceptionCode(),
        new Vector<String>(),
        new String());
  }

  /**
   * Instantiate and throw an exception
   *
   * @param eType exception type
   * @param eMsg exception message
   * @param eLocation exception location
   * @param eBacktrace exception backtrace information
   * @param eExceptionCode exception message code
   * @param eSubstitionText substition text vector
   */
  public static void instantiateAndThrow(
      String eType,
      String eMsg,
      String eLocation,
      String eBacktrace,
      DistilleryExceptionCode eExceptionCode,
      Vector<String> eSubstitutionText)
      throws DistilleryException {
    instantiateAndThrow(
        eType, eMsg, eLocation, eBacktrace, eExceptionCode, eSubstitutionText, new String());
  }

  /**
   * Instantiate and throw an exception
   *
   * @param eType exception type
   * @param eMsg exception message
   * @param eLocation exception location
   * @param eBacktrace exception backtrace information
   * @param eExceptionCode exception message code
   * @param eSubstitionText substition text vector
   * @param eAdditionalData additional exception data in object form
   */
  public static void instantiateAndThrow(
      String eType,
      String eMsg,
      String eLocation,
      String eBacktrace,
      DistilleryExceptionCode eExceptionCode,
      Vector<String> eSubstitutionText,
      AdditionalExceptionDataType eAdditionalData)
      throws DistilleryException {
    String addData = new String();
    try {
      addData = toString(eAdditionalData);
    } catch (DistilleryException e) {
      throw e;
    }

    instantiateAndThrow(
        eType, eMsg, eLocation, eBacktrace, eExceptionCode, eSubstitutionText, addData);
  }

  private static com.ibm.streams.platform.services.ObjectFactory newObjf() {
    return new ObjectFactory();
  }

  private static String toString(AdditionalExceptionDataType addData) throws DistilleryException {
    String xmlString = new String();
    try {
      synchronized (initJc()) {
        JAXBElement<AdditionalExceptionDataType> obj =
            newObjf().createAdditionalExceptionData(addData);
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        marshaller.marshal(obj, baos);
        xmlString = baos.toString();
      }
    } catch (JAXBException je) {
      throw new com.ibm.distillery.utils.exc.SerializationException(
          "Could not marshal additional exception data: " + addData, je);
    }
    return xmlString;
  }

  @SuppressWarnings("unchecked")
  private static AdditionalExceptionDataType getAdditionalDataObject(String xmlString)
      throws DistilleryException {
    AdditionalExceptionDataType addData = new AdditionalExceptionDataType();
    // marshall the additional data
    try {
      synchronized (initJc()) {
        JAXBElement<AdditionalExceptionDataType> elem =
            (JAXBElement<AdditionalExceptionDataType>)
                unmarshaller.unmarshal(new StreamSource(new StringReader(xmlString)));
        addData = elem.getValue();
      }
    } catch (JAXBException je) {
      throw new com.ibm.distillery.utils.exc.SerializationException(
          "Could not instantiate exception: " + xmlString, je);
    }
    return addData;
  }

  /**
   * Instantiate and throw and exception
   *
   * @param xmlString xml string exceptionType
   */
  public static void instantiateAndThrow(String xmlString) throws DistilleryException {
    try {
      synchronized (initJc()) {
        JAXBElement<ExceptionType> elem =
            (JAXBElement<ExceptionType>)
                unmarshaller.unmarshal(new StreamSource(new StringReader(xmlString)));
        ExceptionType et = elem.getValue();
        Vector<String> subVec = new Vector<String>(et.getSubstitutionText());
        instantiateAndThrow(
            et.getType(),
            et.getMsg(),
            et.getLocation(),
            et.getBacktrace(),
            new DistilleryExceptionCode(et.getExceptionCode()),
            subVec,
            et.getExceptionAdditionalData());
      }
    } catch (JAXBException je) {
      throw new com.ibm.distillery.utils.exc.SerializationException(
          "Could not instantiate exception: " + xmlString, je);
    }
  }

  /**
   * Instantiate and throw and exception
   *
   * @param eType exception type
   * @param eMsg exception message
   * @param eLocation exception location
   * @param eBacktrace exception backtrace information
   * @param eExceptionCode the exception related exception code
   * @param eSubstitutionText the exception code related substitution text
   * @param eAdditionalData the additional data object XML string
   */
  public static void instantiateAndThrow(
      String eType,
      String eMsg,
      String eLocation,
      String eBacktrace,
      DistilleryExceptionCode eExceptionCode,
      Vector<String> eSubstitutionText,
      String eAdditionalData)
      throws DistilleryException {
    Class eClass;
    Class[] StringArgsClass = new Class[] {String.class};
    Object[] StringArgs = new Object[] {eMsg};
    DistilleryException exception = null;
    try {
      if (eType == null) {
        throw new DistilleryRuntimeException(
            "Exception type is not defined -- possible malformed XML-transported exception");
      }
      StringTokenizer st = new StringTokenizer(eType, "::");
      String nsName = null;
      String eName = null;
      if (st.countTokens() == 1) {
        eName = st.nextToken();
      } else {
        nsName = st.nextToken();
        eName = st.nextToken();
      }
      if (eName == null) {
        throw new DistilleryRuntimeException(
            "Could not extract C++ exception name from '" + eType + "'");
      }
      String javaClassName = null;
      if (nsName == null) {
        javaClassName = "com.ibm.distillery.utils." + eName;
      } else if (nsName.equals("UTILS_NAMESPACE")
          || nsName.equals("UTILS")
          || nsName.equals("Distillery")) {
        javaClassName = "com.ibm.distillery.utils.exc." + eName;
      } else if (nsName.equals("SPC_NAMESPACE") || nsName.equals("SPC")) {
        javaClassName = "com.ibm.distillery.spc.exc." + eName;
      } else if (nsName.equals("NAM_NAMESPACE") || nsName.equals("NAM")) {
        javaClassName = "com.ibm.distillery.nam.exc." + eName;
      } else if (nsName.equals("TRC_NAMESPACE") || nsName.equals("TRC")) {
        javaClassName = "com.ibm.distillery.trc.exc." + eName;
      } else if (nsName.equals("SAM_NAMESPACE") || nsName.equals("SAM")) {
        javaClassName = "com.ibm.streams.sam.exc." + eName;
      } else if (nsName.equals("HC_NAMESPACE") || nsName.equals("HC")) {
        javaClassName = "com.ibm.streams.hc.exc." + eName;
      } else if (nsName.equals("PEC_NAMESPACE") || nsName.equals("PEC")) {
        javaClassName = "com.ibm.streams.pec.exc." + eName;
      } else if (nsName.equals("SRM_NAMESPACE") || nsName.equals("SRM")) {
        javaClassName = "com.ibm.streams.srm.exc." + eName;
      } else if (nsName.equals("SCH_NAMESPACE") || nsName.equals("SCH")) {
        javaClassName = "com.ibm.streams.sch.exc." + eName;
      } else if (nsName.equals("SEC")) {
        javaClassName = "com.ibm.streams.sec.exc." + eName;
      } else if (nsName.equals("SPL")) {
        javaClassName = "com.ibm.streams.spl.internal.exc." + eName;
      } else {
        throw new DistilleryRuntimeException("Unknown namespace in '" + eType + "'");
      }
      eClass = Class.forName(javaClassName);
      Constructor StringArgsConstructor = eClass.getConstructor(StringArgsClass);
      exception = (DistilleryException) createObject(StringArgsConstructor, StringArgs);
      exception.location = eLocation;
      exception.backtrace = eBacktrace;
      exception.exceptionCode = eExceptionCode;
      exception.substitutionText = eSubstitutionText;
      exception.additionalData = eAdditionalData;

    } catch (ClassNotFoundException e) {
      throw new DistilleryRuntimeException("Attempting to create an object '" + eType + "'", e);
    } catch (NoSuchMethodException e) {
      throw new DistilleryRuntimeException("Attempting to create an object '" + eType + "'", e);
    }
    throw exception;
  }

  public String toXMLString() throws DistilleryException {
    ExceptionType e = new ExceptionType();

    // type
    String type = "DistilleryException";
    String canonicalName = getClass().getCanonicalName();
    String simpleName = getClass().getSimpleName();
    if (canonicalName.startsWith("com.ibm.distillery.utils.exc.")) {
      if (this instanceof HashStreamException || this instanceof SymbolReaderException) {
        type = "Distillery" + "::" + simpleName;
      } else {
        type = "UTILS_NAMESPACE" + "::" + simpleName;
      }
    } else if (canonicalName.startsWith("com.ibm.distillery.spc.exc.")) {
      type = "SPC_NAMESPACE" + "::" + simpleName;
    } else if (canonicalName.startsWith("com.ibm.distillery.nam.exc.")) {
      type = "NAM_NAMESPACE" + "::" + simpleName;
    } else if (canonicalName.startsWith("com.ibm.distillery.trc.exc.")) {
      type = "TRC_NAMESPACE" + "::" + simpleName;
    } else if (canonicalName.startsWith("com.ibm.streams.sam.exc.")) {
      type = "SAM_NAMESPACE" + "::" + simpleName;
    } else if (canonicalName.startsWith("com.ibm.streams.hc.exc.")) {
      type = "HC_NAMESPACE" + "::" + simpleName;
    } else if (canonicalName.startsWith("com.ibm.streams.pec.exc.")) {
      type = "PEC_NAMESPACE" + "::" + simpleName;
    } else if (canonicalName.startsWith("com.ibm.streams.srm.exc.")) {
      type = "SRM_NAMESPACE" + "::" + simpleName;
    } else if (canonicalName.startsWith("com.ibm.streams.sch.exc.")) {
      type = "SCH_NAMESPACE" + "::" + simpleName;
    } else if (canonicalName.startsWith("com.ibm.streams.sec.exc.")) {
      type = "SEC" + "::" + simpleName;
    } else if (canonicalName.startsWith("com.ibm.streams.spl.internal.exc.")) {
      type = "SPL" + "::" + simpleName;
    }

    e.setType(type);

    // location
    e.setLocation(getLocation());

    // message
    e.setMsg(getMessage());

    // backtrace
    StringWriter sw = new StringWriter();
    printStackTrace(new PrintWriter(sw));
    e.setBacktrace(sw.toString());

    // distillery exception code
    e.setExceptionCode(getExceptionCode().getMessageId());

    // distillery exception code substitution text
    for (String substitutionText : getSubstitutionText()) {
      e.getSubstitutionText().add(substitutionText);
    }

    // additional data
    e.setExceptionAdditionalData(additionalData);

    return toString(e);
  }

  private static String toString(ExceptionType e) throws DistilleryException {
    String xmlString = new String();
    try {
      synchronized (initJc()) {
        JAXBElement<ExceptionType> obj = newObjf().createException(e);
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        marshaller.marshal(obj, baos);
        xmlString = baos.toString();
      }
    } catch (JAXBException je) {
      throw new com.ibm.distillery.utils.exc.SerializationException(
          "Could not marshal exception: " + e, je);
    }
    return xmlString;
  }

  public AdditionalExceptionDataType getMessageInfo() throws DistilleryException {
    MessageElementType primary = new MessageElementType();
    primary.setMessageCode(getExceptionCode().getMessageId());
    for (String s : getSubstitutionText()) {
      primary.getSubstitutionText().add(s);
    }
    AdditionalExceptionDataType addData = new AdditionalExceptionDataType();
    addData.setPrimaryInformation(primary);
    if (hasAdditionalData()) {
      addData.getNestedAdditionalExceptionData().add(getAdditionalData());
    }
    return addData;
  }
}
