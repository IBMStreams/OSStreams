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

package com.ibm.streams.instance.sam.model.topology;

import static com.google.common.base.Preconditions.checkNotNull;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNull;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkNotNullAndValidate;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class DeploymentDescriptor implements Validator {

  private final SharedObject sharedObject;
  private Map<String, String> envVars;
  private Boolean optimized;
  private Language language;
  // Not sure how this plays into the AADL PE we need to generate
  private final List<RuntimeConstant> runtimeConstants;
  private Checkpointing checkpointing;
  private Profiling profile;
  private Boolean singleThreadedOnInputs;
  private Boolean singleThreadedOnOutputs;
  private final BigInteger toolkitIndex;
  private List<String> jvmArguments; // null unless the operator needs a jvm.
  // May be an empty list if it needs no
  // args

  public DeploymentDescriptor(
      SharedObject sharedObject,
      List<RuntimeConstant> runtimeConstants,
      BigInteger toolkitIndex,
      boolean optimized,
      boolean singleThreadedOnInputs,
      boolean singleThreadedOnOutputs,
      Checkpointing checkpointing) {
    this.sharedObject = sharedObject;
    this.runtimeConstants = runtimeConstants;
    this.toolkitIndex = toolkitIndex;
    this.optimized = optimized;
    this.singleThreadedOnInputs = singleThreadedOnInputs;
    this.singleThreadedOnOutputs = singleThreadedOnOutputs;
    this.checkpointing = checkpointing;
    envVars = new HashMap<>();
  }

  public BigInteger getToolkitIndex() {
    return toolkitIndex;
  }

  public SharedObject getSharedObject() {
    return sharedObject;
  }

  public Map<String, String> getEnvVars() {
    return envVars;
  }

  public void setEnvVars(Map<String, String> envVars) {
    this.envVars = envVars;
  }

  public boolean isOptimized() {
    return optimized.booleanValue();
  }

  public void setOptimized(boolean optimized) {
    this.optimized = Boolean.valueOf(optimized);
  }

  public Language getLanguage() {
    return language;
  }

  public void setLanguage(Language language) {
    this.language = language;
  }

  public Checkpointing getCheckpoint() {
    return checkpointing;
  }

  public void setCheckpoint(Checkpointing checkpoint) {
    this.checkpointing = checkpoint;
  }

  public Profiling getProfile() {
    return profile;
  }

  public void setProfile(Profiling profile) {
    this.profile = profile;
  }

  public boolean isSingleThreadedOnInputs() {
    return singleThreadedOnInputs.booleanValue();
  }

  public void setSingleThreadedOnInputs(boolean singleThreadedOnInputs) {
    this.singleThreadedOnInputs = Boolean.valueOf(singleThreadedOnInputs);
  }

  public boolean isSingleThreadedOnOutputs() {
    return singleThreadedOnOutputs.booleanValue();
  }

  public void setSingleThreadedOnOutputs(boolean singleThreadedOnOutputs) {
    this.singleThreadedOnOutputs = Boolean.valueOf(singleThreadedOnOutputs);
  }

  public List<RuntimeConstant> getRuntimeConstants() {
    return runtimeConstants;
  }

  public List<String> getJvmArguments() {
    return jvmArguments;
  }

  public void setJvmArguments(List<String> jvmArguments) {
    this.jvmArguments = jvmArguments;
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<deploymentDescriptor toolkitIndex=\""
            + toolkitIndex
            + "\" optimized=\""
            + optimized
            + "\" singleThreadedOnInputs=\""
            + singleThreadedOnInputs
            + "\" singleThreadedOnOutputs=\""
            + singleThreadedOnOutputs
            + "\">");
    dumper.indent();
    dumper.append(sharedObject.toString());
    if (checkpointing != null) {
      dumper.append(checkpointing.toString());
    }
    if (runtimeConstants != null && runtimeConstants.size() > 0) {
      dumper.line("<runtimeConstants>");
      dumper.indent();
      dumper.append(runtimeConstants);
      dumper.outdent();
      dumper.line("</runtimeConstants>");
    }
    if (jvmArguments != null) {
      dumper.line("<jvmArguments>");
      dumper.indent();
      for (String s : jvmArguments) {
        dumper.line("<jvmArgument>" + s + "</jvmArgument>");
      }
      dumper.outdent();
      dumper.line("</jvmArguments>");
    }
    dumper.outdent();
    dumper.line("</deploymentDescriptor>");
    return dumper.toString();
  }

  public void validate() throws Exception {
    checkNotNullAndValidate(sharedObject);
    checkContentsNotNull(envVars.keySet());
    checkContentsNotNull(envVars.values());
    checkNotNull(optimized);
    // checkNotNull(language); // Enums are either null or valid
    // checkNotNullAndValidate(runtimeConstant);
    // checkNotNullAndValidate(checkpointing);
    // checkNotNullAndValidate(profile);
    checkNotNull(singleThreadedOnInputs);
    checkNotNull(singleThreadedOnOutputs);
  }
}
