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

package com.ibm.streams.instance.sam.model;

import com.google.common.collect.ArrayListMultimap;
import com.google.common.collect.ImmutableMultimap;
import com.google.common.collect.Multimap;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.runtime.RuntimeStream;
import com.ibm.streams.instance.sam.model.runtime.RuntimeStreamView;
import com.ibm.streams.instance.sam.model.runtime.StreamId;
import com.ibm.streams.instance.sam.model.runtime.StreamViewState;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class StreamView extends SamObject {

  private static final String PROPERTIES_NEED_SPLITTING = "throttle";

  private BigInteger id;
  private StreamId streamId;
  private Multimap<String, String> nameValuePairs;
  private List<RuntimeStreamView> runs;

  public StreamView(StreamId sid) {
    super(false);
    id = SAMStaticContext.getObjectTracker().newStreamViewId();
    runs = new ArrayList<RuntimeStreamView>();
    streamId = sid;
    nameValuePairs = ArrayListMultimap.create();
    SAMStaticContext.getObjectTracker().registerStreamView(this);
  }

  public void clearChannelViews() {
    runs.clear();
  }

  public void addChannelView(RuntimeStreamView view) {
    runs.add(view);
  }

  public List<RuntimeStreamView> getChannelViews() {
    return runs;
  }

  public RuntimeStreamView getChannelView(int channelIndex) {
    for (RuntimeStreamView view : runs) {
      if (view.getChannelIndex() == channelIndex) {
        return view;
      }
    }
    return null;
  }

  public BigInteger getId() {
    return id;
  }

  public StreamId getStreamId() {
    return streamId;
  }

  public void setStreamId(StreamId id) {
    streamId = id;
  }

  public ImmutableMultimap<String, String> getNameValuePairs() {
    return ImmutableMultimap.copyOf(nameValuePairs);
  }

  public void setNameValuePairs(Multimap<String, String> pairs) {
    nameValuePairs = pairs;
  }

  public void addNameValuePair(String name, String value) {
    nameValuePairs.put(name, value);
  }

  public void init(Job job) {
    RuntimeStream st = job.getViewableStream(streamId);
    if (st.isLogical()) {
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Logical view, processing for its channel views...");
      }
      List<RuntimeStream> phyStreams = job.getPhysicalViewableStreams(streamId);
      int size = phyStreams.size();
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Got " + size + " channel views related to this logical view.");
      }
      Multimap<String, String> newProps = createChannelSVIProperties(size);
      for (RuntimeStream phyStream : phyStreams) {
        RuntimeStreamView chSVI = new RuntimeStreamView(id);
        chSVI.setStreamId(phyStream.getStreamId());
        chSVI.setNameValuePairs(newProps);
        chSVI.setChannelIndex(phyStream.getChannelNum());
        addChannelView(chSVI);
      }
    } else {
      // creating one channelStreamViewInfo object and add to list.
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Physical view, creating default channel view...");
      }
      RuntimeStreamView chSVI = new RuntimeStreamView(id);
      chSVI.setStreamId(st.getStreamId());
      Multimap<String, String> newProps = ArrayListMultimap.create();
      newProps.putAll(nameValuePairs);
      chSVI.setNameValuePairs(newProps);
      addChannelView(chSVI);
    }
  }

  private Multimap<String, String> createChannelSVIProperties(int numChannels) {
    Multimap<String, String> props = ArrayListMultimap.create();
    for (String key : nameValuePairs.keySet()) {
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Processing property " + key);
      }
      if (PROPERTIES_NEED_SPLITTING.indexOf(key) != -1) {
        for (String value : nameValuePairs.get(key)) {
          int intVal = Integer.parseInt(value);
          int realVal = intVal / numChannels;
          props.put(key, Integer.toString(realVal));
        }
      } else {
        props.putAll(key, nameValuePairs.get(key));
      }
    }
    return props;
  }

  public void start() {
    for (RuntimeStreamView view : runs) {
      view.start();
    }
  }

  public void start(BigInteger peid) {
    for (RuntimeStreamView view : runs) {
      view.start(peid);
    }
  }

  public void stop() {
    for (RuntimeStreamView view : runs) {
      view.stop();
    }
    if (isClosed()) {
      SAMStaticContext.getObjectTracker().unregisterStreamView(getId());
    }
  }

  public void stop(BigInteger peid) {
    for (RuntimeStreamView view : runs) {
      view.stop(peid);
    }
    // Do not remove view even view are closed, since this call is from PE stopped.
  }

  public void setViewState(StreamViewState state, int channelIndex) {
    for (RuntimeStreamView view : runs) {
      if (view.getChannelIndex() == channelIndex) {
        view.setViewState(state);
      }
    }
  }

  public BigInteger getCurNotifCount(int channelIndex) {
    for (RuntimeStreamView view : runs) {
      if (view.getChannelIndex() == channelIndex) {
        return view.getCurNotifCount();
      }
    }
    return null;
  }

  public void setCurNotifCount(BigInteger count, int channelIndex) {
    for (RuntimeStreamView view : runs) {
      if (view.getChannelIndex() == channelIndex) {
        view.setCurNotifCount(count);
      }
    }
  }

  public void setProperties(Multimap<String, String> updatedProps) {
    nameValuePairs = updatedProps;
    Multimap<String, String> newProps = createChannelSVIProperties(runs.size());
    for (RuntimeStreamView view : runs) {
      view.setNameValuePairs(newProps);
      view.update();
    }
  }

  public boolean hasViewForPE(BigInteger peid) {
    boolean included = false;
    for (RuntimeStreamView view : runs) {
      if (view.isForPE(peid)) {
        included = true;
        break;
      }
    }
    return included;
  }

  public boolean isForJob(BigInteger jobid) {
    if (streamId.getJobId().compareTo(jobid) == 0) {
      return true;
    } else {
      return false;
    }
  }

  public boolean isClosed() {
    boolean closed = true;
    for (RuntimeStreamView view : runs) {
      if (view.getViewState() != StreamViewState.CLOSED) {
        closed = false;
        break;
      }
    }
    return closed;
  }

  @Override
  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append("id=" + id);
    builder.append(" streamId=" + streamId);
    builder.append(" nameValuePairs=" + nameValuePairs);
    return builder.toString();
  }

  @Override
  public boolean isJobConstructed() {
    return true;
  }

  @Override
  public int hashCode() {
    return Objects.hash(id, streamId, nameValuePairs, runs);
  }
}
