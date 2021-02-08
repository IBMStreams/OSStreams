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

#ifndef LOGICAL_TO_PHYSICAL_H
#define LOGICAL_TO_PHYSICAL_H

#include <SAM/SAMNamespaceDefs.h>
#include <SAM/SAMTypes.h>
#include <SAM/applicationModel.h>
#include <SAM/physicalApplicationModel.h>
#include <UTILS/HashMapHelpers.h>

#include <limits>

SAM_NAMESPACE_BEGIN

namespace LogicalToPhysical {

namespace Transformer {

class Model;
class LogicalModel;
class PhysicalModel;
class ModelCompositeInputPort;
class ModelCompositeOutputPort;
class ModelCompositeOperator;
class ModelPrimitiveOperatorBase;
class ModelPrimitiveOperator;
class PhysicalConnection;
class ModelConnection;
class ModelOperator;
class PhysicalOperator;
class PhysicalPE;
class PhysicalPEPort;
class PhysicalPEInputPort;
class PhysicalPEOutputPort;
class ModelPrimitiveInputPort;
class ModelPrimitiveOutputPort;
class PhysicalOperatorPort;
class PhysicalOperatorInputPort;
class PhysicalOperatorOutputPort;
class StaticConnection;
class Viewable;
class ThreadedPort;
class Hostpool;
class PoolLocation;
class Resources;
class ImportedStream;
class PhysicalImportedStream;
class ModelPrimitiveImportOutputPort;
class ModelPrimitiveExportInputPort;
class ModelPrimitiveExportOperator;
class ModelPrimitiveImportOperator;
class ExportedStream;
class PhysicalExportedStream;
class ParallelRegion;
class ParallelRegions;
class PhysicalParallelChannel;
class Splitter;
class ModelSplitterOperator;
class ModelMergerOperator;
class ModelDumper;
class PhysicalSplitter;
class CCRegion;
class CCInfo;
struct MergedPair;

/// This class will need to change to support nested parallelism
class SplitterMapEntry
{
  public:
    SplitterMapEntry(uint64_t operIndex_, uint64_t portIndex_, uint64_t splitterIndex_)
      : _operIndex(operIndex_)
      , _portIndex(portIndex_)
      , _splitterIndex(splitterIndex_)
    {}
    uint64_t operIndex() const { return _operIndex; }
    uint64_t portIndex() const { return _portIndex; }
    uint64_t splitterIndex() const { return _splitterIndex; }

  private:
    uint64_t _operIndex;
    uint64_t _portIndex;
    uint64_t _splitterIndex;
};

// Enums
enum PortKind
{
    Input,
    Output
};
enum HostpoolMembership
{
    Shared,
    Exclusive
};

// Types
typedef std::vector<ModelCompositeInputPort*> CompositeInputPortsType;
typedef std::vector<ModelCompositeOutputPort*> CompositeOutputPortsType;
typedef std::vector<ModelConnection*> ModelConnectionsType;
typedef std::vector<PhysicalConnection*> PhysicalConnectionsType;
typedef std::vector<ModelCompositeOperator*> CompositeOperatorsType;
typedef std::vector<ModelPrimitiveOperatorBase*> PrimitiveOperatorsType;
typedef std::vector<PhysicalOperator*> PhysicalOperatorsType;
typedef std::vector<PhysicalPEInputPort*> PhysicalPEInputPortsType;
typedef std::vector<PhysicalPEOutputPort*> PhysicalPEOutputPortsType;
typedef std::vector<ModelPrimitiveInputPort*> PrimitiveInputPortsType;
typedef std::vector<ModelPrimitiveExportInputPort*> PrimitiveExportInputPortsType;
typedef std::vector<ModelPrimitiveOutputPort*> PrimitiveOutputPortsType;
typedef std::vector<ModelPrimitiveImportOutputPort*> PrimitiveImportOutputPortsType;
typedef std::vector<PhysicalOperatorInputPort*> PhysicalOperatorInputPortsType;
typedef std::vector<PhysicalOperatorOutputPort*> PhysicalOperatorOutputPortsType;
typedef std::vector<StaticConnection*> StaticConnectionsType;
typedef std::vector<const ImportedStream*> ImportedStreamsType;
typedef std::vector<const PhysicalImportedStream*> PhysicalImportedStreamsType;
typedef std::vector<const ExportedStream*> ExportedStreamsType;
typedef std::vector<const PhysicalExportedStream*> PhysicalExportedStreamsType;
typedef std::vector<PhysicalParallelChannel*> PhysicalParallelChannelsType;
typedef std::vector<Splitter*> SplittersType;
typedef std::vector<PhysicalSplitter*> PhysicalSplittersType;
typedef std::vector<SplitterMapEntry> SplitterMappings;
typedef std::vector<std::string> HostNamesType;

typedef std::tr1::unordered_map<uint64_t, uint64_t> OperatorIndexMap;
typedef std::tr1::unordered_set<uint64_t> OperatorIndexSet;
typedef std::tr1::unordered_map<uint64_t, ModelOperator*> ModelOperatorMap;
typedef std::tr1::unordered_map<uint64_t, const ModelPrimitiveOperator*> PrimitiveOperatorMap;
typedef std::tr1::unordered_map<uint64_t, PhysicalOperator*> PhysicalOperatorMap;
typedef std::tr1::unordered_map<uint64_t, PhysicalPE*> PEMap;
typedef std::tr1::unordered_map<uint64_t, const ParallelRegion*> ParallelRegionMap;
typedef std::tr1::unordered_map<uint64_t, const ParallelRegion*> ParallelOperatorMap;
typedef std::tr1::unordered_map<std::string, const ImportedStream*> ImportedStreamsMap;
typedef std::tr1::unordered_map<std::string, const ExportedStream*> ExportedStreamsMap;
typedef std::tr1::unordered_map<uint64_t, const ADL::parallelRegionSplitterType*>
  ParallelRegionSplitterMap;
typedef std::tr1::unordered_map<uint64_t, SplitterMappings*> SplitterMap;
typedef std::tr1::unordered_map<uint64_t, Hostpool*> HostpoolMap;
typedef std::tr1::unordered_map<std::pair<uint64_t, uint64_t>, CCRegion*> CCRegionMap;
typedef std::tr1::unordered_map<MergedPair, CCRegion*> MergeMap;
typedef std::pair<uint64_t, uint64_t> ParallelRegionAndChannel;
typedef std::tr1::unordered_map<ParallelRegionAndChannel, Hostpool*> ReplicaHostpoolMap;
typedef std::tr1::unordered_set<std::string> TagSet;
typedef std::tr1::unordered_set<CCInfo*> CCInfoSet;
typedef std::tr1::unordered_set<ModelPrimitiveOperatorBase*> ModelPrimitiveOperatorsSet;

struct MergedPair
{
    MergedPair(const CCRegion& regionA, const CCRegion& regionB)
      : _regionA(&regionA)
      , _regionB(&regionB)
    {}
    const CCRegion* _regionA;
    const CCRegion* _regionB;
};

class CCRegion
{
  public:
    CCRegion(const ADL::compositeOperInstanceType::splAnnotation_type& annot);
    CCRegion(uint64_t index_, const CCRegion& other)
      : _annot(other._annot)
      , _logicalIndex(other._logicalIndex)
      , _index(index_)
      , _isOperatorDriven(other._isOperatorDriven)
      , _drainTimeout(other._drainTimeout)
      , _resetTimeout(other._resetTimeout)
      , _mergedWith(NULL)
    {}

    void addRegionEntry(ModelPrimitiveOperatorBase& oper, Model& model);

    uint64_t logicalIndex() const { return _logicalIndex; }
    uint64_t index() const { return _index; }
    bool isOperatorDriven() const { return _isOperatorDriven; }
    double drainTimeout() const { return _drainTimeout; }
    double resetTimeout() const { return _resetTimeout; }

    ModelPrimitiveOperatorsSet& operators() { return _operators; }
    const ModelPrimitiveOperatorsSet& operators() const { return _operators; }

    void merge(CCRegion& other);
    void setMergedWith(CCRegion& region);
    CCRegion& mergedWith() const;
    bool wasMerged() const { return NULL != _mergedWith; }
    bool operator==(const CCRegion& other) const
    {
        return &this->mergedRegion() == &other.mergedRegion();
    }
    CCRegion& mergedRegion()
    {
        if (NULL != _mergedWith) {
            return _mergedWith->mergedRegion();
        }
        return *this;
    }
    const CCRegion& mergedRegion() const
    {
        if (NULL != _mergedWith) {
            return _mergedWith->mergedRegion();
        }
        return *this;
    }

    const ADL::compositeOperInstanceType::splAnnotation_type& annotation() const { return *_annot; }

  private:
    const ADL::compositeOperInstanceType::splAnnotation_type* _annot;
    uint64_t _logicalIndex;
    uint64_t _index;
    bool _isOperatorDriven;
    double _drainTimeout;
    double _resetTimeout;
    CCRegion* _mergedWith;
    ModelPrimitiveOperatorsSet _operators;
};

class CCInfo
{
  public:
    CCInfo(const ModelPrimitiveOperatorBase& oper)
      : _oper(oper)
      , _index(0)
      , _isStartOfRegion(false)
      , _isEndOfRegion(false)
      , _isOblivious(true)
    {}
    CCInfo(const CCInfo& other, const ModelPrimitiveOperatorBase& oper)
      : _oper(oper)
      , _index(other._index)
      , _isStartOfRegion(other._isStartOfRegion)
      , _isEndOfRegion(other._isEndOfRegion)
      , _isOblivious(other._isOblivious)
      , _keyValues(other._keyValues)
    {}

    void setRegionEntry(const ADL::primitiveOperInstanceType::splAnnotation_type& annot);
    void setOblivious() { _isOblivious = true; }
    uint32_t addedToRegion(CCRegion& ccRegion_)
    {
        _regions.push_back(&ccRegion_);
        return _regions.size();
    }

    uint64_t index() const { return _index; }

    bool isStartCutRegion() const { return _isStartOfRegion; }
    bool isEndCutRegion() const { return _isEndOfRegion; }
    bool isOblivious() const { return _isOblivious; }

    const std::vector<CCRegion*>& regions() const { return _regions; }
    void setMergedRegion(CCRegion& ccRegion_)
    {
        _regions.clear();
        _regions.push_back(&ccRegion_);
    }

    void populatePADL(PADL::physOperInstanceType::splAnnotation_sequence& annotations) const;

    ModelDumper& print(ModelDumper& dumper) const;

  private:
    const ModelPrimitiveOperatorBase& _oper;
    uint64_t _index;
    bool _isStartOfRegion;
    bool _isEndOfRegion;
    bool _isOblivious;
    std::vector<CCRegion*> _regions;
    std::vector<const ADL::splAnnotationType::value_type*> _keyValues;
};

class ParallelTransformState
{
  public:
    OperatorIndexMap& operatorIndexMap() { return _operatorIndexMap; }
    const OperatorIndexMap& operatorIndexMap() const { return _operatorIndexMap; }
    uint64_t mappedOperatorIndex(uint64_t operIndex) const;

