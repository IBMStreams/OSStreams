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

package com.ibm.streams.operator.internal.management;

import javax.management.Notification;
import javax.management.NotificationBroadcasterSupport;

public abstract class NotifyInfo extends NotificationBroadcasterSupport {
  private long sequenceNumber;

  protected NotifyInfo() {}

  protected synchronized long nextSequence() {
    return sequenceNumber++;
  }

  public void notify(String type) {
    Notification event = new Notification(type, this, nextSequence(), System.currentTimeMillis());
    sendNotification(event);
  }

  public void notify(String type, Object userData) {
    Notification event = new Notification(type, this, nextSequence(), System.currentTimeMillis());
    event.setUserData(userData);
    sendNotification(event);
  }
}
