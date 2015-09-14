/**\file
 *
 * \copyright
 * Copyright (c) 2012-2015, ef.gy Project Members
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

#if !defined(EF_GY_IRCD_H)
#define EF_GY_IRCD_H

#include <ef.gy/irc.h>

namespace efgy {
namespace ircd {

template <class sock>
static std::size_t setup(net::endpoint<sock> lookup,
                         io::service &service = io::service::common()) {
  return lookup.with([&service, &lookup](typename sock::endpoint &endpoint)
                         -> bool {
    net::irc::server<sock> *s = new net::irc::server<sock>(endpoint, service);

    s->name = lookup.name();

    return true;
  });
}

static cli::option
    socket("-{0,2}irc:unix:(.+)",
           [](std::smatch &m) -> bool {
             return setup(net::endpoint<asio::local::stream_protocol>(m[1])) >
                    0;
           },
           "Listen for IRC connections on the given unix socket[1].");

static cli::option
    tcp("-{0,2}irc:(.+):([0-9]+)",
        [](std::smatch &m) -> bool {
          return setup(net::endpoint<asio::ip::tcp>(m[1], m[2])) > 0;
        },
        "Listen for IRC connections on the given host[1] and port[2].");
}
}

#endif
