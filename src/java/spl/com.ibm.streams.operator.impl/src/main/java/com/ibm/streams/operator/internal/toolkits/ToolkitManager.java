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

package com.ibm.streams.operator.internal.toolkits;

import java.io.File;
import java.net.URI;
import java.util.HashMap;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

public class ToolkitManager {
  private HashMap<ToolkitKey, URI> toolkitMap;
  private static ToolkitManager manager_;

  public static synchronized ToolkitManager getInstance(String adlFilePath) throws Exception {
    if (manager_ == null) {
      manager_ = new ToolkitManager();
      manager_.initialize(adlFilePath);
    }
    return manager_;
  }

  private void initialize(String adlFilePath) throws Exception {

    toolkitMap = new HashMap<ToolkitKey, URI>();

    File adlFile = new File(adlFilePath);
    if (adlFile.exists()) {
      DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
      DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
      Document doc = dBuilder.parse(adlFile);

      if (doc != null) {
        NodeList list = doc.getElementsByTagName("toolkit");
        for (int i = 0; i < list.getLength(); i++) {
          Element elem = (Element) list.item(i);
          String name = elem.getAttribute("name");
          String version = elem.getAttribute("version");
          toolkitMap.put(new ToolkitKey(name, version), new URI(elem.getAttribute("uri")));
        }
      }
    }
  }

  public URI getToolkitPath(String name, String version) {
    ToolkitKey key = new ToolkitKey(name, version);
    return toolkitMap.get(key);
  }
}
