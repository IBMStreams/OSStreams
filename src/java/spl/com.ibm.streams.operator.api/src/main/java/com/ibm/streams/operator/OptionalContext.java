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

package com.ibm.streams.operator;

import com.ibm.streams.operator.state.ConsistentRegionContext;

/**
 * An invocation of an operator may have optional contexts, that provide information specific to a
 * particular environment, such as a {@link ConsistentRegionContext consistent region}. <br>
 * An optional context can be obtained through {@link OperatorContext#getOptionalContext(Class)}.
 * <br>
 * Any optional context interface will extend this interface.
 *
 * @since InfoSphere&reg; Streams Version 4.0
 */
public interface OptionalContext {}
