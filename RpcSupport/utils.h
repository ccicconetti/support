/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2019 Claudio Cicconetti https://ccicconetti.github.io/

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <grpc++/grpc++.h>

#include <stdexcept>

namespace grpc {
class Status;
}

namespace uiiit {
namespace rpc {

struct RpcFailed : public std::runtime_error {
  explicit RpcFailed(const grpc::Status& aStatus);
};

void checkStatus(const grpc::Status& aStatus);

/**
 * @brief Return SSL/TLS server credential options.
 *
 * The client is forced to authenticate.
 *
 * The CA and server certificates and the server key are read from files
 * that can be specified in the environment variables CA_CRT, SERVER_CRT, and
 * SERVER_KEY, respectively. If not specified, then the following default files
 * are used: ca.crt, server.crt, server.key.
 *
 * @return grpc::SslServerCredentialsOptions
 *
 * @throws std::runtime_error if the certificate/key files do not exist or
 * they cannot be read.
 */
grpc::SslServerCredentialsOptions sslServerCredOpts();

/**
 * @brief Return SSL/TLS client credential options.
 *
 * The CA and client certificates and the client key are read from files
 * that can be specified in the environment variables CA_CRT, CLIENT_CRT, and
 * CLIENT_KEY, respectively. If not specified, then the following default files
 * are used: ca.crt, client.crt, client.key.
 *
 * @return grpc::SslServerCredentialsOptions
 *
 * @throws std::runtime_error if the certificate/key files do not exist or
 * they cannot be read.
 */
grpc::SslCredentialsOptions sslClientCredOpts();

} // namespace rpc
} // namespace uiiit