  private:
    OperatorIndexMap _operatorIndexMap;
};

class PhysicalParallelChannel
{
  public:
    PhysicalParallelChannel(uint64_t index_, const ModelConnectionsType& connections);
    ~PhysicalParallelChannel();
    uint64_t index() const { return _index; }
    void setPEPort(const PhysicalPEPort& pePort_) { _pePort = &pePort_; }
    const PhysicalPEPort& pePort() const;
    uint64_t pePortIndex() const;
    bool needsPEPort(const Model& model) const;
    void buildPhysicalConnections(const PhysicalOperator& owningOper,
                                  const PhysicalOperatorOutputPort& owningPort,
                                  const Model& model);
    void buildStaticConnections(const Model& model, StaticConnectionsType& staticConnections) const;
    void populatePADL(ADL::splitterType& splitter, const Model& model) const;

    ModelDumper& print(ModelDumper& dumper) const;

  private:
    uint64_t _index;
    const PhysicalPEPort* _pePort;
    const ModelConnectionsType& _modelConnections;
    PhysicalConnectionsType _connections;
};

class PhysicalSplitter
{
  public:
    PhysicalSplitter(uint64_t index_, uint64_t iPortIndex, const ParallelRegion& parallelRegion_);
    ~PhysicalSplitter();

    uint64_t index() const { return _index; }

    const PhysicalParallelChannelsType& parallelChannels() const { return _parallelChannels; }
    PhysicalParallelChannelsType& parallelChannels() { return _parallelChannels; }

    bool needsPEPorts(const Model& model) const;
    uint64_t pePortIndex(uint64_t channelIndex) const;
    void buildPhysicalConnections(const PhysicalOperator& owningOper,
                                  const PhysicalOperatorOutputPort& owningPort,
                                  const Model& model);
    void buildStaticConnections(uint64_t channelIndex,
                                const Model& model,
                                StaticConnectionsType& staticConnections) const;
    void populatePADL(PADL::physOperInstanceOutputPortType& oPort,
                      bool isInParallelRegion,
                      const Model& model) const;

    ModelDumper& print(ModelDumper& dumper) const;

  private:
    uint64_t _index;
    uint64_t _iPortIndex;
    const ParallelRegion& _parallelRegion;
    PhysicalParallelChannelsType _parallelChannels;
};

class ModelConnection
{
  public:
    ModelConnection(const ADL::operInstancePortConnectionType& conn);
    ModelConnection(const ModelConnection& other)
      : _operIndex(other._operIndex)
      , _portIndex(other._portIndex)
      , _portKind(other._portKind)
    {}
    ModelConnection(uint64_t operIndex_, uint64_t portIndex_, PortKind portKind_)
      : _operIndex(operIndex_)
      , _portIndex(portIndex_)
      , _portKind(portKind_)
    {}

    void getPhysicalConnections(uint64_t fromPhysicalOperIndex,
                                uint64_t fromPortIndex,
                                PortKind fromPortKind,
                                const Model& model,
                                PhysicalConnectionsType& physConnections) const;
    void getImportedStreams(PhysicalImportedStreamsType& importedStreams, const Model& model) const;
    void getExportedStreams(PhysicalExportedStreamsType& exportedStreams, const Model& model) const;
    void getSplitters(const PhysicalOperator& owningOper,
                      const PhysicalOperatorOutputPort& owningPort,
                      PhysicalSplittersType& _splitters,
                      Model& model) const;

    // Called on a replicated port connection to add the reverse connection to the operator it points to
    void addConnection(ModelConnection& connection, Model& model) const;

    void fixReverseConnection(uint64_t oldToOperIndex,
                              uint64_t oldToPortIndex,
                              uint64_t newToOperIndex,
                              uint64_t newToPortIndex,
                              PortKind toPortKind,
                              Model& model);

    uint64_t operIndex() const { return _operIndex; }
    void setOperIndex(uint64_t operIndex_) { _operIndex = operIndex_; }

    uint64_t portIndex() const { return _portIndex; }
    void setPortIndex(uint64_t portIndex_) { _portIndex = portIndex_; }

    PortKind portKind() const { return _portKind; }
    void setPortKind(PortKind portKind_) { _portKind = portKind_; }
    void fixOperatorIndex(const ParallelTransformState& state)
    {
        _operIndex = state.mappedOperatorIndex(_operIndex);
    }
    void computeCCReachabilityGraph(CCRegion& ccRegion,
                                    ModelPrimitiveOperatorsSet& operatorsSeen,
                                    Model& model) const;

    ModelDumper& print(ModelDumper& dumper) const;

  private:
    uint64_t _operIndex;
    uint64_t _portIndex;
    PortKind _portKind;
};

class ModelOperator
{
  public:
    ModelOperator(const std::string& name_,
                  uint64_t index_,
                  const std::string& description_,
                  ModelCompositeOperator* owningComposite_,
                  bool isReplica);

    // Copy ctor used to replicate an operator
    ModelOperator(const ModelOperator& other,
                  uint64_t index_,
                  uint64_t channelIndex_,
                  ModelCompositeOperator& owningComp);
    virtual ~ModelOperator() {}

    /// Is this operator a replica
    /// @return Retuns true if this operator was replicated, otherwise false
    virtual bool isReplica() const { return _isReplica; }

    /// Is the concrete type of this operator primitive
    /// @ return Returns true if this is a primitive operator, otherwise false
    virtual bool isPrimitive() const { return false; }

    /// Is the concrete type of this operator composite
    /// @ return Returns true if this is a composite operator, otherwise false
    virtual bool isComposite() const { return false; }

    /// Is the concrete type of this operator primitive Import operator
    /// @ return Returns true if this is a primtive Import operator, otherwise false
    virtual bool isImport() const { return false; }

    /// Is the concrete type of this operator primitive Export operator
    /// @return Returns true if this is a primtive Export operator, otherwise false
    virtual bool isExport() const { return false; }

    /// Is the concrete type of this operator a primitive Splitter operator
    /// @return Returns true if this is a primtive Splitter operator, otherwise false
    virtual bool isSplitter() const { return false; }

    /// Is the concrete type of this operator a primitive Merger operator
    /// @return Returns true if this is a primtive Merger operator, otherwise false
    virtual bool isMerger() const { return false; }

    /// Get the physical index of this modelOperator.  Note that this should only
    /// be called for a ModelPrimitiveOperator and will throw if otherwise.
    virtual uint64_t physicalIndex() const;

    /// Get the fully-qualified logical name of this operator
    /// @return Returns the fully-qualified operator's logical name
    std::string fullyQualifiedLogicalName() const;

    /// Get the fully-qualified name of this operator
    /// @return Returns the fully-qualified operator's name
    std::string fullyQualifiedName() const;

    /// Get the fully-qualified logical name of the owning composite.
    /// Returns an empty string if this is the main composite
    /// @return Returns the fully-qualified logical name, or the empty string
    std::string fullyQualifiedOwningLogicalCompositeName() const;

    /// Get the fully-qualified name of the owning composite.
    /// Returns an empty string if this is the main composite
    /// @return Returns the fully-qualified name, or the empty string
    std::string fullyQualifiedOwningCompositeName() const;

    /// Is this the main composite?
    /// @return Returns true if this is the main composite, otherwise false
    bool isMainComposite() const { return NULL == _owningComposite; }

    /// Get the index for this operator
    /// @return Returns this operator's index
    uint64_t index() const { return _index; }

    /// Mark this operator as a parallel regions and set it's channel
    void setParallelRegionInfo(const ParallelRegion& region, uint64_t channelIndex)
    {
        _parallelRegion = &region;
        _channelIndex = channelIndex;
    }

    /// Is this operator contained within a parallel region, or
    /// does this operator represent a parallel region?
    /// @return Returns true if this is, or is contained in, a prallel region, otherwise false
    bool isInParallelRegion() const;

    /// Is this operator designated as a parallel region?
    /// @return Returns true if this operator is designated as a parallel region
    bool isParallelRegion() const { return NULL != _parallelRegion; }

    /// Called on an operator that is within a parallel region to get the closest containing
    /// parallel region
    /// @return Returns the closest region.  Throws ???? if this operator is not within a parallel region
    const ParallelRegion& containingParallelRegion() const;

    /// Called to get the channel index of the closest containing parallel channel
    /// @return Returns the closest index.  Throws ???? if this operator is not within a parallel region
    uint64_t containingParallelChannelIndex() const;

    /// Called on an operator to create Splitters for all operators feeding this operator's input ports
    /// and Mergers for all operators consuming output from this operator
    virtual void createSplittersAndMergers(Model& model) = 0;

    /// Add this connection to the receiving operator's input port connections
    virtual void addInputPortConnection(uint64_t portIndex, ModelConnection& conn) = 0;

    /// Add this connection to the receiving operator's output port connections
    virtual void addOutputPortConnection(uint64_t portIndex, ModelConnection& connection);

    const ParallelRegion& parallelRegion() const;

    virtual ModelDumper& print(ModelDumper& dumper) const { return dumper; }

    const ModelCompositeOperator* owningComposite() const;
    ModelCompositeOperator* owningComposite();

    virtual void fixReverseConnection(uint64_t fromPortIndex,
                                      PortKind fromPortKind,
                                      uint64_t oldToOperIndex,
                                      uint64_t oldToPortIndex,
                                      uint64_t newToOperIndex,
                                      uint64_t newToPortIndex,
                                      PortKind toPortKind);

  private:
    /// Get the logical name for this operator
    /// @return Returns the operator's logical name
    const std::string& logicalName() const { return _name; }

    /// Get the name, with any parallel channel subscripting
    std::string name() const;

    std::string _name;
    uint64_t _index;
    uint64_t _channelIndex;
    ModelCompositeOperator* _owningComposite;
    bool _isReplica;
    const ParallelRegion* _parallelRegion;
};

/// The base class for all composite and primitive operator ports
class ModelPort
{
  public:
    uint64_t index() const { return _index; }
    virtual ~ModelPort() {}
    virtual ModelDumper& print(ModelDumper& dumper) const { return dumper; }

  protected:
    ModelPort(uint64_t index_)
      : _index(index_)
    {}
    ModelPort(const ModelPort& other)
      : _index(other._index)
    {}

  private:
    uint64_t _index;
};

class ModelPrimitivePort : public ModelPort
{
  public:
    ModelPrimitivePort(uint64_t index_,
                       const std::string& name_,
                       const std::string& transport_,
                       const std::string& encoding_,
                       bool isMutable_,
                       uint64_t tupleTypeIndex_,
                       const ADL::operInstancePortConnectionsType* connections_,
                       ModelPrimitiveOperatorBase& owningPrimitiveOp,
                       Model& model);
    ModelPrimitivePort(uint64_t index_, ModelPrimitiveOperatorBase& owningPrimitiveOp);
    ModelPrimitivePort(const ModelPrimitivePort& other,
                       ModelPrimitiveOperatorBase& owningPrimitiveOp);
    virtual ~ModelPrimitivePort();
    const ModelConnectionsType& connections() const { return _connections; }
    ModelConnectionsType& connections() { return _connections; }

    const std::string& name() const { return _name; }
    const std::string& transport() const { return _transport; }
    const std::string& encoding() const { return _encoding; }
    bool isMutable() const { return _isMutable; }
    uint64_t tupleTypeIndex() const { return _tupleTypeIndex; }

