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

package com.ibm.streams.operator.internal.runtime;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.encoding.BinaryEncoding;
import com.ibm.streams.operator.encoding.CharacterEncoding;
import com.ibm.streams.operator.internal.network.TupleDecoder;
import com.ibm.streams.operator.internal.object.OpInputTuple;
import com.ibm.streams.operator.internal.types.ImplementationType;
import com.ibm.streams.operator.internal.types.Types;
import com.ibm.streams.operator.meta.BoundedType;
import com.ibm.streams.operator.meta.CollectionType;
import com.ibm.streams.operator.meta.MapType;
import com.ibm.streams.operator.meta.OptionalType;
import com.ibm.streams.operator.meta.TupleType;
import com.ibm.streams.operator.types.ValueFactory;
import com.ibm.streams.operator.types.XML;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.io.ObjectStreamException;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.WeakHashMap;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

/**
 * An immutable object representing the schema of a stream. A stream consists of one or more ordered
 * attributes each with a name and a type.
 */
public final class Schema implements StreamSchema, Serializable {
  private static final String XMLNS_SPL_TUPLE_TYPE =
      "http://www.ibm.com/xmlns/prod/streams/spl/tupleType";

  private static final long serialVersionUID = -7224918441340432784L;

  /** Set of attribute names. Used for serialization. */
  private final String[] names;

  /** Set of attribute types. Used for serialization. */
  private final String[] typeNames;

  /** My set of attributes. */
  private final transient List<Attribute> attributes;

  /** Map of attribute names to attribute index. */
  private final transient Map<String, Integer> nameMap;

  private final transient Set<String> nameSet;

  /** List of the full type information ordered by the spade index. */
  private final transient ImplementationType<?>[] fullTypes;

  private final transient TupleDecoder<Tuple> decoder;

  private final transient Object[] defaultValues;
  private final transient Tuple defaultTuple;

  public Schema(ImplementationType<?>[] types, String[] names, String[] typeNames) {

    this.names = names;
    this.typeNames = typeNames;

    List<Attribute> attrs = new ArrayList<Attribute>(types.length);
    Map<String, Integer> tempNameMap = new HashMap<String, Integer>();

    for (int i = 0; i < types.length; i++) {
      final int index = i;
      final Type type = types[i];
      final String name = names[i];
      attrs.add(
          new Attribute() {

            public String getName() {
              return name;
            }

            public Type getType() {
              return type;
            }

            public int getIndex() {
              return index;
            }

            public boolean same(Attribute other) {
              return getName().equals(other.getName()) && getType().equals(other.getType());
            }
          });
      tempNameMap.put(name, index);
    }
    attributes = Collections.unmodifiableList(attrs);
    nameMap = Collections.unmodifiableMap(tempNameMap);
    nameSet = Collections.unmodifiableSet(new HashSet<String>(nameMap.keySet()));
    this.fullTypes = types;
    decoder = OpInputTuple.getTupleDecoder(this);

    defaultValues = new Object[attributes.size()];
    for (int i = 0; i < defaultValues.length; i++)
      defaultValues[i] = getImplementationTypes()[i].getDefaultValue();

    defaultTuple = getTuple(Collections.EMPTY_LIST);
  }

  private Object readResolve() throws ObjectStreamException {
    return Types.getSchema(typeNames, names);
  }

  /** Return the meta-data for the attribute at the given index according to the SPL program. */
  public Attribute getAttribute(int attributeIndex) {
    return attributes.get(attributeIndex);
  }

  public Attribute getAttribute(String attributeName) {
    int index = getAttributeIndex(attributeName);
    if (index == -1) return null;
    return getAttribute(index);
  }

  /**
   * Return the index of the attribute (according to the spade program) for the given attribute
   * name.
   */
  public int getAttributeIndex(String attributeName) {

    Integer index = nameMap.get(attributeName);
    if (index == null) return -1;

    return index;
  }

  /**
   * Returns the number of attributes in this schema.
   *
   * @return number of attributes in this schema.
   */
  public int getAttributeCount() {
    return attributes.size();
  }

  public Iterator<Attribute> iterator() {
    return attributes.iterator();
  }

  public String toString() {
    StringBuilder sb = new StringBuilder();

    for (Attribute attr : this) {
      if (sb.length() != 0) sb.append(", ");

      sb.append(attr.getType().getLanguageType());
      sb.append(" ");
      sb.append(attr.getName());
    }
    return sb.toString();
  }

  public ImplementationType<?>[] getImplementationTypes() {
    return fullTypes;
  }

  @Override
  public Tuple getTuple() {
    return defaultTuple;
  }

