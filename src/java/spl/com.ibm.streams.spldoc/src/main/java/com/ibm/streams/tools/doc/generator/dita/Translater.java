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
import com.ibm.streams.tools.doc.generator.dita.GeneratorDita.LocationType;
import com.ibm.streams.tools.doc.generator.dita.ParserSPLDOC.SPLDOCElementNode;
import com.ibm.streams.tools.doc.generator.dita.ParserSPLDOC.SPLDOCElementType;
import com.ibm.streams.tools.doc.generator.dita.ParserSPLDOC.SPLDOCErrorNode;
import com.ibm.streams.tools.doc.generator.dita.ParserSPLDOC.SPLDOCMarkup;
import com.ibm.streams.tools.doc.generator.dita.ParserSPLDOC.SPLDOCTableEntryNode;
import com.ibm.streams.tools.doc.generator.dita.ParserSPLDOC.SPLDOCTableNode;
import com.ibm.streams.tools.doc.generator.dita.ParserSPLDOC.SPLDOCTableRowsNode;
import com.ibm.streams.tools.doc.utilities.Encoder;
import com.ibm.streams.tools.doc.utilities.ErrorManager;
import com.ibm.streams.tools.doc.utilities.FileOutput;
import com.ibm.streams.tools.doc.utilities.InputParameters;
import com.ibm.streams.tools.doc.utilities.Logger;
import com.ibm.streams.tools.doc.utilities.ToolkitManager;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.Artifact;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.ArtifactType;
import java.io.File;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Pattern;
import org.eclipse.core.runtime.Path;

/**
 * This class provides methods for translating the SPLDOC model (the abstract syntax tree
 * representing SPLDOC markup) to DITA format.
 *
 * @author afrenk
 */
public class Translater {

  // Regex for a file path - must have a "/" followed by a file name.
  private static String PathScheme = "^(.*?/)(.+)[^/]$";
  private static Pattern PathScheme_Pattern = Pattern.compile(PathScheme);

  // Regex for URI scheme: alpha *( alpha | digit | "+" | "-" | "." )
  private static String URIScheme = "^[a-zA-Z]{1}[0-9a-zA-Z\\.\\+\\-]*:[^:].*";
  private static Pattern URIScheme_Pattern = Pattern.compile(URIScheme);

  // General variables.
  private static InputParameters inputParameters = InputParameters.getInputParameters();
  private static Logger logger = Logger.getInstance();
  private static ToolkitManager toolkitMgr = ToolkitManager.getInstance();
  private static ToolkitManager.Toolkit currentToolkit = null;
  private static GeneratorDita genDita = GeneratorDita.getInstance();

  // The source of the SPLDOC markup being translated.
  private static String docFileUri;
  private static LocationType docLocation;

  // Translation state.
  private static boolean inTable;

  // The type, namespace and name of a link to an SPL artifact.
  private static ArtifactType linkSPLType;
  private static String linkSPLNamespace;
  private static String linkSPLName;

  // Set to indicate a link to a file path or URL.
  private static boolean linkisFilePath;
  private static boolean linkisURL;

  // Map each new page node in an SPLDOC syntax tree to the file name for the page.
  private static Map<SPLDOCElementNode, String> newPageFileNameMap =
      new HashMap<SPLDOCElementNode, String>();

  /** Get the current toolkit being processed. */
  static ToolkitManager.Toolkit getCurrentToolkit() {
    return currentToolkit;
  }

  /**
   * Set the current toolkit being processed.
   *
   * <p>This is the toolkit where the SPLDOC markup comes from. Note, however, that the current
   * document being generated might be for all toolkits.
   */
  static void setCurrentToolkit(ToolkitManager.Toolkit toolkit) {
    currentToolkit = toolkit;
  }

