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

package com.ibm.streams.tools.doc.generator.compositeviz;

// import java.awt.Color;
import com.ibm.streams.spl.messages.doc.StreamsSPLDOCMessagesKey.Key;
import com.ibm.streams.tools.doc.utilities.FileOutput;
import com.ibm.streams.tools.doc.utilities.InputParameters;
import com.ibm.streams.tools.doc.utilities.Logger;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.Toolkit;
import com.ibm.streams.tools.doc.utilities.rendering.CompositeGraphLayoutException;
import com.ibm.streams.tools.doc.utilities.rendering.SVGToJPEGConverter;
import com.ibm.streams.tools.doc.utilities.rendering.SVGTooltipManager;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import org.apache.batik.svggen.SVGGeneratorContext;
import org.apache.batik.svggen.SVGGraphics2D;
import org.w3c.dom.Document;

public class CompositeOperatorGraphImageGenerator {

  // General variables.
  private static Logger logger = Logger.getInstance();
  private static InputParameters inputParameters = InputParameters.getInputParameters();
  private static boolean isAddJpeg =
      inputParameters.getImageType().equals(InputParameters.AddJpegImageType);
  private static boolean isJpegOnly =
      inputParameters.getImageType().equals(InputParameters.JpegOnlyImageType);

  private CompositeDefinitionType operModel_ = null;

  public CompositeOperatorGraphImageGenerator(CompositeDefinitionType model) {
    operModel_ = model;
  }

  private ArrayList<Integer> generate(String outFileName)
      throws CompositeGraphLayoutException, IOException {
    CompositeOperatorGraphBuilder builder = new CompositeOperatorGraphBuilder(operModel_);
    CompositeOperatorGraph operGraph = builder.build();
    if (operGraph == null) {
      return null;
    }

    // Define a dummy graph (due to the removal of JViews).
    ArrayList<Integer> imageDimensions = new ArrayList<>(2);
    imageDimensions.add(0, 40);
    imageDimensions.add(1, 40);

    // Get a DOMImplementation.
    org.w3c.dom.DOMImplementation domImpl =
        org.apache.batik.dom.GenericDOMImplementation.getDOMImplementation();

    // Create an instance of org.w3c.dom.Document.
    Document document = domImpl.createDocument(SVGGraphics2D.SVG_NAMESPACE_URI, "svg", null);
    SVGGeneratorContext context = SVGGeneratorContext.createDefault(document);
    context.setEmbeddedFontsOn(true);

    // Create an instance of the SVG Graphics.
    org.apache.batik.svggen.SVGGraphics2D svgGraphics = new SVGGraphics2D(context, true);
    SVGTooltipManager.getInstance().finalizeTooltips(svgGraphics);

    // Finally, stream out SVG to the output file using UTF-8 encoding.
    svgGraphics.stream(outFileName, true);
    return imageDimensions;
  }

  /*
  // Test code
  public static void main(String[] args) {
  	if (args.length == 0 || args[0] == null | args[0].length() == 0) {
  		System.err.println("spl-make-doc: no toolkit defined!");
  		return;
  	}

  	ToolkitManager toolkitMgr = ToolkitManager.newInstance(args[0]);
  	ToolkitType toolkitModel;
  	String fqProject = args[0];
  	// Use the default directory for spldoc output
  	FileOutput.setDefaultOutputDirectory(fqProject);
  	String toolkitPath = fqProject + Path.SEPARATOR + "toolkit.xml";
  	toolkitModel = LoadModel.loadToolkitModel(toolkitPath);
  	//  	    String toolkitName = toolkitModel.getName();
  	//   	    String toolkitVersion = toolkitModel.getVersion();
  	// Generate documentation for compilation units (SPL files)
  	{
  		toolkitMgr.addToolkit(toolkitModel);
  		try {
  			createImages(fqProject);
  		} catch(Exception e) {
  			e.printStackTrace();
  		}
  	}
  	System.out.println("spl-make-doc Image Generator complete");
  }
  */

  /**
   * Generate an image for the specified composite operator.
   *
   * @return map of image file name to image dimensions
   */
  public static Map<String, ArrayList<Integer>> generateCompositeOpImage(
      Toolkit toolkit, String namespace, CompositeDefinitionType compositeDef) {
    // Generate SVG image.
    Map<String, ArrayList<Integer>> imageDimensionMap = new HashMap<String, ArrayList<Integer>>(1);
    ArrayList<Integer> imageDimensions = null;
    String compositeName = compositeDef.getCompositeHead().getName();
    String svgFileName = FileOutput.getSvgOutputFileName(toolkit, namespace, compositeName);
    File svgFile = new File(svgFileName);
    CompositeOperatorGraphImageGenerator generator =
        new CompositeOperatorGraphImageGenerator(compositeDef);
    try {
      imageDimensions = generator.generate(svgFileName);
      if (!isJpegOnly && imageDimensions != null) {
        // Reference SVG image unless using only JPEG.
        imageDimensionMap.put(svgFile.getName(), imageDimensions);
      }
    } catch (CompositeGraphLayoutException e) {
      logger.printError(Key.CDISP7040E, new String[] {svgFileName}, e);
    } catch (IOException e) {
      logger.printError(Key.CDISP7040E, new String[] {svgFileName}, e);
    }
    // If JPEG required, convert to JPEG image.
    if ((isAddJpeg || isJpegOnly) && imageDimensions != null) {
      assert (imageDimensions.size() > 1);
      String jpegFileName = FileOutput.getJpegOutputFileName(toolkit, namespace, compositeName);
      File jpegFile = new File(jpegFileName);
      SVGToJPEGConverter converter = new SVGToJPEGConverter();
      try {
        converter.convert(
            svgFile,
            jpegFile,
            imageDimensions.get(0).intValue(),
            imageDimensions.get(1).intValue());
        if (isJpegOnly) {
          // Reference JPEG image when using only JPEG.
          imageDimensionMap.put(jpegFile.getName(), imageDimensions);
          svgFile.delete();
        }
      } catch (IOException e) {
        logger.printError(Key.CDISP7040E, new String[] {jpegFileName}, e);
      }
    }
    return imageDimensionMap;
  }
}
