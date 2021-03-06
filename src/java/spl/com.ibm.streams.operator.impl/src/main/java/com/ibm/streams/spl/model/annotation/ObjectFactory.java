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

//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference
// Implementation,
// v2.2.8-b130911.1802
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a>
// Any modifications to this file will be lost upon recompilation of the source schema.
// Generated on: 2017.05.17 at 12:18:59 PM EDT
//

package com.ibm.streams.spl.model.annotation;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElementDecl;
import javax.xml.bind.annotation.XmlRegistry;
import javax.xml.namespace.QName;

/**
 * This object contains factory methods for each Java content interface and Java element interface
 * generated in the com.ibm.streams.spl.model.annotation package.
 *
 * <p>An ObjectFactory allows you to programatically construct new instances of the Java
 * representation for XML content. The Java representation of XML content can consist of schema
 * derived interfaces and classes representing the binding of schema type definitions, element
 * declarations and model groups. Factory methods for each of these are provided in this class.
 */
@XmlRegistry
public class ObjectFactory {

  private static final QName _Annotation_QNAME =
      new QName("http://www.ibm.com/xmlns/prod/streams/spl/annotation", "annotation");
  private static final QName _ListOrSetTypeSet_QNAME =
      new QName("http://www.ibm.com/xmlns/prod/streams/spl/annotation", "set");
  private static final QName _ListOrSetTypeValue_QNAME =
      new QName("http://www.ibm.com/xmlns/prod/streams/spl/annotation", "value");
  private static final QName _ListOrSetTypeTuple_QNAME =
      new QName("http://www.ibm.com/xmlns/prod/streams/spl/annotation", "tuple");
  private static final QName _ListOrSetTypeMap_QNAME =
      new QName("http://www.ibm.com/xmlns/prod/streams/spl/annotation", "map");
  private static final QName _ListOrSetTypeList_QNAME =
      new QName("http://www.ibm.com/xmlns/prod/streams/spl/annotation", "list");
  private static final QName _MapTypeKey_QNAME =
      new QName("http://www.ibm.com/xmlns/prod/streams/spl/annotation", "key");

  /**
   * Create a new ObjectFactory that can be used to create new instances of schema derived classes
   * for package: com.ibm.streams.spl.model.annotation
   */
  public ObjectFactory() {}

  /** Create an instance of {@link AnnotationType } */
  public AnnotationType createAnnotationType() {
    return new AnnotationType();
  }

  /** Create an instance of {@link AttributeType } */
  public AttributeType createAttributeType() {
    return new AttributeType();
  }

  /** Create an instance of {@link ValueType } */
  public ValueType createValueType() {
    return new ValueType();
  }

  /** Create an instance of {@link MapType } */
  public MapType createMapType() {
    return new MapType();
  }

  /** Create an instance of {@link TupleType } */
  public TupleType createTupleType() {
    return new TupleType();
  }

  /** Create an instance of {@link ListOrSetType } */
  public ListOrSetType createListOrSetType() {
    return new ListOrSetType();
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link AnnotationType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "annotation")
  public JAXBElement<AnnotationType> createAnnotation(AnnotationType value) {
    return new JAXBElement<AnnotationType>(_Annotation_QNAME, AnnotationType.class, null, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link ListOrSetType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "set",
      scope = ListOrSetType.class)
  public JAXBElement<ListOrSetType> createListOrSetTypeSet(ListOrSetType value) {
    return new JAXBElement<ListOrSetType>(
        _ListOrSetTypeSet_QNAME, ListOrSetType.class, ListOrSetType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link ValueType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "value",
      scope = ListOrSetType.class)
  public JAXBElement<ValueType> createListOrSetTypeValue(ValueType value) {
    return new JAXBElement<ValueType>(
        _ListOrSetTypeValue_QNAME, ValueType.class, ListOrSetType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link TupleType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "tuple",
      scope = ListOrSetType.class)
  public JAXBElement<TupleType> createListOrSetTypeTuple(TupleType value) {
    return new JAXBElement<TupleType>(
        _ListOrSetTypeTuple_QNAME, TupleType.class, ListOrSetType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link MapType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "map",
      scope = ListOrSetType.class)
  public JAXBElement<MapType> createListOrSetTypeMap(MapType value) {
    return new JAXBElement<MapType>(
        _ListOrSetTypeMap_QNAME, MapType.class, ListOrSetType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link ListOrSetType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "list",
      scope = ListOrSetType.class)
  public JAXBElement<ListOrSetType> createListOrSetTypeList(ListOrSetType value) {
    return new JAXBElement<ListOrSetType>(
        _ListOrSetTypeList_QNAME, ListOrSetType.class, ListOrSetType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link ValueType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "value",
      scope = MapType.class)
  public JAXBElement<ValueType> createMapTypeValue(ValueType value) {
    return new JAXBElement<ValueType>(
        _ListOrSetTypeValue_QNAME, ValueType.class, MapType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link ValueType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "key",
      scope = MapType.class)
  public JAXBElement<ValueType> createMapTypeKey(ValueType value) {
    return new JAXBElement<ValueType>(_MapTypeKey_QNAME, ValueType.class, MapType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link ListOrSetType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "set",
      scope = ValueType.class)
  public JAXBElement<ListOrSetType> createValueTypeSet(ListOrSetType value) {
    return new JAXBElement<ListOrSetType>(
        _ListOrSetTypeSet_QNAME, ListOrSetType.class, ValueType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link TupleType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "tuple",
      scope = ValueType.class)
  public JAXBElement<TupleType> createValueTypeTuple(TupleType value) {
    return new JAXBElement<TupleType>(
        _ListOrSetTypeTuple_QNAME, TupleType.class, ValueType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link MapType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "map",
      scope = ValueType.class)
  public JAXBElement<MapType> createValueTypeMap(MapType value) {
    return new JAXBElement<MapType>(_ListOrSetTypeMap_QNAME, MapType.class, ValueType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link ListOrSetType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "list",
      scope = ValueType.class)
  public JAXBElement<ListOrSetType> createValueTypeList(ListOrSetType value) {
    return new JAXBElement<ListOrSetType>(
        _ListOrSetTypeList_QNAME, ListOrSetType.class, ValueType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link ListOrSetType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "set",
      scope = AttributeType.class)
  public JAXBElement<ListOrSetType> createAttributeTypeSet(ListOrSetType value) {
    return new JAXBElement<ListOrSetType>(
        _ListOrSetTypeSet_QNAME, ListOrSetType.class, AttributeType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link TupleType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "tuple",
      scope = AttributeType.class)
  public JAXBElement<TupleType> createAttributeTypeTuple(TupleType value) {
    return new JAXBElement<TupleType>(
        _ListOrSetTypeTuple_QNAME, TupleType.class, AttributeType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link MapType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "map",
      scope = AttributeType.class)
  public JAXBElement<MapType> createAttributeTypeMap(MapType value) {
    return new JAXBElement<MapType>(
        _ListOrSetTypeMap_QNAME, MapType.class, AttributeType.class, value);
  }

  /** Create an instance of {@link JAXBElement }{@code <}{@link ListOrSetType }{@code >}} */
  @XmlElementDecl(
      namespace = "http://www.ibm.com/xmlns/prod/streams/spl/annotation",
      name = "list",
      scope = AttributeType.class)
  public JAXBElement<ListOrSetType> createAttributeTypeList(ListOrSetType value) {
    return new JAXBElement<ListOrSetType>(
        _ListOrSetTypeList_QNAME, ListOrSetType.class, AttributeType.class, value);
  }
}
