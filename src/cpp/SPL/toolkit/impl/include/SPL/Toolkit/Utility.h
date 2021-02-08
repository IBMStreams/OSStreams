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

#ifndef SPL_RUNTIME_TOOLKIT_UTILITY_H
#define SPL_RUNTIME_TOOLKIT_UTILITY_H

#include <boost/filesystem/path.hpp>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Type/String.h>
#include <string>

namespace SPL {
    class ProcessingElement;

    PUSH_DLL_PUBLIC

    class ProcessingElement;

    /// Support Atomic renames/moves, even across filesystems
    class AtomicRenamer {
        public:
            /// Atomic rename support from toDir.  If fromDir and toDir are on different
            /// filesystems, a '.rename' directory will be created under toDir.
            /// @param pe ProcessingElement
            /// @param toDir pathname to target directory.  If this is a relative path, it will be converted to an
            /// absolute path relative to the data directory
            /// @throw Boost filesystem errors
            AtomicRenamer (ProcessingElement& pe, const boost::filesystem::path& toDir);

            /// Atomic rename support from toDir.  If fromDir and toDir are on different
            /// filesystems, a '.rename' directory will be created under toDir.
            /// @param pe ProcessingElement
            /// @param toDir pathname to target directory (rstring). If this is a relative path, it will be converted to an
            /// absolute path relative to the data directory
            /// @throw Boost filesystem errors
            AtomicRenamer (ProcessingElement& pe, const rstring &toDir);

            /// Move a file to toDir.  Any existing file of the same name in toDir will
            /// be replaced. If the file  and toDir are on different
            /// filesystems, a '.rename' directory will be created under toDir.
            /// @param file file to be moved atomically to toDir
            /// @return pathname to fileName in target directory
            /// @throw Boost filesystem errors
            /// @throw SPLRuntimeOperatorException if a same filesystem rename fails
            const std::string rename (const boost::filesystem::path& file);

            /// Return the path to the destination directory
            /// @return destination directory
            const boost::filesystem::path& toDir() const { return _toDir;}

        private:
            boost::filesystem::path _toDir;
            ProcessingElement& _pe;
    };

    /// Put the socket temporarily into non-blocking mode, and see if EOF has
    /// been received.
    /// @param fs socket descriptor
    /// @return true if EOF has been received
    bool isEOFReceived(int fd);

    /// Clean up a pathname by removing . and .. components
    /// @param path path to clean
    /// @return cleaned path
    boost::filesystem::path cleanPath(const boost::filesystem::path & path);

    /// Return the numeric hostname associated with an interface, or the default address if empty
    /// @param interface Interface name or empty string
    /// @return the associated hostname in numeric form
    std::string getHostName(const std::string& interface);

    POP_DLL_PUBLIC
};

#endif /*SPL_RUNTIME_TOOLKIT_UTILITY_H*/
