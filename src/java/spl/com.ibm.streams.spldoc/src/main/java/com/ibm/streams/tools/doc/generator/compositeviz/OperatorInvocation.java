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

package com.ibm.streams.tools.doc.generator.compositeviz;

import java.util.ArrayList;
import java.util.List;

public class OperatorInvocation {
  public OperatorInvocation(String name, String kind) {
    name_ = name;
    kind_ = kind;
  }

  public void addInputPorts(List<? extends List<String>> streamNames) {
    for (List<String> bundle : streamNames) inPorts_.add(new ArrayList<String>(bundle));
    for (int i = 0; i < inPorts_.size(); ++i) upstreams_.add(new ArrayList<UpstreamConnection>());
  }

  public void addOutputPorts(List<String> streamNames) {
    outPorts_.addAll(streamNames);
    for (int i = 0; i < outPorts_.size(); ++i)
      downstreams_.add(new ArrayList<DownstreamConnection>());
  }

  void addDownstreamConnection(int port, OperatorInvocation dop, int iport) {
    downstreams_.get(port).add(new DownstreamConnection(dop, iport));
  }

  void addUpstreamConnection(int port, OperatorInvocation uop, int oport) {
    upstreams_.get(port).add(new UpstreamConnection(uop, oport));
  }

  public String getName() {
    return name_;
  }

  public String getKind() {
    return kind_;
  }

  public float getX() {
    return x_;
  }

  public void setX(float x) {
    x_ = x;
  }

  public float getY() {
    return y_;
  }

  public void setY(float y) {
    y_ = y;
  }

  public float getWidth() {
    return width_;
  }

  public void setWidth(float w) {
    width_ = w;
  }

  public float getHeight() {
    return height_;
  }

  public void setHeight(float h) {
    height_ = h;
  }

  public int getNumberOfInputPorts() {
    return inPorts_.size();
  }

  public int getNumberOfOutputPorts() {
    return outPorts_.size();
  }

  public List<String> getInputPortStreams(int port) {
    return inPorts_.get(port);
  }

  public String getOutputPortStream(int port) {
    return outPorts_.get(port);
  }

  public List<DownstreamConnection> getDownstreamConnections(int port) {
    return downstreams_.get(port);
  }

  public List<UpstreamConnection> getUpstreamConnections(int port) {
    return upstreams_.get(port);
  }

  private String name_, kind_;
  private float x_, y_, width_, height_;
  private ArrayList<ArrayList<String>> inPorts_ = new ArrayList<ArrayList<String>>();
  private ArrayList<String> outPorts_ = new ArrayList<String>();
  private ArrayList<ArrayList<DownstreamConnection>> downstreams_ =
      new ArrayList<ArrayList<DownstreamConnection>>();
  private ArrayList<ArrayList<UpstreamConnection>> upstreams_ =
      new ArrayList<ArrayList<UpstreamConnection>>();
}
