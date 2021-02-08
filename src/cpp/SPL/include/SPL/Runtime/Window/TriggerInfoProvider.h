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

#ifndef SPL_RUNTIME_WINDOW_TRIGGER_INFO_PROVIDER_H
#define SPL_RUNTIME_WINDOW_TRIGGER_INFO_PROVIDER_H

/// @file TriggerInfoProvider.h
/// @brief Definition of the SPL::TriggerInfoProvider class.

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

PUSH_DLL_PUBLIC
namespace SPL
{
/**
 * @ingroup EventTime Window
 * @brief Interface which provides information about an event-time window,
 * observable within the WindowEvent handler code.
 *
 * Windows defined over the event time domain implement this interface.
 *
 * @since IBM Streams Version 4.3.0
 */
class TriggerInfoProvider
{
public:
  // If enum PaneTiming changes, corresponding changes must be made
  // to WindowTimeEnum in SPLFunctions.cpp.
  /**
   * @brief Enumerates the timing possibilities for the trigger firing
   * related to the current watermark of the enclosing operator.
   *
   * A window may trigger multiple times, and the timing of those triggers
   * is considered:
   *
   * - @c paneEarly if the window pane is not complete when it triggers.
   * - @c paneOnComplete if the window pane has just completed and, as a
   *   result, the pane has triggered.
   * - @c paneLate if the window pane has triggered because late tuples being
   *   inserted.
   */
  enum PaneTiming {
    paneEarly,      ///< window pane is not complete
    paneOnComplete, ///< window pane has just completed
    paneLate        ///< window pane contains late data
  };

  /**
   * @brief Returns the start of the time interval covered by this window.
   *
   * All window tuples have an event time greater or equal to the start time.
   * @return the start time
   */
  virtual timestamp getStartTime() const = 0;

  /**
   * @brief Returns the end of the time interval covered by this window.
   *
   * All window tuples have an event time less than the end time.
   * @return the end time
   */
  virtual timestamp getEndTime() const = 0;

  /**
   * @brief Returns the timing of the associated window trigger.
   * @return the window trigger timing
   */
  virtual PaneTiming getPaneTiming() const = 0;

  /**
   * @brief Returns the zero-based index of the trigger firing for this window.
   * @return the trigger firing index
   */
  virtual uint64 getPaneIndex() const = 0;

protected:
#ifndef DOXYGEN_SKIP_FOR_USERS
  virtual ~TriggerInfoProvider() {}
#endif
};
} // namespace SPL
POP_DLL_PUBLIC

#endif //SPL_RUNTIME_WINDOW_TRIGGER_INFO_PROVIDER_H
