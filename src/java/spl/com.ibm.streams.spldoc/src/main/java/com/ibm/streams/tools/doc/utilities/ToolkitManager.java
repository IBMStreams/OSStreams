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

package com.ibm.streams.tools.doc.utilities;

import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.doc.StreamsSPLDOCMessagesKey.Key;
import com.ibm.streams.tools.doc.messages.Messages;
import com.ibm.xmlns.prod.streams.spl.common.UriType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.UriTableType;
import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;

/**
 * The toolkit manager manages the collection of toolkits being processed, and the collection of
 * artifacts contained in each toolkit.
 */
public class ToolkitManager {

  // General variables
  private static InputParameters inputParameters = InputParameters.getInputParameters();
  private static Logger logger = Logger.getInstance();

  // Types of artifact in an SPL toolkit.
  public static enum ArtifactType {
    Toolkit,
    Namespace,
    Operator,
    Function,
    FunctionModel,
    Type,
    CompilationUnit
  };

  public static enum ArtifactSubType {
    Composite,
    Primitive,
    NonNative,
    Native,
    None
  };

  // Types of artifacts which have names that are not scoped by a namespace.
  public static final List<ArtifactType> NoNamespaceArtfacts =
      Arrays.asList(new ArtifactType[] {ArtifactType.Toolkit, ArtifactType.Namespace});

  /** An artifact of a toolkit (eg compilation unit, operator, function or type). */
  public class Artifact implements Comparable<Artifact> {
    public Toolkit toolkit;
    public String name;
    public String namespace;
    public ArtifactType type;
    public ArtifactSubType subtype;
    public String uriValue; // used to map to the generated document file for the artifact
    public String altName; // allows matching by alternate name instead of by name
    public EObject eObject; // artifact object, if needed

    // allows sorting by name, namespace, type, toolkit
    @Override
    public int compareTo(Artifact other) {
      if (!name.equals(other.name)) return name.compareTo(other.name);
      if (namespace != null && !namespace.equals(other.namespace))
        return namespace.compareTo(other.namespace);
      if (!type.equals(other.type)) return type.compareTo(other.type);
      if (!toolkit.equals(other.toolkit)) {
        ToolkitType model = toolkit.getToolkitModel();
        ToolkitType otherModel = other.toolkit.getToolkitModel();
        if (!model.getName().equals(otherModel.getName()))
          return model.getName().compareTo(otherModel.getName());
        return model.getVersion().compareTo(otherModel.getVersion());
      }
      return 0;
    }

    // Allows matching by name, and optional namespace and optional type.
    // If alternate name is supplied, match this instead of name.
    // Artifacts without a namespace match only if the type is specified.
    @Override
    public boolean equals(Object object) {
      if (!(object instanceof Artifact)) {
        return false;
      }
      Artifact other = (Artifact) object;
      if (altName != null) {
        if (!altName.equals(altName)) {
          return false;
        }
      } else if (!name.equals(other.name)) {
        return false;
      }
      if (namespace != null && !namespace.equals(other.namespace)) {
        return false;
      }
      if (type != null && type != other.type) {
        return false;
      }
      if (type == null && NoNamespaceArtfacts.contains(other.type)) {
        return false;
      }
      if (toolkit != null && toolkit != other.toolkit) {
        return false;
      }
      return true;
    }

    private Artifact(
        Toolkit toolkit,
        String namespace,
        String name,
        ArtifactType type,
        ArtifactSubType subtype,
        String uriValue,
        String altName,
        EObject eObject) {
      this.toolkit = toolkit;
      this.namespace = namespace;
      this.name = name;
      this.type = type;
      this.subtype = subtype;
      this.uriValue = uriValue;
      this.altName = altName;
      this.eObject = eObject;
    }

    private Artifact(Toolkit toolkit, String namespace, String name, ArtifactType type) {
      this(toolkit, namespace, name, type, null, null, null, null);
    }

    public boolean isSample() {
      return toolkit.isSample();
    }

    // Can be used for user messages.
    public String toString() {
      return ArtifactString(toolkit, namespace, name, altName, type, subtype);
    }

    // Can be used for user messages where the containing file is known.
    public String toShortString() {
      return type + ":" + (type == ArtifactType.Function ? altName : name);
    }
  }

