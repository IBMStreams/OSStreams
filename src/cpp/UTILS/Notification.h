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

#ifndef UTILS_NOTIFICATION_H_
#define UTILS_NOTIFICATION_H_

#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

/**
 * Should be implemented by an object that wants to receive notifications.
 */
template<typename T>
class NotificationListener
{
  public:
    /**
     * Invoked when a notification occurs.  The implementation of this method
     * should return as soon as possible, to avoid blocking its
     * notification emitter.
     * @param notification  the notification.
     * @param handback  an opaque pointer which helps the listener associate
     *   information regarding the event emitter. This pointer is passed to
     *   the emitter when this listener is added and resent without
     *   modification to the listener.
     */
    virtual void handleNotification(const T& notification, void* handback) = 0;

    virtual std::string getNotificationListenerKey() { return ""; }

  protected:
    virtual ~NotificationListener() {}
};

/**
 * Should be implemented by an object that needs to emit notifications to
 * listeners.
 */
template<typename T>
class NotificationEmitter
{
  public:
    /**
     * Adds a notification listener to an emitter.  The emitter will notify all
     * the listeners on issued notifications.
     *
     * @param listener  The listener which will handle notifications issued
     * by this emitter.
     * @param handback  An opaque pointer which helps the listener associate
     *   information regarding the event emitter. This pointer is passed to
     *   the emitter when this listener is added and resent without
     *   modification to the listener.
     */
    virtual void addNotificationListener(NotificationListener<T>& listener, void* handback) = 0;

    virtual void removeNotificationListener(std::string key){};

  protected:
    virtual ~NotificationEmitter() {}
};

class Notification
{
  public:
    Notification(const std::string& category, std::string& type)
      : _category(category)
      , _type(type)
      , _source(NULL)
      , _userData(NULL)
    {}

    Notification(const std::string& category, const std::string& type, void* source)
      : _category(category)
      , _type(type)
      , _source(source)
      , _userData(NULL)
    {}

    Notification(const std::string& category, const std::string& type, void* source, void* userData)
      : _category(category)
      , _type(type)
      , _source(source)
      , _userData(userData)
    {}

    virtual ~Notification() {}

    inline std::string getCategory() const { return _category; }

    inline void setCategory(const std::string& category) { _category = category; }

    inline std::string getType() const { return _type; }

    inline void setType(const std::string& type) { _type = type; }

    inline void* getSource() const { return _source; }

    inline void setSource(void* source) { _source = source; }

    inline void* getUserData() const { return _userData; }

    inline void setUserData(void* userData) { _userData = userData; }

  private:
    std::string _category;
    std::string _type;
    void* _source;
    void* _userData;
};

UTILS_NAMESPACE_END

#endif /* !UTILS_NOTIFICATION_H_ */
