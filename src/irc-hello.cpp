/**\file
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
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#define ASIO_DISABLE_THREADS
#include <ef.gy/irc.h>

using namespace efgy;
using asio::ip::tcp;
using asio::local::stream_protocol;

/**\brief Main function for the IRC demo
 *
 * This is the main function for the IRC Hello World demo.
 *
 * \param[in] argc Process argument count.
 * \param[in] argv Process argument vector
 *
 * \returns 0 when nothing bad happened, 1 otherwise.
 */
int main(int argc, char *argv[]) {
  try {
    int targets = 0;

    if (argc < 2) {
      std::cerr << "Usage: irc-hello [irc:<host>:<port>|irc:unix:<path>]...\n";
      return 1;
    }

    asio::io_service io_service;
    tcp::resolver resolver(io_service);

    for (unsigned int i = 1; i < argc; i++) {
      static const std::regex irc("irc:(.+):([0-9]+)");
      static const std::regex ircSocket("irc:unix:(.+)");
      std::smatch matches;

      if (std::regex_match(std::string(argv[i]), matches, ircSocket)) {
        std::string socket = matches[1];

        stream_protocol::endpoint endpoint(socket);
        net::irc::server<stream_protocol> *s =
            new net::irc::server<stream_protocol>(io_service, endpoint);

        s->name = socket;

        targets++;
      } else if (std::regex_match(std::string(argv[i]), matches, irc)) {
        std::string host = matches[1];
        std::string port = matches[2];

        tcp::resolver::query query(host, port);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

        if (endpoint_iterator != end) {
          tcp::endpoint endpoint = *endpoint_iterator;
          net::irc::server<tcp> *s =
              new net::irc::server<tcp>(io_service, endpoint);

          s->name = host;

          targets++;
        }
      }
    }

    if (targets > 0) {
      io_service.run();
    }

    return 0;
  }
  catch (std::exception & e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  catch (std::system_error & e) {
    std::cerr << "System Error: " << e.what() << "\n";
  }

  return 1;
}
