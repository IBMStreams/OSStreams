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

#ifndef OPERATOR_STATS_H
#define OPERATOR_STATS_H

#include <iostream>
#include <stdint.h>
#include <vector>

namespace xmlns {
namespace prod {
namespace streams {
namespace spl {
namespace operator_ {
namespace profile {
class bucketDataType;
class portType;
class totalType;
class operatorProfileType;
}
}
}
}
}
}

namespace SPL {
class OperatorGraph;
class OperatorGraphNode;

//! Operator Stats Class
class OperatorStats
{
  public:
    struct Data;

    /// Constructor
    /// Initialize empty graph
    /// @param keepData 'true' if all the bucket data should be kept
    OperatorStats(bool keepData) { _keepData = keepData; }

    /// Destructor
    /// Delete all the nodes added
    ~OperatorStats();

    /// Load the Stats for an OperatorGraph
    /// @param graph OperatorGraph (for nodes)
    /// @param dir directory to load the files from
    /// @returns 'true' if successful
    bool loadStats(const OperatorGraph& graph, const std::string& dir);

    /// Return the number of nodes we have
    /// @return the number of nodes (Must match OperatorGraph::numNodes())
    uint32_t numNodes() const { return _nodes.size(); }

    /// Return the CPU fraction of operator n
    /// @param n Node Number
    /// @return the percentage of CPU time spent processing in this operator
    double cpuFraction(uint32_t n) const;

    /// Return the Tuple Rate for an input port
    /// @param n Node Number
    /// @param p Input Port Number
    /// @return the number of tuples processed per second
    double inputTupleRate(uint32_t n, uint32_t p) const;

    /// Return the Byte Rate for an input port
    /// @param n Node Number
    /// @param p Input Port Number
    /// @return the number of bytes processed per second
    double inputByteRate(uint32_t n, uint32_t p) const;

    /// Return the Tuple Rate for an output port
    /// @param n Node Number
    /// @param p Output Port Number
    /// @return the number of tuples processed per second
    double outputTupleRate(uint32_t n, uint32_t p) const;

    /// Return the Byte Rate for an input port
    /// @param n Node Number
    /// @param p Output Port Number
    /// @return the number of bytes processed per second
    double outputByteRate(uint32_t n, uint32_t p) const;

    /// Return ALL the data for a given operator
    /// @return the data for a node
    const Data& operator[](uint32_t n) const;

    /// Print Statistics
    /// @param graph Operator Graph (for printing node names)
    /// @param str Output Stream
    /// @return Output Stream
    std::ostream& print(const OperatorGraph& graph, std::ostream& str) const;

    //! One row of normalized data from the statistic files
    struct Row
    {
        uint64_t _timeStamp; //! when
        uint64_t _value;     //! what
        std::ostream& print(std::ostream&) const;
    };

    struct RowSorter : std::binary_function<const Row&, const Row&, bool>
    {
        bool operator()(const Row& l, const Row& r) { return l._timeStamp < r._timeStamp; }
    };

    //! Total statistics from a file
    struct Total
    {
        uint64_t _initialTimestamp; //! collection begin time
        uint64_t _finalTimestamp;   //! collection end time
        uint64_t _duration;         //! end time - begin time
        uint64_t _cnt;              //! how many
        uint64_t _sum;              //! sum
        uint64_t _min;              //! min
        uint64_t _max;              //! max
        double _avg;                //! average
        void load(const ::xmlns::prod::streams::spl::operator_::profile::totalType& tot);
        std::ostream& print(std::ostream&) const;
    };

    //! Data from one file (operator or port)
    struct Info
    {
        Info();
        ~Info();
        std::vector<Row>* _buckets; //! individual rows
        Total _total;
        void load(const ::xmlns::prod::streams::spl::operator_::profile::bucketDataType& bucket,
                  bool keepData);
        std::ostream& print(std::ostream&) const;
    };

    //! Derived statistics
    struct Operator : Info
    {
        double _cpuFrac;
        void load(const ::xmlns::prod::streams::spl::operator_::profile::bucketDataType& bucket,
                  bool keepData);
        std::ostream& print(std::ostream&) const;
    };

    //! Derived statistics
    struct OnePort : Info
    {
        double _rate;
        void load(const ::xmlns::prod::streams::spl::operator_::profile::bucketDataType& bucket,
                  char fchar,
                  bool keepData);
        std::ostream& print(std::ostream&) const;
    };

    //! Port Triple n/s/t
    struct Port
    {
        OnePort _num;
        OnePort _size;
        OnePort _time;
        void load(const ::xmlns::prod::streams::spl::operator_::profile::portType&, bool keepData);
        std::ostream& print(std::ostream&) const;
    };

    //! Statistic data for one OperatorGraphNode
    struct Data
    {
        Operator _base;
        std::vector<Port> _inputPorts;
        std::vector<Port> _outputPorts;
        void load(const ::xmlns::prod::streams::spl::operator_::profile::operatorProfileType& prof,
                  bool keepData);
        std::ostream& print(std::ostream&) const;
    };

  private:
    bool _keepData;
    void clean(); // clean up _nodes
    std::vector<Data*> _nodes;
};
};

#endif /* OPERATOR_STATS_H */
