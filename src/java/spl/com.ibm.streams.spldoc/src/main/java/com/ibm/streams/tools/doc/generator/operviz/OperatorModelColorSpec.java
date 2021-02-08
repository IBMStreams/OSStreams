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

package com.ibm.streams.tools.doc.generator.operviz;

import java.awt.Color;

public class OperatorModelColorSpec {
  private Color operFillColor_ = new Color(255, 255, 150); // light yellow
  private Color optionalPortColor_ = Color.gray;
  private Color mandatoryPortColor_ = Color.black;
  private Color optionalWindowColor_ = Color.gray;
  private Color mandatoryWindowColor_ = Color.black;

  public Color getOperatorFillColor() {
    return operFillColor_;
  }

  public void setOptionalPortColor(Color optionalPortColor) {
    optionalPortColor_ = optionalPortColor;
  }

  public Color getOptionalPortColor() {
    return optionalPortColor_;
  }

  public void setMandatoryPortColor(Color mandatoryPortColor) {
    mandatoryPortColor_ = mandatoryPortColor;
  }

  public Color getMandatoryPortColor() {
    return mandatoryPortColor_;
  }

  public void setOptionalWindowColor(Color optionalWindowColor) {
    optionalWindowColor_ = optionalWindowColor;
  }

  public Color getOptionalWindowColor() {
    return optionalWindowColor_;
  }

  public void setMandatoryWindowColor(Color mandatoryWindowColor) {
    mandatoryWindowColor_ = mandatoryWindowColor;
  }

  public Color getMandatoryWindowColor() {
    return mandatoryWindowColor_;
  }
}
