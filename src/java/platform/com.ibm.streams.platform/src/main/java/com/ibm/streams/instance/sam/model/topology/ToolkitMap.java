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

import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.ToolkitMapType;
import com.ibm.streams.platform.services.ToolkitsMapType;
import java.util.List;

public class ToolkitMap implements IXmlSerializer<ToolkitsMapType> {
  private final List<Toolkit> toolkits;

  public ToolkitMap(List<Toolkit> toolkits) {
    this.toolkits = toolkits;
  }

  @Override
  public ToolkitsMapType toXml() {
    ToolkitsMapType toolkitsMap = new ToolkitsMapType();
    for (Toolkit toolkit : toolkits) {
      ToolkitMapType toolkitMap = new ToolkitMapType();
      toolkitMap.setIndex(toolkit.getIndex());
      toolkitMap.setName(toolkit.getName());
      toolkitsMap.getToolkitMap().add(toolkitMap);
    }

    return toolkitsMap;
  }
}