  /**
   * Get a new default values array for a new tuple. This is equivalent to defaultValues.clone() but
   * testing showed an explicit new and arraycopy is faster.
   */
  public final Object[] newDefaultValues() {
    final Object[] values = new Object[defaultValues.length];
    System.arraycopy(defaultValues, 0, values, 0, values.length);
    return values;
  }

  @Override
  public Tuple getTuple(Tuple other) {
    // Same schema, take a shortcut, we can use
    // reference equality as we intern schema objects.
    if (this == other.getStreamSchema()) return other.asReadOnlyTuple();

    return newTuple(getTupleAssign(other.getStreamSchema()).copyFromTuple(other));
  }

  /**
   * Maintain a lookup of matching attributes with other schemas. Used to optimize getTuple(Tuple)
   * and OutputTuple.assign(Tuple)
   */
  private transient Map<StreamSchema, TupleAssign> tupleAssigns =
      new WeakHashMap<StreamSchema, TupleAssign>();

  private class TupleAssign {
    private TupleAssign(Set<String> matchingNames, int[][] matchingIndexes) {
      this.matchingNames = matchingNames;
      this.matchingIndexes = matchingIndexes;
    }

    @SuppressWarnings("unused")
    private final Set<String> matchingNames;

    private final int[][] matchingIndexes;

    private Object[] copyFromTuple(Tuple tuple) {
      Object[] values = newDefaultValues();
      copyIntoValues(values, tuple);
      return values;
    }

    private void copyIntoValues(Object[] values, Tuple tuple) {
      final int[][] ii = matchingIndexes;
      for (int i = 0; i < ii.length; i++) {
        final int[] aii = ii[i];
        // getObject makes a copy if required, so safe
        // to directly store it in the object array.
        values[aii[0]] = tuple.getObject(aii[1]);
      }
    }
  }

  public final void copyIntoValues(Object[] values, Tuple tuple) {
    getTupleAssign(tuple.getStreamSchema()).copyIntoValues(values, tuple);
  }

  private TupleAssign getTupleAssign(StreamSchema otherSchema) {
    TupleAssign ta = tupleAssigns.get(otherSchema);
    if (ta != null) return ta;

    return newTupleAssign(otherSchema);
  }

  private TupleAssign newTupleAssign(final StreamSchema otherSchema) {
    Set<String> names = new HashSet<String>();
    for (Attribute attr : this) {
      Attribute oa = otherSchema.getAttribute(attr.getName());
      if (oa != null) {
        // Allow assignment from non-optional to optional matching types.
        Type type = attr.getType();
        Type valueType =
            (type.getMetaType().isOptional() ? ((OptionalType) type).getValueType() : type);
        if (attr.getName().equals(oa.getName())
            && (type.equals(oa.getType()) || valueType.equals(oa.getType())))
          names.add(attr.getName());
      }
    }
    if (names.isEmpty()) names = Collections.emptySet();
    names = Collections.unmodifiableSet(names);

    int[][] matchingIndexes = new int[names.size()][];
    int i = 0;
    for (String name : names) {
      int[] indexes = new int[2];
      indexes[0] = getAttributeIndex(name);
      indexes[1] = otherSchema.getAttributeIndex(name);
      matchingIndexes[i++] = indexes;
    }

    final TupleAssign ta = new TupleAssign(names, matchingIndexes);
    tupleAssigns.put(otherSchema, ta);
    return ta;
  }

  @Override
  public Tuple getTuple(Map<String, ?> map) {
    Object[] values = getValuesArray(true);
    for (Attribute attr : this) {
      final int ai = attr.getIndex();
      final String an = attr.getName();
      if (map.containsKey(an))
        values[ai] = getImplementationTypes()[ai].convertFromObjectCopy(map.get(an));
    }
    return newTuple(values);
  }

  @Override
  public Tuple getTuple(List<?> list) {
    Object[] values = getValuesArray(false);
    final int copyCount = Math.min(list.size(), values.length);
    for (int ai = 0; ai < copyCount; ai++) {
      values[ai] = getImplementationTypes()[ai].convertFromObjectCopy(list.get(ai));
    }
    fillAsDefault(copyCount, values);

    return newTuple(values);
  }

  @Override
  public Tuple getTuple(Object[] array) {
    Object[] values = getValuesArray(false);
    final int copyCount = Math.min(array.length, values.length);
    for (int ai = 0; ai < copyCount; ai++) {
      values[ai] = getImplementationTypes()[ai].convertFromObjectCopy(array[ai]);
    }
    fillAsDefault(copyCount, values);

    return newTuple(values);
  }

