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

// TODO: Code retired. Delete this class

/*
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.ecore.resource.impl.ResourceSetImpl;

import com.ibm.streams.tools.doc.core.docmodel.AnnotationType;
import com.ibm.streams.tools.doc.core.docmodel.CompositeDocumentType;
import com.ibm.streams.tools.doc.core.docmodel.DescriptionType;
import com.ibm.streams.tools.doc.core.docmodel.DocmodelPackage;
import com.ibm.streams.tools.doc.core.docmodel.IncludeType;
import com.ibm.streams.tools.doc.core.docmodel.InputPortType;
import com.ibm.streams.tools.doc.core.docmodel.LinkType;
import com.ibm.streams.tools.doc.core.docmodel.ListType;
import com.ibm.streams.tools.doc.core.docmodel.OperatorParameterType;
import com.ibm.streams.tools.doc.core.docmodel.OperatorType;
import com.ibm.streams.tools.doc.core.docmodel.OrderedListType;
import com.ibm.streams.tools.doc.core.docmodel.OutputPortType;
import com.ibm.streams.tools.doc.core.docmodel.OverviewType;
import com.ibm.streams.tools.doc.core.docmodel.ParagraphSegmentType;
import com.ibm.streams.tools.doc.core.docmodel.ParagraphType;
import com.ibm.streams.tools.doc.core.docmodel.ParameterType;
import com.ibm.streams.tools.doc.core.docmodel.SectionSegmentType;
import com.ibm.streams.tools.doc.core.docmodel.SectionType;
import com.ibm.streams.tools.doc.core.docmodel.SourceCodeDocumentType;
import com.ibm.streams.tools.doc.core.docmodel.SourceCodeDocumentsType;
import com.ibm.streams.tools.doc.core.docmodel.ToolkitInfoType;
import com.ibm.streams.tools.doc.core.docmodel.UnorderedListType;
import com.ibm.streams.tools.doc.core.docmodel.ValueType;
import com.ibm.streams.tools.doc.core.docmodel.impl.DocmodelFactoryImpl;
*/
/**
 * Represents the document model for a toolkit.
 *
 * <p>The document model contains multiple source document objects, one for each SPL file.
 *
 * @author andyfrenkiel
 */