  /** A toolkit being processed. */
  public class Toolkit {
    private int toolkitNumber;
    private boolean isSample = false;
    private String toolkitPath;
    private ToolkitType toolkitModel;
    // A map of toolkit URI indexes that maps a URI index to the value of that index.
    // This map is used to get file paths referenced within a toolkit index.
    // The paths are required only while loading the models for the artifacts.
    private HashMap<Integer, String> toolkitUriIndexMap = null;

    private Toolkit(String toolkitPath, ToolkitType toolkitModel) {
      this.toolkitNumber = ++LastToolkitNumber;
      this.toolkitPath = toolkitPath;
      this.toolkitModel = toolkitModel;
    }

    public int getToolkitNumber() {
      return toolkitNumber;
    }

    public void setAsSample() {
      isSample = true;
    }

    public boolean isSample() {
      return isSample;
    }

    public String getToolkitPath() {
      return toolkitPath;
    }

    public ToolkitType getToolkitModel() {
      return toolkitModel;
    }

    /** Returns relative path in toolkit index. */
    public String getUriIndexValue(int uriIndex) throws IllegalArgumentException {
      if (toolkitUriIndexMap == null) {
        String[] args = {toolkitModel.getName()};
        String message =
            new Message(Key.CDISP7017E, (Object[]) args).getLocalizedMessageAndPrefix();
        throw new IllegalArgumentException(message);
      }
      String value = toolkitUriIndexMap.get(new Integer(uriIndex));
      if (value == null) {
        String[] args = {toolkitModel.getName(), uriIndex + ""};
        String message =
            new Message(Key.CDISP7018E, (Object[]) args).getLocalizedMessageAndPrefix();
        throw new IllegalArgumentException(message);
      }
      return value;
    }

    /** Returns relative path to model file for operator. */
    public String getModelFileNameForOp(PrimitiveToolkitOperatorType primOp) {
      String opPath = getUriIndexValue(primOp.getModelUriIndex());
      String name = primOp.getName();
      if (opPath.length() == 0) {
        // Default namespace
        return name + ".xml";
      }
      return opPath + Path.SEPARATOR + name + ".xml";
    }
  }

  // The singleton toolkit manager.
  private static ToolkitManager INSTANCE = null;

  // The collection of toolkits being processed.
  private Collection<Toolkit> Toolkits = new HashSet<Toolkit>();

  // A sequential number used to identify each toolkit.
  private int LastToolkitNumber = 0;

  // An index by name of all artifacts in all processed toolkits.
  // This is used to map an SPLDOC link to an SPL artifact to the location
  // in the SPLDOC output where the artifact is described.
  private Map<String, ArrayList<Artifact>> artifactsByName =
      new HashMap<String, ArrayList<Artifact>>();

  // An index by alternate name of all artifacts in all processed toolkits.
  private Map<String, ArrayList<Artifact>> artifactsByAltName =
      new HashMap<String, ArrayList<Artifact>>();

  // An index by artifact type of all artifacts in all processed toolkits.
  // This is used to produce lists of each main artifact type for all toolkits.
  private Map<ArtifactType, ArrayList<Artifact>> artifactsByType =
      new HashMap<ArtifactType, ArrayList<Artifact>>();

  // An index by artifact type of all artifacts in a given toolkit.
  // This is used to produce lists of each main artifact type for each toolkit.
  private Map<Toolkit, Map<ArtifactType, ArrayList<Artifact>>> artifactsByToolkitAndType =
      new HashMap<Toolkit, Map<ArtifactType, ArrayList<Artifact>>>();

  // Sets that define whether an artifact is excluded from the generated documentation.
  // An artifact is excluded if it is explicitly excluded, or if it is excluded at a
  // higher level (eg namespace) and not included at an equal or lower level.
  private HashSet<Artifact> ExcludedArtifacts = new HashSet<Artifact>();
  private HashSet<Artifact> IncludedArtifacts = new HashSet<Artifact>();
  private HashSet<Artifact> ContainsIncludedArtifacts = new HashSet<Artifact>();

  // File names used for native function model files.
  private static final String[] FunctionModelFileNames =
      new String[] {FileOutput.CppFunctionModelFileName, FileOutput.JavaFunctionModelFileName};

  public static ToolkitManager newInstance() {
    if (INSTANCE == null) {
      INSTANCE = new ToolkitManager();
    }
    return INSTANCE;
  }

  public static ToolkitManager getInstance() {
    if (INSTANCE == null) {
      new Exception("Tookit Instance not defined").printStackTrace();
      return null;
    }
    return INSTANCE;
  }

