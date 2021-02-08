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

/*
 * Interface for implementing parsing of XML
 */
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <string>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {
    class XMLParseImpl;
    class DLL_PUBLIC XMLParse {
        public:
            /// Constructor
            /// @param isUTF16 true if input is already UTF-16
            /// @param ignoreNamespaces true if only local names should be used
            XMLParse(bool isUTF16, bool ignoreNamespaces);

            /// Destructor
            virtual ~XMLParse();

            /// Parse XML, invocking callbacks
            void parse(const char *data, size_t dataLen);

            /// Notify the XML Parser that we are shutting down
            void shutdown();

            /// Are we shutting down?
            /// @return true if we have shutdown() has been called
            bool shuttingDown() { return _shutdown;}

            /// Reset the XML Parser to the 'top level' state
            void resetParser();

            /// Get the current number of bytes that were not used during
            /// the last parse
            uint64_t unusedDataLeft() const;

            /* Callback interface to XML Parser */

            /// Start a new XML element
            /// @param name element name
            /// @param attrs list of attribute name, attribute value pairs, terminated by NULL
            virtual void doStartElement(const char *name, const char** attrs) = 0;

            /// Characters in an XML element value
            /// @param chars character data
            /// @param len length of character data
            virtual void doCharacters(const char *data, int len) = 0;

            /// End an XML element
            /// @param name element name
            virtual void doEndElement(const char *name) = 0;

            /// Document finished parsing
            virtual void finishedParse() = 0;

            // Handle a warning from the XML parser
            virtual void warning(const char *msg) = 0;

            // Handle an error from the XML parser
            virtual void error(const char *msg) = 0;

            void initState() {
                resetState();
                _state.push_back(0);
            }
            void resetState() {
                _state.clear();
                _depth.clear();
                _charbuf.clear();
            }
            /// Constructor
            /// @param isUTF16 true if input is already UTF-16
            /// @param ignoreNamespaces true if only local names should be used
            /// @param xmlParseHuge true if libxml2 arbitrary buffer limits should not be imposed
            XMLParse(bool isUTF16, bool ignoreNamespaces, bool xmlParseHuge);
        protected:
            bool _shutdown;
            std::vector<int32_t>     _state;
            std::vector<int32_t>     _depth;
            std::vector<std::string> _charbuf;
        private:
            XMLParseImpl& _impl;
    };
};
