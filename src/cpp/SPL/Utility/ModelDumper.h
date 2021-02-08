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

#ifndef SPL_MODELDUMPER_H
#define SPL_MODELDUMPER_H

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Runtime/Utility/Singleton.h>

#include <fstream>
#include <ostream>
#include <sstream>
#include <string>

#define MODEL(text)                                                                                \
    do {                                                                                           \
        if (CompilerConfiguration::instance().isSet(CompilerConfiguration::ModelFile)) {           \
            std::stringstream ss;                                                                  \
            ss << text;                                                                            \
            ModelDumper& md = ModelDumper::instance();                                             \
            md << ss.str();                                                                        \
        }                                                                                          \
    } while (0)

namespace SPL {

class ModelDumper : public Singleton<ModelDumper>
{
  public:
    ModelDumper(const std::string& file)
      : Singleton<ModelDumper>(*this)
      , _os(_fb.open(file.c_str(), std::ios::out))
    {}
    ~ModelDumper() { _fb.close(); }
    std::ostream& operator<<(const std::string& s)
    {
        _os << s;
        _os.flush();
        return _os;
    }

  private:
    std::filebuf _fb;
    std::ostream _os;
};

} // namespace SPL

#endif // SPL_MODELDUMPER_H