  /**
   * Log the given warning or error message, using location information, if provided, unless no
   * logging is set to prevent multiple messages for each occurrence.
   */
  private static void printWarnOrError(
      SPLDOCElementNode splDocNode,
      Key messageId,
      String[] args,
      boolean noLog,
      boolean isWarning) {
    if (noLog) {
      return;
    }
    String locString = "";
    if (docLocation != null) {
      locString += docLocation.toString();
    }
    if (splDocNode.lineOffset >= 0) {
      Message locMessage = new Message(Key.CDISP7028E, splDocNode.lineOffset + 1);
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

  /** Log the given warning message. */
  private static void printWarn(
      SPLDOCElementNode splDocNode, Key messageId, String[] args, boolean noLog) {
    printWarnOrError(splDocNode, messageId, args, noLog, true);
  }

  /** Log the given error message. */
  private static void printError(
      SPLDOCElementNode splDocNode, Key messageId, String[] args, boolean noLog) {
    printWarnOrError(splDocNode, messageId, args, noLog, false);
  }

  /** Log the given parsing error. */
  private static void printParseError(SPLDOCElementNode splDocNode, boolean noLog) {
    SPLDOCErrorNode splDocErrorNode = (SPLDOCErrorNode) splDocNode;
    // String messageId;
    Key messageId;
    boolean isWarning = false;
    switch (splDocErrorNode.errorType) {
      case InternalError:
        printError(splDocNode, Key.CDISP7124E, new String[] {splDocNode.text}, false);
        ErrorManager.throwFatalErrorException();
      case InvalidInlineMarkupError:
        // Report as a warning as this may occur in current toolkits.
        messageId = Key.CDISP7026W;
        isWarning = true;
        break;
      case NoTitleError:
        messageId = Key.CDISP7024E;
        break;
      case ReservedInlineError:
        messageId = Key.CDISP7023E;
        break;
      case ReservedNestedInlineError:
        // Report as a warning as this may occur in current toolkits.
        messageId = Key.CDISP7027W;
        isWarning = true;
        break;
      case ReservedNewlineError:
        messageId = Key.CDISP7021E;
        break;
      case ReservedTitleError:
        messageId = Key.CDISP7022E;
        break;
      case TableEndError:
        messageId = Key.CDISP7029E;
        break;
      case TableSyntaxError:
        messageId = Key.CDISP7030E;
        break;
      case TableEOLSyntaxError:
        messageId = Key.CDISP7031E;
        break;
      case TableTooFewColumnsError:
        messageId = Key.CDISP7032E;
        break;
      case TableTooManyColumnsError:
        messageId = Key.CDISP7033E;
        break;
      case TableMixedColumnBoundaryError:
        messageId = Key.CDISP7034E;
        break;
      case TableNoColsRowBoundaryError:
        messageId = Key.CDISP7035E;
        break;
      case TableShortRowBoundaryError:
        messageId = Key.CDISP7036E;
        break;
      default:
        messageId = Key.CDISP7025E;
    }
    String[] args = (splDocNode.text == null ? null : new String[] {splDocNode.text});
    if (isWarning) {
      printWarn(splDocNode, messageId, args, noLog);
    } else {
      printError(splDocNode, messageId, args, noLog);
    }
  }

  /**
   * Return a space if the last character in the given string is neither whitespace nor the XML end
   * of tag character, otherwise return an empty string.
   */
  private static String endSpacing(StringBuilder sb) {
    int length = sb.length();
    if (length > 0) {
      char c = sb.charAt(length - 1);
      if (c != '>' && !ParserSPLDOC.isWhitespace(c)) {
        return " ";
      }
    }
    return "";
  }

  /**
   * Get the file name for the document that is the parent of the new page for the toolkit artifact,
   * where the new page is represented by the given SPLDOC syntax tree node.
   */
  private static String getNewPageParentFileName(SPLDOCElementNode newPageNode, Artifact artifact) {
    SPLDOCElementNode parent = newPageNode.getAncestor(SPLDOCElementType.NewPage);
    if (parent == null) {
      // The parent is the main document for the artifact.
      return FileOutput.getDocFileNameArtifact(artifact);
    }
    return newPageFileNameMap.get(parent);
  }

  /**
   * Given an artifact, URI or file link formatted as one of: <artifact type>:<namespace>::<artifact
   * name> <artifact type>:<artifact name> <namespace>::<artifact name> <artifact name> <URI> <local
   * file path> set the type, namespace and name for matching the specified artifact, or set the
   * artifact name to null if this is not a valid artifact specifier. If <artifact type> or
   * <namespace> is set to "*" to indicate that any value can be matched, then the type or namespace
   * for matching is set to null. If the link is not a valid artifact specifier, but is a possible
   * file or URI link, then the corresponding indicator is set.
   *
   * <p>Note that <artifact name> for a function is of the form: <function name>(<parameter type>..)
   * where <parameter type> can include a namespace followed by "::".
   */
  private static void getLinkTarget(String linkURL) {
    linkSPLType = null;
    linkSPLNamespace = null;
    linkSPLName = null;
    linkisFilePath = false;
    linkisURL = false;
    if (linkURL.length() <= 0) {
      // Zero-length specifier.
      return;
    }
    if (isFilePath(linkURL)) {
      // <local file path>
      linkisFilePath = true;
      return;
    }
    // Get the name without any function parameter types.
    int ixParen = linkURL.indexOf('(');
    String name = ixParen < 0 ? linkURL : linkURL.substring(0, ixParen);
    int ix1 = name.indexOf(':');
    if (ix1 < 0) {
      // <artifact name>
      linkSPLName = linkURL;
      return;
    }
    int ix2 = name.lastIndexOf(':');
    if (ix2 == ix1 + 1) {
      // <namespace>::<artifact name>
      if (linkURL.substring(ix2 + 1).length() <= 0) {
        // Zero-length name.
        return;
      }
      String namespace = linkURL.substring(0, ix1);
      linkSPLNamespace = (namespace.equals("*") ? null : namespace);
      linkSPLName = linkURL.substring(ix2 + 1);
      return;
    }
    String type = name.substring(0, ix1);
    if (type.equalsIgnoreCase("*")) {
      linkSPLType = null;
    } else if (type.equalsIgnoreCase("toolkit")) {
      linkSPLType = ArtifactType.Toolkit;
    } else if (type.equalsIgnoreCase("namespace")) {
      linkSPLType = ArtifactType.Namespace;
    } else if (type.equalsIgnoreCase("operator")) {
      linkSPLType = ArtifactType.Operator;
    } else if (type.equalsIgnoreCase("function")) {
      linkSPLType = ArtifactType.Function;
    } else if (type.equalsIgnoreCase("type")) {
      linkSPLType = ArtifactType.Type;
    } else if (type.equalsIgnoreCase("compilationunit")) {
      linkSPLType = ArtifactType.CompilationUnit;
    } else {
      // Not a valid artifact type.
      if (URIScheme_Pattern.matcher(linkURL).matches()) {
        // A possible URI scheme.
        linkisURL = true;
      }
      return;
    }
    if (ix2 > ix1 && name.indexOf(":", ix1 + 1) != ix2 - 1) {
      // Too many delimiters.
      return;
    }
    if (name.substring(ix2 + 1).length() <= 0) {
      // Zero-length name.
      return;
    }
    boolean NoNamespaceType = ToolkitManager.NoNamespaceArtfacts.contains(linkSPLType);
    if (ix2 > ix1) {
      // <artifact type>:<namespace>::<artifact name>
      if (NoNamespaceType) {
        // Namespace not allowed for this type.
        return;
      }
      String namespace = linkURL.substring(ix1 + 1, ix2 - 1);
      linkSPLNamespace = (namespace.equals("*") ? null : namespace);
    } else {
      // <artifact type>:<artifact name>
      if (!NoNamespaceType) {
        // Namespace required for this type.
        return;
      }
    }
    linkSPLName = linkURL.substring(ix2 + 1);
    return;
  }

  /**
   * Given a link or image URI, optional label and the corresponding markup objects, this method
   * returns suitable output text for an unresolved SPLDOC link or image.
   */
  private static String formatSPLDOCLinkOrImage(
      String URI, String label, SPLDOCMarkup markup, SPLDOCMarkup labelMarkup) {
    String text = markup.markup_open + URI;
    if (label != null && !label.isEmpty()) {
      text += labelMarkup.markup_open + label;
    }
    text += markup.markup_close;
    return Encoder.HTMLEntityEncode(text);
  }

  /**
   * Tests whether the input link is a possible path to a file. Returns true if possible path, false
   * otherwise.
   */
  private static boolean isFilePath(String linkURL) {
    if (!URIScheme_Pattern.matcher(linkURL).matches()
        && PathScheme_Pattern.matcher(linkURL).matches()) {
      return true;
    } else {
      return false;
    }
  }

  /** Return a string listing the given artifacts for use in messages. */
  private static String listArtifacts(List<Artifact> v) {
    StringBuilder sb = new StringBuilder();
    for (Artifact a : v) {
      sb.append(
          ToolkitManager.ArtifactString(
              a.toolkit, a.namespace, a.name, a.altName, a.type, a.subtype));
      sb.append(", ");
    }
    // Delete the last ",";
    sb.delete(sb.length() - 2, sb.length());
    return sb.toString();
  }

  /**
   * For the given SPLDOC abstract syntax tree, create the corresponding DITA markup. The abstract
   * syntax tree represents a single description that is generally represented as a section in the
   * generated document, and which may contain nested pages and subsections.
   *
   * <p>Where: - inlineOnly is true only if block markup is to be translated into a single line of
   * output. - noLog is true only if no warning or error messages are to be logged. -
   * allowNewSection is true only if SPLDPOC markup for a section heading is allowed. - artifact is
   * set only if SPLDPOC markup for a new page is allowed.
   */
  private static String xlate(
      SPLDOCElementNode splDocRoot,
      String fileUri,
      LocationType location,
      boolean inlineOnly,
      boolean noLog,
      boolean allowNewSection,
      Artifact artifact) {
    if (new File(fileUri).isAbsolute()) {
      // The file URI must be relative to the toolkit root directory.
      // TODO this should e translatable.
      logger.printError(
          Key.CDISP7124E,
          new String[] {
            "SPLDOC translation file URI must be relative to the toolkit root directory"
          });
      ErrorManager.throwFatalErrorException();
    }
    if (currentToolkit == null) {
      // Currently we support SPLDOC markup only within a toolkit.
      // TODO this should be translatable
      logger.printError(
          Key.CDISP7124E,
          new String[] {"attempt to translate SPLDOC markup without a toolkit defined"});
      ErrorManager.throwFatalErrorException();
    }
    docFileUri = fileUri;
    docLocation = location;
    inTable = false;
    StringBuilder ditaDoc = new StringBuilder();
    xlate(ditaDoc, splDocRoot, inlineOnly, noLog, allowNewSection, artifact);
    return ditaDoc.toString();
  }

  /** For each node in the SPLDOC abstract syntax tree, create the corresponding DITA markup. */
  private static StringBuilder xlate(
      StringBuilder ditaDoc,
      SPLDOCElementNode splDocParent,
      boolean inlineOnly,
      boolean noLog,
      boolean allowNewSection,
      Artifact artifact) {
    List<SPLDOCElementNode> splDocNodes = splDocParent.getChildren();
    for (SPLDOCElementNode splDocNode : splDocNodes) {
      if (splDocNode.type == SPLDOCElementType.Error) {
        printParseError(splDocNode, noLog);
      } else if (splDocNode.type == SPLDOCElementType.Paragraph) {
        if (splDocNode.getChildren().size() > 0) {
          ditaDoc.append(inlineOnly ? endSpacing(ditaDoc) : "\n" + "<p>");
          xlate(ditaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
          ditaDoc.append(inlineOnly ? "" : "\n" + "</p>");
        }
      } else if (splDocNode.type == SPLDOCElementType.UList) {
        ditaDoc.append(inlineOnly ? "" : "\n" + "<ul>");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
        ditaDoc.append(inlineOnly ? "" : "\n" + "</ul>");
      } else if (splDocNode.type == SPLDOCElementType.UListItem) {
        ditaDoc.append(inlineOnly ? "" : "\n" + "<li>");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
        ditaDoc.append(inlineOnly ? "" : "</li>");
      } else if (splDocNode.type == SPLDOCElementType.OList) {
        ditaDoc.append(inlineOnly ? "" : "\n" + "<ol>");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
        ditaDoc.append(inlineOnly ? "" : "\n" + "</ol>");
      } else if (splDocNode.type == SPLDOCElementType.OListItem) {
        ditaDoc.append(inlineOnly ? "" : "\n" + "<li>");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
        ditaDoc.append(inlineOnly ? "" : "</li>");
      } else if (splDocNode.type == SPLDOCElementType.Link) {
        // A link may have two children - a URL and a label
        List<SPLDOCElementNode> linkURLs = splDocNode.getChildren(SPLDOCElementType.LinkURL);
        List<SPLDOCElementNode> linkLabels = splDocNode.getChildren(SPLDOCElementType.LinkLabel);
        String linkURL = (linkURLs.isEmpty() ? "" : linkURLs.get(0).text);
        String linkLabel = (linkLabels.isEmpty() ? "" : linkLabels.get(0).text);
        String linkRef;
        // If the link looks like a reference to an SPL artifact, create a
        // non HTML xref, otherwise create a xref for an HTML link.  A non-HTML link
        // references the generated DITA document for the artifact.
        getLinkTarget(linkURL);
        if (linkSPLName != null) {
          // Qualified function names are normalized and encoded.
          linkSPLName = Encoder.DITANameEncode(linkSPLName);
          // First try to resolve a reference within the current toolkit.
          // This allows resolution of an ambiguous reference if it is unambiguous within the
          // current toolkit.
          List<Artifact> list =
              toolkitMgr.getArtifacts(currentToolkit, linkSPLNamespace, linkSPLName, linkSPLType);
          if (list.size() <= 0) {
            // Search all toolkits.
            list = toolkitMgr.getArtifacts(linkSPLNamespace, linkSPLName, linkSPLType);
            if (list.size() > 0
                && !ToolkitManager.NoNamespaceArtfacts.contains(linkSPLType)
                && linkSPLNamespace == null) {
              // Require an inter-toolkit link to a namespace-scoped artifiact
              // to specify a namespace to reduce the possibility of ambiguity.
              printError(
                  splDocNode, Key.CDISP7108E, new String[] {linkURL, listArtifacts(list)}, noLog);
              // Print out SPLDOC link as is, so user can fix it.
              ditaDoc.append(
                  formatSPLDOCLinkOrImage(
                      linkURL, linkLabel, ParserSPLDOC.Link, ParserSPLDOC.LinkLabel));
              continue;
            }
          }
          if (list.size() == 1) {
            Artifact a = list.get(0);
            linkRef = genDita.createDitaLinkRef(a);
            ditaDoc.append("<xref href=");
          } else if (list.size() > 1) {
            // Link matches multiple SPL artifacts, so is ambiguous.
            printError(
                splDocNode, Key.CDISP7107E, new String[] {linkURL, listArtifacts(list)}, noLog);
            // Print out SPLDOC link as is, so user can fix it.
            ditaDoc.append(
                formatSPLDOCLinkOrImage(
                    linkURL, linkLabel, ParserSPLDOC.Link, ParserSPLDOC.LinkLabel));
            continue;
          } else {
            // Link is to an SPL artifact and is in error.
            if (inputParameters.isWarnOnCrossLinkError()
                && (ToolkitManager.NoNamespaceArtfacts.contains(linkSPLType)
                    || (linkSPLNamespace != null
                        && toolkitMgr
                            .getArtifacts(null, null, linkSPLNamespace, ArtifactType.Namespace)
                            .isEmpty()))) {
              // This is a link to an unknown or excluded toolkit or namespace, and the user wants
              // only a warning.
              printWarn(splDocNode, Key.CDISP7117W, new String[] {linkURL}, noLog);
            } else {
              printError(splDocNode, Key.CDISP7109E, new String[] {linkURL}, noLog);
            }
            // Print out SPLDOC link as is, so user can fix it.
            ditaDoc.append(
                formatSPLDOCLinkOrImage(
                    linkURL, linkLabel, ParserSPLDOC.Link, ParserSPLDOC.LinkLabel));
            continue;
          }
        } else if (linkisFilePath || linkisURL) {
          // If link is formatted as a local path, make it into a proper URI by adding the file
          // scheme.
          linkRef = linkURL;
          if (linkisFilePath) {
            if (linkURL.startsWith("/")) {
              // Absolute link.
              linkRef = "file://" + linkURL;
            } else {
              // Relative links are given relative to the html output directory.
              // Add one level to make it relative to the toolkit artifact document file.
              linkRef = "../" + linkURL;
            }
          }
          try {
            new URI(linkRef);
          } catch (URISyntaxException e) {
            String SPLDOCLink =
                formatSPLDOCLinkOrImage(
                    linkURL, linkLabel, ParserSPLDOC.Link, ParserSPLDOC.LinkLabel);
            printError(
                splDocNode, Key.CDISP7127E, new String[] {SPLDOCLink, e.getMessage()}, noLog);
            // As it may not be intended to be a link, print out SPLDOC link as is.
            ditaDoc.append(SPLDOCLink);
            continue;
          }
          ditaDoc.append("<xref format= \"html\"" + " href=");
        } else {
          // Link is not valid.
          printError(splDocNode, Key.CDISP7121E, new String[] {linkURL}, noLog);
          // Print out SPLDOC link as is, so user can fix it.
          ditaDoc.append(
              formatSPLDOCLinkOrImage(
                  linkURL, linkLabel, ParserSPLDOC.Link, ParserSPLDOC.LinkLabel));
          continue;
        }
        ditaDoc.append("\"" + Encoder.HTMLEntityEncode(linkRef) + "\"");
        ditaDoc.append(">");
        // Use the link label, if specified. Otherwise use the link URL as the label.
        ditaDoc.append(Encoder.HTMLEntityEncode(linkLabel.isEmpty() ? linkURL : linkLabel));
        ditaDoc.append("</xref>");
      } else if (splDocNode.type == SPLDOCElementType.Image) {
        // An embedded image may have two children - a URL and alternate text
        List<SPLDOCElementNode> imageURLs = splDocNode.getChildren(SPLDOCElementType.ImageURL);
        List<SPLDOCElementNode> imageLabels = splDocNode.getChildren(SPLDOCElementType.ImageLabel);
        String imageUrlString = (imageURLs.isEmpty() ? "" : imageURLs.get(0).text);
        String imageText = (imageLabels.isEmpty() ? "" : imageLabels.get(0).text);
        try {
          if (!imageUrlString.startsWith("http:") && !imageUrlString.startsWith("file:")) {
            imageUrlString = "file:" + imageUrlString;
          }
          URL url = new URL(imageUrlString);
          String path = url.getPath();
          String protocol = url.getProtocol();
          // If path is relative, assume path is rooted in directory containing
          // the SPL artifact (e.g. the SPL file, operator model file).
          if ("file".equals(protocol)) {
            if (!path.startsWith("/")) {
              String toolkitPath = currentToolkit.getToolkitPath();
              // Strip file name from path to SPL artifact
              int ix = docFileUri.lastIndexOf("/");
              if (ix == -1) {
                // SPL artifact in toolkit root
                path = toolkitPath + Path.SEPARATOR + path;
              } else {
                String relRoot = docFileUri.substring(0, ix);
                // Current working directory for DITA is <toolkit>/doc/spldoc/dita.
                // Reposition path to image file so that the toolkit is the root.
                path = toolkitPath + Path.SEPARATOR + relRoot + Path.SEPARATOR + path;
              }
            }
            File file = new File(path);
            if (inputParameters.getImageType().equals(InputParameters.JpegOnlyImageType)) {
              // The user has requested only JPEG image format, so check the file extension.
              String filename = file.getName();
              int ix = filename.lastIndexOf('.');
              if (ix > 0
                  && ix < filename.length() - 1
                  && !filename.substring(ix + 1, ix + 2).equalsIgnoreCase("j")) {
                // The file name has a non-JPEG extension, so try looking for a JPEG file.
                String[] jpegExtensions = {"jpg", "jpeg", "JPG", "JPEG"};
                for (String extension : jpegExtensions) {
                  String jpegPath = path.substring(0, path.lastIndexOf('.') + 1) + extension;
                  File jpegFile = new File(jpegPath);
                  if (jpegFile.exists()) {
                    // A file with JPEG extension was found, so reference that instead.
                    file = jpegFile;
                    path = jpegPath;
                  }
                }
              }
            }
            if (!(file.exists() && file.canRead() && file.isFile())) {
              printError(splDocNode, Key.CDISP7104E, new String[] {path}, noLog);
              // Output bad SPLDOC link to document, without converting to DITA link.
              ditaDoc.append(
                  formatSPLDOCLinkOrImage(
                      imageUrlString, imageText, ParserSPLDOC.Image, ParserSPLDOC.ImageLabel));
              continue;
            }
            if (inputParameters.isCopyImageFiles()) {
              // Copy the image file to the output directory and use a relative path
              // so the documentation will be relocatable.
              // Make the path relative to the toolkit artifact document file.
              imageUrlString =
                  new File(new File("..", ".."), FileOutput.copyImageFile(file)).toString();
            } else {
              imageUrlString = "file://" + path;
            }
          }
        } catch (MalformedURLException e) {
          printError(splDocNode, Key.CDISP7103E, new String[] {imageUrlString}, noLog);
          // Output bad SPLDOC link to document, without converting to DITA link.
          ditaDoc.append(
              formatSPLDOCLinkOrImage(
                  imageUrlString, imageText, ParserSPLDOC.Image, ParserSPLDOC.ImageLabel));
          continue;
        }
        if (inlineOnly) {
          // For summary sentences, display image text rather than embedding an image.
          ditaDoc.append(
              ParserSPLDOC.Image.markup_open + imageText + ParserSPLDOC.Image.markup_close);
        } else {
          // Output DITA tag for embedding text.
          ditaDoc.append("<image placement=\"break\"" + " href=");
          ditaDoc.append("\"" + Encoder.HTMLEntityEncode(imageUrlString) + "\"");
          ditaDoc.append(">");
          ditaDoc.append("  <alt>" + Encoder.HTMLEntityEncode(imageText) + "</alt>");
          ditaDoc.append("</image>");
        }
      } else if (splDocNode.type == SPLDOCElementType.Bold) {
        ditaDoc.append("<b>");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
        ditaDoc.append("</b>");
      } else if (splDocNode.type == SPLDOCElementType.Italics) {
        ditaDoc.append("<i>");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
        ditaDoc.append("</i>");
      } else if (splDocNode.type == SPLDOCElementType.Monospace) {
        ditaDoc.append("<tt>");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
        ditaDoc.append("</tt>");
      } else if (splDocNode.type == SPLDOCElementType.CodeBlock) {
        String codeBlockText = "";
        for (SPLDOCElementNode codeBlockNode : splDocNode.getChildren()) {
          codeBlockText += codeBlockNode.text + "\n";
        }
        if (codeBlockText.length() > 0) {
          ditaDoc.append(inlineOnly ? endSpacing(ditaDoc) : "\n" + "<codeblock>" + "\n");
          ditaDoc.append("<![CDATA[" + codeBlockText + "]]>");
          ditaDoc.append(inlineOnly ? "" : "</codeblock>" + "\n");
        }
      } else if (splDocNode.type == SPLDOCElementType.Text) {
        ditaDoc.append(Encoder.HTMLEntityEncode(splDocNode.text));
        // Process any children in case there are errors.
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
      } else if (splDocNode.type == SPLDOCElementType.Title) {
        if (splDocNode.getParent().type == SPLDOCElementType.NewPage && !inlineOnly) {
          // The title has already been processed with the new page node.
          continue;
        }
        ditaDoc.append(inlineOnly ? endSpacing(ditaDoc) : "\n" + "<title>");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
        ditaDoc.append(inlineOnly ? ": " : "\n" + "</title>");
      } else if (splDocNode.type == SPLDOCElementType.NewPage) {
        if (artifact == null) {
          // New pages are not allowed. Handle as in-line markup.
          xlate(ditaDoc, splDocNode, true, noLog, false, null);
          continue;
        }
        // Add the DITA document for the new page to the set of documents being generated.
        String filename = FileOutput.generateDocFileNameNewPage(artifact);
        SPLDOCElementNode titleNode = splDocNode.getChildren(SPLDOCElementType.Title).get(0);
        String title =
            xlate(new StringBuilder(), titleNode, true, noLog, allowNewSection, artifact)
                .toString();
        String parentFileName = getNewPageParentFileName(splDocNode, artifact);
        genDita.addChildDocument(artifact.toolkit, filename, title, parentFileName);
        // Add the file name to the node map so it is available to its descendants.
        newPageFileNameMap.put(splDocNode, filename);
        // Create the DITA document for the new page.
        StringBuilder newDitaDoc = new StringBuilder();
        newDitaDoc.append(Constants.XHTML_Header_Reference);
        newDitaDoc.append("\n" + "<reference id=\"" + Constants.RefId_Page + "\">");
        newDitaDoc.append("\n" + "<title>" + title + "</title>");
        newDitaDoc.append("\n" + "<refbody>");
        newDitaDoc.append("\n" + genDita.generateChildDocPathSection(artifact, filename));
        newDitaDoc.append("\n" + "<section>");
        xlate(newDitaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
        newDitaDoc.append("\n" + "</section>");
        newDitaDoc.append("\n" + "</refbody>");
        newDitaDoc.append("\n" + "</reference>");
        genDita.setChildDocumentContent(artifact.toolkit, filename, newDitaDoc.toString());
        // Add a link to the new page to the current document and the first sentence from the new
        // page.
        ditaDoc.append("\n" + "  <dl><dlentry><dt>");
        ditaDoc.append(
            genDita.createDitaLink(genDita.createDitaLinkRef(artifact.toolkit, filename), title));
        ditaDoc.append("\n" + "</dt><dd>");
        xlate(ditaDoc, ParserSPLDOC.parse(splDocNode.text), true, true, false, null);
        ditaDoc.append("\n" + "</dd></dlentry></dl>");
      } else if (splDocNode.type == SPLDOCElementType.Section) {
        if (!allowNewSection) {
          // New sections are not allowed. Handle as in-line markup.
          xlate(ditaDoc, splDocNode, true, noLog, false, null);
          continue;
        }
        ditaDoc.append(inlineOnly ? "" : "\n" + "</section>");
        ditaDoc.append(inlineOnly ? "" : "\n" + "<section>");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, allowNewSection, artifact);
      } else if (splDocNode.type == SPLDOCElementType.Table) {
        // Tables do not support nested tables, new pages or sections.
        if (inTable) {
          // Handle as in-line markup.
          xlate(ditaDoc, splDocNode, true, noLog, false, null);
          continue;
        }
        inTable = true;
        SPLDOCTableNode tableNode = (SPLDOCTableNode) splDocNode;
        String rowHeaderString =
            (tableNode.hasFirstColumnRowHeader ? " rowheader=\"firstcol\"" : "");
        ditaDoc.append(inlineOnly ? "" : "\n" + "<table" + rowHeaderString + ">");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, false, null);
        ditaDoc.append(inlineOnly ? "" : "\n" + "</table>");
        inTable = false;
      } else if (splDocNode.type == SPLDOCElementType.TableGroup) {
        List<SPLDOCElementNode> entryNodes =
            splDocNode
                .getChildren(SPLDOCElementType.TableRows)
                .get(0)
                .getChildren(SPLDOCElementType.TableRow)
                .get(0)
                .getChildren(SPLDOCElementType.TableEntry);
        int numCols =
            ((SPLDOCTableEntryNode) entryNodes.get(entryNodes.size() - 1)).colEnd
                - ((SPLDOCTableEntryNode) entryNodes.get(0)).colStart
                + 1;
        ditaDoc.append(inlineOnly ? "" : "\n" + "<tgroup cols=\"" + numCols + "\">");
        for (int i = 0; i < numCols; i++) {
          ditaDoc.append(inlineOnly ? "" : "\n" + "<colspec colname=\"" + i + "\"/>");
        }
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, false, null);
        ditaDoc.append(inlineOnly ? "" : "\n" + "</tgroup>");
      } else if (splDocNode.type == SPLDOCElementType.TableRows) {
        if (((SPLDOCTableRowsNode) splDocNode).isColumnHeader) {
          ditaDoc.append(inlineOnly ? "" : "\n" + "<thead>");
          xlate(ditaDoc, splDocNode, inlineOnly, noLog, false, null);
          ditaDoc.append(inlineOnly ? "" : "\n" + "</thead>");
        } else {
          ditaDoc.append(inlineOnly ? "" : "\n" + "<tbody>");
          xlate(ditaDoc, splDocNode, inlineOnly, noLog, false, null);
          ditaDoc.append(inlineOnly ? "" : "\n" + "</tbody>");
        }
      } else if (splDocNode.type == SPLDOCElementType.TableRow) {
        ditaDoc.append(inlineOnly ? "" : "\n" + "<row>");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, false, null);
        ditaDoc.append(inlineOnly ? "" : "\n" + "</row>");
      } else if (splDocNode.type == SPLDOCElementType.TableEntry) {
        SPLDOCTableEntryNode entryNode = (SPLDOCTableEntryNode) splDocNode;
        String colStartString = " namest=\"" + entryNode.colStart + "\"";
        String colEndString = " nameend=\"" + entryNode.colEnd + "\"";
        String moreRowsString =
            (entryNode.moreRows > 0 ? " morerows=\"" + entryNode.moreRows + "\"" : "");
        String hAlignString;
        switch (entryNode.hAlign) {
          case Left:
            hAlignString = " align=\"left\"";
            break;
          case Right:
            hAlignString = " align=\"right\"";
            break;
          case Center:
            hAlignString = " align=\"center\"";
            break;
          default:
            hAlignString = "";
            break;
        }
        String vAlignString;
        switch (entryNode.vAlign) {
          case Top:
            vAlignString = " valign=\"top\"";
            break;
          case Bottom:
            vAlignString = " valign=\"bottom\"";
            break;
          case Middle:
            vAlignString = " valign=\"middle\"";
            break;
          default:
            vAlignString = "";
            break;
        }
        ditaDoc.append(
            inlineOnly
                ? endSpacing(ditaDoc)
                : "\n"
                    + "<entry"
                    + colStartString
                    + colEndString
                    + moreRowsString
                    + hAlignString
                    + vAlignString
                    + ">");
        xlate(ditaDoc, splDocNode, inlineOnly, noLog, false, null);
        ditaDoc.append(inlineOnly ? "" : "\n" + "</entry>");
      } else {
        logger.printError(
            Key.CDISP7124E,
            new String[] {"unexpected SPLDOC node type: " + splDocNode.type.toString()});
        ErrorManager.throwFatalErrorException();
      }
    }
    return ditaDoc;
  }

