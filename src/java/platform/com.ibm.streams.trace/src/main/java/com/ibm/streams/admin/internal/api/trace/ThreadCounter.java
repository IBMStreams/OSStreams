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

package com.ibm.streams.admin.internal.api.trace;

import java.lang.management.ThreadInfo;
import java.util.HashMap;
import java.util.Map;

public class ThreadCounter {
  private Map<Thread.State, Counter> stateMap = new HashMap<Thread.State, Counter>();
  private Map<String, Counter> nameMap = new HashMap<String, Counter>();

  public ThreadCounter() {}

  public void count(ThreadInfo thread) {
    countState(thread.getThreadState());
    countName(thread.getThreadName());
  }

  /*
  public void summarize() {
    summarizeState();
    summarizeName();
  }*/

  private void countState(Thread.State state) {
    Counter counter = stateMap.get(state);
    if (counter == null) {
      counter = new Counter();
      stateMap.put(state, counter);
    }
    counter.increment();
  }

  private void countName(String name) {
    Counter counter = nameMap.get(name);
    if (counter == null) {
      counter = new Counter();
      nameMap.put(name, counter);
    }
    counter.increment();
  }

  public Map<Thread.State, Counter> getStateMap() {
    return stateMap;
  }

  public Map<String, Counter> getNameMap() {
    return nameMap;
  }

  /*
  private void summarizeState() {
    Trace.logDebug("By thread state:");
    Set<Thread.State> states = stateMap.keySet();
    for (Thread.State state : states) {
      Counter counter = stateMap.get(state);
      Trace.logDebug("    " + state + "=" + counter.getCount());
    }
  }

  private void summarizeName() {
    Trace.logDebug("By thread name:");
    Set<String> names = nameMap.keySet();
    for (String name : names) {
      Counter counter = nameMap.get(name);
      Trace.logDebug("    " + name + "=" + counter.getCount());
    }
  }*/

  public class Counter {
    private int count = 0;

    private Counter() {}

    public int increment() {
      return count++;
    }

    public int getCount() {
      return count;
    }
  }
}