    const ModelPrimitiveOperatorBase& owningOperator() const { return _owningPrimitiveOp; }
    ModelPrimitiveOperatorBase& owningOperator() { return _owningPrimitiveOp; }

    void addConnection(ModelConnection& conn) { _connections.push_back(&conn); }
    virtual ModelDumper& print(ModelDumper& dumper) const;
    void fixConnections(const ParallelTransformState& state);
    void fixConnection(uint64_t oldToOperIndex,
                       uint64_t oldToPortIndex,
                       uint64_t newToOperIndex,
                       uint64_t newToPortIndex,
                       PortKind newToPortKind);
    PhysicalParallelChannel& createParallelChannel(const PhysicalOperator& owningOper,
                                                   const PhysicalOperatorOutputPort& owningPort,
                                                   const Model& model) const;
    void getPhysicalConnections(uint64_t fromPhysicalOperIndex,
                                uint64_t fromPortIndex,
                                PortKind fromPortKind,
                                const Model& model,
                                PhysicalConnectionsType& physConnections) const;
    void getSplitters(const PhysicalOperator& owningOper,
                      const PhysicalOperatorOutputPort& owningPort,
                      PhysicalSplittersType& splitters,
                      Model& model) const;

    void computeCCReachabilityGraph(CCRegion& ccRegion,
                                    ModelPrimitiveOperatorsSet& operatorsSeen,
                                    Model& model) const;

  private:
    std::string _name;
    std::string _transport;
    std::string _encoding;
    bool _isMutable;
    uint64_t _tupleTypeIndex;
    ModelPrimitiveOperatorBase& _owningPrimitiveOp;
    ModelConnectionsType _connections;
};

class ModelPrimitiveInputPort : public ModelPrimitivePort
{
  public:
    ModelPrimitiveInputPort(uint64_t index_,
                            const ADL::operInstancePortConnectionsType* connections_,
                            const PhysicalOperatorInputPort& piPort,
                            ModelPrimitiveOperator& owningPrimitiveOp,
                            Model& model);
    ModelPrimitiveInputPort(const ModelPrimitiveInputPort& other,
                            ModelPrimitiveOperator& owningPrimitiveOp);
    virtual ~ModelPrimitiveInputPort();

    bool hasThreadedPort() const { return NULL != _threadedPort; }
    const ThreadedPort& threadedPort() const { return *_threadedPort; }
    void createSplitters(const ParallelRegion& parallelRegion, Model& model);
    void addReverseConnections(uint64_t channelIndex_, Model& model) const;
    bool isControl() const { return _isControl; }

  private:
    const ThreadedPort* _threadedPort;
    bool _isControl;
};

class ModelPrimitiveOutputPort : public ModelPrimitivePort
{
  public:
    ModelPrimitiveOutputPort(uint64_t index_,
                             const ADL::operInstancePortConnectionsType* connections_,
                             const PhysicalOperatorOutputPort& poPort,
                             ModelPrimitiveOperator& owningPrimitiveOp,
                             Model& model);
    ModelPrimitiveOutputPort(const ModelPrimitiveOutputPort& other,
                             ModelPrimitiveOperator& owningPrimitiveOp);
    virtual ~ModelPrimitiveOutputPort();

    const std::string& logicalStreamName() const { return _streamName; }
    std::string streamName() const;
    bool viewableSet() const { return NULL != _viewable; }
    bool viewable() const;
    bool singleThreadedOnOutput() const { return _singleThreadedOnOutput; }
    void addReverseConnections(uint64_t channelIndex_, Model& model) const;
    virtual void addConnection(ModelConnection& connection);
    void createMergers(const ParallelRegion& parallelRegion, Model& model);

  private:
    std::string _streamName;
    const Viewable* _viewable;
    bool _singleThreadedOnOutput;
};

class ModelPrimitiveImportOutputPort : public ModelPrimitivePort
{
  public:
    ModelPrimitiveImportOutputPort(uint64_t index_,
                                   const ImportedStream* importedStream_,
                                   const ADL::operInstancePortConnectionsType* connections_,
                                   ModelPrimitiveImportOperator& owningPrimitiveOp,
                                   Model& model);
    ModelPrimitiveImportOutputPort(const ModelPrimitiveImportOutputPort& other,
                                   ModelPrimitiveImportOperator& owningPrimitiveOp);

    void importedStream(PhysicalImportedStreamsType& importedStreams) const;
    void addReverseConnections(uint64_t channelIndex_, Model& model) const;

  private:
    const ImportedStream* _importedStream;
};

class ModelPrimitiveExportInputPort : public ModelPrimitivePort
{
  public:
    ModelPrimitiveExportInputPort(uint64_t index_,
                                  const ExportedStream& exportedStream_,
                                  const ADL::operInstancePortConnectionsType* connections_,
                                  ModelPrimitiveExportOperator& owningPrimitiveOp,
                                  Model& model);
    ModelPrimitiveExportInputPort(const ModelPrimitiveExportInputPort& other,
                                  ModelPrimitiveExportOperator& owningPrimitiveOp);

    void exportedStream(PhysicalExportedStreamsType& exportedStreams) const;
    void addReverseConnections(uint64_t channelIndex_) const;

  private:
    const ExportedStream& _exportedStream;
};

class ModelPrimitiveOperatorBase : public ModelOperator
{
  public:
    ModelPrimitiveOperatorBase(
      const std::string& name_,
      uint64_t index_,
      const std::string& description,
      const std::string& kind_,
      uint64_t toolkitIndex_,
      const ADL::primitiveOperInstanceType::splAnnotation_sequence* splAnnotations,
      ModelCompositeOperator& owningComp,
      bool isRep,
      Model& model);
    ModelPrimitiveOperatorBase(const ModelPrimitiveOperatorBase& other,
                               uint64_t index_,
                               uint64_t channelIndex_,
                               ModelCompositeOperator& owningComp,
                               Model& model);

    virtual void createSplittersAndMergers(Model& model) = 0;
    virtual void addReverseConnections(uint64_t channelIndex_, Model& model) = 0;
    virtual void addInputPortConnection(uint64_t portIndex, ModelConnection& conn) = 0;

    virtual ModelPrimitiveOperatorBase& replicate(uint64_t channelIndex_,
                                                  ModelCompositeOperator& owningComp,
                                                  Model& model) const = 0;
    const std::string& kind() const { return _kind; }

    // Called on a replicated operator to fix it's input and output port connections
    virtual void fixConnections(const ParallelTransformState& state) = 0;

    virtual void computeCCReachabilityGraph(CCRegion& ccRegion,
                                            ModelPrimitiveOperatorsSet& operatorsSeen,
                                            Model& model) = 0;

    const ADL::primitiveOperInstanceType::splAnnotation_sequence& annotations() const
    {
        return _splAnnotations;
    }

    const CCInfo& ccInfo() const { return _ccInfo; }
    CCInfo& ccInfo() { return _ccInfo; }

    uint64_t toolkitIndex() const { return _toolkitIndex; }

  private:
    std::string _kind;
    uint64_t _toolkitIndex;
    ADL::primitiveOperInstanceType::splAnnotation_sequence _splAnnotations;
    CCInfo _ccInfo;
};

class ModelSplitterOperator : public ModelPrimitiveOperatorBase
{
  public:
    ModelSplitterOperator(uint64_t injectingIPortIndex_,
                          ModelCompositeOperator& owningComposite_,
                          const ParallelRegion& parallelRegion_,
                          Model& model);
    virtual ~ModelSplitterOperator();
    virtual bool isSplitter() const { return true; }
    virtual void createSplittersAndMergers(Model& model);
    virtual void addReverseConnections(uint64_t channelIndex_, Model& model);
    virtual void addInputPortConnection(uint64_t portIndex, ModelConnection& conn);
    void addOutputPortConnection(uint64_t portIndex, ModelConnection& connection);
    virtual ModelPrimitiveOperatorBase& replicate(uint64_t channelIndex_,
                                                  ModelCompositeOperator& owningComp,
                                                  Model& model) const;
    virtual void fixConnections(const ParallelTransformState& state);
    virtual ModelDumper& print(ModelDumper& dumper) const;
    virtual void fixReverseConnection(uint64_t fromPortIndex,
                                      PortKind fromPortKind,
                                      uint64_t oldToOperIndex,
                                      uint64_t oldToPortIndex,
                                      uint64_t newToOperIndex,
                                      uint64_t newToPortIndex,
                                      PortKind toPortKind);
    virtual void computeCCReachabilityGraph(CCRegion& ccRegion,
                                            ModelPrimitiveOperatorsSet& operatorsSeen,
                                            Model& model);

    // Create a physical splitter that represents this operator
    void createPhysicalSplitter(const PhysicalOperator& owningOper,
                                const PhysicalOperatorOutputPort& owningPort,
                                PhysicalSplittersType& _splitters,
                                Model& model) const;

  private:
    uint64_t _injectingIPortIndex;
    const ParallelRegion& _parallelRegion;
    ModelPrimitivePort _inputPort;
    std::vector<ModelPrimitivePort*> _outputPorts;
};

class ModelMergerOperator : public ModelPrimitiveOperatorBase
{
  public:
    ModelMergerOperator(ModelCompositeOperator& owningComposite_,
                        const ParallelRegion& parallelRegion_,
                        Model& model);
    virtual ~ModelMergerOperator();

    virtual bool isMerger() const { return true; }
    virtual void createSplittersAndMergers(Model& model);
    virtual void addReverseConnections(uint64_t channelIndex_, Model& model);
    virtual void addInputPortConnection(uint64_t portIndex, ModelConnection& conn);
    void addOutputPortConnection(uint64_t portIndex, ModelConnection& conn);
    virtual ModelPrimitiveOperatorBase& replicate(uint64_t channelIndex_,
                                                  ModelCompositeOperator& owningComp,
                                                  Model& model) const;
    virtual void fixConnections(const ParallelTransformState& state);
    void getOutgoingPhysicalConnections(uint64_t fromPhysicalOperIndex,
                                        uint64_t fromPortIndex,
                                        PortKind fromPortKind,
                                        const Model& model,
                                        PhysicalConnectionsType& physConnections) const;
    void getIncomingPhysicalConnections(uint64_t fromPhysicalOperIndex,
                                        uint64_t fromPortIndex,
                                        PortKind fromPortKind,
                                        const Model& model,
                                        PhysicalConnectionsType& physConnections) const;
    virtual void fixReverseConnection(uint64_t fromPortIndex,
                                      PortKind fromPortKind,
                                      uint64_t oldToOperIndex,
                                      uint64_t oldToPortIndex,
                                      uint64_t newToOperIndex,
                                      uint64_t newToPortIndex,
                                      PortKind toPortKind);
    void getOutputPortSplitters(const PhysicalOperator& owningOper,
                                const PhysicalOperatorOutputPort& owningPort,
                                PhysicalSplittersType& splitters,
                                Model& model) const;
    virtual void computeCCReachabilityGraph(CCRegion& ccRegion,
                                            ModelPrimitiveOperatorsSet& operatorsSeen,
                                            Model& model);
    virtual ModelDumper& print(ModelDumper& dumper) const;

