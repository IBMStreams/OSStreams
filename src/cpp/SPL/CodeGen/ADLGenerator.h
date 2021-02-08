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

#ifndef SPL_CODE_GEN_ADL_GENERATOR_H
#define SPL_CODE_GEN_ADL_GENERATOR_H

#include <SPL/CodeGen/Config.h>
#include <SPL/CodeGen/Expression.h>
#include <SPL/Core/CompilationState.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <UTILS/HashMapHelpers.h>
#include <string>

// forward declarations
namespace xmlns {
namespace prod {
namespace streams {
namespace application {
namespace v4200 {
class applicationSetType;
class applicationType;
class attributeType;
class checkpointPolicyType;
class checkpointingType;
class coLocationType;
class compositeOperInstanceType;
class compositeOperInstancesType;
class compositeOperDefinitionType;
class compositeOperDefinitionsType;
class compositesType;
class congestionPolicyType;
class contextType;
class customMetricsType;
class dependenciesType;
class encodingType;
class envVarType;
class envVarsType;
class exLocationType;
class executableType;
class hostLocationType;
class hostType;
class hostpoolType;
class hostpoolsType;
class languageType;
class librariesType;
class libraryDependencyType;
class libraryType;
class nameBasedImportType;
class primitiveOperDefinitionsType;
class parallelRegionsType;
class pesType;
class poolLocationType;
class primitiveOperInstanceBaseType;
class primitiveImportOperInstanceType;
class primitiveExportOperInstanceType;
class primitiveOperInstancePortType;
class primitiveOperInstanceType;
class propertyBasedImportType;
class resourcesType;
class runtimeConstantType;
class runtimeConstantsType;
class sourceLocationType;
class splAnnotationType;
class streamPropertiesType;
class streamPropertyType;
class submissionTimeValueType;
class submissionTimeValuesType;
class threadedPortType;
class toolkitType;
class toolkitsType;
class transportType;
class tupleTypeType;
class tupleTypesType;
class uriType;
class urisType;
}
}
}
}
}
namespace xmlns {
namespace prod {
namespace streams {
namespace runtime {
namespace utils {
namespace mr {
class tracingLevelType;
}
}
}
}
}
}

namespace SPL {
class TupleTyp;
class PrimitiveOperatorInstance;
class CompositeOperatorInstance;
class OperatorGraphNode;
class CompositeDefinition;
class ExpressionValue;
class SourceLocation;
class SPLSimpleAnnotation;
class SPLParallelAnnotation;
class StreamUseDefLocation;

namespace Operator {
class OperatorModel;
namespace Instance {
class RuntimeConstant;
}
}

class ADLGenerator
{
  public:
    /// Constructor
    ADLGenerator();
    ADLGenerator(CompilationState* statePtr);

    /// Do the work to generate the ADL file
    /// Return the name of the adl file
    std::string generate();

  private:
    struct ParallelRegionInfo
    {
        ParallelRegionInfo(const std::string& n,
                           uint32_t i,
                           const SPLParallelAnnotation& annot,
                           const CompositeOperatorInstance& comp)
          : _name(n)
          , _index(i)
          , _annot(&annot)
          , _primitive(NULL)
          , _composite(&comp)
        {}
        ParallelRegionInfo(const std::string& n,
                           uint32_t i,
                           const SPLParallelAnnotation& annot,
                           const PrimitiveOperatorInstance& prim)
          : _name(n)
          , _index(i)
          , _annot(&annot)
          , _primitive(&prim)
          , _composite(NULL)
        {}

        std::string _name;
        uint32_t _index;
        const SPLParallelAnnotation* _annot;
        const PrimitiveOperatorInstance* _primitive;
        const CompositeOperatorInstance* _composite;
    };

    std::tr1::unordered_map<uint32_t, uint32_t> _uriMap;
    std::vector<uint32_t> _uriOrder;
    std::tr1::unordered_map<uint32_t, uint32_t> _hostpoolMap;
    std::vector<uint32_t> _hostpoolReverseMap;
    std::tr1::unordered_map<const TupleTyp*, uint32_t> _tupleMap;
    std::tr1::unordered_map<LiteralReplacer::ArgInfo, uint32_t> _submissionTimeValueMap;
    std::tr1::unordered_map<std::string, std::pair<std::string, const SourceLocation*> >
      _defaultSubmissionTimeValues;
    std::tr1::unordered_map<Operator::Metric, uint32_t> _metricMap;
    std::vector<Operator::Metric> _metricOrder;
    std::tr1::unordered_map<std::string, uint32_t> _operDefinitionMap;
    std::vector<const PrimitiveOperatorInstance*> _operDefinitionOrder;
    std::tr1::unordered_map<std::string, uint32_t> _parallelRegionMap;
    std::vector<ParallelRegionInfo> _parallelRegions;

    bool _NBO;
    uint32_t _parallelRegionIndex;

    CompilationState* _compilationState;

