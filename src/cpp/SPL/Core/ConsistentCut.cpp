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

#include <SPL/Core/ConsistentCut.h> // Include this one first

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/Core/CompilationState.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/ExportSpec.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/OperatorGraphNode.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SourceLocation.h>
#include <SPL/Utility/SysOut.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include <boost/pool/object_pool.hpp>

using namespace std;
using namespace tr1;
using namespace SPL;

namespace SPL {
namespace ConsistentCutRegion {

struct MergedPair
{
    MergedPair(const CCRegion& regionA, const CCRegion& regionB)
      : _regionA(&regionA)
      , _regionB(&regionB)
    {}
    const CCRegion* _regionA;
    const CCRegion* _regionB;
};

} // namespace ConsistentCutRegion
} // namespace SPL

namespace std {

template<>
struct equal_to<SPL::ConsistentCutRegion::MergedPair>
{
    bool operator()(const SPL::ConsistentCutRegion::MergedPair& l,
                    const SPL::ConsistentCutRegion::MergedPair& r) const
    {
        if (l._regionA == r._regionA && l._regionB == r._regionB)
            return true;
        if (l._regionA == r._regionB && l._regionB == r._regionA)
            return true;
        return false;
    }
};

namespace tr1 {

template<>
struct hash<SPL::ConsistentCutRegion::MergedPair>
{
    size_t operator()(const SPL::ConsistentCutRegion::MergedPair& l) const
    {
        hash<const SPL::ConsistentCutRegion::CCRegion*> h;
        size_t r = 17;
        r = 37 * r + h(l._regionA);
        r = 37 * r + h(l._regionB);
        return r;
    }
};

} // namespace tr1
} // namespace std

namespace SPL {
namespace ConsistentCutRegion {

typedef unordered_map<MergedPair, CCRegion*> MergeMap;

class CCRegion
{
  public:
    enum Trigger
    {
        Periodic,
        OperatorDriven
    };

    CCRegion(const SPLAnnotation& ccAnnot_, const string& name_);
    void addOperator(const PrimitiveOperatorInstance& primitive) { _operators.insert(&primitive); }

    const PrimitiveOperatorSet& operators() const { return _operators; }

    void setMergedWith(CCRegion& region)
    {
        assert(NULL == _mergedWith);
        _mergedWith = &region;
    }
    CCRegion& mergedWith() const
    {
        assert(NULL != _mergedWith);
        return *_mergedWith;
    }
    bool wasMerged() const { return NULL != _mergedWith; }

    void merge(CCRegion& other);

    const string& name() const { return _name; }

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

    bool isOperatorDriven() const;

    const SPLSimpleAnnotation::KeyValuePair& trigger() const { return *_trigger; }
    const SPLSimpleAnnotation::KeyValuePair& period() const
    {
        assert(NULL != _period);
        return *_period;
    }
    const SPLSimpleAnnotation::KeyValuePair& drainTimeout() const { return *_drainTimeout; }
    const SPLSimpleAnnotation::KeyValuePair& resetTimeout() const { return *_resetTimeout; }
    const SPLSimpleAnnotation::KeyValuePair& maxConsecutiveResetAttempts() const
    {
        return *_maxConsecutiveResetAttempts;
    }

    bool operator==(const CCRegion& other) const
    {
        return &this->mergedRegion() == &other.mergedRegion();
    }

    const SourceLocation& sourceLocation() const { return _loc; }

  private:
    const SourceLocation& _loc;
    string _name;
    CCRegion* _mergedWith;
    PrimitiveOperatorSet _operators;
    SPLSimpleAnnotation::KeyValuePair _defaultDrainTimeout;
    SPLSimpleAnnotation::KeyValuePair _defaultResetTimeout;
    SPLSimpleAnnotation::KeyValuePair _defaultMaxConsecutiveResetAttempts;
    const SPLSimpleAnnotation::KeyValuePair* _trigger;
    const SPLSimpleAnnotation::KeyValuePair* _period;
    const SPLSimpleAnnotation::KeyValuePair* _drainTimeout;
    const SPLSimpleAnnotation::KeyValuePair* _resetTimeout;
    const SPLSimpleAnnotation::KeyValuePair* _maxConsecutiveResetAttempts;
};

bool CCRegion::isOperatorDriven() const
{
    assert(NULL != _trigger);
    const string& s = _trigger->value();
    if (s == "\"operatorDriven\"") {
        return true;
    }
    assert(s == "\"periodic\"");
    return false;
}

int32_t getIntegerValue(const string& value)
{
    istringstream istr(value, istringstream::in);
    int64_t i;
    istr >> i;
    return i;
}

double getFloatValue(const string& value)
{
    istringstream istr(value, istringstream::in);
    double d;
    istr >> d;
    return d;
}

CCRegion::CCRegion(const SPLAnnotation& ccAnnot, const string& name_)
  : _loc(ccAnnot.sourceLocation())
  , _name(name_)
  , _mergedWith(NULL)
  , _defaultDrainTimeout("drainTimeout", "180", TypeFactory::instance().primitiveT("float64"), _loc)
  , _defaultResetTimeout("resetTimeout", "180", TypeFactory::instance().primitiveT("float64"), _loc)
  , _defaultMaxConsecutiveResetAttempts("maxConsecutiveResetAttempts",
                                        "5",
                                        TypeFactory::instance().primitiveT("int32"),
                                        _loc)
  , _trigger(NULL)
  , _period(NULL)
  , _drainTimeout(&_defaultDrainTimeout)
  , _resetTimeout(&_defaultResetTimeout)
  , _maxConsecutiveResetAttempts(&_defaultMaxConsecutiveResetAttempts)
{
    assert(ccAnnot.is<SPLSimpleAnnotation>());
    const SPLSimpleAnnotation& annot = ccAnnot.as<SPLSimpleAnnotation>();
    const vector<SPLSimpleAnnotation::KeyValuePair*>& values = annot.values();
    for (vector<SPLSimpleAnnotation::KeyValuePair*>::const_iterator it = values.begin();
         it != values.end(); ++it) {
        const SPLSimpleAnnotation::KeyValuePair& p = **it;
        if (p.key() == "trigger") {
            _trigger = &p;
        } else if (p.key() == "period") {
            _period = &p;
        } else if (p.key() == "drainTimeout") {
            _drainTimeout = &p;
        } else if (p.key() == "resetTimeout") {
            _resetTimeout = &p;
        } else if (p.key() == "maxConsecutiveResetAttempts") {
            _maxConsecutiveResetAttempts = &p;
        } else {
            cerr << "Unrecognized parameter: " << p.key();
        }
    }
}

const SPLSimpleAnnotation::KeyValuePair* bestSubmissionTime(
  const SPLSimpleAnnotation::KeyValuePair& thisOne,
  const SPLSimpleAnnotation::KeyValuePair& theOtherOne,
  const SourceLocation& loc1,
  const SourceLocation& loc2)
{
    // We have two possible submission-time values for a given parameter.  Fail if they have different submission-time names.
    // If they both have defaults, pick the one with the better default.
    // Otherwise pick the one with a default
    // Otherwise pick one.

    if (thisOne.submissionTimeName() != theOtherOne.submissionTimeName()) {
        SysOut::errorln(SPL_CORE_CC_REGION_CONFLICTING_SUBST_NAMES, loc1);
        SysOut::detailsErrorln(SPL_CORE_CONFLICTING_ANNOTATION, loc2);
        return &thisOne;
    }
    // We cannot have two submission-time values with the same name and different defaults.
    // If one has a default, use it
    if (!thisOne.defaultValue().empty())
        return &thisOne;
    if (!theOtherOne.defaultValue().empty())
        return &theOtherOne;

    return &thisOne;
}

const SPLSimpleAnnotation::KeyValuePair* bestPick(
  const SPLSimpleAnnotation::KeyValuePair& thisOne,
  const SPLSimpleAnnotation::KeyValuePair& theOtherOne,
  const SourceLocation& loc1,
  const SourceLocation& loc2)
{
    assert(thisOne.key() == theOtherOne.key());

    // If one OR the other are submission-time values, select the one that is the submission-time value
    if (!thisOne.submissionTimeName().empty()) {
        if (theOtherOne.submissionTimeName().empty())
            return &thisOne;

        // Both are submission-time values.  Validate that they are compatible and pick the best
        return bestSubmissionTime(thisOne, theOtherOne, loc1, loc2);
    }
    if (!theOtherOne.submissionTimeName().empty())
        return &theOtherOne;

    // Neither are submission time.  Pick the best one.
    const string& key = thisOne.key();
    const string& thisValue = thisOne.value();
    const string& theOtherValue = theOtherOne.value();
    if (key == "period" || key == "drainTimeout" || key == "resetTimeout") {
        if (getFloatValue(thisValue) > getFloatValue(theOtherValue))
            return &thisOne;
        return &theOtherOne;
    }
    assert(thisOne.key() == "maxConsecutiveResetAttempts");
    if (getIntegerValue(thisValue) > getIntegerValue(theOtherValue))
        return &thisOne;
    return &theOtherOne;
}

void CCRegion::merge(CCRegion& other)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Merging region " << other.name() << " into region "
                               << this->name(),
           Debug::TraceConsistentCut);