  private:
    std::vector<ModelPrimitivePort*> _inputPorts;
    ModelPrimitivePort _outputPort;
};

class ModelPrimitiveOperator : public ModelPrimitiveOperatorBase
{
  public:
    ModelPrimitiveOperator(const ADL::primitiveOperInstanceType& po,
                           ModelCompositeOperator& owningComp,
                           Model& model);
    ModelPrimitiveOperator(const ModelPrimitiveOperator& other,
                           uint64_t channelIndex_,
                           ModelCompositeOperator& owningComp,
                           Model& model);
    ~ModelPrimitiveOperator();

    virtual bool isPrimitive() const { return true; }
    virtual uint64_t physicalIndex() const { return _physicalIndex; }
    uint64_t logicalIndex() const { return _logicalIndex; }

    const PrimitiveInputPortsType& inputPorts() const { return _inputPorts; }
    const PrimitiveOutputPortsType& outputPorts() const { return _outputPorts; }
    const PhysicalPE& originalPE() const;
    const PhysicalOperator& adlPhysicalOperator() const { return _adlPhysicalOperator; }
    virtual void createSplittersAndMergers(Model& model);
    virtual ModelPrimitiveOperatorBase& replicate(uint64_t channelIndex_,
                                                  ModelCompositeOperator& owningComp,
                                                  Model& model) const;
    virtual void addReverseConnections(uint64_t channelIndex_, Model& model);
    virtual void fixConnections(const ParallelTransformState& state);

    virtual void addInputPortConnection(uint64_t portIndex, ModelConnection& conn);
    virtual void addOutputPortConnection(uint64_t portIndex, ModelConnection& connection);

    virtual ModelDumper& print(ModelDumper& dumper) const;
    virtual void fixReverseConnection(uint64_t fromPortIndex,
                                      PortKind fromPortKind,
                                      uint64_t oldToOperIndex,
                                      uint64_t oldToPortIndex,
                                      uint64_t newToOperIndex,
                                      uint64_t newToPortIndex,
                                      PortKind toPortKind);
    virtual void computeCCReachabilityGraph(CCRegion& ccRegion,
                                            ModelPrimitiveOperatorsSet& operatorsSeen,
                                            Model& model);

  private:
    uint64_t _physicalIndex;
    uint64_t _logicalIndex;
    const PhysicalOperator& _adlPhysicalOperator;
    PrimitiveInputPortsType _inputPorts;
    PrimitiveOutputPortsType _outputPorts;
};

class ModelPrimitiveImportOperator : public ModelPrimitiveOperatorBase
{
  public:
    ModelPrimitiveImportOperator(const ADL::primitiveOperInstanceType& po,
                                 ModelCompositeOperator& owningComp,
                                 Model& model);
    ModelPrimitiveImportOperator(const ModelPrimitiveImportOperator& other,
                                 uint64_t channelIndex_,
                                 ModelCompositeOperator& owningComp,
                                 Model& model);

    virtual bool isImport() const { return true; }
    void importedStream(uint64_t portIndex, PhysicalImportedStreamsType& importedStreams) const;
    virtual void createSplittersAndMergers(Model& model);
    virtual ModelPrimitiveOperatorBase& replicate(uint64_t channelIndex_,
                                                  ModelCompositeOperator& owningComp,
                                                  Model& model) const;
    virtual void addReverseConnections(uint64_t channelIndex_, Model& model);
    virtual void addInputPortConnection(uint64_t portIndex, ModelConnection& conn);
    virtual void fixConnections(const ParallelTransformState& state);
    virtual void computeCCReachabilityGraph(CCRegion& ccRegion,
                                            ModelPrimitiveOperatorsSet& operatorsSeen,
                                            Model& model);

  private:
    PrimitiveImportOutputPortsType _outputPorts;
};

class ModelPrimitiveExportOperator : public ModelPrimitiveOperatorBase
{
  public:
    ModelPrimitiveExportOperator(const ADL::primitiveOperInstanceType& po,
                                 ModelCompositeOperator& owningComp,
                                 Model& model);
    ModelPrimitiveExportOperator(const ModelPrimitiveExportOperator& other,
                                 uint64_t channelIndex_,
                                 ModelCompositeOperator& owningComp,
                                 Model& model);

    virtual bool isExport() const { return true; }
    void exportedStream(uint64_t portIndex, PhysicalExportedStreamsType& exportedStreams) const;
    virtual void createSplittersAndMergers(Model& model);
    virtual ModelPrimitiveOperatorBase& replicate(uint64_t channelIndex_,
                                                  ModelCompositeOperator& owningComp,
                                                  Model& model) const;
    virtual void addReverseConnections(uint64_t channelIndex_, Model& model);
    virtual void addInputPortConnection(uint64_t portIndex, ModelConnection& conn);
    virtual void fixConnections(const ParallelTransformState& state);
    virtual void computeCCReachabilityGraph(CCRegion& ccRegion,
                                            ModelPrimitiveOperatorsSet& operatorsSeen,
                                            Model& model);

  private:
    PrimitiveExportInputPortsType _inputPorts;
};

/// The base class for composite input and output ports
class ModelCompositePort : public ModelPort
{
  public:
    virtual ~ModelCompositePort();

    void getIncomingPhysicalConnections(uint64_t fromPhysicalOperIndex,
                                        uint64_t fromPortIndex,
                                        PortKind fromPortKind,
                                        const Model& model,
                                        PhysicalConnectionsType& physConnections) const;
    void getOutgoingPhysicalConnections(uint64_t fromPhysicalOperIndex,
                                        uint64_t fromPortIndex,
                                        PortKind fromPortKind,
                                        const Model& model,
                                        PhysicalConnectionsType& physConnections) const;
    void getIncomingImportedStreams(PhysicalImportedStreamsType& importedStreams,
                                    const Model& model) const;
    void getOutgoingExportedStreams(PhysicalExportedStreamsType& exportedStreams,
                                    const Model& model) const;

    ModelDumper& print(ModelDumper& dumper) const;
    const ModelCompositeOperator& owningOperator() const { return _owningComposite; }
    ModelCompositeOperator& owningOperator() { return _owningComposite; }

    void fixOutgoingConnections(const ParallelTransformState& state);
    void fixIncomingConnections(const ParallelTransformState& state);

    void computeCCReachabilityGraph(CCRegion& ccRegion,
                                    ModelPrimitiveOperatorsSet& operatorsSeen,
                                    Model& model) const;

  protected:
    ModelCompositePort(const ADL::compositeOperInstancePortType& p,
                       ModelCompositeOperator& owningComposite);
    ModelCompositePort(const ModelCompositePort& other, ModelCompositeOperator& owningComposite);

    ModelCompositeOperator& _owningComposite;
    ModelConnectionsType _incomingConnections;
    ModelConnectionsType _outgoingConnections;
};

class ModelCompositeInputPort : public ModelCompositePort
{
  public:
    ModelCompositeInputPort(const ADL::compositeOperInstancePortType& p,
                            ModelCompositeOperator& owningComposite)
      : ModelCompositePort(p, owningComposite)
    {}
    ModelCompositeInputPort(const ModelCompositeInputPort& other,
                            ModelCompositeOperator& owningComposite)
      : ModelCompositePort(other, owningComposite)
    {}

    virtual ~ModelCompositeInputPort() {}
    void createSplitters(const ParallelRegion& parallelRegion, Model& model);
    void addIncomingReverseConnections(uint64_t channelIndex, Model& model);
    void fixIncomingReverseConnection(uint64_t oldToOperIndex,
                                      uint64_t oldToPortIndex,
                                      uint64_t newToOperIndex,
                                      uint64_t newToPortIndex,
                                      PortKind newPortKind);
    void fixOutgoingReverseConnection(uint64_t oldToOperIndex,
                                      uint64_t oldToPortIndex,
                                      uint64_t newToOperIndex,
                                      uint64_t newToPortIndex,
                                      PortKind newPortKind);
    void getOutgoingSplitters(const PhysicalOperator& owningOper,
                              const PhysicalOperatorOutputPort& owningPort,
                              PhysicalSplittersType& _splitters,
                              Model& model) const;
};

class ModelCompositeOutputPort : public ModelCompositePort
{
  public:
    ModelCompositeOutputPort(const ADL::compositeOperInstancePortType& p,
                             ModelCompositeOperator& owningComposite)
      : ModelCompositePort(p, owningComposite)
    {}
    ModelCompositeOutputPort(const ModelCompositeOutputPort& other,
                             ModelCompositeOperator& owningComposite)
      : ModelCompositePort(other, owningComposite)
    {}
    virtual ~ModelCompositeOutputPort() {}
    void addOutgoingReverseConnections(uint64_t channelIndex, Model& model) const;
    void addOutgoingConnection(uint64_t splitterIndex,
                               uint64_t channelIndex,
                               ModelConnection& connection);
    void fixOutgoingReverseConnection(uint64_t oldToOperIndex,
                                      uint64_t oldToPortIndex,
                                      uint64_t newToOperIndex,
                                      uint64_t newToPortIndex,
                                      PortKind newPortKind);
    void fixIncomingReverseConnection(uint64_t oldToOperIndex,
                                      uint64_t oldToPortIndex,
                                      uint64_t newToOperIndex,
                                      uint64_t newToPortIndex,
                                      PortKind newPortKind);
    void getOutgoingSplitters(const PhysicalOperator& owningOper,
                              const PhysicalOperatorOutputPort& owningPort,
                              PhysicalSplittersType& _splitters,
                              Model& model) const;
    void createMergers(const ParallelRegion& parallelRegion, Model& model);
    ModelDumper& print(ModelDumper& dumper) const;
};

class ModelCompositeOperator : public ModelOperator
{
  public:
    ModelCompositeOperator(const ADL::compositeOperInstanceType& mainComposite,
                           ModelCompositeOperator* owningComposite,
                           Model& model);
    ModelCompositeOperator(const ModelCompositeOperator& other,
                           uint64_t channelIndex_,
                           ModelCompositeOperator& owningComp,
                           Model& model);
    ~ModelCompositeOperator();

    virtual bool isComposite() const { return true; }

    /// Get physical connections emenating from the input port of this composite operator
    void getInputPortPhysicalConnections(uint64_t fromPhysicalOperIndex,
                                         uint64_t fromPortIndex,
                                         PortKind fromPortKind,
                                         uint64_t portIndex,
                                         const Model& model,
                                         PhysicalConnectionsType& physConnections) const;

    /// Called on an input port
    void getInputPortImportedStreams(uint64_t portIndex,
                                     PhysicalImportedStreamsType& importedStreams,
                                     const Model& model) const;

    void getOutputPortImportedStreams(uint64_t portIndex,
                                      PhysicalImportedStreamsType& importedStreams,
                                      const Model& model) const;

    /// Called by an operator outside of a composite
    void getInputPortExportedStreams(uint64_t portIndex,
                                     PhysicalExportedStreamsType& exportedStreams,
                                     const Model& model) const;

    /// Passes a search for splitters through an input port's outgoing connections
    void getInputPortOutgoingSplitters(uint64_t portIndex,
                                       const PhysicalOperator& owningOper,
                                       const PhysicalOperatorOutputPort& owningPort,
                                       PhysicalSplittersType& _splitters,
                                       Model& model) const;

