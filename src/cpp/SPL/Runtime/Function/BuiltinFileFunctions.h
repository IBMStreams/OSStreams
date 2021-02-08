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

#ifndef SPL_RUNTIME_FUNCTION_BUILTIN_FILE_FUNCTIONS_H
#define SPL_RUNTIME_FUNCTION_BUILTIN_FILE_FUNCTIONS_H

/*!
 * \file FileFunctions.h \brief Definitions of C++ counterparts of %SPL native functions for
 * file operations.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#endif /* DOXYGEN_SKIP_FOR_USERS */

#include <SPL/Runtime/Type/SPLType.h>

/*
 * Builtin SPL Utility functions
 */

/// @splcppns SPL::Functions::File
/// @spldir file
/// @splname SPL-File-Functions

/*!
 * \namespace SPL::Functions::File \brief C++ counterparts of %SPL native
 * functions that deal with file operations
 */

namespace SPL {
namespace Functions {
namespace File {

/// Create a directory
/// @param name     Directory name. This must be a fully qualified name. You can use built-in functions like `dataDirectory` to construct the fully qualified name.
/// @param mode     The permissions to be set on the new directory. This should have the equivalent values that would be passed to the C `mkdir` function.
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @splnative public stateful void mkdir(rstring name, uint32 mode, mutable int32 err)
inline void mkdir(const SPL::rstring& name, SPL::uint32 mode, SPL::int32& err)
{
    if (::mkdir(name.c_str(), mode) == -1) {
        err = errno;
    } else {
        err = 0;
    }
}

/// Remove a file or directory.
/// @param name     File or directory name.  This must be a fully qualified name.  You can use built-in functions like `dataDirectory` to construct the fully qualified name.
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @splnative public stateful void remove (rstring name, mutable int32 err)
/// @splnative public stateful void remove (ustring name, mutable int32 err)
inline void remove(const SPL::rstring& name, SPL::int32& err)
{
    if (::remove(name.c_str()) == -1) {
        err = errno;
    } else {
        err = 0;
    }
}

/// Remove a file or directory.
/// @param name     File or directory name.  This must be a fully qualified name.  You can use built-in functions like `dataDirectory` to construct the fully qualified name.
/// @param err      Set to 0 on success, or the `errno` value on failure.
inline void remove(const SPL::ustring& name, SPL::int32& err)
{
    if (::remove(SPL::ustring::toUTF8(name).c_str()) == -1) {
        err = errno;
    } else {
        err = 0;
    }
}

/// Open a file.
/// @param filename File to open.  This must be a fully qualified name.  You can use built-in functions like `dataDirectory` to construct the fully qualified name.
/// @param mode     How the file is to be used (for reading, writing, etc), as required by the system `fopen()` call, for example, "r" for reading.
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @return         The `FILE`* for the file, encoded as `uint64`.
/// @splnative public stateful uint64 fopen (rstring filename, rstring mode, mutable int32 err)
/// @splnative public stateful uint64 fopen (ustring filename, ustring mode, mutable int32 err)
inline SPL::uint64 fopen(const SPL::rstring& filename, const SPL::rstring& mode, SPL::int32& err)
{
    uint64_t res = (uint64_t)::fopen(filename.c_str(), mode.c_str());
    if (res == 0) {
        err = errno;
    } else {
        err = 0;
    }
    return res;
}

/// Open a file.
/// @param filename File to open.  This must be a fully qualified name.  You can use built-in functions like `dataDirectory` to construct the fully qualified name.
/// @param mode     How the file is to be used (for reading, writing, etc), as required by the system `fopen()` call, for example, "r" for reading.
/// @param err      Error, if any.
/// @return         The `FILE`* for the file, encoded as `uint64`.
inline SPL::uint64 fopen(const SPL::ustring& filename, const SPL::ustring& mode, SPL::int32& err)
{
    return fopen(SPL::ustring::toUTF8(filename), SPL::ustring::toUTF8(mode), err);
}

/// Close a file.
/// @param file     File to close (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @return         Return code from the system `fclose()` call.
/// @splnative public stateful int32 fclose (uint64 file, mutable int32 err)
inline SPL::int32 fclose(const SPL::uint64 file, SPL::int32& err)
{
    int32_t res = ::fclose(reinterpret_cast<FILE*>(file));
    if (res == -1) {
        err = errno;
    } else {
        err = 0;
    }
    return res;
}

/// Flush a file to disk.
/// @param file     File to flush (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @return         Return code from the system `fflush()` call.
/// @splnative public stateful int32 fflush (uint64 file, mutable int32 err)
inline SPL::int32 fflush(const SPL::uint64 file, SPL::int32& err)
{
    int32_t res = ::fflush(reinterpret_cast<FILE*>(file));
    if (res != 0) {
        err = errno;
    } else {
        err = 0;
    }
    return res;
}

/// Return the current file position.
/// @param file     File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @return         File position.
/// @splnative public stateful int64 ftell (uint64 file, mutable int32 err)
inline SPL::int64 ftell(const SPL::uint64 file, SPL::int32& err)
{
    int64_t res = ::ftell(reinterpret_cast<FILE*>(file));
    if (res == -1) {
        err = errno;
    } else {
        err = 0;
    }
    return res;
}

/// Set the current file position.
/// @param file     File (`FILE`* encoded as `uint64`).
/// @param offset   Position to seek to, as an offset in bytes.
/// @param whence   Indicator specifying where position is relative to (`optSEEK_SET()`, `optSEEK_CUR()` or `optSEEK_END()`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @splnative public stateful void fseek (uint64 file, int64 offset, int32 whence, mutable int32 err)
inline void fseek(const SPL::uint64 file,
                  const SPL::int64 offset,
                  const SPL::int32 whence,
                  SPL::int32& err)
{
    int32_t res = ::fseek(reinterpret_cast<FILE*>(file), offset, whence);
    if (res != 0) {
        err = errno;
    } else {
        err = 0;
    }
}

/// Return the indicator for specifying to seek relative to the start of the file.
/// @return The `SEEK_SET` indicator.
/// @splnative public int32 optSEEK_SET()
inline SPL::int32 optSEEK_SET()
{
    return SEEK_SET;
}

/// Return the indicator for specifying to seek relative to the current file position.
/// @return The `SEEK_CUR` indicator.
/// @splnative public int32 optSEEK_CUR()
inline SPL::int32 optSEEK_CUR()
{
    return SEEK_CUR;
}

/// Return the indicator for specifying to seek relative to the end of the file.
/// @return The `SEEK_END` indicator.
/// @splnative public int32 optSEEK_END()
inline SPL::int32 optSEEK_END()
{
    return SEEK_END;
}

/// Set the current file position to the start of the file.
/// @param file     File (`FILE`* encoded as `uint64`).
/// @splnative public stateful void rewind (uint64 file)
inline void rewind(const SPL::uint64 file)
{
    ::rewind(reinterpret_cast<FILE*>(file));
}

/// Test if the stream is at the end of the file.
/// @param file     File (`FILE`* encoded as `uint64`).
/// @return The value 'true' if the stream is at the end of the file.
/// @splnative public stateful boolean feof (uint64 file)
inline SPL::boolean feof(const SPL::uint64 file)
{
    return ::feof(reinterpret_cast<FILE*>(file)) != 0;
}

/// Clear the error and end-of-file indicators for an open file.
/// @param file File (`FILE`* encoded as `uint64`).
/// @splnative public stateful void clearerr (uint64 file)
inline void clearerr(const SPL::uint64 file)
{
    ::clearerr(reinterpret_cast<FILE*>(file));
}

/*
 * File Status Functions
 */

/// Return information about a file.
/// @param file         File name.  This must be a fully qualified name.  You can use built-in functions like `dataDirectory` to construct the fully qualified name.
/// @param what         Desired information from the system call `stat`: one of "dev", "rdev", "ino", "mode", "nlink", "uid", "gid", "size", "blksize", or "blocks".
/// @param result       Returned information.
/// @param err          Set to 0 on success, or the `errno` value on failure (`EINVAL` if the `what` string is not valid).
/// @splnative public stateful void fstat (rstring file, rstring what, mutable uint64 result, mutable int32 err)
/// @splnative public stateful void fstat (ustring file, ustring what, mutable uint64 result, mutable int32 err)
void fstat(const SPL::rstring& file,
           const SPL::rstring& what,
           SPL::uint64& result,
           SPL::int32& err);

/// Return information about a file.
/// @param file         File name.  This must be a fully qualified name.  You can use built-in functions like `dataDirectory` to construct the fully qualified name.
/// @param what         Desired information from the system call `stat`: one of "dev", "rdev", "ino", "mode", "nlink", "uid", "gid", "size", "blksize", or "blocks".
/// @param result       Returned information.
/// @param err          Set to 0 on success, or the `errno` value on failure (`EINVAL` if the `what` string is not valid).
inline void fstat(const SPL::ustring& file,
                  const SPL::ustring& what,
                  SPL::uint64& result,
                  SPL::int32& err)
{
    return fstat(SPL::ustring::toUTF8(file), SPL::ustring::toUTF8(what), result, err);
}

/// Return timestamp information about a file.
/// @param file         File name.  This must be a fully qualified name.  You can use built-in functions like `dataDirectory` to construct the fully qualified name.
/// @param what         Desired information from the system call `stat`: one of "atime", "mtime", or "ctime".
/// @param result       Returned information.
/// @param err          Set to 0 on success, or the `errno` value on failure (`EINVAL` if the `what` string is not valid).
/// @splnative public stateful void fstat (rstring file, rstring what, mutable timestamp result, mutable int32 err)
/// @splnative public stateful void fstat (ustring file, ustring what, mutable timestamp result, mutable int32 err)
void fstat(const SPL::rstring& file,
           const SPL::rstring& what,
           SPL::timestamp& result,
           SPL::int32& err);

/// Return timestamp information about a file.
/// @param file         File name.  This must be a fully qualified name.  You can use built-in functions like `dataDirectory` to construct the fully qualified name.
/// @param what         Desired information from the system call `stat`: one of "atime", "mtime", or "ctime".
/// @param result       Returned information.
/// @param err          Set to 0 on success, or the `errno` value on failure (`EINVAL` if the `what` string is not valid).
inline void fstat(const SPL::ustring& file,
                  const SPL::ustring& what,
                  SPL::timestamp& result,
                  SPL::int32& err)
{
    return fstat(SPL::ustring::toUTF8(file), SPL::ustring::toUTF8(what), result, err);
}

/// Return information about a file.
/// @param file         File  (`FILE`* encoded as `uint64`).
/// @param what         Desired information from the system call `fstat`: one of "dev", "rdev", "ino", "mode", "nlink", "uid", "gid", "size", "blksize", or "blocks".
/// @param result       Returned information.
/// @param err          Set to 0 on success, or the `errno` value on failure (`EINVAL` if the `what` string is not valid).
/// @splnative public stateful void fstat (uint64 file, rstring what, mutable uint64 result, mutable int32 err)
/// @splnative public stateful void fstat (uint64 file, ustring what, mutable uint64 result, mutable int32 err)
void fstat(const SPL::uint64 file, const SPL::rstring& what, SPL::uint64& result, SPL::int32& err);

/// Return information about a file.
/// @param file         File  (`FILE`* encoded as `uint64`).
/// @param what         Desired information from the system call `fstat`: one of "dev", "rdev", "ino", "mode", "nlink", "uid", "gid", "size", "blksize", or "blocks".
/// @param result       Returned information.
/// @param err          Set to 0 on success, or the `errno` value on failure (`EINVAL` if the `what` string is not valid).
inline void fstat(const SPL::uint64 file,
                  const SPL::ustring& what,
                  SPL::uint64& result,
                  SPL::int32& err)
{
    return fstat(file, SPL::ustring::toUTF8(what), result, err);
}

/// Return timestamp information about a file.
/// @param file         File  (`FILE`* encoded as `uint64`).
/// @param what         Desired information from the system call `fstat`: one of "atime", "mtime", or "ctime".
/// @param result       Returned information.
/// @param err          Set to 0 on success, or the `errno` value on failure (`EINVAL` if the `what` string is not valid).
/// @splnative public stateful void fstat (uint64 file, rstring what, mutable timestamp result, mutable int32 err)
/// @splnative public stateful void fstat (uint64 file, ustring what, mutable timestamp result, mutable int32 err)
void fstat(const SPL::uint64 file,
           const SPL::rstring& what,
           SPL::timestamp& result,
           SPL::int32& err);

/// Return timestamp information about a file.
/// @param file         File  (`FILE`* encoded as `uint64`).
/// @param what         Desired information from the system call `fstat`: one of "atime", "mtime", or "ctime".
/// @param result       Returned information.
/// @param err          Set to 0 on success, or the `errno` value on failure (`EINVAL` if the `what` string is not valid).
inline void fstat(const SPL::uint64 file,
                  const SPL::ustring& what,
                  SPL::timestamp& result,
                  SPL::int32& err)
{
    return fstat(file, SPL::ustring::toUTF8(what), result, err);
}

/*
 * File I/O functions
 */

/// Write a string to a file.
/// @param val          String value to be written.
/// @param len          Length of the value to be written.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
void fwriteString(const char* val, SPL::uint64 len, const SPL::uint64 file, SPL::int32& err);

/// Write a string to a file.
/// @param val          String value to be written.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @splnative <string T> public stateful void fwriteString (T val, uint64 file, mutable int32 err)
inline void fwriteString(const SPL::rstring& val, const SPL::uint64 file, SPL::int32& err)
{
    fwriteString(val.c_str(), val.size(), file, err);
}

/// Write a string to a file.
/// @param val          String value to be written.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
inline void fwriteString(const SPL::BString& val, const SPL::uint64 file, SPL::int32& err)
{
    fwriteString(val.getCString(), val.getUsedSize(), file, err);
}

/// Write a string to a file.
/// @param val          String value to be written.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
inline void fwriteString(const SPL::ustring& val, const SPL::uint64 file, SPL::int32& err)
{
    fwriteString(SPL::ustring::toUTF8(val), file, err);
}

/// Write an SPL value to a file as a text string.
/// @param val          Value to be written in serialized form.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @splnative <any T> public stateful void fwriteTxt (T val, uint64 file, mutable int32 err)
void fwriteTxt(const SPL::ConstValueHandle& val, const SPL::uint64 file, SPL::int32& err);

/// Write an SPL value to a file using binary format.
/// @param val          Value to be written in serialized form.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @splnative <any T> public stateful void fwriteBin (T val, uint64 file, mutable int32 err)
void fwriteBin(const SPL::ConstValueHandle& val, const SPL::uint64 file, SPL::int32& err);

/// Write a list of bytes to a file using binary format.
/// @param val          List containing the bytes to be written.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param loffset      Starting offset in the list.
/// @param length       Number of elements in the list to be written.
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @splnative public stateful void fwrite (list<uint8> val, uint64 file, uint64 loffset, uint64 length, mutable int32 err)
void fwrite(const SPL::list<SPL::uint8>& val,
            const SPL::uint64 file,
            const SPL::uint64 loffset,
            const SPL::uint64 length,
            SPL::int32& err);

/*
 * Input routines
 */

/// Read a 'token' from a file.  A token is a series of non-whitespace characters.
/// Whitespace is skipped until a non-whitespace character is found, and then all
/// non-whitespace characters in the token are returned.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or -1 if the end of the file is reached.
/// @return The token read from the file.  The token is empty if the end of the file is reached.
/// @splnative public stateful rstring freadToken (uint64 file, mutable int32 err)
SPL::rstring freadToken(const SPL::uint64 file, SPL::int32& err);

/// Read a line from a file.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or -1 if the end of the file is reached.
/// @return The line read from the file (the new line character is not included).
/// @splnative public stateful rstring freadLine (uint64 file, mutable int32 err)
SPL::rstring freadLine(const SPL::uint64 file, SPL::int32& err);

#ifndef DOXYGEN_SKIP_FOR_USERS
void freadTxt(SPL::ValueHandle& val, const SPL::uint64 file, SPL::int32& err);
void freadBin(SPL::ValueHandle& val, const SPL::uint64 file, SPL::int32& err);
#endif /* DOXYGEN_SKIP_FOR_USERS */

/// Read an SPL value from a file as a text string.
/// @param res          Value to read from the file (assumed to have been written using `fwriteTxt`).
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, -1 if the end of the file is reached, or `EIO` if there is an error deserializing the value.
/// @splnative <any T> public stateful void freadTxt (mutable T res, uint64 file, mutable int32 err)
template<class T>
inline void freadTxt(T& res, const SPL::uint64 file, SPL::int32& err)
{
    SPL::ValueHandle vh(res);
    freadTxt(vh, file, err);
}

/// Read an SPL value from a file using binary format.
/// @param res          Value to read from the file (assumed to have been written using `fwriteBin`).
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, -1 if the end of the file is reached, or `EIO` if there is an error deserializing the value.
/// @splnative <any T> public stateful void freadBin (mutable T res, uint64 file, mutable int32 err)
template<class T>
inline void freadBin(T& res, const SPL::uint64 file, SPL::int32& err)
{
    SPL::ValueHandle vh(res);
    freadBin(vh, file, err);
}

/// Read a list of bytes from a file using binary format.
/// @param res          List of bytes read.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param length       Number of bytes to be read.
/// @param err      Set to 0 on success, or -1 if the end of the file is reached.
/// @return Number of bytes read.
/// @splnative public stateful uint64 fread (mutable list<uint8> res, uint64 file, uint64 length, mutable int32 err)
SPL::uint64 fread(SPL::list<SPL::uint8>& res,
                  const SPL::uint64 file,
                  const SPL::uint64 length,
                  SPL::int32& err);

/// Read a whole file into a blob as binary.
/// @param res          Blob of bytes read.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @splnative public stateful void freadfile (mutable blob res, uint64 file, mutable int32 err)
void freadfile(SPL::blob& res, const SPL::uint64 file, SPL::int32& err);

/// Write a blob into a file as binary.
/// @param b            Blob of bytes to be written at the current position.
/// @param file         File (`FILE`* encoded as `uint64`).
/// @param err      Set to 0 on success, or the `errno` value on failure.
/// @splnative public stateful void fwriteblob (blob b, uint64 file, mutable int32 err)
void fwriteblob(const SPL::blob& b, const SPL::uint64 file, SPL::int32& err);

/*
 * Error testing functions
 */

/// Return whether the value is `EOF`.
/// @param v Value.
/// @return The value 'true' if the value is `EOF`.
/// @splnative public boolean isEOF (int32 v)
inline SPL::boolean isEOF(const SPL::uint32 v)
{
    return v == static_cast<SPL::uint32>(EOF);
}

/// Return whether the value is `EACCES`.
/// @param v Value.
/// @return The value 'true' if the value is `EACCES`.
/// @splnative public boolean isEACCES (int32 v)
inline SPL::boolean isEACCES(const SPL::uint32 v)
{
    return v == EACCES;
}

/// Return whether the value is `EAGAIN`.
/// @param v Value.
/// @return The value 'true' if the value is `EAGAIN`.
/// @splnative public boolean isEAGAIN (int32 v)
inline SPL::boolean isEAGAIN(const SPL::uint32 v)
{
    return v == EAGAIN;
}

/// Return whether the value is `EBADF`.
/// @param v Value.
/// @return The value 'true' if the value is `EBADF`.
/// @splnative public boolean isEBADF (int32 v)
inline SPL::boolean isEBADF(const SPL::uint32 v)
{
    return v == EBADF;
}

/// Return whether the value is `EFAULT`.
/// @param v Value.
/// @return The value 'true' if the value is `EFAULT`.
/// @splnative public boolean isEFAULT (int32 v)
inline SPL::boolean isEFAULT(const SPL::uint32 v)
{
    return v == EFAULT;
}

/// Return whether the value is `EFBIG`.
/// @param v Value.
/// @return The value 'true' if the value is `EFBIG`.
/// @splnative public boolean isEFBIG (int32 v)
inline SPL::boolean isEFBIG(const SPL::uint32 v)
{
    return v == EFBIG;
}

/// Return whether the value is `EINTR`.
/// @param v Value.
/// @return The value 'true' if the value is `EINTR`.
/// @splnative public boolean isEINTR (int32 v)
inline SPL::boolean isEINTR(const SPL::uint32 v)
{
    return v == EINTR;
}

/// Return whether the value is `EINVAL`.
/// @param v Value.
/// @return The value 'true' if the value is `EINVAL`.
/// @splnative public boolean isEINVAL (int32 v)
inline SPL::boolean isEINVAL(const SPL::uint32 v)
{
    return v == EINVAL;
}

/// Return whether the value is `EIO`.
/// @param v Value.
/// @return The value 'true' if the value is `EIO`.
/// @splnative public boolean isEIO (int32 v)
inline SPL::boolean isEIO(const SPL::uint32 v)
{
    return v == EIO;
}

/// Return whether the value is `EISDIR`.
/// @param v Value.
/// @return The value 'true' if the value is `EISDIR`.
/// @splnative public boolean isEISDIR (int32 v)
inline SPL::boolean isEISDIR(const SPL::uint32 v)
{
    return v == EISDIR;
}

/// Return whether the value is `EMFILE`.
/// @param v Value.
/// @return The value 'true' if the value is `EMFILE`.
/// @splnative public boolean isEMFILE (int32 v)
inline SPL::boolean isEMFILE(const SPL::uint32 v)
{
    return v == EMFILE;
}

/// Return whether the value is `ENFILE`.
/// @param v Value.
/// @return The value 'true' if the value is `ENFILE`.
/// @splnative public boolean isENFILE (int32 v)
inline SPL::boolean isENFILE(const SPL::uint32 v)
{
    return v == ENFILE;
}

/// Return whether the value is `ELOOP`.
/// @param v Value.
/// @return The value 'true' if the value is `ELOOP`.
/// @splnative public boolean isELOOP (int32 v)
inline SPL::boolean isELOOP(const SPL::uint32 v)
{
    return v == ELOOP;
}

/// Return whether the value is `ENAMETOOLONG`.
/// @param v Value.
/// @return The value 'true' if the value is `ENAMETOOLONG`.
/// @splnative public boolean isENAMETOOLONG (int32 v)
inline SPL::boolean isENAMETOOLONG(const SPL::uint32 v)
{
    return v == ENAMETOOLONG;
}

/// Return whether the value is `ENODEV`.
/// @param v Value.
/// @return The value 'true' if the value is `ENODEV`.
/// @splnative public boolean isENODEV (int32 v)
inline SPL::boolean isENODEV(const SPL::uint32 v)
{
    return v == ENODEV;
}

/// Return whether the value is `ENOENT`.
/// @param v Value.
/// @return The value 'true' if the value is `ENOENT`.
/// @splnative public boolean isENOENT (int32 v)
inline SPL::boolean isENOENT(const SPL::uint32 v)
{
    return v == ENOENT;
}

/// Return whether the value is `ENOMEM`.
/// @param v Value.
/// @return The value 'true' if the value is `ENOMEM`.
/// @splnative public boolean isENOMEM (int32 v)
inline SPL::boolean isENOMEM(const SPL::uint32 v)
{
    return v == ENOMEM;
}

/// Return whether the value is `ENOSPC`.
/// @param v Value.
/// @return The value 'true' if the value is `ENOSPC`.
/// @splnative public boolean isENOSPC (int32 v)
inline SPL::boolean isENOSPC(const SPL::uint32 v)
{
    return v == ENOSPC;
}

/// Return whether the value is `ENOTDIR`.
/// @param v Value.
/// @return The value 'true' if the value is `ENOTDIR`.
/// @splnative public boolean isENOTDIR (int32 v)
inline SPL::boolean isENOTDIR(const SPL::uint32 v)
{
    return v == ENOTDIR;
}

/// Return whether the value is `ENXIO`.
/// @param v Value.
/// @return The value 'true' if the value is `ENXIO`.
/// @splnative public boolean isENXIO (int32 v)
inline SPL::boolean isENXIO(const SPL::uint32 v)
{
    return v == ENXIO;
}

/// Return whether the value is `EOVERFLOW`.
/// @param v Value.
/// @return The value 'true' if the value is `EOVERFLOW`.
/// @splnative public boolean isEOVERFLOW (int32 v)
inline SPL::boolean isEOVERFLOW(const SPL::uint32 v)
{
    return v == EOVERFLOW;
}

/// Return whether the value is `EPIPE`.
/// @param v Value.
/// @return The value 'true' if the value is `EPIPE`.
/// @splnative public boolean isEPIPE (int32 v)
inline SPL::boolean isEPIPE(const SPL::uint32 v)
{
    return v == EPIPE;
}

/// Return whether the value is `EPERM`.
/// @param v Value.
/// @return The value 'true' if the value is `EPERM`.
/// @splnative public boolean isEPERM (int32 v)
inline SPL::boolean isEPERM(const SPL::uint32 v)
{
    return v == EPERM;
}

/// Return whether the value is `EROFS`.
/// @param v Value.
/// @return The value 'true' if the value is `EROFS`.
/// @splnative public boolean isEROFS (int32 v)
inline SPL::boolean isEROFS(const SPL::uint32 v)
{
    return v == EROFS;
}

/// Return whether the value is `ESPIPE`.
/// @param v Value.
/// @return The value 'true' if the value is `ESPIPE`.
/// @splnative public boolean isESPIPE (int32 v)
inline SPL::boolean isESPIPE(const SPL::uint32 v)
{
    return v == ESPIPE;
}

/// Return whether the value is `ETXTBSY`.
/// @param v Value.
/// @return The value 'true' if the value is `ETXTBSY`.
/// @splnative public boolean isETXTBSY (int32 v)
inline SPL::boolean isETXTBSY(const SPL::uint32 v)
{
    return v == ETXTBSY;
}

/// Return whether the value is `EWOULDBLOCK`.
/// @param v Value.
/// @return The value 'true' if the value is `EWOULDBLOCK`.
/// @splnative public boolean isEWOULDBLOCK (int32 v)
inline SPL::boolean isEWOULDBLOCK(const SPL::uint32 v)
{
    return v == EWOULDBLOCK;
}

}
}
}

#endif /* SPL_RUNTIME_FUNCTION_BUILTIN_FILE_FUNCTIONS_H */
