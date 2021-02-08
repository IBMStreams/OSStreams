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

#ifndef SPL_CONSISTENT_CUT_H
#define SPL_CONSISTENT_CUT_H

#include <SPL/Core/OperatorGraph.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <UTILS/HashMapHelpers.h>

#include <vector>

namespace SPL {

class CompilationState;
class CompositeOperatorInstance;
class OperatorGraph;
class SPLAnnotation;
class SPLSimpleAnnotation;
class SourceLocation;
class PathSearch;
class PrimitiveOperatorInstance;

namespace ConsistentCutRegion {
class BuilderState;
class CCInfo;
class CCRegion;
} // namespace ConsistentCutRegion

typedef std::tr1::unordered_set<const OperatorGraphNode*> GraphNodeSet;
typedef std::tr1::unordered_set<const PrimitiveOperatorInstance*> PrimitiveOperatorSet;
typedef std::tr1::unordered_set<const CompositeOperatorInstance*> CompositeOperatorSet;
typedef std::tr1::unordered_set<ConsistentCutRegion::CCInfo*> InfoSet;
typedef std::vector<OperatorGraphNode::ConnectionInfo> Connections;

class ConsistentCutRegionBuilder : public Singleton<ConsistentCutRegionBuilder>
{
  public:
    ConsistentCutRegionBuilder(const OperatorGraph& ograph);
    void build();

  private:
    ConsistentCutRegion::CCInfo& createInfo(const PrimitiveOperatorInstance& primitive,
                                            const OperatorGraphNode& node);
    ConsistentCutRegion::CCInfo& findInfo(const PrimitiveOperatorInstance& primitive) const;
    ConsistentCutRegion::CCInfo& createInfo(const PrimitiveOperatorInstance& primitive);

    void visitPrimitive(const PrimitiveOperatorInstance& primitive,
                        ConsistentCutRegion::BuilderState& builderState);
    void visitComposite(const CompositeOperatorInstance& composite,
                        ConsistentCutRegion::BuilderState& builderState);
    void computeReachabilityGraph(ConsistentCutRegion::CCRegion& region,
                                  const OperatorGraphNode& node,
                                  GraphNodeSet& nodesSeen,
                                  ConsistentCutRegion::BuilderState& builderState) const;
    bool isStartOperator(const PrimitiveOperatorInstance& node,
                         ConsistentCutRegion::BuilderState& builderState);
    bool isEndOperator(const ConsistentCutRegion::CCInfo& ccInfo) const;
    bool allDownstreamOperatorsAreOutsideRegion(const OperatorGraphNode& node,
                                                const ConsistentCutRegion::CCRegion& region) const;
    void computeObliviousPortSet(const ConsistentCutRegion::CCInfo& ccInfo,
                                 const ConsistentCutRegion::CCRegion& region,
                                 std::vector<uint32_t>& obliviousPorts) const;

    void synthesizeRegionAnnotations(
      const CompositeOperatorInstance& composite,
      const std::vector<ConsistentCutRegion::CCRegion*>& regions) const;
    void addIsStartOperator(SPLSimpleAnnotation& annot,
                            const ConsistentCutRegion::CCRegion& region,
                            PrimitiveOperatorInstance& op) const;
    void addIsEndOperator(SPLSimpleAnnotation& annot, PrimitiveOperatorInstance& op) const;
    SPLSimpleAnnotation& synthesizeRegionEntryAnnotation(
      uint32_t regionIndex,
      PrimitiveOperatorInstance& op,
      const std::vector<uint32_t>& obliviousPorts,
      const SourceLocation& loc) const;
    void checkForFeedbackLoops(const ConsistentCutRegion::CCRegion& region);
    void recursivelyCheckOperator(const OperatorGraphNode& op,
                                  const ConsistentCutRegion::CCRegion& region,
                                  std::tr1::unordered_set<uint32_t>& visited,
                                  std::vector<uint32_t>& visitPath);
    void validateOperator(const ConsistentCutRegion::CCInfo& ccInfo);
    bool replicationWouldTriggerAMerge(const OperatorGraphNode& node,
                                       GraphNodeSet& nodesSeen,
                                       const std::string& parallelRegionName) const;

    typedef std::tr1::unordered_map<const PrimitiveOperatorInstance*, ConsistentCutRegion::CCInfo*>
      InfoMap;
    const OperatorGraph& _ograph;
    PathSearch& _path;
    InfoMap _infoMap;
};

} // namespace SPL

#endif // SPL_CONSISTENT_CUT_H
