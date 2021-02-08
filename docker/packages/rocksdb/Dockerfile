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
        make \
 && curl -sL \
      https://github.com/facebook/rocksdb/archive/v5.18.3.tar.gz \
      -o rocksdb-5.18.3.tar.gz \
 && tar -xzf rocksdb-5.18.3.tar.gz \
 && rm -f rocksdb-5.18.3.tar.gz \
 && cd rocksdb-5.18.3 \
 && mkdir build \
 && cd build \
 && cmake3 -DCMAKE_INSTALL_PREFIX=/usr/local -DWITH_GFLAGS=OFF -DWITH_TESTS=OFF -DWITH_TOOLS=OFF .. \
 && make -j $NPROC install \
 && cd ../.. \
 && rm -rf rocksdb-5.18.3 \
 && yum autoremove -y \
        cmake3 \
        gcc \
        gcc-c++ \
        make \
 && yum autoremove -y \
        deltarpm \
        epel-release \
 && yum clean -y all \
 && rm -rf /var/cache/yum
