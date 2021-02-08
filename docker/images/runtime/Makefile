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

NAME := streams-runtime
BUILD := $(shell echo "$(BUILD_TYPE)" | tr '[:upper:]' '[:lower:]')
VERSION := 6.$(DOCKER_ARCH).$(BUILD)

REPO_ROOT := $(shell realpath $(shell pwd)/../../..)
WORKDIR := $(REPO_ROOT)/build/images/runtime

pre-build:
	@./prepare $(REPO_ROOT) $(WORKDIR)

pre-push:

include ../../Makefile.inc
