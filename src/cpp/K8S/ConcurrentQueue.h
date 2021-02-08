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

#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

template<typename T>
class ConcurrentQueue
{
  public:
    ConcurrentQueue() = default;
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

    bool pop(T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        if (queue_.empty()) {
            cond_.wait(mlock);
            if (queue_.empty()) {
                return false;
            }
        }
        item = queue_.front();
        queue_.pop();
        return true;
    }

    template<class Rep, class Period>
    bool pop(T& item, std::chrono::duration<Rep, Period> const& delay)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        if (queue_.empty()) {
            if (cond_.wait_for(mlock, delay) == std::cv_status::timeout || queue_.empty()) {
                return false;
            }
        }
        item = queue_.front();
        queue_.pop();
        return true;
    }

    void push(const T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(item);
        mlock.unlock();
        cond_.notify_one();
    }

    void shutdown() { cond_.notify_one(); }

  private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};
