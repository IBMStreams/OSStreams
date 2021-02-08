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

package com.ibm.streams.controller.events;

import com.ibm.streams.controller.utils.ObjectUtils;
import io.fabric8.kubernetes.api.model.HasMetadata;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.microbean.kubernetes.controller.ResourceTrackingEventQueueConsumer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public abstract class GenericEventQueueConsumer<T extends HasMetadata>
    extends ResourceTrackingEventQueueConsumer<T> implements IEventConsumerDelegate<T> {

  private static final Logger LOGGER = LoggerFactory.getLogger(GenericEventQueueConsumer.class);

  private final List<IEventConsumerDelegate<HasMetadata>> genericListeners;
  private final List<IEventConsumerDelegate<T>> listeners;
  private final String className;

  public GenericEventQueueConsumer(Map<Object, T> knownObjects) {
    super(knownObjects);
    this.genericListeners = new ArrayList<>();
    this.listeners = new ArrayList<>();
    this.className = this.getClass().getSimpleName();
  }

  @Override
  protected void accept(AbstractEvent<? extends T> event) {
    var pri = event.getPriorResource();
    var cur = event.getResource();
    switch (event.getType()) {
      case ADDITION:
        LOGGER.trace("ADD {} {}", className, cur.getMetadata().getName());
        genericListeners.forEach(l -> l.onAddition(event));
        listeners.forEach(l -> l.onAddition(event));
        this.onAddition(event);
        break;
      case MODIFICATION:
        ObjectUtils.diff(pri, cur)
            .ifPresent(
                diff -> {
                  LOGGER.trace("MOD {} {} - {}", className, cur.getMetadata().getName(), diff);
                  genericListeners.forEach(l -> l.onModification(event));
                  listeners.forEach(l -> l.onModification(event));
                  this.onModification(event);
                });
        break;
      case DELETION:
        LOGGER.trace("DEL {} {}", className, cur.getMetadata().getName());
        genericListeners.forEach(l -> l.onDeletion(event));
        listeners.forEach(l -> l.onDeletion(event));
        this.onDeletion(event);
        break;
    }
  }

  public void addGenericListener(IEventConsumerDelegate<HasMetadata> listener) {
    if (genericListeners.contains(listener)) {
      return;
    }
    genericListeners.add(listener);
  }

  public void addListener(IEventConsumerDelegate<T> listener) {
    if (listeners.contains(listener)) {
      return;
    }
    listeners.add(listener);
  }
}
