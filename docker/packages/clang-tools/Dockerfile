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
ARG NPROC

RUN yum install -y \
        deltarpm \
        epel-release \
 && yum install -y \
        cmake3 \
        gcc \
        gcc-c++ \
        git \
        make \
 && git clone https://github.com/llvm/llvm-project \
      --branch release/9.x \
      --depth 1 \
 && cd llvm-project \
 && mkdir build \
 && cd build \
 && cmake3 \
      -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra"  \
      -DCMAKE_BUILD_TYPE=Release \
      -DLLVM_TEMPORARILY_ALLOW_OLD_TOOLCHAIN=ON \
      ../llvm \
 && make -j $NPROC clang-format clang-tidy \
 && strip bin/clang-format \
 && cp bin/clang-format /usr/local/bin \
 && strip bin/clang-tidy \
 && cp bin/clang-tidy /usr/local/bin \
 && mkdir -p /usr/local/clang/include \
 && cp -rf ../clang/lib/Headers/*.h /usr/local/clang/include \
 && cd ../.. \
 && rm -rf llvm-project \
 && yum autoremove -y \
        cmake3 \
        gcc \
        gcc-c++ \
        git \
        make \
 && yum autoremove -y \
        deltarpm \
        epel-release \
 && yum clean -y all \
 && rm -rf /var/cache/yum
