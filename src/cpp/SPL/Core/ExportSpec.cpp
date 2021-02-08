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

#include <SPL/Core/ExportSpec.h>

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/Core/CompilationState.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/HashMapHelpers.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>

#include <iostream>
#include <tr1/unordered_set>

using namespace SPL;
using namespace std;
using namespace tr1;

typedef unordered_set<std::string> setOfNames;

ExportSpec::ExportSpec(const string& streamName,
                       bool allowFilter,
                       const string& congestionPolicy,
                       PrimitiveOperatorInstance const& operatorIR)
  : _isNameBased(true)
  , _allowFilter(allowFilter)
  , _streamId(streamName)
  , _congestionPolicy(congestionPolicy)
  , _propList(NULL)
  , _operatorIR(operatorIR)
{}

ExportSpec::ExportSpec(const vector<Property>& propList,
                       bool allowFilter,
                       const string& congestionPolicy,
                       PrimitiveOperatorInstance const& operatorIR)
  : _isNameBased(false)
  , _allowFilter(allowFilter)
  , _congestionPolicy(congestionPolicy)
  , _propList(&propList)
  , _operatorIR(operatorIR)
{}

ExportSpec::~ExportSpec()
{
    // See defect 36013 regarding the following line being commented out. This late deletion of a Literal caused a compiler hang. This is a
    // crude work-around for that problem.
    //    delete _propList;
}

ostream& ExportSpec::print(ostream& s) const
{
    if (_isNameBased) {
        s << "Stream id: " << _streamId;
    } else {
        s << "Property stream: ";
        for (uint32_t i = 0; i < _propList->size(); i++) {
            if (i > 0)
                s << ", ";
            const Property& p = (*_propList)[i];
            s << p.first << " = " << p.second;
        }
    }
    s << ", allowFilter: " << (_allowFilter ? "true" : "false");
    return s << '\n';
}

static setOfNames exportedNames;
class Specs
{
  public:
    Specs() {}
    ~Specs()
    {
        unordered_map<const PrimitiveOperatorInstance*, const ExportSpec*>::const_iterator it,
          itEnd;
        for (it = v.begin(), itEnd = v.end(); it != itEnd; ++it) {
            delete it->second;
        }
    }
    unordered_map<const PrimitiveOperatorInstance*, const ExportSpec*>& getMap() { return v; }

  private:
    unordered_map<const PrimitiveOperatorInstance*, const ExportSpec*> v;
};

static Specs specs;

