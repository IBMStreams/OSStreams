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

#include <SPL/FrontEnd/Scope.h>

#include <antlr3interfaces.h>

#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ModelDumper.h>
#include <SPL/Utility/SysOut.h>

namespace SPL {
Scope::Scope(Scope* inparentScope, SymbolTableEntry* inholder)
  : _entries()
  , _holder(inholder)
  , _parentScope(inparentScope)
{
    MODEL("Scope(" << (void*)this << ", " << (void*)inparentScope << ", " << (void*)inholder
                   << ")\n");
}

Scope::Scope(SourceLocation const& loc, TupleTyp const& tupleType, Scope* inparentScope)
  : _entries()
  , _holder(NULL)
  , _parentScope(inparentScope)
{
    MODEL("Scope(" << (void*)this << ", " << (void*)inparentScope << ", 0)\n");

    for (int i = 0, n = tupleType.getNumberOfAttributes(); i < n; i++) {
        std::string const& name = tupleType.getAttributeName(i);
        RootTyp const& type = tupleType.getAttributeType(i);
        def(*new AttributeFromTypeSym(loc, *this, name, type));
    }
}

void Scope::addAll(Scope const& other)
{
    for (int i = 0, n = other.size(); i < n; i++) {
        SymbolTableEntry& sym = other.get(i);
        if (!_entries.has(sym.name())) {
            _entries.put(sym.name(), &sym);
            MODEL("AddToScope(" << (void*)this << ", " << (void*)&sym << ")\n");
        }
    }
}

SymbolTableEntry& Scope::def(SymbolTableEntry& sym)
{
    std::string const& name = sym.name();
    MODEL("AddToScope(" << (void*)this << ", " << (void*)&sym << ")\n");
    if (_entries.has(name)) {
        SysOut::errorln(SPL_FRONT_END_IDENTIFIER_ALREADY_DEFINED(name), sym.sourceLocation());
        SysOut::detailsErrorln(SPL_FRONT_END_PREVIOUS_LOCATION(name), (get(name).sourceLocation()));
    } else {
        _entries.put(name, &sym);
#if !NDEBUG
        if (sym.hasAst()) {
            SymbolTableEntry* e = astSymbolPtr(sym.ast());
            if (e && e != &sym) {
                std::cerr << "Sym/AST mismatch, ast: " << sym.ast() << "\n   ast sym: " << *e
                          << "\n   sym: " << sym << std::endl;
                assert(false);
            }
        }
#endif
    }
    return sym;
}

OtherSym& Scope::def(SourceLocation const& loc, AstNode* defNode, std::string const name)
{
    return def(*new OtherSym(loc, name, this, defNode, NULL)).as<OtherSym>();
}

SymbolTableEntry& Scope::get(int i) const
{
    SymbolTableEntry* sym = _entries.get(i);
    assert(NULL != sym);
    return *sym;
}

SymbolTableEntry& Scope::get(std::string const& name) const
{
    assert(has(name));
    return *_entries.get(name);
}

bool Scope::has(std::string const& name) const
{
    return _entries.has(name);
}

std::ostream& Scope::print(std::ostream& os) const
{
    os << "Scope(names={";
    for (int i = 0, n = size(); i < n; i++) {
        if (0 < i) {
            os << ", ";
        }
        os << _entries.getKey(i);
    }
    os << "}, chain=[";
    for (Scope const* s = this; NULL != s; s = s->parentScope()) {
        if (s != this) {
            os << ", ";
        }
        SymbolTableEntry const* holder1 = s->holder();
        if (NULL == holder1) {
            os << "anonymousScope";
        } else if (holder1->is<TupleAttribSym>() || holder1->is<TupleExtendSym>() ||
                   holder1->is<TupleLiteralSym>()) {
            os << "anonymousTuple";
        } else if (holder1->is<OtherScopeHolderSym>()) {
            os << "otherScope";
        } else {
            os << holder1->name();
        }
    }
    return os << "])";
}

// .................................................................
std::ostream& operator<<(std::ostream& os, Scope const* scope)
{
    if (NULL == scope) {
        return os << "NULL_SCOPE";
    }
    return os << *scope;
}

std::ostream& operator<<(std::ostream& os, Scope const& scope)
{
    return scope.print(os);
}

void addEnumConstantsToScope(Scope& scope, RootTyp const& type)
{
    switch (type.getMetaType()) {
        case MetaType::BOOLEAN:
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:
        case MetaType::FLOAT32:
        case MetaType::FLOAT64:
        case MetaType::DECIMAL32:
        case MetaType::DECIMAL64:
        case MetaType::DECIMAL128:
        case MetaType::COMPLEX32:
        case MetaType::COMPLEX64:
        case MetaType::TIMESTAMP:
        case MetaType::RSTRING:
        case MetaType::BSTRING:
        case MetaType::USTRING:
        case MetaType::BLOB:
        case MetaType::XML:
            return;
        case MetaType::LIST:
            addEnumConstantsToScope(scope, type.as<ListTyp>().getElementType());
            return;
        case MetaType::BLIST:
            addEnumConstantsToScope(scope, type.as<BListTyp>().getElementType());
            return;
        case MetaType::SET:
            addEnumConstantsToScope(scope, type.as<SetTyp>().getElementType());
            return;
        case MetaType::BSET:
            addEnumConstantsToScope(scope, type.as<BSetTyp>().getElementType());
            return;
        case MetaType::MAP:
            addEnumConstantsToScope(scope, type.as<MapTyp>().getKeyType());
            return;
            addEnumConstantsToScope(scope, type.as<MapTyp>().getValueType());
            return;
        case MetaType::BMAP:
            addEnumConstantsToScope(scope, type.as<BMapTyp>().getKeyType());
            return;
            addEnumConstantsToScope(scope, type.as<BMapTyp>().getValueType());
            return;
        case MetaType::TUPLE: {
            TupleTyp const& ttype = type.as<TupleTyp>();
            for (int i = 0, n = ttype.getNumberOfAttributes(); i < n; i++) {
                addEnumConstantsToScope(scope, ttype.getAttributeType(i));
            }
            return;
        }
        case MetaType::ENUM: {
            std::vector<std::string> const& values = type.as<EnumTyp>().getValidValues();
            for (int i = 0, n = values.size(); i < n; i++) {
                std::string const& name = values.at(i);
                scope.def(
                  *new OtherSym(SourceLocation::invalidSourceLocation, name, &scope, NULL, &type));
            }
            return;
        }
        default:
            std::cerr << "Unexpected meta-type " << type << std::endl;
            assert(false);
    }
}

Scope* typeToScope(SourceLocation const& loc, RootTyp const& type, Scope* scope)
{
    if (NULL != scope) {
        return scope;
    }
    if (type.is<TupleTyp>()) {
        return new Scope(loc, type.as<TupleTyp>(), NULL);
    }
    return NULL;
}
};
