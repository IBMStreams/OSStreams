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

package com.ibm.streams.operator.model;

import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Declare a default attribute to be used as the value for an optional 'Attribute' parameter.
 *
 * <p>The name of the attribute to be used as the default must be specified as the value of the
 * annotation. If an attribute by the specified name exists in more than one input port's schema
 * then the matching attribute in the first such port will be used as the parameter value. <br>
 * This annotation must be used in conjunction with the {@code
 * com.ibm.streams.operator.model.Parameter} annotation on a setter method of a Java bean of type
 * {@code com.ibm.streams.operator.TupleAttribute}. For example, to specify that the 'jsonAttribute'
 * optional parameter will pick up the 'jsonString' attribute from the stream by default (unless
 * specified otherwise), you would specify the following: <br>
 *
 * <pre><code>
 * &#64;Parameter
 * &#64;DefaultAttribute("jsonString")
 * public void setJsonAttribute(TupleAttribute<Tuple, String> ta) {...}
 * </code></pre>
 *
 * <br>
 * When the {@code DefaultAttribute} parameter is used for the parameter, the parameter defaults to
 * be being optional and having a cardinality of 1.
 *
 * @see com.ibm.streams.operator.model.Parameter
 * @see com.ibm.streams.operator.TupleAttribute
 * @since InfoSphere&reg; Streams Version 4.0
 */
@Target({ElementType.METHOD})
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface DefaultAttribute {
  /** Name of the attribute to be used as the default */
  String value();
}
