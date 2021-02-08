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

#ifndef _HASHSTREAM_H_
#define _HASHSTREAM_H_

#include <iostream>
#include <openssl/evp.h>

#include <UTILS/DistilleryException.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

class HashStream
{
  protected:
    EVP_MD_CTX _mdctx;

  public:
    HashStream(const std::string& algo);
    HashStream(const EVP_MD* md);
    virtual ~HashStream();

    virtual void add(const void* d, size_t cnt);
    virtual std::string toString();
    virtual void toRaw(unsigned char* buffer, unsigned int& len);

    HashStream& operator<<(const std::string& str);
    HashStream& operator<<(const char* str);

#define OP(ty) HashStream& operator<<(const ty v);

    OP(unsigned long long);
    OP(long long);
    OP(unsigned long);
    OP(long);
    OP(unsigned int);
    OP(int);
    OP(unsigned short);
    OP(short);
    OP(unsigned char);
    OP(char);
    OP(double);
    OP(float);

#undef OP

  private:
    HashStream& operator<<(const void* ptr);
};

std::ostream& operator<<(std::ostream& strm, HashStream& hs);

class MD5HashStream : public HashStream
{
  public:
    MD5HashStream();
    ;
    virtual ~MD5HashStream();
};

class SHA1HashStream : public HashStream
{
  public:
    SHA1HashStream();
    virtual ~SHA1HashStream();
};

DECL_EXCEPTION(Distillery, HashStream, Distillery);
DECL_EXCEPTION(Distillery, UnknownDigest, HashStream);
DECL_EXCEPTION(Distillery, DigestInitFailed, HashStream);
DECL_EXCEPTION(Distillery, DigestUpdateFailed, HashStream);
DECL_EXCEPTION(Distillery, DigestFinalFailed, HashStream);

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
