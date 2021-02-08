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

import com.ibm.streams.spl.messages.doc.StreamsSPLDOCMessagesKey.Key;
import com.ibm.streams.tools.doc.utilities.FileOutput;
import com.ibm.streams.tools.doc.utilities.InputParameters;
import com.ibm.streams.tools.doc.utilities.Logger;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.Toolkit;
import com.ibm.streams.tools.doc.utilities.rendering.SVGToJPEGConverter;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import org.eclipse.core.runtime.Path;

public class OperatorModelImageGenerator {

  // General variables.
  private static Logger logger = Logger.getInstance();
  private static InputParameters inputParameters = InputParameters.getInputParameters();
  private static boolean isAddJpeg =
      inputParameters.getImageType().equals(InputParameters.AddJpegImageType);
  private static boolean isJpegOnly =
      inputParameters.getImageType().equals(InputParameters.JpegOnlyImageType);

  private OperatorModelType operModel_ = null;
  private String operKind_ = null;
  private String modelDir_ = null;

  public OperatorModelImageGenerator(OperatorModelType model, String operKind, String modelDir) {
    operModel_ = model;
    operKind_ = operKind;
    modelDir_ = modelDir;
  }

  private ArrayList<Integer> generateSVG(File outFile) throws IOException {
    OperatorModelPropertiesBuilder builder =
        new OperatorModelPropertiesBuilder(operModel_, operKind_, modelDir_);
    OperatorModelProperties operModelProperties = builder.build();
    OperatorModelDrawer modelDrawer = new OperatorModelDrawer(operModelProperties);
    SVGGenerator svgGenerator = new SVGGenerator(modelDrawer);
    return svgGenerator.generate(outFile);
  }

  /*
  	// test code
  	public static void main(String[] argv) throws IOException
  	{
  		if (argv.length == 0 || argv[0] == null | argv[0].length() == 0) {
  			System.err.println("spl-make-doc: no toolkit defined!");
  			return;
  		}

  		ToolkitManager toolkitMgr = ToolkitManager.newInstance(argv[0]);
  		ToolkitType toolkitModel;

  		String fqProject = argv[0];
  		// Use the default directory for spldoc output
  		FileOutput.setDefaultOutputDirectory(fqProject);
  		String toolkitPath = fqProject + Path.SEPARATOR + "toolkit.xml";
  		toolkitModel = LoadModel.loadToolkitModel(toolkitPath);

  		// Generate documentation for compilation units (SPL files)
  		toolkitMgr.addToolkit(toolkitModel);

  		String fqToolkitPath = toolkitMgr.getFqToolkitPath();

  		for (ToolkitNamespaceType namespace : toolkitModel.getNamespace()) {
  			// Generate docs for primitive operators
  			for (PrimitiveToolkitOperatorType primOp : namespace.getPrimitiveOp()) {
  				String opModelFile = toolkitMgr.getModelFileNameForOp(primOp);
  				System.out.println("Processing primitive operator: " + opModelFile);
  				OperatorModelType opModel = LoadModel.loadOperatorModel(fqToolkitPath + File.separator + opModelFile);
  				OperatorModelImageGenerator.generateOpImage(fqToolkitPath, opModelFile, opModel, primOp);
  			}
  		}
  	}
  */

  public static Map<String, ArrayList<Integer>> generateOpImage(
      Toolkit toolkit,
      String namespace,
      String opModelFile,
      OperatorModelType opModel,
      PrimitiveToolkitOperatorType primOp) {

    String opModelPath = toolkit.getToolkitPath() + Path.SEPARATOR + opModelFile;
    String opKind = primOp.getName();
    File opModelPathFile = new File(opModelPath);
    String opModelDir = opModelPathFile.getParentFile().getAbsolutePath();
    OperatorModelImageGenerator generator =
        new OperatorModelImageGenerator(opModel, opKind, opModelDir);

    Map<String, ArrayList<Integer>> imageDimensionMap = new HashMap<String, ArrayList<Integer>>(1);
    ArrayList<Integer> imageDimensions = null;

    // Generate SVG image.
    String svgFileName = FileOutput.getSvgOutputFileName(toolkit, namespace, opKind);
    File svgFile = new File(svgFileName);
    try {
      imageDimensions = generator.generateSVG(svgFile);
      if (!isJpegOnly && imageDimensions != null) {
        // Reference SVG image unless using only JPEG.
        imageDimensionMap.put(svgFile.getName(), imageDimensions);
      }
    } catch (IOException e) {
      logger.printError(Key.CDISP7040E, new String[] {svgFileName}, e);
    }

    // If JPEG required, convert to JPEG image.
    if ((isAddJpeg || isJpegOnly) && imageDimensions != null) {
      assert (imageDimensions.size() > 1);
      SVGToJPEGConverter converter = new SVGToJPEGConverter();
      String jpegFileName = FileOutput.getJpegOutputFileName(toolkit, namespace, opKind);
      File jpegFile = new File(jpegFileName);
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
