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

package com.ibm.streams.controller.crds.pes.instance;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.pes.Connectivity;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.events.IEventConsumerDelegate;

public interface IProcessingElementCoordinator extends IEventConsumerDelegate<ProcessingElement> {

  void updatePeConnectivity(ProcessingElement pe, Connectivity connectivity);

  void incrementPeLaunchCount(ProcessingElement pe);

  void touchPe(Job job, ProcessingElement pe);

  void updatePeContentIdAndIncrementLaunchCount(Job job, ProcessingElement pe, String contentId);
}
