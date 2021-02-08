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
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/ant:1.9.6.${DOCKER_ARCH}      AS ant
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/antlr3:3.1.3.${DOCKER_ARCH}   AS antlr3
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/clang-tools:9.${DOCKER_ARCH}  AS clang-tools
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/dita-ot:1.5.4.${DOCKER_ARCH}  AS dita-ot
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/maven:3.6.3.${DOCKER_ARCH}    AS maven
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/xsd:4.0.0.${DOCKER_ARCH}      AS xsd
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/streams-base:6.${DOCKER_ARCH} AS base

# Copy the files
#
COPY --from=ant         /usr/local /usr/local
COPY --from=antlr3      /usr/local /usr/local
COPY --from=clang-tools /usr/local /usr/local
COPY --from=dita-ot     /usr/local /usr/local
COPY --from=maven       /usr/local /usr/local
COPY --from=xsd         /usr       /usr      

# Define JAVA_HOME
#
ARG DOCKER_ARCH
ENV JAVA_HOME="/usr/local/java-${DOCKER_ARCH}-80"

# Update PATH
#
ARG DOCKER_ARCH
ENV PATH="/usr/local/apache-ant-1.9.6/bin:${PATH}"
ENV PATH="/usr/local/apache-maven-3.6.3/bin:${PATH}"
ENV PATH="/usr/local/java-${DOCKER_ARCH}-80/bin:${PATH}"

# Update ld.so.conf
#
RUN echo "/usr/local/lib" >> /etc/ld.so.conf.d/local.conf \
 && echo "/usr/local/lib64" >> /etc/ld.so.conf.d/local.conf \
 && echo "${JAVA_HOME}/jre/lib/amd64" >> /etc/ld.so.conf.d/openjdk.conf \
 && echo "${JAVA_HOME}/jre/lib/amd64/compressedrefs" >> /etc/ld.so.conf.d/openjdk.conf \
 && echo "${JAVA_HOME}/jre/lib/amd64/j9vm" >> /etc/ld.so.conf.d/openjdk.conf \
 && ldconfig

# Install any needed packages
#
RUN yum install -y \
        deltarpm \
        epel-release \
 && yum clean -y all \
 && rm -rf /var/cache/yum

# Documentation-specific dependencies
#
RUN yum install -y \
        doxygen \
        python36-pip \
        tidyp \
 && yum clean -y all \
 && rm -rf /var/cache/yum

# Build-specific dependencies
#
RUN yum install -y \
        cmake3 \
        gcc \
        gcc-c++ \
        jq \
        make \
        ninja-build \
        perl-YAML \
        rsync \
 && yum clean -y all \
 && rm -rf /var/cache/yum \
 && ln -s /usr/bin/cmake3 /usr/bin/cmake

# Install mkdocs
#
RUN pip3 install mkdocs mkdocs_cinder

# Add the doas script
#
ADD doas /usr/local/bin

# Install the development version of the libraries
#
RUN yum install -y \
        binutils-devel \
        hiredis-devel \
        libcap-devel \
        libcurl-devel \
        libxml2-devel \
        libxslt-devel \
        openldap-devel \
        openssl-devel \
        pam-devel \
        perl-Data-Dumper \
        perl-XML-Simple \
        protobuf-devel \
        xerces-c-devel \
 && yum clean -y all \
 && rm -rf /var/cache/yum
