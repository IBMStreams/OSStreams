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

package com.ibm.streams.tools.doc.generator.dita;

import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.doc.StreamsSPLDOCMessagesKey.Key;
import com.ibm.streams.tools.doc.generator.compositeviz.CompositeOperatorGraphImageGenerator;
import com.ibm.streams.tools.doc.generator.dita.ParserSPLDOC.SPLDOCAnnotation;
import com.ibm.streams.tools.doc.generator.operviz.OperatorModelImageGenerator;
import com.ibm.streams.tools.doc.messages.Messages;
import com.ibm.streams.tools.doc.references.References;
import com.ibm.streams.tools.doc.utilities.Encoder;
import com.ibm.streams.tools.doc.utilities.ErrorManager;
import com.ibm.streams.tools.doc.utilities.FileOutput;
import com.ibm.streams.tools.doc.utilities.InputParameters;
import com.ibm.streams.tools.doc.utilities.Logger;
import com.ibm.streams.tools.doc.utilities.ToolkitManager;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.Artifact;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.ArtifactSubType;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.ArtifactType;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.Toolkit;
import com.ibm.streams.tools.sdl.reader.LoadModel;
import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType;
import com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType;
import com.ibm.xmlns.prod.streams.spl.common.LibraryType;
import com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType;
import com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType;
import com.ibm.xmlns.prod.streams.spl.function.DependenciesType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionModelType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionSetType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionsType;
import com.ibm.xmlns.prod.streams.spl.function.PrototypeType;
import com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType;
import com.ibm.xmlns.prod.streams.spl.operator.CodeTemplatesType;
import com.ibm.xmlns.prod.streams.spl.operator.ContextType;
import com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionSetType;
import com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType;
import com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionsType;
import com.ibm.xmlns.prod.streams.spl.operator.EnumerationType;
import com.ibm.xmlns.prod.streams.spl.operator.EnumerationsType;
import com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.InputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpLibraryDependenciesType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpParametersType;
import com.ibm.xmlns.prod.streams.spl.operator.LibraryDependenciesType;
import com.ibm.xmlns.prod.streams.spl.operator.MetricType;
import com.ibm.xmlns.prod.streams.spl.operator.MetricsType;
import com.ibm.xmlns.prod.streams.spl.operator.OpModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.OutputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.ParameterType;
import com.ibm.xmlns.prod.streams.spl.operator.ParametersType;
import com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType;
import com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeGraphType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeInputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifiersType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeOutputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeParametersType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeTypesType;
import com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifiersType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionParametersType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParameterType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType;
import com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType;
import com.ibm.xmlns.prod.streams.spl.source.code.SourceModelType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplDocAnnotationType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplDocDescriptionType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplDocType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplNamespaceType;
import com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType;
import com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierType;
import com.ibm.xmlns.prod.streams.spl.source.code.TypeModifiersType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;

/**
 * Generate DITA files that document a set of toolkits.
 *
 * <p>The generated documentation consists of a table of contents, and a set of pages, each
 * describing a toolkit artifact, or a set of artifacts. There is one DITA document file for each
 * page in the set, and a DITA map file for the table of contents.
 *
 * <p>TODO: Change String ditaDoc to StringBuilder ditaDoc
 *
 * @author afrenk
 */
public class GeneratorDita {

  private static GeneratorDita INSTANCE = null;

  // IDs used to link to elements in the reference documents.
  static final String OperatorModelRefPath =
      new File("..", FileOutput.getDitaPathNameReference(References.OperatorModelRefFile))
          .toString();
  static final String FunctionModelRefPath =
      new File("..", FileOutput.getDitaPathNameReference(References.FunctionModelRefFile))
          .toString();
  static final String RefId_Operator_ContextId =
      OperatorModelRefPath
          + "#"
          + References.OperatorModelRefId
          + "/"
          + References.OperatorModelContextId;

  static final String RefId_Operator_MetricKindId =
      RefId_Operator_ContextId
          + "__"
          + References.OperatorModelMetricsId
          + "_"
          + References.OperatorModelMetricsKindId;

  static final String RefId_Operator_ParametersId =
      OperatorModelRefPath + "#" + References.OperatorModelRefId + "/" + "parameters";

  static final String RefId_Operator_ParameterId = RefId_Operator_ParametersId + "__parameter";

  static final String RefId_Operator_InputPortSetId =
      OperatorModelRefPath + "#" + References.OperatorModelRefId + "/" + "input_port_set";

  static final String RefId_Operator_OutputPortSetId =
      OperatorModelRefPath + "#" + References.OperatorModelRefId + "/" + "output_port_set";

  static final String RefId_Operator_Dependencies = RefId_Operator_ContextId + "__dependencies";
  static final String RefId_Operator_Dependencies_ManagedLibId =
      RefId_Operator_Dependencies + "__managedlibrary";
  static final String RefId_Operator_Dependencies_LibId =
      RefId_Operator_Dependencies_ManagedLibId + "__lib";
  static final String RefId_Operator_Dependencies_LibPathId =
      RefId_Operator_Dependencies_ManagedLibId + "__libpath";
  static final String RefId_Operator_Dependencies_IncludePathId =
      RefId_Operator_Dependencies_ManagedLibId + "__includepath";

  // CSS output classes for images
  static final String OutputClass_Graph = "splgraph";
  static final String OutputClass_PrimOp = "splprimop";

  Integer DefaultImageWidth = 640;
  Integer DefaultImageHeight = 480;
  Dimension DefaultDimension = new Dimension(DefaultImageWidth, DefaultImageHeight);
  // The dimension of each operator image embedded in the DITA doc, by operator name.
  // This used to hold a set of images, but now holds only one, so is really not needed.
  Map<String, ArrayList<Integer>> imageDimensions;

  // Valid SPLDOC annotation tags.
  static String[] StandardAnnotationTags = {"exclude", "include"};
  static String[] CompositeAnnotationTags = {"input", "output", "param"};
  static String[] NativeFunctionAnnotationTags = {"param", "return", "throws"};
  static String[] SPLFunctionAnnotationTags = {"param", "return"};
  static String[] ToolkitAnnotationTags = {"sample"};

  static int DefaultIndent = 4;

  private InputParameters inputParameters = InputParameters.getInputParameters();
  private Logger logger = Logger.getInstance();
  private ToolkitManager toolkitMgr = ToolkitManager.getInstance();
  private Toolkit currentToolkit = null;
  private String docTitle;
  private boolean buildingDitaMap = false;
  private boolean hasSamples = false;
  private boolean generatingSampleToolkitDocs = false;
  private boolean isPDF = inputParameters.getTransType().equals(InputParameters.PDFTransType);
  private boolean isJpegOnly =
      inputParameters.getImageType().equals(InputParameters.JpegOnlyImageType);
  private boolean toolkitsHaveNativeFunctions = false;
  private boolean toolkitsHavePrimitiveOperators = false;

  // A map of dita document file names to dita document content
  // (for toolkit, namespace, SPL file, operator, etc).
  // This is used for batching the writing of files, though this is not really necessary.
  private Map<String, String> ditaPages = new HashMap<String, String>();

  // A map of child dita document path names to their parent and child documents.
  // This is used for child documents created using the SPLDOC new page markup.
  private class ChildDocument {
    String filename;
    String title;
    ChildDocument parent;
    ArrayList<ChildDocument> children;

    ChildDocument(String filename, String title, ChildDocument parent) {
      this.filename = filename;
      this.title = title;
      this.parent = parent;
      this.children = new ArrayList<ChildDocument>();
    }
  }

  private Map<String, ChildDocument> ditaChildDocs = new HashMap<String, ChildDocument>();

  /**
   * Create a new instance of this DITA
   *
   * @return
   */
  public static GeneratorDita newInstance() {
    if (INSTANCE == null) {
      INSTANCE = new GeneratorDita();
    }
    return INSTANCE;
  }

  public static GeneratorDita getInstance() {
    if (INSTANCE == null) {
      new Exception(
              new Message(Key.CDISP7124E, "GeneratorDita Instance not defined")
                  .getLocalizedMessageAndPrefix())
          .printStackTrace();
      return null;
    }
    return INSTANCE;
  }

  /**
   * Log the given warning or error message for an annotation, including the line number of the
   * annotation.
   */
  private void printAnnotationWarnOrError(
      Key messageId,
      LocationType location,
      SPLDOCAnnotation annotation,
      String[] args,
      boolean isWarning) {
    String locString = "";
    if (location != null) {
      locString += location.toString();
    }
    if (annotation != null && annotation.lineOffset >= 0) {
      Message locMessage = new Message(Key.CDISP7028E, annotation.lineOffset + 1);
      locString += ":" + locMessage.getLocalizedMessage();
    }
    if (locString.isEmpty()) {
      locString = null;
    }
    if (isWarning) {
      logger.printWarn(messageId, locString, args);
    } else {
      logger.printError(messageId, locString, args);
    }
  }

  /** Log the given warning message for an annotation. */
  private void printAnnotationWarn(
      Key messageId, LocationType location, SPLDOCAnnotation annotation, String[] args) {
    printAnnotationWarnOrError(messageId, location, annotation, args, true);
  }

  /** Log the given error message for an annotation. */
  private void printAnnotationError(
      Key messageId, LocationType location, SPLDOCAnnotation annotation, String[] args) {
    printAnnotationWarnOrError(messageId, location, annotation, args, false);
  }

  /**
   * Formats the given label and value of a property, and returns the corresponding string in DITA
   * format. The string is HTML entity encoded if encode is set true. The label is formatted as a
   * name if labelAsName is set true.
   */
  private String formatProperty(String label, String value, boolean encode, boolean labelAsName) {
    if (encode) {
      value = Encoder.HTMLEntityEncode(value);
    }
    return (labelAsName ? "<tt>" : "<b>")
        + label
        + (labelAsName ? "</tt>" : "</b>")
        + (value.length() == 0 ? "" : ": " + value)
        + "\n";
  }

  /**
   * Formats the given label and value of a property, and returns the corresponding string in DITA
   * format. The string is HTML entity encoded if encode is set true.
   */
  private String formatProperty(String label, String value, boolean encode) {
    return formatProperty(label, value, encode, false);
  }

  /**
   * Formats the given label and value of a property, and returns a corresponding HTML entity
   * encoded string in DITA format.
   */
  private String formatProperty(String label, String value) {
    return formatProperty(label, value, true);
  }

  /**
   * Formats the given label and value of a property, and returns a corresponding HTML entity
   * encoded string in DITA format. If the given link prefix is not null, the prefix and label are
   * used to make the label into a link to the property definition.
   */
  private String formatProperty(String label, String value, String link_prefix) {
    if (link_prefix != null) {
      return formatReferenceLink(link_prefix + "__" + label.toLowerCase(), label)
          + (value.length() == 0 ? "" : ": " + Encoder.HTMLEntityEncode(value))
          + "\n";
    }
    return formatProperty(label, value, true);
  }

  /**
   * Formats the given label and boolean value of a property, and returns a corresponding HTML
   * entity encoded string in DITA format. If the given link prefix is not null, the prefix and
   * label are used to make the label into a link to the property definition.
   */
  private String formatProperty(String label, boolean b, String link_prefix) {
    return formatProperty(label, Boolean.toString(b), link_prefix);
  }

  /**
   * Format the given label and link to an SPL reference document. If the generation of reference
   * documents is suppressed, return just the label, and not a link.
   */
  private String formatReferenceLink(String link, String label) {
    if (inputParameters.isSuppressReferenceDocs()) {
      return label;
    }
    return "<xref href=\"" + link + "\">" + label + "</xref>";
  }

  /**
   * Given a composite head, returns an HTML entity encoded prototype for that composite.
   *
   * <p>For example: composite M (output streamType K, L; input G, H)
   */
  private String formatCompositePrototype(CompositeHeadType head) {
    String prototype = isPublic(head) ? "" : "public ";
    prototype += "composite " + head.getName();
    CompositeOutputsType outputs = head.getOutputs();
    CompositeInputsType inputs = head.getInputs();
    if (outputs == null && inputs == null) {
      return prototype;
    }
    prototype += "(";
    if (outputs != null) {
      prototype += "output ";
      for (CompositePortType oPort : outputs.getOport()) {
        if (oPort.getType() != null) {
          prototype += oPort.getType() + " ";
        }
        prototype += oPort.getName() + ", ";
      }
      // Remove last ", ".  Simple, but not efficient
      prototype = prototype.substring(0, prototype.length() - 2);
      if (inputs != null) {
        prototype += "; ";
      }
    }
    if (inputs != null) {
      prototype += "input ";
      for (CompositePortType iPort : inputs.getIport()) {
        if (iPort.getType() != null) {
          prototype += iPort.getType() + " ";
        }
        prototype += iPort.getName() + ", ";
      }
      // Remove last ", ".  Simple, but not efficient
      prototype = prototype.substring(0, prototype.length() - 2);
    }
    prototype += ")";
    return Encoder.HTMLEntityEncode(prototype);
  }

  // The dimension of an image
  static class Dimension {
    Dimension(Integer width, Integer height) {
      this.width = width;
      this.height = height;
    }

    Integer width;
    Integer height;
  }

  private Dimension getImageDimension(String imageName) {
    if (imageDimensions == null) {
      logger.printTrace(Key.CDISP7072I, new String[] {imageName});
      return DefaultDimension;
    }
    ArrayList<Integer> a = imageDimensions.get(imageName);
    if (a == null) {
      logger.printTrace(Key.CDISP7073I, new String[] {imageName});
      return DefaultDimension;
    }
    return new Dimension(a.get(0), a.get(1));
  }

  private String createImageLinkOp(
      String namespace, String name, Key messageId, String outputclass) {
    assert (outputclass != null);
    StringBuilder imageLink = new StringBuilder();
    String imagePathName =
        FileOutput.getImagePathNameOp(currentToolkit, namespace, name)
            + "."
            + ((isPDF || isJpegOnly) ? FileOutput.JPEGFileExt : FileOutput.SVGFileExt);
    File docDir = new File("..", "..");
    File imageRelPath = new File(docDir, imagePathName);
    String imageRelPathName = imageRelPath.toString();
    Dimension dim = getImageDimension(imageRelPath.getName());
    Message message = new Message(messageId, imageRelPathName);
    // Output class for this section corresponds to CSS rule for formatting image.
    imageLink.append("<section outputclass=\"" + outputclass + "\">" + "\n");
    imageLink.append(
        "  <image href=\""
            + imageRelPathName
            + "\""
            + (isPDF ? "" : " width=\"" + dim.width + "\"" + " height=\"" + dim.height + "\"")
            + ">\n");
    imageLink.append("    <alt>" + message.getLocalizedMessage() + "</alt>" + "\n");
    imageLink.append("  </image>" + "\n");
    imageLink.append("</section>" + "\n");
    return imageLink.toString();
  }
  /** Report an error stating that the given artifact was not found in the given toolkit. */
  private void reportArtifactNotFoundError(
      Toolkit toolkit, String namespace, String name, ArtifactType type) {
    String toolkitPath = toolkit.getToolkitPath();
    Message message =
        new Message(
            Key.CDISP7020E,
            toolkitPath,
            ToolkitManager.ArtifactString(null, namespace, name, type),
            toolkitPath + Path.SEPARATOR + FileOutput.ToolkitModelFileName);
    logger.printException(new Exception(message.getLocalizedMessageAndPrefix()));
    return;
  }

  /** Return the specified artifact and report an error if the artifact is not found. */
  private Artifact getKnownArtifact(
      Toolkit toolkit, String namespace, String name, ArtifactType type) {
    Artifact artifact = toolkitMgr.getArtifact(toolkit, namespace, name, type);
    if (artifact == null) {
      reportArtifactNotFoundError(toolkit, namespace, name, type);
    }
    return artifact;
  }

  /** Return the specified artifact and report an error if the artifact is not found. */
  private Artifact getKnownArtifactByAltName(
      Toolkit toolkit, String namespace, String name, ArtifactType type) {
    Artifact artifact = toolkitMgr.getArtifactByAltName(toolkit, namespace, name, type);
    if (artifact == null) {
      reportArtifactNotFoundError(toolkit, namespace, name, type);
    }
    return artifact;
  }
  /**
   * Given an SPLDOC markup description and the uri of the file containing the description, return
   * the first sentence.
   */
  private String getFirstSentence(String description, String fileUri) {
    if (description == null) {
      return "";
    }
    return Translater.xlateFirstSentence(description, fileUri);
  }

  /**
   * Given an SplDoc element and the uri of the file containing the description, return the first
   * sentence.
   */
  private String getFirstSentence(SplDocType splDoc, String fileUri) {
    SplDocDescriptionType descriptionType;
    if (splDoc == null || (descriptionType = splDoc.getDescription()) == null) {
      return "";
    }
    return getFirstSentence(descriptionType.getDescription(), fileUri);
  }

  /** Return the display label for a list of toolkit artifacts. */
  private String getListLabel(ArtifactType type) {
    switch (type) {
      case Toolkit:
        return logger.getMessage(Messages.MSG_TITLE_TOOLKITS);
      case Namespace:
        return logger.getMessage(Messages.MSG_TITLE_NAMESPACES);
      case Function:
        return logger.getMessage(Messages.MSG_TITLE_FUNCTIONS);
      case Operator:
        return logger.getMessage(Messages.MSG_TITLE_OPERATORS);
      case Type:
        return logger.getMessage(Messages.MSG_TITLE_TYPES);
      default:
        throw new IllegalArgumentException(
            new Message(Key.CDISP7124E, "Unexpected toolkit artifact type: " + type)
                .getLocalizedMessageAndPrefix());
    }
  }

  /**
   * Return the display label for the title of a list of the main or, if isSample is true, the
   * sample toolkit artifacts.
   */
  private String getListTitleLabel(ArtifactType type, boolean isSample) {
    if (!isSample) {
      return getListLabel(type);
    }
    switch (type) {
      case Toolkit:
        return logger.getMessage(Messages.MSG_TITLE_SAMPLE_TOOLKITS);
      case Namespace:
        return logger.getMessage(Messages.MSG_TITLE_SAMPLE_NAMESPACES);
      case Function:
        return logger.getMessage(Messages.MSG_TITLE_SAMPLE_FUNCTIONS);
      case Operator:
        return logger.getMessage(Messages.MSG_TITLE_SAMPLE_OPERATORS);
      case Type:
        return logger.getMessage(Messages.MSG_TITLE_SAMPLE_TYPES);
      default:
        throw new IllegalArgumentException(
            new Message(Key.CDISP7124E, "Unexpected toolkit artifact type: " + type)
                .getLocalizedMessageAndPrefix());
    }
  }

  /** Return the display label for a toolkit artifact in an index. */
  private String getListArtifactLabel(Artifact artifact) {
    switch (artifact.type) {
      case Toolkit:
        return getToolkitLabel(artifact.toolkit);
      case Function:
        return Encoder.HTMLEntityEncode(
            getFunctionSignature((ToolkitFunctionType) artifact.eObject));
      case Namespace:
      case Operator:
      case Type:
        return artifact.name;
      default:
        throw new IllegalArgumentException(
            new Message(Key.CDISP7124E, "Unexpected toolkit artifact type: " + artifact.type)
                .getLocalizedMessageAndPrefix());
    }
  }

  /** Return the display label for a toolkit. */
  private String getToolkitLabel(Toolkit toolkit) {
    return toolkit.getToolkitModel().getName() + " " + toolkit.getToolkitModel().getVersion();
  }

  /** Return the display label for a namespace. */
  private String getNamespaceLabel(String namespace) {
    if (namespace.length() == 0) {
      // Format default namespace as "<default>"
      // HN: Not sure we need to be able to translate default.  Could be considered a keyword
      return "&lt;" + logger.getMessage(Messages.MSG_DEFAULT_NAMESPACE_NAME) + "&gt;";
    } else {
      return namespace;
    }
  }

  /** Return the display label for a function model. */
  private String getFunctionModelLabel(String fileUri) {
    return logger.getMessage(
        fileUri.endsWith(FileOutput.CppFunctionModelFileName)
            ? Messages.MSG_TITLE_FUNCTION_MODEL_CPP
            : Messages.MSG_TITLE_FUNCTION_MODEL_JAVA);
  }

  /** Return the display label for the title of a function model. */
  private String getFunctionModelTitleLabel(String fileUri, boolean isSample) {
    if (!isSample) {
      return getFunctionModelLabel(fileUri);
    }
    return logger.getMessage(
        fileUri.endsWith(FileOutput.CppFunctionModelFileName)
            ? Messages.MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP
            : Messages.MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA);
  }

  /** Return the display label for an SPL file. */
  private String getSPLFileLabel(String fqSourceFile) {
    return new File(fqSourceFile).getName();
  }

  /**
   * Create the reference for a link to a DITA document. The reference is to a document for all the
   * main or, if isSample is true, all the sample toolkits, if a null toolkit is specified. The
   * reference is to an element within the document, if a non-null identifier is specified. The
   * reference is relative to the document currently being processed.
   */
  private String createDitaLinkRef(Toolkit toolkit, String fileName, String id, boolean isSample) {
    String path = fileName;
    if (buildingDitaMap || toolkit != currentToolkit || isSample != generatingSampleToolkitDocs) {
      path = FileOutput.getDitaPathName(toolkit, path, isSample);
      if (!buildingDitaMap) path = new File("..", path).toString();
    }
    return path + "." + FileOutput.DitaFileExt + (id == null ? "" : "#" + id);
  }
  // Toolkit can be null.
  public String createDitaLinkRef(Toolkit toolkit, String fileName, boolean isSample) {
    return createDitaLinkRef(toolkit, fileName, null, isSample);
  }
  // Toolkit must not be null.
  public String createDitaLinkRef(Toolkit toolkit, String fileName, String id) {
    return createDitaLinkRef(toolkit, fileName, id, toolkit.isSample());
  }
  // Toolkit must not be null.
  public String createDitaLinkRef(Toolkit toolkit, String fileName) {
    return createDitaLinkRef(toolkit, fileName, null, toolkit.isSample());
  }

  public String createDitaLinkRef(String fileName, boolean isSample) {
    return createDitaLinkRef(null, fileName, null, isSample);
  }

  /** Create the reference for a link to a toolkit artifact in a DITA document. */
  public String createDitaLinkRef(Artifact artifact) {
    if (artifact.type == ArtifactType.Toolkit) {
      return createDitaLinkRef(
          artifact.toolkit, FileOutput.getDocFileNameToolkit(artifact.toolkit));
    } else if (artifact.type == ArtifactType.Namespace) {
      return createDitaLinkRef(artifact.toolkit, FileOutput.getDocFileNameNamespace(artifact.name));
    } else if (artifact.type == ArtifactType.Function) {
      if (artifact.subtype == ArtifactSubType.NonNative) {
        return createDitaLinkRef(
            artifact.toolkit,
            FileOutput.getDocFileNameSPLFile(artifact.namespace, artifact.uriValue),
            Constants.ElementId_Prefix_SplFunction + artifact.name);
      } else {
        return createDitaLinkRef(
            artifact.toolkit,
            FileOutput.getDocFileNameFunction(artifact.namespace, artifact.uriValue),
            Constants.ElementId_Prefix_NativeFunction + artifact.name);
      }
    } else if (artifact.type == ArtifactType.FunctionModel) {
      return createDitaLinkRef(
          artifact.toolkit,
          FileOutput.getDocFileNameFunction(artifact.namespace, artifact.uriValue));
    } else if (artifact.type == ArtifactType.Operator) {
      if (artifact.subtype == ArtifactSubType.Composite) {
        return createDitaLinkRef(
            artifact.toolkit,
            FileOutput.getDocFileNameSPLFile(artifact.namespace, artifact.uriValue),
            Constants.ElementId_Prefix_Composite + artifact.name);
      } else {
        return createDitaLinkRef(
            artifact.toolkit, FileOutput.getDocFileNameOp(artifact.namespace, artifact.name));
      }
    } else if (artifact.type == ArtifactType.Type) {
      if (artifact.subtype == ArtifactSubType.Composite) {
        return createDitaLinkRef(
            artifact.toolkit,
            FileOutput.getDocFileNameSPLFile(artifact.namespace, artifact.uriValue),
            Constants.ElementId_Prefix_CompositeType + artifact.name);
      } else {
        return createDitaLinkRef(
            artifact.toolkit,
            FileOutput.getDocFileNameSPLFile(artifact.namespace, artifact.uriValue),
            Constants.ElementId_Prefix_SplType + artifact.name);
      }
    } else if (artifact.type == ArtifactType.CompilationUnit) {
      return createDitaLinkRef(
          artifact.toolkit,
          FileOutput.getDocFileNameSPLFile(artifact.namespace, artifact.uriValue));
    } else {
      logger.printError(
          Key.CDISP7124E,
          new String[] {"unexpected type for a toolkit artifact link: " + artifact.type});
      ErrorManager.throwFatalErrorException();
      return null;
    }
  }

  /** Create a link to a DITA document. */
  public String createDitaLink(String linkReference, String label) {
    return "<xref href=\"" + linkReference + "\">" + label + "</xref>";
  }

  /**
   * Create a link to a DITA document. The link is to a document for all the main or, if isSample is
   * true, all the sample toolkits, if a null toolkit is specified. The link is to an element within
   * the document, if a non-null identifier is specified.
   */
  private String createDitaLink(
      Toolkit toolkit, String ditaFileName, String ditaElementId, String label, boolean isSample) {
    return createDitaLink(createDitaLinkRef(toolkit, ditaFileName, ditaElementId, isSample), label);
  }

  /** Create a link to a DITA document for a toolkit artifact or index of artifacts. */
  private String createDitaLinkToolkits() {
    return createDitaLink(null, FileOutput.AllToolkitsFileName, null, docTitle, false);
  }

  private String createDitaLinkSamples() {
    return createDitaLink(
        null,
        FileOutput.AllToolkitsFileName,
        null,
        logger.getMessage(Messages.MSG_TITLE_SAMPLE_TOOLKITS),
        true);
  }

  private String createDitaLinkToolkit(Toolkit toolkit) {
    return createDitaLink(
        toolkit,
        FileOutput.getDocFileNameToolkit(toolkit),
        null,
        getToolkitLabel(toolkit),
        toolkit.isSample());
  }

  private String createDitaLinkNamespace(Toolkit toolkit, String namespace) {
    return createDitaLink(
        toolkit,
        FileOutput.getDocFileNameNamespace(namespace),
        null,
        getNamespaceLabel(namespace),
        toolkit.isSample());
  }

  private String createDitaLinkOp(Toolkit toolkit, String namespace, String name) {
    return createDitaLink(
        toolkit, FileOutput.getDocFileNameOp(namespace, name), null, name, toolkit.isSample());
  }

  /**
   * Given a fully qualified name of a SPL file, create a link to the DITA document generated for
   * that file. example input: my.examples/Sample.spl OR my/examples/Sample.spl example output:
   * <xref href="my.example$Sample.xml">my.examples/Sample.spl</xref>
   */
  private String createDitaLinkSplFile(Toolkit toolkit, String namespace, String fqSourceFile) {
    return createDitaLink(
        toolkit,
        FileOutput.getDocFileNameSPLFile(namespace, fqSourceFile),
        null,
        getSPLFileLabel(fqSourceFile),
        toolkit.isSample());
  }

  // A set of names of composites that have no graphs and are not operators.
  private HashSet<String> CompositeWithoutGraph = new HashSet<String>();

  /**
   * Given a the name of a composite, returns true if the composite is an operator, otherwise false.
   */
  private boolean compositeIsOperator(String name) {
    return !CompositeWithoutGraph.contains(name);
  }

  /** Given a list of artifacts, filter out any composites that are not operators. */
  private void removeNonOperatorComposites(List<Artifact> artifactList) {
    for (Iterator<Artifact> it = artifactList.iterator(); it.hasNext(); ) {
      Artifact artifact = it.next();
      if (artifact.type == ArtifactType.Operator && !compositeIsOperator(artifact.name)) {
        it.remove();
      }
    }
  }

  /** Given a composite head, returns true if the composite is public, otherwise false. */
  private boolean isPublic(CompositeHeadType head) {
    CompositeModifiersType modifiers = head.getModifiers();
    if (modifiers != null) {
      for (CompositeModifierType modifier : modifiers.getModifier()) {
        if (modifier.getName() == CompositeModifierEnumType.PUBLIC) {
          return true;
        }
      }
    }
    return false;
  }

  /**
   * Given a composite unit (SPL file), returns true if the composite unit contains public
   * components, or public and private components, if private components are to be included,
   * otherwise false.
   */
  private boolean hasContents(
      CompilationUnitType compUnit, Map<String, TreeMap<String, TypeDefinitionType>> staticTypes) {
    DefinitionsType defs = compUnit.getDefinitions();
    if (defs == null) {
      // No contents.
      return false;
    }
    for (CompositeDefinitionType composite : defs.getCompositeDefinition()) {
      CompositeHeadType head = composite.getCompositeHead();
      // Contains composites.
      if (isPublic(head) || inputParameters.isIncludePrivateArtifacts()) {
        return true;
      }
    }
    if (!staticTypes.isEmpty()) {
      // Contains static types.
      return true;
    }
    for (FunctionDefinitionType function : defs.getFunctionDefinition()) {
      FunctionHeadType head = function.getFunctionHead();
      if (isPublic(head) || inputParameters.isIncludePrivateArtifacts()) {
        // Contains functions.
        return true;
      }
    }
    if (!defs.getTypeDefinition().isEmpty()) {
      // Contains standalone types.
      return true;
    }
    // No included contents.
    return false;
  }

