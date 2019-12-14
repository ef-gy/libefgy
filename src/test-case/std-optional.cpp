/* Test cases for the 'optional' template
 *
 * This file contains test cases for the 'optional' template. std::optional
 * is coming up in the next iteration of the C++ standard (C++17) and is
 * currently available in clang when using std=c++14. Or, well, at least an
 * experimental version is.
 *
 * This should replace efgy::maybe, so I've copied the test cases for that and
 * changed them slightly so they work with std::optional.
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

#include <ef.gy/test-case.h>

#include <iostream>
#include <optional>
#include <sstream>

using namespace std;

/* 'Optional' assignment and initialisation tests
 * @log Where to write log messages to.
 *
 * Initialises several instances of the 'optional' template of different numeric
 * types to test if they're working as expected.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testOptional(std::ostream &log) {
  optional<int> nothing;

  if (nothing) {
    log << "value should have been 'nothing'\n";
    return false;
  }

  optional<int> something = 42;

  if (!something) {
    log << "value should have been something other than 'nothing'\n";
    return false;
  }

  if (*something != 42) {
    log << "value should have been '43', but is '" << *something << "'\n";
    return false;
  }

  something = nothing;

  if (something) {
    log << "'something' was changed to 'nothing', but still thinks "
           "otherwise.\n";
    return false;
  }

  optional<double> somethingElse;

  if (somethingElse) {
    log << "value should have been 'nothing'\n";
    return false;
  }

  somethingElse = optional<double>(42);

  if (!somethingElse) {
    log << "value should have been something other than 'nothing'\n";
    return false;
  }

  if (*somethingElse != 42) {
    log << "value should have been '42', but is '" << *somethingElse << "'\n";
    return false;
  }

  return true;
}

namespace test {
using efgy::test::function;

static function optional(testOptional);
}  // namespace test
