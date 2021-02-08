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

#include <UTILS/Security.h>
#include <openssl/err.h>

UTILS_NAMESPACE_USE;

Security::Type Security::parseType(const std::string& value)
{
    if (value == "none") {
        return Security::None;
    }
    if (value == "SSLv3") {
        return Security::SSLv3;
    }
    if (value == "TLSv1") {
        return Security::TLSv1;
    }
    if (value == "TLSv1.1") {
        return Security::TLSv1_1;
    }
    if (value == "TLSv1.2") {
        return Security::TLSv1_2;
    }
    return Security::None;
}

std::string Security::getError()
{
    BIO* bio = BIO_new(BIO_s_mem());
    ERR_print_errors(bio);
    char* buf = NULL;
    size_t len = BIO_get_mem_data(bio, &buf);
    std::string result(buf, len);
    BIO_free(bio);
    return result;
}

std::string Security::getError(SSL* ssl, int code)
{
    switch (SSL_get_error(ssl, code)) {
        case SSL_ERROR_NONE:
            return "SSL_ERROR_NONE";
        case SSL_ERROR_ZERO_RETURN:
            return "SSL_ERROR_ZERO_RETURN";
        case SSL_ERROR_WANT_READ:
            return "SSL_ERROR_WANT_READ";
        case SSL_ERROR_WANT_WRITE:
            return "SSL_ERROR_WANT_WRITE";
        case SSL_ERROR_WANT_CONNECT:
            return "SSL_ERROR_WANT_CONNECT";
        case SSL_ERROR_WANT_ACCEPT:
            return "SSL_ERROR_WANT_ACCEPT";
        case SSL_ERROR_WANT_X509_LOOKUP:
            return "SSL_ERROR_WANT_X509_LOOKUP";
        case SSL_ERROR_SYSCALL: {
            std::string ssl_err(ERR_error_string(ERR_peek_error(), NULL));
            return "SSL_ERROR_SYSCALL(" + ssl_err + ")";
        }
        case SSL_ERROR_SSL: {
            std::string ssl_err(ERR_error_string(ERR_peek_error(), NULL));
            return "SSL_ERROR_SSL(" + ssl_err + ")";
        }
        default:
            return "UNKNOWN_ERROR";
    }
}
