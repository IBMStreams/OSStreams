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

package com.ibm.streams.instance.sam.model.logical;

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.applicationModel.ApplicationModelFactory;
import com.ibm.streams.instance.sam.model.logical.spl.SPLLogicalModel;
import com.ibm.streams.instance.sam.model.logical.spl.v4000.V4000SPLLogicalModel;
import com.ibm.streams.instance.sam.model.logical.spl.v4200.V4200SPLLogicalModel;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;
import javax.xml.bind.JAXBException;
import org.apache.commons.io.IOUtils;

public class LogicalModelFactory {

  // TODO: Improve the error handling here
  public static LogicalModel createLogicalModel(
      String adlString, Properties submissionParms, Properties configParms)
      throws StreamsException {
    SPLLogicalModel lm = null;

    try {
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Trying v4200 adl");
      }
      com.ibm.streams.platform.services.v4200.ApplicationType adlModel =
          ApplicationModelFactory.createV4200ApplicationModel(getInputStream(adlString));
      lm = new V4200SPLLogicalModel(adlModel, submissionParms, configParms);
    } catch (StreamsException e) {
      Trace.logError(e.getMessage(), e);
      throw e;
    } catch (JAXBException e) {
      // e.printStackTrace();
      // Loading the v4200 adl failed...Try v4000
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("v4200 adl failed", e);
      }
      try {
        if (Trace.isEnabled(Level.TRACE)) {
          Trace.logTrace("Trying v4000 adl");
        }
        com.ibm.streams.platform.services.v4000.ApplicationType adlModel =
            ApplicationModelFactory.createV4000ApplicationModel(getInputStream(adlString));
        lm = new V4000SPLLogicalModel(adlModel, submissionParms, configParms);
        if (Trace.isEnabled(Level.TRACE)) {
          Trace.logTrace("v4000 adl succeeded");
        }
        return lm;
      } catch (StreamsException e1) {
        Trace.logError(e.getMessage(), e1);
        throw e1;
      } catch (JAXBException e1) {
        if (Trace.isEnabled(Level.TRACE)) {
          Trace.logTrace("v4000 adl failed", e1);
        }
        // TODO: Throw an "unsupported ADL error"
        throw new StreamsException(e1);
      }
    }
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("v4200 adl succeeded");
    }
    return lm;
  }

  private static InputStream getInputStream(String adlString) throws StreamsException {
    InputStream iStream = null;
    try {
      iStream = IOUtils.toInputStream(adlString, "UTF-8");
      return iStream;
    } catch (IOException e) {
      throw new StreamsException(e);
    }
  }
}
