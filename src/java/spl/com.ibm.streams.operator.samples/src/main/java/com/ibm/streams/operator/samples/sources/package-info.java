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
 * Sample SPL Java primitive operators that source data as streams. If an operator class is
 * annotated with {@link com.ibm.streams.operator.model.PrimitiveOperator} then a corresponding
 * primitive operator exists in the sample {@code JavaOperators} toolkit, located in {@code
 * $STREAMS_INSTALL/samples/spl/feature/JavaOperators}. Sample operators in this Java package have
 * the SPL namespace {@code com.ibm.streams.javaprimitivesamples.sources} defined by the {@link
 * com.ibm.streams.operator.model.Namespace} annotation for this package.
 */
@Namespace("com.ibm.streams.javaprimitivesamples.sources")
package com.ibm.streams.operator.samples.sources;

import com.ibm.streams.operator.model.Namespace;
