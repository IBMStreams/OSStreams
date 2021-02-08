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

# -*- makefile -*-

PWD := $(shell pwd)

REPO_ROOT :=  $(realpath $(PWD)/..)
DST_TOOLS := $(REPO_ROOT)/tools
BUILD_INFO := $(DST_TOOLS)/build-info
DESTDIR := $(shell $(BUILD_INFO) --installdir)
INSTALL_DIR := $(shell $(BUILD_INFO) --installdir)

NPROC ?= 12
DST_BUILD_DIR ?= $(shell $(BUILD_INFO) --gendir)/objects
BUILD_TYPE ?= Debug

SPL_MAKE_TOOLKIT = LD_LIBRARY_PATH=$(INSTALL_DIR)/lib:$(INSTALL_DIR)/system/impl/lib STREAMS_INSTALL=$(INSTALL_DIR) $(INSTALL_DIR)/bin/spl-make-toolkit
SPL_MAKE_DOC = LD_LIBRARY_PATH=$(INSTALL_DIR)/lib:$(INSTALL_DIR)/system/impl/lib STREAMS_INSTALL=$(INSTALL_DIR) $(INSTALL_DIR)/bin/spl-make-doc

PACKAGE_VERSION = $(shell $(DST_TOOLS)/product-info --version V.R.M.L)
PKG_CONFIG_INSTALL = $(DST_TOOLS)/install-pkg-config $(shell $(DST_TOOLS)/build-info --pkgconfigdir)

.PHONY: dstconfig configure all test format lint install pkgconfig toolkits clean distclean

default: dstconfig configure all pkgconfig install toolkits

$(DST_BUILD_DIR)/cpp/dst-config.h:
	mkdir -p $(DST_BUILD_DIR)/cpp
	echo -e "#pragma once\n\n#define PACKAGE_VERSION \"$(PACKAGE_VERSION)\"" > $@

configure: $(DST_BUILD_DIR)/cpp/dst-config.h
	cd $(DST_BUILD_DIR); cmake -GNinja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(PWD); cd $(PWD)

all:
	ninja-build -C $(DST_BUILD_DIR) -j $(NPROC) $@

doc:
	ninja-build -C $(DST_BUILD_DIR) -j $(NPROC) spl_doc_generate

# The install step also installs PKGCONFIG files in BUILD so that some tests
# using ${BUILD}/tools/bin/dst-pkg-config.sh can pass.
#
install:
	ninja-build -C $(DST_BUILD_DIR) $@
	$(PKG_CONFIG_INSTALL) pkgconfigcfg/*.pc.tmpl
	$(PKG_CONFIG_INSTALL) ${DST_BUILD_DIR}/cpp/SPL/pkgconfigcfg/*.pc

test:
	ninja-build -C $(DST_BUILD_DIR) test

format:
	ninja-build -C $(DST_BUILD_DIR) streams_format

lint:
	ninja-build -C $(DST_BUILD_DIR) streams_lint

clean:
	ninja-build -C $(DST_BUILD_DIR) $@

distclean:
	rm -rf $(DST_BUILD_DIR)

standard-toolkit:
	$(SPL_MAKE_TOOLKIT) -i $(INSTALL_DIR)/toolkits/spl
	$(SPL_MAKE_DOC) --output-directory $(INSTALL_DIR)/toolkits/spl/doc/en_US/spldoc -t $(INSTALL_DIR)/toolkits/spl --doc-title "SPL standard Toolkit"

toolkits: standard-toolkit
