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

package com.ibm.streams.operator.internal.types;

import com.ibm.streams.operator.internal.runtime.Schema;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.lang.ref.WeakReference;
import java.text.CharacterIterator;
import java.text.StringCharacterIterator;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.WeakHashMap;

/** Container class for the language types supported by the Java operator. */
public final class Types {

  /** Map of language type name to implementation types. */
  static final Map<String, ImplementationType<?>> languageTypes =
      Collections.synchronizedMap(new HashMap<String, ImplementationType<?>>());

  static {
    /*
     * Primitive types, and lists of them.
     */
    addImplementationType(new ListBoolean(addImplementationType(new BooleanSpl())));

    addImplementationType(new ListInt8(addImplementationType(new Int8())));

    addImplementationType(new ListInt16(addImplementationType(new Int16())));
    addImplementationType(new ListInt32(addImplementationType(new Int32())));
    addImplementationType(new ListInt64(addImplementationType(new Int64())));

    addImplementationType(new ListFloat32(addImplementationType(new Float32())));
    addImplementationType(new ListFloat64(addImplementationType(new Float64())));

    addImplementationType(new ListUstring(addImplementationType(new Ustring())));

    addImplementationType(ListSpl.list(addImplementationType(new Uint8())));
    addImplementationType(ListSpl.list(addImplementationType(new Uint16())));
    addImplementationType(ListSpl.list(addImplementationType(new Uint32())));
    addImplementationType(ListSpl.list(addImplementationType(new Uint64())));

    addImplementationType(new Complex32());
    addImplementationType(new Complex64());

    addImplementationType(new Decimal32());
    addImplementationType(new Decimal64());
    addImplementationType(new Decimal128());

    addImplementationType(new TimestampSpl());
    addImplementationType(new BlobSpl());
    addImplementationType(new RstringSpl());

    addImplementationType(new XMLSpl());
  }

  /**
   * Add a type keyed by its meta type and its language name.
   *
   * @param type
   */
  private static ImplementationType<?> addImplementationType(ImplementationType<?> type) {
    languageTypes.put(type.getLanguageType(), type);
    return type;
  }

  /**
   * Get the type representation from its type name.
   *
   * @param languageType name in the calling language
   * @return
   */
  public static ImplementationType<?> getTypeFromLanguageType(String languageType) {
    ImplementationType<?> itype = languageTypes.get(languageType);
    if (itype == null) {
      if (languageType.startsWith(XMLSplWithSchema.XML_SCHEMA_PREFIX))
        itype = XMLSplWithSchema.getXMLTypeWithSchema(languageType);
      else {
        itype = createCompositeType(languageType);
      }
    }
    return itype;
  }

  /** Get an array of types corresponding to an array of language type names. */
  public static ImplementationType<?>[] getTypesFromLanguage(String[] languageTypes) {
    ImplementationType<?>[] types = null;
    if (languageTypes != null) {
      types = new ImplementationType[languageTypes.length];
      for (int i = 0; i < languageTypes.length; i++) {
        types[i] = getTypeFromLanguageType(languageTypes[i]);
        if (types[i] == null)
          throw new UnsupportedOperationException(
              new Message(Key.SPL_RUNTIME_JAVAOP_INVALID_LANGUAGE_TYPE, languageTypes[i])
                  .getLocalizedMessageAndPrefix());
      }
    }
    return types;
  }

  private static final String BSTRING_PREFIX = "rstring[";
  private static final String LIST_PREFIX = "list<";
  private static final String SET_PREFIX = "set<";
  private static final String MAP_PREFIX = "map<";
  public static final String TUPLE_PREFIX = "tuple<";
  private static final String ENUM_PREFIX = "enum{";
  private static final String OPTIONAL_PREFIX = "optional<";

