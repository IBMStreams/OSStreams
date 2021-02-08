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

package com.ibm.streams.controller.instance.sam;

import com.ibm.streams.admin.internal.api.application.bundle.BundleInfo;
import com.ibm.streams.controller.bundle.Bundle;
import java.io.File;
import java.io.FileOutputStream;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.Optional;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.w3c.dom.Element;
import org.xml.sax.InputSource;

public class ADLUtils {

  private static final Logger LOGGER = LoggerFactory.getLogger(ADLUtils.class);

  public static Optional<String> getADLFromFile(String fileName) {
    try {
      /*
       * Load the bundle info.
       */
      var bundleInfo = new BundleInfo(fileName, null);
      var bundleId = bundleInfo.getBuildID();
      var xml = bundleInfo.getADLXML();
      /*
       * Parse the document.
       */
      var dbFactory = DocumentBuilderFactory.newInstance();
      var dBuilder = dbFactory.newDocumentBuilder();
      var doc = dBuilder.parse(new InputSource(new StringReader(xml)));
      var app = (Element) doc.getElementsByTagName("splApplication").item(0);
      app.setAttribute("buildId", bundleId);
      /*
       * Fix-up the output directory.
       */
      var odir = app.getAttribute("outputDirectory");
      app.setAttribute("outputDirectory", "/tmp/assets/" + odir);
      /*
       * Fix-up the application directory.
       */
      var adir = app.getAttribute("applicationDirectory");
      app.setAttribute("applicationDirectory", "/tmp/assets/" + odir + "/" + adir);
      /*
       * Generate the new document.
       */
      var tf = TransformerFactory.newInstance();
      var transformer = tf.newTransformer();
      transformer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
      var writer = new StringWriter();
      transformer.transform(new DOMSource(doc), new StreamResult(writer));
      return Optional.of(writer.getBuffer().toString());
    } catch (Exception e) {
      e.printStackTrace();
      return Optional.empty();
    }
  }

  public static Optional<String> getADLFromBundle(Bundle bundle) {
    try {
      var fileName = "/tmp/" + bundle.getName();
      var tmpFile = new File(fileName);
      var ostream = new FileOutputStream(tmpFile);
      ostream.write(bundle.getContent());
      var adl = getADLFromFile(fileName);
      ostream.close();
      tmpFile.delete();
      return adl;
    } catch (Exception e) {
      e.printStackTrace();
      return Optional.empty();
    }
  }
}