    // There is some non-determinism in the order things are processed because of the use of addresses in underdered sets
    // Try and make the error outputs consistent
    bool useThis = _loc.line() < other.sourceLocation().line();
    const SourceLocation& loc1 = useThis ? _loc : other.sourceLocation();
    const SourceLocation& loc2 = useThis ? other.sourceLocation() : _loc;

    // Update the CC region parameters to reflect the merged set
    if (isOperatorDriven() || other.isOperatorDriven()) {
        SysOut::errorln(SPL_CORE_CC_REGION_INCOMPATIBLE_REGIONS, loc1);
        SysOut::detailsErrorln(SPL_CORE_CC_REGION_INCOMPATIBLE_REGIONS_DETAILS, loc2);
        return;
    }

    const PrimitiveOperatorSet& opsToMerge = other.operators();
    for (PrimitiveOperatorSet::const_iterator it = opsToMerge.begin(), end = opsToMerge.end();
         it != end; ++it) {
        const PrimitiveOperatorInstance& op = **it;
        _operators.insert(&op);
    }
    other.setMergedWith(*this);

    // We cannot merge operator driven regions, so they must both be periodic
    assert(NULL != _period && NULL != other._period);
    _period = bestPick(*_period, *other._period, loc1, loc2);
    _drainTimeout = bestPick(*_drainTimeout, *other._drainTimeout, loc1, loc2);
    _resetTimeout = bestPick(*_resetTimeout, *other._resetTimeout, loc1, loc2);
    _maxConsecutiveResetAttempts =
      bestPick(*_maxConsecutiveResetAttempts, *other._maxConsecutiveResetAttempts, loc1, loc2);
}

class RegionStackEntry
{
  public:
    enum RegionKind
    {
        Cut,
        Oblivious
    };

    virtual ~RegionStackEntry() {}
    const CompositeOperatorInstance& regionComposite() const { return *_regionComposite; }
    virtual CCRegion& region() const = 0;
    virtual RegionKind kind() const = 0;

  protected:
    RegionStackEntry(const CompositeOperatorInstance& composite)
      : _regionComposite(&composite)
    {}

  private:
    const CompositeOperatorInstance* _regionComposite;
};

class CCRegionStackEntry : public RegionStackEntry
{
  public:
    CCRegionStackEntry(CCRegion& region_, const CompositeOperatorInstance& composite)
      : RegionStackEntry(composite)
      , _region(&region_)
    {}
    virtual ~CCRegionStackEntry() {}
    virtual CCRegion& region() const { return *_region; }
    virtual RegionStackEntry::RegionKind kind() const { return RegionStackEntry::Cut; }

  private:
    CCRegion* _region;
};

class CORegionStackEntry : public RegionStackEntry
{
  public:
    CORegionStackEntry(const CompositeOperatorInstance& composite)
      : RegionStackEntry(composite)
    {}
    virtual ~CORegionStackEntry() {}
    virtual CCRegion& region() const
    {
        assert(false);
        return *(CCRegion*)NULL;
    }
    virtual RegionStackEntry::RegionKind kind() const { return RegionStackEntry::Oblivious; }
};

class BuilderState
{
  public:
    BuilderState()
      : _currentRegion(NULL)
      , _jcpOp(NULL)
    {}
    ~BuilderState();

    CCRegion& newRegion(const SPLAnnotation& annot);

    vector<CCRegion*>& regions() { return _ccRegions; }
    const vector<CCRegion*>& regions() const { return _ccRegions; }

    bool inRegion() const { return NULL != _currentRegion; }
    RegionStackEntry& currentRegion()
    {
        assert(NULL != _currentRegion);
        return *_currentRegion;
    }

    void pushCCRegion(CCRegion& region, const CompositeOperatorInstance& composite);
    void pushCORegion(const CompositeOperatorInstance& composite)
    {
        _regionStack.push_back(new CORegionStackEntry(composite));
        _currentRegion = _regionStack.back();
    }
    void popRegion();

    const CompositeOperatorInstance& regionComposite() const
    {
        assert(NULL != _currentRegion);
        return _currentRegion->regionComposite();
    }

    void addStartOperator(const PrimitiveOperatorInstance& op) { _startOperators.insert(&op); }
    const PrimitiveOperatorSet& startOperators() const { return _startOperators; }

    void addMultiRegionInfo(CCInfo& info) { _infoSet.insert(&info); }
    const InfoSet& multiRegionInfos() const { return _infoSet; }

    const CompositeOperatorInstance* jcp() const { return _jcpOp; }
    void setJcp(const CompositeOperatorInstance& op) { _jcpOp = &op; }

  private:
    vector<CCRegion*> _ccRegions;
    vector<RegionStackEntry*> _regionStack;
    RegionStackEntry* _currentRegion;
    PrimitiveOperatorSet _startOperators;
    InfoSet _infoSet;
    const CompositeOperatorInstance* _jcpOp;
};

BuilderState::~BuilderState()
{
    for (vector<CCRegion*>::const_iterator it = _ccRegions.begin(); it != _ccRegions.end(); ++it) {
        delete *it;
    }
}

