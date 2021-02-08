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

package com.ibm.streams.operator.types;

import java.io.Serializable;
import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.concurrent.TimeUnit;

/**
 * SPL timestamp. A Timestamp represents a number of seconds and nanoseconds, where nanoseconds must
 * be in the range 0-999,999,999. Additional an application specific machine integer identifier is
 * provided to allow precise time calculations if the time skew across machines is known.
 *
 * <p>Timestamp may be used as an absolute point in time, by using the convention that its value is
 * relative to January 1, 1970, 00:00:00 GMT (UTC), the epoch. This is the standard epoch for SPL,
 * Java and Linux. A number of utility methods exist for interacting with Java's millisecond
 * representation of the time since the epoch, typically obtained through <code>
 * System.currentTimeMillis()</code>.
 *
 * <p>A Timestamp object may also be used to represent an interval, e.g. an instance representing a
 * delay of four seconds could be created as <code>new Timestamp(4L, 0);</code>. <br>
 * The Java enumeration <code>java.unit.concurrent.TimeUnit</code> provides standard conversion
 * between various units of time, for example a delay of 700 milliseconds can be created as
 *
 * <pre>
 *   Timestamp delay = new Timestamp(0L,
 *      (int) TimeUnit.MILLISECONDS.toNanos(700L));
 * </pre>
 *
 * or
 *
 * <pre>
 *   Timestamp delay = Timestamp.getTimestamp(
 *              TimeUnit.MILLISECONDS.toNanos(700L));
 * </pre>
 */
public final class Timestamp implements Serializable, Comparable<Timestamp> {

  private static final int NS_S = (int) TimeUnit.SECONDS.toNanos(1);

  private static final BigDecimal NS_S_BD = new BigDecimal(NS_S);

  /** */
  private static final long serialVersionUID = -153678588052923889L;

  /** Default Machine identifier (zero). */
  public static final int DEFAULT_MACHINE = 0;

  private final long seconds;
  private final int nanoseconds;
  private final int machineId;

  /**
   * Construct a Timestamp with the given values and a machine identifier set to zero.
   *
   * @param seconds Number of seconds since epoch.
   * @param nanoseconds Number of nanoseconds since epoch.
   */
  public Timestamp(long seconds, int nanoseconds) {
    this(seconds, nanoseconds, DEFAULT_MACHINE);
  }

  /**
   * Construct a Timestamp with the given values.
   *
   * @param seconds Number of seconds.
   * @param nanoseconds Number of seconds.
   * @param machineId Machine identifier.
   */
  public Timestamp(long seconds, int nanoseconds, int machineId) {
    if (nanoseconds < 0 || nanoseconds >= NS_S)
      throw new IllegalArgumentException(
          "Nano-second value must be in the range of 0-999999999:nanos=" + nanoseconds);
    this.seconds = seconds;
    this.nanoseconds = nanoseconds;
    this.machineId = machineId;
  }

  /**
   * Create a Timestamp representing the current time since the epoch. The time is obtained from
   * <code>
   * System.currentTimeMillis()</code> and thus the resolution will at most be in milliseconds.
   * Actual resolution is dependent on the resolution of the system clock.
   *
   * @return Timestamp set to current time relative to the epoch and machine identifier set to zero.
   * @see System#currentTimeMillis()
   */
  public static Timestamp currentTime() {
    return currentTime(DEFAULT_MACHINE);
  }

  /**
   * Create a Timestamp representing the current time since the epoch and the give machine
   * identifier.
   *
   * @param machineId Machine identifier.
   * @return Timestamp set to current time relative to the epoch.
   * @see System#currentTimeMillis()
   */
  public static Timestamp currentTime(int machineId) {
    return Timestamp.getTimestamp(System.currentTimeMillis(), machineId);
  }

  /**
   * Create a Timestamp from a number of milliseconds. This may be used to generate Timestamp
   * objects using a value that represents the number of milliseconds since the epoch.
   *
   * @param milliseconds number of milliseconds.
   * @return New Timestamp object set from milliseconds with machineId set to zero
   * @see java.util.Date#getTime()
   */
  public static Timestamp getTimestamp(long milliseconds) {
    return Timestamp.getTimestamp(milliseconds, DEFAULT_MACHINE);
  }

  /**
   * Create a Timestamp from a number of milliseconds and a machine identifier. This may be used to
   * generate Timestamp objects using a value that represents the number of milliseconds since the
   * epoch.
   *
   * @param milliseconds number of milliseconds.
   * @param machineId Machine identifier.
   * @return New Timestamp object
   * @see java.util.Date#getTime()
   */
  public static Timestamp getTimestamp(final long milliseconds, final int machineId) {

    long s = TimeUnit.MILLISECONDS.toSeconds(milliseconds);
    int ns = (int) TimeUnit.MILLISECONDS.toNanos(milliseconds % 1000L);

    // Need to always represent the nano-seconds as positive, so
    // if ns is negative here (because the remainder (%) operator takes
    // the sign of the dividend) re-factor it into a positive value
    // by adding one second to the nano-second value and compensating
    // by subtracting one second from the seconds value.
    // Seconds (s) cannot underflow as the minimum value at this point is
    // Long.MIN_VALUE / 1000.
    if (ns < 0) {
      s--;
      ns += NS_S;
    }

    return new Timestamp(s, ns, machineId);
  }

