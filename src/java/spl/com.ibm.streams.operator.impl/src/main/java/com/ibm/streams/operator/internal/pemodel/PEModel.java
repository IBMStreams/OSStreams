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

package com.ibm.streams.operator.internal.pemodel;

import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.platform.services.AugmentedPEInputPortType;
import com.ibm.streams.platform.services.AugmentedPEOutputPortType;
import com.ibm.streams.platform.services.AugmentedPEType;
import com.ibm.streams.platform.services.OperDefinitionType;
import com.ibm.streams.platform.services.PhysOperInstanceType;
import java.io.File;
import java.math.BigInteger;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public abstract class PEModel {

  /**
   * Index where the PE standalone attribute is stored in the array returned by {@link
   * #getParameters()}.
   */
  protected static final int PEX_STANDALONE = 0;

  /**
   * Index where the PE configuredHostName attribute is stored in the array returned by {@link
   * #getParameters()}.
   */
  protected static final int PEX_CONFIGURED_HOST_NAME = 1;

  /** Schemas used in the PE, indexed by the index in their tupleType. */
  protected final Map<BigInteger, StreamSchema> schemas = new HashMap<BigInteger, StreamSchema>();

  /** Operator definitions used in the PE, indexed by the index. */
  protected final Map<BigInteger, OperDefinitionType> opDefs =
      new HashMap<BigInteger, OperDefinitionType>();

  /** Extra info not covered by the AugmentedPEType schema. */
  private final String[] parameters;

  /** Toolkit name ==> Toolkit index */
  protected final Map<String, BigInteger> toolkitIndexMap = new HashMap<>();

  /** Toolkit index ==> Toolkit path */
  protected final Map<BigInteger, File> toolkitDirectoryMap = new HashMap<BigInteger, File>();

  private boolean standalone;
  private String configuredHostName;

  protected PEModel(final String[] params) {
    validateParams(params);
    parameters = params;
  }

  public abstract PhysOperInstanceType getOperatorInstance(int operatorIndex);

  public final OperDefinitionType getOperatorDefinition(PhysOperInstanceType op) {
    return opDefs.get(op.getDefinitionIndex());
  }

  public abstract AugmentedPEType getPEType();

  public final StreamSchema getSchema(BigInteger index) {
    return schemas.get(index);
  }

  public abstract int getOperatorCount();

  public final String[] getParameters() {
    return parameters;
  }

  /** Get the PE input port for an operator input port.n */
  public abstract AugmentedPEInputPortType getPEInputPort(BigInteger opIndex, BigInteger portIndex);

  /** Get the PE output port for an operator output port. */
  public abstract AugmentedPEOutputPortType getPEOutputPort(
      BigInteger opIndex, BigInteger portIndex);

  private void validateParams(final String[] params) {
    if (params == null) throw new IllegalArgumentException("parameters == null");
    if (params.length != 2)
      throw new IllegalArgumentException("parameters.length == " + params.length);
    standalone = Boolean.valueOf(params[PEX_STANDALONE]);
    configuredHostName = params[PEX_CONFIGURED_HOST_NAME];

    if (!standalone && configuredHostName == null)
      throw new IllegalArgumentException("parameters[PEX_CONFIGURED_HOST_NAME] == null");
    if (!standalone && configuredHostName.length() == 0)
      throw new IllegalArgumentException("parameters[PEX_CONFIGURED_HOST_NAME] == \"\"");
  }

  public final File getToolkitDirectory(PhysOperInstanceType op) {
    BigInteger toolkitIndex = op.getToolkitIndex();
    File toolkitDir = toolkitDirectoryMap.get(toolkitIndex);
    return toolkitDir;
  }

  /** Return a read-only view over toolkitIndexMap joined with toolkitDirectoryMap */
  public final Map<String, File> getToolkitDirectories() {
    return new Map<String, File>() {

      @Override
      public int size() {
        return toolkitDirectoryMap.size();
      }

      @Override
      public boolean isEmpty() {
        return toolkitDirectoryMap.isEmpty();
      }

      @Override
      public boolean containsKey(Object key) {
        return toolkitIndexMap.containsKey(key);
      }

      @Override
      public boolean containsValue(Object value) {
        return toolkitDirectoryMap.containsValue(value);
      }

      @Override
      public File get(Object key) {
        BigInteger toolkitIndex = toolkitIndexMap.get(key);
        return (toolkitIndex != null) ? toolkitDirectoryMap.get(toolkitIndex) : null;
      }

      @Override
      public Set<String> keySet() {
        return toolkitIndexMap.keySet();
      }

      @Override
      public Collection<File> values() {
        return toolkitDirectoryMap.values();
      }

      @Override
      public Set<java.util.Map.Entry<String, File>> entrySet() {
        Set<java.util.Map.Entry<String, File>> fileEntries = new HashSet<>(toolkitIndexMap.size());
        for (java.util.Map.Entry<String, BigInteger> indexEntry : toolkitIndexMap.entrySet()) {
          fileEntries.add(new Entry(indexEntry.getKey(), get(indexEntry.getKey())));
        }
        return fileEntries;
      }

      // Mutators are wrapped by Collections.unmodifiableMap()

      @Override
      public File put(String key, File value) {
        throw new UnsupportedOperationException();
      }

      @Override
      public File remove(Object key) {
        throw new UnsupportedOperationException();
      }

      @Override
      public void putAll(Map<? extends String, ? extends File> m) {
        throw new UnsupportedOperationException();
      }

      @Override
      public void clear() {
        throw new UnsupportedOperationException();
      }

      class Entry implements Map.Entry<String, File> {

        private final String name;
        private File dir;

        Entry(String name, File dir) {
          this.name = name;
          this.dir = dir;
        }

        @Override
        public String getKey() {
          return name;
        }

        @Override
        public File getValue() {
          return dir;
        }

        @Override
        public File setValue(File value) {
          File oldValue = dir;
          dir = value;
          return oldValue;
        }
      }
    };
  }

  public final boolean isStandalone() {
    return standalone;
  }
  // Note: if isStandalone(), this may be null or empty.
  public final String getConfiguredHostName() {
    return configuredHostName;
  }
}
