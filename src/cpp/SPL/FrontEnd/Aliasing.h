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

#ifndef SPL_FRONT_END_ALIASING_H
#define SPL_FRONT_END_ALIASING_H

#include <antlr3interfaces.h>

namespace SPL {
typedef ANTLR3_BASE_TREE_struct AstNode;
class InverseAst;

class Aliasing
{
  public:
    enum Value
    {
        DISJOINT,
        PREFIX_OF,
        SUFFIX_OF,
        ALIASED
    };

  private:
    Aliasing();
    Value _value;

  public:
    /// Construct an aliasing result with the specified value.
    Aliasing(Value v)
      : _value(v)
    {}

    /// Find out the aliasing between the two expression ASTs.
    Aliasing(AstNode& ast1, AstNode& ast2);

    /// Find the longest paths result1 and result2 such that result1 is a suffix of origin1, result2
    /// is a suffix of origin2, and result1 and result2 are aliases. Set both to NULL if the roots
    /// are not aliases.
    static void findLongestAliasedSuffix(AstNode& origin1,
                                         AstNode& origin2,
                                         InverseAst& iast1,
                                         InverseAst& iast2,
                                         AstNode** result1,
                                         AstNode** result2);

    /// Do the two expressions refer to any overlapping data?
    static bool isAccessTo(AstNode& accessedAst, AstNode& otherAst);

    /// Does the data referenced by 'accessedAst' subsume the data referenced by 'otherAst'?
    static bool isEntireAccessTo(AstNode& accessedAst, AstNode& otherAst);

    /// If 'e1' and 'e2' are ASTs for access paths in compound data types,
    /// is the suffix (i.e. the AST root) of the two access paths the same?
    static bool isPathSameSuffix(AstNode& e1, AstNode& e2);

    /// Assuming 'child' is an access path into compound data, is
    /// 'parent' a suffix that accesses a component of that data?
    static bool isPathSuffix(AstNode& child, AstNode* parent);

    /// If this expression consistutes a path of attribute accesses or subscripts, find its starting
    /// point.
    static AstNode& pathOrigin(AstNode& expr);

    /// The wrapped enumeration-constant.
    Value value() const { return _value; }

    /// Does the other piece of aliasing information have the same value?
    bool operator==(Aliasing const& a) const { return _value == a.value(); }
};
}

#endif /* SPL_FRONT_END_ALIASING_H */
