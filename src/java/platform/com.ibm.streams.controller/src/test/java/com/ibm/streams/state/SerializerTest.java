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

package com.ibm.streams.state;

import static org.junit.Assert.assertNotNull;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.ibm.streams.controller.state.subscription.ExportSubscription;
import com.ibm.streams.controller.state.subscription.ExportSubscriptions;
import com.ibm.streams.controller.state.subscription.ImportSubscriptionEntry;
import java.math.BigInteger;
import java.util.HashMap;
import lombok.var;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class SerializerTest {

  private static final Logger logger = LoggerFactory.getLogger(SerializerTest.class);

  private ImportSubscriptionEntry makeImportSubscriptionEntry() {
    var entry = new ImportSubscriptionEntry();
    entry.setJobId(BigInteger.ONE);
    entry.setPeId(BigInteger.valueOf(2));
    entry.setPortId(BigInteger.valueOf(3));
    entry.setPortIndex(BigInteger.ONE);
    return entry;
  }

  private ExportSubscription makeExportSubscription() {
    ExportSubscription sub = new ExportSubscription();
    sub.setPortId(BigInteger.ONE);
    var entries = new HashMap<String, ImportSubscriptionEntry>();
    entries.put("entry0", makeImportSubscriptionEntry());
    sub.setEntries(entries);
    return sub;
  }

  private ExportSubscriptions makeExportSubscriptions() {
    var subs = new ExportSubscriptions();
    subs.put("sub0", makeExportSubscription());
    subs.put("sub1", makeExportSubscription());
    return subs;
  }

  @Test
  public void importSubscriptionEntryTest() throws JsonProcessingException {
    var mapper = new ObjectMapper();
    var s = mapper.writeValueAsString(makeImportSubscriptionEntry());
    assertNotNull(s);
    logger.info(s);
  }

  @Test
  public void exportSubscriptionTest() throws JsonProcessingException {
    var mapper = new ObjectMapper();
    var s = mapper.writeValueAsString(makeExportSubscription());
    assertNotNull(s);
    logger.info(s);
  }

  @Test
  public void exportSubscriptionsTest() throws JsonProcessingException {
    var mapper = new ObjectMapper();
    var s = mapper.writeValueAsString(makeExportSubscriptions());
    assertNotNull(s);
    logger.info(s);
  }
}
