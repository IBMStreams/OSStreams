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

package com.ibm.streams.tools.doc.generator.operviz;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import org.apache.batik.dom.GenericDOMImplementation;
import org.apache.batik.svggen.SVGGeneratorContext;
import org.apache.batik.svggen.SVGGraphics2D;
import org.w3c.dom.DOMImplementation;
import org.w3c.dom.Document;

public class SVGGenerator {
  private OperatorModelDrawer drawer_ = null;

  public SVGGenerator(OperatorModelDrawer drawer) {
    drawer_ = drawer;
  }

  public ArrayList<Integer> generate(File outFile) throws IOException {
    // Get a DOMImplementation.
    DOMImplementation domImpl = GenericDOMImplementation.getDOMImplementation();

    // Create an instance of org.w3c.dom.Document.
    //        String svgNS = "http://www.w3.org/2000/svg";
    Document document = domImpl.createDocument(SVGGraphics2D.SVG_NAMESPACE_URI, "svg", null);
    SVGGeneratorContext context = SVGGeneratorContext.createDefault(document);
    context.setEmbeddedFontsOn(true);

    // Create an instance of the SVG Graphics.
    SVGGraphics2D svgGraphics = new SVGGraphics2D(context, true);

    // Ask the test to render into the SVG Graphics2D implementation.
    ArrayList<Integer> imageDimensions = drawer_.paint(svgGraphics);

    // Finally, stream out SVG to the standard output using
    // UTF-8 encoding.
    boolean useCSS = true; // we want to use CSS style attributes

    FileWriter fileWriter = null;
    BufferedWriter bufferedWriter = null;
    try {
      fileWriter = new FileWriter(outFile);
      bufferedWriter = new BufferedWriter(fileWriter);
      svgGraphics.stream(bufferedWriter, useCSS);
    } catch (IOException e) {
      throw e;
    } finally {
      bufferedWriter.close();
      fileWriter.close();
    }
    return imageDimensions;
  }
}
