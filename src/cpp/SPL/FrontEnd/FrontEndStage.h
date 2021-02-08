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

#ifndef SPL_FRONT_END_STAGE_H
#define SPL_FRONT_END_STAGE_H

#include <string>

namespace SPL {
class FrontEndStage
{
  public:
    enum Value
    {
        ERROR = -1,
        PARSER,
        DIAGNOSTICIAN,
        NAME_DEF_FINDER,
        LEXICAL_NAME_USE_FINDER,
        COMPOSITE_FORMAL_EXPANDER,
        SCOPE_EXPANDER,
        RELATIVE_NAME_USE_FINDER,
        TYPE_ANALYZER,
        TYPE_PROMOTER,
        PROMOTION_CHECKER,
        EXPRESSION_TYPE_FINDER,
        EMPTY_CONTAINER_TYPE_FINDER,
        VARIABLE_USE_CHECKER,
        LOGIC_SUBMIT_CHECKER,
        OUTPUT_SIDE_EFFECTS_CHECKER,
        DONE,
    };

  private:
    FrontEndStage() {}
    Value _value;

  public:
    FrontEndStage(Value v)
      : _value(v)
    {}
    FrontEndStage& operator=(Value v)
    {
        _value = v;
        return *this;
    }
    bool operator<(Value v) const { return _value < v; }
    bool operator>(FrontEndStage const& s) const { return _value > s.value(); }
    bool operator>=(Value v) const { return _value >= v; }
    bool operator==(FrontEndStage const& s) const { return _value == s.value(); }
    bool operator!=(FrontEndStage const& s) const { return _value != s.value(); }
    std::string toString() const;
    Value value() const { return _value; }
};

}
#endif /* SPL_FRONT_END_DRIVER_H*/