void BuilderState::pushCCRegion(CCRegion& region, const CompositeOperatorInstance& composite)
{
    _regionStack.push_back(new CCRegionStackEntry(region, composite));
    _currentRegion = _regionStack.back();
}

void BuilderState::popRegion()
{
    assert(_regionStack.size() > 0);
    RegionStackEntry* entry = _regionStack.back();
    _regionStack.pop_back();
    _currentRegion = (_regionStack.size() > 0) ? _regionStack.back() : NULL;
    delete entry;
}

CCRegion& BuilderState::newRegion(const SPLAnnotation& annot)
{
    ostringstream ss;
    ss << "R";
    ss << _ccRegions.size();
    CCRegion* region = new CCRegion(annot, ss.str());
    _ccRegions.push_back(region);
    return *region;
}

class CCInfo
{
  public:
    enum OperatorKind
    {
        Start,
        Oblivious,
        None
    };
    CCInfo(const OperatorGraphNode& node)
      : _node(&node)
      , _kind(None)
    {}
    CCInfo()
      : _node(NULL)
      , _kind(None)
    {}

    uint32_t addRegion(CCRegion& ccRegion_)
    {
        _regions.push_back(&ccRegion_);
        return _regions.size();
    }
    void setMergedRegion(CCRegion& ccRegion_)
    {
        _regions.clear();
        _regions.push_back(&ccRegion_);
    }

    vector<CCRegion*>& regions() { return _regions; }
    const vector<CCRegion*>& regions() const { return _regions; }

    void setIsStartOperator() { _kind = Start; }
    bool isStartOperator() const { return _kind == Start; }
    void setIsObliviousOperator() { _kind = Oblivious; }
    bool isObliviousOperator() const { return _kind == Oblivious; }

    PrimitiveOperatorInstance& primitiveOperator() const
    {
        assert(NULL != _node->getOperatorIR());
        return *_node->getOperatorIR();
    }
    const OperatorGraphNode& graphNode() const { return *_node; }

    bool isInCCRegion() const { return _regions.size() > 0; }
    const CCRegion& ccRegion() const
    {
        assert(_regions.size() == 1);
        return *_regions[0];
    }

  private:
    const OperatorGraphNode* _node;
    OperatorKind _kind;
    vector<CCRegion*> _regions;
};

} // namespace ConsistentCutRegion
} // namespace SPL

using namespace ConsistentCutRegion;

boost::object_pool<ConsistentCutRegion::CCInfo> _infoPool;

ConsistentCutRegionBuilder::ConsistentCutRegionBuilder(const OperatorGraph& ograph)
  : Singleton<ConsistentCutRegionBuilder>(*this)
  , _ograph(ograph)
  , _path(PathSearch::instance())
{}

ConsistentCutRegion::CCInfo& ConsistentCutRegionBuilder::createInfo(
  const PrimitiveOperatorInstance& primitive,
  const OperatorGraphNode& node)
{
    assert(_infoMap.find(&primitive) == _infoMap.end());
    CCInfo* info;
    info = _infoPool.construct(node);
    _infoMap.insert(make_pair(&primitive, info));
    return *info;
}

ConsistentCutRegion::CCInfo& ConsistentCutRegionBuilder::createInfo(
  const PrimitiveOperatorInstance& primitive)
{
    assert(_infoMap.find(&primitive) == _infoMap.end());
    CCInfo* info;
    info = _infoPool.construct();
    _infoMap.insert(make_pair(&primitive, info));
    return *info;
}

ConsistentCutRegion::CCInfo& ConsistentCutRegionBuilder::findInfo(
  const PrimitiveOperatorInstance& primitive) const
{
    InfoMap::const_iterator it = _infoMap.find(&primitive);
    CCInfo* info;
    assert(it != _infoMap.end());
    info = it->second;
    return *info;
}

bool isEnclosedBy(const PrimitiveOperatorInstance& op, const CompositeOperatorInstance& composite)
{
    const CompositeOperatorInstance* owningComposite = op.getCompOperInstance();
    while (NULL != owningComposite) {
        if (owningComposite == &composite)
            return true;
        owningComposite = owningComposite->parent();
    }
    return false;
}

void ConsistentCutRegionBuilder::computeReachabilityGraph(CCRegion& region,
                                                          const OperatorGraphNode& node,
                                                          GraphNodeSet& nodesSeen,
                                                          BuilderState& builderState) const
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Checking " << node.getName(), Debug::TraceConsistentCut);
    // Walk the subgraph rooted in node and bounded by composite
    uint32_t numOutputPorts = node.getNumberOfOutputs();
    for (uint32_t i = 0; i < numOutputPorts; ++i) {
        const Connections& connections = node.getDownstreamConnections(i);
        for (Connections::const_iterator it1 = connections.begin(); it1 != connections.end();
             ++it1) {
            const OperatorGraphNode& downstreamNode = it1->getOperator();
            SPLDBG(__PRETTY_FUNCTION__ << ": Checking downstream connection from operator "
                                       << node.getName() << " to operator "
                                       << downstreamNode.getName(),
                   Debug::TraceConsistentCut);
            // If we've already seen this node, quit recursing
            if (nodesSeen.find(&downstreamNode) != nodesSeen.end())
                continue;
            nodesSeen.insert(&downstreamNode);
            const PrimitiveOperatorInstance* op = downstreamNode.getOperatorIR();
            CCInfo& ccInfo = findInfo(*op);
            // If the downstream operator is a start operator, or is oblivious, stop recursing down this connection
            if (ccInfo.isStartOperator() || ccInfo.isObliviousOperator())
                continue;
            // If this operator is already in the region then we've visited it before and don't need to again
            if (region.operators().find(op) != region.operators().end())
                continue;
            region.addOperator(*op);
            uint32_t numRegions = ccInfo.addRegion(region);
            if (numRegions > 1) {
                // We have an operator in more than one region, so we will need to merge regions.  Track this fact.
                builderState.addMultiRegionInfo(ccInfo);
            }
            computeReachabilityGraph(region, downstreamNode, nodesSeen, builderState);
        }
    }
    return;
}

