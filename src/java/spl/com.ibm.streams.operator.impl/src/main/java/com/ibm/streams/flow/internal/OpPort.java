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

package com.ibm.streams.flow.internal;

import com.ibm.streams.flow.declare.InputPortDeclaration;
import com.ibm.streams.flow.declare.OperatorInvocation;
import com.ibm.streams.flow.declare.OutputPortDeclaration;
import com.ibm.streams.flow.declare.PortDeclaration;
import com.ibm.streams.flow.declare.StreamConnection;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.internal.runtime.AbstractStream;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.MessageKey;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

/** Declared output port. */
abstract class OpPort<P extends PortDeclaration<P>> extends AbstractStream
    implements PortDeclaration<P> {

  /** my operator */
  private final OpInvocation<?> op;

  private final List<StreamConnection> connections =
      Collections.synchronizedList(new ArrayList<StreamConnection>());

  OpPort(OpInvocation<?> op, int index, String name, StreamSchema schema) {
    super(index, name, schema, NOT_CONNECTED_TO_PE_PORT);
    this.op = op;
  }

  public final synchronized boolean isConnected() {
    return !connections.isEmpty();
  }

  final OpInvocation<?> op() {
    return op;
  }

  @Override
  public final OperatorInvocation<?> operator() {
    return op;
  }

  final void checkNotConnected() {
    if (isConnected())
      throw new IllegalStateException(portMsg(Key.SPL_RUNTIME_OPERATOR_PORT_IS_CONNECTED));
  }

  /** Format a message where arg {0} is always the port name. */
  protected String portMsg(String fmt, Object... arguments) {
    Object[] pargs = new Object[arguments.length + 1];
    pargs[0] = getName();
    System.arraycopy(arguments, 0, pargs, 1, arguments.length);
    return MessageFormat.format(fmt, pargs);
  }

  /** Load a message where arg {0} is always the port name. */
  protected String portMsg(MessageKey key, Object... arguments) {
    return new Message(key, getName(), arguments).getLocalizedMessageAndPrefix();
  }

  @Override
  public final Collection<StreamConnection> getConnections() {
    return Collections.unmodifiableList(connections);
  }

  // StreamConnections don't have any real meat yet, so just implement
  // them as a anon class.
  void addConnection(final InputPortDeclaration input, final OutputPortDeclaration output) {
    connections.add(
        new StreamConnection() {

          @Override
          public InputPortDeclaration getInput() {
            return input;
          }

          @Override
          public OutputPortDeclaration getOutput() {
            return output;
          }
        });
  }
}
