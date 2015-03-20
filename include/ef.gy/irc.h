/**\file
 * \brief asio.hpp IRC Server
 *
 * \copyright
 * Copyright (c) 2015, ef.gy Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#if !defined(EF_GY_HTTP_H)
#define EF_GY_HTTP_H

#include <set>
#include <map>
#include <string>
#include <sstream>

#include <regex>
#include <system_error>
#include <algorithm>
#include <functional>

#include <ef.gy/server.h>

namespace efgy {
namespace net {
namespace irc {

template <typename base, typename requestProcessor> class session;

template <typename base, typename requestProcessor>
using server = net::server<base, requestProcessor, session>;

template <typename base, typename requestProcessor> class session {
protected:
  typedef server<base, requestProcessor> serverType;

  enum {
    expect_nick_user,
    expect_nick,
    expect_user,
    nominal,
    expect_pong,
    shutdown
  } status;

public:
  typename base::socket socket;

  std::string user;
  std::string nick;
  std::set<std::string> subscriptions;

  std::string content;

  serverType &server;

  session(serverType &pServer)
      : server(pServer), socket(pServer.io), status(expect_nick_user), input() {}

  ~session(void) {
    status = shutdown;

    try {
      socket.shutdown(base::socket::shutdown_both);
    }
    catch (std::system_error & e) {
      std::cerr << "exception while shutting down socket: " << e.what() << "\n";
    }

    try {
      socket.close();
    }
    catch (std::system_error & e) {
      std::cerr << "exception while closing socket: " << e.what() << "\n";
    }
  }

  void start() { read(); }

  void reply() {
  }

protected:
  void handleRead(const std::error_code &error, size_t bytes_transferred) {
    if (status == shutdown) {
      return;
    }

    if (!error) {
      static const std::regex line("(:[^ ]+)? (.*)\r?");
      std::istream is(&input);
      std::string s;
      std::smatch matches;

      std::getline(is, s);

      server.log << s << "\n";

      read();
    } else {
      delete this;
    }
  }

  void handleWrite(const std::error_code &error) {
    if (status == shutdown) {
      return;
    }

    if (!error) {
      read();
    } else {
      delete this;
    }
  }

  void read(void) {
    switch (status) {
    case shutdown:
      break;
    default:
      asio::async_read_until(
          socket, input, "\n",
          [&](const asio::error_code & error, std::size_t bytes_transferred) {
        handleRead(error, bytes_transferred);
      });
      break;
    }
  }

  asio::streambuf input;
};
}
}
}

#endif