  /**
   * Converts the given spldoc description from the file with the given URI into DITA format, and
   * returns the result.
   *
   * <p>Only in-line markup is allowed. To avoid duplicate messages, error messages are not
   * generated.
   *
   * <p>This translation method should be used to generate summary sentences that should not include
   * block markup, such as paragraphs and lists. This method can be used to append a description to
   * a line of text.
   */
  static String xlateAsText(String description, String fileUri) {
    SPLDOCElementNode descriptionRoot = ParserSPLDOC.parse(description);
    // descriptionRoot.print();
    return xlate(descriptionRoot, fileUri, null, true, true, false, null);
  }

  /**
   * Converts the given spldoc description from the given location in the file with the given URI
   * into DITA format, and returns the result.
   *
   * <p>Only in-line markup is allowed. Error messages are generated.
   *
   * <p>This translation method should be used to generate summary sentences that should not include
   * block markup, such as paragraphs and lists. This method can be used to append a description to
   * a line of text.
   */
  static String xlateAsText(String description, String fileUri, LocationType location) {
    SPLDOCElementNode descriptionRoot = ParserSPLDOC.parse(description);
    // descriptionRoot.print();
    return xlate(descriptionRoot, fileUri, location, true, false, false, null);
  }

  /**
   * Converts the given spldoc description from the given location in the file with the given URI
   * into DITA format, and returns the result.
   *
   * <p>Section heading markup is allowed. New page markup is not allowed. Error messages are
   * generated.
   */
  static String xlateAsSections(String description, String fileUri, LocationType location) {
    SPLDOCElementNode descriptionRoot = ParserSPLDOC.parse(description);
    // descriptionRoot.print();
    return xlate(descriptionRoot, fileUri, location, false, false, true, null);
  }

