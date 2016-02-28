/**\file
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_IRCD_H)
#define EF_GY_IRCD_H

#include <ef.gy/irc.h>

namespace efgy {
namespace ircd {

template <class sock>
static std::size_t setup(net::endpoint<sock> lookup,
                         io::service &service = io::service::common()) {
  return lookup.with([&service, &lookup](
                         typename sock::endpoint &endpoint) -> bool {
    net::irc::server<sock> *s = new net::irc::server<sock>(endpoint, service);

    s->name = lookup.name();

    return true;
  });
}

static cli::option socket("-{0,2}irc:unix:(.+)", [](std::smatch &m) -> bool {
  return setup(net::endpoint<asio::local::stream_protocol>(m[1])) > 0;
}, "Listen for IRC connections on the given unix socket[1].");

static cli::option tcp("-{0,2}irc:(.+):([0-9]+)", [](std::smatch &m) -> bool {
  return setup(net::endpoint<asio::ip::tcp>(m[1], m[2])) > 0;
}, "Listen for IRC connections on the given host[1] and port[2].");
}
}

#endif