  private Tuple newTuple(Object[] values) {
    return decoder.newTuple(values);
  }

  private Object[] getValuesArray(boolean setDefault) {
    Object[] values = new Object[getAttributeCount()];
    if (setDefault) fillAsDefault(0, values);
    return values;
  }

  private void fillAsDefault(int start, Object[] values) {
    System.arraycopy(defaultValues, start, values, start, (values.length - start));
  }

  @Override
  public int hashCode() {
    final int prime = 73;
    int result = 1;
    result = prime * result + Arrays.hashCode(names);
    result = prime * result + Arrays.hashCode(typeNames);
    return result;
  }

  /** Two schemas are equal if they have exact same attributes in the exact same order. */
  @Override
  public boolean equals(Object other) {
    if (other == this) return true;

    if (!(other instanceof Schema)) return false;

    final Schema os = (Schema) other;
    final int attributeCount = getAttributeCount();
    if (os.getAttributeCount() != attributeCount) return false;

    for (int i = 0; i < attributeCount; i++) {
      if (!getAttribute(i).same(os.getAttribute(i))) return false;
    }
    return true;
  }

  /**
   * Allow two schemas to match if they have matching attribute names and types in the exact same
   * order, where the type of an attribute is allowed to match if it has the same type, or the
   * underlying type of an optional type is the same type.
   */
  public boolean equalsOrOptional(Object other) {
    if (other == this) return true;

    if (!(other instanceof Schema)) return false;

    final Schema os = (Schema) other;
    final int attributeCount = getAttributeCount();
    if (os.getAttributeCount() != attributeCount) return false;

    for (int i = 0; i < attributeCount; i++) {
      Attribute attr = getAttribute(i);
      Attribute oa = os.getAttribute(i);
      if (!attr.getName().equals(oa.getName())
          || !((ImplementationType<?>) attr.getType()).equalsOrOptional(oa.getType())) return false;
    }
    return true;
  }

  @Override
  public Comparator<Tuple> getAttributesComparator(boolean ascending, String... attributeNames) {
    final int[] ais;
    if (attributeNames == null || attributeNames.length == 0) {
      List<Attribute> la = new ArrayList<Attribute>();
      for (Attribute attr : this) {
        if (isAttributeComparable(attr)) la.add(attr);
      }
      ais = new int[la.size()];
      for (int i = 0; i < ais.length; i++) ais[i] = la.get(i).getIndex();
    } else {
      ais = new int[attributeNames.length];
      int i = 0;
      for (String name : attributeNames) {
        Attribute attr = getAttribute(name);
        if (!isAttributeComparable(attr)) throw new NullPointerException(); // TODO
        ais[i++] = attr.getIndex();
      }
    }
    Comparator<Tuple> ct =
        new Comparator<Tuple>() {

          @SuppressWarnings("unchecked")
          @Override
          public int compare(Tuple t1, Tuple t2) {

            for (int i = 0; i < ais.length; i++) {
              final int ai = ais[i];
              Comparable<Object> a1 = (Comparable<Object>) t1.getObject(ai);
              Comparable<Object> a2 = (Comparable<Object>) t2.getObject(ai);
              int ares = a1.compareTo(a2);
              if (ares != 0) return ares;
            }
            return 0;
          }
        };

    return ascending ? ct : Collections.reverseOrder(ct);
  }

  private boolean isAttributeComparable(Attribute attr) {
    Type type = attr.getType();
    return !type.getMetaType().isOptional()
        && Comparable.class.isAssignableFrom(type.getObjectType());
  }

  /** Get a new NativeBinaryEncoding instance. */
  @Override
  public BinaryEncoding newNativeBinaryEncoding() {
    return new NativeBinaryEncoding(this);
  }

  @Override
  public String getLanguageType() {
    boolean firstAttribute = true;
    StringBuilder sb = new StringBuilder("tuple<");
    for (Attribute attr : this) {
      if (!firstAttribute) sb.append(",");
      sb.append(attr.getType().getLanguageType());
      sb.append(" ");
      sb.append(attr.getName());
      firstAttribute = false;
    }
    sb.append(">");
    return sb.toString();
  }

  /** Get the tuple type as an XML type with the schema tupleModelType.xsd. */
  @Override
  public XML getXMLTupleType() {
    return getXMLTupleType((String) null);
  }