  /**
   * Return a string for an artifact that can be used for user messages. If the toolkit is
   * identified elsewhere in the message, toolkit can be set to null.
   */
  public static String ArtifactString(
      Toolkit toolkit, String namespace, String name, ArtifactType type) {
    String tkName = null;
    if (toolkit != null) {
      ToolkitType tkModel = toolkit.toolkitModel;
      tkName = tkModel.getName() + " " + tkModel.getVersion();
      if (type == ArtifactType.Toolkit) {
        return tkName;
      }
    }
    String nsName = (type == ArtifactType.Namespace ? name : namespace);
    if (nsName != null && nsName.isEmpty()) {
      nsName = "<" + logger.getMessage(Messages.MSG_DEFAULT_NAMESPACE_NAME) + ">";
    }
    StringBuilder sb = new StringBuilder();
    sb.append(type + ":");
    if (!ToolkitManager.NoNamespaceArtfacts.contains(type)) {
      sb.append(nsName + "::");
    }
    sb.append(name);
    if (tkName != null) {
      sb.append(" (");
      sb.append(tkName);
      sb.append(")");
    }
    return sb.toString();
  }

  /**
   * Return a string for an artifact that can be used for user messages. If the toolkit is
   * identified elsewhere in the message, toolkit can be set to null.
   */
  public static String ArtifactString(
      Toolkit toolkit,
      String namespace,
      String name,
      String altName,
      ArtifactType type,
      ArtifactSubType subtype) {
    return ArtifactString(
        toolkit,
        namespace,
        (type == ArtifactType.Function && subtype == ArtifactSubType.Native && altName != null)
            ? altName
            : name,
        type);
  }

  /** Return the container of the specified artifact, or null, if none. */
  private Artifact getContainer(Artifact artifact) {
    Artifact container = null;
    String containerName;
    switch (artifact.type) {
      case Toolkit:
        return null;
      case Namespace:
        container =
            getArtifact(
                artifact.toolkit,
                null,
                artifact.toolkit.getToolkitModel().getName(),
                ArtifactType.Toolkit);
        break;
      case Operator:
        switch (artifact.subtype) {
          case Composite:
            containerName = new File(artifact.uriValue).getName();
            container =
                getArtifact(
                    artifact.toolkit,
                    artifact.namespace,
                    containerName,
                    ArtifactType.CompilationUnit);
            break;
          case Primitive:
            container =
                getArtifact(artifact.toolkit, null, artifact.namespace, ArtifactType.Namespace);
            break;
          default:
            break;
        }
        break;
      case Function:
        switch (artifact.subtype) {
          case NonNative:
            containerName = new File(artifact.uriValue).getName();
            container =
                getArtifact(
                    artifact.toolkit,
                    artifact.namespace,
                    containerName,
                    ArtifactType.CompilationUnit);
            break;
          case Native:
            // The function model and function set have no SPLDOC descriptions, and so cannot be
            // excluded.
            container =
                getArtifact(artifact.toolkit, null, artifact.namespace, ArtifactType.Namespace);
            break;
          default:
            break;
        }
        break;
      case FunctionModel:
        switch (artifact.subtype) {
          case None:
            container =
                getArtifact(artifact.toolkit, null, artifact.namespace, ArtifactType.Namespace);
            break;
          default:
            break;
        }
        break;
      case Type:
        switch (artifact.subtype) {
          case Composite:
            containerName = artifact.name.substring(0, artifact.name.indexOf('.'));
            container =
                getArtifact(
                    artifact.toolkit, artifact.namespace, containerName, ArtifactType.Operator);
            break;
          case NonNative:
            containerName = new File(artifact.uriValue).getName();
            container =
                getArtifact(
                    artifact.toolkit,
                    artifact.namespace,
                    containerName,
                    ArtifactType.CompilationUnit);
            break;
          default:
            break;
        }
        break;
      case CompilationUnit:
        container = getArtifact(artifact.toolkit, null, artifact.namespace, ArtifactType.Namespace);
        break;
      default:
        break;
    }
    if (container == null) {
      logger.printException(
          new Exception(
              logger.getMessage(Key.CDISP7020E, new String[] {artifact.toolkit.getToolkitPath()})));
      return null;
    }
    return container;
  }

