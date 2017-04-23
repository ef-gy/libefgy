/* CLI argument handling.
 *
 * This file provides regex-based command line argument matching and a few
 * default command line argument types to go along with that.
 *
 * Example usage:
 *
 *     static efgy::cli::flag<std::string> example("some-flag");
 *
 *     int main(int argc, char **argv) {
 *       efgy::cli::options opts(argc, argv);
 *       // the options are now parsed, and you're free to use them as you
 *       // see fit.
 *       if (std::string(example) == "foo") {
 *         // do frobulations.
 *       }
 *       return 0;
 *     }
 *
 * See also:
 * * Project Documentation: https://ef.gy/documentation/libefgy
 * * Project Source Code: https://github.com/ef-gy/libefgy
 * * Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 *
 * @copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 */

#if !defined(EF_GY_CLI_H)
#define EF_GY_CLI_H

#include <functional>
#include <iostream>
#include <regex>
#include <set>
#include <string>

#include <ef.gy/global.h>

namespace efgy {
namespace cli {
/* Command line argument processor.
 * @option The type for command line options.
 * @hint The type for command line usage hints.
 *
 * Allows parsing of command line functions by declaring options wherever
 * appropriate and then processing all of them through this function. The nice
 * thing about this, is that this way any libraries that are linked in will be
 * able to define their own arguments.
 *
 * As a case in point, see the declaration of the 'help' argument, which prints
 * usage information to stdout, below.
 */
template <typename option, typename hint>
class processor {
 public:
  /* Default constructor
   * @pOpts Options that would be applied to arguments.
   * @pHints Hints for the usage summary.
   *
   * Initialises everything to default values, i.e. no matches and no remainder.
   * Arguments are not applied.
   */
  processor(std::set<option*> &pOpts = global<std::set<option*>>(),
            std::set<hint*> &pHints = global<std::set<hint*>>())
      : showUsage(false),
        matches(0),
        remainder({}),
        opts(pOpts),
        hints(pHints) {}

  /* Construct with argument vector.
   * @args Argument vector.
   * @pShowUsage Whether to show the usage summary if nothing matches.
   * @pOpts Options that would be applied to arguments.
   * @pHints Hints for the usage summary.
   *
   * Arguments are applied to the argument vector described in 'args'. This
   * popualtes the 'matches' and 'remainder' fields.
   */
  processor(const std::vector<std::string> &args, bool pShowUsage = true,
            std::set<option*> &pOpts = global<std::set<option*>>(),
            std::set<hint*> &pHints = global<std::set<hint*>>())
      : showUsage(pShowUsage),
        matches(0),
        remainder({}),
        opts(pOpts),
        hints(pHints) {
    apply(args);
  }

  /* Construct with C-style arguments.
   * @argc Argument count.
   * @argv Argument vector.
   * @pShowUsage Whether to show the usage summary if nothing matches.
   * @pOpts Options that would be applied to arguments.
   * @pHints Hints for the usage summary.
   *
   * Arguments are applied to the argument vector described in 'argv'. This
   * popualtes the 'matches' and 'remainder' fields.
   *
   * This constructor is provided for convenience, and may be used in the
   * programme's main() function.
   */
  processor(int argc, char *argv[], bool pShowUsage = true,
            std::set<option*> &pOpts = global<std::set<option*>>(),
            std::set<hint*> &pHints = global<std::set<hint*>>())
      : showUsage(pShowUsage),
        matches(0),
        remainder({}),
        opts(pOpts),
        hints(pHints) {
    apply(argc, argv);
  }

  /* Number of matched arguments. */
  std::size_t matches;

  /* Remaining, i.e. unmatched arguments. */
  std::vector<std::string> remainder;

  /* Display usage summary.
   * @out Where to put the usage summary. Should be std:cout.
   * @name The name of the programme.
   *
   * This prints a list of valid command line options, based on the 'opts' and
   * 'hints' referenced in this class.
   */
  void usage(std::ostream &out, const std::string &name = "<command>") {
    out << "Usage: " << name << " [options...] [--] [input...]\n"
        << "\n"
        << "Where [options...] is any of the following:\n";

    for (const auto &opt : opts) {
      out << "  " << opt->regex;
      if (opt->regex.size() < 25) {
        for (std::size_t i = opt->regex.size() + 1; i < 26; i++) {
          out << " ";
        }
      } else {
        out << "\n                           ";
      }
      out << opt->description << "\n";
    }

    for (const auto &hn : hints) {
      out << "\n" << hn->title << ":\n" << hn->usage();
    }
  }