  /**
   * Convert a JDBC Timestamp to an SPL timestamp representation with its machine identifier set to
   * zero. JDBC timestamps have the same resolution as this class.
   *
   * @param sqlTimestamp Value to convert from.
   * @return The converted timestamp.
   */
  public static Timestamp getTimestamp(final java.sql.Timestamp sqlTimestamp) {
    return getTimestamp(sqlTimestamp, DEFAULT_MACHINE);
  }
  /**
   * Convert a JDBC Timestamp to an SPL timestamp representation with a specified machine
   * identifier. JDBC timestamps have the same resolution as this class.
   *
   * @param sqlTimestamp Value to convert from.
   * @param machineId Machine identifier
   * @return The converted timestamp.
   */
  public static Timestamp getTimestamp(final java.sql.Timestamp sqlTimestamp, final int machineId) {

    long s = TimeUnit.MILLISECONDS.toSeconds(sqlTimestamp.getTime());
    return new Timestamp(s, sqlTimestamp.getNanos(), machineId);
  }

  /**
   * Create a timestamp from a double representing number of seconds with its machine identifier set
   * to zero.
   *
   * @param seconds Value to set timestamp to.
   * @return Timestamp object set from seconds.
   */
  public static Timestamp getTimestamp(final double seconds) {
    return getTimestamp(new BigDecimal(seconds));
  }

  /**
   * Create a timestamp from a BigDecimal representing number of seconds with its machine identifier
   * set to zero. The <code>seconds</code> value is rounded down to a nano-second resolution.
   *
   * @param seconds Value to set timestamp to.
   * @return Timestamp object set from seconds.
   */
  public static Timestamp getTimestamp(BigDecimal seconds) {
    seconds = seconds.setScale(9, RoundingMode.DOWN);
    BigDecimal nanoseconds = seconds.remainder(BigDecimal.ONE);
    BigDecimal wholeSeconds = seconds.subtract(nanoseconds);
    return new Timestamp(
        wholeSeconds.longValueExact(),
        nanoseconds.multiply(NS_S_BD).intValueExact(),
        DEFAULT_MACHINE);
  }

  /**
   * Return the application specific machine identifier associated with this timestamp.
   *
   * @return Application specific machine identifier.
   */
  public int getMachineId() {
    return machineId;
  }

  /**
   * Return nanoseconds portion of the time since the epoch.
   *
   * @return nanoseconds portion since epoch.
   */
  public int getNanoseconds() {
    return nanoseconds;
  }

  /**
   * Return seconds portion of the time since the epoch.
   *
   * @return seconds portion since epoch.
   */
  public long getSeconds() {
    return seconds;
  }

  /**
   * Get the value as a number of milliseconds. If this Timestamp contains a value relative to the
   * epoch then the returned value matches the standard Java long representation of a time since the
   * epoch.
   *
   * @return Number of milliseconds.
   * @see java.util.Date#setTime(long)
   */
  public long getTime() {

    long s = getSeconds();
    int ns = getNanoseconds();

    // If seconds is negative then increase by one seconds with a compensating
    // factor of decreasing the nanoseconds by one, to avoid overflow
    // when converting to milli-seconds.
    if (s < 0) {
      s++;
      ns -= NS_S;
    }

    long msSec = TimeUnit.SECONDS.toMillis(s);

    if (msSec == Long.MAX_VALUE || msSec == Long.MIN_VALUE) {
      throw conversionError("Timestamp.getTime()");
    }

    long msNanos = TimeUnit.NANOSECONDS.toMillis(ns);

    long ms = msSec + msNanos;

    // Check for overflow when we added the nano-seconds in. Nano-seconds
    // is always positive thus an overflow is indicated by ms being negative
    // and msSec being negative.
    if (ms < 0 && msSec > 0) {
      throw conversionError("Timestamp.getTime()");
    }

    return ms;
  }

  /**
   * Return a SQL Timestamp object set from this Timestamp.
   *
   * @return A SQL Timestamp set to the same value.
   */
  public java.sql.Timestamp getSQLTimestamp() {

    long ms = TimeUnit.SECONDS.toMillis(getSeconds());
    if (ms == Long.MAX_VALUE || ms == Long.MIN_VALUE)
      throw conversionError("Timestamp.getSQLTimestamp()");
    final java.sql.Timestamp sql = new java.sql.Timestamp(ms);
    sql.setNanos(getNanoseconds());
    return sql;
  }