  /**
   * Returns true if a container of the specified artifact is set to be excluded from the generated
   * documentation, and there is no more immediate container that is explicitly included, otherwise
   * false.
   *
   * <p>Note that a container that is set to be excluded may still be shown in the output if it
   * contains an explicitly included artifact.
   */
  private boolean hasExcludedArtifactContainer(Artifact artifact) {
    if (ExcludedArtifacts.isEmpty()) {
      // No artifact is excluded.
      return false;
    }
    Artifact container = getContainer(artifact);
    if (container == null) {
      // There is no container.
      return false;
    }
    if (IncludedArtifacts.contains(container)) {
      // The container is explicitly included.
      return false;
    }
    if (ExcludedArtifacts.contains(container) || hasExcludedArtifactContainer(container)) {
      // The container or a higher-level container is explicitly excluded.
      return true;
    }
    // The container is not excluded.
    return false;
  }

  /**
   * Returns true if the specified artifact is to be excluded from the generated documentation,
   * otherwise false.
   */
  public boolean isExcludedArtifact(Artifact artifact) {
    if (ExcludedArtifacts.isEmpty()
        || IncludedArtifacts.contains(artifact)
        || ContainsIncludedArtifacts.contains(artifact)) {
      // No artifact is excluded, or this artifact or some content is explicitly included.
      return false;
    }
    if (ExcludedArtifacts.contains(artifact) || hasExcludedArtifactContainer(artifact)) {
      // Artifact or its container is explicitly excluded.
      return true;
    }
    // Artifact is included by default.
    return false;
  }

  /**
   * Specify that the given artifact is to be excluded from the generated documentation, unless it
   * is a container for an explicitly included artifact.
   */
  public void excludeArtifact(Artifact artifact) {
    if (inputParameters.isIncludeExcludedArtifacts()) {
      return;
    }
    ExcludedArtifacts.add(artifact);
  }

  /**
   * Specify that the given artifact is to be included in the generated documentation, even if its
   * container has been set to be excluded.
   */
  public void includeArtifact(Artifact artifact) {
    if (inputParameters.isIncludeExcludedArtifacts()) {
      return;
    }
    IncludedArtifacts.add(artifact);
    while ((artifact = getContainer(artifact)) != null) {
      if (!ContainsIncludedArtifacts.contains(artifact)) {
        ContainsIncludedArtifacts.add(artifact);
      }
    }
  }

  /**
   * Get the artifact specified by the given toolkit, namespace and type, and either the given name,
   * if not null, or the given alternate name. Returns null if the artifact is not found. The
   * artifact will be returned even if it has been excluded from the generated documentation.
   */
  private Artifact getArtifact(
      Toolkit toolkit, String namespace, String name, String altName, ArtifactType type) {
    ArrayList<Artifact> list =
        (name != null ? artifactsByName.get(name) : artifactsByAltName.get(altName));
    if (list == null) {
      return null;
    }
    Artifact artifact = null;
    int count = 0;
    for (Artifact a : list) {
      if (a.toolkit == toolkit
          && (a.namespace == null || a.namespace.equals(namespace))
          && ((name == null && a.altName.equals(altName)) || a.name.equals(name))
          && a.type == type) {
        artifact = a;
        count++;
      }
    }
    if (count > 1) {
      logger.printException(
          new Exception(
              logger.getMessage(Key.CDISP7020E, new String[] {toolkit.getToolkitPath()})));
    }
    return artifact;
  }

  /**
   * Get the artifact specified by the given toolkit, namespace, name and type. Returns null if the
   * artifact is not found. The artifact will be returned even if it has been excluded from the
   * generated documentation.
   */
  public Artifact getArtifact(Toolkit toolkit, String namespace, String name, ArtifactType type) {
    return getArtifact(toolkit, namespace, name, null, type);
  }

  /**
   * Get all artifacts from all toolkits by toolkit, namespace, alternate name and type. Returns
   * null if the artifact is not found. The artifact will be returned even if it has been excluded
   * from the generated documentation. This is used when the usual name is not available.
   */
  public Artifact getArtifactByAltName(
      Toolkit toolkit, String namespace, String altName, ArtifactType type) {
    return getArtifact(toolkit, namespace, null, altName, type);
  }

  /**
   * Given a set of SPL artifacts and an artifact, this method returns the set of artifacts that
   * share the same properties, such as name, namespace, and type, as the input artifact s.
   * Artifacts that have been excluded from the generated documentation are not included.
   */
  private ArrayList<Artifact> getArtifactSubset(Artifact s, ArrayList<Artifact> list) {
    ArrayList<Artifact> artifact_subset = new ArrayList<Artifact>();
    for (Artifact a : list) {
      if (s.equals(a) && !isExcludedArtifact(a)) {
        artifact_subset.add(a);
      }
    }
    return artifact_subset;
  }

