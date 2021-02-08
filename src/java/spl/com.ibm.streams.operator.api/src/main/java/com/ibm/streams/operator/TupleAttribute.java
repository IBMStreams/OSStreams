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

/**
 * Defines an attribute for a specific port. An {@code Operator} that requires an {@code Attribute}
 * parameter must contain a property of this type.
 *
 * <p>The setter method for the property of type {@code TupleAttribute} must then be annotated with
 * the {@link com.ibm.streams.operator.model.Parameter} annotation. At operator initialization, the
 * Java runtime will initialize the property with the attribute information as specified in the SPL
 * application by calling its setter method. <br>
 * For example, an operator property can be specified as follows:
 *
 * <pre>
 * <code>
 * private TupleAttribute&lt;Tuple,Integer&gt; sortBy;
 * ...
 * &#64;Parameter
 * public void setSortBy(TupleAttribute&lt;Tuple,Integer&gt; s) {
 * this.sortBy = s;
 * }
 * </code>
 * </pre>
 *
 * <br>
 * In an SPL application, the parameter can then be used as follows:
 *
 * <pre>
 * <code>
 * param
 * param1: s;
 * </code>
 * </pre>
 *
 * where {@code s} is an attribute on an input stream of the operator. <br>
 * To specify a default attribute to be used with an {@code Atribute} parameter, specify the
 * attribute name using the {@code com.ibm.streams.operator.model.DefaultAttribute} annotation.
 *
 * @param T Type of the tuple that is supported by the referenced input port ({@link
 *     com.ibm.streams.operator.StreamingInput})
 * @param A Type of the attribute
 * @see com.ibm.streams.operator.model.Parameter
 * @see com.ibm.streams.operator.model.DefaultAttribute
 * @since InfoSphere&reg; Streams Version 4.0
 */
public interface TupleAttribute<T, A> {
  /** The port whose schema includes the referenced attribute */
  StreamingInput<T> getPort();

  /** A reference to the actual attribute object */
  Attribute getAttribute();

  /**
   * The value of the attribute in the specified tuple.
   *
   * @param tuple The tuple in which the attribute is to be located
   * @return The value of the attribute in tuple T as type A
   */
  A getValue(T tuple);
}
