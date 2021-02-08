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

# Build arguments
#
ARG DOCKER_REGISTRY
ARG DOCKER_NAMESPACE
ARG DOCKER_ARCH

# Bring in all the dependencies
#
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/antlr3c:3.1.3.${DOCKER_ARCH}          AS antlr3c
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/aws-sdk-cpp:1.0.133.${DOCKER_ARCH}    AS aws-sdk-cpp
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/boost:1.55.0.${DOCKER_ARCH}           AS boost
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/decnumber:1.0.0.${DOCKER_ARCH}        AS decnumber
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/icu4c:56.2.${DOCKER_ARCH}             AS icu4c
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/openjdk:8.${DOCKER_ARCH}              AS openjdk
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/prometheus-cpp:0.11.0.${DOCKER_ARCH}  AS prometheus-cpp
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/rocksdb:5.18.3.${DOCKER_ARCH}         AS rocksdb
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/tecla:1.6.1.${DOCKER_ARCH}            AS tecla
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/xqilla:2.3.0.${DOCKER_ARCH}           AS xqilla

# Copy the files
#
COPY --from=antlr3c         /usr/local /usr/local
COPY --from=aws-sdk-cpp     /usr/local /usr/local
COPY --from=boost           /usr/local /usr/local
COPY --from=decnumber       /usr/local /usr/local
COPY --from=icu4c           /usr/local /usr/local
COPY --from=openjdk         /usr/local /usr/local
COPY --from=prometheus-cpp  /usr/local /usr/local
COPY --from=rocksdb         /usr/local /usr/local
COPY --from=tecla           /usr/local /usr/local

# LC
#
RUN localedef -i en_US -f UTF-8 en_US
ENV LC_ALL="en_US.UTF-8"

# Required by OpenJDK
#
RUN yum install -y \
        fontconfig \
 && yum clean -y all \
 && rm -rf /var/cache/yum
