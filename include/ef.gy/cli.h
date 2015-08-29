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

  int apply(int argc, char *argv[]) {
    int r = 0;
    remainder = std::vector<std::string>();

    for (int i = 0; i < argc; i++) {
      std::string s = argv[i];
      for (const auto &opt : opts) {
        std::smatch matches;
        if (std::regex_match(s, matches, opt->match) && opt->handler(matches)) {
          r++;
        }
      }
    }

    return r;
  }

  std::vector<std::string> remainder;

protected:
  std::set<option *> opts;
};

class option {
public:
  option(const std::regex &pMatch, std::function<bool(std::smatch &)> pHandler,
         options<option> &pOpts = options<option>::common())
      : match(pMatch), handler(pHandler), opts(pOpts) {
    opts.add(*this);
  }

  ~option(void) { opts.remove(*this); }

  const std::regex match;
  const std::function<bool(std::smatch &)> handler;

protected:
  options<option> &opts;
};
}
}

#endif
