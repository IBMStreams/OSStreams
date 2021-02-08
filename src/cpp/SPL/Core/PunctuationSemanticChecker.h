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

#ifndef SPL_PUNCTUATION_CHECKER_H
#define SPL_PUNCTUATION_CHECKER_H

#include <SPL/Core/OperatorGraph.h>
#include <iostream>

namespace SPL {

class OperatorGraph;
class PathSearch;
class OperatorGraphNode;
namespace Operator {
class OperatorModel;
};

class PunctuationSemanticChecker
{
  public:
    PunctuationSemanticChecker();
    void check(const OperatorGraph& graph);

  private:
    const Operator::OperatorModel& findOperatorModel(const OperatorGraphNode& gn);
    bool portIsExpectingPunctuation(const OperatorGraphNode& gn, uint32_t port);
    PathSearch& _path;
};

};

#endif /* SPL_PUNCTUATION_CHECKER_H */