    /// Passes a search for splitters through an output port's outgoing connections
    void getOutputPortOutgoingSplitters(uint64_t portIndex,
                                        const PhysicalOperator& owningOper,
                                        const PhysicalOperatorOutputPort& owningPort,
                                        PhysicalSplittersType& _splitters,
                                        Model& model) const;

    /// Must be called from an operator inside the conposite
    void getOutputPortExportedStreams(uint64_t portIndex,
                                      PhysicalExportedStreamsType& exportedStreams,
                                      const Model& model) const;

    /// Get physical connections emenating from the output port of this composite operator
    void getOutputPortPhysicalConnections(uint64_t fromPhysicalOperIndex,
                                          uint64_t fromPortIndex,
                                          PortKind fromPortKind,
                                          uint64_t portIndex,
                                          const Model& model,
                                          PhysicalConnectionsType& physConnections) const;

    void performParallelTransform(Model& model);

    /// Create a splitter on all input port incoming connections
    virtual void createSplittersAndMergers(Model& model);
    virtual void addInputPortConnection(uint64_t portIndex, ModelConnection& conn);

    // Called on a replicated composite to add connections from the upstream splitter to the replicted composite
    void addReverseConnections(uint64_t channelIndex, Model& model);

    // Called on a replica to fix it's input port incoming and output port outgoing connections
    void fixConnections(const ParallelTransformState& state);

    void addOutputPortConnection(uint64_t portIndex,
                                 uint64_t splitterIndex,
                                 uint64_t channelIndex,
                                 ModelConnection& connection);

    ModelCompositeOperator& replicate(uint64_t channelIndex_,
                                      ModelCompositeOperator& owningComp,
                                      Model& model) const;

    virtual ModelDumper& print(ModelDumper& dumper) const;

    void addPrimitiveOperator(ModelPrimitiveOperatorBase& oper)
    {
        _primitiveOperators.push_back(&oper);
    }
    virtual void fixReverseConnection(uint64_t fromPortIndex,
                                      PortKind fromPortKind,
                                      uint64_t oldToOperIndex,
                                      uint64_t oldToPortIndex,
                                      uint64_t newToOperIndex,
                                      uint64_t newToPortIndex,
                                      PortKind toPortKind);

    void computeCCReachabilityGraph(uint64_t portIndex,
                                    PortKind portKind,
                                    CCRegion& ccRegion,
                                    ModelPrimitiveOperatorsSet& operatorsSeen,
                                    Model& model) const;

  private:
    uint64_t _compositeOperIndex;
    std::string _description;
    CompositeInputPortsType _inputPorts;
    CompositeOutputPortsType _outputPorts;
    CompositeOperatorsType _compositeOperators;
    PrimitiveOperatorsType _primitiveOperators;
    const ADL::compositeOperInstanceType::splAnnotation_sequence& _splAnnotations;
};

class PhysicalOperatorPort
{
  public:
    PhysicalOperatorPort(uint64_t index_,
                         const std::string& name_,
                         bool isMutable_,
                         uint64_t tupleTypeIndex_,
                         const ModelConnectionsType* connections_,
                         PortKind fromPortKind,
                         const PhysicalOperator& owningOper,
                         const Model& model);
    virtual ~PhysicalOperatorPort();

    uint64_t index() const { return _index; }

    const PhysicalConnectionsType& connections() const { return _connections; }
    void setPEPort(const PhysicalPEPort& pePort_) { _pePort = &pePort_; }
    const PhysicalPEPort& pePort() const;
    virtual uint64_t pePortIndex() const;
    virtual uint64_t pePortIndex(uint64_t, uint64_t) const;

    const std::string& name() const { return _name; }
    const std::string& transport() const { return _transport; }
    const std::string& encoding() const { return _encoding; }
    bool isMutable() const { return _isMutable; }
    uint64_t tupleTypeIndex() const { return _tupleTypeIndex; }

    void populatePADL(ADL::operInstanceInputPortType& iPort, const Model& model) const;
    void populatePADL(PADL::physOperInstanceOutputPortType& oPort, const Model& model) const;

    virtual void setPortAttributes(const std::string& transport_, const std::string& encoding_)
    {
        _transport = transport_;
        _encoding = encoding_;
    }
    const PhysicalOperator& owningOperator() const { return _owningOper; }

    void buildPhysicalConnections(PortKind fromPortKind, const Model& model);
    virtual void buildStaticConnections(PortKind toPortKind,
                                        const Model& model,
                                        StaticConnectionsType& staticConnections) const;
    virtual void buildStaticConnections(const Model& model,
                                        StaticConnectionsType& staticConnections) const = 0;
    virtual void buildStaticConnections(uint64_t splitterIndex,
                                        uint64_t channelIndex,
                                        const Model& model,
                                        StaticConnectionsType& staticConnections) const;

    virtual ModelDumper& print(ModelDumper& dumper) const;

    uint64_t numberOfIntraPEConnections(const Model& model) const;

  protected:
    /// Determine if this operator port has any inter-pe connections
    bool hasInterPEConnections(const Model& model) const;

  private:
    void populatePADL(ADL::operInstancePortType& port) const;

    uint64_t _index;
    std::string _name;
    std::string _transport;
    std::string _encoding;
    bool _isMutable;
    bool _isControl;
    uint64_t _tupleTypeIndex;
    const ModelConnectionsType* _modelConnections;
    const PhysicalOperator& _owningOper;
    PhysicalConnectionsType _connections;
    const PhysicalPEPort* _pePort;
};

class ThreadedPort
{
  public:
    ThreadedPort(const std::string& congestionPolicy,
                 uint64_t queueSize,
                 bool singleThreadedOnInput)
      : _congestionPolicy(congestionPolicy)
      , _queueSize(queueSize)
      , _singleThreadedOnInput(singleThreadedOnInput)
    {}
    ThreadedPort()
      : _congestionPolicy("wait")
      , _queueSize(0)
      , _singleThreadedOnInput(true)
    {}
    void populatePADL(ADL::operInstanceInputPortType& port) const;
    void setNotSingleThreadedOnInput() { _singleThreadedOnInput = false; }

  private:
    std::string _congestionPolicy;
    uint64_t _queueSize;
    bool _singleThreadedOnInput;
};

class PhysicalOperatorInputPort : public PhysicalOperatorPort
{
  public:
    PhysicalOperatorInputPort(uint64_t index_,
                              const std::string& name_,
                              bool isMutable_,
                              bool isControl_,
                              uint64_t tupleTypeIndex_,
                              ThreadedPort* threadedPort_,
                              const ModelConnectionsType* connections_,
                              const PhysicalOperator& owningOper,
                              const Model& model);
    virtual ~PhysicalOperatorInputPort();

    /// Determine if this operator input port needs a PE input port
    bool needsPEPort(const Model& model) const;

    void populatePADL(ADL::operInstanceInputPortsType& iPorts, const Model& model) const;
    bool hasThreadedPort() const { return NULL != _threadedPort; }
    uint64_t observableUpstreamNewThreads(const Model& model, OperatorIndexSet& seen) const;
    const ThreadedPort& threadedPort() const;
    virtual void setPortAttributes(const std::string& transport_, const std::string& encoding_)
    {
        PhysicalOperatorPort::setPortAttributes(transport_, encoding_);
    }
    const PhysicalImportedStreamsType& importedStreams() const { return _importedStreams; }
    virtual void buildStaticConnections(const Model& model,
                                        StaticConnectionsType& staticConnections) const;
    void buildPhysicalConnections(Model& model);
    void injectThreadedPorts(Model& model);
    bool isControl() const { return _isControl; }

    virtual ModelDumper& print(ModelDumper& dumper) const;

  private:
    bool isFedByColocatedSplitter(const Model& model) const;

    ThreadedPort* _threadedPort;
    bool _hasInjectedThread;
    bool _isControl;
    PhysicalImportedStreamsType _importedStreams;
};

class Hostpool
{
  public:
    Hostpool(const ADL::hostpoolType& host, Model& model);
    Hostpool(uint64_t index,
             const std::string& name,
             uint64_t size,
             HostpoolMembership membership,
             const TagSet& tags,
             const HostNamesType& hosts);
    uint64_t index() const { return _index; }
    const TagSet& tags() const { return _tags; }
    const Hostpool& findOrCreateReplica(uint64_t i,
                                        uint64_t channelIndex,
                                        const TagSet& intersectTags,
                                        Model& model);
    void populatePADL(ADL::hostpoolsType& hostpools) const;

  private:
    uint64_t _index;
    std::string _name;
    uint64_t _size;
    HostpoolMembership _membership;
    HostNamesType _hosts;
    TagSet _tags;
    ReplicaHostpoolMap _replicaMap;
};

class PoolLocation
{
  public:
    PoolLocation(const ADL::poolLocationType& poolLocation);
    uint64_t index() const { return _index; }
    void setIndex(uint64_t i) { _index = i; }
    void populatePADL(ADL::poolLocationType& poolLocation) const;

  private:
    uint64_t _index;
    bool _hasInPool;
    uint64_t _inPool;
};

class Resources
{
  public:
    Resources(const ADL::resourcesType& resources);
    ~Resources();
    bool hasPoolLocation() const { return NULL != _poolLocation; }
    PoolLocation& poolLocation() const { return *_poolLocation; }
    void populatePADL(ADL::resourcesType& resources,
                      uint64_t channelIndex,
                      uint64_t maxChannels) const;

  private:
    const ADL::resourcesType& _adlResources;
    PoolLocation* _poolLocation;
};

class Viewable
{
  public:
    Viewable(bool isViewable)
      : _viewable(isViewable)
    {}
    void populatePADL(ADL::operInstanceOutputPortType& oPorts) const;
    bool viewable() const { return _viewable; }

  private:
    bool _viewable;
};

class PhysicalOperatorOutputPort : public PhysicalOperatorPort
{
  public:
    PhysicalOperatorOutputPort(uint64_t index_,
                               const std::string& name_,
                               bool isMutable_,
                               uint64_t tupleTypeIndex_,
                               const std::string& logicalStreamName_,
                               const std::string& streamName_,
                               const Viewable* viewable_,
                               const ModelConnectionsType* connections_,
                               const PhysicalOperator& owningOper,
                               Model& model);
    virtual ~PhysicalOperatorOutputPort();

    /// Determine if this operator input port needs a PE input port
    bool needsPEPort(const Model& model) const;

    void populatePADL(ADL::operInstanceOutputPortsType& oPorts, const Model& model) const;
    const std::string& logicalStreamName() const { return _logicalStreamName; }
    bool hasViewable() const { return NULL != _viewable; }
    const Viewable& viewable() const;
    bool singleThreadedOnOutput() const { return _singleThreadedOnOutput; }
    void setNotSingleThreadedOnOutput() { _singleThreadedOnOutput = false; }
    virtual void buildStaticConnections(const Model& model,
                                        StaticConnectionsType& staticConnections) const;
    virtual void buildStaticConnections(uint64_t splitterIndex,
                                        uint64_t channelIndex,
                                        const Model& model,
                                        StaticConnectionsType& staticConnections) const;

    virtual void setPortAttributes(const std::string& transport_,
                                   const std::string& encoding_,
                                   bool singleThreadedOnOutput_)
    {
        PhysicalOperatorPort::setPortAttributes(transport_, encoding_);
        _singleThreadedOnOutput = singleThreadedOnOutput_;
    }
    const PhysicalSplittersType& splitters() const { return _splitters; }