  /**
   * Get all artifacts from all toolkits by toolkit, namespace, name and type. Each parameter is
   * optional, and may be specified as null, except name. Returns an empty ArrayList if none match.
   * Artifacts that have been excluded from the generated documentation are not included.
   */
  public List<Artifact> getArtifacts(
      Toolkit toolkit, String namespace, String name, ArtifactType type) {
    ArrayList<Artifact> list = artifactsByName.get(name);
    if (list == null) {
      return new ArrayList<Artifact>();
    }
    return getArtifactSubset(new Artifact(toolkit, namespace, name, type), list);
  }

  /**
   * Get all artifacts from all toolkits by namespace, name and type. Each parameter is optional,
   * and may be specified as null, except name. Returns an empty ArrayList if none match. Artifacts
   * that have been excluded from the generated documentation are not included.
   */
  public List<Artifact> getArtifacts(String namespace, String name, ArtifactType type) {
    return getArtifacts(null, namespace, name, type);
  }

  /**
   * Given a set of SPL artifacts, return the subset that includes only artifacts that are to be
   * included in the generated documentation.
   */
  private ArrayList<Artifact> getIncludedArtifacts(ArrayList<Artifact> list) {
    ArrayList<Artifact> artifact_subset = new ArrayList<Artifact>();
    for (Artifact a : list) {
      if (!isExcludedArtifact(a)) {
        artifact_subset.add(a);
      }
    }
    return artifact_subset;
  }

  /**
   * Get all artifacts from the main or, if isSample is true, all the sample toolkits by type.
   * Returns an empty ArrayList if there are none. Artifacts that have been excluded from the
   * generated documentation are not included.
   */
  public List<Artifact> getArtifactsByType(ArtifactType type, boolean isSample) {
    ArrayList<Artifact> artifact_subset = new ArrayList<Artifact>();
    ArrayList<Artifact> list = artifactsByType.get(type);
    if (list == null) return artifact_subset;
    for (Artifact a : getIncludedArtifacts(list)) {
      if (a.isSample() == isSample) {
        artifact_subset.add(a);
      }
    }
    return artifact_subset;
  }

  /**
   * Get all artifacts from the specified toolkit by type. Returns an empty ArrayList if there are
   * none. Artifacts that have been excluded from the generated documentation are not included.
   */
  public List<Artifact> getArtifactsByToolkitAndType(Toolkit toolkit, ArtifactType type) {
    Map<ArtifactType, ArrayList<Artifact>> map = artifactsByToolkitAndType.get(toolkit);
    if (map == null) return new ArrayList<Artifact>();
    ArrayList<Artifact> list = map.get(type);
    if (list == null) return new ArrayList<Artifact>();
    return getIncludedArtifacts(list);
  }

  /**
   * Get all artifacts from the specified toolkit by namespace and type, and by optional subtype and
   * URI value. Returns an empty ArrayList if there are none. Artifacts that have been excluded from
   * the generated documentation are not included.
   */
  private List<Artifact> getArtifactsByToolkitNamespaceTypeOptionalSubTypeUri(
      Toolkit toolkit,
      String namespace,
      ArtifactType type,
      ArtifactSubType subtype,
      String uriValue) {
    ArrayList<Artifact> list = new ArrayList<Artifact>();
    Map<ArtifactType, ArrayList<Artifact>> map = artifactsByToolkitAndType.get(toolkit);
    if (map == null) return list;
    ArrayList<Artifact> listByType = map.get(type);
    if (listByType == null) return list;
    for (Artifact artifact : listByType) {
      if (artifact.namespace.equals(namespace)
          && (subtype == null || artifact.subtype == subtype)
          && (uriValue == null || artifact.uriValue.equals(uriValue))
          && !isExcludedArtifact(artifact)) {
        list.add(artifact);
      }
    }
    return list;
  }

  /**
   * Get all artifacts from the specified toolkit by namespace and type. Returns an empty ArrayList
   * if there are none. Artifacts that have been excluded from the generated documentation are not
   * included.
   */
  public List<Artifact> getArtifactsByToolkitNamespaceAndType(
      Toolkit toolkit, String namespace, ArtifactType type) {
    return getArtifactsByToolkitNamespaceTypeOptionalSubTypeUri(
        toolkit, namespace, type, null, null);
  }

