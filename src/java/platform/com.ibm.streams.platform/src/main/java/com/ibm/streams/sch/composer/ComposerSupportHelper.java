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

package com.ibm.streams.sch.composer;

import com.ibm.streams.messages.MessageUtilities;
import com.ibm.streams.platform.services.MessageElementType;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

public class ComposerSupportHelper {

  public static String convertMessageToText(MessageElementType msg) {
    return (MessageUtilities.getMessageAndPrefix(
        msg.getMessageCode(), msg.getSubstitutionText().toArray()));
  }

  public static List<String> convertMessagesToText(Collection<MessageElementType> msgs) {
    List<String> msgList = new ArrayList<>();
    for (MessageElementType msg : msgs) {
      msgList.add(convertMessageToText(msg));
    }
    return (msgList);
  }
}
