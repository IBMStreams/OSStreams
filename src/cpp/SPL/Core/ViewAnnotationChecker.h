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

#ifndef SPL_VIEW_ANNOTATION_CHECKER
#define SPL_VIEW_ANNOTATION_CHECKER

namespace SPL {

class CompositeOperatorInstance;
class OperatorGraph;

class ViewAnnotationChecker
{
  public:
    ViewAnnotationChecker() {}
    void check(const OperatorGraph& graph);

  private:
    void checkForViewAnnotationsOnComposites(const CompositeOperatorInstance& comp);
    void checkViewAnnotationsOnPrimitives(const OperatorGraph& graph);
};

} // namespace SPL
#endif // SPL_VIEW_ANNOTATION_CHECKER
