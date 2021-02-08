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

import com.ibm.streams.controller.events.IEventConsumerDelegate;
import io.fabric8.kubernetes.api.model.HasMetadata;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Queue;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public abstract class Coordinator<T extends HasMetadata, S extends ICommandStatus>
    implements IEventConsumerDelegate<T> {

  private static final Logger logger = LoggerFactory.getLogger(Coordinator.class);

  private final Map<String, Queue<Command<T, S>>> commandQueues;

  protected Coordinator() {
    commandQueues = new HashMap<>();
  }

  protected void apply(T resource, Command<T, S> command) {
    synchronized (commandQueues) {
      var queue = commandQueues.get(resource.getMetadata().getName());
      if (!queue.isEmpty() || command.run() == Command.Action.Wait) {
        queue.add(command);
      }
    }
  }

  public void onAddition(AbstractEvent<? extends T> event) {
    synchronized (commandQueues) {
      var resource = event.getResource();
      commandQueues.put(resource.getMetadata().getName(), new LinkedList<>());
    }
  }

  public void onModification(AbstractEvent<? extends T> event) {
    synchronized (commandQueues) {
      var pre = event.getPriorResource();
      var cur = event.getResource();
      /*
       * Grab the queue.
       */
      if (!commandQueues.containsKey(cur.getMetadata().getName())) {
        return;
      }
      var queue = commandQueues.get(cur.getMetadata().getName());
      /*
       * If the queue has no command, return.
       */
      if (queue.isEmpty()) {
        return;
      }
      /*
       * Grab the head of the queue.
       */
      var head = queue.element();
      /*
       * If the head of the queue validates the event, remove it.
       *
       * NOTE(xrg) This filters out modifications of the resource that were triggered outside the coordinator
       * mechanism, for instance the hash update applied by the UDP logic.
       */
      if (head.check(pre, cur)) {
        logger.debug("Removing succeeded command from queue {}", cur.getMetadata().getName());
        queue.remove();
      } else {
        logger.warn("Keeping failed command in queue {}", cur.getMetadata().getName());
      }
      /*
       * Return if the queue is empty.
       */
      if (queue.isEmpty()) {
        return;
      }
      /*
       * Otherwise process the next elements.
       */
      while (queue.element().run() == Command.Action.Remove) {
        queue.remove();
      }
    }
  }

  public void onDeletion(AbstractEvent<? extends T> event) {
    synchronized (commandQueues) {
      var resource = event.getResource();
      commandQueues.remove(resource.getMetadata().getName());
    }
  }
}
