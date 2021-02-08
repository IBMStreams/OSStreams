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

#ifndef SPL_RUNTIME_WINDOW_WINDOW_TRACKER_H
#define SPL_RUNTIME_WINDOW_WINDOW_TRACKER_H

#include <stack>

#include <SPL/Runtime/Window/WindowCommon.h>
#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

namespace SPL
{
/// @ingroup Window
/// @brief Class for tracking which window is active at any time.
class DLL_PUBLIC WindowTracker
{
public:
  /// @brief Return which window the current thread is executing
  /// @return operator index number
  static BaseWindow * getCurrentWindow() {
    return (!currentWindowStack_ || currentWindowStack_->empty())
        ? NULL : currentWindowStack_->top();
  }

private:
  friend class AutoSetWindow;

  /// Record the window the current thread is executing
  static void pushCurrentWindow(BaseWindow * win)
  { initIfNeeded(); currentWindowStack_->push(win); }

  /// Record that the current thread has finished executing
  /// the current window.
  static void popCurrentWindow()
  { assert(currentWindowStack_); currentWindowStack_->pop(); }

  static void initIfNeeded() {
    if (!currentWindowStack_) {
      currentWindowStack_ = new std::stack<BaseWindow *>;
    }
  }
  WindowTracker() {} // disallow instantiation
  static __thread std::stack<BaseWindow *>* currentWindowStack_;
};

/**
 * @ingroup Window
 * @brief Class that provides an RAII handler for a code block.
 *
 * Track the current window by declaring a local %AutoSetWindow variable:
 *
@verbatim
{
  AutoSetWindow a(&myWindow); // start tracking my window

  // Code that manipulates the window
  ...

} // stop tracking the window
@endverbatim
     */
class DLL_PUBLIC AutoSetWindow {
public:
  AutoSetWindow(BaseWindow* currentWindow) {
    WindowTracker::pushCurrentWindow(currentWindow);
  }
  ~AutoSetWindow() {
      WindowTracker::popCurrentWindow();
  }
};
};

#endif /* SPL_RUNTIME_WINDOW_WINDOW_TRACKER_H */