 protected:
  /* Apply command line specs to given arguments.
   * @args The incoming command line arguments.
   *
   * The number of matches will end up in 'matches' and unmatched arguments are
   * put in 'remainder'.
   */
  void apply(const std::vector<std::string> &args) {
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
          std::smatch match;
          if (std::regex_match(arg, match, opt->match) && opt->handler(match)) {
            matches++;
            haveMatch = true;
          }
        }
      }

      if (!haveMatch && !firstArg) {
        remainder.push_back(arg);
      }

      firstArg = false;
    }

    if (showUsage && (matches == 0) && (args.size() > 0) &&
        (remainder.size() == 0)) {
      usage(std::cout, args[0]);
    }
  }

  /* Apply command line specs to given arguments.
   * @argc The number of command line arguments.
   * @argv The actual command line arguments to match.
   *
   * This is a wrapper around the vector form of 'apply'. This wrapper is useful
   * in the main function of the programme, which receives arguments in the
   * argc/argv form.
   */
  void apply(int argc, char *argv[]) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++) {
      args.push_back(argv[i]);
    }
    apply(args);
  }

  /* Whether to show usage information if there are no matches. */
  const bool showUsage;

  /* Command line option specifications to use. */
  std::set<option*> &opts;

  /* Command line usage hints to print after the summary. */
  std::set<hint*> &hints;
};

/* Command line option.
 *
 * Any instance of this class represents a single command line option, which is
 * registered with an options registry. By default this is with the global
 * instance of that registry.
 */
class option {
 public:
  /* Construct with regex and handling function.
   * @pMatch The regex to match arguments against.
   * @pHandler The handler to call if the regex matches.
   * @pDescription A textual description of what the argument does.
   * @pOpts Which options set to register with.
   *
   * The basic CLI options allow for matching any individual option against a
   * regex, and if that should match then the given handler function is called.
   *
   * This is sufficient to implement any higher order types, e.g. booleans or
   * strings.
   */
  option(const std::string &pMatch, std::function<bool(std::smatch &)> pHandler,
         const std::string &pDescription = "please document me",
         std::set<option*> &pOpts = global<std::set<option*>>())
      : regex(pMatch),
        match(pMatch),
        handler(pHandler),
        description(pDescription),
        root(pOpts) {
    root.insert(this);
  }

  /* Destructor.
   *
   * Removes the option from the registry it's been registered with. This
   * implies you can have options defined in a function, and when they go out of
   * scope they will clean up after themselves.
   */
  ~option(void) { root.erase(this); }

  /* Original regex string.
   *
   * The regex is compiled into the 'match' member, so this is a copy of the
   * original so it can be printed elsewhere.
   */
  const std::string regex;

  /* CLI argument description.
   *
   * A free text description of what the argument does. In combination with the
   * regex string, this should describe how to use the argument and what effect
   * it will have.
   */
  const std::string description;

  /* Compiled argument regex.
   *
   * The compiled form of 'regex', which is used when applying arguments.
   * Compiling this regex once provides minor speed advantages.
   */
  const std::regex match;

  /* CLI argument handler function.
   *
   * Called if the provided regex matches a command line argument. The one
   * parameter that this handler gets is the regex capture vector, which should
   * be sufficient to set whatever variables need setting.
   *
   * The return value of the function should indicate whether the match was
   * actually successful; if it's set to false, the argument is processed
   * further, otherwise it's discarded as 'done'.
   */
  const std::function<bool(std::smatch &)> handler;

 protected:
  /* CLI option registry.
   *
   * Where the option was registered. Only used in the destructor, to clean up
   * by unregistering from there.
   */
  std::set<option*> &root;
};

/* A boolean CLI flag.
 * @type The flag's C++ type. A boolean in this case.
 *
 * Can either be 'yes' or no. The name you give is used as the flag name. If
 * you use --name on the command line, that sets the flag, and if you use
 * --no-name, then that unsets it. The regex is slightly more lenient than that,
 * even.
 *
 * Due to how this translates to a regex, you can probably create interesting
 * effect depending on the name you pick.
 */
