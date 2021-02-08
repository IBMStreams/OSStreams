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
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/streams-base:6.${DOCKER_ARCH} AS base

# Arguments
#
ARG DOCKER_ARCH
ARG BUILD_TYPE

# Add the kubernetes repo
#
COPY kubernetes-${DOCKER_ARCH}.repo /etc/yum.repos.d/kubernetes.repo

# Add the doas script
#
ADD doas /usr/local/bin

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

# Update the environment
#
ENV STREAMS_INSTALL=/usr/local/streams
ENV PATH="/usr/local/streams/bin:${PATH}"

# Install some extra dependencies
#
RUN yum install -y epel-release \
 && yum install -y kubectl redis \
 && (([ "${BUILD_TYPE}x" = "debugx" ] && yum install -y gdb valgrind) || /bin/true) \
 && yum clean -y all \
 && rm -rf /var/cache/yum

# Install the production version of the libraries
#
RUN yum install -y \
        binutils \
        hiredis \
        libcap \
        libcurl \
        libxml2 \
        libxslt \
        openldap \
        openssl \
        pam \
        perl-Data-Dumper \
        perl-XML-Simple \
        protobuf \
        xerces-c \
 && yum clean -y all \
 && rm -rf /var/cache/yum

# Install the compiler toolchain, required for SC
#
RUN yum install -y \
        gcc-c++ \
 && yum clean -y all \
 && rm -rf /var/cache/yum

# Build information
#
ADD build.json /etc

# Add the install files
#
COPY install /usr/local/streams

# Update ld.so.conf
#
RUN echo "/usr/local/lib" >> /etc/ld.so.conf.d/local.conf \
 && echo "/usr/local/lib64" >> /etc/ld.so.conf.d/local.conf \
 && echo "${JAVA_HOME}/jre/lib/amd64" >> /etc/ld.so.conf.d/openjdk.conf \
 && echo "${JAVA_HOME}/jre/lib/amd64/compressedrefs" >> /etc/ld.so.conf.d/openjdk.conf \
 && echo "${JAVA_HOME}/jre/lib/amd64/j9vm" >> /etc/ld.so.conf.d/openjdk.conf \
 && echo "${STREAMS_INSTALL}/lib" >> /etc/ld.so.conf.d/streams.conf \
 && echo "${STREAMS_INSTALL}/system/impl/lib" >> /etc/ld.so.conf.d/streams.conf \
 && ldconfig
