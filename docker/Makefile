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

DOCKER_ARCH ?= $(shell uname -m)
DOCKER_NAMESPACE ?= openstreams
DOCKER_REGISTRY ?= docker.io

# Build parameters
#
BUILD_TYPE ?= Debug
NPROC ?= $(shell ./tools/cpucount)

# Select the proper centos base image
#
ifeq ($(ARCH),ppc64le)
CENTOS_IMAGE := ppc64le/centos:7
else
CENTOS_IMAGE := centos:7
endif

# Build streams
#
all:
	@echo "Streams image build system:"
	@echo " - builder: streams-builder image to build Streams"
	@echo " - runtime: streams-runtime image to run Streams"

base: \
	antlr3c-pkg \
	aws-sdk-cpp-pkg \
	icu4c-pkg \
	boost-pkg \
	decnumber-pkg \
	dita-ot-pkg \
	openjdk-pkg \
	prometheus-cpp-pkg \
	rocksdb-pkg \
	tecla-pkg \
	xqilla-pkg \
	base-img

builder: \
	base \
	ant-pkg \
	antlr3-pkg \
	clang-tools-pkg \
	maven-pkg \
	xsd-pkg \
	builder-img

runtime: \
	base \
	runtime-img

# Build/push dependencies
#
%-pkg: %-pkg-build %-pkg-push
	@echo "[PKG] $*"

%-pkg-build:
	@echo "[BLD] $*";
	@$(MAKE) --no-print-directory -C packages/$* \
		DOCKER_ARCH=$(DOCKER_ARCH) \
		DOCKER_IMAGE=$(CENTOS_IMAGE) \
		DOCKER_NAMESPACE=$(DOCKER_NAMESPACE) \
		DOCKER_REGISTRY=$(DOCKER_REGISTRY) \
		BUILD_TYPE=$(BUILD_TYPE) \
		NPROC=$(NPROC) \
		build

%-pkg-push:
	@echo "[PSH] $*"
	@$(MAKE) --no-print-directory -C packages/$* \
		DOCKER_ARCH=$(DOCKER_ARCH) \
		DOCKER_IMAGE=$(CENTOS_IMAGE) \
		DOCKER_NAMESPACE=$(DOCKER_NAMESPACE) \
		DOCKER_REGISTRY=$(DOCKER_REGISTRY) \
		BUILD_TYPE=$(BUILD_TYPE) \
		push

# Image rules
#
%-img: %-img-build %-img-push
	@echo "[IMG] $*"

%-img-build:
	@echo "[BLD] $*";
	@$(MAKE) --no-print-directory -C images/$* \
		DOCKER_ARCH=$(DOCKER_ARCH) \
		DOCKER_IMAGE=$(CENTOS_IMAGE) \
		DOCKER_NAMESPACE=$(DOCKER_NAMESPACE) \
		DOCKER_REGISTRY=$(DOCKER_REGISTRY) \
		BUILD_TYPE=$(BUILD_TYPE) \
		NPROC=$(NPROC) \
		build

%-img-push:
	@echo "[PSH] $*"
	@$(MAKE) --no-print-directory -C images/$* \
		DOCKER_ARCH=$(DOCKER_ARCH) \
		DOCKER_IMAGE=$(CENTOS_IMAGE) \
		DOCKER_NAMESPACE=$(DOCKER_NAMESPACE) \
		DOCKER_REGISTRY=$(DOCKER_REGISTRY) \
		BUILD_TYPE=$(BUILD_TYPE) \
		push