const ExportSpec* ExportSpec::createFromExportOper(const PrimitiveOperatorInstance& op)
{
    unordered_map<const PrimitiveOperatorInstance*, const ExportSpec*>::const_iterator it1 =
      specs.getMap().find(&op);
    if (it1 != specs.getMap().end()) {
        return it1->second;
    }

    // Walk through the parameter list, checking for correctness
    const SourceLocation& loc = op.getLocation();
    const vector<Param*>& params = op.getParams();
    uint32_t i, numParams = params.size();
    const Param* exportProperties = NULL;
    string streamId;
    bool streamIdSpecified = false;
    bool allowFilter = true;
    string singleString;
    string congestionPolicy = "wait";

    for (i = 0; i < numParams; i++) {
        const Param& p = *params[i];
        const string& id = p.getName();

        if (id == "properties") {
            exportProperties = &p;
        } else if (id == "streamId") {
            streamIdSpecified = true;
            CompilationState& state = CompilationState::instance();
            state.push();
            state.setStringizeGetChannel();
            state.setStringizeGetMaxChannels();
            state.setStringizeGetLocalChannel();
            state.setStringizeGetLocalMaxChannels();
            bool brc = p.getSingleString(id, singleString);
            state.pop();
            if (brc) {
                streamId = singleString;
                // check if export stream already exists
                setOfNames::const_iterator it = exportedNames.find(streamId);
                if (it == exportedNames.end()) {
                    exportedNames.insert(streamId);
                } else {
                    SysOut::errorln(SPL_CORE_EXPORT_STREAM_ID_EXISTS(streamId), loc);
                }
                // Check if it's in a parallel region
                if (op.isInParallelRegion()) {
                    if (streamId.find("getChannel") == string::npos &&
                        streamId.find("getLocalChannel") == string::npos) {
                        // Figure out the width if we can.  If it is hard coded, or if it is submission-time with a
                        // default > 1, then emit an error if they don't use getChannel
                        const SPLParallelAnnotation& annot = op.getParallelAnnotation();
                        if ((annot.submissionTimeName().empty() || !annot.defaultWidth().empty()) &&
                            annot.width() > 1) {
                            SysOut::errorln(SPL_CORE_EXPORT_STREAM_ID_NEEDS_GETCHANNEL(streamId),
                                            loc);
                        }
                    }
                }
            } else {
                // something went wrong when getting the single string param
                const Expression* e1 = p.getSingleExpression(id);
                // was it the number of params or the type of the param ?
                if (e1) {
                    SysOut::errorln(
                      SPL_CORE_SINGLE_STRING_LITERAL_EXPECTED_IN_PARAMETER(id, e1->toString()),
                      loc);
                } else {
                    SysOut::errorln(SPL_CORE_SINGLE_EXPRESSION_EXPECTED_IN_PARAMETER(id), loc);
                }
            }
        } else if (id == "allowFilter") {
            const Expression* e = p.getSingleExpression("allowFilter");
            const Literal* l = e->getLiteral();
            if (l) {
                const ExpressionValue& ev = l->primitive();
                allowFilter = ev.get_boolean();
            }
        } else if (id == "congestionPolicy") {
            const Expression* e = p.getSingleExpression("congestionPolicy");
            assert(NULL != e);
            assert(e->is<CustomLiteralExpression>());
            congestionPolicy = e->as<CustomLiteralExpression>().getId();
        }
    }

    if (!exportProperties) {
        if (streamIdSpecified) { // Easy case.   This is just the stream name
            ExportSpec* spec = new ExportSpec(streamId, allowFilter, congestionPolicy, op);
            specs.getMap().insert(make_pair(&op, spec));
            return spec;
        }
        // create generic properties.... to be set at runtime
        vector<Property>* genericPropertiesList = new vector<Property>;
        ExportSpec* spec =
          new ExportSpec(*genericPropertiesList, allowFilter, congestionPolicy, op);
        specs.getMap().insert(make_pair(&op, spec));
        return spec;
    }

    // at this point properties were specified so report error if streamId is
    // also specified
    if (streamIdSpecified) {
        SysOut::errorln(SPL_CORE_STREAM_ID_AND_PROPERTIES_ON_EXPORT, loc);
        specs.getMap().insert(make_pair(&op, (const ExportSpec*)NULL));
        return NULL;
    }

    // We are expecting a tuple literal that we have to take apart
    const vector<const Expression*>& paramExpns = exportProperties->getExpressionList();
    auto_ptr<vector<Property> > pList(new vector<Property>);

    AutoErrorTrap aet(false); // count errors in current scope, never die
    ExpressionEvaluator ee(CompilerConfiguration::instance());
    unordered_map<string, Literal> ids;

    for (uint32_t i1 = 0; i1 < paramExpns.size(); i1++) {
        const Expression& paramExpn = *paramExpns[i1];
        auto_ptr<Expression> simplifiedParam(Expression::simplify(&paramExpn.clone(), ee));
        const Literal* lit = simplifiedParam->getLiteral();
        if (!lit || lit->getKind() != Literal::Tuple || lit->numElements() == 0) {
            SysOut::errorln(SPL_CORE_EXPORT_PROPERTY_EXPECTING_EXPRESSION(paramExpn.toString()),
                            paramExpn.getLocation());
            specs.getMap().insert(make_pair(&op, (const ExportSpec*)NULL));
            return NULL;
        }

        for (uint32_t i2 = 0; i2 < lit->numElements(); i2++) {
            // Need to check the tuple element, and convert to Property
            const Literal::TupleLiteral& tLit = lit->tupleElement(i2);
            const Literal& l = *tLit._value;
            Property p;
            p.first = tLit._id;
            p.second = l;

            pair<unordered_map<string, Literal>::iterator, bool> lookup;
            lookup = ids.insert(make_pair(tLit._id, l));
            if (!lookup.second) {
                // we have seen it before
                if (lookup.first->second != l) {
                    // We have seen this id before with a different value
                    SysOut::errorln(SPL_CORE_EXPORT_PROPERTY_MULTIPLE_DEFS(tLit._id),
                                    paramExpn.getLocation());
                }
                continue;
            }

            // It has to be a literal int64/float64/rstring or a list of those

            // Check the type of the literal
            const RootTyp* t = NULL;
            if (l.getKind() == Literal::List) {
                t = &l.getType();
                t = &static_cast<const ListTyp*>(t)->getElementType();
            } else if (l.getKind() == Literal::Primitive) {
                t = &l.getType();
            } else if (l.getKind() == Literal::Expn) {
                const Expression& expr = l.expression();
                if (expr.getKind() == Expression::Call) {
                    string st = expr.toString();
                    if (st == "getMaxChannels()" || st == "getChannel()" ||
                        st == "getLocalChannel()" || st == "getLocalMaxChannels()") {
                        t = &TypeFactory::instance().primitiveT(MetaType::INT64);
                    }
                }
            }
            if (t == NULL) {
                ostringstream s;
                s << l;
                SysOut::errorln(SPL_CORE_EXPORT_PROPERTY_EXPECTING_LITERAL(s.str()),
                                paramExpn.getLocation());
                break;
            }

            switch (t->getMetaType()) {
                case MetaType::RSTRING:
                case MetaType::INT8:
                case MetaType::INT16:
                case MetaType::INT32:
                case MetaType::INT64:
                case MetaType::UINT8:
                case MetaType::UINT16:
                case MetaType::UINT32:
                case MetaType::FLOAT32:
                case MetaType::FLOAT64:
                    break;
                default: {
                    ostringstream s;
                    s << l;
                    SysOut::errorln(SPL_CORE_EXPORT_PROPERTY_EXPECTING_LITERAL(s.str()),
                                    paramExpn.getLocation());
                } break;
            }

            if (aet.getNumErrorsInCurrentTrap() == 0) {
                pList->push_back(p);
            }
        }
    }

    if (aet.getNumErrorsInCurrentTrap() > 0) {
        specs.getMap().insert(make_pair(&op, (const ExportSpec*)NULL));
        return NULL;
    }

    // Nothing much to do now....
    ExportSpec* spec = new ExportSpec(*pList.release(), allowFilter, congestionPolicy, op);
    specs.getMap().insert(make_pair(&op, spec));
    return spec;
}
