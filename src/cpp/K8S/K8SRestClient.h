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

#include <K8S/K8STypes.h>

K8S_NAMESPACE_BEGIN

class K8SRestClient
{
  public:
    enum HTTPCode
    {
        Ok = 200,
        NoContent = 204,
        BadRequest = 400,
        NotFound = 404
    };

    K8SRestClient(std::string const& ns,
                  std::string const& cls,
                  std::string const& jobName,
                  const uint64_t peId);

    K8SRestClient(std::string const& service, std::string const& ns, std::string const& cls);

    long del(std::string const& path);
    long del(std::string const& path, std::string const& content, std::string const& type);
    long get(std::string const& path, std::ostream& response);
    long patch(std::string const& path, std::string const& content, std::string const& type);
    long post(std::string const& path,
              std::string const& content,
              std::string const& type,
              std::ostream& response);
    long put(std::string const& path, std::string const& content, std::string const& type);

  private:
    static size_t onWrite(void* content, size_t size, size_t nmemb, void* userp);

    std::string m_svc;
};

K8S_NAMESPACE_END
