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

#pragma once

#include "Visitor.h"
#include <map>

namespace adl2dot {

class Dotty : public Visitor<std::ostream>
{
  public:
    void visit(SPLApplication const& app, std::ostream& data);
    void visit(CompositeOperatorInstance const& op, std::ostream& data);
    void visit(PrimitiveExportOperatorInstance const& op, std::ostream& data);
    void visit(PrimitiveImportOperatorInstance const& op, std::ostream& data);
    void visit(PrimitiveOperatorInstance const& op, std::ostream& data);
    void visit(PrimitiveOperatorInstanceInputPort const& port, const int idx, std::ostream& data);
    void visit(PrimitiveOperatorInstanceOutputPort const& port, const int idx, std::ostream& data);

  private:
    void makeInEdges(PrimitiveOperatorInstanceBase const& op,
                     PrimitiveOperatorInstanceInputPort const& port,
                     std::ostream& data);
    void makeOutEdges(PrimitiveOperatorInstanceBase const& op,
                      PrimitiveOperatorInstanceOutputPort const& port,
                      std::ostream& data);

    std::map<std::string, CompositeOperatorInstance> m_composites;
};
}