  /**
   * Get all artifacts from the specified toolkit by namespace, type and subtype. Returns an empty
   * ArrayList if there are none. Artifacts that have been excluded from the generated documentation
   * are not included.
   */
  public List<Artifact> getArtifactsByToolkitNamespaceTypeAndSubType(
      Toolkit toolkit, String namespace, ArtifactType type, ArtifactSubType subtype) {
    return getArtifactsByToolkitNamespaceTypeOptionalSubTypeUri(
        toolkit, namespace, type, subtype, null);
  }

  /**
   * Get all artifacts from the specified toolkit by namespace, type and URI value. Returns an empty
   * ArrayList if there are none. Artifacts that have been excluded from the generated documentation
   * are not included.
   */
  public List<Artifact> getArtifactsByToolkitNamespaceTypeAndUri(
      Toolkit toolkit, String namespace, ArtifactType type, String uriValue) {
    return getArtifactsByToolkitNamespaceTypeOptionalSubTypeUri(
        toolkit, namespace, type, null, uriValue);
  }

  /** Add toolkit to ToolkitManager's index of SPL artifacts. */
  private void addArtifact(Toolkit toolkit) {
    addArtifact(
        toolkit,
        null,
        toolkit.getToolkitModel().getName(),
        ArtifactType.Toolkit,
        ArtifactSubType.None,
        null,
        null,
        null);
  }

  /** Add namespace to ToolkitManager's index of SPL artifacts. */
  private void addArtifact(Toolkit toolkit, String namespace) {
    addArtifact(
        toolkit, null, namespace, ArtifactType.Namespace, ArtifactSubType.None, null, null, null);
  }

  /**
   * Add composite operator to ToolkitManager's index of SPL artifacts. Also add any composite
   * operator static types for this composite operator.
   */
  private void addArtifact(Toolkit toolkit, String namespace, CompositeToolkitOperatorType op) {
    if (!op.isPublic() && !inputParameters.isIncludePrivateArtifacts()) {
      // Do not add a private composite operator or its components. */
      return;
    }
    String uriValue = toolkit.getUriIndexValue(op.getUriIndex());
    String compName = op.getName();
    // Add composite operator.
    addArtifact(
        toolkit,
        namespace,
        compName,
        ArtifactType.Operator,
        ArtifactSubType.Composite,
        uriValue,
        null,
        null);

    // Add composite operator static types.
    for (ToolkitTypeType type : op.getType()) {
      if (type.isStatic()) {
        addArtifact(
            toolkit,
            namespace,
            compName + "." + type.getName(),
            ArtifactType.Type,
            ArtifactSubType.Composite,
            uriValue,
            null,
            null);
      }
    }
  }

  /** Add primitive operator to ToolkitManager's index of SPL artifacts. */
  private void addArtifact(Toolkit toolkit, String namespace, PrimitiveToolkitOperatorType op) {
    String uriValue = toolkit.getUriIndexValue(op.getModelUriIndex());
    addArtifact(
        toolkit,
        namespace,
        op.getName(),
        ArtifactType.Operator,
        ArtifactSubType.Primitive,
        uriValue,
        null,
        null);
  }

  /** Adds a function to ToolkitManager's index of artifacts. */
  private void addArtifact(Toolkit toolkit, String namespace, ToolkitFunctionType function) {
    String uriValue;
    ArtifactSubType subType;
    // Use the function signature to uniquely identify a function.
    // For native functions, we also need to be able to find the function using the full prototype
    // string.
    String altName = function.getPrototype();
    if (function.isNative()) {
      subType = ArtifactSubType.Native;
      uriValue = toolkit.getUriIndexValue(function.getModelUriIndex());
      if (!isFunctionModelPathName(uriValue)) {
        logger.printError(Key.CDISP7128E, new String[] {uriValue, toolkit.getToolkitPath()});
        throw new IllegalArgumentException();
      }
      // Add the function model for the namespace, if not already added.
      String modelName = new File(uriValue).getName();
      if (getArtifact(toolkit, namespace, modelName, ArtifactType.FunctionModel) == null) {
        addArtifact(
            toolkit,
            namespace,
            modelName,
            ArtifactType.FunctionModel,
            ArtifactSubType.None,
            uriValue,
            null,
            null);
      }
    } else {
      if (!function.isPublic() && !inputParameters.isIncludePrivateArtifacts()) {
        // Do not add a private SPL function. */
        return;
      }
      uriValue = toolkit.getUriIndexValue(function.getUriIndex());
      subType = ArtifactSubType.NonNative;
    }
    // Encode function prototypes so they can be used in cross-reference links.
    String overloaded_name = Encoder.getOverloadedName(function);
    addArtifact(
        toolkit,
        namespace,
        overloaded_name,
        ArtifactType.Function,
        subType,
        uriValue,
        altName,
        function);
  }