    const PhysicalExportedStreamsType& exportedStreams() const { return _exportedStreams; }
    virtual uint64_t pePortIndex() const { return PhysicalOperatorPort::pePortIndex(); }
    virtual uint64_t pePortIndex(uint64_t splitterIndex, uint64_t channelIndex) const;
    void buildPhysicalConnections(Model& model);

    virtual ModelDumper& print(ModelDumper& dumper) const;

  private:
    std::string _logicalStreamName;
    std::string _streamName;
    const Viewable* _viewable;
    bool _singleThreadedOnOutput;
    PhysicalExportedStreamsType _exportedStreams; // This should only ever have 0 or 1 stream in it
    PhysicalSplittersType _splitters;
};

class PhysicalOperator
{
  public:
    PhysicalOperator(const ADL::operInstanceType& adlOper,
                     const PhysicalPE& originalPE,
                     Model& model);
    PhysicalOperator(const ModelPrimitiveOperator& mOper, Model& model);
    ~PhysicalOperator();

    /// Assigns this operator to a PE, creating PEs as needed
    void assignPE(Model& model);

    uint64_t index() const { return _index; }
    uint64_t logicalIndex() const { return _adlOper.index(); }
    std::string name() const { return _name; }
    std::string logicalName() const { return _adlOper.name(); }

    const ADL::operInstanceType& adlOperator() const { return _adlOper; }

    void setOwningPE(PhysicalPE& pe);
    PhysicalPE& owningPE() const;
    const PhysicalPE& originalPE() const { return _originalPE; }

    const PhysicalOperatorInputPortsType& inputPorts() const { return _inputPorts; }
    const PhysicalOperatorOutputPortsType& outputPorts() const { return _outputPorts; }

    /// Given a port index and type, find the corresponding PE and PE port indexes
    void peAndPortIndexes(uint64_t operPortIndex,
                          PortKind portKind,
                          uint64_t& toPEIndex,
                          uint64_t& toPEPortIndex) const;

    /// Given an output port index and type, find the corresponding PE and PE port indexes
    void peAndPortIndexes(uint64_t operPortIndex,
                          uint64_t splitterIndex,
                          uint64_t channelIndex,
                          uint64_t& toPEIndex,
                          uint64_t& toPEPortIndex) const;

    void populatePADL(ADL::operInstancesType& operInstances, const Model& model) const;

    const PhysicalOperatorInputPort& inputPort(uint64_t index_) const
    {
        return *_inputPorts[index_];
    }
    PhysicalOperatorInputPort& inputPort(uint64_t index_) { return *_inputPorts[index_]; }
    const PhysicalOperatorOutputPort& outputPort(uint64_t index_) const
    {
        return *_outputPorts[index_];
    }
    PhysicalOperatorOutputPort& outputPort(uint64_t index_) { return *_outputPorts[index_]; }

    const Resources& resources() const { return _resources; }

    void buildPhysicalConnections(Model& model);
    void injectThreadedPorts(Model& model);
    void fixSingleThreaded(Model& model);
    uint64_t observableUpstreamNewThreads(const Model& model, OperatorIndexSet& seen) const;

    ModelDumper& print(ModelDumper& dumper) const;

    bool isInParallelRegion() const { return _modelOper->isInParallelRegion(); }
    const ParallelRegion& containingParallelRegion() const
    {
        return _modelOper->containingParallelRegion();
    }
    const std::string& placement() const { return _placement; }

    bool hasColocationConstraint() const { return _colocationConstraint; }

    uint64_t containingParallelChannelIndex() const
    {
        return _modelOper->containingParallelChannelIndex();
    }

    bool isReplica() const { return _modelOper->isReplica(); }

    const ADL::primitiveOperInstanceType::splAnnotation_sequence& annotations() const
    {
        return _modelOper->annotations();
    }

    const CCInfo& ccInfo() const { return _modelOper->ccInfo(); }

    uint64_t toolkitIndex() const { return _modelOper->toolkitIndex(); }

    const ModelCompositeOperator& owningComposite() const { return *_modelOper->owningComposite(); }

    bool isParallelRegion() const { return _modelOper->isParallelRegion(); }

  private:
    uint64_t originalPEIndex() const;

    bool isColocatedWithNonUDPOperator(const Model& model) const;

    const ADL::operInstanceType& _adlOper;
    uint64_t _index;
    std::string _name;
    std::string _placement;
    PhysicalPE* _owningPE;
    const PhysicalPE& _originalPE;
    const ModelPrimitiveOperator* _modelOper;
    bool _colocationConstraint;
    bool _singleThreadedOnInputs;
    bool _singleThreadedOnOutputs;
    PhysicalOperatorInputPortsType _inputPorts;
    PhysicalOperatorOutputPortsType _outputPorts;
    Resources _resources;
};

class PhysicalPEPort
{
  public:
    virtual ~PhysicalPEPort();

    uint64_t index() const { return _index; }
    uint64_t tupleTypeIndex() const { return _tupleTypeIndex; }
    const std::string& transport() const { return _transport; }
    const std::string& encoding() const { return _encoding; }
    const PhysicalPE& owningPE() const { return _owningPE; }
    ModelDumper& print(ModelDumper& dumper) const;

  protected:
    PhysicalPEPort(uint64_t index_,
                   uint64_t tupleTypeIndex_,
                   const std::string& transport_,
                   const std::string& encoding_,
                   const PhysicalPE& owningPE,
                   const PhysicalOperator& physicalOper,
                   const PhysicalOperatorPort& physicalOperPort);

    uint64_t physicalOperatorIndex() const;
    uint64_t physicalPortIndex() const;

    void populatePADL(ADL::incomingStaticConnectionsType& conns, const Model& model);
    void populatePADL(ADL::outgoingStaticConnectionsType& conns, const Model& model);

    const PhysicalOperatorPort& physicalOperatorPort() const { return _physicalOperPort; }

    StaticConnectionsType& staticConnections() { return _staticConnections; }

  private:
    uint64_t _index;
    uint64_t _tupleTypeIndex;
    std::string _transport;
    std::string _encoding;
    const PhysicalPE& _owningPE;
    const PhysicalOperator& _physicalOper;
    const PhysicalOperatorPort& _physicalOperPort;

    StaticConnectionsType _staticConnections;
};

class PhysicalImportedStream
{
  public:
    virtual ~PhysicalImportedStream() {}
    virtual void populatePADL(PADL::physImportedStreamsType& is, uint64_t& index) const = 0;

    const std::string& importOperName() const { return _importOperName; }
    const std::string& logicalImportOperName() const { return _logicalImportOperName; }
    const std::string& filter() const { return _filter; }
    const std::string& applicationScope() const { return _applicationScope; }
    int64_t maxChannels() const { return _maxChannels; }
    int64_t channelIndex() const { return _channelIndex; }

  protected:
    PhysicalImportedStream(const std::string& importOperName_,
                           const std::string& logicalImportOperName_,
                           const std::string& filter_,
                           const std::string& applicationScope_,
                           bool isInParallelRegion_,
                           int64_t maxChannels_,
                           int64_t channelIndex_)
      : _importOperName(importOperName_)
      , _logicalImportOperName(logicalImportOperName_)
      , _filter(filter_)
      , _applicationScope(applicationScope_)
      , _isInParallelRegion(isInParallelRegion_)
      , _maxChannels(maxChannels_)
      , _channelIndex(channelIndex_)
    {}

  private:
    std::string _importOperName;
    std::string _logicalImportOperName;
    std::string _filter;
    std::string _applicationScope;
    bool _isInParallelRegion;
    int64_t _maxChannels;
    int64_t _channelIndex;
};

class PhysicalNameBasedImport : public PhysicalImportedStream
{
  public:
    PhysicalNameBasedImport(const std::string& importOperName_,
                            const std::string& logicalImportOperName_,
                            const std::string& filter_,
                            const std::string& applicationScope_,
                            const std::string& applicationName_,
                            const std::string& streamName_,
                            bool isInParallelRegion_,
                            int64_t maxChannels_,
                            int64_t channelIndex_)
      : PhysicalImportedStream(importOperName_,
                               logicalImportOperName_,
                               filter_,
                               applicationScope_,
                               isInParallelRegion_,
                               maxChannels_,
                               channelIndex_)
      , _applicationName(applicationName_)
      , _streamName(streamName_)
    {}

    virtual void populatePADL(PADL::physImportedStreamsType& is, uint64_t& index) const;

  private:
    std::string _applicationName;
    std::string _streamName;
};

class PhysicalPropertyBasedImport : public PhysicalImportedStream
{
  public:
    PhysicalPropertyBasedImport(const std::string& importOperName_,
                                const std::string& logicalImportOperName_,
                                const std::string& filter_,
                                const std::string& applicationScope_,
                                const std::string& subscription_,
                                bool isInParallelRegion_,
                                int64_t maxChannels_,
                                int64_t channelIndex_)
      : PhysicalImportedStream(importOperName_,
                               logicalImportOperName_,
                               filter_,
                               applicationScope_,
                               isInParallelRegion_,
                               maxChannels_,
                               channelIndex_)
      , _subscription(subscription_)
    {}

    virtual void populatePADL(PADL::physImportedStreamsType& is, uint64_t& index) const;

  private:
    std::string _subscription;
};

class ImportedStream
{
  public:
    virtual ~ImportedStream() {}

    const std::string& filter() const { return _filter; }
    const std::string& applicationScope() const { return _applicationScope; }
    const std::string& transport() const { return _transport; }
    const std::string& encoding() const { return _encoding; }
    uint64_t tupleTypeIndex() const { return _tupleTypeIndex; }

    virtual const PhysicalImportedStream& createPhysicalImportedStream(
      const std::string& importOperName,
      const std::string& logicalImportOperName,
      bool isInParallelRegion,
      int64_t maxChannels,
      int64_t channelIndex) const = 0;

  protected:
    ImportedStream(const std::string& filter_,
                   const std::string& applicationScope_,
                   const std::string& transport_,
                   const std::string& encoding_,
                   uint64_t tupleTypeIndex_)
      : _filter(filter_)
      , _applicationScope(applicationScope_)
      , _transport(transport_)
      , _encoding(encoding_)
      , _tupleTypeIndex(tupleTypeIndex_)
    {}

  private:
    std::string _filter;
    std::string _applicationScope;
    std::string _transport;
    std::string _encoding;
    uint64_t _tupleTypeIndex;
};

class NameBasedImport : public ImportedStream
{
  public:
    NameBasedImport(const ADL::nameBasedImportType& nameBasedImport,
                    const std::string& filter_,
                    const std::string& transport_,
                    const std::string& encoding_,
                    uint64_t tupleTypeIndex_);

    virtual const PhysicalImportedStream& createPhysicalImportedStream(
      const std::string& importOperName,
      const std::string& logicalImportOperName,
      bool isInParallelRegion,
      int64_t maxChannels,
      int64_t channelIndex) const;

  private:
    std::string _applicationName;
    std::string _streamName;
};

class PropertyBasedImport : public ImportedStream
{
  public:
    PropertyBasedImport(const ADL::propertyBasedImportType& propertyBasedImport,
                        const std::string& filter_,
                        const std::string& transport_,
                        const std::string& encoding_,
                        uint64_t tupleTypeIndex_);

