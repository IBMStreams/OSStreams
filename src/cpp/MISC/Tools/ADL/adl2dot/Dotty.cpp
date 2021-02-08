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

#include "Dotty.h"
#include <iostream>
#include <list>
#include <vector>

using namespace adl2dot;

void Dotty::visit(SPLApplication const& app, std::ostream& data)
{
    data << "digraph \"" << app.name() << "\" {" << std::endl;
    data << "rankdir=\"LR\";" << std::endl;
    data << "graph[labeljust=left, fontsize=12];" << std::endl;
    Visitor::visit(app, data);
    data << "}" << std::endl;
}

void Dotty::visit(CompositeOperatorInstance const& op, std::ostream& data)
{
    /*
     * Record the composite operator.
     */
    m_composites[op.name()] = op;
    /*
     * Generate the cluster subgraph.
     */
    data << "subgraph cluster_OP" << op.index() << " {" << std::endl;
    data << "label=<<B><I>" << op.name() << "</I></B>>;" << std::endl;
    data << "style=\"dotted\";" << std::endl;
    Visitor::visit(op, data);
    data << "}" << std::endl;
}

void Dotty::visit(PrimitiveExportOperatorInstance const& op, std::ostream& data)
{
    data << "OP" << op.index() << " [shape=\"box\", style=\"rounded\", label=";
    /*
     * Generate the table header.
     */
    data << "<<table border=\"0\">";
    data << "<tr><td colspan=\"2\" align=\"left\"><B>";
    data << op.name();
    data << "</B></td></tr>";
    data << "<tr>";
    /*
     * Generate the output ports. There is 1 by construction.
     */
    data << "<td><table border=\"0\">";
    data << "<tr><td border=\"1\" port=\"IN0\">";
    data << op.inputPorts()->port().front().name();
    data << "</td></tr>";
    data << "</table></td>";
    /*
     * Generate the table footer.
     */
    data << "</tr>";
    data << "</table>>];";
    data << std::endl;
}

void Dotty::visit(PrimitiveImportOperatorInstance const& op, std::ostream& data)
{
    data << "OP" << op.index() << " [shape=\"box\", style=\"rounded\", label=";
    /*
     * Generate the table header.
     */
    data << "<<table border=\"0\">";
    data << "<tr><td colspan=\"2\" align=\"left\"><B>";
    data << op.name();
    data << "</B></td></tr>";
    data << "<tr>";
    /*
     * Generate the output port. There is 1 by construction.
     */
    data << "<td><table border=\"0\">";
    data << "<tr><td border=\"1\" port=\"OUT0\">";
    data << op.outputPorts()->port().front().name();
    data << "</td></tr>";
    data << "</table></td>";
    /*
     * Generate the table footer.
     */
    data << "</tr>";
    data << "</table>>];";
    data << std::endl;
    /*
     * Generate the connections.
     */
    auto const& port = op.outputPorts()->port().front();
    makeOutEdges(op, port, data);
}

void Dotty::visit(PrimitiveOperatorInstance const& op, std::ostream& data)
{
    data << "OP" << op.index() << " [shape=\"box\", label=";
    /*
     * Generate the table header.
     */
    data << "<<table border=\"0\">";
    data << "<tr><td colspan=\"2\" align=\"left\"><B>";
    data << op.name();
    data << "</B></td></tr>";
    data << "<tr>";
    /*
     * Generate the input ports.
     */
    if (op.inputPorts().present()) {
        data << "<td><table border=\"0\">";
        Visitor::visit(op.inputPorts().get(), op.index(), data);
        data << "</table></td>";
    }
    /*
     * Generate the output ports.
     */
    if (op.outputPorts().present()) {
        data << "<td><table border=\"0\">";
        Visitor::visit(op.outputPorts().get(), op.index(), data);
        data << "</table></td>";
    }
    /*
     * Generate the table footer.
     */
    data << "</tr>";
    data << "</table>>];";
    data << std::endl;
    /*
     * Generate the input connections to composites only.
     */
    if (op.inputPorts().present()) {
        for (auto const& in : op.inputPorts()->port()) {
            makeInEdges(op, in, data);
        }
    }
    /*
     * Generate the output connections.
     */
    if (op.outputPorts().present()) {
        for (auto const& out : op.outputPorts()->port()) {
            makeOutEdges(op, out, data);
        }
    }
}