void ConsistentCutRegionBuilder::visitPrimitive(const PrimitiveOperatorInstance& primitive,
                                                BuilderState& builderState)
{
    const string& kind = primitive.getKind();
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting primitive instance : " << primitive.getNames()[0]
                               << " of kind " << kind,
           Debug::TraceConsistentCut);
    if (kind == "spl.adapter::Import" || kind == "spl.adapter::Export") {
        createInfo(primitive);
    }

    CCInfo& ccInfo = findInfo(primitive);

    const vector<const SPLAnnotation*>& annotations = primitive.annotations();
    for (vector<const SPLAnnotation*>::const_iterator it = annotations.begin();
         it != annotations.end(); ++it) {
        const SPLAnnotation& annot = **it;
        if (annot.tag() == "consistent") {
            SPLDBG(__PRETTY_FUNCTION__ << ": Found consistent annotation: "
                                       << primitive.getNames()[0],
                   Debug::TraceConsistentCut);
            if (kind == "spl.adapter::Import" || kind == "spl.adapter::Export") {
                SysOut::errorln(
                  SPL_CORE_NO_ANNOTATE_OPERATOR(
                    "consistent", kind == "spl.adapter::Import" ? "Import" : "Export"),
                  primitive.getLocation());
                return;
            }
            CCRegion& region = builderState.newRegion(annot);
            builderState.addStartOperator(primitive);
            ccInfo.addRegion(region);
            if (ccInfo.isObliviousOperator()) {
                SysOut::errorln(SPL_CORE_CONSISTENT_AND_AUTONOMOUS, primitive.getLocation());
            }
            ccInfo.setIsStartOperator();
        } else if (annot.tag() == "autonomous") {
            SPLDBG(__PRETTY_FUNCTION__ << ": Found autonomous annotation: "
                                       << primitive.getNames()[0],
                   Debug::TraceConsistentCut);
            if (ccInfo.isStartOperator()) {
                SysOut::errorln(SPL_CORE_CONSISTENT_AND_AUTONOMOUS, primitive.getLocation());
            }
            ccInfo.setIsObliviousOperator();
        }
    }

    // @cutOblivious on a primitive trumps @consistentCut on an enclosing composite
    if (ccInfo.isStartOperator() || ccInfo.isObliviousOperator())
        return;

    if (builderState.inRegion()) {
        if (kind == "spl.adapter::Import" || kind == "spl.adapter::Export") {
            // Export operators are diagnosed later
            if (builderState.currentRegion().kind() == RegionStackEntry::Cut &&
                kind == "spl.adapter::Import") {
                SysOut::errorln(SPL_CORE_OP_CANNOT_LIVE_IN_CC_REGION("Import"),
                                primitive.getLocation());
            }
            return;
        }

        // We are in a CC or CO region defined by a composite.  Find out if this primitive is a start operator in the region
        if (isStartOperator(primitive, builderState)) {
            if (builderState.currentRegion().kind() == RegionStackEntry::Cut) {
                SPLDBG(__PRETTY_FUNCTION__ << ": " << primitive.getNames()[0]
                                           << " is a start operator within region composite "
                                           << builderState.regionComposite().getInstanceName(),
                       Debug::TraceConsistentCut);
                builderState.addStartOperator(primitive);
                ccInfo.addRegion(builderState.currentRegion().region());
                ccInfo.setIsStartOperator();
            } else {
                ccInfo.setIsObliviousOperator();
                SPLDBG(__PRETTY_FUNCTION__ << ": " << primitive.getNames()[0]
                                           << " is an oblivious operator within region composite "
                                           << builderState.regionComposite().getInstanceName(),
                       Debug::TraceConsistentCut);
            }
        }
    }
}

void ConsistentCutRegionBuilder::visitComposite(const CompositeOperatorInstance& composite,
                                                BuilderState& builderState)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting composite instance : " << composite.getInstanceName(),
           Debug::TraceConsistentCut);

    const vector<const SPLAnnotation*>& annotations = composite.annotations();
    // See if we have any CC related annotations
    for (vector<const SPLAnnotation*>::const_iterator it = annotations.begin();
         it != annotations.end(); ++it) {
        const SPLAnnotation& annot = **it;
        if (annot.tag() == "consistent") {
            SPLDBG(__PRETTY_FUNCTION__ << ": Found consistent annotation: "
                                       << composite.getInstanceName(),
                   Debug::TraceConsistentCut);
            if (builderState.inRegion() && &builderState.regionComposite() == &composite) {
                SysOut::errorln(SPL_CORE_CONSISTENT_AND_AUTONOMOUS, composite.getStartLocation());
            }

            CCRegion& region = builderState.newRegion(annot);
            builderState.pushCCRegion(region, composite);
            if (composite.composite().getKind() == "spl.control::JobControlPlane") {
                SysOut::errorln(SPL_CORE_ANNOTATION_NOT_ALLOWED_ON_OP(
                                  annot.tag(), composite.composite().getName()),
                                composite.getStartLocation());
            }
        } else if (annot.tag() == "autonomous") {
            SPLDBG(__PRETTY_FUNCTION__ << ": Found autonomous annotation: "
                                       << composite.getInstanceName(),
                   Debug::TraceConsistentCut);
            if (builderState.inRegion() && &builderState.regionComposite() == &composite) {
                SysOut::errorln(SPL_CORE_CONSISTENT_AND_AUTONOMOUS, composite.getStartLocation());
            }
            builderState.pushCORegion(composite);
            if (composite.composite().getKind() == "spl.control::JobControlPlane") {
                SysOut::errorln(SPL_CORE_ANNOTATION_NOT_ALLOWED_ON_OP(
                                  annot.tag(), composite.composite().getName()),
                                composite.getStartLocation());
            }
        }
    }
    if (composite.composite().getKind() == "spl.control::JobControlPlane") {
        if (builderState.jcp() != NULL) {
            SysOut::errorln(SPL_CORE_TOO_MANY_JCP("JobControlPlane"), composite.getStartLocation());
            SysOut::detailsErrorln(SPL_CORE_TOO_MANY_JCP_DETAILS("JobControlPlane"),
                                   builderState.jcp()->getStartLocation());
        } else {
            builderState.setJcp(composite);
        }
        if (composite.isInParallelRegion()) {
            SysOut::errorln(SPL_CORE_OP_NOT_ALLOWED_IN_PARALLEL_REGION("JobControlPlane"),
                            composite.getStartLocation());
        }
        // Don't look for nested operators in the JobControlPlane composite as it does not participate in a consistent region, as such
    } else {
        // Find any nested regions
        const vector<const CompositeOperatorInstance*> composites =
          composite.getCompositeInstances();
        for (vector<const CompositeOperatorInstance*>::const_iterator it1 = composites.begin();
             it1 != composites.end(); ++it1) {
            const CompositeOperatorInstance& comp = **it1;
            visitComposite(comp, builderState);
        }

        const vector<const PrimitiveOperatorInstance*>& primitives =
          composite.getPrimitiveInstances();
        for (vector<const PrimitiveOperatorInstance*>::const_iterator it2 = primitives.begin();
             it2 != primitives.end(); ++it2) {
            const PrimitiveOperatorInstance& primitive = **it2;
            visitPrimitive(primitive, builderState);
        }
    }

    // This composite defines a CC region, so pop the region as we leave
    if (builderState.inRegion() && &builderState.currentRegion().regionComposite() == &composite)
        builderState.popRegion();
}

bool allUpstreamOperatorsLieOutsideRegionComposite(const OperatorGraphNode& node,
                                                   const CompositeOperatorInstance& regionComposite)
{
    uint32_t numInputPorts = node.getNumberOfInputs();
    for (uint32_t i = 0; i < numInputPorts; ++i) {
        const Connections& connections = node.getUpstreamConnections(i);
        for (Connections::const_iterator it = connections.begin(), end = connections.end();
             it != end; ++it) {
            const OperatorGraphNode& upstreamNode = it->getOperator();
            SPLDBG(__PRETTY_FUNCTION__ << ": checking connection from " << node.getName() << " to "
                                       << upstreamNode.getName(),
                   Debug::TraceConsistentCut);
            const PrimitiveOperatorInstance* op = upstreamNode.getOperatorIR();
            assert(op);
            const CompositeOperatorInstance* owningComposite = op->getCompOperInstance();
            while (NULL != owningComposite) {
                if (owningComposite == &regionComposite) {
                    SPLDBG(__PRETTY_FUNCTION__
                             << ": node " << op->getNames()[0]
                             << " is contained within the CC region defined by composite "
                             << regionComposite.getInstanceName(),
                           Debug::TraceConsistentCut);
                    return false;
                }
                owningComposite = owningComposite->parent();
            }
        }
    }

    return true;
}

