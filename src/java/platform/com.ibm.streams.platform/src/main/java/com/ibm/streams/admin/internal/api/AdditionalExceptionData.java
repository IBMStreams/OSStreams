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

package com.ibm.streams.admin.internal.api;

import com.ibm.streams.platform.services.AdditionalExceptionDataType;
import com.ibm.streams.platform.services.MessageElementType;
import java.util.ArrayList;
import java.util.List;

public class AdditionalExceptionData {

  private AdditionalExceptionDataType additionalData_;

  public AdditionalExceptionData(AdditionalExceptionDataType additionalData) {
    additionalData_ = additionalData;
  }

  public AdditionalExceptionDataType getAdditionalExceptionData() {
    return additionalData_;
  }

  public void setAdditionalExceptionData(AdditionalExceptionDataType additionalData) {
    additionalData_ = additionalData;
  }

  public StreamsMessage getPrimaryInformation() {
    if (additionalData_ != null) {
      MessageElementType msg = additionalData_.getPrimaryInformation();
      if (msg != null) {
        return StreamsMessage.isStreamsMessageID(msg.getMessageCode(), msg.getSubstitutionText());
      }
    }
    return null;
  }

  public List<StreamsMessage> getSecondaryInformation() {
    if (additionalData_ != null) {
      List<MessageElementType> msgList = additionalData_.getSecondaryInformation();
      if (msgList != null) {
        ArrayList<StreamsMessage> secondaryList = new ArrayList<StreamsMessage>();
        for (int i = 0; i < msgList.size(); i++) {
          MessageElementType msg = msgList.get(i);
          secondaryList.add(
              StreamsMessage.isStreamsMessageID(msg.getMessageCode(), msg.getSubstitutionText()));
        }
        return secondaryList;
      } else {
        return null;
      }
    } else {
      return null;
    }
  }

  public void addAdditionalNestedData(AdditionalExceptionData aed) {
    additionalData_.getNestedAdditionalExceptionData().add(aed.getAdditionalExceptionData());
  }

  public void addAdditionalNestedDatas(List<AdditionalExceptionData> aeds) {
    for (AdditionalExceptionData aed : aeds) {
      addAdditionalNestedData(aed);
    }
  }

  public List<AdditionalExceptionData> getAdditionalNestedData() {
    if (additionalData_ != null) {
      List<AdditionalExceptionDataType> dataList =
          additionalData_.getNestedAdditionalExceptionData();
      if (dataList != null) {
        ArrayList<AdditionalExceptionData> exceptionDataList =
            new ArrayList<AdditionalExceptionData>();
        for (int i = 0; i < dataList.size(); i++) {
          AdditionalExceptionDataType exceptionData = dataList.get(i);

          exceptionDataList.add(new AdditionalExceptionData(exceptionData));
        }
        return exceptionDataList;
      } else {
        return null;
      }
    } else {
      return null;
    }
  }
}