    typedef xmlns::prod::streams::runtime::utils::mr::tracingLevelType TracingLevelType;

    uint32_t rememberSourceIndex(uint32_t sourceIndex);
    uint32_t rememberToolkitIndex(uint32_t toolkitIndex);
    uint32_t getHostpoolIndex(uint32_t hpIndex);
    void populateSourceUris(xmlns::prod::streams::application::v4200::urisType& sourceUris);
    void populatePrimitiveOperatorInputPorts(
      const PrimitiveOperatorInstance& oper,
      xmlns::prod::streams::application::v4200::primitiveOperInstanceBaseType& primitive);
    void populatePrimitiveOperatorOutputPorts(
      const PrimitiveOperatorInstance& oper,
      xmlns::prod::streams::application::v4200::primitiveOperInstanceBaseType& primitive);
    void populatePrimitiveOperatorInstanceBase(
      const PrimitiveOperatorInstance& oper,
      xmlns::prod::streams::application::v4200::primitiveOperInstanceBaseType& primitive);
    void populatePrimitiveImportOperatorInstance(
      const PrimitiveOperatorInstance& oper,
      xmlns::prod::streams::application::v4200::primitiveImportOperInstanceType& primitive);
    void populatePrimitiveExportOperatorInstance(
      const PrimitiveOperatorInstance& oper,
      xmlns::prod::streams::application::v4200::primitiveExportOperInstanceType& primitive);
    void populatePrimitiveOperatorInstance(
      const PrimitiveOperatorInstance& oper,
      xmlns::prod::streams::application::v4200::primitiveOperInstanceType& primitive);
    void populateOperatorInstanceResources(
      const OperatorGraphNode& operNode,
      xmlns::prod::streams::application::v4200::resourcesType& resources);
    void populateCompositeOperatorInstance(
      CompositeOperatorInstance const& compOperInstanceIR,
      xmlns::prod::streams::application::v4200::compositeOperInstanceType& instance);
    void populateCompositeOperatorInstances(
      xmlns::prod::streams::application::v4200::compositeOperInstancesType& instances);
    void populateCompositeOperatorDefinitions(
      xmlns::prod::streams::application::v4200::compositeOperDefinitionsType& definitions);
    void populateComposites(xmlns::prod::streams::application::v4200::compositesType& composites);
    TracingLevelType getTraceLevel(Config::TracingKind lk);
    uint32_t getTupleIndex(const TupleTyp& tuple);
    std::string getEncoding();
    RootTyp const& getPropType(RootTyp const& base);
    void getPropValue(xmlns::prod::streams::application::v4200::streamPropertyType& prop,
                      const ExpressionValue& val,
                      bool stripQuotes);
    void populateTupleTypes(xmlns::prod::streams::application::v4200::tupleTypesType& tupleTypes);
    void populateHostpools(xmlns::prod::streams::application::v4200::hostpoolsType& hostpools);
    void populateLibraries(xmlns::prod::streams::application::v4200::librariesType& libraries);
    void populateToolkits(xmlns::prod::streams::application::v4200::toolkitsType& toolkits);
    void populateMetrics(xmlns::prod::streams::application::v4200::customMetricsType& toolkits);
    void populateOperDefinitions(
      xmlns::prod::streams::application::v4200::primitiveOperDefinitionsType& defs);
    void populateApplication(const std::string& appname,
                             const std::string& adlRootName,
                             const std::string& appscope,
                             xmlns::prod::streams::application::v4200::applicationType& app);
    void populateParallelRegions(
      xmlns::prod::streams::application::v4200::parallelRegionsType& parallelRegions);
    void populateApplicationSet(
      const CompositeDefinition& main,
      xmlns::prod::streams::application::v4200::applicationSetType& appset,
      const std::string& adlRootName);
    int32_t getSubmissionTimeIndex(const Operator::Instance::RuntimeConstant& rc);
    void populateSubmissionValuesTable(
      xmlns::prod::streams::application::v4200::submissionTimeValuesType& submissionArgs);
    uint32_t getOperatorInstanceIndex(const std::string& n, bool isMainComposite);
    void findParallelRegions(const CompositeOperatorInstance& inst);
    void findParallelRegions();
    void populateLogicClauseAnnotation(
      const std::string key,
      bool value,
      xmlns::prod::streams::application::v4200::splAnnotationType& an);
    void populateOperatorInstancePlacement(
      const OperatorGraphNode& operNode,
      xmlns::prod::streams::application::v4200::primitiveOperInstanceType& primitive);
    bool isMultiThreadedTupleGenerator(Operator::OperatorModel const& om,
                                       OperatorGraphNode const& gn);
    std::auto_ptr<xmlns::prod::streams::application::v4200::splAnnotationType> createSPLAnnotation(
      const SPLSimpleAnnotation& annotation);
};
};

#endif // SPL_CODE_GEN_ADL_GENERATOR_H
