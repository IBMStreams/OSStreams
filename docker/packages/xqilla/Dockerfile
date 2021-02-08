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
        bison \
        file \
        flex \
        gcc \
        gcc-c++ \
        make \
        xerces-c-devel \
 && curl -sL \
      https://sourceforge.net/projects/xqilla/files/XQilla-2.3.0.tar.gz/download \
      -o XQilla-2.3.0.tar.gz \
 && tar -xzf XQilla-2.3.0.tar.gz \
 && rm -f XQilla-2.3.0.tar.gz \
 && cd XQilla-2.3.0 \
 && ./configure --with-xerces=/usr \
 && make -j $NPROC install \
 && cd .. \
 && rm -rf XQilla-2.3.0 \
 && yum autoremove -y \
        bison \
        file \
        flex \
        gcc \
        gcc-c++ \
        make \
        xerces-c-devel \
 && yum autoremove -y \
        deltarpm \
 && yum clean -y all \
 && rm -rf /var/cache/yum