void Dotty::visit(PrimitiveOperatorInstanceInputPort const& port, const int idx, std::ostream& data)
{
    data << "<tr><td border=\"1\" port=\"IN" << port.index() << "\">";
    data << port.name();
    data << "</td></tr>";
}

void Dotty::visit(PrimitiveOperatorInstanceOutputPort const& port,
                  const int idx,
                  std::ostream& data)
{
    data << "<tr><td border=\"1\" port=\"OUT" << port.index() << "\">";
    data << port.name();
    data << "</td></tr>";
}

void Dotty::makeInEdges(PrimitiveOperatorInstanceBase const& op,
                        PrimitiveOperatorInstanceInputPort const& port,
                        std::ostream& data)
{
    if (port.connections().present()) {
        for (auto const& c : port.connections()->connection()) {
            /*
             * Skip if the source is not a composite.
             */
            if (m_composites.find(*c.operName()) == m_composites.end()) {
                continue;
            }
            /*
             * Build the initial lists.
             */
            std::list<OperatorInstancePortConnectionType> targets, result;
            targets.push_back(c);
            /*
             * Resolve all transitional composite ports.
             */
            do {
                auto cur = targets.front();
                targets.pop_front();
                if (m_composites.find(*cur.operName()) == m_composites.end()) {
                    result.push_back(cur);
                } else {
                    auto const& comp = m_composites[*cur.operName()];
                    if (c.portKind() == "output") {
                        if (!comp.outputPorts().present()) {
                            continue;
                        }
                        for (auto const& o : comp.outputPorts()->port()) {
                            if (!o.incomingConnections().present()) {
                                continue;
                            }
                            for (auto const& c : o.incomingConnections()->connection()) {
                                targets.push_back(c);
                            }
                        }
                    }
                }
            } while (!targets.empty());
            /*
                 * Add all edges.
                 */
            for (auto const& c : result) {
                data << "OP" << c.operIndex() << ":OUT" << c.portIndex() << ":e";
                data << " -> ";
                data << "OP" << op.index() << ":IN" << port.index();
                data << ";" << std::endl;
            }
        }
    }
}

void Dotty::makeOutEdges(PrimitiveOperatorInstanceBase const& op,
                         PrimitiveOperatorInstanceOutputPort const& port,
                         std::ostream& data)
{
    if (port.connections().present()) {
        for (auto const& c : port.connections()->connection()) {
            std::list<OperatorInstancePortConnectionType> targets, result;
            targets.push_back(c);
            /*
                 * Resolve all transitional composite ports.
                 */
            do {
                auto cur = targets.front();
                targets.pop_front();
                if (m_composites.find(*cur.operName()) == m_composites.end()) {
                    result.push_back(cur);
                } else {
                    auto const& comp = m_composites[*cur.operName()];
                    if (c.portKind() == "input") {
                        if (!comp.inputPorts().present()) {
                            continue;
                        }
                        for (auto const& o : comp.inputPorts()->port()) {
                            if (!o.outgoingConnections().present()) {
                                continue;
                            }
                            for (auto const& c : o.outgoingConnections()->connection()) {
                                targets.push_back(c);
                            }
                        }
                    }
                }
            } while (!targets.empty());
            /*
                 * Add all edges.
                 */
            for (auto const& c : result) {
                data << "OP" << op.index() << ":OUT" << port.index() << ":e";
                data << " -> ";
                data << "OP" << c.operIndex() << ":IN" << c.portIndex();
                data << ";" << std::endl;
            }
        }
    }
}
