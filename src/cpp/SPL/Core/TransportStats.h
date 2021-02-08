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

#ifndef TRANSPORT_STATS_H
#define TRANSPORT_STATS_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <vector>

namespace SPL {
//! Transport Stats Class
class DLL_PUBLIC TransportStats
{
  public:
    struct Data;

    /// Constructor
    /// Initialization
    TransportStats() {}

    /// Load the Stats for an TransportGraph
    /// @returns 'true' if successful
    bool loadStats();

    /// Return the BOGOMips for the processor
    /// @return the BOGOMips number
    uint32_t getBogoMips() const { return _bogomips; }

    /// Return an estimate of the performance of data transfer
    /// based on tuple size
    /// @param size Tuple size (in bytes
    /// @return The estimate of the performance
    Data estimate(uint32_t size) const;

    /// Print current data
    /// @param str Output Stream
    /// @return Output Stream
    std::ostream& print(std::ostream& str) const;

    //! One row of data from the statistic files
    /// Also used to return estimates
    class Data
    {
      public:
        /// Create one row
        /// @param ts Tuple Size (Bytes)
        /// @param mr Max Rate (Bytes/sec)
        /// @param perSend CPU utilization (%) on sender
        /// @param perRec  CPU utilization (%) on receiver
        Data(uint32_t ts, uint32_t mr, uint32_t perSend, uint32_t perRec)
          : _tupleSize(ts)
          , _maxRate(mr)
          , _percentCPUSender(perSend)
          , _percentCPUReceiver(perRec)
        {}

        /// Return Tuple Size in bytes
        /// @return number of bytes
        uint32_t getTupleSize() const { return _tupleSize; }

        /// Return Maximum rate in bytes/sec
        /// @return number of bytes
        uint32_t getMaxRate() const { return _maxRate; }

        /// Return CPU % utilization for sender
        /// @return percentance CPU used to send tuples this size
        uint32_t getSenderCPUPercent() const { return _percentCPUSender; }

        /// Return CPU % utilization for receiver
        /// @return percentance CPU used to receive tuples this size
        uint32_t getReceiverCPUPercent() const { return _percentCPUReceiver; }

        /// Print a row of data
        std::ostream& print(std::ostream&) const;

      private:
        uint32_t _tupleSize;          //! size in bytes
        uint32_t _maxRate;            //! max rate (bytes/sec)
        uint32_t _percentCPUSender;   //! CPU util (%) sender
        uint32_t _percentCPUReceiver; //! CPU util (%) receiver
    };

    struct DataSorter : std::binary_function<const Data&, const Data&, bool>
    {
        bool operator()(const Data& l, const Data& r)
        {
            return l.getTupleSize() < r.getTupleSize();
        }
    };

  private:
    /// check if transport file exists (depending on default transport)
    /// @param stat_path path for directory where transport file resides
    /// @return true if file exists
    bool foundTransportStatFile(boost::filesystem::path& stat_path);

    uint32_t _bogomips;      // CPU performance
    std::vector<Data> _data; // loaded data for interpolation
};
};

#endif /* TRANSPORT_STATS_H */