bool ConsistentCutRegionBuilder::isStartOperator(const PrimitiveOperatorInstance& op,
                                                 BuilderState& builderState)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Checking " << op.getNames()[0], Debug::TraceConsistentCut);
    const CCInfo& ccInfo = findInfo(op);
    const OperatorGraphNode& node = ccInfo.graphNode();

    // This is a start operator if:
    // - it has no input ports
    // -  all it's incoming connections are from operators outside of the region composite
    uint32_t numInputPorts = node.getNumberOfInputs();
    if (0 == numInputPorts) {
        SPLDBG(__PRETTY_FUNCTION__ << ": node " << node.getName()
                                   << " is a start operator because it has no input ports",
               Debug::TraceConsistentCut);
        return true;
    }

    const CompositeOperatorInstance& regionComposite = builderState.regionComposite();
    if (allUpstreamOperatorsLieOutsideRegionComposite(node, regionComposite)) {
        SPLDBG(__PRETTY_FUNCTION__ << ": node " << node.getName()
                                   << " is a start operator because all operators feeding it lie "
                                      "outside it's containing CC region",
               Debug::TraceConsistentCut);
        return true;
    }

    return false;
}

CCRegion& mergeRegions(CCRegion& regionA, CCRegion& regionB, MergeMap& mergeMap)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": merging regions " << regionA.name() << " and "
                               << regionB.name(),
           Debug::TraceConsistentCut);
    if (&regionA == &regionB)
        return regionA;

    MergeMap::const_iterator it = mergeMap.find(MergedPair(regionA, regionB));
    if (it != mergeMap.end()) {
        SPLDBG(__PRETTY_FUNCTION__ << ": region " << regionA.name()
                                   << " has already been merged with region " << regionB.name()
                                   << " into region " << it->second->name(),
               Debug::TraceConsistentCut);
        return *it->second;
    }

    if (regionA.wasMerged())
        return mergeRegions(regionA.mergedWith(), regionB, mergeMap);
    if (regionB.wasMerged())
        return mergeRegions(regionA, regionB.mergedWith(), mergeMap);

    mergeMap.insert(make_pair(MergedPair(regionA, regionB), &regionA));
    regionA.merge(regionB);
    return regionA;
}

bool ConsistentCutRegionBuilder::allDownstreamOperatorsAreOutsideRegion(
  const OperatorGraphNode& node,
  const CCRegion& region) const
{
    uint32_t numOutputPorts = node.getNumberOfOutputs();
    for (uint32_t i = 0; i < numOutputPorts; ++i) {
        const Connections& connections = node.getDownstreamConnections(i);
        for (Connections::const_iterator it = connections.begin(); it != connections.end(); ++it) {
            const OperatorGraphNode& downstreamNode = it->getOperator();
            const PrimitiveOperatorInstance* op = downstreamNode.getOperatorIR();
            const CCInfo& ccInfo = findInfo(*op);
            if (ccInfo.isInCCRegion()) {
                const CCRegion& downstreamOperatorRegion = ccInfo.ccRegion();
                if (region == downstreamOperatorRegion)
                    return false;
            }
        }
    }

    return true;
}

bool ConsistentCutRegionBuilder::isEndOperator(const CCInfo& ccInfo) const
{
    assert(ccInfo.regions().size() == 1);
    const CCRegion& containingRegion = ccInfo.regions()[0]->mergedRegion();

    // We may also be an end operator if all our downstream operators are outside the CC region containing this operator
    const OperatorGraphNode& node = ccInfo.graphNode();

    return allDownstreamOperatorsAreOutsideRegion(node, containingRegion);
}

void ConsistentCutRegionBuilder::computeObliviousPortSet(const CCInfo& ccInfo,
                                                         const CCRegion& region,
                                                         vector<uint32_t>& obliviousPorts) const
{
    // An input port is oblivious if all the connections to a given input port come from outside the CC region
    const OperatorGraphNode& node = ccInfo.graphNode();
    for (uint32_t i = 0, end = node.getNumberOfInputs(); i < end; ++i) {
        bool allConnectionsAreRegionExternal = true;
        const Connections& connections = node.getUpstreamConnections(i);
        for (Connections::const_iterator it = connections.begin(), itEnd = connections.end();
             it != itEnd; ++it) {
            const OperatorGraphNode& upstreamNode = it->getOperator();
            const PrimitiveOperatorInstance* op = upstreamNode.getOperatorIR();
            const CCInfo& upstreamInfo = findInfo(*op);
            if (upstreamInfo.isInCCRegion()) {
                const CCRegion& upstreamOpRegion = upstreamInfo.ccRegion();
                if (region == upstreamOpRegion) {
                    allConnectionsAreRegionExternal = false;
                    break;
                }
            }
        }

        if (allConnectionsAreRegionExternal) {
            SPLDBG(__PRETTY_FUNCTION__ << ": Port " << i << " of operator " << node.getName()
                                       << " is oblivious",
                   Debug::TraceConsistentCut);
            obliviousPorts.push_back(i);
        }
    }
}

bool myCCVectorSort(const CCInfo* one, const CCInfo* two)
{
    return one->graphNode().getNode() < two->graphNode().getNode();
}

