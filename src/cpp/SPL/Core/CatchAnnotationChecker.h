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

#ifndef SPL_CATCH_ANNOTATION_CHECKER
#define SPL_CATCH_ANNOTATION_CHECKER

namespace SPL {

class CompositeOperatorInstance;
class OperatorGraph;
class PrimitiveOperatorInstance;
class SPLSimpleAnnotation;

class CatchAnnotationChecker
{
  public:
    CatchAnnotationChecker() {}
    void check(const OperatorGraph& graph);

  private:
    void visitComposite(const CompositeOperatorInstance& composite);
    void propagateAnnotation(const CompositeOperatorInstance& composite,
                             const SPLSimpleAnnotation& annot);
    void propagateAnnotation(const PrimitiveOperatorInstance& primitive,
                             const SPLSimpleAnnotation& annot);
    bool visitPrimitive(const PrimitiveOperatorInstance& primitive);
};

} // namespace SPL

#endif // SPL_CATCH_ANNOTATION_CHECKER