  /** Adds a standalone type to ToolkitManager's index of artifacts. */
  private void addArtifact(Toolkit toolkit, String namespace, ToolkitTypeType type) {
    String uriValue = toolkit.getUriIndexValue(type.getUriIndex());
    addArtifact(
        toolkit,
        namespace,
        type.getName(),
        ArtifactType.Type,
        ArtifactSubType.NonNative,
        uriValue,
        null,
        null);
  }

  /**
   * Adds an SPL artifact to the ToolkitManager's indexes of artifacts. Also adds the containing SPL
   * compilation unit, if any, if not already added.
   */
  private void addArtifact(
      Toolkit toolkit,
      String namespace,
      String name,
      ArtifactType type,
      ArtifactSubType subtype,
      String uriValue,
      String altName,
      EObject eObject) {
    Artifact artifact =
        new Artifact(toolkit, namespace, name, type, subtype, uriValue, altName, eObject);
    ArrayList<Artifact> list;
    // Add artifact to index by artifact name.
    list = artifactsByName.get(name);
    if (list == null) {
      list = new ArrayList<Artifact>();
      artifactsByName.put(name, list);
    }
    list.add(artifact);
    if (altName != null) {
      // Add artifact to index by alternate artifact name.
      list = artifactsByAltName.get(altName);
      if (list == null) {
        list = new ArrayList<Artifact>();
        artifactsByAltName.put(altName, list);
      }
      list.add(artifact);
    }
    // Add artifact to index by artifact type.
    list = artifactsByType.get(type);
    if (list == null) {
      list = new ArrayList<Artifact>();
      artifactsByType.put(type, list);
    }
    list.add(artifact);
    // Add artifact to index by artifact toolkit and type.
    Map<ArtifactType, ArrayList<Artifact>> map = artifactsByToolkitAndType.get(toolkit);
    if (map == null) {
      map = new HashMap<ArtifactType, ArrayList<Artifact>>();
      artifactsByToolkitAndType.put(toolkit, map);
    }
    list = map.get(type);
    if (list == null) {
      list = new ArrayList<Artifact>();
      map.put(type, list);
    }
    list.add(artifact);
    // Add the containing SPL file compilation unit artifact, if any, if not already added.
    if (uriValue == null) return;
    File artifactFile = new File(toolkit.getToolkitPath(), uriValue);
    String fileName = artifactFile.getName();
    if (!artifactFile.exists()) {
      logger.printError(Key.CDISP7016E, new String[] {artifactFile.toString()});
      ErrorManager.throwFatalErrorException();
    }
    if (!uriValue.endsWith(".spl")
        || getArtifact(toolkit, namespace, fileName, ArtifactType.CompilationUnit) != null) {
      return;
    }
    addArtifact(
        toolkit,
        namespace,
        fileName,
        ArtifactType.CompilationUnit,
        ArtifactSubType.None,
        uriValue,
        null,
        null);
  }

  /** Utility for printing out artifact records. Useful for debugging. */
  public void printArtifacts(Toolkit toolkit) {
    System.out.println(
        "Toolkit Artifact Index for toolkit: " + toolkit.getToolkitModel().getName());
    for (String k : artifactsByName.keySet()) {
      for (Artifact a : artifactsByName.get(k)) {
        if (a.toolkit == toolkit) {
          if (k != null) {
            System.out.println(" key: " + k);
            k = null;
          }
          System.out.println("  " + a.toString());
        }
      }
    }
  }

  /**
   * Given a toolkit, namespace and composite operator name, return the relative path (namespace + /
   * + SPL file name) containing the operator.
   *
   * @param namespace
   * @param name
   * @return relative path, with respect to toolkit root, of the operator. Returns null if no match
   *     found
   */
  public String getPathForCompOp(Toolkit toolkit, String namespace, String name) {
    assert (namespace != null);
    assert (name != null);
    Artifact artifact = getArtifact(toolkit, namespace, name, ArtifactType.Operator);
    if (artifact == null || artifact.subtype != ToolkitManager.ArtifactSubType.Composite) {
      return null;
    }
    return artifact.uriValue;
  }