  /**
   * Converts the given spldoc description from the given location in the file with the given URI,
   * for the given artifact, into DITA format, and returns the result.
   *
   * <p>Section heading markup is allowed. New page markup is allowed. Error messages are generated.
   */
  static String xlateAsPages(
      String description, String fileUri, LocationType location, Artifact artifact) {
    SPLDOCElementNode descriptionRoot = ParserSPLDOC.parse(description);
    // descriptionRoot.print();
    newPageFileNameMap.clear();
    String ditaDoc = xlate(descriptionRoot, fileUri, location, false, false, true, artifact);
    return ditaDoc;
  }

  /**
   * Converts the given spldoc description from the given location in the file with the given URI
   * into DITA format, and returns the result.
   *
   * <p>Section heading markup is not allowed. New page markup is not allowed. Error messages are
   * generated.
   */
  static String xlate(String description, String fileUri, LocationType location) {
    SPLDOCElementNode descriptionRoot = ParserSPLDOC.parse(description);
    // descriptionRoot.print();
    return xlate(descriptionRoot, fileUri, location, false, false, false, null);
  }

  /**
   * Converts the first sentence of the given spldoc description from the file with the given URI
   * into DITA format, and returns the result.
   *
   * <p>Only in-line markup is allowed. To avoid duplicate messages, error messages are not
   * generated.
   */
  static String xlateFirstSentence(String description, String fileUri) {
    SPLDOCElementNode descriptionRoot =
        ParserSPLDOC.parse(ParserSPLDOC.getFirstSentence(description));
    // descriptionRoot.print();
    return xlate(descriptionRoot, fileUri, null, true, true, false, null);
  }
}
