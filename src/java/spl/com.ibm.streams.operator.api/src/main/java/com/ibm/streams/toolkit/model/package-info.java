/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Annotation to define a set of Java class libraries to be added at execution time to the toolkit's
 * class loader. The toolkit's class loader is used for loading SPL Java native functions declared
 * with the {@link com.ibm.streams.function.model.Function} annotation. The library paths added to
 * the toolkit's class loader may be specified with the {@link
 * com.ibm.streams.toolkit.model.ToolkitLibraries} annotation.
 *
 * @since IBM& Streams Version 4.2
 */
package com.ibm.streams.toolkit.model;
