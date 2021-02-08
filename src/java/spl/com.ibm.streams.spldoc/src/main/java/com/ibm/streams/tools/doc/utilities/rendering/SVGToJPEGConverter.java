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

package com.ibm.streams.tools.doc.utilities.rendering;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import org.apache.batik.transcoder.TranscoderException;
import org.apache.batik.transcoder.TranscoderInput;
import org.apache.batik.transcoder.TranscoderOutput;
import org.apache.batik.transcoder.image.JPEGTranscoder;

public class SVGToJPEGConverter {

  public void convert(File svgFile, File jpegFile, int width) throws IOException {
    JPEGTranscoder t = new JPEGTranscoder();
    t.addTranscodingHint(JPEGTranscoder.KEY_WIDTH, new Float(width));
    TranscoderInput input = new TranscoderInput(svgFile.toURI().toURL().toString());

    FileOutputStream fileOstrm = null;
    BufferedOutputStream bufferedOstrm = null;
    try {
      fileOstrm = new FileOutputStream(jpegFile);
      bufferedOstrm = new BufferedOutputStream(fileOstrm);
      TranscoderOutput output = new TranscoderOutput(bufferedOstrm);
      t.transcode(input, output);
    } catch (IOException e) {
      throw e;
    } catch (TranscoderException e) {
      e.printStackTrace();
      throw new IOException("Cannot transcode image. Details:" + e.getMessage());
    } finally {
      bufferedOstrm.close();
      fileOstrm.close();
    }
  }

  public void convert(File svgFile, File jpegFile, int width, int height) throws IOException {
    JPEGTranscoder t = new JPEGTranscoder();
    t.addTranscodingHint(JPEGTranscoder.KEY_WIDTH, new Float(width));
    t.addTranscodingHint(JPEGTranscoder.KEY_HEIGHT, new Float(height));
    t.addTranscodingHint(JPEGTranscoder.KEY_QUALITY, new Float(1.0));
    TranscoderInput input = new TranscoderInput(svgFile.toURI().toURL().toString());

    FileOutputStream fileOstrm = null;
    BufferedOutputStream bufferedOstrm = null;
    try {
      fileOstrm = new FileOutputStream(jpegFile);
      bufferedOstrm = new BufferedOutputStream(fileOstrm);
      TranscoderOutput output = new TranscoderOutput(bufferedOstrm);
      t.transcode(input, output);
    } catch (IOException e) {
      throw e;
    } catch (TranscoderException e) {
      e.printStackTrace();
      throw new IOException("Cannot transcode image. Details:" + e.getMessage());
    } finally {
      bufferedOstrm.close();
      fileOstrm.close();
    }
  }
}
