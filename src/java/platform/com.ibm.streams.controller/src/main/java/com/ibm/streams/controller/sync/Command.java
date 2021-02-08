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

package com.ibm.streams.controller.sync;

import io.fabric8.kubernetes.api.model.HasMetadata;

public abstract class Command<Type extends HasMetadata, Status extends ICommandStatus> {

  protected enum Action {
    Remove,
    Wait
  }

  private Status status;

  protected Command(Status status) {
    this.status = status;
  }

  public synchronized Status get() {
    if (status.isUnknown()) {
      try {
        wait();
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
    }
    return status;
  }

  protected synchronized void set(Status value) {
    status = value;
    notify();
  }

  public abstract boolean check(Type pre, Type cur);

  public abstract Action run();
}