template <typename type = bool>
class flag : public option {
 public:
  /* Construct with name.
   * @pName The name for the boolean CLI flag.
   * @pDescription A textual description of what the flag does.
   * @pOpts The option registry to register with.
   *
   * The name you give the flag is used as part of a regex, so keep it simple.
   */
  flag(const std::string &pName,
       const std::string &pDescription = "please document me",
       std::set<option*> &pOpts = global<std::set<option*>>())
      : option("-{0,2}((no)-?)?" + pName,
               [this](std::smatch &m) -> bool {
                 value = m[2] != "no";
                 return true;
               },
               "[bool] " + pDescription, pOpts),
        value(false) {}

  /* Cast to a boolean.
   *
   * This allows accessing the value of this option. The default is 'false',
   * unless it's been set at the command line.
   *
   * @return 'value', but forced to be const.
   */
  operator const bool(void) const { return value; }

 protected:
  /* Raw value.
   *
   * Don't use this. Use the provided type cast operator instead.
   */
  bool value;
};

/* A string CLI flag.
 *
 * Exactly what it says on the tin. The regex that this resolves to allows for
 * the following syntax variants:
 *
 *     --name=foo
 *     --name:foo
 *
 * Dashes are optional, and you can either use -name or --name. It's not
 * possible to split this into two arguments, however, e.g. "--name foo" is not
 * a valid way to set the flag.
 */
template <>
class flag<std::string> : public option {
 public:
  /* Construct with name.
   * @pName The name for the string CLI flag.
   * @pDescription A textual description of what the flag does.
   * @pOpts The option registry to register with.
   *
   * The name you give the flag is used as part of a regex, so keep it simple.
   */
  flag(const std::string &pName,
       const std::string &pDescription = "please document me",
       std::set<option*> &pOpts = global<std::set<option*>>())
      : option("-{0,2}" + pName + "[:=](.*)",
               [this](std::smatch &m) -> bool {
                 value = m[1];
                 return true;
               },
               "[string] " + pDescription, pOpts),
        value("") {}

  /* Cast to a string.
   *
   * This allows accessing the value of this option. The default is empty,
   * unless it's been set at the command line.
   *
   * @return 'value', but forced to be const.
   */
  operator const std::string(void) const { return value; }

 protected:
  /* Raw value.
   *
   * Don't use this. Use the provided type cast operator instead.
   */
  std::string value;
};

/* Additional CLI usage hints.
 *
 * This class provides additional usage hints to users of your programme. A
 * typical use case would be something that needs documenting at a programme
 * level and should be displayed along with CLI options on the help screen, or
 * usage summary.
 *
 * Think of a summary of servlets built into a network server. In cases like
 * that, your users would want to know what's actually built into a binary that
 * they'd want to run.
 */
class hint {
 public:
  /* Construct with title and usage function.
   * @pTitle A title governing the hints provided by this.
   * @pUsage Usage function to generate the hints.
   * @pItems The set of items to register with.
   *
   * The usage summary is generated by a callback function, so that it's
   * possible for that function to perform some sort of data gathering in the
   * binary.
   */
  hint(const std::string &pTitle, std::function<std::string(void)> pUsage,
       std::set<hint*> &pItems = global<std::set<hint*>>())
      : title(pTitle), usage(pUsage), root(pItems) {
    root.insert(this);
  }

  /* Destructor.
   *
   * Unregisters from the registry that was used.
   */
  ~hint(void) { root.erase(this); }

  /* Title of the hint.
   *
   * Every good piece of information should have a title.
   */
  const std::string title;

  /* Usage hint generator.
   *
   * Your function should return a std::string with your usage summary.
   */
  std::function<std::string(void)> usage;

 protected:
  /* CLI hint registry.
   *
   * Where the hint was registered. Only used in the destructor, to clean up by
   * unregistering from there.
   */
  std::set<hint*> &root;
};

/* Default CLI argument processor class.
 *
 * This is a type alias for the default CLI argument processor, and the default
 * hints those would have. Mostly for convenience, because long types are
 * unwieldy.
 */
using options = processor<option, hint>;

/* CLI option to display all usage information.
 *
 * When triggered, this will call the usage() function for the default set of
 * options and write all of that to std::cout.
 */
static option help("-{0,2}help",
                   [](std::smatch &m) -> bool {
                     options().usage(std::cout);
                     return true;
                   },
                   "print this help screen");
}
}

#endif
