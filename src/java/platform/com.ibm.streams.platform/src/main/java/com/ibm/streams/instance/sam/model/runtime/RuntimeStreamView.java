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

package com.ibm.streams.instance.sam.model.runtime;

import com.google.common.collect.ArrayListMultimap;
import com.google.common.collect.ImmutableMultimap;
import com.google.common.collect.Multimap;
import java.math.BigInteger;
import java.util.Objects;

public class RuntimeStreamView {

  private static final int DEFAULT_CHANNEL_INDEX = -1;

  private BigInteger id;
  private StreamId streamId;
  private Multimap<String, String> nameValuePairs;
  private BigInteger curNotifCount;
  private BigInteger maxNotifCount;
  private int channelIndex;
  private StreamViewState viewState;

  public RuntimeStreamView(BigInteger viewid) {
    id = viewid;
    nameValuePairs = ArrayListMultimap.create();
    viewState = StreamViewState.INITED;
    channelIndex = DEFAULT_CHANNEL_INDEX;
    curNotifCount = BigInteger.ZERO;
    maxNotifCount = BigInteger.ZERO;
  }

  // for change detection
  public int hashCode() {
    return Objects.hash(
        super.hashCode(),
        id,
        streamId,
        nameValuePairs,
        curNotifCount,
        maxNotifCount,
        channelIndex,
        viewState);
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

  public BigInteger getCurNotifCount() {
    return curNotifCount;
  }

  public void setCurNotifCount(BigInteger count) {
    curNotifCount = count;
  }

  public BigInteger getMaxNotifCount() {
    return maxNotifCount;
  }

  public void setMaxNotifCount(BigInteger count) {
    maxNotifCount = count;
  }

  public int getChannelIndex() {
    return channelIndex;
  }

  public void setChannelIndex(int index) {
    channelIndex = index;
  }

  public StreamViewState getViewState() {
    return viewState;
  }

  public void setViewState(StreamViewState state) {
    viewState = state;
  }

  public void start() {}

  public void start(BigInteger peid) {}

  public void update() {}

  public void stop() {}

  public void stop(BigInteger peid) {}

  public boolean isForPE(BigInteger peid) {
    BigInteger peId = getStreamId().getPeId();
    if (peId != null && peId.compareTo(peid) == 0) {
      return true;
    } else {
      return false;
    }
  }
}
