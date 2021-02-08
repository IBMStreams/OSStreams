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
 && yum install -y \
        gcc \
        gcc-c++ \
        make \
 && curl -sL \
      https://github.com/unicode-org/icu/releases/download/release-56-2/icu4c-56_2-src.tgz \
      -o icu4c-56_2-src.tgz \
 && tar -xzf icu4c-56_2-src.tgz \
 && rm -f icu4c-56_2-src.tgz \
 && cd icu/source \
 && ./configure \
 && make -j $NPROC install \
 && cd ../.. \
 && rm -rf icu \
 && yum autoremove -y \
        gcc \
        gcc-c++ \
        make \
 && yum autoremove -y \
        deltarpm \
 && yum clean -y all \
 && rm -rf /var/cache/yum
