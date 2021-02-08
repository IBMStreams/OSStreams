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

GROFF=groff
#/homes/hny1/bgedik/local64/bin/groff

all: html/sc.1.html html/spl-make-operator.1.html html/spl-make-toolkit.1.html

html_dir:
	mkdir -p html

html/sc.1.html: src/sc.1 | html_dir
	$(GROFF) -man -Thtml $< > $@

html/spl-make-operator.1.html: src/spl-make-operator.1 | html_dir
	$(GROFF) -man -Thtml $< > $@

html/spl-make-toolkit.1.html: src/spl-make-toolkit.1 | html_dir
	$(GROFF) -man -Thtml $< > $@

clean:
	rm -fr html