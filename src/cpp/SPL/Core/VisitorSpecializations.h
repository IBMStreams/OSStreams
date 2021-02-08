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

#ifndef VISITOR_SPECIALIZATIONS_H
#define VISITOR_SPECIALIZATIONS_H

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/Core/Collector.h>
#include <SPL/Core/Visitor.h>

namespace SPL {

template<>
bool DepthFirstVisitor<CollectionContainer>::visit(CollectionContainer& context,
                                                   DeclStatement const& stmt);

template<>
bool DepthFirstVisitor<CollectionContainer>::visit(CollectionContainer& context,
                                                   Expression const& expr);

template<>
bool DepthFirstVisitor<CollectionContainer>::visit(CollectionContainer& context,
                                                   CallExpression const& expr);

template<>
bool DepthFirstVisitor<CollectionContainer>::visit(CollectionContainer& context,
                                                   LiteralExpression const& expr);

} // namespace SPL

#endif // VISITOR_SPECIALIZATIONS_H
