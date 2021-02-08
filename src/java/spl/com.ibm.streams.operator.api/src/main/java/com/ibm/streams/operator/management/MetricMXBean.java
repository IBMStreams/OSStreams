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

package com.ibm.streams.operator.management;

import com.ibm.streams.operator.metrics.Metric;
import com.ibm.streams.operator.metrics.Metric.Kind;
import java.util.Hashtable;

/**
 * The management bean for a {@link Metric}.
 *
 * @since InfoSphere&reg; Streams Version 3.0
 * @see OperatorContextMXBean#registerCustomMetric(String)
 * @see OperatorContextMXBean#registerInputPortMetrics(int)
 * @see OperatorContextMXBean#registerOutputPortMetrics(int)
 * @see Metric
 */
public interface MetricMXBean {

  /**
   * Property key in the registered bean's object name that defines the type of the metric. The
   * value will correspond to a {@link MetricType#type()} return value.
   */
  public String METRIC_PROPERTY = "metric";

  /** Type of metrics represented by a {@link MetricMXBean} */
  public static enum MetricType {
    /** Operator custom metric. */
    CUSTOM {

      /**
       * Type for an operator custom metric.
       *
       * @return custom
       */
      @Override
      public String type() {
        return "custom";
      }
    },
    /** Operator input port metric. */
    OPERATOR_INPUT_PORT {
      /**
       * Type for an operator custom metric.
       *
       * @return operatorInputPort
       */
      @Override
      public String type() {
        return "operatorInputPort";
      }
    },
    /** Operator output port metric. */
    OPERATOR_OUTPUT_PORT {
      /**
       * Type for an operator custom metric.
       *
       * @return operatorOutputPort
       */
      @Override
      public String type() {
        return "operatorOutputPort";
      }
    },
    ;
    /**
     * Return the value used for {@link MetricMXBean#METRIC_PROPERTY} in an {@code ObjectName}.
     *
     * @return Value for {@code METRIC_PROPERTY}
     */
    public abstract String type();

    /**
     * Add this metric type to {@code objectNameProperties} for use in creating an {@code
     * ObjectName}.
     *
     * @param objectNameProperties Properties used to create an {@code ObjectName}
     */
    public void addType(final Hashtable<String, String> objectNameProperties) {
      objectNameProperties.put(METRIC_PROPERTY, type());
    }
  }

  /**
   * Get the name of this metric.
   *
   * @return Name of the metric.
   */
  public String getName();

  /**
   * Get the description of this metric.
   *
   * @return Description of the metric.
   */
  public String getDescription();

  /**
   * Get the kind of this metric.
   *
   * @return Kind of the metric.
   */
  public Kind getKind();

  /**
   * Get the value of this metric.
   *
   * @return Value of the metric.
   * @exception IllegalStateException Operator has been shutdown.
   */
  public long getValue();
}
