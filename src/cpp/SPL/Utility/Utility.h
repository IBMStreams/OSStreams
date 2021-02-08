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

#ifndef SPL_UTILITY_H
#define SPL_UTILITY_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/HashMapHelpers.h>
#include <boost/filesystem/path.hpp>
#include <string>
#include <typeinfo>

#define SPL_USER_ERROR 10

namespace SPL {
class RootTyp;
class Utility
{
  public:
    /// Expand environment variables embeded between @'s in the text
    /// @param text source text to be used for expanding environment variables
    /// @return the expanded text
    static std::string expandEnvironmentVariables(const std::string& text);

    /// Generate a random partition of the set (0,...,n-1)
    ///
    ///  Reference:
    ///    Albert Nijenhuis, Herbert Wilf,
    ///    Combinatorial Algorithms,
    ///    Academic Press, 1978, second edition,
    ///    ISBN 0-12-519260-6.
    ///
    /// pcnt  - partition count (partitions are 0,...,pcnt-1)
    /// assgn - partition assignment of each element
    static void randomPartitioning(const uint32_t n, uint32_t& pcnt, uint32_t assgn[]);

    /// Shorten a long text by putting ... before 1/4ths of it from the end,
    /// if longer than a given bound
    /// @param txt source text
    /// @param n maximum length of the result
    /// @return the shortened string
    static std::string shorten(std::string const& txt, size_t const n);

    /// Write a file if the contents are different (or file not there)
    /// @param file File to read/write
    /// @param value file contents
    static void writeIfNecessary(const boost::filesystem::path& file, const std::string& value);

    /// Replace double colons in a string with a dot
    /// @param str string on which double colons will be replaced
    /// @return string after replacement
    static std::string replaceDoubleColonsWithDot(std::string const& str);

    /// Change 'opath' into a relative path, rooted at 'root', if 'opath' is
    /// contained under 'root'
    /// Store the number of levels remaining in the resulting relative path in 'nup'
    /// @param opath Output path
    /// @param root  Root path
    /// @param nup   Levels to go up from 'opath' to 'root'
    /// @return 'true' if opath was contained in 'root'
    static bool makeRelativePath(boost::filesystem::path& opath,
                                 boost::filesystem::path const& root,
                                 int& nup);

    /// Normlize the given path.  Removes embedded ./ and ../. Also removes leading and trailing ./.
    /// @param path
    /// @return Returns a normalized path
    static boost::filesystem::path normalize(const boost::filesystem::path& path);

    /// Find the number of levels one needs to go up from output directory
    /// to application directory
    /// @return -1 if the output directory is not at or under the
    /// application directory, number of levels to go up otherwise
    static int findNumLevelsUpFromOutputDirToAppDir();

    /// Take a path relative to the application directory and make it
    /// relative to the output directory
    /// @param path path relative to the application directory
    static void makeAppDirRelativePathOutputDirRelative(boost::filesystem::path& path);

    /// Remove a file
    /// @param file file to remove
    /// @param dieOnError if true, die on error with a message, else ignore errors
    static void removeFile(const boost::filesystem::path& file, bool dieOnError = true);

    /// Remove a directory
    /// @param dir dir to remove
    /// @param evenIfNonEmpty if true, remove non-empty a dir, else ignore it
    /// @param dieOnError if true, die on error with a message, else ignore errors
    static void removeDirectory(const boost::filesystem::path& dir,
                                bool evenIfNonEmpty,
                                bool dieOnError = true);

    /// Clean up a pathname by removing . and .. components
    /// @param path path to clean
    /// @return cleaned path
    static boost::filesystem::path cleanPath(const boost::filesystem::path& path);

    /// Create a directory, constructing intermediate directories if needed
    /// @param dir dir to create
    /// @param dieOnError if true, die on error with a message, else ignore errors
    static void createDirectory(const boost::filesystem::path& dir, bool dieOnError = true);

    /// ensure an id doesn't clash with C++ keyword or macro
    /// @param id variable to remap
    /// @return 'id' if okay, or remapped one if not
    static const std::string& remapId(const std::string& id);

    /// Is the id a reserved keyword?
    /// @param id variable to remap
    /// @return 'true' if this is a C++ keyword
    static bool reservedName(const std::string& id);

    /// Is id a reserved namespace?   This includes C++ keywords and certain directory
    /// names not checked by spl-make-toolkit (and 'streams')
    /// @param id name to check
    /// @return 'true' if id is a problem.
    static bool reservedNamespace(const std::string& id);

    /// Is id a reserved directory?   These are directories not checked by spl-make-toolkit
    /// @param id name to check
    /// @return 'true' if id is a problem.
    static bool reservedDirectory(const std::string& id);

    /// Is the operator param a reserved one for Java primitive operators?
    /// @param name parameter name
    /// @return 'true' if reserved, 'false' otherwise
    static bool isReservedJavaOperatorParam(std::string const& name);

    /// Is the type a valid SPL type?
    /// @param type string to be checked
    /// @return 'true' if an explicit SPL type
    static bool validSPLType(const std::string& type);

    /// Convert a string to an SPL Basetype if possible
    /// @param type string to be checked
    /// @return NULL if not an explicit SPL type
    static const RootTyp* splType(const std::string& type);

    /// Class to pull information from an @spldoc comment in an easy to use way
    /// Pass it the comment, and let it get pulled apart
    class DLL_PUBLIC splDoc
    {
      public:
        /// Constructor
        /// Most times, we don't know if there is a commment, so we use this
        splDoc() {}

        /// Construct and parse the comment
        /// @param comment comment to be picked apart
        splDoc(const std::string& comment) { parse(comment); }

        /// Parse the @spldoc comment
        /// @param comment comment to be picked apart
        void parse(const std::string& comment);

        /// Return the short description
        /// @return description that follows /** in the comment
        const std::string& shortDescription() const { return _shortDescription; }

        /// Return the long description
        /// @return information following the short comment
        const std::string& longDescription() const { return _longDescription; }

        /// Search for a key
        /// @param key A key such as 'input X'
        /// @return ptr to description if found, or NULL
        const std::string* find(const std::string& key) const;

      private:
        std::tr1::unordered_map<std::string, std::string> _map;
        std::string _shortDescription;
        std::string _longDescription;
    };

    /// Encode a filename with 'percent encoding' for a URI'
    /// @param filename input filename
    /// @return filename, properly percent coded
    static std::string uriEncode(const std::string& filename);

    /// Decode a 'percent encoded' URI
    /// @param input 'percent encoded' URI
    /// @return decoded URI
    static std::string uriDecode(const std::string& input);

    /// Convert an Ant style pattern to a regEx
    /// @param pattern The pattern to convert
    /// @return Returns the converted pattern
    static std::string convertAntPatternToRegEx(const std::string& pattern);
};
};

#endif /*SPL_UTILITY_H*/