  /**
   * Given a toolkit, namespace and SPL type name, return the relative path (namespace + / + SPL
   * file name) containing the type definition.
   *
   * @param namespace
   * @param name
   * @return relative path, with respect to toolkit root, of the type. Returns null if no match
   *     found
   */
  public String getPathForType(Toolkit toolkit, String namespace, String name) {
    assert (namespace != null);
    assert (name != null);
    Artifact artifact = getArtifact(toolkit, namespace, name, ArtifactType.Type);
    if (artifact == null) {
      return null;
    }
    return artifact.uriValue;
  }

  /** Caches information about the given toolkit. */
  public Toolkit addToolkit(String toolkitPath, ToolkitType toolkitModel) {
    String toolkitName = toolkitModel.getName();
    String toolkitVersion = toolkitModel.getVersion();
    String[] args = {toolkitName, toolkitVersion};
    logger.printTrace(Key.CDISP7084I, args);

    // Do not allow multiple copies or versions of the same toolkit.
    List<ToolkitManager.Artifact> list = getArtifacts(null, toolkitName, ArtifactType.Toolkit);
    if (list.size() > 0) {
      logger.printError(
          Key.CDISP7019E,
          new String[] {toolkitPath, list.get(0).toolkit.getToolkitPath(), toolkitName});
      return null;
    }

    // Add the toolkit to the list of toolkits to be processed.
    Toolkit toolkit = new Toolkit(toolkitPath, toolkitModel);
    Toolkits.add(toolkit);

    // Create the toolkit URI index map.
    UriTableType uriTable = toolkitModel.getUriTable();
    if (uriTable == null) return toolkit;
    EList<UriType> uriList = uriTable.getUri();
    if (uriList == null) return toolkit;
    toolkit.toolkitUriIndexMap = new HashMap<Integer, String>();
    for (UriType uri : uriList) {
      toolkit.toolkitUriIndexMap.put(new Integer(uri.getIndex()), uri.getValue());
    }

    // Add the toolkit to the collection of all toolkit artifacts that can be
    // linked to using an SPLDOC cross-reference link.
    addArtifact(toolkit);

    // Add namespaces.
    for (ToolkitNamespaceType namespace : toolkitModel.getNamespace()) {
      String namespaceName = namespace.getName();
      addArtifact(toolkit, namespaceName);

      // Add primitive operators.
      for (PrimitiveToolkitOperatorType primitiveOp : namespace.getPrimitiveOp()) {
        addArtifact(toolkit, namespaceName, primitiveOp);
      }

      // Add composite operators and their static types.
      for (CompositeToolkitOperatorType compositeOp : namespace.getCompositeOp()) {
        addArtifact(toolkit, namespaceName, compositeOp);
      }

      // Add functions.
      for (ToolkitFunctionType function : namespace.getFunction()) {
        addArtifact(toolkit, namespaceName, function);
      }

      // Add standalone types.
      for (ToolkitTypeType type : namespace.getType()) {
        addArtifact(toolkit, namespaceName, type);
      }
    }
    // printArtifacts(toolkit);
    return toolkit;
  }

  /** Return the toolkits that were added. */
  public Collection<Toolkit> getToolkits() {
    return Toolkits;
  }

  public List<String> getNativeFunctionModelFiles(ToolkitType toolkit) {
    ArrayList<String> modelFileList = new ArrayList<String>();
    UriTableType uriTable = toolkit.getUriTable();
    if (uriTable == null) {
      return null;
    }
    EList<UriType> uris = uriTable.getUri();
    if (uris == null) {
      return null;
    }
    for (UriType uri : uris) {
      if (isFunctionModelPathName(uri.getValue())) {
        modelFileList.add(uri.getValue());
      }
    }
    return modelFileList;
  }

  private boolean isFunctionModelPathName(String path) {
    File file = new File(path);
    File dir = file.getParentFile();
    if (dir == null || !dir.getName().equals(FileOutput.functionModelDirName)) {
      return false;
    }
    String fileName = file.getName();
    for (String modelFileName : FunctionModelFileNames) {
      if (fileName.equals(modelFileName)) {
        return true;
      }
    }
    return false;
  }

  /**
   * Return the name to be used for the document file for the toolkit.
   *
   * <p>Currently this is the toolkit name. However, since a toolkit name can have any characters,
   * it may become necessary to clean the names in the future.
   */
  public static String getSanitizedToolkitName(String toolkitName) {
    return toolkitName;
  }
}