  private static ImplementationType<?> createCompositeType(String name) {
    if (name.startsWith(BSTRING_PREFIX)) {
      int blen = Integer.valueOf(name.substring(BSTRING_PREFIX.length(), name.length() - 1));
      return new BstringSpl(blen);
    }

    if (name.startsWith(LIST_PREFIX)) {
      String listType = subTypeName(name);
      ImplementationType<?> elementType = getTypeFromLanguageType(listType);
      if (name.endsWith("]")) {
        int bound = Integer.valueOf(name.substring(name.lastIndexOf('[') + 1, name.length() - 1));

        return ListSpl.boundList(elementType, bound);
      }
      return ListSpl.list(elementType);
    }
    if (name.startsWith(SET_PREFIX)) {
      String setType = subTypeName(name);
      if (name.endsWith("]")) {
        int bound = Integer.valueOf(name.substring(name.lastIndexOf('[') + 1, name.length() - 1));
        return SetSpl.boundSet(getTypeFromLanguageType(setType), bound);
      }
      return SetSpl.set(getTypeFromLanguageType(setType));
    }

    if (name.startsWith(MAP_PREFIX)) {
      String types = subTypeName(name);
      String[] kvtypes = mapTypes(types);
      if (name.endsWith("]")) {
        int bound = Integer.valueOf(name.substring(name.lastIndexOf('[') + 1, name.length() - 1));
        return MapSpl.boundMap(
            getTypeFromLanguageType(kvtypes[0]), getTypeFromLanguageType(kvtypes[1]), bound);
      }
      return MapSpl.map(getTypeFromLanguageType(kvtypes[0]), getTypeFromLanguageType(kvtypes[1]));
    }
    if (name.startsWith(TUPLE_PREFIX)) {
      String types = subTypeName(name);
      if (types == null) return null;
      return TupleSpl.newTupleSpl(name, tupleTypes(types.trim()));
    }

    if (name.startsWith(ENUM_PREFIX)) {
      return EnumSpl.getEnumType(name);
    }

    if (name.startsWith(OPTIONAL_PREFIX)) {
      String optionalType = subTypeName(name);
      return OptionalSpl.optional(getTypeFromLanguageType(optionalType));
    }

    return null;
  }

  private static String subTypeName(String name) {

    int tc = 0;
    int st = 0;
    CharacterIterator it = new StringCharacterIterator(name);
    for (char c = it.first(); c != CharacterIterator.DONE; c = it.next()) {
      if (c == '<') {
        if (tc == 0) st = it.getIndex() + 1;
        tc++;
        continue;
      }
      if (c == '>') {
        tc--;
        if (tc != 0) continue;

        // Type is complete.
        return name.substring(st, it.getIndex());
      }
    }
    return null;
  }

  private static String[] mapTypes(String types) {

    int tc = 0;
    CharacterIterator it = new StringCharacterIterator(types);
    for (char c = it.first(); c != CharacterIterator.DONE; c = it.next()) {
      switch (c) {
        case '<':
        case '{': // enum's can be embedded as the type
          tc++;
          break;
        case '>':
        case '}':
          tc--;
          break;
        case ',':
          if (tc != 0) continue;
          String kt = types.substring(0, it.getIndex());
          String vt = types.substring(it.getIndex() + 1, types.length());
          return new String[] {kt, vt};
      }
    }
    return null;
  }

  /**
   * Look for tuple types: type name[,type name]*
   *
   * @param types
   * @return
   */
  private static Schema tupleTypes(String types) {

    int tc = 0;
    int st = 0;
    List<String> attributeNames = new ArrayList<String>();
    List<String> attributeTypes = new ArrayList<String>();

    boolean needType = true;

    CharacterIterator it = new StringCharacterIterator(types);
    for (char c = it.first(); c != CharacterIterator.DONE; c = it.next()) {
      switch (c) {
        case ' ':
          // Skip white space at the start of an token.
          if (st == it.getIndex()) {
            st++;
            continue;
          }
          if (needType && tc == 0) {
            attributeTypes.add(types.substring(st, it.getIndex()).trim());
            st = it.getIndex() + 1;
            needType = false;
          }
          break;
        case '<':
        case '{':
          tc++;
          break;
        case '>':
        case '}':
          tc--;
          break;
        case ',':
          if (tc != 0) continue;
          attributeNames.add(types.substring(st, it.getIndex()).trim());
          st = it.getIndex() + 1;
          needType = true;
          break;
      }
    }

    attributeNames.add(types.substring(st, types.length()).trim());

    return getSchema(
        attributeTypes.toArray(new String[attributeTypes.size()]),
        attributeNames.toArray(new String[attributeNames.size()]));
  }

