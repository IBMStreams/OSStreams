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

#include <SPL/Runtime/Type/SPLType.h>

using namespace SPL;
using namespace std;

TupleAttribute TupleIterator::operator*() const
{
    return tp_->getAttribute(idx_);
}

ConstTupleAttribute ConstTupleIterator::operator*() const
{
    return tp_->getAttribute(idx_);
}

ValueHandle TupleAttribute::getValue() const
{
    return tuple_->getAttributeValue(index_);
}

TupleAttribute::operator ValueHandle() const
{
    return tuple_->getAttributeValue(index_);
}

ConstValueHandle ConstTupleAttribute::getValue() const
{
    return tuple_->getAttributeValue(index_);
}

ConstTupleAttribute::operator ConstValueHandle() const
{
    return tuple_->getAttributeValue(index_);
}
