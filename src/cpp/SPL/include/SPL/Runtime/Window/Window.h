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

#ifndef SPL_RUNTIME_WINDOW_WINDOW_H
#define SPL_RUNTIME_WINDOW_WINDOW_H

/**
 * @defgroup Window Window
 *
 * @brief Classes which provide windowing functionality.
 *
 * The C++ windowing library provides consistent window policy semantics
 * across operators and simplifies primitive operator implementation
 * when using windows.
 *
 * The library offers tumbling, sliding, and event-time windows. A window
 * keeps all the incoming data in memory until its tuple eviction policy
 * triggers. The window fires events when significant changes occur in the
 * object's state. Developers can implement the event handling actions
 * independently of the window policy details.
 */

/**
 * @file Window.h @brief Master include file for window related classes.
 */

#include <SPL/Runtime/Window/WindowEvent.h>
#include <SPL/Runtime/Window/PartitionEviction.h>
#include <SPL/Runtime/Window/SlidingWindow.h>
#include <SPL/Runtime/Window/TumblingWindow.h>
#include <SPL/Runtime/Window/TimeIntervalWindow.h>
#include <SPL/Runtime/Window/TimeIntervalWindowPane.h>

#endif /* SPL_RUNTIME_WINDOW_WINDOW_H */
