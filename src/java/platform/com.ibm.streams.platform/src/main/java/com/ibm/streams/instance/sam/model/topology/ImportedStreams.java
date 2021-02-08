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

package com.ibm.streams.instance.sam.model.topology;

import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNullAndValidate;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsPositionIndex;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.validateIfPresent;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.platform.services.ImportedStreamsType;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

public class ImportedStreams implements Validator {

  private final Map<BigInteger, NameBasedImport> nameBasedImports;
  private PropertyBasedImport propertyBasedImport; // May be null
  private String filter; // May be null

  public ImportedStreams() {
    this.nameBasedImports = new HashMap<BigInteger, NameBasedImport>();
  }

  public ImportedStreams(ImportedStreams other) {
    if (other.nameBasedImports != null) {
      this.nameBasedImports = new HashMap<BigInteger, NameBasedImport>();
      for (Map.Entry<BigInteger, NameBasedImport> entry : other.nameBasedImports.entrySet()) {
        this.nameBasedImports.put(entry.getKey(), entry.getValue());
      }
    } else {
      this.nameBasedImports = null;
    }

    if (other.propertyBasedImport != null) {
      this.propertyBasedImport = new PropertyBasedImport(other.propertyBasedImport);
    } else {
      this.propertyBasedImport = null;
    }

    this.filter = other.filter;
  }

  public void add(ImportedStreams other) {
    nameBasedImports.putAll(other.getNameBasedImports());
    if (other.getPropertyBasedImport() != null) {
      propertyBasedImport = other.getPropertyBasedImport();
    }
    if (other.getFilter() != null) {
      filter = other.getFilter();
    }
  }

  public Map<BigInteger, NameBasedImport> getNameBasedImports() {
    return nameBasedImports;
  }

  public PropertyBasedImport getPropertyBasedImport() {
    return propertyBasedImport;
  }

  public void setPropertyBasedImport(PropertyBasedImport propertyBasedImport) {
    this.propertyBasedImport = propertyBasedImport;
  }

  public String getFilter() {
    return filter;
  }

  public void setFilter(String filter) {
    this.filter = filter;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine("<importedStreams");
    if (filter != null) {
      dumper.append(" filter=\"" + filter + "\"");
    }
    dumper.append(">").end();
    dumper.indent();
    if (nameBasedImports.size() > 0) {
      dumper.append(nameBasedImports);
    }
    if (propertyBasedImport != null) {
      dumper.append(propertyBasedImport.toString());
    }
    dumper.outdent();
    dumper.line("</importedStreams>");
    return dumper.toString();
  }

  public void validate() throws Exception {
    // Filter is optional
    validateIfPresent(propertyBasedImport);

    checkContentsPositionIndex(nameBasedImports.keySet());
    checkContentsNotNullAndValidate(nameBasedImports.values());
  }

  public ImportedStreamsType toXml() {
    ImportedStreamsType importedStreamsType = new ImportedStreamsType();
    if (filter != null) {
      importedStreamsType.setFilter(getFilter());
    }
    if (getNameBasedImports() != null) {
      // Name based imports seem to have a 1-based index
      BigInteger i = BigInteger.ONE;
      for (BigInteger index : getNameBasedImports().keySet()) {
        importedStreamsType.getNameBasedImport().add(getNameBasedImports().get(index).toXml(i));
        i = i.add(BigInteger.ONE);
      }
    }
    if (getPropertyBasedImport() != null) {
      importedStreamsType.setPropertyBasedImport(getPropertyBasedImport().toXml());
    }
    return importedStreamsType;
  }

  @Override
  public int hashCode() {
    return Objects.hash(nameBasedImports, propertyBasedImport, filter);
  }

  @Override
  public boolean equals(Object obj) {
    boolean equals = true;
    if (obj instanceof ImportedStreams) {
      ImportedStreams t = (ImportedStreams) obj;
      if ((getNameBasedImports() == null && t.getNameBasedImports() != null)
          || (getNameBasedImports() != null && t.getNameBasedImports() == null)) {
        equals = false;
      } else if (getNameBasedImports() != null && t.getNameBasedImports() != null) {
        if (getNameBasedImports().size() != t.getNameBasedImports().size()) {
          equals = false;
        } else {
          for (BigInteger index : getNameBasedImports().keySet()) {
            if (!getNameBasedImports().get(index).equals(t.getNameBasedImports().get(index))) {
              equals = false;
              break;
            }
          }
        }
      }

      if (equals) {
        if ((getPropertyBasedImport() == null && t.getPropertyBasedImport() != null)
            || (getPropertyBasedImport() != null && t.getPropertyBasedImport() == null)) {
          equals = false;
        } else if (getPropertyBasedImport() != null
            && t.getPropertyBasedImport() != null
            && !getPropertyBasedImport().equals(t.getPropertyBasedImport())) {
          equals = false;
        }
      }

      if (equals) {
        if ((getFilter() == null && t.getFilter() != null)
            || (getFilter() != null && t.getFilter() == null)) {
          equals = false;
        } else if (getFilter() != null
            && t.getFilter() != null
            && !getFilter().equals(t.getFilter())) {
          equals = false;
        }
      }
    } else {
      equals = false;
    }

    return equals;
  }
}