  @Override
  public XML getXMLTupleType(String namespacePrefix) {
    try {
      final DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
      dbf.setNamespaceAware(true);
      final Document doc = dbf.newDocumentBuilder().newDocument();

      // The tuple type is the root.
      Element tupleTypeXML = getXMLTupleTypeElement(doc, namespacePrefix);
      doc.appendChild(tupleTypeXML);

      Transformer transformer = TransformerFactory.newInstance().newTransformer();
      transformer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");

      transformer.setOutputProperty(OutputKeys.INDENT, "yes");

      XML v = ValueFactory.newXML(transformer, new DOMSource(doc));
      return v;
    } catch (Exception e) {
      throw new IllegalStateException(e);
    }
  }

  @Override
  public Element getXMLTupleTypeElement(final Document doc, String namespacePrefix) {
    return getTupleTypeElement(doc, this, namespacePrefix);
  }

  private static void addTupleTypeElement(
      final Document doc, final String prefix, Node parent, StreamSchema schema) {
    parent.appendChild(getTupleTypeElement(doc, schema, prefix));
  }

  private static String qualifiedName(String prefix, String name) {
    return prefix == null ? name : prefix + ":" + name;
  }

  private static Element getTupleTypeElement(
      final Document doc, final StreamSchema schema, final String prefix) {

    final Element tuple = doc.createElementNS(XMLNS_SPL_TUPLE_TYPE, qualifiedName(prefix, "tuple"));

    tuple.appendChild(doc.createComment(schema.getLanguageType()));

    for (Attribute attr : schema) {
      final Type type = attr.getType();
      final Element attrElem =
          doc.createElementNS(tuple.getNamespaceURI(), qualifiedName(prefix, "attr"));
      attrElem.setAttribute("name", attr.getName());

      addAttributeType(doc, prefix, type, attrElem, "type");
      tuple.appendChild(attrElem);
    }

    return tuple;
  }

  private static void addAttributeType(
      final Document doc,
      final String prefix,
      final Type type,
      final Element attrElem,
      final String primitiveAttributeName) {
    switch (type.getMetaType()) {
      case LIST:
      case BLIST:
        {
          CollectionType listType = (CollectionType) type;
          Element list =
              doc.createElementNS(attrElem.getNamespaceURI(), qualifiedName(prefix, "list"));
          list.appendChild(doc.createComment(type.getLanguageType()));
          addAttributeType(doc, prefix, listType.getElementType(), list, "elementType");
          if (Type.MetaType.BLIST == type.getMetaType())
            list.setAttribute("bound", Integer.toString(((BoundedType) type).getBoundedSize()));
          attrElem.appendChild(list);
          break;
        }
      case SET:
      case BSET:
        {
          CollectionType setType = (CollectionType) type;
          Element set =
              doc.createElementNS(attrElem.getNamespaceURI(), qualifiedName(prefix, "set"));
          set.appendChild(doc.createComment(type.getLanguageType()));
          addAttributeType(doc, prefix, setType.getElementType(), set, "elementType");
          if (Type.MetaType.BSET == type.getMetaType())
            set.setAttribute("bound", Integer.toString(((BoundedType) type).getBoundedSize()));

          attrElem.appendChild(set);
          break;
        }
      case MAP:
      case BMAP:
        {
          MapType mapType = (MapType) type;
          Element map =
              doc.createElementNS(attrElem.getNamespaceURI(), qualifiedName(prefix, "map"));
          map.appendChild(doc.createComment(type.getLanguageType()));
          MetaType keyMeta = mapType.getKeyType().getMetaType();
          Element key = map;
          if (keyMeta.isCollectionType() || keyMeta == MetaType.TUPLE) {
            key = doc.createElementNS(attrElem.getNamespaceURI(), qualifiedName(prefix, "key"));
            map.appendChild(key);
          }
          addAttributeType(doc, prefix, mapType.getKeyType(), key, "keyType");

          MetaType valueMeta = mapType.getValueType().getMetaType();
          Element value = map;
          if (valueMeta.isCollectionType() || valueMeta == MetaType.TUPLE) {
            value = doc.createElementNS(attrElem.getNamespaceURI(), qualifiedName(prefix, "value"));
            map.appendChild(value);
          }
          addAttributeType(doc, prefix, mapType.getValueType(), value, "valueType");

          if (Type.MetaType.BMAP == type.getMetaType())
            map.setAttribute("bound", Integer.toString(((BoundedType) type).getBoundedSize()));

          attrElem.appendChild(map);
          break;
        }
      case OPTIONAL:
        {
          OptionalType optionalType = (OptionalType) type;
          Element optional =
              doc.createElementNS(attrElem.getNamespaceURI(), qualifiedName(prefix, "optional"));
          optional.appendChild(doc.createComment(type.getLanguageType()));
          addAttributeType(doc, prefix, optionalType.getValueType(), optional, "elementType");
          attrElem.appendChild(optional);
          break;
        }
      case TUPLE:
        addTupleTypeElement(doc, prefix, attrElem, ((TupleType) type).getTupleSchema());
        break;
      default:
        // primitive types
        attrElem.setAttribute(primitiveAttributeName, type.getLanguageType());
        break;
    }
  }

