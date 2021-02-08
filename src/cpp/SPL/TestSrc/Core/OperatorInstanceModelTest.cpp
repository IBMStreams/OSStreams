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

#include <SPL/Core/CompilerException.h>
#include <SPL/Core/OperatorInstanceModelImpl.h>
#include <UTILS/DistilleryApplication.h>

#include <fstream>
#include <string>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Operator::Instance;
using namespace opimodel;

class OperatorInstanceModelTest : public UTILS_NAMESPACE::DistilleryApplication
{
  public:
    OperatorInstanceModelTest() {}

    void getArguments(option_vector_t& options) { setKeepRemainingArguments(); }
    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        OperatorInstanceModel model;
        populateModel(model);
        auto_ptr<operatorInstanceModelType> modelXml = model.toXsdInstance();
        xml_schema::namespace_infomap map;
        map[""].name = "http://www.ibm.com/xmlns/prod/streams/spl/operator/instance";
        map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
        xmlns::prod::streams::spl::operator_::instance::operatorInstanceModel(cout, *modelXml, map);
        return EXIT_SUCCESS;
    }
    void populateModel(OperatorInstanceModel& model)
    {
        populateContext(model.getContext());

        model.getParameters().push_back(ParameterPtr(new Parameter()));
        model.getParameters().push_back(ParameterPtr(new Parameter()));
        model.getParameters().push_back(ParameterPtr(new Parameter()));
        model.getParameters().push_back(ParameterPtr(new Parameter()));
        populateEqualityLHSParameter(*model.getParameters()[0]);
        populateEqualityRHSParameter(*model.getParameters()[1]);
        populateMatchParameter(*model.getParameters()[2]);
        populateStyleParameter(*model.getParameters()[3]);

        model.getInputPorts().push_back(InputPortPtr(new InputPort()));
        model.getInputPorts().push_back(InputPortPtr(new InputPort()));
        populateInputPort0(*model.getInputPorts()[0]);
        populateInputPort1(*model.getInputPorts()[1]);

        model.getOutputPorts().push_back(OutputPortPtr(new OutputPort()));
        populateOutputPort0(*model.getOutputPorts()[0]);
    }
    void populateContext(Context& context)
    {
        context.getKind() = "spl.Join";
        context.getClass() = "Bidder::MatchedBids";
        context.getOptimized() = true;
        context.getVerbose() = false;
        context.getNetworkByteOrdering() = false;
        context.getCodeBeautification() = false;
        context.getIncludes().push_back("/opt/Distillery/include/SPL/Runtime/SPLFunctions.h");
        context.getIncludes().push_back("/usr/home/bgedik/spl/MyFunctions.h");
        context.getStateVariables().push_back(StateVariablePtr(new StateVariable()));
        populateStateVariable(*context.getStateVariables()[0]);
        context.getRuntimeConstants().push_back(RuntimeConstantPtr(new RuntimeConstant()));
        populateRuntimeConstant(*context.getRuntimeConstants()[0]);
        context.getSourceLocation() = SourceLocation("foof", 99, 98);
    }
    void populateRuntimeConstant(RuntimeConstant& rconst)
    {
        rconst.getName() = "rconst1";
        rconst.getCppType() = "SPL::int32";
        rconst.getSPLType() = "int32";
        rconst.getValue() = "5";
    }
    void populateStateVariable(StateVariable& svar)
    {
        svar.getName() = "svar1";
        svar.getCppType() = "SPL::int32";
        svar.getSPLType() = "int32";
        Expression* exp0 = new Expression();
        exp0->getCppType() = "SPL::int32";
        exp0->getSPLType() = "int32";
        exp0->getHasRuntimeConstant() = true;
        exp0->getCppExpression() = "10";
        exp0->getSPLExpression() = "10";
        exp0->getHasFunctions() = false;
        exp0->getHasSideEffects() = false;
        exp0->getHasStreamAttributes() = false;
        exp0->getSourceLocation() = SourceLocation("foo", 10, 57);
        svar.setValue(ExpressionPtr(exp0));
        svar.getSourceLocation() = SourceLocation("barf", 66, 55);
    }
    void populateEqualityLHSParameter(Parameter& param)
    {
        param.getName() = "EqualityLHS";
        Expression* exp0 = new Expression();
        exp0->getCppType() = "SPL::int32";
        exp0->getSPLType() = "int32";
        exp0->getHasRuntimeConstant() = false;
        exp0->getCppExpression() = "iport0_.get_id()";
        exp0->getSPLExpression() = "Bids.id";
        exp0->getHasFunctions() = false;
        exp0->getHasSideEffects() = false;
        exp0->getHasStreamAttributes() = true;
        exp0->getSourceLocation() = SourceLocation("fo", 11, 58);
        param.getValues().push_back(ExpressionPtr(exp0));
        Expression* exp1 = new Expression();
        exp1->getCppType() = "string";
        exp1->getSPLType() = "string";
        exp1->getHasRuntimeConstant() = false;
        exp1->getCppExpression() = "iport0_.get_name()";
        exp1->getSPLExpression() = "Bids.name";
        exp1->getHasFunctions() = false;
        exp1->getHasSideEffects() = false;
        exp1->getHasStreamAttributes() = true;
        exp1->getSourceLocation() = SourceLocation("bar", 11, 37);
        param.getValues().push_back(ExpressionPtr(exp1));
        param.getSourceLocation() = SourceLocation("farb", 33, 44);
    }
    void populateEqualityRHSParameter(Parameter& param)
    {
        param.getName() = "EqualityRHS";
        Expression* exp0 = new Expression();
        exp0->getCppType() = "int32";
        exp0->getSPLType() = "int32";
        exp0->getHasRuntimeConstant() = false;
        exp0->getCppExpression() = "iport1_.get_id()";
        exp0->getSPLExpression() = "Quotes.id";
        exp0->getHasFunctions() = false;
        exp0->getHasSideEffects() = false;
        exp0->getHasStreamAttributes() = true;
        exp0->getSourceLocation() = SourceLocation("foob", 13, 17);
        param.getValues().push_back(ExpressionPtr(exp0));
        Expression* exp1 = new Expression();
        exp1->getCppType() = "string";
        exp1->getSPLType() = "string";
        exp1->getHasRuntimeConstant() = false;
        exp1->getCppExpression() = "iport1_.get_name()";
        exp1->getSPLExpression() = "Quotes.name";
        exp1->getHasFunctions() = false;
        exp1->getHasSideEffects() = false;
        exp1->getHasStreamAttributes() = true;
        exp1->getSourceLocation() = SourceLocation("foobar", 12, 17);
        param.getValues().push_back(ExpressionPtr(exp1));
        param.getSourceLocation() = SourceLocation("afrb", 22, 11);
    }
    void populateMatchParameter(Parameter& param)
    {
        param.getName() = "Match";
        Expression* exp0 = new Expression();
        exp0->getCppType() = "boolean";
        exp0->getSPLType() = "boolean";
        exp0->getHasRuntimeConstant() = false;
        exp0->getCppExpression() = "iport0_.get_sell() < iport1_.get_buy()";
        exp0->getSPLExpression() = "Quotes.sell < Bids.buy";
        exp0->getHasFunctions() = false;
        exp0->getHasSideEffects() = false;
        exp0->getHasStreamAttributes() = true;
        exp0->getSourceLocation() = SourceLocation("barfoo", 13, 7);
        param.getValues().push_back(ExpressionPtr(exp0));
        param.getSourceLocation() = SourceLocation("arfb", 23, 14);
    }
    void populateStyleParameter(Parameter& param)
    {
        param.getName() = "Style";
        Expression* exp0 = new Expression();
        exp0->getCppType() = "int32";
        exp0->getSPLType() = "CustomLiteral";
        exp0->getHasRuntimeConstant() = false;
        exp0->getCppExpression() = "";
        exp0->getSPLExpression() = "LeftOuterJoin";
        exp0->getHasFunctions() = false;
        exp0->getHasSideEffects() = false;
        exp0->getHasStreamAttributes() = false;
        exp0->getSourceLocation() = SourceLocation("barbar", 14, 9);
        param.getValues().push_back(ExpressionPtr(exp0));
        param.getSourceLocation() = SourceLocation("ffff", 123, 114);
    }
    void populateInputPort0(InputPort& port)
    {
        port.getIndex() = 0;
        port.getHasHistory() = true;
        port.getHistory() = 3;
        port.getCppTupleName() = "iport0_";
        port.getCppTupleType() = "IPort0";
        port.getSPLTupleType() = "tuple<int id,string name,float sell>";
        port.getFacade() = false;
        port.getMutationAllowed() = true;
        port.setWindow(WindowPtr(new Window()));
        port.setTupleLogic("tuple logic");
        populateWindow0(port.getWindow());
        port.getAttributes().push_back(InputAttributePtr(new InputAttribute()));
        port.getAttributes().push_back(InputAttributePtr(new InputAttribute()));
        port.getAttributes().push_back(InputAttributePtr(new InputAttribute()));
        populateInput0Attribute_Id(*port.getAttributes()[0]);
        populateInput0Attribute_Name(*port.getAttributes()[1]);
        populateInput0Attribute_Sell(*port.getAttributes()[2]);
        port.getSourceLocation() = SourceLocation("aaaa", 223, 214);
    }
    void populateInputPort1(InputPort& port)
    {
        port.getIndex() = 1;
        port.getHasHistory() = false;
        port.getHistory() = 1;
        port.getCppTupleName() = "iport1_";
        port.getCppTupleType() = "IPort1";
        port.getSPLTupleType() = "tuple<int id,string name,float buy>";
        port.getFacade() = true;
        port.getMutationAllowed() = false;
        port.setWindow(WindowPtr(new Window()));
        port.setPunctLogic("punct logic");
        populateWindow1(port.getWindow());
        port.getAttributes().push_back(InputAttributePtr(new InputAttribute()));
        port.getAttributes().push_back(InputAttributePtr(new InputAttribute()));
        port.getAttributes().push_back(InputAttributePtr(new InputAttribute()));
        populateInput1Attribute_Id(*port.getAttributes()[0]);
        populateInput1Attribute_Name(*port.getAttributes()[1]);
        populateInput1Attribute_Buy(*port.getAttributes()[2]);
        port.getSourceLocation() = SourceLocation("aabb", 323, 314);
    }
    void populateWindow0(Window& window)
    {
        window.getPartitioned() = true;
        window.getEvictionPolicy().getStyle() = WindowStyle::time;
        Expression* exp0 = new Expression();
        exp0->getCppType() = "int32";
        exp0->getSPLType() = "int32";
        exp0->getHasRuntimeConstant() = true;
        exp0->getCppExpression() = "runtimeConstant21_";
        exp0->getSPLExpression() = "6";
        exp0->getHasFunctions() = false;
        exp0->getHasSideEffects() = false;
        exp0->getHasStreamAttributes() = false;
        exp0->getSourceLocation() = SourceLocation("foofoo", 18, 19);
        window.getEvictionPolicy().setSize(ExpressionPtr(exp0));
        WindowPolicy* slide = new TriggerPolicy();
        slide->getStyle() = WindowStyle::count;
        Expression* exp1 = new Expression();
        exp1->getCppType() = "int32";
        exp1->getSPLType() = "int32";
        exp1->getHasRuntimeConstant() = true;
        exp1->getCppExpression() = "runtimeConstant22_";
        exp1->getSPLExpression() = "1";
        exp1->getHasFunctions() = false;
        exp1->getHasSideEffects() = false;
        exp1->getHasStreamAttributes() = false;
        exp1->getSourceLocation() = SourceLocation("foofoofoo", 20, 21);
        slide->setSize(ExpressionPtr(exp1));
        window.setTriggerPolicy(TriggerPolicyPtr(slide));
        window.getSourceLocation() = SourceLocation("bbaa", 423, 414);
    }
    void populateWindow1(Window& window)
    {
        window.getPartitioned() = false;
        window.getEvictionPolicy().getStyle() = WindowStyle::time;
        Expression* exp0 = new Expression();
        exp0->getCppType() = "int32";
        exp0->getSPLType() = "int32";
        exp0->getHasRuntimeConstant() = true;
        exp0->getCppExpression() = "runtimeConstant23_";
        exp0->getSPLExpression() = "5";
        exp0->getHasFunctions() = false;
        exp0->getHasSideEffects() = false;
        exp0->getHasStreamAttributes() = false;
        exp0->getSourceLocation() = SourceLocation("foobarfoo", 28, 29);
        window.getEvictionPolicy().setSize(ExpressionPtr(exp0));
        WindowPolicy* slide = new TriggerPolicy();
        slide->getStyle() = WindowStyle::count;
        Expression* exp1 = new Expression();
        exp1->getCppType() = "int32";
        exp1->getSPLType() = "int32";
        exp1->getHasRuntimeConstant() = true;
        exp1->getCppExpression() = "runtimeConstant24_";
        exp1->getSPLExpression() = "1";
        exp1->getHasFunctions() = false;
        exp1->getHasSideEffects() = false;
        exp1->getHasStreamAttributes() = false;
        exp1->getSourceLocation() = SourceLocation("barfoobar", 38, 49);
        slide->setSize(ExpressionPtr(exp1));
        window.setTriggerPolicy(TriggerPolicyPtr(slide));
        window.getSourceLocation() = SourceLocation("baab", 523, 514);
    }
    void populateInput0Attribute_Id(InputAttribute& attrb)
    {
        attrb.getName() = "id";
        attrb.getCppType() = "SPL::int32";
        attrb.getSPLType() = "int32";
    }
    void populateInput0Attribute_Name(InputAttribute& attrb)
    {
        attrb.getName() = "name";
        attrb.getCppType() = "string";
        attrb.getSPLType() = "string";
    }
    void populateInput0Attribute_Sell(InputAttribute& attrb)
    {
        attrb.getName() = "sell";
        attrb.getCppType() = "float32";
        attrb.getSPLType() = "float32";
    }
    void populateInput1Attribute_Id(InputAttribute& attrb) { populateInput0Attribute_Id(attrb); }
    void populateInput1Attribute_Name(InputAttribute& attrb)
    {
        populateInput0Attribute_Name(attrb);
    }
    void populateInput1Attribute_Buy(InputAttribute& attrb)
    {
        attrb.getName() = "buy";
        attrb.getCppType() = "float32";
        attrb.getSPLType() = "float32";
    }
    void populateOutputPort0(OutputPort& port)
    {
        port.getIndex() = 0;
        port.getCppTupleName() = "oport0_";
        port.getCppTupleType() = "OPort0";
        port.getSPLTupleType() = "tuple<int id,string name,float sell,float buy>";
        port.getFacade() = false;
        port.getMutationAllowed() = true;
        port.getAttributes().push_back(OutputAttributePtr(new OutputAttribute()));
        port.getAttributes().push_back(OutputAttributePtr(new OutputAttribute()));
        port.getAttributes().push_back(OutputAttributePtr(new OutputAttribute()));
        port.getAttributes().push_back(OutputAttributePtr(new OutputAttribute()));
        populateOutput0Attribute_Id(*port.getAttributes()[0]);
        populateOutput0Attribute_Name(*port.getAttributes()[1]);
        populateOutput0Attribute_Sell(*port.getAttributes()[2]);
        populateOutput0Attribute_Buy(*port.getAttributes()[3]);
        port.getFinalPunctuationPortScope().push_back(1);
        port.getSourceLocation() = SourceLocation("abba", 623, 614);
    }
    void populateOutput0Attribute_Id(OutputAttribute& attrb)
    {
        populateInput0Attribute_Id(attrb);
        attrb.setAssignment(AssignmentPtr(new Assignment()));
        Expression* exp0 = new Expression();
        exp0->getCppType() = "int32";
        exp0->getSPLType() = "int32";
        exp0->getHasRuntimeConstant() = false;
        exp0->getCppExpression() = "iport0_.get_id()";
        exp0->getSPLExpression() = "Quotes.id";
        exp0->getHasFunctions() = false;
        exp0->getHasSideEffects() = false;
        exp0->getHasStreamAttributes() = true;
        exp0->getSourceLocation() = SourceLocation("foofoobarbar", 98, 39);
        attrb.getAssignment().getValues().push_back(ExpressionPtr(exp0));
    }
    void populateOutput0Attribute_Name(OutputAttribute& attrb)
    {
        populateInput0Attribute_Name(attrb);
        attrb.setAssignment(AssignmentPtr(new Assignment()));
        Expression* exp0 = new Expression();
        exp0->getCppType() = "string";
        exp0->getSPLType() = "string";
        exp0->getHasRuntimeConstant() = false;
        exp0->getCppExpression() = "iport0_.get_name()";
        exp0->getSPLExpression() = "Quotes.name";
        exp0->getHasFunctions() = false;
        exp0->getHasSideEffects() = false;
        exp0->getHasStreamAttributes() = true;
        exp0->getSourceLocation() = SourceLocation("barbarfoofoo", 88, 49);
        attrb.getAssignment().getValues().push_back(ExpressionPtr(exp0));
    }
    void populateOutput0Attribute_Sell(OutputAttribute& attrb)
    {
        populateInput0Attribute_Sell(attrb);
        attrb.setAssignment(AssignmentPtr(new Assignment()));
        Expression* exp0 = new Expression();
        exp0->getCppType() = "float32";
        exp0->getSPLType() = "float32";
        exp0->getHasRuntimeConstant() = false;
        exp0->getCppExpression() = "iport0_.get_sell()";
        exp0->getSPLExpression() = "Quotes.sell";
        exp0->getHasFunctions() = false;
        exp0->getHasSideEffects() = false;
        exp0->getHasStreamAttributes() = true;
        exp0->getSourceLocation() = SourceLocation("bbff", 78, 59);
        attrb.getAssignment().getValues().push_back(ExpressionPtr(exp0));
    }
    void populateOutput0Attribute_Buy(OutputAttribute& attrb)
    {
        populateInput1Attribute_Buy(attrb);
        attrb.setAssignment(AssignmentPtr(new Assignment()));
        Expression* exp0 = new Expression();
        exp0->getCppType() = "float32";
        exp0->getSPLType() = "float32";
        exp0->getHasRuntimeConstant() = false;
        exp0->getCppExpression() = "iport1_.get_buy()";
        exp0->getSPLExpression() = "Bids.buy";
        exp0->getHasFunctions() = false;
        exp0->getHasSideEffects() = false;
        exp0->getHasStreamAttributes() = true;
        exp0->getSourceLocation() = SourceLocation("fbbf", 68, 79);
        attrb.getAssignment().getValues().push_back(ExpressionPtr(exp0));
    }

  private:
    string fileName_;
};

MAIN_APP(OperatorInstanceModelTest)