public class SourceDocumentationModel {
  //
  //	String toolkitName;
  //	String toolkitVersion;
  //	String toolkitRequiredProductVersion;
  //	String description = "Not specified.";
  //
  //	SourceCodeDocumentsType sourceCodeDocuments = null;
  //	DocmodelFactoryImpl factory = null;
  //
  //	public String getToolkitName() {
  //		return toolkitName;
  //	}
  //
  //	public void setToolkitName(String toolkitName) {
  //		this.toolkitName = toolkitName;
  //	}
  //
  //	public String getToolkitVersion() {
  //		return toolkitVersion;
  //	}
  //
  //	public void setToolkitVersion(String toolkitVersion) {
  //		this.toolkitVersion = toolkitVersion;
  //	}
  //
  //	public String getToolkitRequiredProductVersion() {
  //		return toolkitRequiredProductVersion;
  //	}
  //
  //	public void setToolkitRequiredProductVersion(
  //			String toolkitRequiredProductVersion) {
  //		this.toolkitRequiredProductVersion = toolkitRequiredProductVersion;
  //	}
  //
  //	public SourceDocumentationModel(String toolkitName, String toolkitVersion, String
  // toolkitRequiredProductVersion,
  // String description) {
  //		this.toolkitName = toolkitName;
  //		this.toolkitVersion = toolkitVersion;
  //		this.toolkitRequiredProductVersion = toolkitRequiredProductVersion;
  //		this.description = description;
  //	}
  //
  //	// Default constructor not supported.
  //	private SourceDocumentationModel() {
  //	}
  //
  //
  //	/**
  //	 * Get a document model object for a toolkit.
  //	 *
  //	 * Returns a model object for the toolkit if it's already been created,
  //	 * or creates a new one, otherwise.
  //	 */
  //	public SourceCodeDocumentsType get() {
  //
  //		if (sourceCodeDocuments != null) {
  //			return sourceCodeDocuments;
  //		}
  //
  //		ResourceSet resourceSet = new ResourceSetImpl();
  //		 resourceSet.getResourceFactoryRegistry().getExtensionToFactoryMap().put
  //			(Resource.Factory.Registry.DEFAULT_EXTENSION,
  //			 new DocmodelFactoryImpl());
  //
  //		 // Register the package to ensure it is available during loading.
  //		 resourceSet.getPackageRegistry().put(DocmodelPackage.eNS_URI,
  //				                              DocmodelPackage.eINSTANCE);
  //
  //		 factory = new DocmodelFactoryImpl();
  //		 sourceCodeDocuments = factory.createSourceCodeDocumentsType();
  //
  //		 ToolkitInfoType toolkit = factory.createToolkitInfoType();
  //		 toolkit.setName(toolkitName);
  //		 toolkit.setVersion(toolkitVersion);
  //		 toolkit.setRequiredProductVersion(toolkitRequiredProductVersion);
  //		 toolkit.setDescription(description);
  //		 sourceCodeDocuments.setToolkit(toolkit);
  //
  //		 return sourceCodeDocuments;
  //	}
  //
  //	/**
  //	 * Creates as a new model object for an SPL source code document.
  //	 * @param description a description of the SPL source document
  //	 * @return a model object representing an SPL source document.
  //	 */
  //	public SourceCodeDocumentType createSouceCodeDoc(String description) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		SourceCodeDocumentType sourceCodeDocument = factory.createSourceCodeDocumentType();
  //		EList<SourceCodeDocumentType> docs = sourceCodeDocuments.getDocuments();
  //		docs.add(sourceCodeDocument);
  //		OverviewType overview = factory.createOverviewType();
  //		if (description != null) {
  //		   overview.setDescription(description);
  //		}
  //		sourceCodeDocument.setOverview(overview);
  //		return sourceCodeDocument;
  //	}
  //
  //	public CompositeDocumentType createCompositeDoc(SourceCodeDocumentType sourceCodeDoc) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		CompositeDocumentType compositeDocument = factory.createCompositeDocumentType();
  //		EList<CompositeDocumentType> compositeDocuments = sourceCodeDoc.getComposites();
  //		compositeDocuments.add(compositeDocument);
  //		return compositeDocument;
  //	}
  //
  //	public InputPortType createInputPort(CompositeDocumentType compositeDocument) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		InputPortType port = factory.createInputPortType();
  //		EList<InputPortType> inputPorts = compositeDocument.getInputPorts();
  //		inputPorts.add(port);
  //		return port;
  //	}
  //
  //	public OperatorType createOperator(CompositeDocumentType compositeDocument) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		OperatorType operator = factory.createOperatorType();
  //		EList<OperatorType> operators = compositeDocument.getOperators();
  //		operators.add(operator);
  //		return operator;
  //	}
  //
  //	public OperatorParameterType createParameter(OperatorType operator) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		OperatorParameterType parameter = factory.createOperatorParameterType();
  //		EList<OperatorParameterType> parameters = operator.getParameters();
  //		parameters.add(parameter);
  //		return parameter;
  //	}
  //
  //
  //	public OutputPortType createOutputPort(CompositeDocumentType compositeDocument) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		OutputPortType port = factory.createOutputPortType();
  //		EList<OutputPortType> outputPorts = compositeDocument.getOutputPorts();
  //		outputPorts.add(port);
  //		return port;
  //	}
  //
  //	public AnnotationType createTag(SourceCodeDocumentType sourceCodeDoc) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		EList<AnnotationType> annotations = sourceCodeDoc.getTags();
  //		AnnotationType annotation = factory.createAnnotationType();
  //		annotations.add(annotation);
  //		return annotation;
  //	}
  //
  //	public AnnotationType createTag(CompositeDocumentType compositeDocument) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		EList<AnnotationType> annotations = compositeDocument.getTags();
  //		AnnotationType annotation = factory.createAnnotationType();
  //		annotations.add(annotation);
  //		return annotation;
  //	}
  //
  //	public DescriptionType createParagraph(CompositeDocumentType compositeDocument) {
  //		DescriptionType description = factory.createDescriptionType();
  //		return null;
  //	}
  //
  //	public ParameterType createParameter(CompositeDocumentType compositeDocument) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		EList<ParameterType> parameters = compositeDocument.getParameters();
  //		ParameterType parameter = factory.createParameterType();
  //		parameters.add(parameter);
  //		return parameter;
  //	}
  //
  //	public ValueType createComplexContent(AnnotationType annotation) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		ValueType valueType = factory.createValueType();
  //		annotation.setComplexContent(valueType);
  //		return valueType;
  //	}
  //
  //	public SectionType createSection(ValueType complexContent, String name) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		SectionType section = factory.createSectionType();
  //		section.setName(name);
  //		complexContent.setSection(section);
  //		return section;
  //	}
  //
  //	public SectionType createSection(SectionSegmentType sectionSegment, String name) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //
  //		SectionType segment = factory.createSectionType();
  //		segment.setName(name);
  //		EList<SectionType> segments = sectionSegment.getSections();
  //		segments.add(segment);
  //		return segment;
  //	}
  //
  //	public SectionSegmentType createSectionSegment(SectionType section) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //
  //		SectionSegmentType segment = factory.createSectionSegmentType();
  //		EList<SectionSegmentType> segments = section.getSegments();
  //		segments.add(segment);
  //		return segment;
  //	}
  //
  //	public ParagraphType createParagraph(SectionType section) {
  //		SectionSegmentType segment = createSectionSegment(section);
  //		return createParagraph(segment);
  //	}
  //
  //	public ParagraphType createParagraph(SectionSegmentType segment) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		EList<ParagraphType> paragraphs = segment.getParagraphs();
  //		ParagraphType paragraph = factory.createParagraphType();
  //		paragraphs.add(paragraph);
  //		return paragraph;
  //	}
  //
  //	public IncludeType createIncludeCode(SectionType section) {
  //		SectionSegmentType segment = createSectionSegment(section);
  //		return createIncludeCode(segment);
  //	}
  //
  //
  //	public IncludeType createIncludeCode(SectionSegmentType segment) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		EList<IncludeType> includes = segment.getIncludes();
  //		IncludeType include = factory.createIncludeType();
  //		include.setSourceLocation(factory.createSourceLocationType());
  //		includes.add(include);
  //		return include;
  //	}
  //
  //	public ParagraphSegmentType createParagraphSegment(ParagraphType paragraph) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		EList<ParagraphSegmentType> segments = paragraph.getSegments();
  //		ParagraphSegmentType segment = factory.createParagraphSegmentType();
  //		segments.add(segment);
  //		return segment;
  //	}
  //
  //	public LinkType createLink(ParagraphSegmentType segment) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		LinkType link = factory.createLinkType();
  //		segment.setLink(link);
  //		return link;
  //	}
  //
  //	public String createCode(SectionType section, String code) {
  //		SectionSegmentType segment = createSectionSegment(section);
  //		return createCode(segment, code);
  //	}
  //
  //	public String createCode(SectionSegmentType segment, String code) {
  //		EList<String> codes = segment.getCode();
  //		codes.add(code);
  //		return code;
  //	}
  //
  //	public UnorderedListType createUnorderedList(SectionType section) {
  //		SectionSegmentType segment = createSectionSegment(section);
  //		return createUnorderedList(segment);
  //	}
  //
  //	public UnorderedListType createUnorderedList(SectionSegmentType segment) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		EList<UnorderedListType> lists = segment.getUnorderedList();
  //		UnorderedListType list = factory.createUnorderedListType();
  //		lists.add(list);
  //		return list;
  //	}
  //
  //	public OrderedListType createOrderedList(SectionType section) {
  //		SectionSegmentType segment = createSectionSegment(section);
  //		return createOrderedList(segment);
  //	}
  //
  //	public OrderedListType createOrderedList(SectionSegmentType segment) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		EList<OrderedListType> lists = segment.getOrderedList();
  //		OrderedListType list = factory.createOrderedListType();
  //		lists.add(list);
  //		return list;
  //	}
  //
  //	public ParagraphType createListItem(ListType list) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		EList<ParagraphType> listitems = list.getListitems();
  //		ParagraphType paragraph = factory.createParagraphType();
  //		listitems.add(paragraph);
  //		return paragraph;
  //	}
  //
  //	public ParagraphType createListItem(OrderedListType list) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		EList<ParagraphType> listitems = list.getListitems();
  //		ParagraphType paragraph = factory.createParagraphType();
  //		listitems.add(paragraph);
  //		return paragraph;
  //	}
  //
  //	public ParagraphType createListItem(UnorderedListType list) {
  //		if (sourceCodeDocuments == null) {
  //			throw new IllegalStateException("SouceCodeDocuments not yet created.  Invoke get method");
  //		}
  //		assert(factory != null);
  //		EList<ParagraphType> listitems = list.getListitems();
  //		ParagraphType paragraph = factory.createParagraphType();
  //		listitems.add(paragraph);
  //		return paragraph;
  //	}
  //
  //	public void print() {
  //		EList<SourceCodeDocumentType> docs = sourceCodeDocuments.getDocuments();
  //		for (SourceCodeDocumentType doc : docs) {
  //			System.out.println("doc overview: " + doc.getOverview().getDescription());
  //			System.out.println("    namespace: " + doc.getNamespace());
  //			System.out.println("    file name: " + doc.getFileName());
  //			EList<CompositeDocumentType> compositeDocuments = doc.getComposites();
  //			for (CompositeDocumentType compositeDocument : compositeDocuments) {
  //				System.out.println("    composite name: " + compositeDocument.getName());
  //				System.out.println("              description: " + compositeDocument.getDescription());
  //				EList<InputPortType> inputPorts = compositeDocument.getInputPorts();
  //				System.out.println("              tags:");
  //				EList<AnnotationType> annotations = compositeDocument.getTags();
  //				for (AnnotationType annotation : annotations) {
  //					System.out.println("                tags: description: " + annotation.getSimpleContent() +
  // ", target: " +
  // annotation.getTarget());
  //				}
  //				System.out.println("    input ports: ");
  //				for (InputPortType inputPort : inputPorts) {
  //					System.out.println("      name: " + inputPort.getName()
  //							+ ", type: " + inputPort.getType()
  //							+ ", description: " + inputPort.getDescription());
  //				}
  //				EList<OutputPortType> outputPorts = compositeDocument.getOutputPorts();
  //				System.out.println("    output ports: ");
  //				for (OutputPortType outputPort : outputPorts) {
  //					System.out.println("      name: " + outputPort.getName()
  //							+ ", type: " + outputPort.getType()
  //							+ ", description: " + outputPort.getDescription());
  //				}
  //			}
  //
  //		}
  //	}
  //
  //	/**
  //	 * @param args
  //	 */
  //	public static void main(String[] args) {
  //		SourceDocumentationModel docModel = new SourceDocumentationModel();
  //		SourceCodeDocumentsType sourceCodeDocuments = docModel.get();
  //		SourceCodeDocumentType sourceCodeDocument = docModel.createSouceCodeDoc("A description of the
  // SPL source code");
  //
  //		EList<SourceCodeDocumentType> docs = sourceCodeDocuments.getDocuments();
  //		for (SourceCodeDocumentType doc : docs) {
  //			System.out.println("doc, overview: " + doc.getOverview().getDescription());
  //		}
  //
  //	}

}