  /** Given an SPL function head, returns true if the composite is public, otherwise false. */
  private boolean isPublic(FunctionHeadType head) {
    FunctionModifiersType modifiers = head.getModifiers();
    if (modifiers != null) {
      for (FunctionModifierType modifier : modifiers.getModifier()) {
        if (modifier.getName() == FunctionModifierEnumType.PUBLIC) {
          return true;
        }
      }
    }
    return false;
  }

  /** Return the starting index of a function name in a function prototype. */
  private int getFunctionNameIndexInPrototype(String prototype) {
    int ix = prototype.lastIndexOf('(');
    if (ix == -1) {
      ix = prototype.length();
    }
    // moving right-to-left, skip spaces between ( and end of function name
    while (ix > 0 && prototype.charAt(ix - 1) == ' ') ix--;
    // moving from right-to-left, decrement index until we get to beginning of function name.
    while (ix > 0 && prototype.charAt(ix - 1) != ' ') ix--;
    return ix;
  }

  /**
   * Given a prototype of a function, return the function's signature. Currently this is used only
   * for sorting the function prototypes.
   *
   * <p>For example, given prototype: <complex T> public T ln (T f) return: ln (T f)
   */
  private String getFunctionSignature(String prototype) {
    if (prototype == null || prototype.length() == 0) {
      return null;
    }
    return prototype.substring(getFunctionNameIndexInPrototype(prototype));
  }

  /** Given a ToolkitFunctionType, return the function signature used for output. */
  private String getFunctionSignature(ToolkitFunctionType function) {
    StringBuilder sb = new StringBuilder();
    sb.append(function.getName() + "(");
    boolean first = true;
    for (com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType parameter : function.getParameter()) {
      if (!first) {
        sb.append(", ");
      } else {
        first = false;
      }
      // Remove unnecessary spaces within types and add space after comma.
      String type = parameter.getType();
      type = type.replaceAll("\\s*([,{}\\[\\]<>])\\s*", "$1");
      type = type.replaceAll(",", ", ");
      // Try to add spaces to long, unbroken strings to allow output to wrap.
      String s = sb.toString();
      int slen = s.length();
      int ix = s.lastIndexOf(' ');
      if (ix >= 0) {
        slen = slen - (ix + 1);
      }
      int tlen = type.length();
      ix = type.indexOf(' ');
      if (ix >= 0) {
        tlen = ix;
      }
      if (slen + tlen > Constants.MaxCharForLineWrap) {
        sb.append(" ");
      }
      sb.append(type);
    }
    sb.append(")");
    return sb.toString();
  }

  /** Given the URI for a function model file, return the namespace. */
  private String getNamespaceForFunctionModel(String fileUri) {
    String dir = new File(fileUri).getParentFile().getParent();
    return (dir == null ? "" : dir).replace(File.separatorChar, '.');
  }

  /** Given the URI for a function model file, return the file name. */
  private String getNameForFunctionModel(String fileUri) {
    return new File(fileUri).getName();
  }

  /**
   * Given a list of composites, return a map containing the set of static types for each composite.
   */
  private Map<String, TreeMap<String, TypeDefinitionType>> getStaticTypes(
      EList<CompositeDefinitionType> composites) {
    Map<String, TreeMap<String, TypeDefinitionType>> staticTypes =
        new HashMap<String, TreeMap<String, TypeDefinitionType>>();
    for (CompositeDefinitionType composite : composites) {
      CompositeBodyType compBody = composite.getCompositeBody();
      if (compBody == null) {
        continue;
      }
      CompositeHeadType head = composite.getCompositeHead();
      if (!isPublic(head) && !inputParameters.isIncludePrivateArtifacts()) {
        // Do not include a private composite's components. */
        continue;
      }
      CompositeTypesType types = compBody.getTypes();
      if (types != null) {
        // Create a map of static types for this composite.
        // Note that this map of types is sorted by type name.
        TreeMap<String, TypeDefinitionType> typesMap = new TreeMap<String, TypeDefinitionType>();
        for (TypeDefinitionType type : types.getType()) {
          TypeModifiersType modifiers = type.getModifiers();
          if (modifiers != null) {
            for (TypeModifierType modifier : modifiers.getModifier()) {
              if ("static".equals(modifier.getName().getLiteral())) {
                typesMap.put(type.getName(), type);
              }
            }
          }
        }
        if (!typesMap.isEmpty()) {
          staticTypes.put(head.getName(), typesMap);
        }
      }
    }
    return staticTypes;
  }

  /** A common class for locating information in a source file for use in user messages. */
  class LocationType {
    String fileName = null;
    String itemName = null;
    BigInteger initialValue = new BigInteger("-1");
    BigInteger startLine = initialValue;
    BigInteger endLine = initialValue;
    BigInteger startColumn = initialValue;
    BigInteger endColumn = initialValue;
    EObject modelObject = null;

    LocationType(CompositeDefinitionType composite) {
      CompositeHeadType head = composite.getCompositeHead();
      assert (head != null);
      startLine = head.getLine();
      endLine = composite.getEndLine();
      startColumn = head.getColumn();
      modelObject = composite;
    }

    LocationType(CompositeDefinitionType composite, String fileName) {
      this(composite);
      setFileName(fileName);
    }

    LocationType(FunctionDefinitionType function) {
      FunctionHeadType head = function.getFunctionHead();
      assert (head != null);
      startLine = head.getLine();
      // TODO: Remove this hack when compiler fixed to provide
      // correct endLine
      endLine = function.getEndLine().add(new BigInteger("1"));
      startColumn = head.getColumn();
      modelObject = function;
    }

    LocationType(FunctionDefinitionType function, String fileName) {
      this(function);
      setFileName(fileName);
    }

    LocationType(TypeDefinitionType type) {
      startLine = type.getStartLine();
      endLine = type.getEndLine();
      startColumn = type.getStartColumn();
      modelObject = type;
    }

    LocationType(TypeDefinitionType type, String fileName) {
      this(type);
      setFileName(fileName);
    }

    // Not used.
    LocationType(OperatorInvocationType operator) {
      OperatorInvocationHeadType head = operator.getOperatorInvocationHead();
      assert (head != null);
      startLine = head.getLine();
      startColumn = head.getColumn();
      modelObject = operator;
    }

    LocationType(OperatorInvocationType operator, String fileName) {
      this(operator);
      setFileName(fileName);
    }

    LocationType(OperatorInvocationParameterType parameter) {
      startLine = parameter.getLine();
      startColumn = parameter.getColumn();
      modelObject = parameter;
    }

    LocationType(OperatorInvocationParameterType parameter, String fileName) {
      this(parameter);
      setFileName(fileName);
    }

    LocationType(String itemName, String fileName) {
      this.itemName = itemName;
      setFileName(fileName);
    }

    LocationType(Artifact artifact, String fileName) {
      this(artifact.toShortString(), fileName);
    }

    /**
     * Allow the specified location file name to be an absolute path or a path relative to the
     * current toolkit root directory.
     */
    private void setFileName(String fileName) {
      File file = new File(fileName);
      if (!file.isAbsolute()) file = new File(currentToolkit.getToolkitPath(), fileName);
      this.fileName = file.toString();
    }

    /**
     * Returns the location as a string using format: <file name>:<start line>:<start column> Where
     * the file name and start column may be empty
     */
    public String toString() {
      String loc_string = "";
      if (fileName != null) {
        loc_string += fileName;
      }
      if (startLine.intValue() != -1) {
        loc_string += ":" + startLine;
      }
      if (startColumn.intValue() != -1) {
        loc_string += ":" + startColumn;
      }
      if (itemName != null) {
        loc_string += ":" + itemName;
      }
      assert (loc_string.length() > 0);
      return loc_string;
    }

    public BigInteger getStartLine() {
      return startLine;
    }

    public BigInteger getEndLine() {
      return endLine;
    }

    public BigInteger getStartColumn() {
      return startColumn;
    }

    public BigInteger getEndColumn() {
      return endColumn;
    }

    public EObject getModelObject() {
      return modelObject;
    }
  }

  static final String CDATAClose = "]]>";
  static final String EscapedCDATAClose = "]]]]><![CDATA[>";

  /**
   * Given a character sequence containing the closing markup for an XML CDATA section, escape the
   * closing markup.
   *
   * <p>When contained in a CDATA section, the closing markup for a CDATA section ("]]>") must be
   * escaped so that it won't be treated as the end of the enclosing CDATA section.
   *
   * <p>For example, given the following SPL source code:
   *
   * <p>mutable tuple<XMLFrag> f = {}; f.xmlfrag = "]]><morestuff>stuff</morestuff>";
   *
   * <p>the SPLDOC generator will include this code in a CDATA section in the generated DITA
   * document. This will yield an XML parsing error, as the "]]>" in the code will be treated as a
   * closing of the CDATA section.
   *
   * <p>This method resolves the parsing error by replacing each occurrence of "]]>" in the source
   * code with "]]]]><![CDATA[>". This breaks the ]]> into two CDATA sections. For example, the
   * above source code would become:
   *
   * <p>mutable tuple<XMLFrag> f = {}; f.xmlfrag = "]]]]><![CDATA[><morestuff>stuff</morestuff>";
   *
   * @param text
   * @return
   */
  static String escapeCDATACloseMarkup(String text) {
    if (text == null || text.length() < CDATAClose.length()) {
      return text;
    }
    return text.replaceAll(CDATAClose, EscapedCDATAClose);
  }

  /**
   * Given the name of an SPL file and an object containing starting and ending line numbers, return
   * a string containing the source code.
   *
   * @param splFileName
   * @param sourceLocation
   * @return code extract
   */
  // TODO: Add support for columns
  private String getIncludeCode(String splFileName, LocationType sourceLocation) {
    // HashMap<BigInteger, LocationType> anchorMap = getAnchorMap(composite);
    StringBuffer splCompositeCode = new StringBuffer();
    try {
      File splFile = new File(splFileName);
      if (!splFile.exists() || !splFile.canRead()) {
        String[] args = {splFileName};
        logger.printError(Key.CDISP7009E, args);
        return null;
      }
      FileInputStream fileStream = new FileInputStream(splFileName);
      DataInputStream dataStream = new DataInputStream(fileStream);
      InputStreamReader inputReader = new InputStreamReader(dataStream);
      BufferedReader bufferedReader = new BufferedReader(inputReader);
      int startLine = sourceLocation.getStartLine().intValue();
      int endLine = sourceLocation.getEndLine().intValue();
      String splLine;
      int currentLine = 0;
      while ((splLine = bufferedReader.readLine()) != null) {
        if (++currentLine >= startLine) {
          if (currentLine == startLine) {
            // If it's the first line, read from the start column.
            int startCol = sourceLocation.getStartColumn().intValue();
            if (startCol > -1) {
              if (startCol - 1 < splLine.length()) {
                // The 1st column is startCol == 1 in the source model
                splLine = splLine.substring(startCol - 1);
              } else {
                String[] args = {startCol + "", currentLine + "", splLine.length() + ""};
                logger.printError(Key.CDISP7014E, splFileName, args);
              }
            }
          }
          if (currentLine <= endLine) {
            if (currentLine == endLine) {
              // if it's the end line, read up to and including the end column.
              int endCol = sourceLocation.getEndColumn().intValue();
              if (endCol > -1) {
                // The 1st column is startCol == 1 and the last column == splLine.length()
                if (endCol - 1 < splLine.length()) {
                  splLine = splLine.substring(0, endCol);
                } else {
                  String[] args = {endCol + "", currentLine + "", splLine.length() + ""};
                  logger.printError(Key.CDISP7015E, splFileName, args);
                }
              }
            }

            splCompositeCode.append(" ");
            // LocationType sourceLoc;
            // if ((sourceLoc = anchorMap.get(new BigInteger(String.valueOf(currentLine)))) != null)
            // {
            //   splCompositeCode.append("<span id=\"" + sourceLoc.getStartLine() + "\"" + ">" +
            // "</span>");
            // }
            splCompositeCode.append(splLine + "\n");

          } else {
            break;
          }
        }
      }
      bufferedReader.close();
      inputReader.close();
      dataStream.close();
      fileStream.close();
    } catch (IOException e) {
      logger.printException(e);
      return null;
    }
    // anchorMaps.put(composite, anchorMap);
    return escapeCDATACloseMarkup(splCompositeCode.toString());
  }

  /**
   * Given the name of an SPL file and an SPL function model object, return the function's code.
   *
   * @param splFileName
   * @param SPL function model object
   * @return string containing function's source code
   */
  private String getIncludeCode(String splFileName, FunctionDefinitionType function) {
    LocationType sourceLocation = new LocationType(function);
    return getIncludeCode(splFileName, sourceLocation);
  }

  /**
   * Given the name of an SPL file and a composite model object, return the composite code.
   *
   * @param splFileName
   * @param composite
   * @return
   */
  private String getIncludeCode(String splFileName, CompositeDefinitionType composite) {
    LocationType sourceLocation = new LocationType(composite);
    return getIncludeCode(splFileName, sourceLocation);
  }

  /**
   * Given an SPL function head, return a function prototype
   *
   * @param functionHead
   * @return HTML entity encoded string containing a function prototype
   */
  private String formatFunctionPrototype(FunctionHeadType functionHead) {
    StringBuilder prototype = new StringBuilder();
    prototype.append(functionHead.getReturnType() + " ");
    prototype.append(functionHead.getName() + "(");
    FunctionParametersType parameters = functionHead.getParameters();
    if (parameters != null) {
      boolean first = true;
      for (FunctionParameterType parameter : parameters.getParameter()) {
        if (!first) {
          prototype.append(", ");
        } else {
          first = false;
        }
        prototype.append(parameter.getType() + " " + parameter.getName());
      }
    }
    prototype.append(")");
    return Encoder.HTMLEntityEncode(prototype.toString());
  }

  /**
   * Generate a path that shows the containers for a toolkit artifact or index document: <Toolkits>
   * [-> <Samples>] [-> <Toolkit>] [-> <Namespace>] -> <label> where the containers are formatted as
   * links for navigation.
   *
   * <p>The label can be a document title, an SPL artifact name, or a sub-path with multiple levels.
   * The toolkit and namespace should be null when they are not in the path leading to the label.
   * Set isSample to true to include the samples container.
   */
  private String generatePathSection(
      Toolkit toolkit, boolean isSample, String namespace, String label) {
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append("<section>\n");
    ditaDoc.append("<p>\n");
    ditaDoc.append(createDitaLinkToolkits());
    ditaDoc.append(" &gt; ");
    if (isSample) {
      ditaDoc.append(createDitaLinkSamples());
      ditaDoc.append(" &gt; ");
    }
    // the final element in the path represents the current page and is not a link
    if (toolkit == null) {
      ditaDoc.append(label);
    } else {
      ditaDoc.append(createDitaLinkToolkit(toolkit));
      ditaDoc.append(" &gt; ");
      if (namespace == null) {
        ditaDoc.append(label);
      } else {
        ditaDoc.append(createDitaLinkNamespace(toolkit, namespace));
        ditaDoc.append(" &gt; ");
        ditaDoc.append(label);
      }
    }
    ditaDoc.append("</p>\n");
    ditaDoc.append("</section>\n");
    return ditaDoc.toString();
  }

  private String generatePathSection(Toolkit toolkit, String namespace, String label) {
    return generatePathSection(toolkit, toolkit.isSample(), namespace, label);
  }

  private String generatePathSectionSubPath(String parentSubPath, String childSubPath) {
    return parentSubPath + " &gt; " + childSubPath;
  }

  private String generateSamplesPathSection() {
    return generatePathSection(
        null, false, null, logger.getMessage(Messages.MSG_TITLE_SAMPLE_TOOLKITS));
  }

  private String generateToolkitPathSection() {
    return generatePathSection(
        null, currentToolkit.isSample(), null, getToolkitLabel(currentToolkit));
  }

  private String generateNamespacePathSection(String namespace) {
    return generatePathSection(currentToolkit, null, getNamespaceLabel(namespace));
  }

  private String generateArtifactPathSection(String namespace, String name) {
    return generatePathSection(currentToolkit, namespace, name);
  }

  private String generateChildDocPathSection(
      Artifact artifact, ChildDocument childDoc, String childSubPath) {
    ChildDocument parentDoc = childDoc.parent;
    if (parentDoc.parent == null) {
      // The parent is the main document for the artifact.
      if (artifact.type == ArtifactType.Toolkit) {
        return generatePathSection(artifact.toolkit, null, childSubPath);
      } else if (artifact.type == ArtifactType.Namespace) {
        return generatePathSection(artifact.toolkit, artifact.name, childSubPath);
      } else if (artifact.type == ArtifactType.Operator
          && artifact.subtype == ArtifactSubType.Primitive) {
        childSubPath =
            generatePathSectionSubPath(
                createDitaLinkOp(artifact.toolkit, artifact.namespace, artifact.name),
                childSubPath);
        return generatePathSection(artifact.toolkit, artifact.namespace, childSubPath);
      } else if (artifact.type == ArtifactType.CompilationUnit) {
        childSubPath =
            generatePathSectionSubPath(
                createDitaLinkSplFile(artifact.toolkit, artifact.namespace, artifact.uriValue),
                childSubPath);
        return generatePathSection(artifact.toolkit, artifact.namespace, childSubPath);
      } else {
        logger.printError(
            Key.CDISP7124E,
            new String[] {"unexpected toolkit artifact type for a path section: " + artifact.type});
        ErrorManager.throwFatalErrorException();
        return null;
      }
    }
    String parentLink =
        createDitaLink(createDitaLinkRef(artifact.toolkit, parentDoc.filename), parentDoc.title);
    String parentSubPath = generatePathSectionSubPath(parentLink, childSubPath);
    return generateChildDocPathSection(artifact, parentDoc, parentSubPath);
  }

  public String generateChildDocPathSection(Artifact artifact, String filename) {
    ChildDocument childDoc =
        ditaChildDocs.get(FileOutput.getDitaPathName(artifact.toolkit, filename));
    return generateChildDocPathSection(artifact, childDoc, childDoc.title);
  }

  /**
   * Return a topicref element for a document, with optional: - enable the automatic inclusion of
   * links to and from related documents. This element is used in the DITA map to reference each SPL
   * artifact document.
   */
  private String getTopicRefStart(String ditaLinkRef, String label, boolean allowLinks) {
    return "<topicref"
        + (label != null ? " navtitle=\"" + label + "\"" + " locktitle=\"yes\"" : "")
        + " href=\""
        + ditaLinkRef
        + "\""
        + " toc=\"yes\""
        + " linking=\""
        + (allowLinks ? "normal" : "none")
        + "\">"
        + "\n";
  }

  private String getTopicRefStart(String ditaLinkRef) {
    return getTopicRefStart(ditaLinkRef, null, false);
  }

  private String getTopicRefStart(String ditaLinkRef, String label) {
    return getTopicRefStart(ditaLinkRef, label, false);
  }

  private String getTopicRefStartAllowLinks(String ditaLinkRef, String label) {
    return getTopicRefStart(ditaLinkRef, label, true);
  }

  private String getTopicRefEnd() {
    return "</topicref>" + "\n";
  }

  /**
   * Generates a set of nested topic references for any child documents of the document with the
   * given file name in the given toolkit, and their descendants.
   */
  private void generateDescendantTopicRefs(
      StringBuilder ditaDoc, Toolkit toolkit, String filename) {
    ChildDocument parentDoc = ditaChildDocs.get(FileOutput.getDitaPathName(toolkit, filename));
    if (parentDoc == null) {
      return;
    }
    for (ChildDocument childDoc : parentDoc.children) {
      ditaDoc.append(getTopicRefStart(createDitaLinkRef(toolkit, childDoc.filename)));
      generateDescendantTopicRefs(ditaDoc, toolkit, childDoc.filename);
      ditaDoc.append(getTopicRefEnd());
    }
  }

  /**
   * Returns a topic reference for the toolkit artifact index of the specified type, or null, if
   * none. If the toolkit is set to null, the index is either for all the main or, if isSample is
   * true, all the sample toolkits.
   */
  private String getIndexTopicRef(Toolkit toolkit, ArtifactType type, boolean isSample) {
    String linkRef = getIndexLinkRef(toolkit, type, isSample);
    if (linkRef == null) {
      return null;
    }
    return getTopicRefStart(linkRef, getListLabel(type)) + getTopicRefEnd();
  }

  /**
   * Generates the topic references for the toolkit artifact indexes. If the toolkit is set to null,
   * the indexes are either for all the main or, if isSample is true, all the sample toolkits.
   */
  private void generateIndexTopicRefs(StringBuilder ditaDoc, Toolkit toolkit, boolean isSample) {
    String topicRef;
    if ((topicRef = getIndexTopicRef(toolkit, ArtifactType.Namespace, isSample)) != null) {
      ditaDoc.append(topicRef);
    }
    if ((topicRef = getIndexTopicRef(toolkit, ArtifactType.Operator, isSample)) != null) {
      ditaDoc.append(topicRef);
    }
    if ((topicRef = getIndexTopicRef(toolkit, ArtifactType.Function, isSample)) != null) {
      ditaDoc.append(topicRef);
    }
    if ((topicRef = getIndexTopicRef(toolkit, ArtifactType.Type, isSample)) != null) {
      ditaDoc.append(topicRef);
    }
  }

  /**
   * Add a DITA document that is a child of another document.
   *
   * <p>The child document will be written to a file with the given name, and shown in the table of
   * contents, using the given title, as a child of the parent document in the given toolkit with
   * the given file name.
   *
   * <p>The contents of the child document should be set using setChildDocumentContent().
   */
  public void addChildDocument(
      Toolkit toolkit, String filename, String title, String parentFilename) {
    String childPath = FileOutput.getDitaPathName(toolkit, filename);
    String parentPath = FileOutput.getDitaPathName(toolkit, parentFilename);
    ChildDocument parentDoc = ditaChildDocs.get(parentPath);
    if (parentDoc == null) {
      // Add the parent as the root of this tree of documents.
      parentDoc = new ChildDocument(parentFilename, null, null);
      ditaChildDocs.put(parentPath, parentDoc);
    }
    ChildDocument childDoc = new ChildDocument(filename, title, parentDoc);
    ditaChildDocs.put(childPath, childDoc);
    parentDoc.children.add(childDoc);
  }

  /** Set the contents of a child document added with addChildDocument(). */
  public void setChildDocumentContent(Toolkit toolkit, String filename, String ditaDoc) {
    ditaPages.put(FileOutput.getDitaPathName(toolkit, filename), ditaDoc);
  }

  /**
   * Generates the topic references for either all the main or, if isSample is true, all the sample
   * toolkits.
   */
  private void generateToolkitsTopicRefs(StringBuilder ditaDoc, boolean isSample) {
    List<Artifact> tkArtifactList = toolkitMgr.getArtifactsByType(ArtifactType.Toolkit, isSample);
    if (tkArtifactList.isEmpty()) {
      return;
    }
    // List indexes for the toolkits.
    generateIndexTopicRefs(ditaDoc, null, isSample);
    // List the toolkits in sorted order.
    Collections.sort(tkArtifactList);
    for (Artifact tkArtifact : tkArtifactList) {
      // List the toolkit.
      ditaDoc.append(
          getTopicRefStart(createDitaLinkRef(tkArtifact), getToolkitLabel(tkArtifact.toolkit)));
      // List any descendant description pages.
      generateDescendantTopicRefs(
          ditaDoc, tkArtifact.toolkit, FileOutput.getDocFileNameToolkit(tkArtifact.toolkit));
      // List indexes per toolkit.
      generateIndexTopicRefs(ditaDoc, tkArtifact.toolkit, isSample);
      // List namespaces per toolkit in sorted order.
      List<Artifact> nsArtifactList =
          toolkitMgr.getArtifactsByToolkitAndType(tkArtifact.toolkit, ArtifactType.Namespace);
      Collections.sort(nsArtifactList);
      for (Artifact nsArtifact : nsArtifactList) {
        // List the namespace.
        ditaDoc.append(
            getTopicRefStart(createDitaLinkRef(nsArtifact), getNamespaceLabel(nsArtifact.name)));
        // List any descendant description pages.
        generateDescendantTopicRefs(
            ditaDoc, tkArtifact.toolkit, FileOutput.getDocFileNameNamespace(nsArtifact.name));
        // List primitive operator pages for this namespace in sorted order.
        List<Artifact> opArtifactList =
            toolkitMgr.getArtifactsByToolkitNamespaceTypeAndSubType(
                tkArtifact.toolkit,
                nsArtifact.name,
                ArtifactType.Operator,
                ArtifactSubType.Primitive);
        Collections.sort(opArtifactList);
        for (Artifact opArtifact : opArtifactList) {
          // List the page.
          ditaDoc.append(getTopicRefStart(createDitaLinkRef(opArtifact), opArtifact.name));
          // List any descendant description pages.
          generateDescendantTopicRefs(
              ditaDoc, tkArtifact.toolkit, FileOutput.getDocFileNameArtifact(opArtifact));
          ditaDoc.append(getTopicRefEnd());
        }
        // List any C++ function model page for this namespace.
        List<Artifact> fcArtifactList =
            toolkitMgr.getArtifacts(
                tkArtifact.toolkit,
                nsArtifact.name,
                FileOutput.CppFunctionModelFileName,
                ArtifactType.FunctionModel);
        for (Artifact fcArtifact : fcArtifactList) {
          // List the page.
          ditaDoc.append(
              getTopicRefStart(
                  createDitaLinkRef(fcArtifact), getFunctionModelLabel(fcArtifact.uriValue)));
          // List any descendant description pages.
          generateDescendantTopicRefs(
              ditaDoc, tkArtifact.toolkit, FileOutput.getDocFileNameArtifact(fcArtifact));
          ditaDoc.append(getTopicRefEnd());
        }
        // List any Java function model page for this namespace.
        List<Artifact> fjArtifactList =
            toolkitMgr.getArtifacts(
                tkArtifact.toolkit,
                nsArtifact.name,
                FileOutput.JavaFunctionModelFileName,
                ArtifactType.FunctionModel);
        for (Artifact fjArtifact : fjArtifactList) {
          // List the page.
          ditaDoc.append(
              getTopicRefStart(
                  createDitaLinkRef(fjArtifact), getFunctionModelLabel(fjArtifact.uriValue)));
          // List any descendant description pages.
          generateDescendantTopicRefs(
              ditaDoc, tkArtifact.toolkit, FileOutput.getDocFileNameArtifact(fjArtifact));
          ditaDoc.append(getTopicRefEnd());
        }
        // List SPL file pages in this namespace in sorted order.
        List<Artifact> splArtifactList =
            toolkitMgr.getArtifactsByToolkitNamespaceTypeAndSubType(
                tkArtifact.toolkit,
                nsArtifact.name,
                ArtifactType.CompilationUnit,
                ArtifactSubType.None);
        Collections.sort(splArtifactList);
        for (Artifact splArtifact : splArtifactList) {
          // List the page.
          ditaDoc.append(
              getTopicRefStart(
                  createDitaLinkRef(splArtifact), getSPLFileLabel(splArtifact.uriValue)));
          // List any descendant description pages.
          generateDescendantTopicRefs(
              ditaDoc, tkArtifact.toolkit, FileOutput.getDocFileNameArtifact(splArtifact));
          ditaDoc.append(getTopicRefEnd());
        }
        ditaDoc.append(getTopicRefEnd());
      }
      ditaDoc.append(getTopicRefEnd());
    }
  }

  /**
   * Generate the DITA map for the generated DITA documents.
   *
   * <p>The DITA map defines the table of contents for the set of documents.
   */
  private void generateMap() {
    buildingDitaMap = true;
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append(Constants.XHTML_Header_Map);
    ditaDoc.append("<map id=\"" + Constants.RefId_Map + "\"" + ">" + "\n");
    ditaDoc.append("<title>" + docTitle + "</title>" + "\n");

    // Root element of structured table of contents.
    // The document title linked to the all toolkits document.
    ditaDoc.append(
        getTopicRefStartAllowLinks(
            createDitaLinkRef(FileOutput.AllToolkitsFileName, false), docTitle));

    // List the main toolkits.
    generateToolkitsTopicRefs(ditaDoc, false);

    // List the sample toolkits.
    if (hasSamples) {
      ditaDoc.append(
          getTopicRefStart(
              createDitaLinkRef(FileOutput.AllToolkitsFileName, true),
              logger.getMessage(Messages.MSG_TITLE_SAMPLE_TOOLKITS)));
      generateToolkitsTopicRefs(ditaDoc, true);
      ditaDoc.append(getTopicRefEnd());
    }

    // List the reference documents.
    if (!inputParameters.isSuppressReferenceDocs()) {
      if (toolkitsHavePrimitiveOperators) {
        ditaDoc.append(
            getTopicRefStart(
                FileOutput.getDitaPathNameReference(References.OperatorModelRefFileName)
                    + "."
                    + FileOutput.DitaFileExt,
                logger.getMessage(Messages.MSG_TITLE_OPERATOR_MODEL_DOC)));
        ditaDoc.append(getTopicRefEnd());
      }
      if (toolkitsHaveNativeFunctions) {
        ditaDoc.append(
            getTopicRefStart(
                FileOutput.getDitaPathNameReference(References.FunctionModelRefFileName)
                    + "."
                    + FileOutput.DitaFileExt,
                logger.getMessage(Messages.MSG_TITLE_FUNCTION_MODEL_DOC)));
        ditaDoc.append(getTopicRefEnd());
      }
    }

    ditaDoc.append(getTopicRefEnd());
    ditaDoc.append("</map>\n");
    ditaPages.put(FileOutput.DitaMapName, ditaDoc.toString());
    buildingDitaMap = false;
  }

