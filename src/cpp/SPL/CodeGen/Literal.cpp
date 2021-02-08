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

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Utility/SourceLocation.h>

using namespace std;
using namespace std::tr1;

namespace SPL {
//    unordered_map<const Literal*, string> Literal::_enumMap;

Literal::Literal()
  : _kind(Invalid)
  , _type(NULL)
  , _varType(NULL)
  , _expn(NULL)
{}

Literal::Literal(Expression& exp)
  : _type(&exp.getType())
  , _varType(_type)
{
    _kind = Expn;
    _expn = &exp;
}

Literal::Literal(const RootTyp& type, const RootTyp* varType, Expression& exp)
  : _type(&type)
  , _varType(varType)
{
    _kind = Expn;
    _expn = &exp;
}

Literal::Literal(const RootTyp& type, const ExpressionValue* v, const string& inenumValueName)
  : _type(&type)
  , _varType(_type)
{
    initialize(v);
    _enumMap.insert(make_pair<const Literal*, string>(this, inenumValueName));
}

Literal::Literal(const RootTyp& type, const ExpressionValue* v)
  : _type(&type)
  , _varType(_type)
{
    initialize(v);
}

Literal::Literal(const RootTyp& type, const RootTyp& varType, const ExpressionValue* v)
  : _type(&type)
  , _varType(&varType)
{
    initialize(v);
}

Literal::Literal(const Literal& rhs)
{
    copy(rhs);
}

void Literal::copy(const Literal& rhs)
{
    _kind = rhs._kind;
    _type = rhs._type;
    _varType = rhs._varType;
    uint32_t num;
    switch (rhs._kind) {
        case Expn:
            _expn = &rhs._expn->clone();
            break;
        case List:
        case Set:
            num = rhs._listSet->size();
            _listSet = new vector<Literal*>;
            for (uint32_t i = 0; i < num; i++) {
                _listSet->push_back(&(*rhs._listSet)[i]->clone());
            }
            break;
        case Map:
            num = rhs._map->size();
            _map = new vector<MapLiteral>;
            for (uint32_t i = 0; i < num; i++) {
                MapLiteral& m = (*rhs._map)[i];
                MapLiteral newM;
                newM._key = &m._key->clone();
                newM._value = &m._value->clone();
                _map->push_back(newM);
            }
            break;
        case Tuple:
            num = rhs._tuple->size();
            _tuple = new vector<TupleLiteral>;
            for (uint32_t i = 0; i < num; i++) {
                TupleLiteral& t = (*rhs._tuple)[i];
                TupleLiteral newT;
                newT._id = t._id;
                newT._value = &t._value->clone();
                _tuple->push_back(newT);
            }
            break;
        case Invalid:
            break;
        case Nul:
            assert(false);
            break;
        default: {
            _primitives = new ExpressionValue(*rhs._primitives);
            const string* eName = rhs.enumValueName();
            if (eName) {
                _enumMap.insert(make_pair<const Literal*, string>(this, *eName));
            }
        } break;
    }
}

const string* Literal::enumValueName() const
{
    unordered_map<const Literal*, string>::const_iterator it;
    it = _enumMap.find(this);
    if (it != _enumMap.end()) {
        return &it->second;
    }
    return NULL;
}

void Literal::initialize(const ExpressionValue* v)
{
    const RootTyp& type =
      (_type->isOptional()) ? _type->as<OptionalTyp>().getUnderlyingType() : *_type;
    switch (type.getMetaType()) {
        case MetaType::LIST:
        case MetaType::BLIST:
            _kind = List;
            _listSet = new vector<Literal*>;
            break;
        case MetaType::SET:
        case MetaType::BSET:
            _kind = Set;
            _listSet = new vector<Literal*>;
            break;
        case MetaType::MAP:
        case MetaType::BMAP:
            _kind = Map;
            _map = new vector<MapLiteral>;
            break;
        case MetaType::TUPLE:
            _kind = Tuple;
            _tuple = new vector<TupleLiteral>;
            break;
        case MetaType::XML:
            _kind = XML;
            assert(v != NULL);
            _primitives = v;
            break;
        case MetaType::NUL:
            _kind = Nul;
            break;
        case MetaType::INVALID:
        case MetaType::BLOB:
        case MetaType::UNKNOWN:
            THROW(SPLCompilerAssertionFailed, "Unable to handle type '" << _type->getName() << "'");
            break;

        default:
            _kind = Primitive;
            assert(v != NULL);
            _primitives = v;
            break;
    }
}

Literal::~Literal()
{
    destroy();
}

void Literal::destroy()
{
    uint32_t num;
    switch (_kind) {
        case List:
        case Set:
            num = _listSet->size();
            for (uint32_t i = 0; i < num; i++) {
                delete (*_listSet)[i];
            }
            delete _listSet;
            break;
        case Map:
            num = _map->size();
            for (uint32_t i = 0; i < num; i++) {
                MapLiteral& m = (*_map)[i];
                delete m._key;
                delete m._value;
            }
            delete _map;
            break;
        case Tuple:
            num = _tuple->size();
            for (uint32_t i = 0; i < num; i++) {
                TupleLiteral& t = (*_tuple)[i];
                delete t._value;
            }
            delete _tuple;
            break;
        case Expn:
            delete _expn;
            break;
        case Invalid:
            break;
        case Nul:
            break;
        default: {
            unordered_map<const Literal*, string>::const_iterator it;
            it = _enumMap.find(this);
            if (it != _enumMap.end()) {
                _enumMap.erase(it);
            }
            delete _primitives;
        } break;
    }
}

Literal& Literal::operator=(const Literal& rhs)
{
    // clean myself up
    destroy();

    // Now copy the rhs values
    copy(rhs);
    return *this;
}

bool Literal::hasSideEffects(void) const
{
    switch (_kind) {
        case Expn:
            return _expn->hasSideEffects();
        case List:
        case Set:
            for (uint32_t i = 0, num = _listSet->size(); i < num; i++) {
                if ((*_listSet)[i]->hasSideEffects()) {
                    return true;
                }
            }
            break;
        case Map:
            for (uint32_t i = 0, num = _map->size(); i < num; i++) {
                MapLiteral& m = (*_map)[i];
                if (m._value->hasSideEffects() || m._key->hasSideEffects()) {
                    return true;
                }
            }
            break;
        case Tuple:
            for (uint32_t i = 0, num = _tuple->size(); i < num; i++) {
                TupleLiteral& t = (*_tuple)[i];
                if (t._value->hasSideEffects()) {
                    return true;
                }
            }
            break;
        case Invalid:
        case Nul:
        default:
            break;
    }
    return false;
}

bool Literal::readsState(void) const
{
    switch (_kind) {
        case Expn:
            return _expn->readsState();
        case List:
        case Set:
            for (uint32_t i = 0, num = _listSet->size(); i < num; i++) {
                if ((*_listSet)[i]->readsState()) {
                    return true;
                }
            }
            break;
        case Map:
            for (uint32_t i = 0, num = _map->size(); i < num; i++) {
                MapLiteral& m = (*_map)[i];
                if (m._value->readsState() || m._key->readsState()) {
                    return true;
                }
            }
            break;
        case Tuple:
            for (uint32_t i = 0, num = _tuple->size(); i < num; i++) {
                TupleLiteral& t = (*_tuple)[i];
                if (t._value->readsState()) {
                    return true;
                }
            }
            break;
        case Invalid:
        case Nul:
        default:
            break;
    }
    return false;
}

bool Literal::compileTimeEvaluatable() const
{
    switch (_kind) {
        case Expn:
            return false;
        case List:
        case Set:
            for (uint32_t i = 0, num = _listSet->size(); i < num; i++) {
                if (!(*_listSet)[i]->compileTimeEvaluatable()) {
                    return false;
                }
            }
            break;
        case Map:
            for (uint32_t i = 0, num = _map->size(); i < num; i++) {
                MapLiteral& m = (*_map)[i];
                if (!m._value->compileTimeEvaluatable() || !m._key->compileTimeEvaluatable()) {
                    return false;
                }
            }
            break;
        case Tuple:
            for (uint32_t i = 0, num = _tuple->size(); i < num; i++) {
                TupleLiteral& t = (*_tuple)[i];
                if (!t._value->compileTimeEvaluatable()) {
                    return false;
                }
            }
            break;
        case Invalid:
            return false;
        case Nul:
        default:
            break;
    }
    return true;
}

bool Literal::shouldAllowBackwardsCodeMotion() const
{
    switch (_kind) {
        case Primitive:
            return true;
            break;

        case Expn:
            // TODO:  we could walk the expression and find out if it's truly constant or not
            // for now we just assume any expression might not be constant
            return false;
            break;

        case List:
        case Set:
            for (uint32_t i = 0, num = _listSet->size(); i < num; i++) {
                if (!(*_listSet)[i]->shouldAllowBackwardsCodeMotion()) {
                    return false;
                }
            }
            break;
        case Map:
            for (uint32_t i = 0, num = _map->size(); i < num; i++) {
                MapLiteral& m = (*_map)[i];
                if (!m._value->compileTimeEvaluatable() ||
                    !m._key->shouldAllowBackwardsCodeMotion()) {
                    return false;
                }
            }
            break;
        case Tuple:
            for (uint32_t i = 0, num = _tuple->size(); i < num; i++) {
                TupleLiteral& t = (*_tuple)[i];
                if (!t._value->shouldAllowBackwardsCodeMotion()) {
                    return false;
                }
            }
            break;
        case Invalid:
            return false;
        case Nul:
        default:
            break;
    }
    return true;
}

bool Literal::hasEmptyCompositeLiterals() const
{
    switch (_kind) {
        case List:
        case Set:
            if (_listSet->size() == 0) {
                return true;
            }
            for (uint32_t i = 0, num = _listSet->size(); i < num; i++) {
                if ((*_listSet)[i]->hasEmptyCompositeLiterals()) {
                    return true;
                }
            }
            break;
        case Map:
            if (_map->size() == 0) {
                return true;
            }
            for (uint32_t i = 0, num = _map->size(); i < num; i++) {
                MapLiteral& m = (*_map)[i];
                if (m._value->hasEmptyCompositeLiterals() || m._key->hasEmptyCompositeLiterals()) {
                    return true;
                }
            }
            break;
        case Tuple:
            if (_tuple->size() == 0) {
                return true;
            }
            for (uint32_t i = 0, num = _tuple->size(); i < num; i++) {
                TupleLiteral& t = (*_tuple)[i];
                if (t._value->hasEmptyCompositeLiterals()) {
                    return true;
                }
            }
            break;
        case Invalid:
        case Expn:
        case Nul:
        default:
            break;
    }
    return false;
}

Literal* Literal::replaceLits(LiteralReplacer& lit)
{
    if (compileTimeEvaluatable() && !emptyTupleLiteral()) {
        // We can replace this right now
        uint32_t litIndex;
        const SourceLocation none;
        uint32_t idIndex = lit.addLiteral(*this, none, litIndex);
        return new Literal(*new LiteralSymbolExpression(*_type, none, idIndex, litIndex));
    }
    switch (_kind) {
        case Expn: {
            Expression* e = Expression::replaceLits(const_cast<Expression*>(_expn), lit, false);
            if (e != _expn) {
                return new Literal(*e);
            }
        } break;
        case List:
        case Set:
            for (uint32_t i = 0, num = _listSet->size(); i < num; i++) {
                Literal*& l = (*_listSet)[i];
                l = l->replaceLits(lit);
            }
            break;
        case Map:
            for (uint32_t i = 0, num = _map->size(); i < num; i++) {
                MapLiteral& m = (*_map)[i];
                m._value = m._value->replaceLits(lit);
                m._key = m._key->replaceLits(lit);
            }
            break;
        case Tuple:
            for (uint32_t i = 0, num = _tuple->size(); i < num; i++) {
                TupleLiteral& t = (*_tuple)[i];
                t._value = t._value->replaceLits(lit);
            }
            break;
        default:
            break;
    }
    return this;
}

void Literal::collectTypeAndFunctions(unordered_set<const RootTyp*>* types,
                                      unordered_set<const FunctionHeadSym*>* fcns,
                                      bool inFcns) const
{
    switch (_kind) {
        case Expn:
            _expn->collectTypeAndFunctions(types, fcns, inFcns);
            break;
        case List:
        case Set:
            for (uint32_t i = 0, num = _listSet->size(); i < num; i++) {
                (*_listSet)[i]->collectTypeAndFunctions(types, fcns, inFcns);
            }
            break;
        case Map:
            for (uint32_t i = 0, num = _map->size(); i < num; i++) {
                MapLiteral& m = (*_map)[i];
                m._value->collectTypeAndFunctions(types, fcns, inFcns);
                m._key->collectTypeAndFunctions(types, fcns, inFcns);
            }
            break;
        case Tuple:
            for (uint32_t i = 0, num = _tuple->size(); i < num; i++) {
                TupleLiteral& t = (*_tuple)[i];
                t._value->collectTypeAndFunctions(types, fcns, inFcns);
            }
            break;
        case Nul: // Not sure if we need this type
        default:
            break;
    }
}

Literal& Literal::clone() const
{
    uint32_t num;
    Literal* p;
    switch (_kind) {
        case Expn:
            p = new Literal(*_type, _varType, _expn->clone());
            break;
        case List:
        case Set:
            num = _listSet->size();
            p = new Literal(*_type, *_varType, NULL);
            for (uint32_t i = 0; i < num; i++) {
                p->_listSet->push_back(&(*_listSet)[i]->clone());
            }
            break;
        case Map:
            num = _map->size();
            p = new Literal(*_type, *_varType, NULL);
            for (uint32_t i = 0; i < num; i++) {
                MapLiteral& m = (*_map)[i];
                MapLiteral newM;
                newM._key = &m._key->clone();
                newM._value = &m._value->clone();
                p->_map->push_back(newM);
            }
            break;
        case Tuple:
            num = _tuple->size();
            p = new Literal(*_type, *_varType, NULL);
            for (uint32_t i = 0; i < num; i++) {
                TupleLiteral& t = (*_tuple)[i];
                TupleLiteral newT;
                newT._id = t._id;
                newT._value = &t._value->clone();
                p->_tuple->push_back(newT);
            }
            break;
        case Invalid:
            p = new Literal();
            break;
        case Nul:
            p = new Literal(*_type, *_varType, NULL);
            break;
        default: {
            const string* eName = enumValueName();
            if (eName) {
                p = new Literal(*_type, new ExpressionValue(*_primitives), *eName);
            } else {
                p = new Literal(*_type, *_varType, new ExpressionValue(*_primitives));
            }
        } break;
    }
    return *p;
}

uint32_t Literal::numElements() const
{
    switch (_kind) {
        case List:
        case Set:
            return _listSet->size();
        case Map:
            return _map->size();
        case Tuple:
            return _tuple->size();
        case Invalid:
            return 0;
        default:
            return 1;
    }
}

const ExpressionValue& Literal::primitive() const
{
    assert(_kind == Primitive || _kind == XML);
    return *_primitives;
}

const Expression& Literal::expression() const
{
    assert(_kind == Expn);
    return *_expn;
}

Expression& Literal::expression()
{
    assert(_kind == Expn);
    return *_expn;
}

const Literal& Literal::listSetElement(uint32_t i) const
{
    assert(_kind == Set || _kind == List);
    return *(*_listSet)[i];
}

Literal& Literal::listSetElement(uint32_t i)
{
    assert(_kind == Set || _kind == List);
    return *(*_listSet)[i];
}

const Literal::MapLiteral& Literal::mapElement(uint32_t i) const
{
    assert(_kind == Map);
    return (*_map)[i];
}

Literal::MapLiteral& Literal::mapElement(uint32_t i)
{
    assert(_kind == Map);
    return (*_map)[i];
}

const Literal::TupleLiteral& Literal::tupleElement(uint32_t i) const
{
    assert(_kind == Tuple);
    return (*_tuple)[i];
}

Literal::TupleLiteral& Literal::tupleElement(uint32_t i)
{
    assert(_kind == Tuple);
    return (*_tuple)[i];
}

void Literal::addListElement(Literal& l)
{
    assert(_kind == List);
    _listSet->push_back(&l);
}

bool Literal::addSetElement(Literal& l)
{
    // We have to keep the list sorted as well, to ensure [1,2] == [2,1]
    assert(_kind == Set);
    uint32_t num = _listSet->size();
    for (uint32_t i = 0; i < num; i++) {
        const Literal& e = *(*_listSet)[i];
        if (e == l) {
            return true;
        }
        if (e > l) {
            // Need to insert it here
            _listSet->insert(_listSet->begin() + i, 1, &l);
            return false;
        }
    }

    // Haven't seen it
    _listSet->push_back(&l);
    return false;
}

bool Literal::addMapElement(Literal& k, Literal& v)
{
    assert(_kind == Map);
    uint32_t num = _map->size();
    // We have to keep the list sorted as well, to ensure adding
    // elements in different orders doesn't change ==
    MapLiteral ml;
    for (uint32_t i = 0; i < num; i++) {
        MapLiteral& m = (*_map)[i];
        if (*m._key == k) {
            delete m._value;
            m._value = &v;
            return true;
        }
        if (*m._key > k) {
            // Need to insert it here
            ml._key = &k;
            ml._value = &v;
            _map->insert(_map->begin() + i, 1, ml);
            return false;
        }
    }

    // Haven't seen it
    ml._key = &k;
    ml._value = &v;
    _map->push_back(ml);
    return false;
}

bool Literal::addTupleElement(const string& id, Literal& v)
{
    assert(_kind == Tuple);
    uint32_t num = _tuple->size();
    TupleLiteral tl;
    for (uint32_t i = 0; i < num; i++) {
        TupleLiteral& t = (*_tuple)[i];
        if (t._id == id) {
            delete t._value;
            t._value = &v;
            return true;
        }
    }
    tl._id = id;
    tl._value = &v;
    _tuple->push_back(tl);
    return false;
}

bool Literal::operator>(const Literal& rhs) const
{
    if (_kind != rhs._kind || *_type != *rhs._type || _kind == Expn) {
        return false;
    }

    uint32_t num;
    switch (_kind) {
        case Literal::List:
        case Literal::Set:
            num = _listSet->size();
            if (num != rhs._listSet->size()) {
                return num > rhs._listSet->size();
            }
            for (uint32_t i = 0; i < num; i++) {
                if (*(*_listSet)[i] > *(*rhs._listSet)[i]) {
                    return true;
                }
            }
            break;
        case Literal::Map:
            num = _map->size();
            if (num != rhs._map->size()) {
                return num > rhs._map->size();
            }
            for (uint32_t i = 0; i < num; i++) {
                const Literal::MapLiteral& m = (*_map)[i];
                const Literal::MapLiteral& rm = (*rhs._map)[i];
                if (*m._key > *rm._key) {
                    return true;
                }
            }
            break;
        case Literal::Tuple:
            num = _tuple->size();
            if (num != rhs._tuple->size()) {
                return num > rhs._tuple->size();
            }
            for (uint32_t i = 0; i < num; i++) {
                const Literal::TupleLiteral& t = (*_tuple)[i];
                const Literal::TupleLiteral& rt = (*rhs._tuple)[i];
                if (t._id > rt._id) {
                    return true;
                }
            }
            break;
        case XML:
        case Invalid:
        case Nul:
            return false;
        default:
            return *_primitives > *rhs._primitives;
    }
    return true;
}

bool Literal::operator==(const Literal& rhs) const
{
    if (_kind != rhs._kind || *_type != *rhs._type) {
        return false;
    }

    uint32_t num;
    switch (_kind) {
        case Literal::Expn:
            return *_expn == *rhs._expn;
        case Literal::List:
        case Literal::Set:
            num = _listSet->size();
            if (num != rhs._listSet->size()) {
                return false;
            }
            for (uint32_t i = 0; i < num; i++) {
                if (*(*_listSet)[i] != *(*rhs._listSet)[i]) {
                    return false;
                }
            }
            break;
        case Literal::Map:
            num = _map->size();
            if (num != rhs._map->size()) {
                return false;
            }
            for (uint32_t i = 0; i < num; i++) {
                const Literal::MapLiteral& m = (*_map)[i];
                const Literal::MapLiteral& rm = (*rhs._map)[i];
                if (*m._key != *rm._key || *m._value != *rm._value) {
                    return false;
                }
            }
            break;
        case Literal::Tuple:
            num = _tuple->size();
            if (num != rhs._tuple->size()) {
                return false;
            }
            for (uint32_t i = 0; i < num; i++) {
                const Literal::TupleLiteral& t = (*_tuple)[i];
                const Literal::TupleLiteral& rt = (*rhs._tuple)[i];
                if (t._id != rt._id || *t._value != *rt._value) {
                    return false;
                }
            }
            break;
        case XML:
        case Invalid:
            return false;
        case Nul:
            break;
        default:
            return *_primitives == *rhs._primitives;
    }
    return true;
}

// Note: SPL does not have bstring, bset, blist, bmap, blob literals
void Literal::generate(ostream& ostr, bool justInitializer, bool genTemplate) const
{
    TypeGenerator const& tg = TypeGenerator::instance();
    string cppName = tg.getCppType(*_type);

    if (_kind == Primitive) {
        switch (_type->getMetaType()) {
            case MetaType::USTRING: {
                const SPL::ustring& v = _primitives->get_ustring();
                if (justInitializer) {
                    ostr << '(';
                }
                ostr << cppName << "::fromUTF8(" << makeCStringLiteral(v);
                if (v.indexOf(UChar(0)) != -1) {
                    // Use the length version
                    ostr << ", " << v.length();
                }
                ostr << ')';
                if (justInitializer) {
                    ostr << ')';
                }
                break;
            }
            case MetaType::BSTRING:
            case MetaType::RSTRING: {
                const rstring& v = _primitives->get_rstring();
                if (!justInitializer) {
                    ostr << cppName;
                }
                ostr << '(' << makeCStringLiteral(v);
                if (v.find('\0') != string::npos) {
                    // Use the length version
                    ostr << ", " << v.size();
                }
                ostr << ')';
                break;
            }
            case MetaType::BOOLEAN: {
                if (justInitializer) {
                    ostr << '(' << *_primitives << ')';
                } else {
                    ostr << *_primitives;
                }
                break;
            }
            case MetaType::INT8:
            case MetaType::INT16:
            case MetaType::INT32:
            case MetaType::UINT8:
            case MetaType::UINT16: {
                if (justInitializer) {
                    ostr << *_primitives;
                } else {
                    ostr << cppName << '(' << *_primitives << ')';
                }
                break;
            }
            case MetaType::FLOAT32: {
                ostringstream tmp;
                tmp << *_primitives;
                float32 val = _primitives->get_float32();
                string res;
                if (isnan(val)) {
                    res = (val < 0) ? "-NAN" : "NAN";
                } else if (isinf(val)) {
                    res = (val < 0) ? "-INFINITY" : "INFINITY";
                } else {
                    if (tmp.str().find(".") == string::npos &&
                        tmp.str().find("e") == string::npos &&
                        tmp.str().find("E") == string::npos) {
                        tmp << ".0";
                    }
                    tmp << 'f';
                    res = tmp.str();
                }
                if (justInitializer) {
                    ostr << res;
                } else {
                    ostr << cppName << '(' << res << ')';
                }
                break;
            }
            case MetaType::FLOAT64: {
                ostringstream tmp;
                tmp << *_primitives;
                float64 val = _primitives->get_float64();
                string res;
                if (isnan(val)) {
                    res = (val < 0) ? "-NAN" : "NAN";
                } else if (isinf(val)) {
                    res = (val < 0) ? "-INFINITY" : "INFINITY";
                } else {
                    if (tmp.str().find(".") == string::npos &&
                        tmp.str().find("e") == string::npos &&
                        tmp.str().find("E") == string::npos) {
                        tmp << ".0";
                    }
                    res = tmp.str();
                }
                if (justInitializer) {
                    ostr << res;
                } else {
                    ostr << cppName << '(' << res << ')';
                }
                break;
            }
            case MetaType::UINT32: {
                assert(!justInitializer);
                ostr << cppName << '(' << *_primitives << "U)";
                break;
            }
            case MetaType::INT64: {
                assert(!justInitializer);
                ostr << cppName << '(' << *_primitives << "LL)";
                break;
            }
            case MetaType::UINT64: {
                assert(!justInitializer);
                ostr << cppName << '(' << *_primitives << "LLU)";
                break;
            }
            case MetaType::DECIMAL32:
            case MetaType::DECIMAL64:
            case MetaType::DECIMAL128: {
                assert(!justInitializer);
                ostr << cppName << "(\"" << *_primitives << "\")";
                break;
            }
            case MetaType::COMPLEX32:
            case MetaType::COMPLEX64: {
                assert(!justInitializer);
                ostr << cppName << *_primitives; // '(',')' are printed for us
                break;
            }
            case MetaType::TIMESTAMP: {
                assert(!justInitializer);
                ostr << cppName << '(' << _primitives->get_timestamp().getSeconds() << "LL,"
                     << _primitives->get_timestamp().getNanoseconds() << "u)";
                break;
            }
            case MetaType::BLOB: // no blob literals
            default:
                assert(!"cannot happen");
        }
        return;
    } else if (_kind == Expn) {
        assert(!justInitializer);
        _expn->generate(ostr, 0, Expression::General, genTemplate);
        return;
    } else if (_kind == XML) {
        const string& v = _primitives->get_rstring();
        if (!justInitializer) {
            ostr << cppName;
        }
        ostr << "(std::string(" << makeCStringLiteral(v);
        ostr << "))";
        return;
    } else if (_kind == Nul) {
        assert(NULL != _varType);

        cppName = tg.getCppType(*_varType);

        if (!justInitializer) {
            ostr << cppName;
        }

        ostr << "( )";
        return;
    }

    // We are a composite - generate .add's
    // first declare the temporary
    uint32_t num;
    switch (_kind) {
        case Literal::List:
        case Literal::Set:
            num = _listSet->size();
            if (justInitializer) {
                if (num == 0) {
                    return;
                }
                ostr << '(';
            }
            ostr << cppName << "()";
            for (uint32_t i = 0; i < num; i++) {
                ostr << ".add(";
                if (genTemplate) {
                    ostr << "{value:" << i << '}';
                } else {
                    (*_listSet)[i]->generate(ostr, false);
                }
                ostr << ')';
            }
            if (justInitializer) {
                ostr << ')';
            }
            break;
        case Literal::Map:
            num = _map->size();
            if (justInitializer) {
                if (num == 0) {
                    return;
                }
                ostr << '(';
            }
            ostr << cppName << "()";
            for (uint32_t i = 0; i < num; i++) {
                const MapLiteral& ml = (*_map)[i];
                ostr << ".add(";
                if (genTemplate) {
                    ostr << "{key:" << i << '}';
                } else {
                    ml._key->generate(ostr, false);
                }
                ostr << ',';
                if (genTemplate) {
                    ostr << "{value:" << i << '}';
                } else {
                    ml._value->generate(ostr, false);
                }
                ostr << ')';
            }
            if (justInitializer) {
                ostr << ')';
            }
            break;
        case Literal::Tuple:
            num = _tuple->size();
            if (justInitializer) {
                ostr << '(';
            } else {
                ostr << cppName << '(';
            }
            for (uint32_t i = 0; i < num; i++) {
                const Literal::TupleLiteral& t = (*_tuple)[i];
                if (i > 0) {
                    ostr << ", ";
                }
                if (genTemplate) {
                    ostr << "{attr:" << i << '}';
                } else {
                    t._value->generate(ostr, false);
                }
            }
            ostr << ')';
            break;
        default:
            assert(!"cannot happen");
            break;
    }
}

ostream& operator<<(ostream& ostr, const Literal& v)
{
    uint32_t num = v.numElements();
    switch (v.getKind()) {
        case Literal::Expn:
            ostr << v.expression();
            break;
        case Literal::List:
        case Literal::Set:
            ostr << ((v.getKind() == Literal::List) ? '[' : '{');
            for (uint32_t i = 0; i < num; i++) {
                ostr << v.listSetElement(i);
                if (i != num - 1) {
                    ostr << ", ";
                }
            }
            ostr << ((v.getKind() == Literal::List) ? ']' : '}');
            break;
        case Literal::Map:
            ostr << '{';
            for (uint32_t i = 0; i < num; i++) {
                const Literal::MapLiteral& m = v.mapElement(i);
                ostr << *m._key << " : " << *m._value;
                if (i != num - 1) {
                    ostr << ", ";
                }
            }
            ostr << '}';
            break;
        case Literal::Tuple:
            ostr << '{';
            for (uint32_t i = 0; i < num; i++) {
                const Literal::TupleLiteral& t = v.tupleElement(i);
                ostr << t._id << " = " << *t._value;
                if (i != num - 1) {
                    ostr << ", ";
                }
            }
            ostr << '}';
            break;
        case Literal::Invalid:
            ostr << "<<Invalid Literal>>";
            break;
        case Literal::Nul:
            ostr << "null";
            break;
        default: {
            const string* eName = v.enumValueName();
            if (eName) {
                ostr << *eName;
            } else {
                return serializeWithPrecision(ostr, v.primitive());
            }
            break;
        }
    }
    return ostr;
}

ostream& operator<<(ostream& ostr, Literal::Kind v)
{
    switch (v) {
        case Literal::Expn:
            ostr << "ExpnLiteral";
            break;
        case Literal::List:
            ostr << "ListLiteral";
            break;
        case Literal::Set:
            ostr << "SetLiteral";
            break;
        case Literal::Map:
            ostr << "MapLiteral";
            break;
        case Literal::Tuple:
            ostr << "TupleLiteral";
            break;
        case Literal::Invalid:
            ostr << "<<Invalid Literal>>";
            break;
        case Literal::Nul:
            ostr << "NullLiteral";
            break;
        default:
            ostr << "EnumLiteral";
            break;
    }
    return ostr;
}
}
