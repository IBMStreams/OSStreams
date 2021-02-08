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

#ifndef SPL_PARALLEL_REGION_CHECKER_H
#define SPL_PARALLEL_REGION_CHECKER_H

#include <stdint.h>
namespace SPL {

namespace Operator {
class OperatorModel;
};

class CompilationState;
class CompositeOperatorInstance;
class OperatorGraph;
class OperatorGraphNode;
class PathSearch;

class ParallelRegionChecker
{
  public:
    ParallelRegionChecker();
    void check(const OperatorGraph& graph);

  private:
    void checkForImportExport(const CompositeOperatorInstance& composite);
    void checkImportExportOperators(const OperatorGraph& graph);
    void checkForPunctExpectingPortsDownstreamFromParallelRegions(const OperatorGraph& graph);
    bool portIsExpectingPunctuation(const OperatorGraphNode& gn, uint32_t port);
    const Operator::OperatorModel& findOperatorModel(const OperatorGraphNode& gn);

    PathSearch& _path;
};

} // namespace SPL

#endif // SPL_PARALLEL_REGION_CHECKER_H
