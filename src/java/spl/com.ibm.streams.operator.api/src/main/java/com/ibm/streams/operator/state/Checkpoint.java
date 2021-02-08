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

package com.ibm.streams.operator.state;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

/**
 * Represents a checkpoint.
 *
 * <p>An instance of this interface will be created and passed to the operator's registered {@code
 * StateHandler} through its {@code checkpoint} or {@code reset methods}. <br>
 * The same interface will be used at checkpoint or restore time. The following example shows how an
 * operator can save state information (in this case, an int and a string) to a back end store using
 * this interface:
 *
 * <pre>
 * <code>
 * public void checkpoint(Checkpoint checkpoint) throws Exception {
 * 		// get an outputstream to write data to
 * 		ObjectOutputStream stream = checkpoint.getOutputStream();
 *
 * 		// write an integer
 * 		stream.writeInt(42);
 *
 * 		// write a string
 * 		stream.writeUTF("string");
 * }
 * </code>
 * </pre>
 *
 * <br>
 * The following example shows how the corresponding reset method would be used to restore the
 * values stored above:
 *
 * <pre>
 * <code>
 * public void reset(Checkpoint checkpoint) throws Exception {
 * 		// get an inputstream to read data from.
 * 		ObjectInputStream stream = checkpoint.getInputStream();
 *
 * 		// read an int (this would be 42 if the checkpoint method above is used)
 * 		int intValue = stream.readInt();
 *
 * 		// read the string as a UTF-8 value
 * 		String stringValue = stream.readUTF();
 *
 * 		//...
 * }
 * </code>
 * </pre>
 *
 * @since InfoSphere&reg; Streams Version 4.0
 */
public interface Checkpoint {

  /**
   * Sequence identifier of this checkpoint.
   *
   * @return identifier for the checkpoint
   */
  long getSequenceId();

  /**
   * Return the time (in millisecs) at which the checkpoint was created. Only valid at restore time.
   *
   * @return Timestamp as long value since Jan 1, 1970.
   * @throws IllegalStateException if the method is called at checkpoint time.
   * @throws IOException if there is an error retrieving the checkpoint timestamp.
   */
  long getTimestamp() throws IllegalStateException, IOException;

  /**
   * Provides an {@code ObjectInputStream} object that can be used to read data stored in the
   * checkpoint. The data should be read in the same order that it was put in during checkpointing.
   * Note that the runtime will automatically call the {@code close()} method on the stream.
   *
   * @return Input Stream for reading data out of the checkpoint.
   * @throws IllegalStateException if this method is called at checkpoint time.
   * @throws IOException if a input stream could not be created.
   */
  ObjectInputStream getInputStream() throws IllegalStateException, IOException;

  /**
   * Provides an {@code ObjectOutputStream} object that can be used to put data into the checkpoint.
   * Note that the runtime will automatically call the {@code flush()} and {@code close()} methods
   * on the stream.
   *
   * @return Output Stream for writing data to the checkpoint.
   * @throws IllegalStateException if this method is called at restore time.
   * @throws IOException if an output stream could not be created.
   */
  ObjectOutputStream getOutputStream() throws IllegalStateException, IOException;
}
