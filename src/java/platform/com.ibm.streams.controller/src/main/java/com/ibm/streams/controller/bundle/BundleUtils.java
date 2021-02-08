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

package com.ibm.streams.controller.bundle;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Optional;
import lombok.var;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okio.Okio;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import redis.clients.jedis.Jedis;
import redis.clients.jedis.exceptions.JedisConnectionException;

public class BundleUtils {

  private static final Logger LOGGER = LoggerFactory.getLogger(BundleUtils.class);

  public static Optional<byte[]> loadBundleFromRedis(String name, String ns) {
    Optional<byte[]> result = Optional.empty();
    var host = "streams-api." + ns;
    /*
     * Try to fetch the SAB from redis. Retries for 30 seconds.
     */
    for (int i = 0; i < 30; i += 1) {
      try {
        Jedis jedis = new Jedis(host);
        /*
         * Connect to the repository and check if the file exists.
         */
        if (!jedis.hexists("apps", name)) {
          throw new RuntimeException("SAB file not present in the repository");
        }
        /*
         * Get the SAB data.
         */
        result = Optional.of(jedis.hget("apps".getBytes(), name.getBytes()));
        break;
      } catch (JedisConnectionException e) {
        LOGGER.error("Jedis connection exception: {}", e.getMessage());
      }
      try {
        Thread.sleep(1000);
      } catch (InterruptedException ignored) {
      }
    }
    /*
     * Throw an exception if the connection failed.
     */
    return result;
  }

  private static boolean isBundleInRedis(String name, String ns) {
    var host = "streams-api." + ns;
    /*
     * Try to store the SAB to redis. Retries for 30 seconds.
     */
    for (int i = 0; i < 30; i += 1) {
      try {
        var jedis = new Jedis(host);
        return jedis.hexists("apps", name);
      } catch (JedisConnectionException e) {
        LOGGER.error("Jedis connection exception: {}", e.getMessage());
      }
      try {
        Thread.sleep(1000);
      } catch (InterruptedException ignored) {
      }
    }
    return false;
  }

  private static void storeBundleToRedis(String name, byte[] content, String ns) {
    var host = "streams-api." + ns;
    /*
     * Try to store the SAB to redis. Retries for 30 seconds.
     */
    for (int i = 0; i < 30; i += 1) {
      try {
        var jedis = new Jedis(host);
        var status = jedis.hset("apps".getBytes(), name.getBytes(), content);
        LOGGER.info(
            "Bundle {} has been {} in the repository", name, status == 0 ? "updated" : "stored");
        break;
      } catch (JedisConnectionException e) {
        LOGGER.error("Jedis connection exception: {}", e.getMessage());
      }
      try {
        Thread.sleep(1000);
      } catch (InterruptedException ignored) {
      }
    }
  }

  private static Optional<byte[]> loadBundleFromGithub(Request request) {
    Optional<byte[]> result = Optional.empty();
    /*
     * Try to fetch the bundle.
     */
    try {
      var client = new OkHttpClient().newBuilder().build();
      var response = client.newCall(request).execute();
      /*
       * Load the bundle.
       */
      if (response.isSuccessful() && response.body() != null) {
        var outputStream = new ByteArrayOutputStream();
        var sink = Okio.buffer(Okio.sink(outputStream));
        sink.writeAll(response.body().source());
        sink.close();
        var bytes = outputStream.toByteArray();
        result = Optional.of(bytes);
      } else {
        LOGGER.error("GET {} failed ({})", request.url(), response.code());
      }
    } catch (IOException e) {
      LOGGER.error("GET {} failed", request.url());
      e.printStackTrace();
    }
    /*
     * Return the result.
     */
    return result;
  }

  public static Optional<byte[]> loadBundleFromGithub(
      String name, String url, EBundlePullPolicy pullPolicy, String ns) {
    /*
     * Evaluate the pull policy.
     */
    if (pullPolicy == EBundlePullPolicy.IfNotPresent && isBundleInRedis(name, ns)) {
      LOGGER.info("Bundle {} already present in the repository", name);
      return loadBundleFromRedis(name, ns);
    }
    /*
     * Perform the request.
     */
    var request =
        new Request.Builder().url(url).addHeader("Accept", "application/vnd.github.v3.raw").build();
    var result = loadBundleFromGithub(request);
    result.ifPresent(
        v -> {
          LOGGER.info("Bundle {} successfully loaded from {}", name, url);
          storeBundleToRedis(name, v, ns);
        });
    return result;
  }

  public static Optional<byte[]> loadBundleFromGithub(
      String name, String url, String secret, EBundlePullPolicy pullPolicy, String ns) {
    /*
     * Evaluate the pull policy.
     */
    if (pullPolicy == EBundlePullPolicy.IfNotPresent && isBundleInRedis(name, ns)) {
      LOGGER.info("Bundle {} already present in the repository", name);
      return loadBundleFromRedis(name, ns);
    }
    /*
     * Perform the request.
     */
    var request =
        new Request.Builder()
            .url(url)
            .addHeader("Authorization", "token " + secret)
            .addHeader("Accept", "application/vnd.github.v3.raw")
            .build();
    var result = loadBundleFromGithub(request);
    result.ifPresent(
        v -> {
          LOGGER.info("Bundle {} successfully loaded from {}", name, url);
          storeBundleToRedis(name, v, ns);
        });
    return result;
  }
}
