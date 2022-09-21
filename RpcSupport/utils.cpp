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

#include "RpcSupport/utils.h"

#include "Support/fileutils.h"

#include <boost/filesystem.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <glog/logging.h>

#include <cstdlib> // getenv
#include <stdexcept>

namespace bf = boost::filesystem;

namespace uiiit {
namespace rpc {

RpcFailed::RpcFailed(const grpc::Status& aStatus)
    : std::runtime_error("RPC failed: " + aStatus.error_message()) {
}

void checkStatus(const grpc::Status& aStatus) {
  if (not aStatus.ok()) {
    throw RpcFailed(aStatus);
  }
}

struct Env final {
  std::string operator()(const std::string& aName,
                         const std::string& aFallback) const {
    if (getenv(aName.c_str()) == nullptr) {
      return aFallback;
    }
    return std::string(getenv(aName.c_str()));
  }
};

struct Cwd final {
  Cwd(const std::string& aEnvName)
      : theOldPath(bf::current_path())
      , theCertDir(Env()(aEnvName, "")) {
    if (not theCertDir.empty()) {
      const bf::path myCertPath(theCertDir);
      if (not bf::exists(myCertPath) or not bf::is_directory(myCertPath)) {
        throw std::runtime_error("Invalid CERT_DIR: " + theCertDir);
      }
      bf::current_path(myCertPath);
    }
  }

  ~Cwd() {
    if (not theCertDir.empty()) {
      bf::current_path(theOldPath);
    }
  }

  const bf::path    theOldPath;
  const std::string theCertDir;
};

grpc::SslServerCredentialsOptions sslServerCredOpts() {
  Cwd        myCwd("CERT_DIR");
  const auto myRootCrt   = Env()("CA_CRT", "ca.crt");
  const auto myServerKey = Env()("SERVER_KEY", "server.key");
  const auto myServerCrt = Env()("SERVER_CRT", "server.crt");
  VLOG(1) << "root crt: " << myRootCrt << ", server key: " << myServerKey
          << ", server crt: " << myServerCrt << " [CWD " << bf::current_path()
          << "]";
  grpc::SslServerCredentialsOptions ret(
      GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY);
  ret.pem_root_certs = support::readFileAsString(myRootCrt);
  ret.pem_key_cert_pairs.emplace_back(
      grpc::SslServerCredentialsOptions::PemKeyCertPair{
          support::readFileAsString(myServerKey),
          support::readFileAsString(myServerCrt)});
  return ret;
}

grpc::SslCredentialsOptions sslClientCredOpts() {
  Cwd        myCwd("CERT_DIR");
  const auto myRootCrt   = Env()("CA_CRT", "ca.crt");
  const auto myClientKey = Env()("CLIENT_KEY", "client.key");
  const auto myClientCrt = Env()("CLIENT_CRT", "client.crt");
  VLOG(1) << "root crt: " << myRootCrt << ", client key: " << myClientKey
          << ", client crt: " << myClientCrt << " [CWD " << bf::current_path()
          << "]";
  grpc::SslCredentialsOptions ret;
  ret.pem_root_certs  = support::readFileAsString(myRootCrt);
  ret.pem_private_key = support::readFileAsString(myClientKey);
  ret.pem_cert_chain  = support::readFileAsString(myClientCrt);
  return ret;
}

} // namespace rpc
} // namespace uiiit
