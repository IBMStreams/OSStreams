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

import com.ibm.streams.controller.crds.jobs.BundleSpec;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.util.Base64;
import java.util.Optional;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class KubernetesBundleLoader implements IBundleLoader {

  private static final Logger LOGGER = LoggerFactory.getLogger(KubernetesBundleLoader.class);
  private final KubernetesClient client;

  public KubernetesBundleLoader(KubernetesClient client) {
    this.client = client;
  }

  @Override
  public Optional<Bundle> load(BundleSpec spec, String namespace) {
    Optional<byte[]> content = Optional.empty();
    /*
     * Get the content direcly from Redis or from Github.
     */
    if (spec.getUrl() == null) {
      content = BundleUtils.loadBundleFromRedis(spec.getName(), namespace);
    } else if (spec.getSecret() == null) {
      content =
          BundleUtils.loadBundleFromGithub(
              spec.getName(), spec.getUrl(), spec.getPullPolicy(), namespace);
    } else {
      var secret = client.secrets().inNamespace(namespace).withName(spec.getSecret()).get();
      if (secret == null) {
        LOGGER.error("Cannot find secret {}", spec.getSecret());
      } else if (secret.getData() != null && secret.getData().containsKey("token")) {
        var token64 = secret.getData().get("token");
        var token = new String(Base64.getDecoder().decode(token64));
        content =
            BundleUtils.loadBundleFromGithub(
                spec.getName(), spec.getUrl(), token, spec.getPullPolicy(), namespace);
      } else {
        LOGGER.error("Secret {} does not contain a token", spec.getSecret());
      }
    }
    /*
     * Return the bundle.
     */
    return content.map(c -> new Bundle(spec.getName(), c));
  }
}
