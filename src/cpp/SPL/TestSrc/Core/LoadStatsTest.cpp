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
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/OperatorGraph.h>
#include <UTILS/DistilleryApplication.h>
#include <boost/filesystem/path.hpp>

#include <fstream>
#include <math.h>
#include <string>

using namespace std;
using namespace std::tr1;
using namespace SPL;

#define TOL 0.01
#define STRINGIZE(X) #X
#define STR(X) STRINGIZE(X)

static bool eq(const double a, const double b)
{
    bool ans = fabs(a - b) <= TOL * fabs(a);
    if (!ans) {
        cout << "eq fails: a = " << a << ", b = " << b << ", fabs(a-b) = " << fabs(a - b)
             << ", " STR(TOL) " * fabs(a) = " << TOL * fabs(a) << endl;
    }
    return ans;
}

class LoadStatsTest : public CompilerApp
{
  public:
    LoadStatsTest() {}

    void getArguments(option_vector_t& options) { setKeepRemainingArguments(); }
    virtual int run(const std::vector<std::string>& args)
    {
        if (args.size() != 2) {
            cerr << "Syntax:  pgm directory preFixString\n";
            return EXIT_FAILURE;
        }
        computeCompilerPaths();
        cout.precision(numeric_limits<double>::digits10);
        cout.setf(ios_base::scientific, ios_base::floatfield);

        OperatorGraph graph;
        populateGraph(graph);
        if (!graph.loadStats(args[0], false)) {
            return EXIT_FAILURE;
        }
        cout << "Node 0 CPU fraction: " << graph.cpuFraction(0) << endl;
        cout << "Node 0 Output port tuple rate: " << graph.outputTupleRate(0, 0) << endl;
        cout << "Node 0 Output port byte rate: " << graph.outputByteRate(0, 0) << endl;
        graph.print(cout);
        // compare the results to the old Perl version
        FASSERT(eq(graph.cpuFraction(0), 0.974539822364645));
        FASSERT(eq(graph.outputTupleRate(0, 0), 1676.90774918961));
        FASSERT(eq(graph.outputByteRate(0, 0), 31857.7356922027));

        FASSERT(eq(graph.cpuFraction(1), 0.973435220467943));
        FASSERT(eq(graph.inputTupleRate(1, 0), 1602.0655198842));
        FASSERT(eq(graph.inputByteRate(1, 0), 30380.5687732719));
        FASSERT(eq(graph.outputTupleRate(1, 0), 1601.35632369154));
        FASSERT(eq(graph.outputByteRate(1, 0), 55810.8907763427));

        FASSERT(eq(graph.cpuFraction(2), 0.963442976723665));
        FASSERT(eq(graph.inputTupleRate(2, 0), 1604.49475019652));
        FASSERT(eq(graph.inputByteRate(2, 0), 56021.3203305732));
        FASSERT(eq(graph.outputTupleRate(2, 0), 1602.96957596068));
        FASSERT(eq(graph.outputByteRate(2, 0), 43126.859157236));

        FASSERT(eq(graph.cpuFraction(3), 0.907631136250185));
        FASSERT(eq(graph.inputTupleRate(3, 0), 1603.59272918705));
        FASSERT(eq(graph.inputByteRate(3, 0), 43222.2339266827));
        FASSERT(eq(graph.outputTupleRate(3, 0), 1603.86197506415));
        FASSERT(eq(graph.outputByteRate(3, 0), 43020.2169768176));

        FASSERT(eq(graph.cpuFraction(4), 0.000441060411788729));
        FASSERT(eq(graph.inputTupleRate(4, 0), 1603.85604286811));
        FASSERT(eq(graph.inputByteRate(4, 0), 43299.853120438));
        return EXIT_SUCCESS;
    }
    void populateGraph(OperatorGraph& graph)
    {
        // Must match up with the known names in stats directory
        int n0 = graph.addNode("BIOP_Source1", 0, 1, NULL);
        int n1 = graph.addNode("BIOP_Regex1", 1, 1, NULL);
        int n2 = graph.addNode("BIOP_Regex2", 1, 1, NULL);
        int n3 = graph.addNode("BIOP_Regex3", 1, 1, NULL);
        int n4 = graph.addNode("BIOP_Regex3Sink1", 1, 0, NULL);

        graph.connectNode(n0, 0, n1, 0);
        graph.connectNode(n1, 0, n2, 0);
        graph.connectNode(n2, 0, n3, 0);
        graph.connectNode(n3, 0, n4, 0);
    }

  private:
    string fileName_;
};

MAIN_APP(LoadStatsTest)
