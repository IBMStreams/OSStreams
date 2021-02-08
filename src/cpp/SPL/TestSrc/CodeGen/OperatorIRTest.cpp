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

#include "../Utility/UtlTestCommon.h"

#include <SPL/CodeGen/CodeGenHelper.h>
#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/ExportSpec.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/ImportSpec.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/PlacementConstraint.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/FrontEnd/UsedCompositeDefinitions.h>
#include <SPL/FrontEnd/UsedOperatorInvocations.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Utility/SourceLocation.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class OperatorIRTest : public CompilerApp
{
  public:
    OperatorIRTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        // Some basic types
        uint32 a;
        const RootTyp& btu32 = RootTyp::makeType(a);

        Expression* e;

        // basic expression:  4
        Expression* e1 = new LiteralExpression(btu32, *new SourceLocation("f2.spl", 4, 6),
                                               *new Literal(btu32, new ExpressionValue(uint32(4))));

        // Define some Composite Operators
        UsedCompositeDefinitions& cdt = UsedCompositeDefinitions::instance();

        CompositeDefinition& Main = cdt.add("spl.MyNs::Main");
        Main.setStartLocation(*new SourceLocation("f2.spl", 6, 6));
        Main.setMain(true);
        Main.setEndLocation(*new SourceLocation("f2.spl", 6, 8));
        CompositeDefinition& MyOp = cdt.add("spl.MyNs::MyOp");
        MyOp.setStartLocation(*new SourceLocation("f2.spl", 7, 6));
        MyOp.setEndLocation(*new SourceLocation("f2.spl", 7, 9));

        // Now define some composite operator instances
        CompositeOperatorInstance* mainCompOperInstance =
          new CompositeOperatorInstance(Main.getIndex(), NULL);
        CompositeOperatorInstance* myCompOperInstance =
          &mainCompOperInstance->createNestedCompOperInstance(MyOp.getIndex());

        PrimitiveOperatorInstance* o = new PrimitiveOperatorInstance(
          *new SourceLocation("f2.spl", 6, 6), *new SourceLocation("f2.spl", 7, 7), "Testing",
          "spl.relational.Functor", myCompOperInstance, 0);
        myCompOperInstance->addPrimitiveInstance(*o);

        cdt.print(cout);
        mainCompOperInstance->print(cout);
        myCompOperInstance->print(cout);
        cout << endl;

        // Populate
        typedef tuple<rstring FIELD(name)> Person;
        Person p;

        const TupleTyp* pType = &(const TupleTyp&)RootTyp::makeType(p);
        const SourceLocation& loc = *new SourceLocation("test.foo", 5, 6);
        PrimitiveOperatorInstance::Port* s1 = new PrimitiveOperatorInstance::Port(pType, loc);
        s1->addStream("MyStream");
        PrimitiveOperatorInstance::Port* s2 = new PrimitiveOperatorInstance::Port(pType, loc);
        s2->addStream("OutStream");
        PrimitiveOperatorInstance::Port* s3 = new PrimitiveOperatorInstance::Port(pType, loc);
        s3->addStream("OutStream2");

        s1->setAlias("a1");
        s1->addStream("i2");
        s1->addStream("fred");
        s3->setAlias("O");

        o->addInputPort(*s1);
        o->addOutputPort(*s2);

        PrimitiveOperatorInstance::Logic& l = o->getLogic();

        l.setState(
          *new DeclStatement(*new SourceLocation("f2.spl", 5, 6), true, btu32, "a", &e1->clone()));
        l.addTuplePort(0,
                       *new ExpressionStatement(
                         *new SourceLocation("f2.spl", 5, 6),
                         *new BinaryExpression(
                           btu32, *new SourceLocation("f2.spl", 5, 6), Expression::plusEq,
                           *new SymbolExpression(btu32, *new SourceLocation("f2.spl", 5, 6), "a"),
                           e1->clone())));
        l.addTuplePort(1,
                       *new ExpressionStatement(
                         *new SourceLocation("f2.spl", 5, 6),
                         *new BinaryExpression(
                           btu32, *new SourceLocation("f2.spl", 5, 6), Expression::starEq,
                           *new SymbolExpression(btu32, *new SourceLocation("f2.spl", 5, 6), "a"),
                           e1->clone())));
        l.addPunctPort(1,
                       *new ExpressionStatement(
                         *new SourceLocation("f2.spl", 5, 6),
                         *new BinaryExpression(
                           btu32, *new SourceLocation("f2.spl", 5, 6), Expression::starEq,
                           *new SymbolExpression(btu32, *new SourceLocation("f2.spl", 5, 6), "x"),
                           e1->clone())));

        // Windows - Tumbling
        o->addWindow(*new PrimitiveOperatorInstance::Window(
          loc, "zap", 0, PrimitiveOperatorInstance::Window::Count, NULL, &e1->clone()));
        o->addWindow(*new PrimitiveOperatorInstance::Window(
          loc, "zip", 1, PrimitiveOperatorInstance::Window::Delta,
          new SymbolExpression(btu32, *new SourceLocation("f2.spl", 5, 6), "a"), &e1->clone()));
        o->addWindow(*new PrimitiveOperatorInstance::Window(
          loc, "zap2", 2, PrimitiveOperatorInstance::Window::Time, NULL, &e1->clone()));
        o->addWindow(*new PrimitiveOperatorInstance::Window(
          loc, "zap3", 3, PrimitiveOperatorInstance::Window::Punct, NULL, NULL));

        // Windows - Sliding
        o->addWindow(*new PrimitiveOperatorInstance::Window(
          loc, "szap", 3, PrimitiveOperatorInstance::Window::Count,
          PrimitiveOperatorInstance::Window::Delta, NULL, &e1->clone(),
          new SymbolExpression(btu32, *new SourceLocation("f2.spl", 5, 6), "a"), &e1->clone()));
        o->addWindow(*new PrimitiveOperatorInstance::Window(
          loc, "szip", 4, PrimitiveOperatorInstance::Window::Delta,
          PrimitiveOperatorInstance::Window::Count,
          new SymbolExpression(btu32, *new SourceLocation("f2.spl", 5, 6), "a"), &e1->clone(), NULL,
          &e1->clone()));
        o->addWindow(*new PrimitiveOperatorInstance::Window(
          loc, "szap2", 5, PrimitiveOperatorInstance::Window::Time,
          PrimitiveOperatorInstance::Window::Time, NULL, &e1->clone(), NULL, &e1->clone()));

        std::vector<const Expression*>* expnList = new std::vector<const Expression*>;
        expnList->push_back(&e1->clone());
        expnList->push_back(&e1->clone());
        o->addParam(*new Param("match", *new SourceLocation("f2.spl", 5, 6), *expnList));

        // Outputs to a stream
        PrimitiveOperatorInstance::Output* out = &o->getOutput(0);
        out->add(*new BinaryExpression(
          btu32, *new SourceLocation("f2.spl", 5, 6), Expression::eq,
          *new SymbolExpression(btu32, *new SourceLocation("f2.spl", 5, 6), "a"), e1->clone()));
        out->add(*new BinaryExpression(
          btu32, *new SourceLocation("f2.spl", 5, 6), Expression::eq,
          *new SymbolExpression(btu32, *new SourceLocation("f2.spl", 5, 6), "name"), e1->clone()));

        // Configs
        o->addConfig(Config::fromCheckpoint(loc, 5.12, false));

        // config defaultPoolPolicy : compileTime
        // config defaultPoolSize : size(5);
        o->addConfig(Config::fromDefaultPool(loc, Config::RunTime, true, 5));
        o->addConfig(Config::fromLogLevel(loc, Config::Debug));
        o->addConfig(Config::fromRelocatable(loc, false));
        o->addConfig(Config::fromRestartable(loc, true));
        o->addConfig(Config::fromInputTransport(loc, Config::TCP));
        o->addConfig(Config::fromOutputTransport(loc, Config::TCP));

        // Hosts
        std::vector<Config::OneHostPool>* hosts = new std::vector<Config::OneHostPool>();
        set<rstring> ss;
        rstring str;
        Literal* s = new Literal(RootTyp::makeType(ss), NULL);
        s->addSetElement(
          *new Literal(RootTyp::makeType(str), new ExpressionValue(rstring("hostA"))));
        s->addSetElement(
          *new Literal(RootTyp::makeType(str), new ExpressionValue(rstring("1.2.3.5"))));
        hosts->push_back(Config::OneHostPool("compTime", 15));
        hosts->push_back(Config::OneHostPool("aRuntime", *s));
        o->addConfig(Config::fromHostPools(loc, *hosts));

        PlacementConstraint* pc = new PlacementConstraint();
        pc->setHostPlacement(PlacementConstraint::PoolCompileTime, 1, 0);
        pc->addPlacementConstraint(PlacementConstraint::PartitionColocation, "placement #1");
        pc->addPlacementConstraint(PlacementConstraint::PartitionExlocation, "ex #1");
        pc->addPlacementConstraint(PlacementConstraint::HostColocation, "B");
        pc->addPlacementConstraint(PlacementConstraint::HostExlocation, "A");
        pc->setPartitionIsolation(true);
        ;
        pc->setHostIsolation(true);

        o->addConfig(Config::fromPlacementConstraint(loc, *pc));
        // Print
        o->print(std::cout);

        UsedOperatorInvocations& uo = UsedOperatorInvocations::instance();
        uo.add(*o);

        // Add some more operators
        PrimitiveOperatorInstance* o2 = new PrimitiveOperatorInstance(
          *new SourceLocation("f.spl", 5, 6), *new SourceLocation("f.spl", 5, 6), "Op2",
          "spl.relational::Functor", myCompOperInstance, 0);
        PrimitiveOperatorInstance::Port* pr = new PrimitiveOperatorInstance::Port(pType, loc);
        o2->addOutputPort(*pr);
        pr->addStream("MyStream");
        pr = new PrimitiveOperatorInstance::Port(pType, loc);
        o2->addInputPort(*pr);
        pr->addStream("OutStream");
        pr = new PrimitiveOperatorInstance::Port(pType, loc);
        o2->addInputPort(*pr);
        pr->addStream("OutStream2");
        o2->print(cout);
        uo.add(*o2);

        // Check the config lookups
        const Config* config = o->findConfig(Config::Wrapper);
        if (config) {
            cout << "unexpected lookup of Wrapper: " << *config << endl;
            FASSERT(config == NULL);
        }
        cout << "as expected, didn't find wrapper\n";

        // Put wrapper into Main definition
        Main.addConfig(Config::fromWrapper(loc, "valgrind"));
        config = o->findConfig(Config::Wrapper);
        FASSERT(config != NULL);
        cout << "found " << *config << endl;
        FASSERT(config->getWrapper() == "valgrind");

        // Put wrapper into MyOp definition
        MyOp.addConfig(Config::fromWrapper(loc, "gdb"));
        config = o->findConfig(Config::Wrapper);
        FASSERT(config != NULL);
        cout << "found " << *config << endl;
        FASSERT(config->getWrapper() == "gdb");

        // Put wrapper into MyOp scope
        myCompOperInstance->addConfig(Config::fromWrapper(loc, "x11"));
        config = o->findConfig(Config::Wrapper);
        FASSERT(config != NULL);
        cout << "found " << *config << endl;
        FASSERT(config->getWrapper() == "x11");

        // And finally into the primitive itself
        o->addConfig(Config::fromWrapper(loc, "nrgdb"));
        config = o->findConfig(Config::Wrapper);
        FASSERT(config != NULL);
        cout << "found " << *config << endl;
        FASSERT(config->getWrapper() == "nrgdb");

        cdt.print(cout);
        mainCompOperInstance->print(cout);
        myCompOperInstance->print(cout);
        cout << endl;

        // Test ImportSpecs;
        PrimitiveOperatorInstance* XXOO;
        std::vector<const Expression*>* elist;
        const RootTyp& strType = RootTyp::makeType(str);

        XXOO = new PrimitiveOperatorInstance(*new SourceLocation("f.spl", 5, 6),
                                             *new SourceLocation("f.spl", 5, 6), "Fred",
                                             "spl.nothing", myCompOperInstance, 0);
        // Make up some Params
        elist = new std::vector<const Expression*>;
        elist->push_back(new LiteralExpression(
          strType, loc,
          *new Literal(strType, new ExpressionValue(rstring("name.space::app.name")))));
        XXOO->addParam(*(new Param("applicationName", loc, *elist)));
        elist = new std::vector<const Expression*>;
        elist->push_back(new LiteralExpression(
          strType, loc, *new Literal(strType, new ExpressionValue(rstring("appScope")))));
        XXOO->addParam(*(new Param("applicationScope", loc, *elist)));

        // Expect an error
        const ImportSpec* i = ImportSpec::createFromImportOper(*XXOO);
        FASSERT(i == NULL);

        // Add a subscription
        elist = new std::vector<const Expression*>;
        elist->push_back(new BinaryExpression(
          strType, loc, Expression::eq, *new SymbolExpression(strType, loc, "streamId"),
          *new LiteralExpression(
            strType, loc, *new Literal(strType, new ExpressionValue(rstring("stream.name"))))));
        XXOO->addParam(*(new Param("subscription", loc, *elist)));
        i = ImportSpec::createFromImportOper(*XXOO);
        FASSERT(i);
        i->print(cout);

        delete XXOO;

        XXOO = new PrimitiveOperatorInstance(*new SourceLocation("f.spl", 5, 6),
                                             *new SourceLocation("f.spl", 5, 6), "Fred",
                                             "spl.nothing", myCompOperInstance, 0);

        // Make an XPATH expression instead
        // e1 =  IBM in companies
        boolean b;
        e1 = new BinaryExpression(
          RootTyp::makeType(b), loc, Expression::in,
          *new LiteralExpression(strType, loc,
                                 *new Literal(strType, new ExpressionValue(rstring("IBM")))),
          *new SymbolExpression(strType, loc, "companies"));
        // e2 = rank < 5
        int64 x;
        const RootTyp& bti64 = RootTyp::makeType(a);
        Expression* e2 = new BinaryExpression(
          RootTyp::makeType(b), loc, Expression::less, *new SymbolExpression(bti64, loc, "rank"),
          *new LiteralExpression(bti64, loc, *new Literal(bti64, new ExpressionValue(int64(5)))));
        // e3 = foo[5] > 100
        Expression* e3 = new BinaryExpression(
          RootTyp::makeType(b), loc, Expression::greater,
          *new SubscriptExpression(
            bti64, loc, *new SymbolExpression(bti64, loc, "sym"), false,
            new LiteralExpression(bti64, loc, *new Literal(bti64, new ExpressionValue(int64(5)))),
            NULL),
          *new LiteralExpression(bti64, loc, *new Literal(bti64, new ExpressionValue(int64(100)))));
        e = new BinaryExpression(RootTyp::makeType(b), loc, Expression::ampAmp, *e1, *e2);
        e = new BinaryExpression(RootTyp::makeType(b), loc, Expression::barBar, *e, *e3);
        elist = new std::vector<const Expression*>;
        elist->push_back(&e->clone());

        XXOO->addParam(*(new Param("subscription", loc, *elist)));
        i = ImportSpec::createFromImportOper(*XXOO);
        FASSERT(i);
        i->print(cout);

        delete XXOO;

        // Add an import operator.   We can use the 'e' from above
        PrimitiveOperatorInstance* o4 = new PrimitiveOperatorInstance(
          *new SourceLocation("f2.spl", 5, 6), *new SourceLocation("f2.spl", 6, 6), "Sam",
          "spl.adapter::Import", myCompOperInstance, 0);
        o4->addOutputPort(*s3); // OutStream2
        elist = new std::vector<const Expression*>;
        elist->push_back(e);
        o4->addParam(*new Param("subscription", loc, *elist));

        cout << '\n';
        o4->print(cout);
        cout << '\n';
        uo.add(*o4);

        OperatorGraph og;
        og.populate(_frontEnd->getUsedOperatorInvocations(), 0, false);
        og.print(cout);

        delete mainCompOperInstance;

        return EXIT_SUCCESS;
    }
};

};

MAIN_APP(SPL::OperatorIRTest)
