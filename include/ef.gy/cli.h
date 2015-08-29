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

#if !defined(EF_GY_CLI_H)
#define EF_GY_CLI_H

#include <functional>
#include <regex>
#include <iostream>
#include <set>
#include <string>

namespace efgy {
namespace cli {
class option;

template <class option = option> class options {
public:
  static options &common(void) {
    static options opt;
    return opt;
  }

  options &add(option &o) {
    opts.insert(&o);
    return *this;
  }

  options &remove(option &o) {
    opts.erase(&o);
    return *this;
  }

  std::size_t apply(int argc, char *argv[]) {
    std::size_t r = 0;
    remainder = std::vector<std::string>();

    for (int i = 1; i < argc; i++) {
      std::string s = argv[i];
      for (const auto &opt : opts) {
        std::smatch matches;
        if (std::regex_match(s, matches, opt->match) && opt->handler(matches)) {
          r++;
        }
      }
    }

    if (r == 0) {
      return usage(argv[0]);
    }

    return r;
  }

  std::size_t usage(const std::string &name) {
    std::cerr << "Usage: " << name << " [options...]\n"
              << "\n"
              << "Where [options...] is any of the following:\n";

    for (const auto &opt : opts) {
      std::cerr << "  " << opt->regex << "\t " << opt->description << "\n";
    }

    return 0;
  }

  std::vector<std::string> remainder;

protected:
  std::set<option *> opts;
};

class option {
public:
  option(const std::string &pMatch, std::function<bool(std::smatch &)> pHandler,
         const std::string &pDescription = "Please document me.",
         options<option> &pOpts = options<option>::common())
      : regex(pMatch), match(pMatch), handler(pHandler),
        description(pDescription), opts(pOpts) {
    opts.add(*this);
  }

  ~option(void) { opts.remove(*this); }

  const std::string regex;
  const std::string description;
  const std::regex match;
  const std::function<bool(std::smatch &)> handler;

protected:
  options<option> &opts;
};
}
}

#endif