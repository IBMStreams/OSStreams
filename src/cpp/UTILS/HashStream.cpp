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

#include <sstream>
#include <string.h>

#include <UTILS/HashStream.h>

using namespace std;
UTILS_NAMESPACE_USE;

HashStream::HashStream(const string& algo)
{
    const EVP_MD* md = EVP_get_digestbyname(algo.c_str());

    if (!md) {
        THROW(UnknownDigest, "");
    }

    EVP_MD_CTX_init(&_mdctx);
    if (EVP_DigestInit_ex(&_mdctx, md, NULL) != 1) {
        THROW(DigestInitFailed, "");
    }
}

HashStream::HashStream(const EVP_MD* md)
{
    EVP_MD_CTX_init(&_mdctx);
    if (EVP_DigestInit_ex(&_mdctx, md, NULL) != 1) {
        THROW(DigestInitFailed, "");
    }
}

HashStream::~HashStream()
{
    EVP_MD_CTX_cleanup(&_mdctx);
}

void HashStream::add(const void* d, size_t cnt)
{
    if (EVP_DigestUpdate(&_mdctx, d, cnt) != 1) {
        THROW(DigestUpdateFailed, "");
    }
}

string HashStream::toString()
{
    unsigned char buffer[EVP_MAX_MD_SIZE];
    unsigned int len = EVP_MAX_MD_SIZE;

    if (EVP_DigestFinal_ex(&_mdctx, buffer, &len) != 1) {
        THROW(DigestFinalFailed, "");
    }

    stringstream res;

    for (unsigned int i = 0; i < len; i++) {
        char b[3];
        snprintf(b, 3, "%02x", buffer[i]);
        res << b;
    }

    return res.str();
}

void HashStream::toRaw(unsigned char* buffer, unsigned int& len)
{
    unsigned int l = len;
    if (EVP_DigestFinal_ex(&_mdctx, buffer, &l) != 1) {
        THROW(DigestFinalFailed, "");
    }
    len = l;
}

HashStream& HashStream::operator<<(const string& str)
{
    add(str.c_str(), str.length());

    return *this;
}

HashStream& HashStream::operator<<(const char* str)
{
    add(str, strlen(str));

    return *this;
}

#define OP(ty)                                                                                     \
    HashStream& HashStream::operator<<(const ty v)                                                 \
    {                                                                                              \
        add(&v, sizeof(v));                                                                        \
        return *this;                                                                              \
    }

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

HashStream& HashStream::operator<<(const void* ptr)
{
    return *this;
}

ostream& Distillery::operator<<(ostream& strm, HashStream& hs)
{
    strm << hs.toString();
    return strm;
}

MD5HashStream::MD5HashStream()
  : HashStream(EVP_md5())
{}

MD5HashStream::~MD5HashStream() {}

SHA1HashStream::SHA1HashStream()
  : HashStream(EVP_sha1())
{}

SHA1HashStream::~SHA1HashStream() {}

#if 0

int main(int argc, char** argv)
{
  char mess1[] = "Test Message\n";
  char mess2[] = "Hello World\n";

  OpenSSL_add_all_digests();

  if(!argv[1])
  {
    printf("Usage: mdtest digestname\n");
    exit(1);
  }

  HashStream hs(argv[1]) ;
  hs << mess1 << mess2 ;
  cout << "Digest is: " << hs << endl ;

  return 0 ;
}

#endif

IMPL_EXCEPTION(Distillery, HashStream, Distillery);
IMPL_EXCEPTION(Distillery, UnknownDigest, HashStream);
IMPL_EXCEPTION(Distillery, DigestInitFailed, HashStream);
IMPL_EXCEPTION(Distillery, DigestUpdateFailed, HashStream);
IMPL_EXCEPTION(Distillery, DigestFinalFailed, HashStream);
