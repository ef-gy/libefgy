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

#if !defined(EF_GY_CLI_H)
#define EF_GY_CLI_H

#include <functional>
#include <iostream>
#include <regex>
#include <set>
#include <string>

namespace efgy {
namespace cli {
class option;
class hint;

template <class option = option, class hint = hint>
class options {
 public:
  static options &common(void) {
    static options opt;
    return opt;
  }

  options &add(option &o) {
    opts.insert(&o);
    return *this;
  }

  options &add(hint &h) {
    hints.insert(&h);
    return *this;
  }

  options &remove(option &o) {
    opts.erase(&o);
    return *this;
  }

  options &remove(hint &h) {
    hints.erase(&h);
    return *this;
  }

  std::size_t apply(const std::vector<std::string> &args) {
    std::size_t r = 0;
    remainder = std::vector<std::string>();
    bool doneWithArgs = false;
    bool firstArg = true;

    for (const auto &arg : args) {
      if (arg == "--") {
        doneWithArgs = true;
        continue;
      }

      bool haveMatch = false;

      if (!doneWithArgs) {
        for (const auto &opt : opts) {
          std::smatch matches;
          if (std::regex_match(arg, matches, opt->match) &&
              opt->handler(matches)) {
            r++;
            haveMatch = true;
          }
        }
      }

      if (!haveMatch && !firstArg) {
        remainder.push_back(arg);
      }

      firstArg = false;
    }

    if ((r == 0) && (args.size() > 0) && (remainder.size() == 0)) {
      return usage(args[0]);
    }

    return r;
  }

  std::size_t apply(int argc, char *argv[]) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++) {
      args.push_back(argv[i]);
    }
    return apply(args);
  }

  std::size_t usage(const std::string &name = "<command>") {
    std::cout << "Usage: " << name << " [options...] [--] [input...]\n"
              << "\n"
              << "Where [options...] is any of the following:\n";

    for (const auto &opt : opts) {
      std::cout << "  " << opt->regex;
      if (opt->regex.size() < 25) {
        for (std::size_t i = opt->regex.size() + 1; i < 26; i++) {
          std::cout << " ";
        }
      } else {
        std::cout << "\n                           ";
      }
      std::cout << opt->description << "\n";
    }

    for (const auto &hn : hints) {
      std::cout << "\n" << hn->title << ":\n" << hn->usage();
    }

    return 0;
  }

  std::vector<std::string> remainder;

 protected:
  std::set<option *> opts;
  std::set<hint *> hints;
};

class option {
 public:
  option(const std::string &pMatch, std::function<bool(std::smatch &)> pHandler,
         const std::string &pDescription = "Please document me.",
         options<option> &pOpts = options<option, hint>::common())
      : regex(pMatch),
        match(pMatch),
        handler(pHandler),
        description(pDescription),
        opts(pOpts) {
    opts.add(*this);
  }

  ~option(void) { opts.remove(*this); }

  const std::string regex;
  const std::string description;
  const std::regex match;
  const std::function<bool(std::smatch &)> handler;

 protected:
  options<option, hint> &opts;
};

template <typename type = bool>
class flag : public option {
 public:
  flag(const std::string &pName,
       const std::string &pDescription = "Please document me.",
       options<option> &pOpts = options<option, hint>::common())
      : option("-{0,2}((no)-?)?" + pName,
               [this](std::smatch &m) -> bool {
                 value = m[2] != "no";
                 return true;
               },
               "[Boolean] " + pDescription, pOpts),
        value(false) {}

  operator const bool(void) const { return value; }

 protected:
  bool value;
};

template <>
class flag<std::string> : public option {
 public:
  flag(const std::string &pName,
       const std::string &pDescription = "Please document me.",
       options<option> &pOpts = options<option, hint>::common())
      : option("-{0,2}" + pName + "[:=](.*)",
               [this](std::smatch &m) -> bool {
                 value = m[1];
                 return true;
               },
               "[String] " + pDescription, pOpts),
        value("") {}

  operator const std::string(void) const { return value; }

 protected:
  std::string value;
};

class hint {
 public:
  hint(const std::string &pTitle, std::function<std::string(void)> pUsage,
       options<option> &pOpts = options<option, hint>::common())
      : title(pTitle), usage(pUsage), opts(pOpts) {
    opts.add(*this);
  }

  ~hint(void) { opts.remove(*this); }

  const std::string title;
  std::function<std::string(void)> usage;

 protected:
  options<option, hint> &opts;
};

static option help("-{0,2}help",
                   [](std::smatch &m) -> bool {
                     return options<>::common().usage() == 0;
                   },
                   "Print this help screen.");
}
}

#endif
