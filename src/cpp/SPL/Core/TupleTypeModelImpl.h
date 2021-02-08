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

#ifndef TUPLE_TYPE_MODEL_IMPL_H
#define TUPLE_TYPE_MODEL_IMPL_H

#include <memory>

namespace xmlns {
namespace prod {
namespace streams {
namespace spl {
namespace tupleType {
class tupleType;
}
}
}
}
}

namespace tupletypemodel = xmlns::prod::streams::spl::tupleType;

namespace SPL {
class TupleTyp;
namespace TupleType {
class TupleTypeModel
{
  public:
    TupleTypeModel(const TupleTyp& tuple)
      : _tuple(tuple)
    {}
    std::auto_ptr<tupletypemodel::tupleType> toXsdInstance() const;

  private:
    const TupleTyp& _tuple;
};
};
};

#endif /* TUPLE_TYPE_MODEL_IMPL_H */
