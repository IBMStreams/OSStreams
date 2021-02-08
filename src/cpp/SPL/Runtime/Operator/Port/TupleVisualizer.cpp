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

#include <SPL/Runtime/Operator/Port/TupleVisualizer.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/PEVisualizer.h>
#include <SPL/Runtime/ProcessingElement/ViewProperties.h>

using namespace SPL;

TupleVisualizer::TupleVisualizer()
  : _peVisualizer(PEImpl::instance().getVisualizer())
  , _tupleSeq(0)
  , _lastChecked(0)
{}

void TupleVisualizer::visualize(const Tuple& tuple)
{
    APPTRC(L_TRACE, "Enter visualize", "VIEW");

    std::vector<uint64_t> viewIds;
    int32_t channel = -1; // no parallel channel by default
    std::vector<int32_t> allChannels;
    prepareForVisualization(viewIds, channel, allChannels);

    // Send them through
    if (!viewIds.empty()) {
        _peVisualizer.write(viewIds, channel, allChannels, _tupleSeq, tuple);
    } else {
        APPTRC(L_TRACE, "no view ids to write to", "VIEW");
    }

    APPTRC(L_TRACE, "Exit visualize", "VIEW");
}

void TupleVisualizer::visualize(const NativeByteBuffer& buffer)
{
    APPTRC(L_TRACE, "Enter visualize", "VIEW");

    std::vector<uint64_t> viewIds;
    int32_t channel = -1; // no parallel channel by default
    std::vector<int32_t> allChannels;
    prepareForVisualization(viewIds, channel, allChannels);

    // Send them through
    if (!viewIds.empty()) {
        _peVisualizer.write(viewIds, channel, allChannels, _tupleSeq, buffer);
    } else {
        APPTRC(L_TRACE, "no view ids to write to", "VIEW");
    }

    APPTRC(L_TRACE, "Exit visualize", "VIEW");
}

void TupleVisualizer::addView(const ViewProperties& view)
{
    Distillery::AutoMutex am(_mutex);
    // Assert that all the views in `_views` have the same operator index
    if (_views.size() > 0) {
        if (_views[0]->getOperator().getIndex() != view.getOperator().getIndex()) {
            THROW(
              SPLRuntimeAssertionFailed,
              "View id " << view.getViewId() << " associated with operator index "
                         << view.getOperator().getIndex()
                         << " is being added to a TupleVisualizer which contains a view associated"
                         << " with operator index " << _views[0]->getOperator().getIndex());
        }
    }
    _views.push_back(&view);
}

void TupleVisualizer::deleteView(const ViewProperties& view)
{
    Distillery::AutoMutex am(_mutex);
    for (std::vector<const ViewProperties*>::iterator it = _views.begin(); it != _views.end();
         it++) {
        if (*it == &view) {
            _views.erase(it);
            break;
        }
    }

    // Just ensure we start from scratch next time
    if (_views.empty()) {
        _lastChecked = 0;
    }
}

void TupleVisualizer::prepareForVisualization(std::vector<uint64_t>& viewIds,
                                              int32_t& channel,
                                              std::vector<int32_t>& allChannels)
{
    _tupleSeq++;

    // Have we generated too many during the last second?
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (_lastChecked != tv.tv_sec) {
        _lastChecked = tv.tv_sec;
        _countDuringLastSecond = 0;
    }

    // Check all the views to see if we should send to them
    _countDuringLastSecond++;

    {
        Distillery::AutoMutex am(_mutex);
        // anyone looking at me?
        if (_views.empty()) {
            return;
        }

        for (std::vector<const ViewProperties*>::const_iterator it = _views.begin();
             it != _views.end(); it++) {
            uint64_t rate = (*it)->getRate();
            uint64_t vid = (*it)->getViewId();

            // The addView() function asserts that all views are associated to the same operator
            // so we just pick up the channelIndex associated with the first view.
            if (it == _views.begin()) {
                channel = (*it)->getOperator().getContext().getChannel();
                std::vector<int32_t> ac = (*it)->getOperator().getContext().getAllChannels();
                allChannels.insert(allChannels.end(), ac.begin(), ac.end());
            }

            // Add the view to the return list if we didn't view too many tuples
            if (_countDuringLastSecond <= rate) {
                viewIds.push_back(vid);
            } else {
                APPTRC(L_TRACE,
                       "Suppressing write to view id "
                         << vid << " channel index " << channel << " because count "
                         << _countDuringLastSecond << " is more than " << rate,
                       "VIEW");
            }
        }
    }
}
