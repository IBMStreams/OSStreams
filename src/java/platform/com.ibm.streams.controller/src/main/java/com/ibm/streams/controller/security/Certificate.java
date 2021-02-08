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

package com.ibm.streams.controller.security;

import java.math.BigInteger;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.SecureRandom;
import java.security.cert.CertificateEncodingException;
import java.security.cert.X509Certificate;
import java.util.Base64;
import java.util.Date;
import lombok.var;
import org.bouncycastle.jce.X509Principal;
import org.bouncycastle.x509.X509V3CertificateGenerator;

public class Certificate {

  private static final String CERTIFICATE_ALGORITHM = "RSA";
  private static final int CERTIFICATE_BITS = 1024;

  private static final String BEGIN_CERT = "-----BEGIN CERTIFICATE-----";
  private static final String END_CERT = "-----END CERTIFICATE-----";

  private static final String BEGIN_KEY = "-----BEGIN PRIVATE KEY-----";
  private static final String END_KEY = "-----END PRIVATE KEY-----";

  private static final String LINE_SEPARATOR = System.getProperty("line.separator");

  @SuppressWarnings("deprecation")
  public static X509Certificate generateCertificate(String dn, KeyPair keyPair, int days)
      throws Exception {
    var keyPairGenerator = KeyPairGenerator.getInstance(CERTIFICATE_ALGORITHM);
    keyPairGenerator.initialize(CERTIFICATE_BITS, new SecureRandom());
    var from = new Date();
    var to = new Date(from.getTime() + days * 86400000L);
    var v3CertGen = new X509V3CertificateGenerator();
    v3CertGen.setSerialNumber(BigInteger.valueOf(System.currentTimeMillis()));
    v3CertGen.setIssuerDN(new X509Principal(dn));
    v3CertGen.setNotBefore(from);
    v3CertGen.setNotAfter(to);
    v3CertGen.setSubjectDN(new X509Principal(dn));
    v3CertGen.setPublicKey(keyPair.getPublic());
    v3CertGen.setSignatureAlgorithm("SHA256WithRSAEncryption");
    return v3CertGen.generateX509Certificate(keyPair.getPrivate());
  }

  public static String certificateToPemString(X509Certificate cert)
      throws CertificateEncodingException {
    Base64.Encoder encoder = Base64.getMimeEncoder(64, LINE_SEPARATOR.getBytes());
    byte[] rawCrtText = cert.getEncoded();
    String encodedCertText = new String(encoder.encode(rawCrtText));
    return BEGIN_CERT + LINE_SEPARATOR + encodedCertText + LINE_SEPARATOR + END_CERT;
  }

  public static String privateKeyToPemString(KeyPair pair) {
    Base64.Encoder encoder = Base64.getMimeEncoder(64, LINE_SEPARATOR.getBytes());
    byte[] rawCrtText = pair.getPrivate().getEncoded();
    String encodedCertText = new String(encoder.encode(rawCrtText));
    return BEGIN_KEY + LINE_SEPARATOR + encodedCertText + LINE_SEPARATOR + END_KEY;
  }
}