    virtual const PhysicalImportedStream& createPhysicalImportedStream(
      const std::string& importOperName,
      const std::string& logicalImportOperName,
      bool isInParallelRegion,
      int64_t maxChannels,
      int64_t channelIndex) const;

  private:
    std::string _subscription;
};

class ExportedStream
{
  public:
    ExportedStream(const ADL::exportedStreamType& exportedStream_,
                   const std::string& transport_,
                   const std::string& encoding_,
                   uint64_t tupleTypeIndex_)
      : _exportedStream(exportedStream_)
      , _transport(transport_)
      , _encoding(encoding_)
      , _tupleTypeIndex(tupleTypeIndex_)
    {}

    const std::string& transport() const { return _transport; }
    const std::string& encoding() const { return _encoding; }
    uint64_t tupleTypeIndex() const { return _tupleTypeIndex; }

    const PhysicalExportedStream& createPhysicalExportedStream(
      const std::string& exportOperName,
      const std::string& logicalExportOperName,
      bool isInParallelRegion,
      int64_t maxChannels,
      int64_t channelIndex) const;

  private:
    const ADL::exportedStreamType& _exportedStream;
    std::string _transport;
    std::string _encoding;
    uint64_t _tupleTypeIndex;
};

class PhysicalExportedStream
{
  public:
    PhysicalExportedStream(const ADL::exportedStreamType& exportedStream,
                           const std::string& exportOperName,
                           const std::string& logicalExportOperName,
                           bool isInParallelRegion_,
                           int64_t maxChannels_,
                           int64_t channelIndex_)
      : _exportedStream(exportedStream)
      , _exportOperName(exportOperName)
      , _logicalExportOperName(logicalExportOperName)
      , _isInParallelRegion(isInParallelRegion_)
      , _maxChannels(maxChannels_)
      , _channelIndex(channelIndex_)
    {}

    void populatePADL(PADL::physExportedStreamType& es) const;
    ModelDumper& print(ModelDumper& dumper) const;

  private:
    const ADL::exportedStreamType& _exportedStream;
    std::string _exportOperName;
    std::string _logicalExportOperName;
    bool _isInParallelRegion;
    int64_t _maxChannels;
    int64_t _channelIndex;
};

class PhysicalPEInputPort : public PhysicalPEPort
{
  public:
    PhysicalPEInputPort(uint64_t index_,
                        uint64_t tupleTypeIndex_,
                        const std::string& transport_,
                        const std::string& encoding_,
                        const PhysicalPE& owningPE,
                        const PhysicalOperator& physicalOper,
                        const PhysicalOperatorInputPort& operIPort);

    void populatePADL(ADL::peInputPortsType& iPorts, const Model& model);
    void addImportedStream(const PhysicalImportedStream& is) { _importedStreams.push_back(&is); }
    void buildStaticConnections(const Model& model);
    ModelDumper& print(ModelDumper& dumper) const;

  private:
    PhysicalImportedStreamsType _importedStreams;
};

class PhysicalPEOutputPort : public PhysicalPEPort
{
  public:
    PhysicalPEOutputPort(uint64_t index_,
                         uint64_t tupleTypeIndex_,
                         const std::string& transport_,
                         const std::string& encoding_,
                         bool singleThreadedOnOutput_,
                         const PhysicalPE& owningPE,
                         const PhysicalOperator& physicalOper,
                         const PhysicalOperatorOutputPort& operOPort);

    void populatePADL(ADL::peOutputPortsType& oPorts, const Model& model);
    bool singleThreadedOnOutput() const { return _singleThreadedOnOutput; }
    void addExportedStream(const PhysicalExportedStream& es) { _exportedStreams.push_back(&es); }
    void setSplitterInfo(uint64_t splitterIndex, uint64_t channelIndex)
    {
        _fedBySplitter = true;
        _splitterIndex = splitterIndex;
        _channelIndex = channelIndex;
    }
    void buildStaticConnections(const Model& model);
    ModelDumper& print(ModelDumper& dumper) const;

  private:
    bool _singleThreadedOnOutput;
    bool _fedBySplitter;
    uint64_t _splitterIndex;
    uint64_t _channelIndex;
    PhysicalExportedStreamsType _exportedStreams; // There should be 0 or one stream max
};

class PhysicalPE
{
  public:
    PhysicalPE(const ADL::peType& pe, Model& model);
    PhysicalPE(const PhysicalPE& pe, Model& model);
    PhysicalPE(const PhysicalPE& pe, uint64_t index_, Model& model);
    ~PhysicalPE();

    /// Allocate input and output ports for all operators in this PE
    void allocatePorts(const Model& model);

    /// Build any static connections for the input/output ports in this PE
    void buildStaticConnections(const Model& model);

    /// Add this PE to the PADL pes section
    /// @param pes The pesType to add this PE to
    void populatePADL(ADL::pesType& pes, const Model& model) const;

    void setParallelRegionInfo(const ParallelRegion& parallelRegion, uint64_t channelIndex)
    {
        _parallelRegion = &parallelRegion;
        _channelIndex = channelIndex;
    }
    void addOperator(PhysicalOperator& oper);
    uint64_t index() const { return _index; }
    uint64_t logicalIndex() const { return _logicalIndex; }
    PhysicalPE& findOrCreateReplica(uint64_t channelIndex, Model& model);
    ModelDumper& print(ModelDumper& dumper) const;

  private:
    PhysicalPEInputPort& allocateInputPort(const PhysicalOperator& physicalOper,
                                           const PhysicalOperatorInputPort& operIPort);
    PhysicalPEOutputPort& allocateOutputPort(const PhysicalOperator& physicalOper,
                                             const PhysicalOperatorOutputPort& operOPort);
    PhysicalPEOutputPort& allocateOutputPort(const PhysicalOperator& physicalOper,
                                             const PhysicalOperatorOutputPort& operOPort,
                                             uint64_t splitterIndex,
                                             uint64_t channelIndex);

    const ADL::peType& _adlPE;
    uint64_t _index;
    uint64_t _logicalIndex;
    PhysicalOperatorsType _operators;
    const ParallelRegion* _parallelRegion;
    uint64_t _channelIndex;
    PhysicalPEInputPortsType _inputPorts;
    PhysicalPEOutputPortsType _outputPorts;
    PEMap _replicaMap;
};

class StaticConnection
{
  public:
    StaticConnection(uint64_t toPEIndex, uint64_t toPEPortIndex, PortKind portKind)
      : _toPEIndex(toPEIndex)
      , _toPEPortIndex(toPEPortIndex)
      , _portKind(portKind)
    {}

    void populatePADL(ADL::incomingStaticConnectionsType& conns, const Model& model) const;
    void populatePADL(ADL::outgoingStaticConnectionsType& conns, const Model& model) const;

    ModelDumper& print(ModelDumper& dumper) const;

  private:
    uint64_t _toPEIndex;
    uint64_t _toPEPortIndex;
    PortKind _portKind;
};

class PhysicalConnection
{
  public:
    PhysicalConnection(uint64_t fromPhysOperIndex,
                       uint64_t fromPhysPortIndex,
                       uint64_t toPhysOperIndex,
                       uint64_t toPhysPortIndex,
                       PortKind portKind);
    PhysicalConnection(uint64_t fromPhysOperIndex,
                       uint64_t fromPhysPortIndex,
                       uint64_t toPhysOperIndex,
                       uint64_t toPhysPortIndex,
                       PortKind portKind,
                       uint64_t splitterIndex_,
                       uint64_t channelIndex_);

    bool isInterPE(const Model& model) const;
    void toPEAndPortIndexes(uint64_t& toPEIndex, uint64_t& toPEPortIndex, const Model& model) const;

    void populatePADL(
      ADL::operInstanceInputPortType::interPeConnection_sequence& interPEConnections,
      ADL::operInstanceInputPortType::intraPeConnection_sequence& intraPEConnections,
      const PhysicalPEPort* port,
      const Model& model) const;
    void populatePADL(
      ADL::operInstanceOutputPortType::interPeConnection_sequence& interPEConnections,
      ADL::operInstanceOutputPortType::intraPeConnection_sequence& intraPEConnections,
      const PhysicalPEPort* port,
      const Model& model) const;

    bool pointsToColocatedSplitter(const Model& model) const;
    uint64_t observableUpstreamNewThreads(const Model& model, OperatorIndexSet& seen) const;

    ModelDumper& print(ModelDumper& dumper) const;

  private:
    bool leavesParallelRegion(const Model& model) const;

    uint64_t _fromPhysOperIndex;
    uint64_t _fromPhysPortIndex;
    uint64_t _toPhysOperIndex;
    uint64_t _toPhysPortIndex;
    PortKind _portKind;
    bool _isSplitterConnection;
    uint64_t _splitterIndex;
    uint64_t _channelIndex;
};

class ConsistentCutRegions
{
  public:
    ~ConsistentCutRegions();
    void addRegion(CCRegion& region, uint64_t channelIndex);
    CCRegion& findRegion(uint64_t regionIndex, uint64_t channelIndex) const;
    CCRegion& findOrCreateRegion(uint64_t regionIndex, uint64_t channelIndex);
    std::vector<CCRegion*>& regions() { return _regions; }

  private:
    std::vector<CCRegion*> _regions;
    CCRegionMap _regionMap;
    //    OperToCCInfoMap            _operToCCInfoMap;
};

class ParallelRegions
{
  public:
    void initialize(const ADL::applicationType& logicalApp_);
    bool empty() const { return _parallelRegionMap.size() == 0; }
    bool isParallelOperator(uint64_t operIndex) const;
    const ParallelRegion& parallelRegion(uint64_t operIndex) const;

  private:
    ParallelRegionMap _parallelRegionMap;
    ParallelOperatorMap _parallelOperatorMap;
};

class ParallelRegion
{
  public:
    ParallelRegion(const ADL::parallelRegionType& parallelRegion_);
    uint64_t index() const { return _index; }
    int32_t width() const { return _width; }
    uint64_t operatorIndex() const { return _operIndex; }
    const TagSet& replicateTags() const { return _replicateTags; }
    void populatePADL(ADL::splitterType& splitter, uint64_t iPortIndex) const;

  private:
    uint64_t _index;
    int32_t _width;
    uint64_t _operIndex;
    ParallelRegionSplitterMap _splitterMap;
    TagSet _replicateTags;
};

/// This class contains information from the physical (pes) section of the ADL
/// that must be carried over to the Model in order to be able to build a new physical model.
class Model
{
  public:
    Model() {}
    virtual ~Model();

    PEMap& peMap() { return _physicalPEs; }
    const PEMap& peMap() const { return _physicalPEs; }
    /// Get the PE for the given index.  It will throw if the index is invalid.
    PhysicalPE& physicalPE(uint64_t index);
    const PhysicalPE& physicalPE(uint64_t index) const;

    virtual HostpoolMap& hostpoolMap() = 0;

    PhysicalOperatorMap& physicalOperatorMap() { return _physicalOperators; }
    const PhysicalOperatorMap& physicalOperatorMap() const { return _physicalOperators; }
    const PhysicalOperator& physicalOperator(uint64_t index) const;
    PhysicalOperator& physicalOperator(uint64_t index);