  // The description for a namespace is supported by allowing the user to provide
  // an SPL file in that namespace that contains only a description for the namespace
  // and does not define any other artifacts, and is named "namespace-info.spl".
  // We record these SPL files so they are not listed in the generated documentation.
  // We also map the namespace to the description file for that namespace.
  HashSet<SourceFileType> namespaceDescFiles = null;
  HashMap<String, SourceFileType> namespaceDescFileMap = null;

  /** Given a source model for the toolkit, find the namespace description files, if any. */
  private void setNamespaceDescFiles(SourceModelType sourceModel) {
    namespaceDescFiles = new HashSet<SourceFileType>();
    namespaceDescFileMap = new HashMap<String, SourceFileType>();
    for (SourceFileType sourceFile : sourceModel.getSourceFile()) {
      CompilationUnitType compUnit = sourceFile.getCompilationUnit();
      SplNamespaceType namespaceType = compUnit.getSplNamespace();
      String namespace;
      if (namespaceType == null) {
        // default namespace
        namespace = "";
      } else {
        namespace = namespaceType.getName();
      }
      DefinitionsType defs = compUnit.getDefinitions();
      if (defs == null
          && new File(sourceFile.getUri()).getName().equals(FileOutput.NamespaceDescFileName)) {
        // There are no artifacts, the file name is correct, so this is a namespace description
        // file.
        namespaceDescFiles.add(sourceFile);
        namespaceDescFileMap.put(namespace, sourceFile);
      }
    }
  }

  /**
   * Given an SPLDOC tag and a list of known tags, return true if the tag is known, and false
   * otherwise.
   */
  private boolean isKnownTag(String tag, String[] knownTagList) {
    for (String knownTag : knownTagList) {
      if (knownTag.equals(tag)) {
        return true;
      }
    }
    return false;
  }

  /**
   * Performs general processing prior to document generation of an annotation with the given name,
   * and given set of known non-standard tag names, at the given location for the given artifact.
   */
  private void processAnnotation(
      String name,
      String[] knownTags,
      Artifact artifact,
      LocationType location,
      SPLDOCAnnotation annotation) {
    if (inputParameters.isCheckTags()) {
      // Output a warning if the annotation tag is not known.
      if (!isKnownTag(name, StandardAnnotationTags)
          && (knownTags != null && !isKnownTag(name, knownTags))) {
        if (annotation != null) {
          printAnnotationWarn(
              Key.CDISP7085W, location, annotation, new String[] {name, artifact.toString()});
        } else {
          logger.printWarn(
              Key.CDISP7085W, location.toString(), new String[] {name, artifact.toString()});
        }
      }
    }
    if (!inputParameters.isIncludeExcludedArtifacts()) {
      if (name.equals("exclude")) {
        // The artifact is explicitly excluded from the generated documentation.
        toolkitMgr.excludeArtifact(artifact);
      } else if (name.equals("include")) {
        // The artifact is explicitly included in the generated documentation.
        toolkitMgr.includeArtifact(artifact);
      }
    }
    if (name.equals("sample") && artifact.type == ArtifactType.Toolkit) {
      // This is a sample toolkit containing sample artifacts.
      artifact.toolkit.setAsSample();
    }
  }

  /**
   * Performs general processing prior to document generation of the description in the given SPLDOC
   * input at the given location in the given SPL file for the given artifact.
   */
  private void processDescription(
      SplDocType splDoc, Artifact artifact, String fileUri, LocationType location) {
    if (splDoc == null) {
      return;
    }
    // Add description to the description map.
    SplDocDescriptionType descriptionType = splDoc.getDescription();
    if (descriptionType != null) {
      addArtifactDescription(artifact, descriptionType.getDescription(), fileUri);
    }
    // Process annotations.
    String[] knownTags = null;
    switch (artifact.type) {
      case Function:
        knownTags = SPLFunctionAnnotationTags;
        break;
      case Operator:
        knownTags = CompositeAnnotationTags;
        break;
      default:
        break;
    }
    for (SplDocAnnotationType annotation : splDoc.getAnnotation()) {
      processAnnotation(annotation.getName(), knownTags, artifact, location, null);
    }
  }

  /**
   * Performs general processing prior to document generation of the given SPLDOC description at the
   * given location in the given source file for the given artifact.
   */
  private void processDescription(
      String description, Artifact artifact, String fileUri, LocationType location) {
    if (description == null) {
      return;
    }
    // Add description to the description map.
    addArtifactDescription(artifact, description, fileUri);
    // Process annotations.
    String[] knownTags = null;
    switch (artifact.type) {
      case Function:
        knownTags = NativeFunctionAnnotationTags;
        break;
      case Toolkit:
        knownTags = ToolkitAnnotationTags;
        break;
      default:
        break;
    }
    for (SPLDOCAnnotation annotation : ParserSPLDOC.parseAnnotations(description)) {
      if (annotation.hasError()) {
        switch (annotation.getErrorType()) {
          case NoTagNameError:
            printAnnotationError(
                Key.CDISP7086E, location, annotation, new String[] {artifact.toString()});
            break;
          case NoTargetError:
            printAnnotationError(
                Key.CDISP7087E,
                location,
                annotation,
                new String[] {annotation.name, artifact.toString()});
            break;
          default:
            printAnnotationError(
                Key.CDISP7124E,
                location,
                annotation,
                new String[] {
                  "unexpected syntax error in SPLDOC annotation tag for artifact "
                      + artifact.toString()
                });
            break;
        }
      } else {
        processAnnotation(annotation.name, knownTags, artifact, location, annotation);
      }
    }
  }

  /**
   * Performs general processing of the given toolkit prior to document generation.
   *
   * <p>This processing includes processing SPLDOC descriptions and determining which composite
   * operators have graphs and are therefore to be included in the various operator indexes.
   *
   * <p>The SPLDOC descriptions include summary sentences and annotations. Processing the
   * annotations determines if any artifacts are to be excluded from the generated documentation.
   * This needs to be done prior to generating the documentation in order to prevent generating a
   * link to an excluded artifact.
   */
  private void process(Toolkit toolkit) {
    // Process the toolkit.
    DescriptionType descriptionType;
    SplDocType splDoc;
    Artifact artifact;
    currentToolkit = toolkit;
    ToolkitType toolkitModel = toolkit.getToolkitModel();
    if ((descriptionType = toolkitModel.getDescription()) != null) {
      artifact =
          getKnownArtifact(
              toolkit, null, toolkit.getToolkitModel().getName(), ArtifactType.Toolkit);
      if (artifact != null) {
        String fileUri = FileOutput.ToolkitModelFileName;
        processDescription(
            descriptionType.getValue(), artifact, fileUri, new LocationType(artifact, fileUri));
      }
    }
    if (toolkit.isSample()) {
      hasSamples = true;
    }
    // Process the SPL files and their contents.
    SourceModelType sourceModel = LoadModel.loadSourceModel(toolkit);
    if (sourceModel == null) {
      ErrorManager.throwFatalErrorException();
    }
    setNamespaceDescFiles(sourceModel);
    EList<SourceFileType> sourceFiles = sourceModel.getSourceFile();
    for (SourceFileType sourceFile : sourceFiles) {
      CompilationUnitType compUnit = sourceFile.getCompilationUnit();
      DefinitionsType defs = compUnit.getDefinitions();
      boolean isNamespaceDescFile = namespaceDescFiles.contains(sourceFile);
      Map<String, TreeMap<String, TypeDefinitionType>> staticTypes = null;
      if (!isNamespaceDescFile) {
        if (defs == null) {
          // Do not include SPL files with no artifacts.
          continue;
        }
        staticTypes = getStaticTypes(defs.getCompositeDefinition());
        if (!hasContents(compUnit, staticTypes)) {
          // Do not include SPL files with no artifacts to be shown.
          continue;
        }
      }
      SplNamespaceType namespaceType = compUnit.getSplNamespace();
      String namespace = (namespaceType == null ? "" : namespaceType.getName());
      String fileUri = sourceFile.getUri();
      splDoc = compUnit.getSplDoc();
      if (splDoc != null) {
        if (isNamespaceDescFile) {
          // Process descriptions for the namespace described by the SPL file.
          // The namespace will not exist and will not be documented if it contains no artifacts.
          artifact = toolkitMgr.getArtifact(toolkit, null, namespace, ArtifactType.Namespace);
        } else {
          // Process descriptions for the SPL file.
          artifact =
              getKnownArtifact(
                  toolkit, namespace, new File(fileUri).getName(), ArtifactType.CompilationUnit);
        }
        if (artifact != null) {
          processDescription(splDoc, artifact, fileUri, new LocationType(artifact, fileUri));
        }
      }
      if (isNamespaceDescFile) {
        // A namespace description file has no artifact definitions.
        continue;
      }
      // Process the composites.
      for (CompositeDefinitionType composite : defs.getCompositeDefinition()) {
        CompositeHeadType head = composite.getCompositeHead();
        if (!isPublic(head) && !inputParameters.isIncludePrivateArtifacts()) {
          // Do not include a private composite operator. */
          continue;
        }
        String name = head.getName();
        splDoc = head.getSplDoc();
        if (splDoc != null) {
          artifact = getKnownArtifact(toolkit, namespace, name, ArtifactType.Operator);
          if (artifact != null) {
            processDescription(splDoc, artifact, fileUri, new LocationType(composite, fileUri));
          }
        }
        // If the composite operator has no graph, do not include it in the operator indexes.
        if (composite.getCompositeBody().getGraph() == null) {
          CompositeWithoutGraph.add(name);
        }
      }
      // Process the composite static types.
      for (String compName : staticTypes.keySet()) {
        TreeMap<String, TypeDefinitionType> compStaticTypes = staticTypes.get(compName);
        for (TypeDefinitionType type : compStaticTypes.values()) {
          splDoc = type.getSplDoc();
          if (splDoc != null) {
            artifact =
                getKnownArtifact(
                    toolkit, namespace, compName + "." + type.getName(), ArtifactType.Type);
            if (artifact != null) {
              processDescription(splDoc, artifact, fileUri, new LocationType(type, fileUri));
            }
          }
        }
      }
      // Process the functions.
      for (FunctionDefinitionType function : defs.getFunctionDefinition()) {
        FunctionHeadType head = function.getFunctionHead();
        if (!isPublic(head) && !inputParameters.isIncludePrivateArtifacts()) {
          // Do not include a private function. */
          continue;
        }
        splDoc = head.getSplDoc();
        if (splDoc != null) {
          artifact =
              getKnownArtifact(
                  toolkit, namespace, Encoder.getOverloadedName(head), ArtifactType.Function);
          if (artifact != null) {
            processDescription(splDoc, artifact, fileUri, new LocationType(function, fileUri));
          }
        }
      }
      // Process the standalone types.
      for (TypeDefinitionType type : defs.getTypeDefinition()) {
        splDoc = type.getSplDoc();
        if (splDoc != null) {
          artifact = getKnownArtifact(toolkit, namespace, type.getName(), ArtifactType.Type);
          if (artifact != null) {
            processDescription(splDoc, artifact, fileUri, new LocationType(type, fileUri));
          }
        }
      }
    }
    // Process the primitive operators.
    String toolkitPath = toolkit.getToolkitPath();
    for (ToolkitNamespaceType namespaceType : toolkitModel.getNamespace()) {
      String namespace = namespaceType.getName();
      for (PrimitiveToolkitOperatorType primOp : namespaceType.getPrimitiveOp()) {
        String opModelFile = toolkit.getModelFileNameForOp(primOp);
        OperatorModelType opModel =
            LoadModel.loadOperatorModel(toolkitPath + File.separator + opModelFile);
        if (opModel == null) ErrorManager.throwFatalErrorException();
        OpModelType cppOpModel = opModel.getCppOperatorModel();
        if (cppOpModel != null) {
          descriptionType = cppOpModel.getContext().getDescription();
        } else {
          JavaOpModelType javaOpModel = opModel.getJavaOperatorModel();
          if (javaOpModel != null) {
            descriptionType = javaOpModel.getContext().getDescription();
          } else {
            logger.printException(
                new Exception(
                    new Message(Key.OPERATOR_MODEL_OBJECT_MISSING).getLocalizedMessageAndPrefix()));
            break;
          }
        }
        if (descriptionType != null) {
          artifact = getKnownArtifact(toolkit, namespace, primOp.getName(), ArtifactType.Operator);
          if (artifact != null) {
            String fileUri = currentToolkit.getModelFileNameForOp(primOp);
            processDescription(
                descriptionType.getValue(), artifact, fileUri, new LocationType(artifact, fileUri));
            if (!toolkitMgr.isExcludedArtifact(artifact)) {
              toolkitsHavePrimitiveOperators = true;
            }
          }
        }
      }
    }
    // Process the native functions.
    List<String> functionModelFiles = toolkitMgr.getNativeFunctionModelFiles(toolkitModel);
    if (functionModelFiles != null) {
      for (String functionModelFile : functionModelFiles) {
        FunctionModelType functionModel =
            LoadModel.loadFunctionModel(toolkitPath + File.separator + functionModelFile);
        if (functionModel == null) {
          ErrorManager.throwFatalErrorException();
        }
        String namespace = getNamespaceForFunctionModel(functionModelFile);
        artifact =
            getKnownArtifact(
                toolkit,
                namespace,
                getNameForFunctionModel(functionModelFile),
                ArtifactType.FunctionModel);
        if (artifact != null && !toolkitMgr.isExcludedArtifact(artifact)) {
          toolkitsHaveNativeFunctions = true;
        }
        for (FunctionSetType functionSet : functionModel.getFunctionSet()) {
          FunctionsType functions = functionSet.getFunctions();
          if (functions != null) {
            for (FunctionType function : functions.getFunction()) {
              if ((descriptionType = function.getDescription()) != null) {
                // Look up function by full prototype string.
                String prototype = function.getPrototype().getValue();
                artifact =
                    getKnownArtifactByAltName(toolkit, namespace, prototype, ArtifactType.Function);
                if (artifact != null) {
                  processDescription(
                      descriptionType.getValue(),
                      artifact,
                      functionModelFile,
                      new LocationType(artifact, functionModelFile));
                }
              }
            }
          }
        }
      }
    }
    currentToolkit = null;
  }

  /** Generate a sorted list of the artifacts of the given type in the given namespace. */
  private String generateListForNamespaceAndType(String namespace, ArtifactType type) {
    List<Artifact> artifactList =
        toolkitMgr.getArtifactsByToolkitNamespaceAndType(currentToolkit, namespace, type);
    if (type == ArtifactType.Operator) {
      removeNonOperatorComposites(artifactList);
    }
    if (artifactList.isEmpty()) {
      return "";
    }
    Collections.sort(artifactList);
    StringBuilder sb = new StringBuilder();
    sb.append("      <dd><dl><dlentry>\n");
    sb.append("        <dt>" + getListLabel(type) + "</dt>\n");
    sb.append("        <dd>\n");
    sb.append(generateList(artifactList));
    sb.append("        </dd>\n");
    sb.append("      </dlentry></dl></dd>\n");
    return sb.toString();
  }

