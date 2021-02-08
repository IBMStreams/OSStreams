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

#include <SPL/FrontEnd/GenericsUnifier.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <UTILS/DistilleryApplication.h>

using namespace Distillery;

namespace SPL {
class GenericsUnifierTest : public DistilleryApplication
{
    int _nFailures;

    void T(const std::string& expected, RootTyp const* const x, RootTyp const* const y)
    {
        std::auto_ptr<GenericsUnifier const> gu(&GenericsUnifier::unify(*x, *y));
        std::string actual;
        if (gu->isFailure()) {
            actual = "failure";
        } else {
            std::ostringstream os;
            os << gu->substitute(*x);
            actual = os.str();
        }
        if (expected != actual) {
            std::cout << "unification failure" << std::endl;
            std::cout << "         x == " << x << std::endl;
            std::cout << "         y == " << y << std::endl;
            std::cout << "  expected == " << expected << std::endl;
            std::cout << "    actual == " << actual << std::endl;
            _nFailures++;
        }
    }

  public:
    GenericsUnifierTest() {}

    virtual int run(std::vector<std::string> const& /*remains*/)
    {
        TypeFactory factory;

        _nFailures = 0;
        TypeFactory* f = &TypeFactory::instance();
        RootTyp const* const tInt32 = &f->primitiveT("int32");
        RootTyp const* const tComplex64 = &f->primitiveT("complex64");
        RootTyp const* const tFloat32 = &f->primitiveT("float32");
        RootTyp const* const tListInt32 = &f->listT(*tInt32);
        TypeFormal const* const tAnyT = &f->typeFormalT(NULL, "any", "T");
        TypeFormal const* const tAnyU = &f->typeFormalT(NULL, "any", "U");
        TypeFormal const* const tOrderedO = &f->typeFormalT(NULL, "ordered", "I");
        TypeFormal const* const tCollectionC = &f->typeFormalT(NULL, "collection", "C");
        RootTyp const* const tListAnyT = &f->listT(*tAnyT);
        RootTyp const* const tListAnyU = &f->listT(*tAnyU);
        BoundsFormal const* const N = new BoundsFormal(NULL, "N");
        BoundsFormal const* const K = new BoundsFormal(NULL, "K");
        // ---- no variables
        T("int32", tInt32, tInt32);
        T("failure", tInt32, tFloat32);
        T("list<int32>", tListInt32, tListInt32);
        T("failure", tListInt32, tInt32);
        T("failure", tInt32, tListInt32);
        // ---- single type variable
        T("int32", tAnyT, tInt32);
        T("list<int32>", tAnyT, tListInt32);
        T("T", tAnyT, tAnyT);
        T("list<int32>", tListInt32, tListAnyT);
        T("failure", tListAnyT, tInt32);
        T("list<T>", tListAnyT, tListAnyT);
        // ---- this one should fail in the "occurs" check
        T("failure", tListAnyT, tAnyT);
        // ---- two type variables
        T("list<U>", tListAnyT, tListAnyU);
        T("list<U>", tAnyT, tListAnyU);
        T("list<T>", tAnyU, tListAnyT);
        // ---- single type variable but multiple matches
        T("map<int32,int32>", &f->mapT(*tAnyT, *tAnyT), &f->mapT(*tInt32, *tInt32));
        T("failure", &f->mapT(*tAnyT, *tAnyT), &f->mapT(*tInt32, *tFloat32));
        T("map<int32,int32>", &f->mapT(*tAnyT, *tInt32), &f->mapT(*tInt32, *tAnyT));
        T("failure", &f->mapT(*tInt32, *tAnyT), &f->mapT(*tAnyT, *tFloat32));
        T("map<list<int32>,int32>", &f->mapT(*tListAnyT, *tAnyT), &f->mapT(*tListInt32, *tInt32));
        T("map<list<int32>,int32>", &f->mapT(*tListAnyT, *tInt32), &f->mapT(*tListInt32, *tAnyT));
        T("failure", &f->mapT(*tListAnyT, *tFloat32), &f->mapT(*tListInt32, *tAnyT));
        // ---- two type variables, multiple matches
        T("map<int32,map<int32,int32>>", &f->mapT(*tAnyT, f->mapT(*tAnyU, *tAnyT)),
          &f->mapT(*tAnyU, f->mapT(*tInt32, *tInt32)));
        T("failure", &f->mapT(*tAnyT, f->mapT(*tAnyU, *tAnyT)),
          &f->mapT(*tAnyU, f->mapT(*tInt32, *tFloat32)));
        // ---- single bounds variable
        T("rstring[42]", &f->rstringT(42), &f->rstringT(N));
        T("rstring[99]", &f->rstringT(99), &f->rstringT(N));
        T("rstring[N]", &f->rstringT(N), &f->rstringT(N));
        T("map<rstring[13],rstring[13]>", &f->mapT(f->rstringT(N), f->rstringT(N)),
          &f->mapT(f->rstringT(13), f->rstringT(13)));
        T("failure", &f->mapT(f->rstringT(N), f->rstringT(N)),
          &f->mapT(f->rstringT(13), f->rstringT(14)));
        // ---- two bounds variables, multiple matches
        T("map<rstring[99],rstring[99]>[99]", &f->mapT(f->rstringT(N), f->rstringT(K), N),
          &f->mapT(f->rstringT(K), f->rstringT(99), 99));
        // ---- type variables with constraints
        T("int32", tInt32, tOrderedO);
        T("int32", tOrderedO, tInt32);
        T("failure", tListInt32, tOrderedO);
        T("list<int32>", tListInt32, tCollectionC);
        T("list<T>", tListAnyT, tCollectionC);
        T("failure", tInt32, tCollectionC);
        T("map<int32,map<int32,int32>>", &f->mapT(*tOrderedO, f->mapT(*tAnyU, *tOrderedO)),
          &f->mapT(*tAnyU, f->mapT(*tInt32, *tInt32)));
        T("failure", &f->mapT(*tOrderedO, f->mapT(*tAnyU, *tOrderedO)),
          &f->mapT(*tAnyU, f->mapT(*tComplex64, *tComplex64)));
        T("map<complex64,map<complex64,complex64>>", &f->mapT(*tAnyT, f->mapT(*tAnyU, *tAnyT)),
          &f->mapT(*tAnyU, f->mapT(*tComplex64, *tComplex64)));
        if (_nFailures > 0) {
            std::cout << std::endl << "There were " << _nFailures << " failures." << std::endl;
        }
        return _nFailures;
    }
};
};

MAIN_APP(SPL::GenericsUnifierTest)
