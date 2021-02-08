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

#ifndef SPL_FEEDBACK_LOOP_DETECTOR_H
#define SPL_FEEDBACK_LOOP_DETECTOR_H

#include <stdint.h>
#include <vector>

#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/HashMapHelpers.h>

namespace SPL {
class OperatorGraph;
class PathSearch;

class DLL_PUBLIC FeedbackLoopDetector
{
  public:
    /// Contructor - nothing interesting done here
    /// @param opgraph      Collection of all operators
    FeedbackLoopDetector(OperatorGraph& opgraph);

    /// Look for fused non-control feedback loops and generate warnings
    void checkForLoops();

  private:
    void recursivelyCheckOperator(uint32_t opNum);

    OperatorGraph& _opgraph;
    PathSearch& _path;
    std::tr1::unordered_set<uint32_t> _visited;
    std::vector<uint32_t> _visitPath;
};
};

#endif /* SPL_FEEDBACK_LOOP_DETECTOR_H */
