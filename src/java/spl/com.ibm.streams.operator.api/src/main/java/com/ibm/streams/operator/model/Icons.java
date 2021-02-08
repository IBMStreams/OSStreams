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
 * Declare a set of icon files for the operator. The icons will be used by Streams Studio to show
 * the operator in the graphical editor.
 *
 * <p>A 16x16 icon file location as well as a 32x32 icon location must be provided. The location of
 * the icons is relative to the root directory of the toolkit containing the Java primitive
 * operator. <br>
 * For example, to specify an icon within the &lt;Toolkit Directory&gt;/icons folder, you would
 * specify the following: <br>
 *
 * <pre><code>
 * &#64;PrimitiveOperator
 * &#64;Icons(location16="icons/operator_icon16.gif" location32="icons/operator_icon32.gif")
 * public class SomeOp extends AbstractOperator
 * </code></pre>
 *
 * @since InfoSphere&reg; Streams Version 3.2
 */
@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface Icons {
  /**
   * Location of the 16x16 icon file relative to the toolkit directory.
   *
   * @return The location of the 16x16 icon file
   */
  String location16();

  /**
   * Location of the 32x32 icon file relative to the toolkit directory.
   *
   * @return The location of the 32x32 icon file
   */
  String location32();
}
