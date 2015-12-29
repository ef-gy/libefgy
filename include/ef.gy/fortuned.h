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

#if !defined(EF_GY_FORTUNED_H)
#define EF_GY_FORTUNED_H

#include <ef.gy/httpd.h>
#include <ef.gy/fortune.h>

namespace efgy {
namespace fortuned {
static const std::string regex = "/fortune(/([0-9]+))?";

template <class transport>
static bool fortune(typename net::http::server<transport>::session &session,
                    std::smatch &) {
  const auto &c = fortune::common().get();
  std::string sc = std::string(c);

  /* note: this escaping is not exactly efficient, but it's fairly simple
     and the std::strings are fairly short, so it shouldn't be much of an issue.
     */
  for (char i = 0; i < 0x20; i++) {
    if ((i == '\n') || (i == '\t')) {
      continue;
    }
    const char org[2] = {i, 0};
    const char rep[3] = {'^', (char)(('A' - 1) + i), 0};
    for (size_t pos = sc.find(org, pos); pos != std::string::npos;
         pos = sc.find(org, pos)) {
      sc.replace(pos, 2, rep);
      pos += 3;
    }
  }

  sc = "<![CDATA[" + sc + "]]>";

  session.reply(
      200, "Content-Type: text/xml; charset=utf-8\r\n",
      std::string("<?xml version='1.0' encoding='utf-8'?>"
                  "<fortune xmlns='http://ef.gy/2012/fortune' sourceFile='" +
                  c.file + "'>" + sc + "</fortune>"));

  return true;
}

static cli::option count("-{0,2}count", [](std::smatch &m) -> bool {
  std::cout << fortune::common().size() << " cookie(s) loaded\n";

  return true;
}, "Prints the number of fortune cookies in the database.");

static cli::option print("-{0,2}print(:([0-9]+))?",
                         [](std::smatch &m) -> bool {
                           if (m[2] != "") {
                             std::cout << std::string(
                                 fortune::common().get(std::stoll(m[2])));
                           } else {
                             std::cout << std::string(fortune::common().get());
                           }

                           return true;
                         },
                         "Print a fortune to stdout - a numerical parameter "
                         "selects a specific cookie.");
}
}

#endif
