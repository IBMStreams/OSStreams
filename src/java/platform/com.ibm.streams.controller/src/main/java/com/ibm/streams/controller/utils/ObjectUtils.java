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

package com.ibm.streams.controller.utils;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import io.fabric8.kubernetes.client.utils.Serialization;
import io.fabric8.zjsonpatch.JsonDiff;
import java.io.IOException;
import java.util.Optional;

public class ObjectUtils {

  public static <T> Optional<T> deepCopy(T original, Class<T> clazz) {
    T result;
    ObjectMapper objectMapper = new ObjectMapper();
    try {
      result = objectMapper.readValue(objectMapper.writeValueAsString(original), clazz);
    } catch (IOException e) {
      result = null;
    }
    return Optional.ofNullable(result);
  }

  public static <A, B> Optional<String> diff(A objA, B objB) {
    ObjectMapper objectMapper = new ObjectMapper();
    JsonNode diff = JsonDiff.asJson(objectMapper.valueToTree(objA), objectMapper.valueToTree(objB));
    try {
      return Optional.of(Serialization.jsonMapper().writeValueAsString(diff));
    } catch (JsonProcessingException e) {
      e.printStackTrace();
    }
    return Optional.empty();
  }
}