void ConsistentCutRegionBuilder::build()
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Building CC regions", Debug::TraceConsistentCut);

    BuilderState builderState;

    // Walk the graph and build a mapping from primitive operator to node
    for (uint32_t i = 0, numNodes = _ograph.numNodes(); i < numNodes; ++i) {
        const OperatorGraphNode& node = _ograph.getNode(i);
        const PrimitiveOperatorInstance* op = node.getOperatorIR();
        assert(op);
        createInfo(*op, node);
    }

    const CompositeDefinition* mainComposite = FrontEndDriver::instance().getMainComposite();
    if (NULL == mainComposite)
        return;

    const CompositeOperatorInstance* mainCompositeInstance = mainComposite->getImplicitInstance();
    assert(NULL != mainCompositeInstance);

    // Find all the CC regions
    visitComposite(*mainCompositeInstance, builderState);

    // We now have a set of start operators.  Compute the reachability graph for each.
    const PrimitiveOperatorSet& startOperators = builderState.startOperators();
    for (PrimitiveOperatorSet::const_iterator it = startOperators.begin(),
                                              end = startOperators.end();
         it != end; ++it) {
        const PrimitiveOperatorInstance& primitive = **it;
        CCInfo& ccInfo = findInfo(primitive);
        const OperatorGraphNode& node = ccInfo.graphNode();
        assert(ccInfo.regions().size() > 0);
        CCRegion& region = *ccInfo.regions()[0];
        region.addOperator(primitive);
        GraphNodeSet nodesSeen;
        nodesSeen.insert(&node);
        computeReachabilityGraph(region, node, nodesSeen, builderState);
    }

    const vector<CCRegion*>& regions = builderState.regions();
    if (regions.size() == 0) {
        return;
    }

    if (indebug(Debug::TraceConsistentCut)) {
        for (vector<CCRegion*>::const_iterator it = regions.begin(), end = regions.end(); it != end;
             ++it) {
            CCRegion& region = **it;
            cerr << "Region " << region.name() << " has operators: ";
            const PrimitiveOperatorSet& operators = region.operators();
            for (PrimitiveOperatorSet::const_iterator it1 = operators.begin(),
                                                      end1 = operators.end();
                 it1 != end1; ++it1) {
                const PrimitiveOperatorInstance& op = **it1;
                cerr << op.getNames()[0] << " ";
            }
            cerr << endl;
        }
    }

    // See if there are any regions that need to be merged
    const InfoSet& regionInfosNeedingMerge = builderState.multiRegionInfos();
    if (regionInfosNeedingMerge.size() > 0) {
        MergeMap mergeMap;
        SPLDBG(__PRETTY_FUNCTION__ << ": There are regions that need to be merged",
               Debug::TraceConsistentCut);
        for (InfoSet::const_iterator it = regionInfosNeedingMerge.begin(),
                                     end = regionInfosNeedingMerge.end();
             it != end; ++it) {
            CCInfo& ccInfo = **it;
            SPLDBG(__PRETTY_FUNCTION__ << ": merging regions that have operator "
                                       << ccInfo.graphNode().getName() << " in common",
                   Debug::TraceConsistentCut);
            vector<CCRegion*>& regionsToMerge = ccInfo.regions();
            assert(regionsToMerge.size() > 1);
            CCRegion& regionA = *regionsToMerge[0];
            CCRegion& regionB = *regionsToMerge[1];
            CCRegion* mergedRegion = &mergeRegions(regionA, regionB, mergeMap);
            for (int32 i = 2, size = regionsToMerge.size(); i < size; ++i) {
                mergedRegion = &mergeRegions(*mergedRegion, *regionsToMerge[i], mergeMap);
            }
            ccInfo.setMergedRegion(*mergedRegion);
        }
    }

    // Synthesize the region annotations on the main composite
    synthesizeRegionAnnotations(*mainCompositeInstance, regions);

    // Walk all the regions, synthesize annotations and do some diagnosing
    uint32_t regionIndex = 0;
    for (vector<CCRegion*>::const_iterator it = regions.begin(), end = regions.end(); it != end;
         ++it) {
        const CCRegion& region = **it;
        // Skip this region if it was merged into another region
        if (region.wasMerged())
            continue;
        const PrimitiveOperatorSet& operators = region.operators();
        vector<const CCInfo*> startOperatorsSeen;
        for (PrimitiveOperatorSet::const_iterator it1 = operators.begin(), end1 = operators.end();
             it1 != end1; ++it1) {
            const PrimitiveOperatorInstance& op = **it1;
            const CCInfo& ccInfo = findInfo(op);
            assert(ccInfo.regions().size() == 1);
            // We have to do a rather ugly mapping to avoid casting away const here
            PrimitiveOperatorInstance& oper = ccInfo.primitiveOperator();
            vector<uint32_t> obliviousPorts;
            computeObliviousPortSet(ccInfo, region, obliviousPorts);
            SPLSimpleAnnotation& annot = synthesizeRegionEntryAnnotation(
              regionIndex, oper, obliviousPorts, region.sourceLocation());

            // Verify that this operator can participate in a consistent region
            validateOperator(ccInfo);

            if (ccInfo.isStartOperator()) {
                // If this start operator is a Custom operator, then the topology is invalid
                if (oper.getKind() == "spl.utility::Custom") {
                    SysOut::errorln(SPL_CORE_CUSTOM_CANT_START_CC_REGION(oper.getNames()[0]),
                                    oper.getLocation());
                }
                addIsStartOperator(annot, region, oper);
                startOperatorsSeen.push_back(&ccInfo);
            }
            if (isEndOperator(ccInfo)) {
                addIsEndOperator(annot, oper);
            }
        }
        if (region.isOperatorDriven()) {
            if (startOperatorsSeen.size() > 1) {
                // Order in the vector is non-deterministic due to processing order
                std::sort(startOperatorsSeen.begin(), startOperatorsSeen.end(), myCCVectorSort);
                ostringstream ss;
                ss << startOperatorsSeen[0]->graphNode().getName();
                for (uint32_t i = 1, iEnd = startOperatorsSeen.size(); i < iEnd; ++i) {
                    ss << ", " << startOperatorsSeen[i]->graphNode().getName();
                }
                SysOut::errorln(SPL_CORE_CC_REGION_TOO_MANY_OPER_DRIVEN_START(ss.str()),
                                region.sourceLocation());
            } else {
                // Check if the start operator is in a parallel region.  If so, and if the width is specified as > 1, emit an error
                assert(startOperatorsSeen.size() == 1);

                for (vector<const CCInfo*>::const_iterator it2 = startOperatorsSeen.begin(),
                                                           it2End = startOperatorsSeen.end();
                     it2 != it2End; ++it2) {
                    const OperatorGraphNode& node = (*it2)->graphNode();
                    const PrimitiveOperatorInstance& op = *node.getOperatorIR();
                    if (op.isInParallelRegion()) {
                        string parallelRegionName = op.parallelRegionName();
                        GraphNodeSet nodesSeen;
                        nodesSeen.insert(&node);
                        if (replicationWouldTriggerAMerge(node, nodesSeen, parallelRegionName)) {
                            const SPLParallelAnnotation& pAnnot = op.getParallelAnnotation();
                            if (pAnnot.submissionTimeName().empty()) {
                                // A width was specified...If it is greater than 1, emit an error
                                if (pAnnot.width() > 1) {
                                    SysOut::errorln(
                                      SPL_CORE_CC_REGION_START_OP_IS_PARALLEL(op.getNames()[0]),
                                      op.getLocation());
                                }
                            } else {
                                // A submission-time value is expected.  Emit an warning saying it will fail at submission-time with any value other than one
                                SysOut::warnln(
                                  SPL_CORE_CC_REGION_START_OP_IS_PARALLEL_SUBTIME(op.getNames()[0]),
                                  op.getLocation());
                            }
                        }
                    }
                }
            }
        }
        checkForFeedbackLoops(region);
        ++regionIndex;
    }
}