    virtual ModelOperatorMap& modelOperatorMap() = 0;
    virtual const ModelOperatorMap& modelOperatorMap() const = 0;
    virtual const ModelOperator& modelOperator(uint64_t index) const = 0;
    virtual ModelOperator& modelOperator(uint64_t index) = 0;
    virtual void setMaxPrimitiveOperIndex(uint64_t index) = 0;

    virtual PrimitiveOperatorMap& primitiveOperatorMap() = 0;
    virtual const PrimitiveOperatorMap& primitiveOperatorMap() const = 0;
    virtual const ModelPrimitiveOperator& primitiveOperator(uint64_t index) const = 0;

    virtual const ADL::applicationType& logicalApp() const = 0;

    virtual const ImportedStream& findOrCreateNameBasedImport(
      const ADL::nameBasedImportType& nameBasedImport,
      const std::string& filter,
      const std::string& transport,
      const std::string& encoding,
      uint64_t tupleTypeIndex_) = 0;
    virtual const ImportedStream& findOrCreatePropertyBasedImport(
      const ADL::propertyBasedImportType& propertyBasedImport,
      const std::string& filter,
      const std::string& transport,
      const std::string& encoding,
      uint64_t tupleTypeIndex_) = 0;
    virtual const ImportedStream* findImportedStream(const std::string& operName) = 0;
    virtual const ExportedStream& findOrCreateExportedStream(
      const ADL::exportedStreamType& exportedStream,
      const std::string& transport,
      const std::string& encoding,
      uint64_t tupleTypeIndex_) = 0;
    virtual const ExportedStream& findExportedStream(const std::string& operName) = 0;

    virtual const ParallelRegions& parallelRegions() const = 0;

    virtual ConsistentCutRegions& ccRegions() = 0;
    virtual const ConsistentCutRegions& ccRegions() const = 0;
    virtual ModelPrimitiveOperatorsSet& multiRegionOperators() = 0;
    virtual PrimitiveOperatorsType& ccStartOperators() = 0;

    SplitterMap& splitterMap() { return _splitterMap; }
    const SplitterMap& splitterMap() const { return _splitterMap; }
    const SplitterMappings& findSplitterMapping(uint64_t splitterOperIndex) const;
    void addSplitterMapping(uint64_t splitterOperIndex,
                            uint64_t owningOperatorIndex,
                            uint64_t owningPortIndex,
                            uint64_t physicalSplitterIndex);

  private:
    PEMap _physicalPEs;
    PhysicalOperatorMap _physicalOperators;
    SplitterMap _splitterMap;
};

class LogicalModel : public Model
{
  public:
    /// Constructs a logical application model from the given ADL
    /// @param logicalApp the logical ADL as provided by the compiler
    LogicalModel(const ADL::applicationType& logicalApp);
    ~LogicalModel();

    /// Performs any transformations on the model indicated in the ADL
    void performTransforms();
    void computeCCRegions();

    virtual HostpoolMap& hostpoolMap() { return _modelHostpools; }

    virtual ModelOperatorMap& modelOperatorMap() { return _modelOperators; }
    virtual const ModelOperatorMap& modelOperatorMap() const { return _modelOperators; }
    virtual const ModelOperator& modelOperator(uint64_t index) const;
    virtual ModelOperator& modelOperator(uint64_t index);

    virtual void setMaxPrimitiveOperIndex(uint64_t index)
    {
        if (_maxPrimitiveOperIndex < index)
            _maxPrimitiveOperIndex = index;
    }

    virtual PrimitiveOperatorMap& primitiveOperatorMap() { return _modelPrimitiveOperators; }
    virtual const PrimitiveOperatorMap& primitiveOperatorMap() const
    {
        return _modelPrimitiveOperators;
    }
    virtual const ModelPrimitiveOperator& primitiveOperator(uint64_t index) const;

    virtual const ADL::applicationType& logicalApp() const { return _logicalApp; }

    virtual const ImportedStream& findOrCreateNameBasedImport(
      const ADL::nameBasedImportType& nameBasedImport,
      const std::string& filter,
      const std::string& transport,
      const std::string& encoding,
      uint64_t tupleTypeIndex);
    virtual const ImportedStream& findOrCreatePropertyBasedImport(
      const ADL::propertyBasedImportType& propertyBasedImport,
      const std::string& filter,
      const std::string& transport,
      const std::string& encoding,
      uint64_t tupleTypeIndex);
    virtual const ImportedStream* findImportedStream(const std::string& operName);
    virtual const ExportedStream& findOrCreateExportedStream(
      const ADL::exportedStreamType& exportedStream,
      const std::string& transport,
      const std::string& encoding,
      uint64_t tupleTypeIndex_);
    virtual const ExportedStream& findExportedStream(const std::string& operName);

    virtual const ParallelRegions& parallelRegions() const { return _parallelRegions; }

    virtual ConsistentCutRegions& ccRegions() { return _ccRegions; }
    virtual const ConsistentCutRegions& ccRegions() const { return _ccRegions; }
    virtual ModelPrimitiveOperatorsSet& multiRegionOperators() { return _multiRegionOperators; }
    virtual PrimitiveOperatorsType& ccStartOperators() { return _ccStartOperators; }

    ModelDumper& print(ModelDumper& dumper) const;

  private:
    const ADL::applicationType& _logicalApp;
    ModelCompositeOperator* _mainComposite;
    uint64_t _maxPrimitiveOperIndex;

    HostpoolMap _modelHostpools;
    ModelOperatorMap _modelOperators;
    PrimitiveOperatorMap _modelPrimitiveOperators;
    ImportedStreamsMap _importedStreams;
    ExportedStreamsMap _exportedStreams;
    ParallelRegions _parallelRegions;
    ConsistentCutRegions _ccRegions;
    ModelPrimitiveOperatorsSet _multiRegionOperators;
    PrimitiveOperatorsType _ccStartOperators;
};

class PhysicalModel : public Model
{
  public:
    PhysicalModel(LogicalModel& logicalModel);

    /// Generates physical ADL from the model
    /// @param Returns the physical ADL that represents the model
    std::auto_ptr<ADL::applicationType> physicalADL();

    virtual HostpoolMap& hostpoolMap() { return _logicalModel.hostpoolMap(); }

    virtual ModelOperatorMap& modelOperatorMap() { return _logicalModel.modelOperatorMap(); }
    virtual const ModelOperatorMap& modelOperatorMap() const
    {
        return _logicalModel.modelOperatorMap();
    }
    virtual const ModelOperator& modelOperator(uint64_t index) const
    {
        return _logicalModel.modelOperator(index);
    }
    virtual ModelOperator& modelOperator(uint64_t index)
    {
        return _logicalModel.modelOperator(index);
    }

    virtual void setMaxPrimitiveOperIndex(uint64_t index)
    {
        _logicalModel.setMaxPrimitiveOperIndex(index);
    }

    virtual PrimitiveOperatorMap& primitiveOperatorMap()
    {
        return _logicalModel.primitiveOperatorMap();
    }
    virtual const PrimitiveOperatorMap& primitiveOperatorMap() const
    {
        return _logicalModel.primitiveOperatorMap();
    }
    virtual const ModelPrimitiveOperator& primitiveOperator(uint64_t index) const
    {
        return _logicalModel.primitiveOperator(index);
    }

    virtual const ADL::applicationType& logicalApp() const { return _logicalModel.logicalApp(); }
    virtual const ParallelRegions& parallelRegions() const
    {
        return _logicalModel.parallelRegions();
    }

    virtual ConsistentCutRegions& ccRegions() { return _logicalModel.ccRegions(); }
    virtual const ConsistentCutRegions& ccRegions() const { return _logicalModel.ccRegions(); }
    virtual ModelPrimitiveOperatorsSet& multiRegionOperators()
    {
        return _logicalModel.multiRegionOperators();
    }
    virtual PrimitiveOperatorsType& ccStartOperators() { return _logicalModel.ccStartOperators(); }

    virtual const ImportedStream& findOrCreateNameBasedImport(const ADL::nameBasedImportType&,
                                                              const std::string&,
                                                              const std::string&,
                                                              const std::string&,
                                                              uint64_t);
    virtual const ImportedStream& findOrCreatePropertyBasedImport(
      const ADL::propertyBasedImportType&,
      const std::string&,
      const std::string&,
      const std::string&,
      uint64_t);
    virtual const ImportedStream* findImportedStream(const std::string& operName);
    virtual const ExportedStream& findOrCreateExportedStream(
      const ADL::exportedStreamType& exportedStream,
      const std::string& transport,
      const std::string& encoding,
      uint64_t tupleTypeIndex_);
    virtual const ExportedStream& findExportedStream(const std::string& operName);

    ModelDumper& print(ModelDumper& dumper) const;

  private:
    LogicalModel& _logicalModel;
};

class End
{};

class Indent
{
  public:
    Indent()
      : _indent(0)
    {}
    uint32_t indent() const { return _indent; }
    void operator++() { _indent += 2; }
    void operator--() { _indent -= 2; }

  private:
    uint32_t _indent;
};

class ModelDumper
{
  public:
    ModelDumper& operator<<(const End&);
    ModelDumper& operator<<(const Indent& i);
    ModelDumper& operator<<(const char*);
    ModelDumper& operator<<(const std::string& s);
    ModelDumper& operator<<(uint64_t v);
    ModelDumper& operator<<(const PhysicalModel& pm)
    {
        pm.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const LogicalModel& lm)
    {
        lm.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const ModelOperator& op)
    {
        op.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const ModelPort& port)
    {
        port.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const PhysicalConnection& conn)
    {
        conn.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const ModelConnection& conn)
    {
        conn.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const StaticConnection& conn)
    {
        conn.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const PhysicalSplitter& splitter)
    {
        splitter.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const ModelSplitterOperator& splitter)
    {
        splitter.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const PhysicalParallelChannel& channel)
    {
        channel.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const ModelCompositePort& port)
    {
        port.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const PhysicalOperatorPort& port)
    {
        port.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const PhysicalPEOutputPort& port)
    {
        port.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const PhysicalPEInputPort& port)
    {
        port.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const PhysicalOperator& op)
    {
        op.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const PhysicalPE& pe)
    {
        pe.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const PhysicalExportedStream& es)
    {
        es.print(*this);
        return *this;
    }
    ModelDumper& operator<<(const PortKind pk);
    ModelDumper& operator<<(const CCInfo& ccInfo)
    {
        ccInfo.print(*this);
        return *this;
    }
};

std::string performIntrinsicFunctionEval(const std::string& expStr,
                                         int64_t maxChannels,
                                         int64_t channelIndex);
std::string submissionTimePropertiesEval(const std::string& propStr,
                                         int64_t maxChannels,
                                         int64_t channelIndex);

} // namespace Transformer

using namespace Transformer;

/// Performs the logical ADL -> physical ADL transformation.
/// @param logicalApp the logical ADL as provided by the compiler
/// @return the transformation of the logicalApp to the physical
std::auto_ptr<ADL::applicationType> transform(const ADL::applicationType& logicalApp)
  __attribute__((visibility("default")));
}

SAM_NAMESPACE_END

#endif // LOGICAL_TO_PHYSICAL_H
