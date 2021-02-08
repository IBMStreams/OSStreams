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

#ifndef SPL_RUNTIME_OPERATOR_PORT_ITEM_H
#define SPL_RUNTIME_OPERATOR_PORT_ITEM_H

#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Type/Tuple.h>

namespace SPL {
class Data
{
  public:
    enum Type
    {
        Tuple,
        Punct,
        Invalid
    };
    Data()
      : tuple_(NULL)
      , punct_(NULL)
      , type_(Invalid)
    {}
    Data(SPL::Tuple& tuple)
      : tuple_(&tuple)
      , type_(Tuple)
    {}
    Data(Punctuation& punct)
      : punct_(&punct)
      , type_(Punct)
    {}

    Type getType() const
    {
        assert(type_ != Invalid);
        return type_;
    }

    bool isTuple() const { return type_ == Tuple; }

    SPL::Tuple& getTuple()
    {
        assert(type_ == Tuple);
        return *tuple_;
    }

    SPL::Tuple* getTuplePtr()
    {
        assert(type_ == Tuple);
        return tuple_;
    }

    Punctuation& getPunctuation()
    {
        assert(type_ == Punct);
        return *punct_;
    }

    Punctuation* getPunctuationPtr()
    {
        assert(type_ == Punct);
        return punct_;
    }

    void setType(Type type) { type_ = type; }

    bool hasTuple() const { return type_ == Tuple; }

    bool hasPunctuation() const { return type_ == Punct; }

    void setTuple(SPL::Tuple& tuple) { tuple_ = &tuple; }

    void setPunctuation(Punctuation& punct) { punct_ = &punct; }

  private:
    SPL::Tuple* tuple_;
    SPL::Punctuation* punct_;
    Type type_;
};
}

#endif /* SPL_RUNTIME_OPERATOR_PORT_ITEM_H */
