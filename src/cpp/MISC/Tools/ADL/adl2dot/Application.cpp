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

#include "Application.h"
#include "Dotty.h"
#include <SAM/v4200/adlModel.h>
#include <TRC/DistilleryDebug.h>
#include <fstream>
#include <boost/filesystem.hpp>

#define die(...)                                                                                   \
    {                                                                                              \
        SPCDBG(L_ERROR, __VA_ARGS__, "ADL2DOT");                                                   \
        exit(__LINE__);                                                                            \
    }

namespace {

class ErrorHandler : public xml_schema::error_handler
{
  public:
    bool handle(const std::basic_string<char>& id,
                unsigned long line,
                unsigned long column,
                severity sev,
                const std::basic_string<char>& message)
    {
        auto level = sev == severity::warning ? L_WARN : L_ERROR;
        SPCDBG(level, id << message, "ADL2DOT");
        return true;
    }
};
}

using namespace adl2dot;

int Application::run(const UTILS_NAMESPACE::arg_vector_t& args)
{
    ErrorHandler eh;
    /*
     * Locate the file.
     */
    boost::filesystem::path filePath(m_fileName);
    auto canonicalFilePath = boost::filesystem::canonical(filePath);
    if (!boost::filesystem::exists(canonicalFilePath)) {
        die("ADL file not found: " << canonicalFilePath);
    }
    /*
     * Locate the schema.
     */
    boost::filesystem::path schemaPath(m_schema);
    auto canonicalSchemaPath = boost::filesystem::canonical(schemaPath);
    if (!boost::filesystem::exists(canonicalSchemaPath)) {
        die("Schema file not found: " << canonicalSchemaPath);
    }
    /*
     * Load the ADL.
     */
    xml_schema::properties props;
    props.schema_location("http://www.ibm.com/xmlns/prod/streams/application/v4200",
                          canonicalSchemaPath.string());
    std::ifstream file(canonicalFilePath.string());
    auto adl = xmlns::prod::streams::application::v4200::applicationSet(file, eh, 0, props);
    /*
     * Check if the ADL is valid.
     */
    if (adl.get() == nullptr) {
        die("Cannot load file at: " << m_fileName);
    }
    /*
     * Visit.
     */
    Dotty p;
    p.visit(adl->splApplication().front(), std::cout);
    return 0;
}

void Application::getArguments(option_vector_t& options)
{
    option_t local_options[] = { { 'f', "file", ARG | REQUIRED, "", "ADL XML file",
                                   STR_OPT(Application::processFileOption) },
                                 { 's', "schema", ARG | REQUIRED, "", "ADL XSD schema",
                                   STR_OPT(Application::processSchemaOption) } };
    APPEND_OPTIONS(options, local_options);
}

std::ostream& Application::printDescription(std::ostream& o) const
{
    o << "Streams ADL DOT generator";
    return o;
}

void Application::processFileOption(const option_t* option, const char* value)
{
    m_fileName = std::string(value);
}

void Application::processSchemaOption(const option_t* option, const char* value)
{
    m_schema = std::string(value);
}