bool ConsistentCutRegionBuilder::replicationWouldTriggerAMerge(
  const OperatorGraphNode& node,
  GraphNodeSet& nodesSeen,
  const string& parallelRegionName) const
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Checking start operator " << node.getName()
                               << " in parallel region " << parallelRegionName,
           Debug::TraceConsistentCut);

    // A merge would be triggered if the flow graph from any output port of the start operator leaves the parallel region and
    // enters a non-oblivious input port
    uint32_t numOutputPorts = node.getNumberOfOutputs();
    for (uint32_t i = 0; i < numOutputPorts; ++i) {
        const Connections& connections = node.getDownstreamConnections(i);
        for (Connections::const_iterator it1 = connections.begin(); it1 != connections.end();
             ++it1) {
            const OperatorGraphNode& downstreamNode = it1->getOperator();
            SPLDBG(__PRETTY_FUNCTION__ << ": Checking downstream connection from operator "
                                       << node.getName() << " to operator "
                                       << downstreamNode.getName(),
                   Debug::TraceConsistentCut);
            // If we've already seen this node, quit recursing
            if (nodesSeen.find(&downstreamNode) != nodesSeen.end())
                continue;
            const PrimitiveOperatorInstance* op = downstreamNode.getOperatorIR();
            const CCInfo& ccInfo = findInfo(*op);
            // If the downstream operator is a start operator, or is oblivious, stop recursing down this connection
            if (ccInfo.isStartOperator() || ccInfo.isObliviousOperator())
                continue;
            // We are still in the CC region.  Do we leave the parallel region?  If yes, we would trigger a merge.
            if (!op->isInParallelRegion() ||
                (op->isInParallelRegion() && op->parallelRegionName() != parallelRegionName))
                return true;
            // Recurse down this path
            if (replicationWouldTriggerAMerge(downstreamNode, nodesSeen, parallelRegionName))
                return true;
        }
    }
    return false;
}

void ConsistentCutRegionBuilder::validateOperator(const CCInfo& ccInfo)
{
    // If the operator has output ports, and has only control input ports, it cannot participate in a consistent region
    const OperatorGraphNode& node = ccInfo.graphNode();
    const PrimitiveOperatorInstance& oper = *node.getOperatorIR();

    // Check if this operator has a config checkpoint
    const Config* c = oper.findConfig(Config::Checkpoint);
    if (NULL != c) {
        SysOut::errorln(SPL_CORE_CONFIG_CHECKPOINT_IN_CONSISTENT_REGION(node.getName()),
                        oper.getLocation());
    }

    // If an input port has a mix of both consistent and anonymous streams it is an invalid
    if (node.getNumberOfInputs() != 0) {
        uint32_t numInputPorts = node.getNumberOfInputs();
        for (uint32_t i = 0; i < numInputPorts; ++i) {
            bool consistentUpstreamOperatorSeen = false;
            bool anonymousUpstreamOperatorSeen = false;
            const Connections& connections = node.getUpstreamConnections(i);
            for (Connections::const_iterator it = connections.begin(), end = connections.end();
                 it != end; ++it) {
                const OperatorGraphNode& upstreamNode = it->getOperator();
                const PrimitiveOperatorInstance* op = upstreamNode.getOperatorIR();
                assert(NULL != op);
                const CCInfo& inf = findInfo(*op);
                if (inf.isInCCRegion()) {
                    consistentUpstreamOperatorSeen = true;
                } else {
                    anonymousUpstreamOperatorSeen = true;
                }
            }
            if (consistentUpstreamOperatorSeen && anonymousUpstreamOperatorSeen) {
                SysOut::errorln(SPL_CORE_MIXED_ANONYMOUS_CONSISTENT_INPUT_PORT(node.getName(), i),
                                oper.getLocation());
            }
        }
    }

    // check if there are any exports off this operator that are not marked anonymous
    uint32_t numOutputs = node.getNumberOfOutputs();
    for (uint32_t i = 0; i < numOutputs; i++) {
        if (!node.isOutputPortExported(i))
            continue;
        const vector<const ExportSpec*>& exports = node.getExportSpecs(i);
        for (vector<const ExportSpec*>::const_iterator it = exports.begin(), itEnd = exports.end();
             it != itEnd; ++it) {
            const ExportSpec& spec = **it;
            const PrimitiveOperatorInstance& op = spec.getOperatorIR();
            const CCInfo& inf = findInfo(op);
            if (!inf.isObliviousOperator()) {
                SysOut::errorln(SPL_CORE_OP_CANNOT_LIVE_IN_CC_REGION("Export"), op.getLocation());
            }
        }
    }

    if (node.getNumberOfInputs() != 0 && node.getNumberOfOutputs() != 0) {
        const string& kind = oper.getKind();
        const Operator::OperatorModel* om = _path.findOperatorModel(kind, NULL);
        assert(om != NULL);
        const Operator::InputPorts& inputPorts = om->getInputPorts();
        bool isCustom = kind == "spl.utility::Custom";
        for (uint32_t i = 0, iEnd = node.getNumberOfInputs(); i < iEnd; ++i) {
            if (isCustom) {
                PrimitiveOperatorInstance::Port const& iport = oper.getInputPort(i);
                if (iport.getSubmitCalled()) {
                    return;
                }
            } else {
                const Operator::InputPort& p = inputPorts.getPortAt(i);
                if (!p.isControlPort()) {
                    return;
                }
            }
        }
        // A start operator can have only control input ports
        if (ccInfo.isStartOperator()) {
            return;
        }
        SysOut::errorln(SPL_CORE_CC_REGION_CONTROL_PORTS_ONLY(node.getName()), oper.getLocation());
    }
}

void ConsistentCutRegionBuilder::checkForFeedbackLoops(const ConsistentCutRegion::CCRegion& region)
{
    const PrimitiveOperatorSet& regionOperators = region.operators();
    vector<uint32_t> sortedOperators;
    for (PrimitiveOperatorSet::const_iterator it = regionOperators.begin(),
                                              itEnd = regionOperators.end();
         it != itEnd; ++it) {
        const PrimitiveOperatorInstance& oper = **it;
        const CCInfo& ccInfo = findInfo(oper);
        const OperatorGraphNode& node = ccInfo.graphNode();
        uint32_t opNum = node.getNode();
        sortedOperators.push_back(opNum);
    }
    // Sort the list of operators to get a deterministic output
    std::sort(sortedOperators.begin(), sortedOperators.end());
    unordered_set<uint32_t> visited;
    vector<uint32_t> visitPath;
    for (vector<uint32_t>::const_iterator it1 = sortedOperators.begin(),
                                          it1End = sortedOperators.end();
         it1 != it1End; ++it1) {
        uint32_t opNum = *it1;
        if (visited.count(opNum) > 0)
            continue;
        const OperatorGraphNode& node = _ograph.getNode(opNum);
        if (node.getNumberOfInputs() == 0 || node.getNumberOfOutputs() == 0)
            continue;
        recursivelyCheckOperator(node, region, visited, visitPath);
    }
}

