#
# Copyright 2021 IBM Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

ARG DOCKER_IMAGE
FROM $DOCKER_IMAGE

# Install
#
ARG DOCKER_ARCH

RUN case "${DOCKER_ARCH}" in \
      "x86_64") \
      JJ9_PKG=OpenJDK8U-jdk_x64_linux_openj9_8u275b01_openj9-0.23.0.tar.gz; \
      ;; \
      "ppc64le") \
      JJ9_PKG=OpenJDK8U-jdk_ppc64le_linux_openj9_8u275b01_openj9-0.23.0.tar.gz; \
      ;; \
      *) \
      echo "Unsupported architecture"; \
      exit 1 \
      ;; \
    esac; \
    curl -sL \
      https://github.com/AdoptOpenJDK/openjdk8-binaries/releases/download/jdk8u275-b01_openj9-0.23.0/${JJ9_PKG} \
      -o ${JJ9_PKG} \
 && tar -xzf ${JJ9_PKG} \
 && mv jdk8u275-b01 /usr/local/java-${DOCKER_ARCH}-80 \
 && rm -f ${JJ9_PKG}
