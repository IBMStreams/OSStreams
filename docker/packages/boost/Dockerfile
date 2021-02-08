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

ARG DOCKER_ARCH
ARG DOCKER_NAMESPACE
ARG DOCKER_REGISTRY

# Derive from our local ICU for boost_locale
#
FROM ${DOCKER_REGISTRY}/${DOCKER_NAMESPACE}/icu4c:56.2.${DOCKER_ARCH}

# Build
#
ARG NPROC

RUN yum install -y \
        deltarpm \
 && yum install -y \
        bzip2-devel \
        gcc \
        gcc-c++ \
        zlib-devel \
 && curl -sL \
      https://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.gz/download \
      -o boost_1_55_0.tar.gz \
 && tar -xzf boost_1_55_0.tar.gz \
 && rm -f boost_1_55_0.tar.gz \
 && cd boost_1_55_0 \
 && ./bootstrap.sh \
 && ./b2 install -j $NPROC --without-python --without-mpi \
 && cd .. \
 && rm -rf boost_1_55_0 \
 && yum autoremove -y \
        bzip2-devel \
        gcc \
        gcc-c++ \
        zlib-devel \
 && yum autoremove -y \
        deltarpm \
 && yum clean -y all \
 && rm -rf /var/cache/yum
