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

package com.ibm.streams.instance.sam.model.runtime;

import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.TracingLevelType;
import com.ibm.streams.platform.services.TracingStyleType;
import com.ibm.streams.platform.services.TracingType;
import java.math.BigInteger;

public class Tracing implements IXmlSerializer<TracingType> {

  private Level level;
  private BigInteger size;
  private Style style;

  public Tracing() {}

  public Level getLevel() {
    return level;
  }

  public void setLevel(Level level) {
    this.level = level;
  }

  public BigInteger getSize() {
    return size;
  }

  public void setSize(BigInteger size) {
    this.size = size;
  }

  public Style getStyle() {
    return style;
  }

  public void setStyle(Style style) {
    this.style = style;
  }

  public enum Level {
    OFF("off"),
    ERROR("error"),
    WARN("warn"),
    INFO("info"),
    DEBUG("debug"),
    TRACE("trace");

    private final String value;

    Level(String value) {
      this.value = value;
    }

    public String getValue() {
      return value;
    }

    public static Level fromValue(String value) {
      for (Level lev : Level.values()) {
        if (lev.value.equals(value)) return lev;
      }
      throw new IllegalArgumentException(value);
    }
  }

  public enum Style {
    SPILL("spill"),
    WRAP("wrap");

    private final String value;

    Style(String value) {
      this.value = value;
    }

    public String getValue() {
      return value;
    }

    public static Style fromValue(String value) {
      for (Style s : Style.values()) {
        if (s.value.equals(value)) return s;
      }
      throw new IllegalArgumentException(value);
    }
  }

  public TracingType toXml() {
    TracingType xml = new TracingType();
    xml.setLevel(TracingLevelType.fromValue(getLevel().getValue()));
    if (size != null && (size.longValue() > 0)) {
      xml.setSize(getSize());
    }
    if (style != null) {
      xml.setStyle(TracingStyleType.fromValue(getStyle().getValue()));
    }
    return xml;
  }
}
