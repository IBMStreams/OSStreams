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

package com.ibm.streams.function.samples.jvm;

import com.ibm.streams.function.model.Function;
import java.lang.management.ManagementFactory;

public class SystemFunctions {

  /** Only expose the static methods. */
  private SystemFunctions() {}

  /**
   * Returns the Java virtual machine implementation name.
   *
   * @return The Java virtual machine implementation name.
   */
  @Function(description = "Returns the Java virtual machine implementation name.")
  public static String getVmName() {
    return ManagementFactory.getRuntimeMXBean().getVmName();
  }
  /**
   * Returns the Java virtual machine implementation vendor.
   *
   * @return The Java virtual machine implementation vendor
   */
  @Function(description = "Returns the Java virtual machine implementation vendor.")
  public static String getVmVendor() {
    return ManagementFactory.getRuntimeMXBean().getVmVendor();
  }
  /**
   * Returns the Java virtual machine implementation version.
   *
   * @return The Java virtual machine implementation version
   */
  @Function(description = "Returns the Java virtual machine implementation version.")
  public static String getVmVersion() {
    return ManagementFactory.getRuntimeMXBean().getVmVersion();
  }

  /** Request garbage collection for the Java virtual machine. */
  @Function(
      stateful = true, // Stateful as it modified state outside the function
      description = "Request garbage collection for the Java virtual machine.")
  public static void gc() {
    System.gc();
  }

  /**
   * Request running of {@code Object.finalize()} for objects pending garbage collection in the Java
   * virtual machine.
   */
  @Function(
      stateful = true, // Stateful as it modified state outside the function
      description =
          "Request running of `Object.finalize()` for objects pending garbage collection in the Java virtual machine.")
  public static void runFinalization() {
    Runtime.getRuntime().runFinalization();
  }

  /**
   * Return the number of processors available to the Java virtual machine.
   *
   * <p>Calls {@code Runtime.getRuntime().availableProcessors()}.
   *
   * @return Return the number of processors available to the Java virtual machine.
   */
  @Function(
      stateful = true, // Stateful as it might return different values
      description = "Return the number of processors available to the Java virtual machine.")
  public static int availableProcessors() {
    return Runtime.getRuntime().availableProcessors();
  }

  /**
   * Returns the amount of free memory (in bytes) in the Java Virtual Machine. Calls {@code
   * Runtime.getRuntime().freeMemory()}.
   *
   * @return The amount of free memory in the Java Virtual Machine.
   */
  @Function(
      stateful = true, // Stateful as it might return different values
      description = "Returns the amount of free memory (in bytes) in the Java Virtual Machine.")
  public static long freeMemory() {
    return Runtime.getRuntime().freeMemory();
  }

  /**
   * Returns the total amount of memory in the Java virtual machine. Calls {@code
   * Runtime.getRuntime().totalMemory()}.
   *
   * @return The total amount of memory in the Java virtual machine.
   */
  @Function(
      stateful = true, // Stateful as it might return different values
      description = "Returns the total amount of memory in the Java virtual machine.")
  public static long totalMemory() {
    return Runtime.getRuntime().totalMemory();
  }
  /**
   * Returns the maximum amount of memory that the Java virtual machine will attempt to use. Calls
   * {@code Runtime.getRuntime().maxMemory()}.
   *
   * @return The maximum amount of memory that the Java virtual machine will attempt to use.
   */
  @Function(
      stateful = true, // Stateful as it might return different values
      description =
          "Returns the maximum amount of memory that the Java virtual machine will attempt to use.")
  public static long maxMemory() {
    return Runtime.getRuntime().maxMemory();
  }
}