  /*
   * Interning handling for StreamSchema objects.
   *
   * We intern StreamSchema to ensure a single instance
   * of the StreamSchema per logical schema. This
   * ensures that:
   *   - When unserializing tuples each tuple
   *   does not result in a new StreamSchema object.
   *   - Passing input tuples (non-output tuples) to an output port
   *   is optimized by allowing a reference check (in Schema.equals)
   *   for verifying the tuple is of the right type.
   * (RTC 19459)
   *
   *  The interning is based upon weak references using standard
   *  WeakHashMap as while an SPL application is (currently) static
   *  wrt its port types, when using the mock framework schemas
   *  come and go, thus we don't want to hold onto unused
   *  StreamSchema objects forever.
   */

  public static Schema getSchema(String[] typeNames, String[] names) {
    return internedSchema(new SchemaKey(typeNames, names));
  }

  private static Schema newSchema(SchemaKey key) {
    ImplementationType<?>[] types = Types.getTypesFromLanguage(key.typeNames);
    return new Schema(types, key.names, key.typeNames);
  }

  private static Schema internedSchema(final SchemaKey key) {
    synchronized (schemas) {
      WeakReference<Schema> wr = schemas.get(key);
      Schema schema = (wr == null) ? null : wr.get();
      if (schema != null) {
        return schema;
      }
      schema = newSchema(key);
      schemas.put(key, new WeakReference<Schema>(schema));
      schemaKeys.put(schema, key);
      return schema;
    }
  }

  /*
   * While a Schema (S) is hard referenced elsewhere:
   *  -  a corresponding SchemaKey (SK) will be kept in schemaKeys,
   *     as the value in a WeakHashMap is kept via a hard reference.
   *  -  this then ensures that schemas contains the same Schema (S)
   *     and SchemaKey (SK). SchemaKey is kept as a weak reference in the
   *     value object to avoid Schema and SchemaKey being linked to
   *     each other in both directions via a hard reference, thus avoiding
   *     garbage collection.
   *
   * Thus a new lookup in schemas via any SchemaKey that is equal
   * to the SchemaKey in schemas/schemaKeys produces the same Schema object S.
   *
   * Once a Schema S object becomes only weakly referenced, this sequence of events happens
   *     a) Initially its corresponding SchemaKey SK is hard referenced through schemaKeys
   *     b) The WeakReference value object in schemas drops its reference to S and thus will return null (see check in internedSchema)
   *     c) The entry for S in schemaKeys is removed, removing the last hard reference to SK, leaving SK weakly referenced.
   *     d) The entry for SK in schemas is removed.
   *
   *  Note that these steps are driven by garbage collection, thus there is no guarantee as to when these
   *  steps happen, they must happen in order of a-d, but each step is not guaranteed to happen or occur
   *  immediately after the previous step.
   *
   *  However, once step a) has happened any request for an schema object of the same type will
   *  always r S and SK with S' and SK' where S.equals(S') and SK.equals(SK') are both true.
   *  Then S and SK will be completely unreferenced.
   */

  private static final Map<Schema, SchemaKey> schemaKeys = new WeakHashMap<Schema, SchemaKey>();

  private static final Map<SchemaKey, WeakReference<Schema>> schemas =
      Collections.synchronizedMap(new WeakHashMap<SchemaKey, WeakReference<Schema>>());

  // For testing
  public static int schemaKeysSize() {
    return schemaKeys.size();
  }

  public static int schemasSize() {
    return schemas.size();
  }

  /**
   * A key for a StreamSchema using SPL type names and attribute names.
   *
   * <p>Schema does not use this class as it is only added for interning, and adding into the
   * existing object causes issues for maintaining the serialization format.
   *
   * <p>Simply a object holding the type names and attribute names will valid equals/hashCode.
   */
  private static class SchemaKey {
    private final String[] typeNames;
    private final String[] names;

    SchemaKey(String[] typeNames, String[] names) {
      this.typeNames = typeNames != null ? typeNames.clone() : null;
      this.names = names != null ? names.clone() : null;
    }

    @Override
    public int hashCode() {
      final int prime = 31;
      int result = 1;
      result = prime * result + Arrays.hashCode(names);
      result = prime * result + Arrays.hashCode(typeNames);
      return result;
    }

    @Override
    public boolean equals(Object obj) {
      if (this == obj) return true;
      if (obj == null) return false;
      if (!(obj instanceof SchemaKey)) return false;
      SchemaKey other = (SchemaKey) obj;
      if (!Arrays.equals(names, other.names)) return false;
      if (!Arrays.equals(typeNames, other.typeNames)) return false;
      return true;
    }
  }
}