  /**
   * Returns a hash code for this {@code Timestamp}. The machine identifier is not used in
   * calculating the hash code.
   */
  @Override
  public int hashCode() {
    return getNanoseconds() ^ (int) getSeconds();
  }

  /**
   * A Timestamp is equal to another Timestamp with the same seconds and nanoseconds value. Machine
   * identifier is ignored.
   */
  @Override
  public boolean equals(Object other) {
    if (!(other instanceof Timestamp)) return false;
    Timestamp ots = (Timestamp) other;
    return (ots.getNanoseconds() == getNanoseconds()) && (ots.getSeconds() == getSeconds());
  }

  /**
   * Get the time in seconds (including the number of nanoseconds) as a BigDecimal.
   *
   * @return A BigDecimal representing this Timestamp's value in seconds.
   */
  public BigDecimal getTimeAsSeconds() {

    BigDecimal s = new BigDecimal(getSeconds());
    BigDecimal ns = new BigDecimal(getNanoseconds()).scaleByPowerOfTen(-9);

    return s.add(ns);
  }

  /**
   * Convert this timestamp into the unit specified.
   *
   * @param unit
   * @return Value of this time in the converted unit.
   * @exception ArithmeticException This timestamp cannot be represented in the specified units.
   */
  public long convertTo(final TimeUnit unit) {
    long ts = unit.convert(getSeconds(), TimeUnit.SECONDS);
    if (ts == Long.MAX_VALUE || ts == Long.MIN_VALUE)
      throw conversionError("Timestamp.convertTo(" + unit + ")");

    long tns = unit.convert(getNanoseconds(), TimeUnit.NANOSECONDS);

    long tunit = ts + tns;
    if (tunit < ts) throw conversionError("Timestamp.convertTo(" + unit + ")");
    return tunit;
  }

  private ArithmeticException conversionError(String where) {
    return new ArithmeticException(where + ": " + getTimeAsSeconds().toPlainString());
  }

  /** Compare this to another Timestamp. Machine identifier is ignored. */
  @Override
  public int compareTo(Timestamp o) {
    if (getSeconds() < o.getSeconds()) return -1;
    if (getSeconds() > o.getSeconds()) return 1;

    if (getNanoseconds() < o.getNanoseconds()) return -1;
    if (getNanoseconds() > o.getNanoseconds()) return 1;
    return 0;
  }

  /**
   * Is this Timestamp after the specified Timestamp.
   *
   * @param ts Timestamp to compare.
   * @return True if this Timestamp is later, false otherwise.
   */
  public boolean after(Timestamp ts) {
    return compareTo(ts) > 0;
  }

  /**
   * Is this Timestamp before the specified Timestamp.
   *
   * @param ts Timestamp to compare.
   * @return True if this Timestamp is earlier, false otherwise.
   */
  public boolean before(Timestamp ts) {
    return compareTo(ts) < 0;
  }

  /**
   * Return a Timestamp that is the result of subtracting the delta Timestamp from this Timestamp.
   * The machine identifier for the resultant Timestamp is set to this Timestamp's machine
   * identifier.
   *
   * @param delta Value to be subtracted from this.
   * @return The result of the subtraction.
   */
  public Timestamp subtract(Timestamp delta) {
    long seconds = getSeconds() - delta.getSeconds();
    int nanoseconds = getNanoseconds() - delta.getNanoseconds();
    if (nanoseconds < 0) {
      seconds--;
      nanoseconds += NS_S;
    }
    return new Timestamp(seconds, nanoseconds, getMachineId());
  }

  /**
   * Return a Timestamp that is the result of adding the delta Timestamp to this Timestamp. The
   * machine identifier for the resultant Timestamp is set to this Timestamp's machine identifier.
   *
   * @param delta Value to be added to this.
   * @return The result of the addition.
   */
  public Timestamp add(Timestamp delta) {
    long seconds = getSeconds() + delta.getSeconds();
    int nanoseconds = getNanoseconds() + delta.getNanoseconds();
    if (nanoseconds >= NS_S) {
      nanoseconds -= NS_S;
      seconds++;
    }
    return new Timestamp(seconds, nanoseconds, getMachineId());
  }

  /**
   * Return this timestamp as a {@code String} using the SPL character encoding. The value returned
   * has the format: {@code (seconds, nanoseconds, machineId)}.
   *
   * @return String representation of this timestamp using SPL character encoding.
   * @since InfoSphere&reg; Streams Version 4.0
   */
  @Override
  public String toString() {
    // 44 is the length of new Timestamp(Long.MAX_VALUE, 999999999, Integer.MAX_VALUE)
    StringBuilder sb = new StringBuilder(44);
    sb.append('(');
    sb.append(getSeconds());
    sb.append(", ");
    sb.append(getNanoseconds());
    sb.append(", ");
    sb.append(getMachineId());
    sb.append(')');
    return sb.toString();
  }
}