  /**
   * Given model objects for the toolkit and source, return a DITA document that describes and
   * indexes the content of the toolkit.
   */
  private String generate(ToolkitType toolkitModel, SourceModelType sourceModel) {
    String fileUri = FileOutput.ToolkitModelFileName;
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append(Constants.XHTML_Header_Reference);
    ditaDoc.append("<reference id=\"" + Constants.RefId_Toolkit + "\">" + "\n");
    ditaDoc.append(
        "<title outputclass=\"spltoolkitname\">"
            + logger.getMessage(
                currentToolkit.isSample()
                    ? Messages.MSG_TITLE_SAMPLE_TOOLKIT
                    : Messages.MSG_TITLE_TOOLKIT)
            + " <tt>"
            + getToolkitLabel(currentToolkit)
            + "</tt></title>"
            + "\n");
    ditaDoc.append("<refbody>" + "\n");
    ditaDoc.append(generateToolkitPathSection());

    // General Information on toolkit - name, version, etc
    ditaDoc.append("<section>" + "\n");
    ditaDoc.append(
        "<title outputclass=\"splhead-1\">"
            + logger.getMessage(Messages.MSG_TITLE_GENERAL_INFORMATION)
            + "</title>"
            + "\n");
    DescriptionType description = toolkitModel.getDescription();
    if (description != null) {
      Artifact artifact =
          getKnownArtifact(
              currentToolkit,
              null,
              currentToolkit.getToolkitModel().getName(),
              ArtifactType.Toolkit);
      if (artifact != null) {
        ditaDoc.append(
            Translater.xlateAsPages(
                    description.getValue(), fileUri, new LocationType(artifact, fileUri), artifact)
                + "\n");
      }
    }
    ditaDoc.append("<dl>" + "\n");
    ditaDoc.append("  <dlentry>" + "\n");
    ditaDoc
        .append("  <dt>")
        .append(logger.getMessage(Messages.MSG_TITLE_VERSION))
        .append("</dt>\n");
    ditaDoc.append("  <dd>" + toolkitModel.getVersion() + "</dd>" + "\n");
    ditaDoc.append("  </dlentry>" + "\n");
    ditaDoc.append("  <dlentry>" + "\n");
    ditaDoc
        .append("  <dt>")
        .append(logger.getMessage(Messages.MSG_TITLE_REQUIRED_PRODUCT_VERSION))
        .append("</dt>\n");
    ditaDoc.append("  <dd>" + toolkitModel.getRequiredProductVersion() + "</dd>" + "\n");
    ditaDoc.append("  </dlentry>" + "\n");
    EList<ToolkitDependencyType> toolkitDependencies = toolkitModel.getDependency();
    if (!toolkitDependencies.isEmpty()) {
      ditaDoc.append("  <dlentry>" + "\n");
      ditaDoc
          .append("  <dt>")
          .append(logger.getMessage(Messages.MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS))
          .append("</dt>\n");
      ditaDoc.append("  <dd><sl>" + "\n");
      for (ToolkitDependencyType toolkitDependency : toolkitDependencies) {
        // Use a link to the required toolkit if it is included in the documentation.
        String name = toolkitDependency.getName();
        List<Artifact> list = toolkitMgr.getArtifacts(null, name, ArtifactType.Toolkit);
        String name_or_link =
            (list.size() == 1 ? createDitaLink(createDitaLinkRef(list.get(0)), name) : name);
        // The required version may be a single version, or a range between limits that can be
        // inclusive or exclusive.
        String version = toolkitDependency.getVersion();
        String version_or_range = "";
        int index = version.indexOf(',');
        if (index < 0) {
          version_or_range = version;
        } else {
          boolean needAfter = false;
          boolean needBefore = false;
          String lowerBound = "";
          String upperBound = "";
          if (version.startsWith("(")) {
            needAfter = true;
            lowerBound = version.substring(1, index);
          } else if (version.startsWith("[")) {
            needAfter = false;
            lowerBound = version.substring(1, index);
          } else {
            logger.printError(
                Key.CDISP7124E,
                new String[] {"unexpected start of toolkit version range string: " + version});
            lowerBound = version.substring(0, index);
          }
          if (version.endsWith(")")) {
            needBefore = true;
            upperBound = version.substring(index + 1, version.length() - 1);
          } else if (version.endsWith("]")) {
            needBefore = false;
            upperBound = version.substring(index + 1, version.length() - 1);
          } else {
            logger.printError(
                Key.CDISP7124E,
                new String[] {"unexpected end of toolkit version range string: " + version});
            upperBound = version.substring(index + 1, version.length());
          }
          String messageKey =
              (needBefore && needAfter)
                  ? Messages.MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER
                  : needBefore
                      ? Messages.MSG_TITLE_VERSION_BOUNDED_BEFORE
                      : needAfter
                          ? Messages.MSG_TITLE_VERSION_BOUNDED_AFTER
                          : Messages.MSG_TITLE_VERSION_BOUNDED;

          version_or_range = logger.getMessage(messageKey, new String[] {lowerBound, upperBound});
        }
        ditaDoc.append("    <sli>" + name_or_link + " (" + version_or_range + ")</sli>" + "\n");
      }
      ditaDoc.append("  </sl></dd>" + "\n");
      ditaDoc.append("  </dlentry>" + "\n");
    }
    String author = inputParameters.getAuthor();
    if (author != null && author.length() > 0) {
      ditaDoc.append("  <dlentry>" + "\n");
      ditaDoc
          .append("  <dt>")
          .append(logger.getMessage(Messages.MSG_TITLE_AUTHOR))
          .append("</dt>\n");
      ditaDoc.append("  <dd>" + Encoder.HTMLEntityEncode(author) + "</dd>" + "\n");
      ditaDoc.append("  </dlentry>" + "\n");
    }
    ditaDoc.append("</dl>" + "\n");
    ditaDoc.append("</section>" + "\n");

    // List the artifact indexes for the toolkit.
    ditaDoc.append(generateIndexSection());

    // List the namespaces in sorted order.
    List<Artifact> nsArtifactList =
        toolkitMgr.getArtifactsByToolkitAndType(currentToolkit, ArtifactType.Namespace);
    Collections.sort(nsArtifactList);
    if (!nsArtifactList.isEmpty()) {
      ditaDoc.append("<section>" + "\n");
      ditaDoc.append(
          "  <title outputclass=\"splhead-1\">"
              + logger.getMessage(Messages.MSG_TITLE_NAMESPACES)
              + "</title>"
              + "\n");
      ditaDoc.append("  <dl>" + "\n");
      for (Artifact nsArtifact : nsArtifactList) {
        ditaDoc.append("    <dlentry>\n");
        ditaDoc.append(
            "      <dt outputclass=\"splpart\">"
                + createDitaLinkNamespace(currentToolkit, nsArtifact.name)
                + "</dt>\n");
        // Avoid empty list error.
        ditaDoc.append("      <dd></dd>\n");
        SourceFileType descFile = namespaceDescFileMap.get(nsArtifact.name);
        if (descFile != null) {
          // Add the first sentence from the namespace description file.
          SplDocType splDoc;
          if ((splDoc = descFile.getCompilationUnit().getSplDoc()) != null) {
            ditaDoc.append("      <dd>\n");
            ditaDoc.append(getFirstSentence(splDoc, descFile.getUri()) + "\n");
            ditaDoc.append("      </dd>\n");
          }
        }
        // List the operators, functions and types in sorted order.
        ditaDoc.append(generateListForNamespaceAndType(nsArtifact.name, ArtifactType.Operator));
        ditaDoc.append(generateListForNamespaceAndType(nsArtifact.name, ArtifactType.Function));
        ditaDoc.append(generateListForNamespaceAndType(nsArtifact.name, ArtifactType.Type));
        ditaDoc.append("     </dlentry>");
      }
      ditaDoc.append("  </dl>\n");
      ditaDoc.append("</section>" + "\n");
    }
    ditaDoc.append("</refbody>" + "\n");
    ditaDoc.append("</reference>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Generate DITA markup for SPLDOC sections where new page and section heading markup is not
   * allowed, using the given SPLDOC markup text from the given location in the given file.
   */
  private String generateSections(SplDocType splDoc, String fileUri, LocationType location) {
    String ditaDoc = "";
    SplDocDescriptionType descriptionType;
    if (splDoc == null || (descriptionType = splDoc.getDescription()) == null) {
      return ditaDoc;
    }
    ditaDoc += "<section>" + "\n";
    ditaDoc += Translater.xlate(descriptionType.getDescription(), fileUri, location) + "\n";
    ditaDoc += "</section>" + "\n";
    return ditaDoc;
  }

  /**
   * Generate DITA markup for SPLDOC sections where new page and section heading markup is allowed,
   * using the given SPLDOC markup text from the given location in the given file describing the
   * given toolkit artifact.
   */
  private String generateSectionsWithPages(
      SplDocType splDoc, String fileUri, LocationType location, Artifact artifact) {
    String ditaDoc = "";
    SplDocDescriptionType descriptionType;
    if (splDoc == null || (descriptionType = splDoc.getDescription()) == null) {
      return ditaDoc;
    }
    ditaDoc += "<section>" + "\n";
    ditaDoc +=
        Translater.xlateAsPages(descriptionType.getDescription(), fileUri, location, artifact)
            + "\n";
    ditaDoc += "</section>" + "\n";
    return ditaDoc;
  }

  /**
   * Given a set of SPLDOC annotation tags in an SPL file, create a hashmap of the annotation names
   * to enable constant time access to a particular type of annotation, and to access annotations in
   * a particular order.
   */
  private HashMap<String, SplDocAnnotationType> createSPLFileAnnotationsMap(
      EList<SplDocAnnotationType> annotations) {
    HashMap<String, SplDocAnnotationType> map = new HashMap<String, SplDocAnnotationType>();
    if (annotations == null) {
      return map;
    }
    for (SplDocAnnotationType annotation : annotations) {
      if (annotation.getTarget() != null) {
        map.put(annotation.getName() + "$" + annotation.getTarget(), annotation);
      } else {
        map.put(annotation.getName() + "$", annotation);
      }
    }
    return map;
  }

  /**
   * Given a set of SPLDOC annotation tags in an XML model file, create a hashmap of the annotation
   * names. to enable constant time access to a particular type of annotation, and to access
   * annotations in a particular order.
   */
  private HashMap<String, SPLDOCAnnotation> createAnnotationsMap(
      List<SPLDOCAnnotation> annotations) {
    HashMap<String, SPLDOCAnnotation> map = new HashMap<String, SPLDOCAnnotation>();
    for (SPLDOCAnnotation annotation : annotations) {
      if (annotation.target != null) {
        map.put(annotation.name + "$" + annotation.target, annotation);
      } else {
        map.put(annotation.name + "$", annotation);
      }
    }
    return map;
  }

  /**
   * Emit the info for an input or output port
   *
   * @param ports The set of ports we are emitting info for
   * @param direction A string with either input or output
   * @param map The map of annotation entities
   * @param fileUri the URI of the source file containing the composite.
   * @param location
   * @param ditaDoc The string builder being updated
   */
  private void generatePortInfo(
      EList<CompositePortType> ports,
      String direction,
      Map<String, SplDocAnnotationType> map,
      String fileUri,
      LocationType location,
      StringBuilder ditaDoc) {
    assert (location.modelObject != null);
    assert (location.modelObject instanceof CompositeDefinitionType);
    CompositeHeadType compHead =
        ((CompositeDefinitionType) location.modelObject).getCompositeHead();
    String compName = "";
    if (compHead != null) {
      // Should not be null
      compName = compHead.getName();
    } else {
      logger.printError(Key.CDISP7119E, location.toString(), new String[] {".sourceModel.xml"});
    }

    ditaDoc.append("<sl>\n");
    String keySuffix = direction + "$";
    for (CompositePortType port : ports) {
      SplDocAnnotationType annotation = map.get(keySuffix + port.getName());
      ditaDoc.append("<sli>");
      String description;
      if (annotation != null && (description = annotation.getDescription()) != null) {
        // Format as in-line text, ignoring markup for paragraphs, code blocks, etc.
        String xlateString = Translater.xlateAsText(description, fileUri, location);
        ditaDoc.append(formatProperty(port.getName(), xlateString, false));
      } else {
        // Report missing @input or @output annotation
        if (inputParameters.isWarnNoComments()) {
          logger.printWarn(
              Key.CDISP7111W,
              location.toString(),
              new String[] {"@" + direction, port.getName(), compName});
        }
        ditaDoc.append(formatProperty(port.getName(), ""));
      }
      ditaDoc.append("</sli>\n");
    }
    ditaDoc.append("</sl>\n");

    // Verify that the port annotations matches a composite port
    for (String annotationKey : map.keySet()) {
      if (!annotationKey.startsWith(keySuffix)) {
        continue;
      }
      boolean match = false;
      String annotationName = annotationKey.substring(keySuffix.length());
      for (CompositePortType port : ports) {
        if (port.getName().equals(annotationName)) {
          match = true;
          break;
        }
      }
      if (match == false) {
        logger.printWarn(
            Key.CDISP7115W,
            location.toString(),
            new String[] {"@" + direction, annotationName, compName});
      }
    }
  }

  /**
   * Returns DITA markup for composite operator's SPLDOC annotations.
   *
   * @param composite
   * @param fileUri the URI of the source file containing the composite.
   * @return
   */
  private String generateAnnotations(CompositeDefinitionType composite, String fileUri) {
    StringBuilder ditaDoc = new StringBuilder();
    CompositeHeadType head = composite.getCompositeHead();
    if (head == null) {
      return "";
    }
    LocationType location = new LocationType(composite, fileUri);
    SplDocType splDoc = head.getSplDoc();
    EList<SplDocAnnotationType> annotations = null;
    if (splDoc != null) {
      annotations = splDoc.getAnnotation();
    }
    Map<String, SplDocAnnotationType> map = createSPLFileAnnotationsMap(annotations);

    // For each parameter, generate documentation:
    //  <parameter name>: <parameter description>
    // The description may be empty if the parameter is not documented in the SPLDOC, or if there's
    // no SPLDOC comment.
    ditaDoc.append("<section>" + "\n");
    CompositeParametersType params = composite.getCompositeBody().getParameters();
    if (params != null) {
      ditaDoc
          .append("<title>")
          .append(logger.getMessage(Messages.MSG_TITLE_PARAMETERS))
          .append("</title>\n");
      ditaDoc.append("<sl>\n");
      for (CompositeParameterType param : params.getParameter()) {
        assert (param.getName().length() > 1);
        // A hack to skip the leading "$" prefix in parameter names
        String paramName = param.getName().substring(1);
        SplDocAnnotationType annotation = map.get("param" + "$" + paramName);
        ditaDoc.append("<sli>");
        String description;
        if (annotation != null && (description = annotation.getDescription()) != null) {
          // Format as in-line text, ignoring markup for paragraphs, code blocks, etc.
          String xlateString = Translater.xlateAsText(description, fileUri, location);
          ditaDoc.append(formatProperty(paramName, xlateString, false));
        } else {
          ditaDoc.append(formatProperty(paramName, ""));
          // Output warning if parameter is not documented by a @param annotation.
          if (inputParameters.isWarnNoComments()) {
            logger.printWarn(
                Key.CDISP7110W, location.toString(), new String[] {paramName, head.getName()});
          }
        }
        ditaDoc.append("</sli>\n");
      }
      ditaDoc.append("</sl>\n");
    }
    ditaDoc.append("</section>" + "\n");

    // Verify that the parameter annotations match a parameter defined for the composite
    for (String annotationKey : map.keySet()) {
      if (!annotationKey.startsWith("param$")) {
        continue;
      }
      boolean match = false;
      String annotationName = annotationKey.substring("param$".length());
      if (params != null) {
        for (CompositeParameterType param : params.getParameter()) {
          String paramName = param.getName();
          // Strip off the "$" prefix for parameter name in composite definition.
          // e.g.$tickers
          if (paramName.startsWith("$")) {
            paramName = paramName.substring(1);
          }
          if (paramName.equals(annotationName)) {
            match = true;
            break;
          }
        }
      }
      if (match == false) {
        logger.printWarn(
            Key.CDISP7116W, location.toString(), new String[] {annotationName, head.getName()});
      }
    }

    ditaDoc.append("<section>" + "\n");
    CompositeInputsType inputs = head.getInputs();
    if (inputs != null) {
      ditaDoc
          .append("<title>")
          .append(logger.getMessage(Messages.MSG_TITLE_INPUT_PORTS))
          .append("</title>\n");
      EList<CompositePortType> ports = inputs.getIport();
      if (ports != null) {
        generatePortInfo(ports, "input", map, fileUri, location, ditaDoc);
      }
    }
    ditaDoc.append("</section>" + "\n");
    ditaDoc.append("<section>" + "\n");
    CompositeOutputsType outputs = head.getOutputs();
    if (outputs != null) {
      ditaDoc
          .append("<title>")
          .append(logger.getMessage(Messages.MSG_TITLE_OUTPUT_PORTS))
          .append("</title>\n");
      EList<CompositePortType> ports = outputs.getOport();
      if (ports != null) {
        generatePortInfo(ports, "output", map, fileUri, location, ditaDoc);
      }
    }
    ditaDoc.append("</section>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Returns DITA markup for SPL function's SPLDOC annotations.
   *
   * @param function
   * @param fileUri the URI of the source file containing the function
   * @return
   */
  private String generateAnnotations(FunctionDefinitionType function, String fileUri) {
    StringBuilder ditaDoc = new StringBuilder();
    FunctionHeadType head = function.getFunctionHead();
    if (head == null) {
      return "";
    }
    LocationType location = new LocationType(function, fileUri);
    SplDocType splDoc = head.getSplDoc();
    EList<SplDocAnnotationType> annotations = null;
    if (splDoc != null) {
      annotations = splDoc.getAnnotation();
    }

    // For each parameter, generate documentation:
    //  <parameter name>: <parameter description>
    // The description may be empty if the parameter is not documented in the SPLDOC, or if there's
    // no SPLDOC comment.
    Map<String, SplDocAnnotationType> map = createSPLFileAnnotationsMap(annotations);
    ditaDoc.append("<section>" + "\n");
    FunctionParametersType params = head.getParameters();
    if (params != null) {
      ditaDoc
          .append("<title>")
          .append(logger.getMessage(Messages.MSG_TITLE_PARAMETERS))
          .append("</title>\n");
      ditaDoc.append(" <sl>\n");
      for (FunctionParameterType param : params.getParameter()) {
        ditaDoc.append("   <sli>\n");
        String paramName = param.getName();
        SplDocAnnotationType annotation = map.get("param" + "$" + paramName);
        String description;
        if (annotation != null && (description = annotation.getDescription()) != null) {
          // Format as in-line text, ignoring markup for paragraphs, code blocks, etc.
          String xlateString = Translater.xlateAsText(description, fileUri, location);
          ditaDoc.append(formatProperty(paramName, xlateString, false));
        } else {
          ditaDoc.append(formatProperty(paramName, ""));
          if (inputParameters.isWarnNoComments()) {
            logger.printWarn(
                Key.CDISP7112W,
                location.toString(),
                new String[] {"@param " + paramName, head.getName()});
          }
        }
        ditaDoc.append("   </sli>\n");
      }
      ditaDoc.append(" </sl>\n");
    }
    ditaDoc.append("</section>" + "\n");

    // Verify that the parameter annotations with targets each match a parameter for the function.
    for (String annotationKey : map.keySet()) {
      if (!annotationKey.startsWith("param$")) {
        continue;
      }
      String annotationName = annotationKey.substring("param$".length());
      if (annotationName.isEmpty()) {
        continue;
      }
      boolean match = false;
      if (params != null) {
        for (FunctionParameterType param : params.getParameter()) {
          if (param.getName().equals(annotationName)) {
            match = true;
            break;
          }
        }
      }
      if (match == false) {
        logger.printWarn(
            Key.CDISP7113W, location.toString(), new String[] {annotationName, head.getName()});
      }
    }

    ditaDoc.append("<section>" + "\n");
    String returns = head.getReturnType();
    // If a SPLDOc includes an annotation for the return value, output the description.
    if (returns != null) {
      SplDocAnnotationType annotation = map.get("return" + "$");
      if (returns.equals("void")) {
        if (annotation != null) {
          // Annotation is not allowed with void return type
          logger.printWarn(Key.CDISP7105W, location.toString(), new String[] {head.getName()});
        }
      } else {
        ditaDoc
            .append("<title>")
            .append(logger.getMessage(Messages.MSG_TITLE_RETURNS))
            .append("</title>\n");
        // Use simple list to indent the return description same as parameters.
        ditaDoc.append("<sl><sli>\n");
        String description;
        if (annotation != null && (description = annotation.getDescription()) != null) {
          // Format as in-line text, ignoring markup for paragraphs, code blocks, etc.
          ditaDoc.append(Translater.xlateAsText(description, fileUri, location));
        } else {
          ditaDoc.append("<tt>" + Encoder.HTMLEntityEncode(returns) + "</tt>\n");
          if (inputParameters.isWarnNoComments()) {
            logger.printWarn(
                Key.CDISP7112W, location.toString(), new String[] {"@return", head.getName()});
          }
        }
        ditaDoc.append("</sli></sl>\n");
      }
    }
    ditaDoc.append("</section>" + "\n");
    return ditaDoc.toString();
  }

  /** Generates a DITA document for the compilation unit (SPL file). */
  private String generate(
      SourceFileType sourceFile,
      Artifact compUnitArtifact,
      Map<String, TreeMap<String, TypeDefinitionType>> staticTypes) {
    CompilationUnitType compUnit = sourceFile.getCompilationUnit();
    DefinitionsType defs = compUnit.getDefinitions();
    String sourceFileName = sourceFile.getUri();
    String qualifiedSourceFileName =
        new File(currentToolkit.getToolkitPath(), sourceFileName).toString();
    String namespace = compUnitArtifact.namespace;
    String compUnitName = compUnitArtifact.name;
    boolean includeSource = (inputParameters.isIncludeSource() || generatingSampleToolkitDocs);
    boolean warnNoComment = inputParameters.isWarnNoComments();
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append(Constants.XHTML_Header_Reference);
    ditaDoc.append("<reference id=\"" + Constants.RefId_CompilationUnit + "\"" + ">" + "\n");
    ditaDoc.append(
        "<title>"
            + logger.getMessage(
                currentToolkit.isSample()
                    ? Messages.MSG_TITLE_SAMPLE_SPL_FILE
                    : Messages.MSG_TITLE_SPL_FILE)
            + " <tt>"
            + compUnitName
            + "</tt></title>"
            + "\n");
    ditaDoc.append("<refbody>" + "\n");
    ditaDoc.append(generateArtifactPathSection(namespace, compUnitName));

    // SPL file overview section
    SplDocType compUnitSplDoc = compUnit.getSplDoc();
    if (compUnitSplDoc != null) {
      ditaDoc.append(
          generateSectionsWithPages(
              compUnitSplDoc,
              sourceFileName,
              new LocationType(compUnitArtifact, sourceFileName),
              compUnitArtifact));
    }

    // Table of contents - links to each of the composite operators, functions, and types in sorted
    // order.
    ditaDoc.append("<section>" + "\n");
    ditaDoc.append(
        "  <title outputclass=\"splhead-1\">"
            + logger.getMessage(Messages.MSG_TITLE_CONTENT)
            + "</title>"
            + "\n");
    ditaDoc.append("  <dl>" + "\n");
    // An empty definition list entry avoids an error if there are no other entries.
    ditaDoc.append("    <dlentry><dt></dt><dd></dd></dlentry>\n");
    List<Artifact> artifactList;
    artifactList =
        toolkitMgr.getArtifactsByToolkitNamespaceTypeAndUri(
            currentToolkit, namespace, ArtifactType.Operator, sourceFileName);
    removeNonOperatorComposites(artifactList);
    if (!artifactList.isEmpty()) {
      Collections.sort(artifactList);
      ditaDoc.append("    <dlentry>\n");
      ditaDoc.append(
          "      <dt outputclass=\"splhead-2\">"
              + logger.getMessage(Messages.MSG_TITLE_OPERATORS)
              + "</dt>\n");
      ditaDoc.append("      <dd>\n");
      ditaDoc.append(generateListWithInLineDesc(artifactList));
      ditaDoc.append("      </dd>\n");
      ditaDoc.append("    </dlentry>\n");
    }
    artifactList =
        toolkitMgr.getArtifactsByToolkitNamespaceTypeAndUri(
            currentToolkit, namespace, ArtifactType.Function, sourceFileName);
    if (!artifactList.isEmpty()) {
      Collections.sort(artifactList);
      ditaDoc.append("    <dlentry>\n");
      ditaDoc.append(
          "      <dt outputclass=\"splhead-2\">"
              + logger.getMessage(Messages.MSG_TITLE_FUNCTIONS)
              + "</dt>\n");
      ditaDoc.append("      <dd>\n");
      ditaDoc.append(generateListWithInLineDesc(artifactList));
      ditaDoc.append("      </dd>\n");
      ditaDoc.append("    </dlentry>\n");
    }
    artifactList =
        toolkitMgr.getArtifactsByToolkitNamespaceTypeAndUri(
            currentToolkit, namespace, ArtifactType.Type, sourceFileName);
    if (!artifactList.isEmpty()) {
      Collections.sort(artifactList);
      ditaDoc.append("    <dlentry>\n");
      ditaDoc.append(
          "      <dt outputclass=\"splhead-2\">"
              + logger.getMessage(Messages.MSG_TITLE_TYPES)
              + "</dt>\n");
      ditaDoc.append("      <dd>\n");
      ditaDoc.append(generateListWithInLineDesc(artifactList));
      ditaDoc.append("      </dd>\n");
      ditaDoc.append("    </dlentry>\n");
    }
    ditaDoc.append("  </dl>" + "\n");
    ditaDoc.append("</section>" + "\n");

    // Create a section for each composite operator.
    boolean compTitleDone = false;
    for (CompositeDefinitionType composite : defs.getCompositeDefinition()) {
      CompositeHeadType compHead = composite.getCompositeHead();
      if (!isPublic(compHead) && !inputParameters.isIncludePrivateArtifacts()) {
        // Do not include a private composite operator. */
        continue;
      }
      String compName = compHead.getName();
      Artifact compArtifact =
          getKnownArtifact(currentToolkit, namespace, compName, ArtifactType.Operator);
      if (compArtifact == null || toolkitMgr.isExcludedArtifact(compArtifact)) {
        // Do not include an artifact that has been excluded from the generated documentation. */
        continue;
      }
      if (!compTitleDone) {
        ditaDoc.append("<section>" + "\n");
        ditaDoc.append(
            "  <title outputclass=\"splhead-1\">"
                + logger.getMessage(Messages.MSG_TITLE_COMPOSITES)
                + "</title>"
                + "\n");
        ditaDoc.append("</section>" + "\n");
        compTitleDone = true;
      }
      LocationType location = new LocationType(composite, sourceFileName);
      String prototype = formatCompositePrototype(compHead);
      ditaDoc.append(
          "<section id=\"" + Constants.SectionId_Prefix_composite + compName + "\"" + ">" + "\n");
      ditaDoc.append("  <title outputclass=\"splpart\">" + prototype + "</title>" + "\n");
      ditaDoc.append("</section>" + "\n");

      // Include the composite operator diagram only if requested, or if processing samples.
      if (inputParameters.isIncludeCompositeOperatorDiagram() || generatingSampleToolkitDocs) {
        CompositeGraphType compGraph = composite.getCompositeBody().getGraph();
        if (compGraph != null) {
          Map<String, ArrayList<Integer>> compOpDimension =
              CompositeOperatorGraphImageGenerator.generateCompositeOpImage(
                  currentToolkit, namespace, composite);
          // Merge the (single) entry from the composite operator dimension map into the global
          // image dimension map.
          imageDimensions = new HashMap<String, ArrayList<Integer>>();
          for (String imageFileName : compOpDimension.keySet()) {
            imageDimensions.put(imageFileName, compOpDimension.get(imageFileName));
          }
          // Embed link to image of composite operator
          ditaDoc.append(createImageLinkOp(namespace, compName, Key.CDISP7011I, OutputClass_Graph));
        }
      }

      // Include overview description, if any.
      SplDocType splDoc = compHead.getSplDoc();
      if (splDoc != null && splDoc.getDescription() != null) {
        ditaDoc.append(generateSections(splDoc, sourceFileName, location));
      } else if (warnNoComment) {
        logger.printWarn(Key.CDISP7080W, location.toString(), new String[] {compName});
      }

      // Generate annotations for composite operator tags - e.g. input ports, output ports
      ditaDoc.append(generateAnnotations(composite, sourceFileName));

      // Create a section for the static types of the composite.
      TreeMap<String, TypeDefinitionType> compStaticTypes = staticTypes.get(compName);
      if (compStaticTypes != null && compStaticTypes.size() > 0) {
        ditaDoc.append("<section>\n");
        // Don't use an outputclass of splpart, as this type is part of a composite
        ditaDoc.append(
            "  <title>"
                + logger.getMessage(Messages.MSG_TITLE_COMPOSITE_STATIC_TYPES)
                + "</title>"
                + "\n");
        ditaDoc.append("</section>\n");
        for (String typeName : compStaticTypes.keySet()) {
          String qualifiedTypeName = compName + "." + typeName;
          Artifact artifact =
              getKnownArtifact(currentToolkit, namespace, qualifiedTypeName, ArtifactType.Type);
          if (artifact == null || toolkitMgr.isExcludedArtifact(artifact)) {
            // Do not include an artifact that has been excluded from the generated documentation.
            // */
            continue;
          }
          TypeDefinitionType type = compStaticTypes.get(typeName);
          // Don't use an outputclass of splpart, as this type is part of a composite
          ditaDoc.append(
              "   <section id=\""
                  + Constants.SectionId_Prefix_composite_type
                  + qualifiedTypeName
                  + "\">"
                  + "\n");
          ditaDoc.append("     <dl>\n");
          ditaDoc.append("       <dlentry>\n");
          ditaDoc.append(
              "         <dt>"
                  + qualifiedTypeName
                  + " = "
                  + Encoder.HTMLEntityEncode(type.getValue())
                  + ";"
                  + "</dt>\n");
          SplDocDescriptionType descriptionType;
          splDoc = type.getSplDoc();
          if (splDoc != null && (descriptionType = splDoc.getDescription()) != null) {
            ditaDoc.append(
                "         <dd>"
                    + Translater.xlate(descriptionType.getDescription(), sourceFileName, location)
                    + "</dd>\n");
          } else {
            if (warnNoComment) {
              logger.printWarn(
                  Key.CDISP7082W, location.toString(), new String[] {qualifiedTypeName});
            }
            ditaDoc.append("         <dd></dd>\n");
          }
          ditaDoc.append("       </dlentry>\n");
          ditaDoc.append("     </dl>\n");
          ditaDoc.append("   </section>\n");
        }
      } // if (staticTypes.size() > 0) ...

      // Include source, if requested.
      if (includeSource) {
        ditaDoc.append("<section>\n");
        ditaDoc.append(
            "  <title outputclass=\"splhead-2\">"
                + logger.getMessage(Messages.MSG_TITLE_SPL_SOURCE_CODE)
                + "</title>"
                + "\n");
        ditaDoc.append("</section>\n");

        ditaDoc.append("\n<section>" + "\n");
        ditaDoc.append("   <codeblock>" + "\n");
        ditaDoc.append("<![CDATA[" + "\n");
        String sourceCode = getIncludeCode(qualifiedSourceFileName, composite);
        ditaDoc.append(
            sourceCode == null
                ? "Source not found for qualifiedSourceFileName" + qualifiedSourceFileName
                : sourceCode);
        ditaDoc.append("]]>" + "\n");
        ditaDoc.append("   </codeblock>" + "\n");
        ditaDoc.append("</section>" + "\n");
      }
    }

    // Create a section for each function.
    boolean functTitleDone = false;
    for (FunctionDefinitionType function : defs.getFunctionDefinition()) {
      FunctionHeadType functHead = function.getFunctionHead();
      if (!isPublic(functHead) && !inputParameters.isIncludePrivateArtifacts()) {
        // Do not include a private function. */
        continue;
      }
      String overloaded_name = Encoder.getOverloadedName(functHead);
      Artifact artifact =
          getKnownArtifact(currentToolkit, namespace, overloaded_name, ArtifactType.Function);
      if (artifact == null || toolkitMgr.isExcludedArtifact(artifact)) {
        // Do not include an artifact that has been excluded from the generated documentation. */
        continue;
      }
      if (!functTitleDone) {
        ditaDoc.append("<section>" + "\n");
        ditaDoc.append(
            "  <title outputclass=\"splhead-1\">"
                + logger.getMessage(Messages.MSG_TITLE_FUNCTIONS)
                + "</title>"
                + "\n");
        ditaDoc.append("</section>" + "\n");
        functTitleDone = true;
      }
      ditaDoc.append(
          "<section id=\""
              + Constants.SectionId_Prefix_function
              + overloaded_name
              + "\""
              + ">"
              + "\n");
      ditaDoc.append(
          "  <title outputclass=\"splpart\">"
              + formatFunctionPrototype(functHead)
              + "</title>"
              + "\n");
      ditaDoc.append("</section>");
      SplDocType splDoc = functHead.getSplDoc();
      LocationType location = new LocationType(function, sourceFileName);
      if (splDoc != null && splDoc.getDescription() != null) {
        ditaDoc.append(generateSections(splDoc, sourceFileName, location));
      } else if (warnNoComment) {
        logger.printWarn(Key.CDISP7081W, location.toString(), new String[] {functHead.getName()});
      }
      // Generate documentation for SPLDOC annotations in comment for function
      ditaDoc.append(generateAnnotations(function, sourceFileName));
      if (includeSource) {
        ditaDoc.append("<section>\n");
        ditaDoc.append(
            "  <title outputclass=\"splhead-2\">"
                + logger.getMessage(Messages.MSG_TITLE_SPL_SOURCE_CODE)
                + "</title>"
                + "\n");
        ditaDoc.append("</section>\n");
        ditaDoc.append("<section>" + "\n");
        ditaDoc.append("   <codeblock>" + "\n");
        ditaDoc.append("<![CDATA[" + "\n");
        String sourceCode = getIncludeCode(qualifiedSourceFileName, function);
        ditaDoc.append(
            sourceCode == null
                ? /*"Source not found for " +  qualifiedSourceFileName*/ logger.getMessage(
                    Messages.MSG_SOURCE_NOT_FOUND, new String[] {qualifiedSourceFileName})
                : sourceCode);
        ditaDoc.append("]]>" + "\n");
        ditaDoc.append("   </codeblock>" + "\n");
        ditaDoc.append("</section>" + "\n");
      }
    }

    // Create a section for each standalone type.
    boolean typeTitleDone = false;
    for (TypeDefinitionType type : defs.getTypeDefinition()) {
      String typeName = type.getName();
      Artifact artifact = getKnownArtifact(currentToolkit, namespace, typeName, ArtifactType.Type);
      if (artifact == null || toolkitMgr.isExcludedArtifact(artifact)) {
        // Do not include an artifact that has been excluded from the generated documentation. */
        continue;
      }
      if (!typeTitleDone) {
        ditaDoc.append("<section>" + "\n");
        ditaDoc.append(
            "  <title outputclass=\"splhead-1\">"
                + logger.getMessage(Messages.MSG_TITLE_STANDALONE_TYPES)
                + "</title>"
                + "\n");
        ditaDoc.append("</section>" + "\n");
        typeTitleDone = true;
      }
      ditaDoc.append(
          "<section id=\"" + Constants.SectionId_Prefix_type + typeName + "\"" + ">" + "\n");
      ditaDoc.append("  <title outputclass=\"splpart\">" + typeName + "</title>" + "\n");
      ditaDoc.append("</section>");
      SplDocType splDoc = type.getSplDoc();
      LocationType location = new LocationType(type, sourceFileName);
      if (splDoc != null && splDoc.getDescription() != null) {
        ditaDoc.append(generateSections(splDoc, sourceFileName, location));
      } else if (warnNoComment) {
        logger.printWarn(Key.CDISP7082W, location.toString(), new String[] {typeName});
      }
      ditaDoc.append("<section>\n");
      ditaDoc.append("   <p>\n");
      ditaDoc.append(
          "<tt>" + typeName + " = " + Encoder.HTMLEntityEncode(type.getValue()) + ";" + "</tt>\n");
      ditaDoc.append("   </p>\n");
      ditaDoc.append("</section>\n");
    }

    ditaDoc.append("</refbody>" + "\n");
    ditaDoc.append("</reference>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Given start and end port indices, construct string to represent the port range.
   *
   * @param start_port
   * @param end_port
   * @return
   */
  private String portRangeString(int start_port, int end_port) {
    if (start_port == end_port) {
      return "(" + start_port + ")";
    }
    if (end_port == -1) {
      return "(" + start_port + "...)";
    }
    return "(" + start_port + "..." + end_port + ")";
  }

  enum SubjectType {
    Operator,
    PortSet
  };

  /**
   * Generates text to describe output assignment capabilities supported by given output port set.
   *
   * @param expressionMode the expression mode for a port set
   * @param autoAssignment true if the operator model specifies autoAssignment, otherwise false
   * @param completeAssignment true if the operator model specified complete assignment, otherwise
   *     false
   * @param subjectType one of the SubjectType enumeration values
   * @return text that describes output port assignments.
   */
  private String outputPortAssignmentText(
      ExpressionModeType expressionMode,
      boolean autoAssignment,
      boolean completeAssignment,
      SubjectType subjectType) {
    // There can only be one output message for non-existent assignment mode
    if (expressionMode == ExpressionModeType.NONEXISTENT) {
      // return logger.getMessage("MSG_" + subject + "_NO_ASSIGNMENTS");
      if (subjectType == SubjectType.Operator) {
        return logger.getMessage(Messages.MSG_OPERATOR_NO_ASSIGNMENTS);
      }
      return logger.getMessage(Messages.MSG_PORTSET_NO_ASSIGNMENTS);
    }

    String text = "";
    if (expressionMode == ExpressionModeType.ATTRIBUTE) {
      // text = logger.getMessage("MSG_" + subject + "_ASSIGN_MUST_BE_ATTRIBUTES");
      if (subjectType == SubjectType.Operator)
        text = logger.getMessage(Messages.MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES);
      else text = logger.getMessage(Messages.MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES);

    } else if (expressionMode == ExpressionModeType.ATTRIBUTE_FREE) {
      // text =  logger.getMessage("MSG_" + subject + "_ASSIGN_MUST_BE_ATTRIBUTE_FREE");
      if (subjectType == SubjectType.Operator)
        text = logger.getMessage(Messages.MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE);
      else text = logger.getMessage(Messages.MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE);

    } else if (expressionMode == ExpressionModeType.CONSTANT) {
      // text = logger.getMessage("MSG_" + subject + "_ASSIGN_MUST_BE_CONSTANT");
      if (subjectType == SubjectType.Operator)
        text = logger.getMessage(Messages.MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT);
      else text = logger.getMessage(Messages.MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT);

    } else if (expressionMode == ExpressionModeType.EXPRESSION) {
      // text = logger.getMessage("MSG_" + subject + "_ASSIGN_MUST_BE_EXPRESSION");
      if (subjectType == SubjectType.Operator)
        text = logger.getMessage(Messages.MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION);
      else text = logger.getMessage(Messages.MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION);
    }

    if (completeAssignment) {
      if (autoAssignment) {
        text += " " + logger.getMessage(Messages.MSG_AUTO_ASSIGNMENT);
      } else {
        text += " " + logger.getMessage(Messages.MSG_COMPLETE_ASSIGNMENT);
      }
    }
    return text;
  }

  /**
   * Given a StringBuilder and an output port set object, return a description of the assignment
   * characteristics of this port set. The description contains markup.
   *
   * @param ditaDoc appended content to this StringBuilder.
   * @param outputPortSet the port set
   * @return
   */
  private StringBuilder getOutputPortAssignmentMarkup(
      StringBuilder ditaDoc, OutputPortOpenSetType outputPortSet) {
    ditaDoc.append("    <dl>\n");
    ditaDoc.append("      <dlentry>\n");
    ditaDoc
        .append("        <dt>")
        .append(logger.getMessage(Messages.MSG_TITLE_ASSIGNMENTS))
        .append("</dt>\n");
    ditaDoc.append(
        "        <dd>"
            + outputPortAssignmentText(
                outputPortSet.getExpressionMode(),
                outputPortSet.isAutoAssignment(),
                outputPortSet.isCompleteAssignment(),
                SubjectType.PortSet)
            + "</dd>\n");
    ditaDoc.append("      </dlentry>\n");
    ditaDoc.append("    </dl>\n");
    return ditaDoc;
  }

  private StringBuilder getInputPortSetConfigurationMarkup(
      StringBuilder ditaDoc, InputPortSetType inputPortSet) {
    ditaDoc.append("   <sl>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "Optional",
                inputPortSet.isOptional(),
                OperatorModelRefPath
                    + "#"
                    + References.OperatorModelRefId
                    + "/"
                    + References.OperatorModelInputPortSetId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append("   </sl>\n");
    return getInputPortSetConfigurationMarkup(ditaDoc, (InputPortOpenSetType) inputPortSet);
  }

  private StringBuilder getInputPortSetConfigurationMarkup(
      StringBuilder ditaDoc, JavaOpInputPortSetType inputPortSet) {
    ditaDoc.append("   <sl>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty("Optional", inputPortSet.isOptional(), RefId_Operator_InputPortSetId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append("   </sl>\n");
    return getInputPortSetConfigurationMarkup(ditaDoc, (JavaOpInputPortOpenSetType) inputPortSet);
  }

  private StringBuilder getInputPortSetConfigurationMarkup(
      StringBuilder ditaDoc, InputPortOpenSetType inputPortSet) {
    String inputPortSetId =
        OperatorModelRefPath
            + "#"
            + References.OperatorModelRefId
            + "/"
            + References.OperatorModelInputPortSetId;
    ditaDoc.append("   <sl>\n");
    ditaDoc.append(
        "     <sli>" + formatProperty("ControlPort", inputPortSet.isControlPort(), inputPortSetId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "TupleMutationAllowed", inputPortSet.isTupleMutationAllowed(), inputPortSetId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "WindowingMode", inputPortSet.getWindowingMode().toString(), inputPortSetId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "WindowPunctuationInputMode",
                inputPortSet.getWindowPunctuationInputMode().toString(),
                inputPortSetId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append("   </sl>\n");
    return ditaDoc;
  }

  private StringBuilder getInputPortSetConfigurationMarkup(
      StringBuilder ditaDoc, JavaOpInputPortOpenSetType inputPortSet) {
    ditaDoc.append("   <sl>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "ControlPort", inputPortSet.isControlPort(), RefId_Operator_InputPortSetId)
            + "</sli>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "WindowingMode",
                inputPortSet.getWindowingMode().toString(),
                RefId_Operator_InputPortSetId)
            + "</sli>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "WindowPunctuationInputMode",
                inputPortSet.getWindowPunctuationInputMode().toString(),
                RefId_Operator_InputPortSetId)
            + "</sli>\n");
    ditaDoc.append("   </sl>\n");
    return ditaDoc;
  }

  private StringBuilder getOutputPortSetConfigurationMarkup(
      StringBuilder ditaDoc, OutputPortSetType outputPortSet) {
    String outputPortSetId =
        OperatorModelRefPath
            + "#"
            + References.OperatorModelRefId
            + "/"
            + References.OperatorModelOutputPortSetId;
    ditaDoc.append("   <sl>\n");
    ditaDoc.append(
        "     <sli>" + formatProperty("Optional", outputPortSet.isOptional(), outputPortSetId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append("   </sl>\n");
    return getOutputPortSetConfigurationMarkup(ditaDoc, (OutputPortOpenSetType) outputPortSet);
  }

  private StringBuilder getOutputPortSetConfigurationMarkup(
      StringBuilder ditaDoc, JavaOpOutputPortSetType outputPortSet) {
    ditaDoc.append("   <sl>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "Optional", outputPortSet.isOptional(), RefId_Operator_OutputPortSetId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append("   </sl>\n");
    return getOutputPortSetConfigurationMarkup(
        ditaDoc, (JavaOpOutputPortOpenSetType) outputPortSet);
  }

  private StringBuilder getOutputPortSetConfigurationMarkup(
      StringBuilder ditaDoc, OutputPortOpenSetType outputPortSet) {
    String outputPortSetId =
        OperatorModelRefPath
            + "#"
            + References.OperatorModelRefId
            + "/"
            + References.OperatorModelOutputPortSetId;
    ditaDoc.append("   <sl>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "TupleMutationAllowed", outputPortSet.isTupleMutationAllowed(), outputPortSetId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "WindowPunctuationOutputMode",
                outputPortSet.getWindowPunctuationOutputMode().toString(),
                outputPortSetId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append("   </sl>\n");
    return ditaDoc;
  }

  private StringBuilder getOutputPortSetConfigurationMarkup(
      StringBuilder ditaDoc, JavaOpOutputPortOpenSetType outputPortSet) {
    String outputPortSetId =
        OperatorModelRefPath
            + "#"
            + References.OperatorModelRefId
            + "/"
            + References.OperatorModelOutputPortSetId;
    ditaDoc.append("   <sl>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "WindowPunctuationOutputMode",
                outputPortSet.getWindowPunctuationOutputMode().toString(),
                outputPortSetId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append("   </sl>\n");
    return ditaDoc;
  }

  /**
   * Given a StringBuilder and parameter, add the markup for the parameter properties to the
   * StringBuilder.
   *
   * @param ditaDoc
   * @param parameter
   * @param opModel the C++ operator model
   * @return
   */
  private StringBuilder getParameterConfigurationMarkup(
      StringBuilder ditaDoc, ParameterType parameter, OpModelType opModel) {
    ditaDoc.append("   <sl>\n");
    // Some property values may be null - which means that they don't apply to the parameter.
    if (parameter.getType() != null) {
      // For each parameter type that's defined as a CustomLiteral, list the enumerated values for
      // the custom literal.
      // HTML entity encode the type, e.g. list<rstring>
      // String type_link = formatReferenceLink(RefId_Operator_ParameterId + "__type",
      //         logger.getMessage(Messages.MSG_OPERATOR_TYPE));
      ditaDoc.append(
          "     <sli>" + formatProperty("Type", parameter.getType(), RefId_Operator_ParameterId));
      if ("CustomLiteral".equals(parameter.getExpressionMode().getName())) {
        ContextType contextType = opModel.getContext();
        EnumerationsType enumsType = contextType.getCustomLiterals();
        if (enumsType != null) {
          for (EnumerationType enumType : enumsType.getEnumeration()) {
            if (enumType.getName().equals(parameter.getType())) {
              ditaDoc.append(" (");
              ditaDoc.append("<tt>");
              ditaDoc.append(commaSeparatedList(enumType.getValue()));
              ditaDoc.append("</tt>");
              ditaDoc.append(")");
              break;
            }
          }
        }
      }
      ditaDoc.append("     </sli>\n");
    }
    if (parameter.getCardinality() != null
        && !parameter.getCardinality().equals(new BigInteger("-1"))) {
      // -1 is equivalent to not specifying a markup.
      ditaDoc.append(
          "     <sli>"
              + formatProperty(
                  "Cardinality",
                  parameter.getCardinality().toString(),
                  RefId_Operator_ParameterId));
      ditaDoc.append("     </sli>\n");
    }
    ditaDoc.append(
        "     <sli>"
            + formatProperty("Optional", parameter.isOptional(), RefId_Operator_ParameterId));
    ditaDoc.append("     </sli>\n");
    ditaDoc.append(
        "     <sli>"
            + formatProperty(
                "ExpressionMode",
                parameter.getExpressionMode().toString(),
                RefId_Operator_ParameterId));
    ditaDoc.append("     </sli>\n");

    if (parameter.getPortScope() != null) {
      ditaDoc.append(
          "     <sli>"
              + formatProperty(
                  "PortScope",
                  commaSeparatedIntegerList(parameter.getPortScope().getPort()),
                  RefId_Operator_ParameterId));
      ditaDoc.append("     </sli>\n");
    }
    ditaDoc.append("   </sl>\n");
    return ditaDoc;
  }

  /**
   * Format parameter list for composite operator. Each parameter is formatted as a link.
   *
   * @param parameters the parameters to be included in the list.
   * @param refId the reference id for the document. Used to define links.
   */
  private void formatParameterList(StringBuilder sb, List<?> parameters, String refId) {
    int num_parameters = parameters.size();
    if (num_parameters == 0) {
      return;
    }
    // Show parameters in sorted order.
    Set<String> sortedParams = new TreeSet<String>();
    boolean isJavaOp = parameters.get(0) instanceof JavaOpParameterType;
    for (Object parameter : parameters) {
      sortedParams.add(
          isJavaOp
              ? ((JavaOpParameterType) parameter).getName()
              : ((ParameterType) parameter).getName());
    }
    sb.append("\n");
    boolean first = true;
    for (String parameterName : sortedParams) {
      if (!first) {
        sb.append(", ");
      } else {
        first = false;
      }
      sb.append("<xref href=\"#" + refId + "/parameter__" + parameterName + "\">");
      sb.append(parameterName);
      sb.append("</xref>");
    }
    sb.append("\n");
  }

  /**
   * List composite operator parameters, as two lists: required parameters and optional parameters.
   */
  private void listParameters(StringBuilder sb, Object parameters, String refId) {
    boolean isJavaOp = parameters instanceof JavaOpParametersType;
    int num_parameters =
        isJavaOp
            ? ((JavaOpParametersType) parameters).getParameter().size()
            : ((ParametersType) parameters).getParameter().size();
    if (num_parameters > 0) {
      Object parameter;
      List<Object> required = new ArrayList<Object>();
      List<Object> optional = new ArrayList<Object>();
      for (int i = 0; i < num_parameters; i++) {
        boolean isOptional;
        if (isJavaOp) {
          parameter = ((JavaOpParametersType) parameters).getParameter().get(i);
          isOptional = ((JavaOpParameterType) parameter).isOptional();
        } else {
          parameter = ((ParametersType) parameters).getParameter().get(i);
          isOptional = ((ParameterType) parameter).isOptional();
        }
        if (isOptional) {
          optional.add(parameter);
        } else {
          required.add(parameter);
        }
      }
      if (!required.isEmpty()) {
        sb.append("\n<p> ");
        sb.append(logger.getMessage(Messages.MSG_REQUIRED_PARAMETER) + " ");
        formatParameterList(sb, required, refId);
        sb.append("\n</p>");
      }
      if (!optional.isEmpty()) {
        sb.append("\n<p> ");
        sb.append(logger.getMessage(Messages.MSG_OPTIONAL_PARAMETER) + " ");
        formatParameterList(sb, optional, refId);
        sb.append("\n</p>");
      }
    }
  }

  /**
   * Given a link to a DITA anchor and a label, return a markup for a title that contains a link.
   *
   * <p>Note that a DITA "title" element can't contain a link. This method provides a workaround.
   *
   * @param link
   * @param label
   * @return
   */
  private String formatTitleLink(String link, String label) {
    return "<p outputclass=\"splhead-1\"><b>" + formatReferenceLink(link, label) + "</b></p>";
  }

  /**
   * Format DITA for operator model code template.
   *
   * @param templates objects from operator model.
   * @return DITA for code templates
   */
  private String formatCodeTemplates(CodeTemplatesType templates) {
    StringBuffer ditaDoc = new StringBuffer();
    if (templates != null && templates.getCodeTemplate().size() > 0) {
      ditaDoc.append("<section>" + "\n");
      // ditaDoc.append("<title outputclass=\"splhead-1\">" + "Templates" + "</title>" + "\n");
      ditaDoc.append(
          formatTitleLink(
              RefId_Operator_ContextId + "__codetemplate",
              logger.getMessage(Messages.MSG_TITLE_CODE_TEMPLATES)));
      ditaDoc.append("  <dl>\n");
      for (CodeTemplateType template : templates.getCodeTemplate()) {
        ditaDoc.append("    <dlentry>\n");
        ditaDoc.append("     <dt>" + template.getName() + "</dt>\n");
        ditaDoc.append("     <dd>\n");
        ditaDoc.append("       <codeblock>\n");
        if (template.getTemplate() != null) {
          ditaDoc.append("<![CDATA[" + template.getTemplate() + "]]>");
        }
        ditaDoc.append("\n      </codeblock>\n");
        ditaDoc.append("     <p></p>\n");
        ditaDoc.append("     </dd>\n");
        ditaDoc.append("    </dlentry>\n");
      }
      ditaDoc.append("  </dl>\n");
      ditaDoc.append("</section>" + "\n");
    }
    return ditaDoc.toString();
  }

  /**
   * Given an operator metrics object, format the metrics summary.
   *
   * @param metrics
   * @param URI of operator model file
   * @return metrics section encoded uses DITA
   */
  private String formatMetricsSummary(MetricsType metrics, String fileUri) {
    StringBuffer ditaDoc = new StringBuffer();
    ditaDoc.append(" <dlentry>" + "\n");
    ditaDoc
        .append("  <dt>")
        .append(logger.getMessage(Messages.MSG_TITLE_METRICS))
        .append("</dt>\n");
    String metricsSummary = "";
    int num_metrics = 0;
    if (metrics != null
        && (metrics.getDescription() != null
            || (metrics.getMetric() != null && !metrics.getMetric().isEmpty()))) {
      if (metrics.getDescription() != null) {
        metricsSummary = logger.getMessage(Messages.MSG_OPERATOR_HAS_METRICS_DESC);
      } else {
        num_metrics = metrics.getMetric().size();
        if (num_metrics == 1) {
          metricsSummary = logger.getMessage(Messages.MSG_OPERATOR_HAS_ONE_METRIC);
        } else {
          metricsSummary =
              logger.getMessage(
                  Messages.MSG_OPERATOR_HAS_METRICS, new String[] {String.valueOf(num_metrics)});
        }
      }
    } else {
      metricsSummary = logger.getMessage(Messages.MSG_OPERATOR_NO_METRICS);
    }
    ditaDoc.append("  <dd>" + metricsSummary + "</dd>" + "\n");
    ditaDoc.append(" </dlentry>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Given an operator metrics object, format the metrics section.
   *
   * @param metrics
   * @param URI of operator model file
   * @return metrics section encoded uses DITA
   */
  private String formatMetrics(MetricsType metrics, String fileUri) {
    StringBuffer ditaDoc = new StringBuffer();
    // Metrics - output metrics only if there is a description, or one or more metrics are defined.
    if (metrics != null
        && (metrics.getDescription() != null
            || (metrics.getMetric() != null && !metrics.getMetric().isEmpty()))) {
      ditaDoc.append("<section>" + "\n");
      ditaDoc.append(
          formatTitleLink(
              RefId_Operator_ContextId + "__metrics",
              logger.getMessage(Messages.MSG_TITLE_METRICS)));
      DescriptionType description = metrics.getDescription();
      if (description != null) {
        ditaDoc.append(
            Translater.xlate(description.getValue(), fileUri, new LocationType("metrics:", fileUri))
                + "\n");
      }
      if (metrics.getMetric() != null && !metrics.getMetric().isEmpty()) {
        // Show metrics in sorted order.
        Map<String, MetricType> sortedMetrics = new TreeMap<String, MetricType>();
        for (MetricType metric : metrics.getMetric()) {
          sortedMetrics.put(metric.getName(), metric);
        }
        ditaDoc.append("<dl>\n");
        for (String metricName : sortedMetrics.keySet()) {
          MetricType metric = sortedMetrics.get(metricName);
          String metricKindLiteral = metric.getKind().getLiteral();
          String metricId = RefId_Operator_MetricKindId + "_" + metricKindLiteral.toLowerCase();
          ditaDoc.append("  <dlentry>\n");
          ditaDoc.append(
              "    <dt><tt>"
                  + metricName
                  + "</tt> - "
                  + formatReferenceLink(metricId, metricKindLiteral)
                  + "</dt>\n");
          ditaDoc.append("    <dd>\n");
          DescriptionType desc = metric.getDescription();
          if (desc != null) {
            ditaDoc.append(
                Translater.xlate(
                        desc.getValue(), fileUri, new LocationType("metric:" + metricName, fileUri))
                    + "\n");
          }
          ditaDoc.append("     <p></p>\n");
          ditaDoc.append("     </dd>\n");
          ditaDoc.append("  </dlentry>\n");
        }
        ditaDoc.append("</dl>\n");
      }
      ditaDoc.append("</section>" + "\n");
    }
    return ditaDoc.toString();
  }

  /** Generate DITA doc for C++ operator */
  private String generate(
      PrimitiveToolkitOperatorType primOp, OpModelType opModel, Artifact artifact) {
    String namespace = artifact.namespace;
    String name = primOp.getName();
    String fileUri = currentToolkit.getModelFileNameForOp(primOp);
    String refId = Constants.RefId_Operator;

    // Compute properties used in page
    InputPortsType inputPorts = opModel.getInputPorts();
    boolean hasInputPortOpenSet = false;
    // maxWindowingMode captures must specific windowing mode for all input ports
    WindowingModeType maxWindowingMode = WindowingModeType.NON_WINDOWED;
    int num_inputPorts = 0;
    for (InputPortSetType inputPortSet : inputPorts.getInputPortSet()) {
      num_inputPorts += inputPortSet.getCardinality().intValue();
      WindowingModeType windowingMode = inputPortSet.getWindowingMode();
      if (maxWindowingMode == WindowingModeType.NON_WINDOWED) {
        maxWindowingMode = windowingMode;
      } else if (windowingMode == WindowingModeType.WINDOWED) {
        maxWindowingMode = windowingMode;
      }
    }
    InputPortOpenSetType inputPortOpenSet = inputPorts.getInputPortOpenSet();
    if (inputPortOpenSet != null) {
      hasInputPortOpenSet = true;
      WindowingModeType windowingMode = inputPortOpenSet.getWindowingMode();
      if (maxWindowingMode == WindowingModeType.NON_WINDOWED) {
        maxWindowingMode = windowingMode;
      } else if (windowingMode == WindowingModeType.WINDOWED) {
        maxWindowingMode = windowingMode;
      }
    }
    OutputPortsType outputPorts = opModel.getOutputPorts();
    boolean hasOutputPortOpenSet = (outputPorts.getOutputPortOpenSet() != null ? true : false);
    int num_outputPorts = 0;
    for (OutputPortSetType outputPortSet : outputPorts.getOutputPortSet()) {
      num_outputPorts += outputPortSet.getCardinality().intValue();
    }

    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append(Constants.XHTML_Header_Reference);
    ditaDoc.append("<reference id=\"" + refId + "\">" + "\n");
    ditaDoc.append(
        "<title>"
            + logger.getMessage(
                currentToolkit.isSample()
                    ? Messages.MSG_TITLE_SAMPLE_OPERATOR
                    : Messages.MSG_TITLE_OPERATOR)
            + " <tt>"
            + name
            + "</tt></title>"
            + "\n");
    ditaDoc.append("<refbody>" + "\n");
    ditaDoc.append(generateArtifactPathSection(namespace, name));

    // Include primitive operator diagram, only if the user did not suppress it.
    if (!inputParameters.isSuppressPrimitiveOperatorDiagram()) {
      ditaDoc.append(
          createImageLinkOp(namespace, primOp.getName(), Key.CDISP7012I, OutputClass_PrimOp));
    }

    // Overview description
    ditaDoc.append("<section>" + "\n");
    String desc;
    DescriptionType descriptionType = opModel.getContext().getDescription();
    LocationType location = new LocationType(artifact, fileUri);
    if (descriptionType != null
        && (desc = descriptionType.getValue()) != null
        && ParserSPLDOC.hasDescription(desc)) {
      ditaDoc.append(Translater.xlateAsPages(desc, fileUri, location, artifact));
    } else if (inputParameters.isWarnNoComments()) {
      logger.printWarn(Key.CDISP7088W, location.toString(), new String[] {name});
    }
    ditaDoc.append("\n");
    ditaDoc.append("</section>" + "\n");

    // Summary section
    ditaDoc.append("<section>" + "\n");
    ditaDoc.append(
        "<title outputclass=\"splhead-1\">"
            + logger.getMessage(Messages.MSG_TITLE_SUMMARY)
            + "</title>"
            + "\n");
    ditaDoc.append("<dl compact=\"yes\">" + "\n");
    ditaDoc.append(" <dlentry>" + "\n");
    ditaDoc.append("  <dt>").append(logger.getMessage(Messages.MSG_TITLE_PORTS)).append("</dt>\n");

    String portSummary =
        generatePortSummary(
            hasInputPortOpenSet, num_inputPorts, hasOutputPortOpenSet, num_outputPorts);

    ditaDoc.append("  <dd>" + portSummary + "</dd>" + "\n");
    ditaDoc.append(" </dlentry>" + "\n");
    ditaDoc.append(" <dlentry>" + "\n");
    ditaDoc
        .append("  <dt>")
        .append(logger.getMessage(Messages.MSG_TITLE_WINDOWING))
        .append("</dt>\n");

    String windowingSummary;
    if (maxWindowingMode == WindowingModeType.NON_WINDOWED) {
      windowingSummary = logger.getMessage(Messages.MSG_OPERATOR_WINDOW_NOT_SUPPORTED);
    } else if (maxWindowingMode == WindowingModeType.OPTIONALLY_WINDOWED) {
      windowingSummary = logger.getMessage(Messages.MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL);
    } else {
      windowingSummary = logger.getMessage(Messages.MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED);
    }

    ditaDoc.append("  <dd>" + windowingSummary + "</dd>" + "\n");
    ditaDoc.append(" </dlentry>" + "\n");
    ditaDoc.append(" <dlentry>" + "\n");
    ditaDoc
        .append("  <dt>")
        .append(logger.getMessage(Messages.MSG_TITLE_PARAMETERS))
        .append("</dt>\n");

    ditaDoc.append("  <dd>");
    ParametersType params = opModel.getParameters();
    ditaDoc.append(getParametersMessage(params));
    listParameters(ditaDoc, params, refId);
    ditaDoc.append("</dd>" + "\n");
    ditaDoc.append(" </dlentry>" + "\n");
    ditaDoc.append(formatMetricsSummary(opModel.getContext().getMetrics(), fileUri));
    ditaDoc.append("</dl>");
    ditaDoc.append("</section>" + "\n");

    // Operator properties
    ditaDoc.append("<section>" + "\n");
    ditaDoc.append(
        "<title outputclass=\"splhead-1\">"
            + logger.getMessage(Messages.MSG_TITLE_PROPERTIES)
            + "</title>"
            + "\n");
    ParametersType parameters = opModel.getParameters();

    ditaDoc.append("<dl compact=\"yes\">" + "\n");
    ditaDoc.append(" <dlentry>" + "\n");
    ditaDoc
        .append("  <dt>")
        .append(logger.getMessage(Messages.MSG_TITLE_IMPLEMENTATION))
        .append("</dt>\n");
    ditaDoc.append("  <dd>" + "C++" + "</dd>" + "\n");
    ditaDoc.append(" </dlentry>" + "\n");

    EList<String> capList = opModel.getContext().getCapability();
    if (capList != null) {
      StringBuilder sb = new StringBuilder();
      for (String capString : capList) {
        sb.append(capString + " ");
      }
      String allCapabilities = sb.toString().trim();
      if (!allCapabilities.isEmpty()) {
        String capabilities[] = allCapabilities.split("\\s+");
        ditaDoc.append(" <dlentry>" + "\n");
        ditaDoc.append(
            "  <dt>"
                + formatReferenceLink(
                    OperatorModelRefPath
                        + "#"
                        + References.OperatorModelRefId
                        + "/"
                        + References.OperatorModelContextId
                        + "__capability",
                    logger.getMessage(Messages.MSG_TITLE_CAPABILITIES))
                + "</dt>"
                + "\n");
        for (String capability : capabilities) {
          ditaDoc.append("  <dd>" + capability + "</dd>" + "\n");
        }
        ditaDoc.append(" </dlentry>" + "\n");
      }
    }

    ditaDoc.append(" <dlentry>" + "\n");

    String threadingContextId =
        OperatorModelRefPath
            + "#"
            + References.OperatorModelRefId
            + "/"
            + References.OperatorModelContextId
            + "__providessinglethreadedexecutioncontext";

    ditaDoc.append(
        "  <dt>"
            + formatReferenceLink(
                threadingContextId, logger.getMessage(Messages.MSG_TITLE_THREADING))
            + "</dt>"
            + "\n");

    // Format threading context: <literal name> - <description>
    String threadedContextMsg = "<tt>";
    threadedContextMsg += opModel.getContext().getProvidesSingleThreadedContext().getLiteral();
    threadedContextMsg += "</tt> - ";
    switch (opModel.getContext().getProvidesSingleThreadedContext().getValue()) {
      case SingleThreadedContextType.NEVER_VALUE:
        threadedContextMsg += logger.getMessage(Messages.MSG_SINGLE_THREADING_CONTEXT_NEVER);
        break;
      case SingleThreadedContextType.ALWAYS_VALUE:
        threadedContextMsg += logger.getMessage(Messages.MSG_SINGLE_THREADING_CONTEXT_ALWAYS);
        break;
      case SingleThreadedContextType.WINDOW_BOUND_VALUE:
        threadedContextMsg += logger.getMessage(Messages.MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND);
        break;
      case SingleThreadedContextType.WINDOW_EVICTION_BOUND_VALUE:
        threadedContextMsg +=
            logger.getMessage(Messages.MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND);
        break;
      case SingleThreadedContextType.WINDOW_TRIGGER_BOUND_VALUE:
        threadedContextMsg +=
            logger.getMessage(Messages.MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND);
        break;
      case SingleThreadedContextType.WINDOW_PARTITION_EVICTION_BOUND_VALUE:
        threadedContextMsg +=
            logger.getMessage(Messages.MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND);
        break;
      default:
        // Should not occur - Use just literal
    }
    ditaDoc.append("  <dd>" + threadedContextMsg + "</dd>" + "\n");
    ditaDoc.append(" </dlentry>" + "\n");
    ditaDoc.append("</dl>\n");
    ditaDoc.append("</section>" + "\n");

    if (num_inputPorts > 0 || hasInputPortOpenSet) {
      // Input ports section
      ditaDoc.append("<section>" + "\n");
      ditaDoc.append(
          formatTitleLink(
              RefId_Operator_InputPortSetId, logger.getMessage(Messages.MSG_TITLE_INPUT_PORTS)));
      // ditaDoc.append("<title outputclass=\"splhead-1\">" + "Input Ports" + "</title>" + "\n");
      // Count input ports in each set. Use the count to initialize the range in the label
      int count_inputPorts = 0;
      ditaDoc.append("<dl>\n");
      for (InputPortSetType inputPortSet : inputPorts.getInputPortSet()) {
        int start_inputPorts = count_inputPorts;
        count_inputPorts += inputPortSet.getCardinality().intValue();
        String range = portRangeString(start_inputPorts, count_inputPorts - 1);
        ditaDoc.append("  <dlentry>\n");
        ditaDoc
            .append("   <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PORTS))
            .append(" ")
            .append(range)
            .append("</dt>\n");
        ditaDoc.append("   <dd>");
        DescriptionType description = inputPortSet.getDescription();
        if (description != null) {
          ditaDoc.append(
              Translater.xlate(
                      description.getValue(),
                      fileUri,
                      new LocationType("inputPortSet:" + range, fileUri))
                  + "\n");
        }
        ditaDoc.append("   <dl>\n");
        String winDescription;
        if (inputPortSet.getWindowingDescription() != null
            && (winDescription = inputPortSet.getWindowingDescription().getValue()) != null
            && !winDescription.equals("")) {
          ditaDoc.append("    <dlentry>\n");
          ditaDoc
              .append("      <dt>")
              .append(logger.getMessage(Messages.MSG_TITLE_WINDOWING))
              .append("</dt>\n");
          ditaDoc.append("      <dd>\n");
          ditaDoc.append(
              Translater.xlate(
                  winDescription,
                  fileUri,
                  new LocationType("inputPortSet:" + range + ":windowingDescription", fileUri)));
          ditaDoc.append("      </dd>\n");
          ditaDoc.append("    </dlentry>\n");
        }
        ditaDoc.append("    <dlentry>\n");
        ditaDoc
            .append("      <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PROPERTIES))
            .append("</dt>\n");
        ditaDoc.append("      <dd>");
        ditaDoc = getInputPortSetConfigurationMarkup(ditaDoc, inputPortSet);
        ditaDoc.append("      </dd>");
        ditaDoc.append("    </dlentry>\n");
        ditaDoc.append("   </dl>\n");
        ditaDoc.append("   <p></p>");
        ditaDoc.append("   </dd>\n");
        ditaDoc.append("  </dlentry>\n");
      }
      if (inputPorts.getInputPortOpenSet() != null) {
        inputPortOpenSet = inputPorts.getInputPortOpenSet();
        String range = portRangeString(count_inputPorts, -1);
        ditaDoc.append("  <dlentry>\n");
        ditaDoc
            .append("   <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PORTS))
            .append(" ")
            .append(range)
            .append("</dt>\n");
        ditaDoc.append("   <dd>\n");
        DescriptionType description = inputPortOpenSet.getDescription();
        if (description != null) {
          ditaDoc.append(
              Translater.xlate(
                      description.getValue(),
                      fileUri,
                      new LocationType("inputPortOpenSet:" + range, fileUri))
                  + "\n");
        }
        ditaDoc.append("   <dl>\n");
        String winDescription;
        if (inputPortOpenSet.getWindowingDescription() != null
            && (winDescription = inputPortOpenSet.getWindowingDescription().getValue()) != null
            && !winDescription.equals("")) {
          ditaDoc.append("    <dlentry>\n");
          ditaDoc
              .append("      <dt>")
              .append(logger.getMessage(Messages.MSG_TITLE_WINDOWING))
              .append("</dt>\n");
          ditaDoc.append("      <dd>\n");
          ditaDoc.append(
              Translater.xlate(
                  winDescription,
                  fileUri,
                  new LocationType(
                      "inputPortOpenSet:" + range + ":windowingDescription", fileUri)));
          ditaDoc.append("      </dd>\n");
          ditaDoc.append("    </dlentry>\n");
        }
        ditaDoc.append("    <dlentry>\n");
        ditaDoc
            .append("      <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PROPERTIES))
            .append("</dt>\n");
        ditaDoc.append("      <dd>");
        ditaDoc = getInputPortSetConfigurationMarkup(ditaDoc, inputPortOpenSet);
        ditaDoc.append("      </dd>");
        ditaDoc.append("    </dlentry>\n");
        ditaDoc.append("   </dl>\n");
        ditaDoc.append("   <p></p>\n");
        ditaDoc.append("   </dd>\n");
        ditaDoc.append("  </dlentry>\n");
      }
      ditaDoc.append("</dl>\n");
      ditaDoc.append("</section>" + "\n");
    }

    // Output ports

    // Determine whether assignment mode is the same for all output ports.  If so,
    // output a single description that applies to all output ports.  If not,
    // output a separate description for each port.
    EList<OutputPortSetType> outputPortSetList = outputPorts.getOutputPortSet();
    boolean sameAssignmentMode = true;
    ExpressionModeType expressionMode = null;
    Boolean autoAssignment = null;
    Boolean completeAssignment = null;
    if (outputPortSetList.size() > 0) {
      OutputPortSetType outputPortSet = outputPortSetList.get(0);
      expressionMode = outputPortSet.getExpressionMode();
      autoAssignment = outputPortSet.isAutoAssignment();
      completeAssignment = outputPortSet.isCompleteAssignment();
      for (int i = 1; i < outputPortSetList.size(); i++) {
        outputPortSet = outputPortSetList.get(i);
        if (expressionMode != outputPortSet.getExpressionMode()
            || autoAssignment != outputPortSet.isAutoAssignment()
            || completeAssignment != outputPortSet.isCompleteAssignment()) {
          sameAssignmentMode = false;
          break;
        }
      }
    }
    if (outputPorts.getOutputPortOpenSet() != null) {
      OutputPortOpenSetType outputPortOpenSet = outputPorts.getOutputPortOpenSet();
      if (expressionMode == null) {
        expressionMode = outputPortOpenSet.getExpressionMode();
        autoAssignment = outputPortOpenSet.isAutoAssignment();
        completeAssignment = outputPortOpenSet.isCompleteAssignment();
      } else {
        if (expressionMode != outputPortOpenSet.getExpressionMode()) {
          sameAssignmentMode = false;
        } else if (autoAssignment != outputPortOpenSet.isAutoAssignment()) {
          sameAssignmentMode = false;
        } else if (completeAssignment != outputPortOpenSet.isCompleteAssignment()) {
          sameAssignmentMode = false;
        }
      }
    }

    if (num_outputPorts > 0 || hasOutputPortOpenSet) {
      ditaDoc.append("<section>" + "\n");
      ditaDoc.append(
          formatTitleLink(
              RefId_Operator_OutputPortSetId, logger.getMessage(Messages.MSG_TITLE_OUTPUT_PORTS)));
      // ditaDoc.append("<title outputclass=\"splhead-1\">" + "Output Ports" + "</title>" + "\n");

      if (sameAssignmentMode) {
        ditaDoc.append("<dl>\n");
        ditaDoc.append("  <dlentry>\n");
        ditaDoc
            .append("     <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_ASSIGNMENTS))
            .append("</dt>\n");
        ditaDoc.append(
            "     <dd>"
                + outputPortAssignmentText(
                    expressionMode, autoAssignment, completeAssignment, SubjectType.Operator)
                + "</dd>\n");
        ditaDoc.append("  </dlentry>\n");
        ditaDoc.append("</dl>\n");
      }
      // Generate doc for custom output function, if any.
      ContextType context = opModel.getContext();
      assert (context != null);
      CustomOutputFunctionsType outputFunctions = context.getCustomOutputFunctions();
      EList<CustomOutputFunctionSetType> outputFunctionSets;
      if (outputFunctions != null
          && (outputFunctionSets = outputFunctions.getCustomOutputFunction()).size() > 0) {
        String outputFunctionsId =
            OperatorModelRefPath
                + "#"
                + References.OperatorModelRefId
                + "/"
                + References.OperatorModelContextId
                + "__customoutputfunctions";
        ditaDoc.append("<dl>\n");
        ditaDoc.append("  <dlentry>");
        ditaDoc.append(
            "    <dt>"
                + formatReferenceLink(
                    outputFunctionsId, logger.getMessage(Messages.MSG_TITLE_OUTPUT_FUNCTIONS))
                + "</dt>\n");
        ditaDoc.append("    <dd>\n");
        ditaDoc.append("      <dl>\n");
        for (CustomOutputFunctionSetType outputFunctionSet : outputFunctionSets) {
          ditaDoc.append("      <dlentry>\n");
          ditaDoc.append("        <dt>" + outputFunctionSet.getName() + "</dt>\n");
          ditaDoc.append("        <dd>\n");
          ditaDoc.append("          <dl>\n");
          // An empty definition list entry avoids an error if there are no other entries.
          ditaDoc.append("            <dlentry><dt></dt><dd></dd></dlentry>\n");
          for (CustomOutputFunctionType outputFunction : outputFunctionSet.getFunction()) {
            ditaDoc.append("            <dlentry>\n");
            ditaDoc.append("              <dt>\n");
            ditaDoc.append("                <tt><![CDATA[");
            ditaDoc.append("   " + outputFunction.getPrototype());
            ditaDoc.append("]]></tt>");
            ditaDoc.append("              </dt>\n");
            ditaDoc.append("            <dd>\n");
            DescriptionType description;
            if ((description = outputFunction.getDescription()) != null) {
              ditaDoc.append(
                  Translater.xlate(
                      description.getValue(),
                      fileUri,
                      new LocationType(
                          "customOutputFunction:" + outputFunction.getPrototype(), fileUri)));
            }
            ditaDoc.append("              <p></p>\n");
            ditaDoc.append("            </dd>\n");
            ditaDoc.append("            </dlentry>");
          } // for ...
          ditaDoc.append("          </dl>");
          ditaDoc.append("        </dd>\n");
          ditaDoc.append("      </dlentry>\n");
        } // for (CustomOutputFunctionSetType ...
        ditaDoc.append("      </dl>\n"); // function sets
        ditaDoc.append("    </dd>\n"); // Output Functions
        ditaDoc.append("  </dlentry>");
        ditaDoc.append("</dl>\n");
      }
      // Count output ports in each set. Use the count to initialize the range in the label
      int count_outputPorts = 0;
      boolean defList = false;
      for (OutputPortSetType outputPortSet : outputPorts.getOutputPortSet()) {
        int start_outputPorts = count_outputPorts;
        count_outputPorts += outputPortSet.getCardinality().intValue();
        String range = portRangeString(start_outputPorts, count_outputPorts - 1);
        ditaDoc.append(defList ? "" : " <dl>\n");
        defList = true;
        ditaDoc.append("  <dlentry>\n");
        ditaDoc
            .append("   <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PORTS))
            .append(" ")
            .append(range)
            .append("</dt>\n");
        DescriptionType description = outputPortSet.getDescription();
        ditaDoc.append("   <dd>\n");
        if (description != null) {
          ditaDoc.append(
              Translater.xlate(
                      description.getValue(),
                      fileUri,
                      new LocationType("outputPortSet:" + range, fileUri))
                  + "\n");
        }
        if (!sameAssignmentMode) {
          ditaDoc = getOutputPortAssignmentMarkup(ditaDoc, outputPortSet);
        }
        ditaDoc.append("    <p></p>\n");
        ditaDoc.append("    <dl>\n");
        ditaDoc.append("     <dlentry>\n");
        ditaDoc
            .append("      <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PROPERTIES))
            .append("</dt>\n");
        ditaDoc.append("      <dd>");
        ditaDoc = getOutputPortSetConfigurationMarkup(ditaDoc, outputPortSet);
        ditaDoc.append("</dd>");
        ditaDoc.append("     </dlentry>\n");
        ditaDoc.append("    </dl>\n");
        ditaDoc.append("    <p></p>\n");
        ditaDoc.append("   </dd>\n");
        ditaDoc.append("  </dlentry>\n");
      }
      OutputPortOpenSetType outputPortOpenSet = outputPorts.getOutputPortOpenSet();
      if (outputPortOpenSet != null) {
        String range = portRangeString(count_outputPorts, -1);
        ditaDoc.append((defList) ? "" : "<dl>\n");
        defList = true;
        ditaDoc.append("  <dlentry>\n");
        ditaDoc
            .append("   <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PORTS))
            .append(" ")
            .append(range)
            .append("</dt>\n");
        DescriptionType description = outputPortOpenSet.getDescription();
        ditaDoc.append("   <dd>\n");
        if (description != null) {
          ditaDoc.append(
              Translater.xlate(
                      description.getValue(),
                      fileUri,
                      new LocationType("outputPortOpenSet:" + range, fileUri))
                  + "\n");
        }
        ditaDoc.append("   <p></p>\n");
        if (!sameAssignmentMode) {
          ditaDoc = getOutputPortAssignmentMarkup(ditaDoc, outputPortOpenSet);
        }
        ditaDoc.append("    <p></p>\n");
        ditaDoc.append("   <dl>\n");
        ditaDoc.append("    <dlentry>\n");
        ditaDoc
            .append("      <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PROPERTIES))
            .append("</dt>\n");
        ditaDoc.append("      <dd>");
        ditaDoc = getOutputPortSetConfigurationMarkup(ditaDoc, outputPortOpenSet);
        ditaDoc.append("</dd>");
        ditaDoc.append("    </dlentry>\n");
        ditaDoc.append("   </dl>\n");
        ditaDoc.append("   <p></p>\n");
        ditaDoc.append("   </dd>\n");
        ditaDoc.append("  </dlentry>\n");
      }
      ditaDoc.append(defList ? "</dl>\n" : "");
      ditaDoc.append("</section>" + "\n");
    }

    // Parameters - output description and properties of parameters, if any
    if (parameters != null && !parameters.getParameter().isEmpty()) {
      ditaDoc.append("<section>" + "\n");
      ditaDoc.append(
          formatTitleLink(
              RefId_Operator_ParametersId, logger.getMessage(Messages.MSG_TITLE_PARAMETERS)));

      DescriptionType summaryDescription = parameters.getDescription();
      if (summaryDescription != null && summaryDescription.getValue() != null) {
        ditaDoc.append(
            Translater.xlate(
                    summaryDescription.getValue(), fileUri, new LocationType("parameters", fileUri))
                + "\n");
      } else {
        ditaDoc.append(getParametersMessage(parameters));
      }
      listParameters(ditaDoc, parameters, refId);

      // Show parameters in sorted order.
      Map<String, ParameterType> sortedParams = new TreeMap<String, ParameterType>();
      for (ParameterType parameter : parameters.getParameter()) {
        sortedParams.put(parameter.getName(), parameter);
      }
      ditaDoc.append("<dl>\n");
      for (String parameterName : sortedParams.keySet()) {
        ParameterType parameter = sortedParams.get(parameterName);
        ditaDoc.append("  <dlentry id=\"parameter__" + parameterName + "\">\n");
        ditaDoc.append("     <dt><tt>" + parameterName + "</tt></dt>\n");
        DescriptionType description = parameter.getDescription();
        ditaDoc.append("     <dd>");
        if (description != null && description.getValue() != null) {
          ditaDoc.append(
              Translater.xlate(
                      description.getValue(),
                      fileUri,
                      new LocationType("parameter:" + parameterName, fileUri))
                  + "\n");
        }
        ditaDoc.append("  <dl>\n");
        ditaDoc.append("    <dlentry>\n");
        ditaDoc.append("     <dt>" + logger.getMessage(Messages.MSG_TITLE_PROPERTIES) + "</dt>\n");
        ditaDoc.append("     <dd>");
        ditaDoc = getParameterConfigurationMarkup(ditaDoc, parameter, opModel);
        ditaDoc.append("     <p></p>");
        ditaDoc.append("     </dd>\n");
        ditaDoc.append("    </dlentry>\n");
        ditaDoc.append("  </dl>\n");
        ditaDoc.append("     </dd>\n");
        ditaDoc.append("  </dlentry>\n");
      }
      ditaDoc.append("</dl>\n");
      ditaDoc.append("</section>" + "\n");
    }

    // Format templates that show how to invoke operator
    ditaDoc.append(formatCodeTemplates(opModel.getContext().getCodeTemplates()));

    // Metrics - output metrics only if one or more metrics are defined.
    ditaDoc.append(formatMetrics(opModel.getContext().getMetrics(), fileUri));

    // Libraries - output library dependencies only if one or more libraries are defined.
    LibraryDependenciesType libDeps = opModel.getContext().getLibraryDependencies();
    EList<LibraryType> libraries = null;
    if (libDeps != null) {
      libraries = libDeps.getLibrary();
    }
    if (libraries != null && libraries.size() > 0) {
      ditaDoc.append("<section>" + "\n");
      ditaDoc.append(
          formatTitleLink(
              RefId_Operator_Dependencies, logger.getMessage(Messages.MSG_TITLE_LIBRARIES)));
      // ditaDoc.append("<title outputclass=\"splhead-1\">" + "Libraries" + "</title>" + "\n");
      // ditaDoc.append(formatReferenceLink(dependenciesId, "Libraries"));

      ditaDoc.append("<dl>\n");
      for (LibraryType lib : libraries) {
        ditaDoc.append(" <dlentry>\n");
        DescriptionType description = lib.getDescription();
        ditaDoc.append("  <dt>");
        if (description != null && description.getValue().length() > 0) {
          ditaDoc.append("<tt>");
          ditaDoc.append(Translater.xlateAsText(lib.getDescription().getValue(), fileUri));
          ditaDoc.append("</tt>");
        } else {
          ditaDoc.append(logger.getMessage(Messages.MSG_NO_DESC_LIB)).append("\n");
        }
        ditaDoc.append("  </dt>\n");
        // Generate at least one empty dd to satify DITA schema
        ditaDoc.append("<dd/>");
        ManagedLibraryType managedLib = lib.getManagedLibrary();
        if (managedLib != null) {
          String command = managedLib.getCommand();
          if (command != null) {
            ditaDoc.append(
                "  <dd>"
                    + formatProperty("Command", command, RefId_Operator_Dependencies_ManagedLibId)
                    + "  </dd>\n");
          }
          EList<String> managedLibNames = managedLib.getLib();
          if (managedLibNames != null && managedLibNames.size() > 0) {
            String libNameList = commaSeparatedList(managedLibNames);
            if (libNameList.length() > 0) {
              String libNameLink =
                  formatReferenceLink(
                      RefId_Operator_Dependencies_LibId,
                      logger.getMessage(Messages.MSG_TITLE_LIBRARY_NAME));
              ditaDoc.append("  <dd>" + libNameLink + ": <tt>" + libNameList + "</tt></dd>\n");
            }
          }
          EList<String> libraryPaths = managedLib.getLibPath();
          if (libraryPaths != null && libraryPaths.size() > 0) {
            String libPathList = commaSeparatedList(libraryPaths);
            if (libPathList.length() > 0) {
              String libPathLink =
                  formatReferenceLink(
                      RefId_Operator_Dependencies_LibPathId,
                      logger.getMessage(Messages.MSG_TITLE_LIBRARY_PATH));
              ditaDoc.append("  <dd>" + libPathLink + ": <tt>" + libPathList + "</tt></dd>\n");
            }
          }
          EList<String> includePaths = managedLib.getIncludePath();
          if (includePaths != null && includePaths.size() > 0) {
            String includePathList = commaSeparatedList(includePaths);
            if (includePathList.length() > 0) {
              String includePathLink =
                  formatReferenceLink(RefId_Operator_Dependencies_IncludePathId, "Include Path");
              ditaDoc.append(
                  "  <dd>" + includePathLink + ": <tt>" + includePathList + "</tt></dd>\n");
            }
          }
        }
        ditaDoc.append(" </dlentry>\n");
      } // for ( ...
      ditaDoc.append("</dl>\n");
      ditaDoc.append("</section>" + "\n");
    }

    ditaDoc.append("</refbody>" + "\n");
    ditaDoc.append("</reference>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Generate a description of the permitted numbers of input ports and output ports.
   *
   * @param hasInputPortOpenSet whether the input port set has no upper bound
   * @param num_inputPorts if hasInputPortOpenSet is true, this is the minimum number of input ports
   *     required. Otherwise, it is the exact number of input ports required.
   * @param hasOutputPortOpenSet whether the output port set has no upper bound
   * @param num_outputPorts if hasOutputPortOpenSet is true, this is the minimum number of output
   *     ports required. Otherwise, it is the exact number of output ports required.
   * @return A description of the required numbers of input and output ports.
   */
  private String generatePortSummary(
      boolean hasInputPortOpenSet,
      int num_inputPorts,
      boolean hasOutputPortOpenSet,
      int num_outputPorts) {
    String portSummaryMsg = "MSG_IN_OUT";
    String portSummary = "";
    String[] args = {String.valueOf(num_inputPorts), String.valueOf(num_outputPorts)};
    if (hasInputPortOpenSet) {
      portSummaryMsg += "_PLURALM";
    } else {
      if (1 == num_inputPorts) {
        portSummaryMsg += "_SINGLE";
      } else {
        portSummaryMsg += "_PLURAL";
      }
    }
    if (hasOutputPortOpenSet) {
      portSummaryMsg += "_PLURALM";
    } else {
      if (1 == num_outputPorts) {
        portSummaryMsg += "_SINGLE";
      } else {
        portSummaryMsg += "_PLURAL";
      }
    }
    String portSummaryKey;
    switch (portSummaryMsg) {
      case "MSG_IN_OUT_PLURAL_PLURAL":
        portSummaryKey = Messages.MSG_IN_OUT_PLURAL_PLURAL;
        break;
      case "MSG_IN_OUT_PLURAL_PLURALM":
        portSummaryKey = Messages.MSG_IN_OUT_PLURAL_PLURALM;
        break;
      case "MSG_IN_OUT_PLURALM_PLURAL":
        portSummaryKey = Messages.MSG_IN_OUT_PLURALM_PLURAL;
        break;
      case "MSG_IN_OUT_PLURALM_PLURALM":
        portSummaryKey = Messages.MSG_IN_OUT_PLURALM_PLURALM;
        break;
      case "MSG_IN_OUT_SINGLE_PLURAL":
        portSummaryKey = Messages.MSG_IN_OUT_SINGLE_PLURAL;
        break;
      case "MSG_IN_OUT_SINGLE_PLURALM":
        portSummaryKey = Messages.MSG_IN_OUT_SINGLE_PLURALM;
        break;
      case "MSG_IN_OUT_SINGLE_SINGLE":
        // This is a special case.  Since there are no insertions,
        // the message must not be processed with MessageFormat.
        // Otherwise, apostrophes will be removed from messages
        // in languages that use apostrophes in the translation
        // of this message.  Making args empty  will prevent the
        // use of MessageFormat.
        portSummaryKey = Messages.MSG_IN_OUT_SINGLE_SINGLE;
        args = new String[0];
        ;
        break;
      case "MSG_IN_OUT_PLURAL_SINGLE":
        portSummaryKey = Messages.MSG_IN_OUT_PLURAL_SINGLE;
        break;
      case "MSG_IN_OUT_PLURALM_SINGLE":
        portSummaryKey = Messages.MSG_IN_OUT_PLURALM_SINGLE;
        break;
      default:
        portSummaryKey = Messages.MSG_IN_OUT_PLURAL_PLURAL;
    }
    portSummary = logger.getMessage(portSummaryMsg, args);
    return portSummary;
  }

  /** Generate a DITA doc for Java primitive operator. */
  private String generate(
      PrimitiveToolkitOperatorType primOp, JavaOpModelType opModel, Artifact artifact) {
    String namespace = artifact.namespace;
    String name = primOp.getName();
    String fileUri = currentToolkit.getModelFileNameForOp(primOp);
    String refId = Constants.RefId_Operator;
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append(Constants.XHTML_Header_Reference);
    ditaDoc.append("<reference id=\"" + refId + "\">" + "\n");
    ditaDoc.append(
        "<title>"
            + logger.getMessage(
                currentToolkit.isSample()
                    ? Messages.MSG_TITLE_SAMPLE_OPERATOR
                    : Messages.MSG_TITLE_OPERATOR)
            + " <tt>"
            + name
            + "</tt></title>"
            + "\n");
    ditaDoc.append("<refbody>" + "\n");
    ditaDoc.append(generateArtifactPathSection(namespace, name));

    JavaOpContextType context = opModel.getContext();
    assert (context != null);

    // Include primitive operator diagram, only if the user did not suppress it.
    if (!inputParameters.isSuppressPrimitiveOperatorDiagram()) {
      ditaDoc.append(
          createImageLinkOp(namespace, primOp.getName(), Key.CDISP7012I, OutputClass_PrimOp));
    }

    // Overview description
    ditaDoc.append("<section>" + "\n");
    String desc;
    DescriptionType descriptionType = context.getDescription();
    LocationType location = new LocationType(artifact, fileUri);
    if (descriptionType != null
        && (desc = descriptionType.getValue()) != null
        && ParserSPLDOC.hasDescription(desc)) {
      ditaDoc.append(Translater.xlateAsPages(desc, fileUri, location, artifact));
    } else if (inputParameters.isWarnNoComments()) {
      logger.printWarn(Key.CDISP7088W, location.toString(), new String[] {name});
    }
    ditaDoc.append("\n");
    ditaDoc.append("</section>" + "\n");

    // Summary section
    // Set properties for summary section
    // maxWindowingMode captures most specific windowing mode for all input ports
    WindowingModeType maxWindowingMode = WindowingModeType.NON_WINDOWED;
    int num_inputPorts = 0;
    boolean hasInputPortOpenSet = false;
    JavaOpInputPortsType inputPorts = opModel.getInputPorts();
    if (inputPorts != null) {
      num_inputPorts = inputPorts.getInputPortSet().size();
      if (inputPorts.getInputPortOpenSet() != null) {
        hasInputPortOpenSet = true;
      }

      for (JavaOpInputPortSetType inputPortSet : inputPorts.getInputPortSet()) {
        WindowingModeType windowingMode = inputPortSet.getWindowingMode();
        if (maxWindowingMode == WindowingModeType.NON_WINDOWED) {
          maxWindowingMode = windowingMode;
        } else if (windowingMode == WindowingModeType.WINDOWED) {
          maxWindowingMode = windowingMode;
        }
      }
      JavaOpInputPortOpenSetType inputPortOpenSet = inputPorts.getInputPortOpenSet();
      if (inputPortOpenSet != null) {
        WindowingModeType windowingMode = inputPortOpenSet.getWindowingMode();
        if (maxWindowingMode == WindowingModeType.NON_WINDOWED) {
          maxWindowingMode = windowingMode;
        } else if (windowingMode == WindowingModeType.WINDOWED) {
          maxWindowingMode = windowingMode;
        }
      }
    }
    int num_outputPorts = 0;
    boolean hasOutputPortOpenSet = false;
    JavaOpOutputPortsType outputPorts = opModel.getOutputPorts();
    if (outputPorts != null) {
      num_outputPorts = outputPorts.getOutputPortSet().size();
      if (outputPorts.getOutputPortOpenSet() != null) {
        hasOutputPortOpenSet = true;
      }
    }

    ditaDoc.append("<section>" + "\n");
    ditaDoc.append(
        "<title outputclass=\"splhead-1\">"
            + logger.getMessage(Messages.MSG_TITLE_SUMMARY)
            + "</title>"
            + "\n");
    ditaDoc.append("<dl compact=\"yes\">" + "\n");
    ditaDoc.append(" <dlentry>" + "\n");
    ditaDoc.append("  <dt>").append(logger.getMessage(Messages.MSG_TITLE_PORTS)).append("</dt>\n");

    String portSummary =
        generatePortSummary(
            hasInputPortOpenSet, num_inputPorts, hasOutputPortOpenSet, num_outputPorts);
    ditaDoc.append("  <dd>" + portSummary + "</dd>" + "\n");
    ditaDoc.append(" </dlentry>" + "\n");
    ditaDoc.append(" <dlentry>" + "\n");
    ditaDoc
        .append("  <dt>")
        .append(logger.getMessage(Messages.MSG_TITLE_WINDOWING))
        .append("</dt>\n");

    String windowingSummary;
    if (maxWindowingMode == WindowingModeType.NON_WINDOWED) {
      windowingSummary = logger.getMessage(Messages.MSG_OPERATOR_WINDOW_NOT_SUPPORTED);
    } else if (maxWindowingMode == WindowingModeType.OPTIONALLY_WINDOWED) {
      windowingSummary = logger.getMessage(Messages.MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL);
    } else {
      windowingSummary = logger.getMessage(Messages.MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED);
    }

    ditaDoc.append("  <dd>" + windowingSummary + "</dd>" + "\n");
    ditaDoc.append(" </dlentry>" + "\n");
    ditaDoc.append(" <dlentry>" + "\n");
    ditaDoc
        .append("  <dt>")
        .append(logger.getMessage(Messages.MSG_TITLE_PARAMETERS))
        .append("</dt>\n");

    // Generate a summary for the parameters.
    JavaOpParametersType params = opModel.getParameters();
    ditaDoc.append("  <dd>");
    ditaDoc.append(getParametersMessage(params));
    listParameters(ditaDoc, params, refId);
    ditaDoc.append("</dd>" + "\n");
    ditaDoc.append(" </dlentry>" + "\n");
    ditaDoc.append(formatMetricsSummary(context.getMetrics(), fileUri));
    ditaDoc.append("</dl>");
    ditaDoc.append("</section>" + "\n");

    // Operator properties
    ditaDoc.append("<section>" + "\n");
    ditaDoc.append(
        "<title outputclass=\"splhead-1\">"
            + logger.getMessage(Messages.MSG_TITLE_PROPERTIES)
            + "</title>"
            + "\n");
    ditaDoc.append("<dl compact=\"yes\">" + "\n");
    ditaDoc.append(" <dlentry>" + "\n");
    ditaDoc
        .append("  <dt>")
        .append(logger.getMessage(Messages.MSG_TITLE_IMPLEMENTATION))
        .append("</dt>\n");
    ditaDoc.append("  <dd>" + "Java" + "</dd>" + "\n");
    ditaDoc.append(" </dlentry>" + "\n");
    ditaDoc.append("</dl>\n");
    ditaDoc.append("</section>" + "\n");

    if (num_inputPorts > 0 || hasInputPortOpenSet) {
      // Input ports section
      ditaDoc.append("<section>" + "\n");
      ditaDoc.append(
          formatTitleLink(
              RefId_Operator_InputPortSetId, logger.getMessage(Messages.MSG_TITLE_INPUT_PORTS)));
      // ditaDoc.append("<title outputclass=\"splhead-1\">" + "Input Ports" + "</title>" + "\n");
      // Count input ports in each set. Use the count to initialize the range in the label
      int count_inputPorts = 0;
      ditaDoc.append("<dl>\n");
      for (JavaOpInputPortSetType inputPortSet : inputPorts.getInputPortSet()) {
        int start_inputPorts = count_inputPorts;
        count_inputPorts += inputPortSet.getCardinality().intValue();
        String range = portRangeString(start_inputPorts, count_inputPorts - 1);
        ditaDoc.append("  <dlentry>\n");
        ditaDoc
            .append("   <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PORTS))
            .append(" ")
            .append(range)
            .append("</dt>\n");
        DescriptionType description = null;
        ditaDoc.append("   <dd>\n");
        if ((description = inputPortSet.getDescription()) != null) {
          ditaDoc.append(
              Translater.xlate(
                  description.getValue(),
                  fileUri,
                  new LocationType("inputPortSet:" + range, fileUri)));
        }
        ditaDoc.append("   </dd>\n");
        ditaDoc.append("  </dlentry>\n");
        String winDescription;
        if (inputPortSet.getWindowingDescription() != null
            && (winDescription = inputPortSet.getWindowingDescription().getValue()) != null
            && !winDescription.equals("")) {
          ditaDoc.append("    <dlentry>\n");
          ditaDoc
              .append("      <dt>")
              .append(logger.getMessage(Messages.MSG_TITLE_WINDOWING))
              .append("</dt>\n");
          ditaDoc.append("      <dd>\n");
          ditaDoc.append(
              Translater.xlate(
                  winDescription,
                  fileUri,
                  new LocationType("inputPortSet:" + range + ":windowingDescription", fileUri)));
          ditaDoc.append("      </dd>\n");
          ditaDoc.append("    </dlentry>\n");
        }
        ditaDoc.append("    <dlentry>\n");
        ditaDoc
            .append("      <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PROPERTIES))
            .append("</dt>\n");
        ditaDoc.append("      <dd>\n");
        ditaDoc = getInputPortSetConfigurationMarkup(ditaDoc, inputPortSet);
        ditaDoc.append("      </dd>\n");
        ditaDoc.append("    </dlentry>\n");
      }
      if (inputPorts.getInputPortOpenSet() != null) {
        String range = portRangeString(count_inputPorts, -1);
        JavaOpInputPortOpenSetType inputPortOpenSet = inputPorts.getInputPortOpenSet();
        ditaDoc.append("  <dlentry>\n");
        ditaDoc
            .append("   <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PORTS))
            .append(" ")
            .append(range)
            .append("</dt>\n");
        ditaDoc.append("    <dd>\n");
        DescriptionType description = inputPortOpenSet.getDescription();
        if (description != null) {
          ditaDoc.append(
              Translater.xlate(
                  description.getValue(),
                  fileUri,
                  new LocationType("inputPortOpenSet:" + range, fileUri)));
        }
        ditaDoc.append("   <dl>\n");
        String winDescription;
        if (inputPortOpenSet.getWindowingDescription() != null
            && (winDescription = inputPortOpenSet.getWindowingDescription().getValue()) != null
            && !winDescription.equals("")) {
          ditaDoc.append("    <dlentry>\n");
          ditaDoc
              .append("      <dt>")
              .append(logger.getMessage(Messages.MSG_TITLE_WINDOWING))
              .append("</dt>\n");
          ditaDoc.append("      <dd>\n");
          ditaDoc.append(
              Translater.xlate(
                  winDescription,
                  fileUri,
                  new LocationType(
                      "inputPortOpenSet:" + range + ":windowingDescription", fileUri)));
          ditaDoc.append("      </dd>\n");
          ditaDoc.append("    </dlentry>\n");
        }
        ditaDoc.append("    <dlentry>\n");

        ditaDoc
            .append("      <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PROPERTIES))
            .append("</dt>\n");
        ditaDoc.append("      <dd>\n");
        ditaDoc = getInputPortSetConfigurationMarkup(ditaDoc, inputPortOpenSet);
        ditaDoc.append("      </dd>\n");
        ditaDoc.append("    </dlentry>\n");
        ditaDoc.append("   </dl>\n");
        ditaDoc.append("  </dd>\n");
        ditaDoc.append("</dlentry>\n");
      }
      ditaDoc.append("</dl>\n");
      ditaDoc.append("</section>" + "\n");
    }

    // Output ports section
    if (num_outputPorts > 0 || hasOutputPortOpenSet) {
      ditaDoc.append("<section>" + "\n");
      ditaDoc.append(
          formatTitleLink(
              RefId_Operator_OutputPortSetId, logger.getMessage(Messages.MSG_TITLE_OUTPUT_PORTS)));

      ditaDoc.append("<dl>\n");
      ditaDoc.append("  <dlentry>\n");
      ditaDoc
          .append("     <dt>")
          .append(logger.getMessage(Messages.MSG_TITLE_ASSIGNMENTS))
          .append("</dt>\n");
      ditaDoc
          .append("       <dd>")
          .append(logger.getMessage(Messages.MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS))
          .append("\n");
      ditaDoc.append("       </dd>\n");
      ditaDoc.append("  </dlentry>\n");
      ditaDoc.append("</dl>\n");

      // Count input ports in each set. Use the count to initialize the range in the label
      int count_outputPorts = 0;
      ditaDoc.append("<dl>\n");
      for (JavaOpOutputPortSetType outputPortSet : outputPorts.getOutputPortSet()) {
        int start_outputPorts = count_outputPorts;
        count_outputPorts += outputPortSet.getCardinality().intValue();
        String range = portRangeString(start_outputPorts, count_outputPorts - 1);
        ditaDoc.append("  <dlentry>\n");
        ditaDoc
            .append("   <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PORTS))
            .append(" ")
            .append(range)
            .append("</dt>\n");
        DescriptionType description;
        ditaDoc.append("   <dd>\n");
        if ((description = outputPortSet.getDescription()) != null) {
          ditaDoc.append(
              Translater.xlate(
                      description.getValue(),
                      fileUri,
                      new LocationType("outputPortSet:" + range, fileUri))
                  + "\n");
          ditaDoc.append("<p></p>\n");
        }
        ditaDoc.append("   <dl>\n");
        ditaDoc.append("    <dlentry>\n");
        ditaDoc
            .append("      <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PROPERTIES))
            .append("</dt>\n");
        ditaDoc.append("      <dd>\n");
        ditaDoc = getOutputPortSetConfigurationMarkup(ditaDoc, outputPortSet);
        ditaDoc.append("      </dd>\n");
        ditaDoc.append("    </dlentry>\n");
        ditaDoc.append("   </dl>\n");
        ditaDoc.append("   <p></p>\n");
        ditaDoc.append("   </dd>\n");
        ditaDoc.append("  </dlentry>\n");
      }
      JavaOpOutputPortOpenSetType outputPortOpenSet = outputPorts.getOutputPortOpenSet();
      if (outputPortOpenSet != null) {
        String range = portRangeString(count_outputPorts, -1);
        ditaDoc.append("  <dlentry>\n");
        ditaDoc
            .append("   <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PORTS))
            .append(" ")
            .append(range)
            .append("</dt>\n");
        DescriptionType description;
        ditaDoc.append("   <dd>\n");
        if ((description = outputPortOpenSet.getDescription()) != null) {
          ditaDoc.append(
              Translater.xlate(
                      description.getValue(),
                      fileUri,
                      new LocationType("outputPortOpenSet:" + range, fileUri))
                  + "\n");
        }
        ditaDoc.append("   <p></p>\n");
        ditaDoc.append("   <dl>\n");
        ditaDoc.append("    <dlentry>\n");
        ditaDoc
            .append("      <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PROPERTIES))
            .append("</dt>\n");
        ditaDoc.append("      <dd>\n");
        ditaDoc = getOutputPortSetConfigurationMarkup(ditaDoc, outputPortOpenSet);
        ditaDoc.append("      </dd>\n");
        ditaDoc.append("    </dlentry>\n");
        ditaDoc.append("   </dl>\n");
        ditaDoc.append("   <p></p>\n");
        ditaDoc.append("   </dd>\n");
        ditaDoc.append("  </dlentry>\n");
      }
      ditaDoc.append("</dl>\n");
      ditaDoc.append("</section>\n");
    }

    if (params.getParameter().size() > 0) {
      assert (params != null);
      ditaDoc.append("<section>\n");
      ditaDoc.append(
          formatTitleLink(
              RefId_Operator_ParametersId, logger.getMessage(Messages.MSG_TITLE_PARAMETERS)));
      // ditaDoc.append("<title outputclass=\"splhead-1\">Parameters</title>\n");
      DescriptionType description = params.getDescription();
      // If a description is provided, use it, otherwise generate a description.
      if (description != null && description.getValue() != null) {
        ditaDoc.append(
            Translater.xlate(
                    description.getValue(), fileUri, new LocationType("parameters", fileUri))
                + "\n");
      } else {
        ditaDoc.append(getParametersMessage(params));
      }
      listParameters(ditaDoc, params, refId);
      // Show parameters in sorted order.
      Map<String, JavaOpParameterType> sortedParams = new TreeMap<String, JavaOpParameterType>();
      for (JavaOpParameterType parameter : params.getParameter()) {
        sortedParams.put(parameter.getName(), parameter);
      }
      ditaDoc.append("<dl>\n");
      for (String parameterName : sortedParams.keySet()) {
        JavaOpParameterType parameter = sortedParams.get(parameterName);
        ditaDoc.append("<dlentry id=\"parameter__" + parameterName + "\">\n");
        ditaDoc.append("<dt><tt>" + parameterName + "</tt></dt>\n");
        ditaDoc.append("<dd>");
        DescriptionType pdesc = parameter.getDescription();
        ditaDoc.append(
            pdesc != null
                ? Translater.xlate(
                    pdesc.getValue(),
                    fileUri,
                    new LocationType("parameter:" + parameterName, fileUri))
                : "");

        // Each parameter has a few properties
        ditaDoc.append("<dl>\n");
        ditaDoc.append("<dlentry>\n");
        ditaDoc.append(" <dt>" + logger.getMessage(Messages.MSG_TITLE_PROPERTIES) + " </dt>\n");
        ditaDoc.append(" <dd>\n");
        ditaDoc.append("  <sl>\n");
        if (parameter.getType() != null) {
          ditaDoc.append("  <sli>");
          ditaDoc.append(formatProperty("Type", parameter.getType(), RefId_Operator_ParameterId));
          // For each parameter type that's defined as a CustomLiteral, list the enumerated values
          // for
          // the custom literal.
          if (parameter.isSetExpressionMode()
              && "CustomLiteral".equals(parameter.getExpressionMode().getName())) {
            EnumerationsType enumsType = context.getCustomLiterals();
            if (enumsType != null) {
              for (EnumerationType enumType : enumsType.getEnumeration()) {
                if (enumType.getName().equals(parameter.getType())) {
                  ditaDoc.append(" (");
                  ditaDoc.append("<tt>");
                  ditaDoc.append(commaSeparatedList(enumType.getValue()));
                  ditaDoc.append("</tt>");
                  ditaDoc.append(")");
                  break;
                }
              }
            }
          }
          ditaDoc.append("  </sli>\n");
        }
        if (parameter.getCardinality() != null
            && !parameter.getCardinality().equals(new BigInteger("-1"))) {
          // -1 is the equivalent of not specifying a cardinality
          ditaDoc.append(
              "     <sli>"
                  + formatProperty(
                      "Cardinality",
                      parameter.getCardinality().toString(),
                      RefId_Operator_ParameterId));
          ditaDoc.append("     </sli>\n");
        }
        ditaDoc.append(
            "     <sli>"
                + formatProperty("Optional", parameter.isOptional(), RefId_Operator_ParameterId));
        ditaDoc.append("     </sli>\n");
        if (parameter.isSetExpressionMode()) {
          ditaDoc.append(
              "     <sli>"
                  + formatProperty(
                      "ExpressionMode",
                      parameter.getExpressionMode().toString(),
                      RefId_Operator_ParameterId));
          ditaDoc.append("     </sli>\n");
        }
        ditaDoc.append("  </sl>\n");
        ditaDoc.append(" </dd>\n");
        ditaDoc.append("</dlentry>\n");
        ditaDoc.append("</dl>\n");
        // Close parameter properties
        ditaDoc.append("</dd>\n");

        // Close description of parameter
        ditaDoc.append("</dlentry>\n");
      }
      ditaDoc.append("</dl>\n");
      ditaDoc.append("</section>\n");
    }

    // Format templates that show how to invoke operator
    ditaDoc.append(formatCodeTemplates(context.getCodeTemplates()));

    // Metrics - output metrics only if one or more metrics are defined.
    ditaDoc.append(formatMetrics(context.getMetrics(), fileUri));

    if (context.getLibraryDependencies() != null) {
      JavaOpLibraryDependenciesType libraries = context.getLibraryDependencies();
      if (libraries.getLibrary().size() > 0) {
        ditaDoc.append("<section>\n");
        ditaDoc.append(
            formatTitleLink(
                    RefId_Operator_Dependencies, logger.getMessage(Messages.MSG_TITLE_LIBRARIES))
                + "\n");
        // ditaDoc.append("<title outputclass=\"splhead-1\">Libraries</title>\n");
        ditaDoc.append(" <dl>\n");
        for (JavaOpLibraryType library : libraries.getLibrary()) {
          ditaDoc.append(" <dlentry>\n");
          DescriptionType description;
          ditaDoc.append("  <dt>");
          if ((description = library.getDescription()) != null
              && description.getValue().length() > 0) {
            ditaDoc.append(Translater.xlateAsText(description.getValue(), fileUri) + "\n");
          } else {
            ditaDoc.append(logger.getMessage(Messages.MSG_NO_DESC_LIB)).append("\n");
          }
          ditaDoc.append("  </dt>\n");
          // Add a least one dd to satify DITA schema
          ditaDoc.append("  <dd/>\n");

          JavaOpManagedLibraryType mLibrary = library.getManagedLibrary();
          if (mLibrary != null) {
            String command = mLibrary.getCommand();
            if (command != null && command.length() > 0) {
              ditaDoc.append(
                  "  <dd>"
                      + formatProperty("Command", command, RefId_Operator_Dependencies_ManagedLibId)
                      + "  </dd>\n");
            }
            EList<String> libPaths = mLibrary.getLibPath();
            if (libPaths != null && libPaths.size() > 0) {
              String libPathList = commaSeparatedList(libPaths);
              if (libPathList.length() > 0) {
                String libPathLink =
                    formatReferenceLink(
                        RefId_Operator_Dependencies_LibPathId,
                        logger.getMessage(Messages.MSG_TITLE_LIBRARY_PATH));
                ditaDoc.append("  <dd>" + libPathLink + ": <tt>" + libPathList + "</tt></dd>\n");
              }
            }
          } else {
            // Generate empty dlentry to satisfy DITA schema
            ditaDoc.append("<dt/><dd/>\n");
          }
          ditaDoc.append(" </dlentry>\n");
        }
        ditaDoc.append(" </dl>\n");
        ditaDoc.append("</section>\n");
      } // if (libraries.getLibrary().size() > 0)
    }
    ditaDoc.append("</refbody>\n");
    ditaDoc.append("</reference>\n");
    return ditaDoc.toString();
  }

  /**
   * Return a message to describe the parameter configuration
   *
   * @param parameters list of defined parameters, if any.
   * @return summary message
   */
  private String getParametersMessage(ParametersType parameters) {
    assert (parameters != null);
    int num_parameters = parameters.getParameter().size();

    // allowAny == false implies that operation can accept only defined parameters
    if (parameters.isAllowAny()) {
      if (num_parameters == 0) {
        return logger.getMessage(Messages.MSG_ALLOWANY_ONLY);
      } else if (num_parameters == 1) {
        return logger.getMessage(Messages.MSG_ALLOWANY_PARAMETER);
      } else {
        return logger.getMessage(
            Messages.MSG_ALLOWANY_PARAMETERS, new String[] {String.valueOf(num_parameters)});
      }
    } else {
      if (num_parameters == 0) {
        return logger.getMessage(Messages.MSG_NO_ALLOWANY_NO_PARAMETERS);
      } else if (num_parameters == 1) {
        return logger.getMessage(Messages.MSG_NO_ALLOWANY_PARAMETER);
      } else {
        return logger.getMessage(
            Messages.MSG_NO_ALLOWANY_PARAMETERS, new String[] {String.valueOf(num_parameters)});
      }
    }
  }

  /**
   * Return a message to describe the parameter configuration
   *
   * @param parameters list of defined parameters, if any.
   * @return summary message
   */
  private String getParametersMessage(JavaOpParametersType parameters) {
    assert (parameters != null);
    int num_parameters = parameters.getParameter().size();

    if (num_parameters == 0) {
      return logger.getMessage(Messages.MSG_NO_ALLOWANY_NO_PARAMETERS);
    } else if (num_parameters == 1) {
      return logger.getMessage(Messages.MSG_NO_ALLOWANY_PARAMETER);
    } else {
      return logger.getMessage(
          Messages.MSG_NO_ALLOWANY_PARAMETERS, new String[] {String.valueOf(num_parameters)});
    }
  }

  /**
   * Generate DITA doc for a primitive operator.
   *
   * @return the DITA document
   */
  private String generate(
      PrimitiveToolkitOperatorType primOp, OperatorModelType opModel, Artifact artifact) {
    OpModelType cppOpModel = opModel.getCppOperatorModel();
    if (cppOpModel != null) {
      return generate(primOp, cppOpModel, artifact);
    }
    JavaOpModelType javaOpModel = opModel.getJavaOperatorModel();
    if (javaOpModel != null) {
      return generate(primOp, javaOpModel, artifact);
    }
    logger.printException(
        new Exception(new Message(Key.CDISP7030E).getLocalizedMessageAndPrefix()));
    return "";
  }

  /** Generate DITA doc for a namespace. */
  private String generate(
      ToolkitNamespaceType namespaceType, SourceModelType sourceModel, Artifact artifact) {
    String fileUri = FileOutput.ToolkitModelFileName;
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append(Constants.XHTML_Header_Reference);
    ditaDoc.append("<reference id=\"" + Constants.RefId_Namespace + "\">" + "\n");
    // If the namespace has no content, return an empty namespace document
    if (namespaceType == null) {
      ditaDoc.append(
          "<title>"
              + logger.getMessage(
                  currentToolkit.isSample()
                      ? Messages.MSG_TITLE_SAMPLE_NAMESPACE
                      : Messages.MSG_TITLE_NAMESPACE)
              + " <tt>"
              + getNamespaceLabel("")
              + "</tt></title>"
              + "\n");
      ditaDoc.append("<refbody>" + "\n");
      ditaDoc.append("</refbody>" + "\n");
      ditaDoc.append("</reference>" + "\n");
      return ditaDoc.toString();
    }
    String namespace = namespaceType.getName();
    ditaDoc.append(
        "<title>"
            + logger.getMessage(
                currentToolkit.isSample()
                    ? Messages.MSG_TITLE_SAMPLE_NAMESPACE
                    : Messages.MSG_TITLE_NAMESPACE)
            + " <tt>"
            + getNamespaceLabel(namespace)
            + "</tt></title>"
            + "\n");
    ditaDoc.append("<refbody>" + "\n");
    ditaDoc.append(generateNamespacePathSection(namespace));
    // Add the namespace description from the toolkit model, in case there is one (this is not
    // expected).
    DescriptionType desc = namespaceType.getDescription();
    if (desc != null) {
      ditaDoc.append("<section>" + "\n");
      ditaDoc.append(
          Translater.xlate(desc.getValue(), fileUri, new LocationType(artifact, fileUri)));
      ditaDoc.append("</section>" + "\n");
    }
    // Add the namespace description from the namespace description file, if any.
    SourceFileType descFile = namespaceDescFileMap.get(namespace);
    if (descFile != null) {
      String descFileName = descFile.getUri();
      ditaDoc.append(
          generateSectionsWithPages(
              descFile.getCompilationUnit().getSplDoc(),
              descFileName,
              new LocationType(artifact, descFileName),
              artifact));
    }
    // Output operators, functions and types in sorted order.
    List<Artifact> artifactList;
    artifactList =
        toolkitMgr.getArtifactsByToolkitNamespaceAndType(
            currentToolkit, namespace, ArtifactType.Operator);
    removeNonOperatorComposites(artifactList);
    Collections.sort(artifactList);
    ditaDoc.append(generateListSection(artifactList, getListLabel(ArtifactType.Operator), true));
    artifactList =
        toolkitMgr.getArtifactsByToolkitNamespaceAndType(
            currentToolkit, namespace, ArtifactType.Function);
    Collections.sort(artifactList);
    ditaDoc.append(generateListSection(artifactList, getListLabel(ArtifactType.Function), true));
    artifactList =
        toolkitMgr.getArtifactsByToolkitNamespaceAndType(
            currentToolkit, namespace, ArtifactType.Type);
    Collections.sort(artifactList);
    ditaDoc.append(generateListSection(artifactList, getListLabel(ArtifactType.Type), true));
    ditaDoc.append("</refbody>" + "\n");
    ditaDoc.append("</reference>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Given a list of Strings, return a comma separated list of the list elements.
   *
   * @param list
   * @return comma separated list of list elements.
   */
  private String commaSeparatedList(EList<String> list) {
    if (list.size() == 0) {
      return "";
    }
    if (list.size() == 1) {
      return list.get(0);
    }
    String csl = "";
    for (int ix = 0; ix < list.size() - 1; ix++) {
      csl += list.get(ix) + ", ";
    }
    return csl += list.get(list.size() - 1);
  }

  /**
   * Given a list of integers, return a comma separated list of the list elements.
   *
   * @param list
   * @return comma separated list of list elements.
   */
  private String commaSeparatedIntegerList(EList<BigInteger> list) {
    if (list.size() == 0) {
      return "";
    }
    if (list.size() == 1) {
      return list.get(0).toString();
    }
    String csl = "";
    for (int ix = 0; ix < list.size() - 1; ix++) {
      csl += list.get(ix).toString() + ", ";
    }
    return csl += list.get(list.size() - 1).toString();
  }

  /**
   * Add DITA markup for a native function's SPLDOC annotations.
   *
   * <p>Since these annotations were added in a later release (Streams 4.1), if there are no
   * relevant annotations present for this function, we do not generate any output, as the
   * parameters, etc, may be documented elsewhere in the description.
   */
  private void generateNativeFunctionAnnotations(
      StringBuilder ditaDoc,
      String description,
      Artifact artifact,
      String fileUri,
      LocationType location) {
    List<SPLDOCAnnotation> annotations = ParserSPLDOC.parseAnnotations(description);
    // Check if this function has function annotations.
    boolean hasFunctionAnnotations = false;
    for (SPLDOCAnnotation annotation : annotations) {
      for (String name : NativeFunctionAnnotationTags) {
        if (annotation.name.equals(name)) {
          hasFunctionAnnotations = true;
          break;
        }
      }
    }
    // Create annotations map.
    Map<String, SPLDOCAnnotation> map = createAnnotationsMap(annotations);
    // Generate documentation corresponding to the annotations.
    if (hasFunctionAnnotations) {
      ToolkitFunctionType function = (ToolkitFunctionType) artifact.eObject;
      ditaDoc.append("   <dl>\n");
      // An empty definition list entry avoids an error if there are no other entries.
      ditaDoc.append("     <dlentry><dt></dt><dd></dd></dlentry>\n");
      // Generate documentation for parameters.
      EList<com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType> params = function.getParameter();
      if (params != null && !params.isEmpty()) {
        ditaDoc.append("     <dlentry>\n");
        ditaDoc
            .append("       <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_PARAMETERS))
            .append("</dt>\n");
        ditaDoc.append("       <dd>\n");
        ditaDoc.append("         <dl compact=\"yes\">\n");
        for (com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType param : params) {
          String paramName = param.getName();
          ditaDoc.append("           <dlentry>\n");
          ditaDoc.append("             <dt><tt>" + paramName + "</tt></dt>\n");
          ditaDoc.append("             <dd>\n");
          String desc;
          SPLDOCAnnotation annotation = map.get("param" + "$" + paramName);
          if (annotation != null && (desc = annotation.description) != null) {
            // Format as in-line text, ignoring markup for paragraphs, code blocks, etc.
            ditaDoc.append(Translater.xlate(desc, fileUri, location) + "\n");
          } else {
            if (inputParameters.isWarnNoComments()) {
              printAnnotationWarn(
                  Key.CDISP7112W,
                  location,
                  annotation,
                  new String[] {"@param " + paramName, function.getPrototype()});
            }
          }
          ditaDoc.append("             </dd>\n");
          ditaDoc.append("           </dlentry>\n");
        }
        ditaDoc.append("         </dl>\n");
        ditaDoc.append("       </dd>\n");
        ditaDoc.append("     </dlentry>\n");
      }
      // Verify that the parameter annotations with targets each match a parameter for the function.
      for (SPLDOCAnnotation annotation : annotations) {
        if (!annotation.name.equals("param") || annotation.target == null) {
          continue;
        }
        boolean match = false;
        if (params != null) {
          for (com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType param : params) {
            if (param.getName().equals(annotation.target)) {
              match = true;
              break;
            }
          }
        }
        if (match == false) {
          printAnnotationWarn(
              Key.CDISP7113W,
              location,
              annotation,
              new String[] {annotation.target, function.getPrototype()});
        }
      }
      // Generate documentation for any return value.
      String returns = function.getReturnType();
      if (returns != null) {
        SPLDOCAnnotation annotation = map.get("return" + "$");
        if (returns.equals("void")) {
          if (annotation != null) {
            // Annotation is not allowed with void return type.
            printAnnotationWarn(
                Key.CDISP7105W, location, annotation, new String[] {function.getPrototype()});
          }
        } else {
          ditaDoc.append("     <dlentry>\n");
          ditaDoc
              .append("       <dt>")
              .append(logger.getMessage(Messages.MSG_TITLE_RETURNS))
              .append("</dt>\n");
          ditaDoc.append("       <dd>\n");
          String desc;
          if (annotation != null && (desc = annotation.description) != null) {
            // Format as in-line text, ignoring markup for paragraphs, code blocks, etc.
            ditaDoc.append(Translater.xlate(desc, fileUri, location));
          } else {
            ditaDoc.append("<tt>" + Encoder.HTMLEntityEncode(returns) + "</tt>\n");
            if (inputParameters.isWarnNoComments()) {
              printAnnotationWarn(
                  Key.CDISP7112W,
                  location,
                  annotation,
                  new String[] {"@return", function.getPrototype()});
            }
          }
          ditaDoc.append("       </dd>\n");
          ditaDoc.append("     </dlentry>\n");
        }
      }
      // Generate documentation for exceptions, in sorted order.
      TreeMap<String, SPLDOCAnnotation> sorted_exceptions = new TreeMap<String, SPLDOCAnnotation>();
      for (SPLDOCAnnotation annotation : annotations) {
        if (annotation.name.equals("throws") && annotation.target != null) {
          sorted_exceptions.put(annotation.target, annotation);
        }
      }
      if (!sorted_exceptions.isEmpty()) {
        ditaDoc.append("     <dlentry>\n");
        ditaDoc
            .append("       <dt>")
            .append(logger.getMessage(Messages.MSG_TITLE_THROWS))
            .append("</dt>\n");
        ditaDoc.append("       <dd>\n");
        ditaDoc.append("         <dl compact=\"yes\">\n");
        for (String exceptionName : sorted_exceptions.keySet()) {
          ditaDoc.append("          <dlentry>\n");
          ditaDoc.append("            <dt><tt>" + exceptionName + "</tt></dt>\n");
          ditaDoc.append("            <dd>\n");
          SPLDOCAnnotation annotation = sorted_exceptions.get(exceptionName);
          String desc;
          if (annotation != null && (desc = annotation.description) != null) {
            // Format as in-line text, ignoring markup for paragraphs, code blocks, etc.
            ditaDoc.append(Translater.xlate(desc, fileUri, location) + "\n");
          } else {
            if (inputParameters.isWarnNoComments()) {
              printAnnotationWarn(
                  Key.CDISP7112W,
                  location,
                  annotation,
                  new String[] {"@throws " + exceptionName, function.getPrototype()});
            }
          }
          ditaDoc.append("            </dd>\n");
          ditaDoc.append("          </dlentry>\n");
        }
        ditaDoc.append("         </dl>\n");
        ditaDoc.append("       </dd>\n");
        ditaDoc.append("     </dlentry>\n");
      }
      ditaDoc.append("</dl>\n");
    }
  }

  /** Generate a section for a set of native functions. */
  private String generateNativeFunctionsSection(
      Artifact fmArtifact, String rootRefId, TreeMap<String, FunctionType> functions_sorted) {
    if (functions_sorted.isEmpty()) {
      return "";
    }
    String namespace = fmArtifact.namespace;
    String fileUri = fmArtifact.uriValue;
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append("<section>" + "\n");
    String functionsId = rootRefId + References.FunctionModelFunctionSetId + "__functions";
    ditaDoc.append(formatTitleLink(functionsId, logger.getMessage(Messages.MSG_TITLE_FUNCTIONS)));
    ditaDoc.append("<dl compact=\"yes\">\n");
    for (String function_signature : functions_sorted.keySet()) {
      FunctionType function = functions_sorted.get(function_signature);
      String prototypeValue = function.getPrototype().getValue();
      // Look up function by full prototype string to get the name used for cross-references.
      Artifact artifact =
          getKnownArtifactByAltName(
              currentToolkit, namespace, prototypeValue, ArtifactType.Function);
      if (artifact == null || toolkitMgr.isExcludedArtifact(artifact)) {
        // Do not include an artifact that has been excluded from the generated documentation. */
        continue;
      }
      ditaDoc.append(" <dlentry>\n");
      ditaDoc.append("   <dt outputclass=\"splpart\" ");
      ditaDoc.append("      id=\"" + artifact.name + "\"><tt>");
      ditaDoc.append("      <![CDATA[" + prototypeValue + "]]>");
      ditaDoc.append("      </tt>");
      ditaDoc.append("   </dt>\n");
      ditaDoc.append("   <dd>");
      LocationType location = new LocationType(artifact, fileUri);
      DescriptionType descriptionType = function.getDescription();
      String description = (descriptionType == null ? null : descriptionType.getValue());
      if (description != null && ParserSPLDOC.hasDescription(description)) {
        ditaDoc.append(Translater.xlate(description, fileUri, location) + "\n");
      } else if (inputParameters.isWarnNoComments()) {
        logger.printWarn(Key.CDISP7081W, location.toString(), new String[] {prototypeValue});
      }
      if (description != null) {
        generateNativeFunctionAnnotations(ditaDoc, description, artifact, fileUri, location);
      }
      ditaDoc.append("   </dd>\n");
      ditaDoc.append(" </dlentry>\n");
    }
    ditaDoc.append("</dl>\n");
    ditaDoc.append("</section>" + "\n");
    return ditaDoc.toString();
  }

  /** Generate DITA doc for a set of native functions. */
  private String generate(FunctionModelType functionModel, Artifact fmArtifact) {
    String namespace = fmArtifact.namespace;
    String fileUri = fmArtifact.uriValue;
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append(Constants.XHTML_Header_Reference);
    ditaDoc.append("<reference id=\"" + Constants.RefId_Functions + "\">" + "\n");
    ditaDoc.append(
        "<title>"
            + getFunctionModelTitleLabel(fileUri, currentToolkit.isSample())
            + ": <tt>"
            + getNamespaceLabel(namespace)
            + "</tt></title>"
            + "\n");
    ditaDoc.append("<refbody>" + "\n");
    ditaDoc.append(generateArtifactPathSection(namespace, getFunctionModelLabel(fileUri)));
    ditaDoc.append("<section>\n");
    ditaDoc.append(logger.getMessage(Messages.MSG_FUNCTION_EXPLANATION) + "\n");
    ditaDoc.append("</section>\n");
    String rootRefId = FunctionModelRefPath + "#" + References.FunctionModelRefId + "/";
    // Allow function set header information to be included for C++ functions, if requested.
    boolean includeFunctionSetHeader =
        inputParameters.isIncludeFunctionSetHeader()
            && fileUri.endsWith(FileOutput.CppFunctionModelFileName);
    // Use a tree map to sort the function prototypes.
    TreeMap<String, FunctionType> functions_sorted = new TreeMap<String, FunctionType>();
    for (FunctionSetType functionSet : functionModel.getFunctionSet()) {
      if (includeFunctionSetHeader) {
        // Output function set header information.
        ditaDoc.append("<section>" + "\n");
        ditaDoc.append(
            formatTitleLink(
                rootRefId + "__functionset", logger.getMessage(Messages.MSG_TITLE_FUNCTION_SET)));
        if (functionSet.getHeaderFileName() != null) {
          String headerFileId =
              rootRefId + References.FunctionModelFunctionSetId + "__headerfilename";
          ditaDoc.append("<dl><dlentry>\n");
          ditaDoc.append("  <dt>");
          ditaDoc.append(
              formatReferenceLink(headerFileId, logger.getMessage(Messages.MSG_TITLE_HEADER_FILE)));
          ditaDoc.append("  </dt>\n");
          ditaDoc.append("  <dd>");
          ditaDoc.append(" " + functionSet.getHeaderFileName());
          ditaDoc.append("  </dd>\n");
          ditaDoc.append("</dlentry></dl>\n");
        }
        if (functionSet.getCppNamespaceName() != null) {
          String cppnamespacenameId =
              rootRefId + References.FunctionModelFunctionSetId + "__cppnamespacename";
          ditaDoc.append("<dl><dlentry>\n");
          ditaDoc.append("  <dt>");
          ditaDoc.append(formatReferenceLink(cppnamespacenameId, "CPP Namespace Name"));
          ditaDoc.append("  </dt>\n");
          ditaDoc.append("  <dd>");
          ditaDoc.append(" " + functionSet.getCppNamespaceName());
          ditaDoc.append("  </dd>\n");
          ditaDoc.append("</dlentry></dl>\n");
        }
        DependenciesType dependencies = functionSet.getDependencies();
        if (dependencies != null && dependencies.getLibrary().size() > 0) {
          String dependenciesId =
              rootRefId + References.FunctionModelFunctionSetId + "__dependencies";
          String managedLibId = dependenciesId + "__managedlibrary";
          String libId = managedLibId + "_lib";
          String includePathId = managedLibId + "_includePath";
          String libPathId = managedLibId + "_libPath";
          String commandId = managedLibId + "_command";
          ditaDoc.append("<dl><dlentry>\n");
          ditaDoc.append("  <dt>");
          ditaDoc.append(
              formatReferenceLink(
                  dependenciesId,
                  "<b>" + logger.getMessage(Messages.MSG_TITLE_DEPENDENCIES) + "</b>"));
          ditaDoc.append("  </dt>\n");
          for (LibraryType library : dependencies.getLibrary()) {
            ditaDoc.append("  <dd>\n");
            ditaDoc.append("    <dl>\n");
            ditaDoc.append("      <dlentry>\n"); // dlentry, dl, dd, dlentry, dl
            ditaDoc.append("         <dt>");
            DescriptionType description = library.getDescription();
            if (description != null && description.getValue() != null) {
              ditaDoc.append(Translater.xlateAsText(description.getValue(), fileUri));
            }
            ditaDoc.append("         </dt>\n");
            // Output single dd to satisfy DITA schema if no managed lib fields.
            ditaDoc.append("          <dd/>");
            ManagedLibraryType mLibrary = library.getManagedLibrary();
            String command = mLibrary.getCommand();
            if (command != null && command.length() > 0) {
              ditaDoc.append("      <dd>" + formatReferenceLink(commandId, "Command") + ": ");
              ditaDoc.append("<tt>" + command + "</tt>");
              ditaDoc.append("      </dd>\n");
            }
            if (mLibrary.getLibPath().size() > 0) {
              ditaDoc.append(
                  "      <dd>"
                      + formatReferenceLink(
                          libPathId, logger.getMessage(Messages.MSG_TITLE_LIBRARY_PATHS))
                      + ": ");
              ditaDoc.append("<tt>" + commaSeparatedList(mLibrary.getLibPath()) + "</tt>");
              ditaDoc.append("      </dd>\n");
            }
            if (mLibrary.getIncludePath() != null && mLibrary.getIncludePath().size() > 0) {
              ditaDoc.append(
                  "      <dd>"
                      + formatReferenceLink(
                          includePathId, logger.getMessage(Messages.MSG_TITLE_INCLUDE_PATHS))
                      + ": ");
              ditaDoc.append("<tt>" + commaSeparatedList(mLibrary.getIncludePath()) + "</tt>");
              ditaDoc.append("      </dd>\n");
            }
            if (mLibrary.getLib() != null && mLibrary.getLib().size() > 0) {
              ditaDoc.append(
                  "      <dd>"
                      + formatReferenceLink(libId, logger.getMessage(Messages.MSG_TITLE_LIBRARIES))
                      + ": ");
              ditaDoc.append("<tt>" + commaSeparatedList(mLibrary.getLib()) + "</tt>");
              ditaDoc.append("      </dd>\n");
            }
            ditaDoc.append("      </dlentry>\n");
            ditaDoc.append(" </dl></dd>\n");
          }
          ditaDoc.append("</dlentry></dl>\n");
        } // if (dependencies != null && dependencies.getLibrary().size() > 0) ...
        ditaDoc.append("</section>" + "\n");
      }
      // Collect the functions for sorting.
      FunctionsType functions = functionSet.getFunctions();
      if (functions != null) {
        for (FunctionType function : functions.getFunction()) {
          PrototypeType prototype = function.getPrototype();
          if (prototype != null) {
            // Sort the function by the function's signature.
            String prototypeValue = prototype.getValue();
            String function_signature = getFunctionSignature(prototypeValue);
            if (function_signature != null) {
              // Add prototype to make a signature with constrained generic types unique.
              functions_sorted.put(function_signature + prototypeValue, function);
            }
          }
        }
      }
      if (includeFunctionSetHeader) {
        // Output functions for this function set in sorted order.
        ditaDoc.append(generateNativeFunctionsSection(fmArtifact, rootRefId, functions_sorted));
        functions_sorted.clear();
      }
    }
    if (!includeFunctionSetHeader) {
      // Output functions for all function sets in sorted order.
      ditaDoc.append(generateNativeFunctionsSection(fmArtifact, rootRefId, functions_sorted));
      functions_sorted.clear();
    }
    ditaDoc.append("</refbody>" + "\n");
    ditaDoc.append("</reference>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Generates DITA documents for a toolkit.
   *
   * <p>Documents are generated for each SPL file, primitive operator, native function model, in
   * addition to indexes to these for the toolkit's namespace, and the toolkit itself.
   */
  private void generate(Toolkit toolkit) {
    boolean hasDefaultNamespace = false;
    ToolkitType toolkitModel = toolkit.getToolkitModel();
    String toolkitPath = toolkit.getToolkitPath();
    Translater.setCurrentToolkit(toolkit);
    currentToolkit = toolkit;

    // Load the source model.
    SourceModelType sourceModel = LoadModel.loadSourceModel(toolkit);
    if (sourceModel == null) ErrorManager.throwFatalErrorException();
    setNamespaceDescFiles(sourceModel);

    // Generate DITA document for the toolkit
    {
      String ditaDoc = generate(toolkitModel, sourceModel);
      String ditaPathName =
          FileOutput.getDitaPathName(toolkit, FileOutput.getDocFileNameToolkit(toolkit));
      ditaPages.put(ditaPathName, ditaDoc);
    }

    // Generate DITA document for each SPL file.
    {
      // Create the documents.
      EList<SourceFileType> sourceFiles = sourceModel.getSourceFile();
      for (SourceFileType sourceFile : sourceFiles) {
        if (namespaceDescFiles.contains(sourceFile)) {
          // Do not include namespace description files.
          continue;
        }
        CompilationUnitType compUnit = sourceFile.getCompilationUnit();
        DefinitionsType defs = compUnit.getDefinitions();
        if (defs == null) {
          // Do not include SPL files with no artifacts.
          continue;
        }
        Map<String, TreeMap<String, TypeDefinitionType>> staticTypes =
            getStaticTypes(defs.getCompositeDefinition());
        if (!hasContents(compUnit, staticTypes)) {
          // Do not include SPL files with no artifacts to be shown.
          continue;
        }
        SplNamespaceType namespaceType = compUnit.getSplNamespace();
        String namespace = (namespaceType == null ? "" : namespaceType.getName());
        String fileUri = sourceFile.getUri();
        Artifact artifact =
            getKnownArtifact(
                toolkit, namespace, new File(fileUri).getName(), ArtifactType.CompilationUnit);
        if (artifact == null || toolkitMgr.isExcludedArtifact(artifact)) {
          // Do not include an artifact that has been excluded from the generated documentation. */
          continue;
        }
        // Generate DITA document for SPL compilation unit
        if (namespaceType == null) {
          hasDefaultNamespace = true;
        }
        String ditaDoc = generate(sourceFile, artifact, staticTypes);
        String ditaPathName =
            FileOutput.getDitaPathName(
                toolkit, FileOutput.getDocFileNameSPLFile(namespace, fileUri));
        ditaPages.put(ditaPathName, ditaDoc);
      }
    }

    // Generate document for each primitive operator and namespace.
    {
      if (hasDefaultNamespace && toolkitModel.getNamespace().size() == 0) {
        // If the toolkit has a default namespace, but that namespace has no content,
        // create an empty namespace document.
        String ditaDoc = generate((ToolkitNamespaceType) null, sourceModel, null);
        String ditaPathName =
            FileOutput.getDitaPathName(toolkit, FileOutput.getDocFileNameNamespace(""));
        ditaPages.put(ditaPathName, ditaDoc);
      }
      for (ToolkitNamespaceType namespaceType : toolkitModel.getNamespace()) {
        String namespace = namespaceType.getName();
        Artifact nsArtifact = getKnownArtifact(toolkit, null, namespace, ArtifactType.Namespace);
        if (nsArtifact == null || toolkitMgr.isExcludedArtifact(nsArtifact)) {
          // Do not include an artifact that has been excluded from the generated documentation. */
          continue;
        }
        // Generate docs for primitive operators
        for (PrimitiveToolkitOperatorType primOp : namespaceType.getPrimitiveOp()) {
          String name = primOp.getName();
          Artifact artifact = getKnownArtifact(toolkit, namespace, name, ArtifactType.Operator);
          if (artifact == null || toolkitMgr.isExcludedArtifact(artifact)) {
            // Do not include an artifact that has been excluded from the generated documentation.
            // */
            continue;
          }
          String opModelFile = currentToolkit.getModelFileNameForOp(primOp);
          logger.printTrace(Key.CDISP7074I, new String[] {opModelFile});
          OperatorModelType opModel =
              LoadModel.loadOperatorModel(toolkitPath + File.separator + opModelFile);
          if (opModel == null) ErrorManager.throwFatalErrorException();
          // Include primitive operator diagrams, only if the user did not suppress it.
          if (!inputParameters.isSuppressPrimitiveOperatorDiagram()) {
            Map<String, ArrayList<Integer>> primOpDimension =
                OperatorModelImageGenerator.generateOpImage(
                    toolkit, namespace, opModelFile, opModel, primOp);
            // Merge the entry (only one) from the primitive operator dimension map into the global
            // image dimension map.
            imageDimensions = new HashMap<String, ArrayList<Integer>>();
            for (String imageFileName : primOpDimension.keySet()) {
              imageDimensions.put(imageFileName, primOpDimension.get(imageFileName));
            }
          }
          String ditaDoc = generate(primOp, opModel, artifact);
          String ditaPathName =
              FileOutput.getDitaPathName(
                  toolkit, FileOutput.getDocFileNameOp(namespace, primOp.getName()));
          ditaPages.put(ditaPathName, ditaDoc);
        }
        // Generate docs for each namespace
        {
          String ditaDoc = generate(namespaceType, sourceModel, nsArtifact);
          String ditaPathName =
              FileOutput.getDitaPathName(toolkit, FileOutput.getDocFileNameNamespace(namespace));
          ditaPages.put(ditaPathName, ditaDoc);
        }
      }
    }

    // Generate a document for each native function model file.
    {
      List<String> functionModelFiles = toolkitMgr.getNativeFunctionModelFiles(toolkitModel);
      if (functionModelFiles != null) {
        for (String functionModelFile : functionModelFiles) {
          String namespace = getNamespaceForFunctionModel(functionModelFile);
          Artifact artifact =
              getKnownArtifact(
                  toolkit,
                  namespace,
                  getNameForFunctionModel(functionModelFile),
                  ArtifactType.FunctionModel);
          if (artifact == null || toolkitMgr.isExcludedArtifact(artifact)) {
            // Do not include an artifact that has been excluded from the generated documentation.
            // */
            continue;
          }
          logger.printTrace(Key.CDISP7075I, new String[] {functionModelFile});
          FunctionModelType functionModel =
              LoadModel.loadFunctionModel(toolkitPath + File.separator + functionModelFile);
          if (functionModel == null) {
            ErrorManager.throwFatalErrorException();
          }
          String ditaDoc = generate(functionModel, artifact);
          String ditaPathName =
              FileOutput.getDitaPathName(
                  toolkit, FileOutput.getDocFileNameFunction(namespace, functionModelFile));
          ditaPages.put(ditaPathName, ditaDoc);
        }
      }
    }

    // Generate index documents for the toolkit.
    generateIndexes();

    Translater.setCurrentToolkit(null);
    currentToolkit = null;
  }

  /** Generates a DITA document listing all the main or all the sample toolkits. */
  private void generate(List<Artifact> tkArtifactList) {
    if (generatingSampleToolkitDocs && tkArtifactList.isEmpty()) {
      // The sample toolkits list is generated only if there are samples.
      return;
    }
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append(Constants.XHTML_Header_Reference);
    ditaDoc.append(
        "<reference id=\""
            + (generatingSampleToolkitDocs
                ? Constants.RefId_SampleToolkits
                : Constants.RefId_Toolkits)
            + "\">"
            + "\n");
    ditaDoc.append(
        "<title>"
            + (generatingSampleToolkitDocs
                ? getListTitleLabel(ArtifactType.Toolkit, generatingSampleToolkitDocs)
                : docTitle)
            + "</title>"
            + "\n");
    ditaDoc.append("<refbody>" + "\n");
    if (generatingSampleToolkitDocs) {
      ditaDoc.append(generateSamplesPathSection());
    }
    if (!tkArtifactList.isEmpty()) {
      // List the artifact indexes for the toolkits.
      ditaDoc.append(generateIndexSection());
      // Add a sorted list of toolkits.
      Collections.sort(tkArtifactList);
      ditaDoc.append(
          generateListSection(tkArtifactList, getListLabel(ArtifactType.Toolkit), false));
    }
    // If this is the main toolkit listing, add a link to the sample toolkits.
    if (!generatingSampleToolkitDocs && hasSamples) {
      ditaDoc.append(generateSamplesSection());
    }
    // List the reference documents for the toolkits.
    ditaDoc.append(generateReferenceSection());

    ditaDoc.append("</refbody>" + "\n");
    ditaDoc.append("</reference>" + "\n");
    ditaPages.put(
        FileOutput.getDitaPathNameToolkits(
            FileOutput.AllToolkitsFileName, generatingSampleToolkitDocs),
        ditaDoc.toString());
  }

  // A map of toolkit artifacts to their SPLDOC descriptions and source files.
  private class ArtifactDescription {
    String description;
    String fileUri;

    ArtifactDescription(String description, String fileUri) {
      this.description = description;
      this.fileUri = fileUri;
    }
  }

  private HashMap<Artifact, ArtifactDescription> artifactDescriptionMap =
      new HashMap<Artifact, ArtifactDescription>();

  /**
   * Add to the artifact description map the given description and source file for the given toolkit
   * artifact.
   */
  private void addArtifactDescription(Artifact artifact, String description, String fileUri) {
    artifactDescriptionMap.put(artifact, new ArtifactDescription(description, fileUri));
  }

  /** Generate a list of the given artifacts. */
  private String generateList(List<Artifact> artifactList) {
    if (artifactList.isEmpty()) {
      return "";
    }
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append("<sl>" + "\n");
    for (Artifact artifact : artifactList) {
      ditaDoc.append("<sli>");
      ditaDoc.append(createDitaLink(createDitaLinkRef(artifact), getListArtifactLabel(artifact)));
      ditaDoc.append("</sli>" + "\n");
    }
    ditaDoc.append("</sl>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Generate a list of the given artifacts, with summary descriptions written to the same line as
   * the artifact names.
   */
  private String generateListWithInLineDesc(List<Artifact> artifactList) {
    if (artifactList.isEmpty()) {
      return "";
    }
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append("<sl>" + "\n");
    for (Artifact artifact : artifactList) {
      ditaDoc.append("<sli>");
      ArtifactDescription desc = artifactDescriptionMap.get(artifact);
      String description;
      if (desc != null) {
        Toolkit prevToolkit = Translater.getCurrentToolkit();
        Translater.setCurrentToolkit(artifact.toolkit);
        description = getFirstSentence(desc.description, desc.fileUri);
        Translater.setCurrentToolkit(prevToolkit);
      } else {
        description = "";
      }
      ditaDoc.append(
          formatProperty(
              createDitaLink(createDitaLinkRef(artifact), getListArtifactLabel(artifact)),
              description,
              false));
      ditaDoc.append("</sli>\n");
    }
    ditaDoc.append("</sl>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Generate a list of the given artifacts, with summary descriptions separated from the artifact
   * names.
   */
  private String generateListWithDesc(List<Artifact> artifactList) {
    if (artifactList.isEmpty()) {
      return "";
    }
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append("<dl>" + "\n");
    for (Artifact artifact : artifactList) {
      ditaDoc.append("<dlentry>" + "\n");
      ditaDoc.append("  <dt>");
      ditaDoc.append(createDitaLink(createDitaLinkRef(artifact), getListArtifactLabel(artifact)));
      ditaDoc.append("</dt>" + "\n");
      ditaDoc.append("  <dd>");
      ArtifactDescription desc = artifactDescriptionMap.get(artifact);
      if (desc != null) {
        Toolkit prevToolkit = Translater.getCurrentToolkit();
        Translater.setCurrentToolkit(artifact.toolkit);
        ditaDoc.append(getFirstSentence(desc.description, desc.fileUri));
        Translater.setCurrentToolkit(prevToolkit);
      }
      ditaDoc.append("</dd>" + "\n");
      ditaDoc.append("</dlentry>" + "\n");
    }
    ditaDoc.append("</dl>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Generate a section with the given title, containing a list of the given artifacts, with summary
   * descriptions. If inLineDesc is true, the summary descriptions are written to the same line as
   * the artifact names.
   */
  private String generateListSection(
      List<Artifact> artifactList, String title, boolean inLineDesc) {
    if (artifactList.isEmpty()) {
      return "";
    }
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append("<section>" + "\n");
    ditaDoc.append("  <title outputclass=\"splhead-1\">" + title + "</title>" + "\n");
    ditaDoc.append(
        inLineDesc ? generateListWithInLineDesc(artifactList) : generateListWithDesc(artifactList));
    ditaDoc.append("</section>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Generate a DITA document that provides an index for all artifacts of the specified type. The
   * index can be for the current toolkit, or if the current toolkit is set to null, the index is
   * for all the main or all the sample toolkits. If there are no artifacts of the specified type,
   * no index is generated.
   */
  private void generateIndex(ArtifactType type) {
    List<Artifact> artifactList;
    if (currentToolkit == null) {
      artifactList = toolkitMgr.getArtifactsByType(type, generatingSampleToolkitDocs);
    } else {
      artifactList = toolkitMgr.getArtifactsByToolkitAndType(currentToolkit, type);
    }
    if (type == ArtifactType.Operator) {
      removeNonOperatorComposites(artifactList);
    }
    if (artifactList.isEmpty()) {
      return;
    }
    Collections.sort(artifactList);
    String title = getListLabel(type);
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append(Constants.XHTML_Header_Reference);
    ditaDoc.append("<reference id=\"" + Constants.RefId_Index + "\">" + "\n");
    ditaDoc.append("<title>");
    ditaDoc.append(
        getListTitleLabel(type, generatingSampleToolkitDocs)
            + ": "
            + (currentToolkit == null ? docTitle : getToolkitLabel(currentToolkit)));
    ditaDoc.append("</title>" + "\n");
    ditaDoc.append("<refbody>" + "\n");
    ditaDoc.append(generatePathSection(currentToolkit, generatingSampleToolkitDocs, null, title));
    ditaDoc.append(generateListSection(artifactList, title, false));
    ditaDoc.append("</refbody>" + "\n");
    ditaDoc.append("</reference>" + "\n");
    ditaPages.put(
        FileOutput.getDitaPathName(
            currentToolkit, FileOutput.getDocFileNameIndex(type), generatingSampleToolkitDocs),
        ditaDoc.toString());
  }

  /**
   * Generates the DITA documents that contain the toolkit artifact indexes for the current toolkit,
   * or if the current toolkit is set to null, the indexes are for all the main or all the sample
   * toolkits.
   */
  private void generateIndexes() {
    generateIndex(ArtifactType.Namespace);
    generateIndex(ArtifactType.Operator);
    generateIndex(ArtifactType.Function);
    generateIndex(ArtifactType.Type);
  }

  /**
   * Returns a link reference for the toolkit artifact index of the specified type, or null, if
   * none. If the toolkit is set to null, the index is either for all the main or, if isSample is
   * true, all the sample toolkits.
   */
  private String getIndexLinkRef(Toolkit toolkit, ArtifactType type, boolean isSample) {
    List<Artifact> artifactList =
        (toolkit == null
            ? toolkitMgr.getArtifactsByType(type, isSample)
            : toolkitMgr.getArtifactsByToolkitAndType(toolkit, type));
    if (type == ArtifactType.Operator) {
      removeNonOperatorComposites(artifactList);
    }
    if (artifactList.isEmpty()) {
      return null;
    }
    return createDitaLinkRef(toolkit, FileOutput.getDocFileNameIndex(type), isSample);
  }

  /**
   * Returns a link to the toolkit artifact index of the specified type, or null, if none. The
   * indexes can be for the current toolkit, or if the current toolkit is set to null, the indexes
   * are either for all the main or, if isSample is true, all the sample toolkits.
   */
  private String getIndexLink(ArtifactType type, boolean isSample) {
    String linkRef = getIndexLinkRef(currentToolkit, type, isSample);
    if (linkRef == null) {
      return null;
    }
    return createDitaLink(linkRef, getListLabel(type));
  }

  /**
   * Generates a section of a DITA document that lists and references the toolkit artifact indexes.
   * The indexes can be for the current toolkit, or if the current toolkit is set to null, the
   * indexes are for all the main or all the sample toolkits.
   */
  private String generateIndexSection() {
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append("<section>" + "\n");
    ditaDoc.append(
        "  <title outputclass=\"splhead-1\">"
            + logger.getMessage(Messages.MSG_TITLE_INDEXES)
            + "</title>"
            + "\n");
    ditaDoc.append("  <dl>" + "\n");
    // An empty definition list entry avoids an error if there are no other entries.
    ditaDoc.append("    <dlentry><dt></dt><dd></dd></dlentry>\n");
    String link;
    if ((link = getIndexLink(ArtifactType.Namespace, generatingSampleToolkitDocs)) != null) {
      ditaDoc.append("    <dlentry><dt>" + link + "</dt><dd></dd></dlentry>" + "\n");
    }
    if ((link = getIndexLink(ArtifactType.Operator, generatingSampleToolkitDocs)) != null) {
      ditaDoc.append("    <dlentry><dt>" + link + "</dt><dd></dd></dlentry>" + "\n");
    }
    if ((link = getIndexLink(ArtifactType.Function, generatingSampleToolkitDocs)) != null) {
      ditaDoc.append("    <dlentry><dt>" + link + "</dt><dd></dd></dlentry>" + "\n");
    }
    if ((link = getIndexLink(ArtifactType.Type, generatingSampleToolkitDocs)) != null) {
      ditaDoc.append("    <dlentry><dt>" + link + "</dt><dd></dd></dlentry>" + "\n");
    }
    ditaDoc.append("  </dl>" + "\n");
    ditaDoc.append("</section>" + "\n");
    return ditaDoc.toString();
  }

  /** Generates a section of a DITA document that links to the list of sample toolkits. */
  private String generateSamplesSection() {
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append("<section>" + "\n");
    ditaDoc.append(
        "  <title outputclass=\"splhead-1\">"
            + logger.getMessage(Messages.MSG_TITLE_SAMPLE_TOOLKITS)
            + "</title>"
            + "\n");
    ditaDoc.append("  <dl>" + "\n");
    ditaDoc.append(
        "    <dlentry><dt>" + createDitaLinkSamples() + "</dt><dd></dd></dlentry>" + "\n");
    ditaDoc.append("  </dl>" + "\n");
    ditaDoc.append("</section>" + "\n");
    return ditaDoc.toString();
  }

  /**
   * Generate the DITA documents containing reference material. These contain descriptions of the
   * operator and function model elements. The reference material is simply copied from existing
   * files.
   */
  private void generateReferenceDocs() {
    // Generate model references only if they are not suppressed from the CLI, and the
    // toolkits have one or more native functions or primitive operators.
    if (!inputParameters.isSuppressReferenceDocs()) {
      if (toolkitsHavePrimitiveOperators) {
        {
          String ditaDoc = References.getOperatorModelRef();
          String ditaPathName =
              FileOutput.getDitaPathNameReference(References.OperatorModelRefFileName);
          ditaPages.put(ditaPathName, ditaDoc);
        }
      }
      if (toolkitsHaveNativeFunctions) {
        {
          String ditaDoc = References.getFunctionModelRef();
          String ditaPathName =
              FileOutput.getDitaPathNameReference(References.FunctionModelRefFileName);
          ditaPages.put(ditaPathName, ditaDoc);
        }
      }
    }
  }

  /** Generates a section of a DITA document that lists the reference material for the toolkits. */
  private String generateReferenceSection() {
    if (inputParameters.isSuppressReferenceDocs()
        || !(toolkitsHaveNativeFunctions || toolkitsHavePrimitiveOperators)) {
      return "";
    }
    StringBuilder ditaDoc = new StringBuilder();
    ditaDoc.append("<section>" + "\n");
    ditaDoc.append(
        "  <title outputclass=\"splhead-1\">"
            + logger.getMessage(Messages.MSG_TITLE_REFERENCES)
            + "</title>"
            + "\n");
    ditaDoc.append("  <dl>" + "\n");
    if (toolkitsHavePrimitiveOperators) {
      String link =
          createDitaLink(
              new File(
                      "..",
                      FileOutput.getDitaPathNameReference(References.OperatorModelRefFileName)
                          + "."
                          + FileOutput.DitaFileExt)
                  .toString(),
              logger.getMessage(Messages.MSG_TITLE_OPERATOR_MODEL_DOC));
      ditaDoc.append("    <dlentry><dt>" + link + "</dt><dd></dd></dlentry>" + "\n");
    }
    if (toolkitsHaveNativeFunctions) {
      String link =
          createDitaLink(
              new File(
                      "..",
                      FileOutput.getDitaPathNameReference(References.FunctionModelRefFileName)
                          + "."
                          + FileOutput.DitaFileExt)
                  .toString(),
              logger.getMessage(Messages.MSG_TITLE_FUNCTION_MODEL_DOC));
      ditaDoc.append("    <dlentry><dt>" + link + "</dt><dd></dd></dlentry>" + "\n");
    }
    ditaDoc.append("  </dl>" + "\n");
    ditaDoc.append("</section>" + "\n");
    return ditaDoc.toString();
  }

  /** Set the title for the set of documents. */
  private void setDocTitle() {
    docTitle = inputParameters.getDocTitle();
    if (docTitle.length() > 0) {
      docTitle = Encoder.HTMLEntityEncode(docTitle);
    } else {
      docTitle = logger.getMessage(Messages.MSG_TITLE_TOOLKITS);
    }
  }

  /** Write the DITA documents to disk. After writing the map is cleared of pending documents. */
  private void write() {
    for (String ditaPage : ditaPages.keySet()) {
      logger.printTrace(Key.CDISP7065I, new String[] {ditaPage});
    }
    FileOutput.write(ditaPages, FileOutput.DitaFormat);
    ditaPages.clear();
  }

  /**
   * Generates DITA documents either for all the main or, if isSample is true, all the sample
   * toolkits.
   */
  public void generateToolkitDocs(boolean isSample) {
    // Flag whether generating the main or sample toolkits.
    generatingSampleToolkitDocs = isSample;

    // Get a list of the toolkits to be included in the documentation.
    List<Artifact> tkArtifactList = toolkitMgr.getArtifactsByType(ArtifactType.Toolkit, isSample);

    // Generate the documents for each toolkit.
    for (Artifact tkArtifact : tkArtifactList) {
      generate(tkArtifact.toolkit);
      write();
    }

    // Generate a document listing all the toolkits.
    generate(tkArtifactList);
    write();

    // Generate indexes of the artifacts in all the toolkits.
    generateIndexes();
    write();

    generatingSampleToolkitDocs = false;
  }

  /** Generates DITA documents for all the toolkits. */
  public void generate() {
    // Initial set up.
    FileOutput.setOutputDirectory(inputParameters.getOutputDir());
    setDocTitle();

    // Process each toolkit prior to document generation.
    for (Toolkit toolkit : toolkitMgr.getToolkits()) {
      process(toolkit);
    }

    // Generate the main toolkit documents.
    generateToolkitDocs(false);
    write();

    // Generate the sample toolkit documents.
    generateToolkitDocs(true);
    write();

    // Generate the reference documents.
    generateReferenceDocs();
    write();

    // Generate the dita map (a table of contents for this set of documents).
    generateMap();
    write();
  }
}