void ConsistentCutRegionBuilder::recursivelyCheckOperator(const OperatorGraphNode& op,
                                                          const CCRegion& region,
                                                          unordered_set<uint32_t>& visited,
                                                          vector<uint32_t>& visitPath)
{
    const PrimitiveOperatorSet& regionOperators = region.operators();

    // If this operator is not in the region, return
    if (regionOperators.find(op.getOperatorIR()) == regionOperators.end())
        return;

    uint32_t opNum = op.getNode();
    // Have we seen this node before?
    if (visited.count(opNum) > 0) {
        // This is a potential problem
        // Check to see if we are on the visit path, or this was already handled
        int32_t i;
        for (i = visitPath.size() - 1; i >= 0; i--) {
            if (visitPath[i] == opNum) {
                // yes, this is a loop
                std::ostringstream path;
                bool embodiesStartOperator = false;
                for (int32_t ui = visitPath.size(); i < ui; i++) {
                    const OperatorGraphNode& o = _ograph.getNode(visitPath[i]);
                    path << o.getName() << "->";
                    const CCInfo& ccInfo = findInfo(*o.getOperatorIR());
                    if (ccInfo.isStartOperator()) {
                        embodiesStartOperator = true;
                    }
                }
                path << op.getName();
                const SourceLocation& loc = op.getOperatorIR()->getLocation();
                if (embodiesStartOperator) {
                    SysOut::errorln(SPL_CORE_CC_REGION_START_FEEDBACK_LOOP(path.str()), loc);
                } else {
                    SysOut::errorln(SPL_CORE_CC_REGION_FEEDBACK_LOOP(path.str()), loc);
                }
                return;
            }
        }

        // No, we just have previously visited this
        return;
    }

    // Been here
    visited.insert(opNum);
    visitPath.push_back(opNum);

    // Walk the outputs for this operator
    uint32_t numOutputs = op.getNumberOfOutputs();
    for (uint32_t i = 0; i < numOutputs; i++) {
        const vector<OperatorGraphNode::ConnectionInfo>& outputs = op.getDownstreamConnections(i);
        uint32_t numConns = outputs.size();
        for (uint32_t j = 0; j < numConns; j++) {
            OperatorGraphNode const& dop = outputs[j].getOperator();
            PrimitiveOperatorInstance const* ir = dop.getOperatorIR();
            assert(ir);
            const CCInfo& ccInfo = findInfo(*ir);
            uint32_t dpi = outputs[j].getPort(); // iport index

            // If this is a start operator, look through control ports
            if (!ccInfo.isStartOperator()) {
                // Check the input port to see if it is a control port
                const string& kind = ir->getKind();
                const Operator::OperatorModel* om = _path.findOperatorModel(kind, NULL);
                assert(om != NULL);
                const Operator::InputPorts& inputPorts = om->getInputPorts();
                // Checking if the code is valid against the operator model is done later and so we need to make
                // sure we are not exceeding the number of input ports here.
                if (dpi < inputPorts.getMaxNumberOfPorts()) {
                    const Operator::InputPort& p = inputPorts.getPortAt(dpi);

                    // Is there logic associated with this?
                    PrimitiveOperatorInstance::Port const& iport = ir->getInputPort(dpi);

                    if (p.isControlPort()) {
                        continue; // A port marked as contol is expecting a feedback loop
                    } else if (kind == "spl.utility::Custom" && !iport.getSubmitCalled()) {
                        // Custom has no way to be marked as a control port.  Just see
                        // if a submit was done from this port.  If not, then this is a control port
                        continue;
                    }
                }
            }

            // Not safe - continue checking
            recursivelyCheckOperator(dop, region, visited, visitPath);
        }
    }
    visitPath.pop_back();
}

void ConsistentCutRegionBuilder::synthesizeRegionAnnotations(
  const CompositeOperatorInstance& composite,
  const vector<CCRegion*>& regions) const
{
    uint32_t regionIndex = 0;
    for (vector<CCRegion*>::const_iterator it = regions.begin(), itEnd = regions.end(); it != itEnd;
         ++it) {
        const CCRegion& region = **it;
        // Skip this region if it was merged into another region
        if (region.wasMerged())
            continue;
        vector<SPLSimpleAnnotation::KeyValuePair*> values;

        const RootTyp& intType = TypeFactory::instance().primitiveT("int64");
        {
            ostringstream os;
            os << regionIndex;
            values.push_back(new SPLSimpleAnnotation::KeyValuePair(
              "index", os.str(), intType, SourceLocation::invalidSourceLocation));
        }

        values.push_back(new SPLSimpleAnnotation::KeyValuePair(region.trigger()));

        // We don't need period if it is operator driven
        if (!region.isOperatorDriven()) {
            values.push_back(new SPLSimpleAnnotation::KeyValuePair(region.period()));
        }

        values.push_back(new SPLSimpleAnnotation::KeyValuePair(region.drainTimeout()));
        values.push_back(new SPLSimpleAnnotation::KeyValuePair(region.resetTimeout()));
        values.push_back(
          new SPLSimpleAnnotation::KeyValuePair(region.maxConsecutiveResetAttempts()));

        SPLSimpleAnnotation& annot = AnnotationFactory::instance().createSimpleAnnotation(
          "consistentRegion", values, region.sourceLocation());
        composite.addAnnotation(annot);
        ++regionIndex;
    }
}

void ConsistentCutRegionBuilder::addIsStartOperator(SPLSimpleAnnotation& annot,
                                                    const CCRegion& region,
                                                    PrimitiveOperatorInstance& op) const
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Building start annotation for node " << op.getNames()[0],
           Debug::TraceConsistentCut);

    vector<SPLSimpleAnnotation::KeyValuePair*>& values = annot.values();
    const RootTyp& bType = TypeFactory::instance().primitiveT("boolean");
    values.push_back(new SPLSimpleAnnotation::KeyValuePair("isStartOfRegion", "true", bType,
                                                           SourceLocation::invalidSourceLocation));
    if (region.isOperatorDriven()) {
        values.push_back(new SPLSimpleAnnotation::KeyValuePair(
          "isTriggerOperator", "true", bType, SourceLocation::invalidSourceLocation));
    }
}

SPLSimpleAnnotation& ConsistentCutRegionBuilder::synthesizeRegionEntryAnnotation(
  uint32_t regionIndex,
  PrimitiveOperatorInstance& op,
  const std::vector<uint32_t>& obliviousPorts,
  const SourceLocation& loc) const
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Building annotation for node " << op.getNames()[0]
                               << " in region " << regionIndex,
           Debug::TraceConsistentCut);
    vector<SPLSimpleAnnotation::KeyValuePair*> values;
    {
        const RootTyp& iType = TypeFactory::instance().primitiveT("int32");
        ostringstream os;
        os << regionIndex;
        values.push_back(new SPLSimpleAnnotation::KeyValuePair(
          "index", os.str(), iType, SourceLocation::invalidSourceLocation));
    }
    const RootTyp& uiType = TypeFactory::instance().primitiveT("uint32");
    for (vector<uint32_t>::const_iterator it = obliviousPorts.begin(), itEnd = obliviousPorts.end();
         it != itEnd; ++it) {
        uint32_t v = *it;
        ostringstream os;
        os << v;
        values.push_back(new SPLSimpleAnnotation::KeyValuePair(
          "autonomousInputPort", os.str(), uiType, SourceLocation::invalidSourceLocation));
    }
    SPLSimpleAnnotation& annot =
      AnnotationFactory::instance().createSimpleAnnotation("consistentRegionEntry", values, loc);
    op.addAnnotation(annot);
    return annot;
}

void ConsistentCutRegionBuilder::addIsEndOperator(SPLSimpleAnnotation& annot,
                                                  PrimitiveOperatorInstance& op) const
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Building end annotation for node " << op.getNames()[0],
           Debug::TraceConsistentCut);
    vector<SPLSimpleAnnotation::KeyValuePair*>& values = annot.values();
    const RootTyp& bType = TypeFactory::instance().primitiveT("boolean");
    values.push_back(new SPLSimpleAnnotation::KeyValuePair("isEndOfRegion", "true", bType,
                                                           SourceLocation::invalidSourceLocation));
}
