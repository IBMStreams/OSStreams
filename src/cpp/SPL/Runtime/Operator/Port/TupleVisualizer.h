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

#ifndef SPL_RUNTIME_OPERATOR_PORT_VISUALIZER_H
#define SPL_RUNTIME_OPERATOR_PORT_VISUALIZER_H

#include <UTILS/Mutex.h>
#include <stdint.h>
#include <string>
#include <sys/time.h>
#include <vector>

namespace SPL {
class PEVisualizer;
class Tuple;
class NativeByteBuffer;
class ViewProperties;

/// @brief Control the frequency of tuple visualization.
/// We don't want to send more than a small number of tuples per second
/// to the visualizer.
class TupleVisualizer
{
  public:
    /// Constructor
    TupleVisualizer();

    /// Send a tuple as a buffer to the visualizer
    /// @param buffer Buffer to potentially visualize
    void visualize(const NativeByteBuffer& buffer);

    /// Send a tuple to the visualizer
    /// @param tuple tuple to potentially visualize
    void visualize(const Tuple& tuple);

    /// Add a view id to this visualizer. There can be several view ids
    /// all watching the same output port.
    /// This function throws an SPLRuntimeAssertionFailed if the client
    /// tries to add a view associated with a different operator index
    /// than the one of the existing view.
    /// @param view view properties for the view to be added
    void addView(const ViewProperties& view);

    /// Remove a view id from this visualizer.
    /// @param view view to be removed
    void deleteView(const ViewProperties& view);

    /// Is there anyone watching this?
    /// @return true if there are any view ids watching this
    bool isVisualizing() const { return _views.size() > 0; }

  private:
    /// Prepare view data before sending it to the PEVisualizer.
    ///
    /// The function iterates through the view properties associated with
    /// this TupleVisualizer and fills in the three function parameters.
    /// @param viewIds vector of view Ids associated with the view tuple
    /// @param channel the parallel channel index of the operator sending
    ///     the tuple
    /// @param allChannels parallel channel indexes for all nested parallel
    ///     regions
    void prepareForVisualization(std::vector<uint64_t>& viewIds,
                                 int32_t& channel,
                                 std::vector<int32_t>& allChannels);

    Distillery::Mutex _mutex;
    std::vector<const ViewProperties*> _views;
    PEVisualizer& _peVisualizer;
    uint64_t _tupleSeq;
    time_t _lastChecked;
    uint32_t _countDuringLastSecond;
};
};

#endif /* SPL_RUNTIME_OPERATOR_PORT_VISUALIZER_H */
