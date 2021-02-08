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

# Build
#
ARG NPROC

RUN yum install -y \
        deltarpm \
        epel-release \
 && yum install -y \
        cmake3 \
        gcc \
        gcc-c++ \
        git \
        libcurl-devel \
        make \
        zlib-devel \
 && git clone https://github.com/jupp0r/prometheus-cpp \
      --depth 1 \
      --branch v0.11.0 \
 && cd prometheus-cpp \
 && git submodule init \
 && git submodule update \
 && mkdir build \
 && cd build \
 && cmake3 .. \
 && make -j $NPROC install \
 && cd ../.. \
 && rm -rf prometheus-cpp \
 && yum autoremove -y \
        cmake3 \
        gcc \
        gcc-c++ \
        git \
        libcurl-devel \
        make \
        zlib-devel \
 && yum autoremove -y \
        deltarpm \
        epel-release \
 && yum clean -y all \
 && rm -rf /var/cache/yum
