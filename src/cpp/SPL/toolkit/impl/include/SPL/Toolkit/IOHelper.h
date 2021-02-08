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

#ifndef SPL_RUNTIME_IO_HELPER_H
#define SPL_RUNTIME_IO_HELPER_H

#include <iostream>
#include <locale>
#include <string>

namespace SPL
{
    /// Skip until we hit the newline (or return or EOF)
    inline std::istream & skipToEndOfLine(std::istream & str)
    {
        for(;;) {
            int c = str.peek();
            if (c == EOF || c == '\r' || c == '\n')
                break;
            (void) str.get();
        }
        return str;
    }

    /// Skip spaces and tabs in an input stream
    inline std::istream & skipSpaceTabs(std::istream & str)
    {
        for(;;) {
            int c = str.peek();
            if (c == '\t' || c == ' ')
                (void) str.get();
            else
                break;
        }
        return str;
    }

    /// Skip spaces, tabs, carriage returns, and new lines
    inline std::istream & skipSpaceTabReturnNewLines(std::istream & str)
    {
        for(;;) {
            int c = str.peek();
            if (c == '\t' || c == ' ' || c == '\r' || c == '\n')
                (void) str.get();
            else
                break;
        }
        return str;
    }

    inline static int safePeek (std::istream & fs)
    {
        if (fs.eof())
            return EOF;
        return fs.peek();
    }

    // Helper class for extracting tokens from a CSV stream.
    // Enum classes have specialization in their generated
    // code.
    template<class T, unsigned char separator>
    class CSVExtractor
    {
    public:
        // Extract one token of type T from stream fs, which contains
        // csv-formatted data separated by separator.
        static inline void extract(std::istream & fs, T& field) {
            SPL::deserializeWithNanAndInfs(fs, field);
        }
    };

    template<class T, unsigned char separator>
    static inline void readCSV(std::istream & fs, T& field)
    {
        SPL::skipSpaceTabs(fs);
        int c = SPL::safePeek(fs);
        if (c == separator || c == '\r' || c == '\n' || c == EOF) {
            // Empty field.  Use default value
            field = T();
            return;
        }
        CSVExtractor<T,separator>::extract(fs, field);
    }

    // This function is almost an exact copy of the above one. This is because
    // we want to avoid having defaultValue=T() as a default argument, in case T
    // is a type that is costly to construct.
    template<class T, unsigned char separator>
    static inline void readCSV(std::istream & fs, T& field,
                               T const & defaultValue)
    {
        SPL::skipSpaceTabs(fs);
        int c = SPL::safePeek(fs);
        if (c == separator || c == '\r' || c == '\n' || c == EOF) {
            // Empty field.  Use default value
            field = defaultValue;
            return;
        }
        CSVExtractor<T,separator>::extract(fs, field);
    }

    // Read a rstring in quotes or without quotes but up until next
    // comma or newline/carriage return
    template <class T, unsigned char separator>
    static inline void readCSV(std::istream & fs, SPL::rstring & field)
    {
        SPL::skipSpaceTabs(fs);
        int c = SPL::safePeek(fs);
        if (c == '\"' || c == '\'') {
            fs >> field;
            return;
        }

        // read string until end of line or comma
        field = "";
        for (; c != EOF && c != '\r' && c != '\n' && c != separator; c = SPL::safePeek(fs))
            field.push_back(fs.get());
    }

    // This function is almost an exact copy of the above one. This is because
    // we want to avoid having a default argument, due to performance considerations
    template <class T, unsigned char separator>
    static inline void readCSV(std::istream & fs, SPL::rstring & field,
                               SPL::rstring const & defaultVal)
    {
        SPL::skipSpaceTabs(fs);
        int c = SPL::safePeek(fs);
        if (c == '\"' || c == '\'') {
            fs >> field;
            return;
        }

        // read string until end of line or comma
        if (c == EOF || c == separator || c == '\r' || c == '\n') {
            field = defaultVal;
            return;
        }

        field = "";
        for (; c != EOF && c != '\r' && c != '\n' && c != separator; c = SPL::safePeek(fs))
            field.push_back(fs.get());
    }

    // Read a bstring in quotes or without quotes but up until next
    // comma or newline/carriage return
    template <class T, unsigned char separator>
    static inline void readCSV(std::istream & fs, SPL::bstring<T::bounded_size> & field)
    {
        SPL::skipSpaceTabs(fs);
        int c = SPL::safePeek(fs);
        if (c == '\"' || c == '\'') {
            fs >> field;
            return;
        }

        SPL::rstring value;
        readCSV<SPL::rstring, separator>(fs, value);
        if (fs)
            field = value;
    }

    // Read a bstring in quotes or without quotes but up until next
    // comma or newline/carriage return
    template <class T, unsigned char separator>
    static inline void readCSV(std::istream & fs, SPL::bstring<T::bounded_size> & field,
                               SPL::bstring<T::bounded_size> const & defaultValue)
    {
        SPL::skipSpaceTabs(fs);
        int c = SPL::safePeek(fs);
        if (c == '\"' || c == '\'') {
            fs >> field;
            return;
        }

        SPL::rstring value;
        readCSV<SPL::rstring, separator>(fs, value, defaultValue);
        if (fs)
            field = value;
    }

    // Read a ustring in quotes or without quotes but up until next
    // comma or newline/carriage return
    // Read the string as a UTF-8 rstring and convert
    template <class T, unsigned char separator>
    static inline void readCSV(std::istream & fs, SPL::ustring & field)
    {
        SPL::rstring value;
        readCSV<SPL::rstring, separator>(fs, value);
        if (fs)
            field = SPL::ustring::fromUTF8(value);
    }

    // Read a ustring in quotes or without quotes but up until next
    // comma or newline/carriage return
    // Read the string as a UTF-8 rstring and convert
    template <class T, unsigned char separator>
    static inline void readCSV(std::istream & fs, SPL::ustring & field,
            SPL::ustring const & defaultValue)
    {
        SPL::skipSpaceTabs(fs);
        int c = SPL::safePeek(fs);
        if (c == '\"' || c == '\'') {
            SPL::rstring rfield;
            fs >> rfield;
            field = SPL::ustring::fromUTF8(rfield);
            return;
        }

        if (c == EOF || c == separator ||  c == '\r' || c == '\n' ) {
            field = defaultValue;
            return;
        }

        // read string until end of line or comma
        SPL::rstring rfield;
        for (; c != EOF && c != '\r' && c != '\n' && c != separator; c = SPL::safePeek(fs))
            rfield.push_back(fs.get());
        if (fs)
            field = SPL::ustring::fromUTF8(rfield);
    }

    static std::string printableCharacter(int c) {
        std::string result;
        static unsigned char character[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        if (std::isprint(c)) {
            result.append(1, c);
        } else {
            result += "\\x";
            int b = (c & 0x00f0) >> 4;
            result.append(1, character[b]);
            b = (c & 0x000f);
            result.append(1, character[b]);
        }
        return result;
    }
};

#endif /*SPL_RUNTIME_IO_HELPER_H */