  @Override
  public final Set<String> getAttributeNames() {
    return nameSet;
  }

  @Override
  public StreamSchema extendBySchemas(StreamSchema... extension) {
    if (extension.length == 0) return this;

    Schema extended = this;
    for (StreamSchema ext : extension) {
      extended = extended.extendFromAttributes(ext);
    }
    return extended;
  }

  @Override
  public StreamSchema extendByAttributes(Attribute... attributes) {
    if (attributes.length == 0) return this;
    return extendFromAttributes(Arrays.asList(attributes));
  }

  private Schema extendFromAttributes(Iterable<Attribute> newAttributes) {

    // Collection of the extended schemas attributes, note
    // the index of the attributes may not be correct,
    // we only care about the name and type.
    // List is there for ordering.
    final Map<String, Attribute> extendedSchema = new HashMap<String, Attribute>();
    final List<Attribute> extendedSchemaList = new ArrayList<Attribute>();

    // Add all of this schema's attribute, since we are the base
    for (Attribute attr : this) {
      extendedSchema.put(attr.getName(), attr);
      extendedSchemaList.add(attr);
    }

    // For each attribute in the new schema, add it to the
    // extended set unless it is the "same" as an existing one.
    for (Attribute newAttr : newAttributes) {

      final String newName = newAttr.getName();

      Attribute existingAttr = extendedSchema.get(newName);
      if (existingAttr != null) {
        // Extension rules, matching attributes are not added.
        if (existingAttr.same(newAttr)) continue;

        // Differing ones is an exception.
        throw new IllegalArgumentException();
      }

      extendedSchema.put(newAttr.getName(), newAttr);
      extendedSchemaList.add(newAttr);
    }

    // Nothing added
    if (extendedSchema.size() == getAttributeCount()) return this;

    final String[] extendedTypeNames = new String[extendedSchema.size()];
    final String[] extendedNames = new String[extendedSchema.size()];

    for (int i = 0; i < extendedNames.length; i++) {
      final Attribute newAttr = extendedSchemaList.get(i);
      extendedTypeNames[i] = newAttr.getType().getLanguageType();
      extendedNames[i] = newAttr.getName();
    }

    return Types.getSchema(extendedTypeNames, extendedNames);
  }

  @Override
  public StreamSchema extend(final Type type, final String name) {
    return extendSingle(type.getLanguageType(), name);
  }

  private StreamSchema extendSingle(String languageType, String name) {
    Schema ex = Types.getSchema(new String[] {languageType}, new String[] {name});
    return extendBySchemas(ex);
  }

  @Override
  public StreamSchema extend(String languageType, String name) {
    return this.extendBySchemas(singleTypeSchema(languageType, name));
  }

  private StreamSchema singleTypeSchema(String languageType, String name) {
    StringBuilder sb = new StringBuilder();
    sb.append("tuple<");
    sb.append(languageType);
    sb.append(' ');
    sb.append(name);
    sb.append('>');
    return createSchema(sb.toString());
  }

  private StreamSchema createSchema(final String definition) {
    return Type.Factory.getTupleType(definition).getTupleSchema();
  }

  @Override
  public StreamSchema remove(String... names) {
    if (names.length == 0) return this;
    final Set<String> remaining = new HashSet<String>(getAttributeNames());
    for (String name : names) remaining.remove(name);

    // Any change?
    if (remaining.size() == getAttributeCount()) return this;

    // No attributes left!
    if (remaining.isEmpty())
      throw new IllegalArgumentException(
          new Message(Key.SPL_RUNTIME_CANNOT_REMOVE_ALL_ATTRIBUTES_FROM_SCHEMA, toString())
              .getLocalizedMessageAndPrefix());

    StreamSchema reduced = null;
    for (Attribute attr : this) {
      if (!remaining.contains(attr.getName())) continue;
      if (reduced == null)
        reduced = singleTypeSchema(attr.getType().getLanguageType(), attr.getName());
      else reduced = reduced.extendByAttributes(attr);
    }

    return reduced;
  }

  @Override
  public CharacterEncoding newCharacterEncoding() {
    return new SPLCharEncoding(this);
  }
}
