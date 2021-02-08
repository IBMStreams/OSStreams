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

package com.ibm.streams.security;

import static org.junit.Assert.assertNotNull;

import com.ibm.streams.controller.security.Certificate;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.cert.X509Certificate;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class CertificateTest {

  private static final Logger logger = LoggerFactory.getLogger(CertificateTest.class);

  @Test
  public void generateCertificate() {
    /*
     * Create the key pair generator.
     */
    KeyPairGenerator keygen;
    try {
      keygen = KeyPairGenerator.getInstance("RSA");
      keygen.initialize(2048);
    } catch (NoSuchAlgorithmException e) {
      logger.error(e.getMessage());
      keygen = null;
    }
    assertNotNull(keygen);
    /*
     * Generate the keys.
     */
    KeyPair keys = keygen.generateKeyPair();
    /*
     * Generate the certificate.
     */
    try {
      String dn = "CN=Streams, L=NewYork, C=US";
      X509Certificate cert = Certificate.generateCertificate(dn, keys, 15);
      assertNotNull(cert);
      /*
       * Print the key and certificate in PEM.
       */
      logger.info(Certificate.privateKeyToPemString(keys));
      logger.info(Certificate.certificateToPemString(cert));
    } catch (Exception e) {
      logger.error("Cannot create certificate", e);
    }
  }
}
