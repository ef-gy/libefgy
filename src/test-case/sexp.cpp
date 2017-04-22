/* Test cases for s-expression parsing
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

#include <iostream>
#include <sstream>

#include <ef.gy/sexp.h>
#include <ef.gy/test-case.h>

using namespace efgy::test;
using efgy::atom;
using efgy::cons;

/* Tests construction of s-expressions
 * @log Where to write log messages to.
 *
 * Constructs an atomic s-expression and a cons s-expression
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testConstruction(std::ostream &log) {
  atom<int> a(23);

  if (a.data != 23) {
    log << "Expected data value: 23. Actual value: " << a.data;
    return false;
  }

  cons<int, int> c(23, 42);
  if (c.car != 23) {
    log << "Expected value of car(cons(23, 42)): 23. Actual value: " << c.car;
    return false;
  }
  if (*c.cdr != 42) {
    log << "Expected value of cdr(cons(23, 42)): 42. Actual value: " << *c.cdr;
    return false;
  }

  return true;
}

/* Tests stream output of s-expressions
 * @log Where to write log messages to.
 *
 * Constructs atomic and cons s-expressions and inserts them into a
 * std::ostringstream.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testStreamOutput(std::ostream &log) {
  atom<int> a(23);
  cons<int, int> c(23, 42);

  std::ostringstream test;
  test << a;
  test << "\n" << c;

  std::string expected = "23\n(23 . 42)";
  if (test.str() != expected) {
    log << "Detected error in stream output. Expected: " << expected;
    log << "\nActual: " << test.str();
    return false;
  } else {
    return true;
  }
}

namespace test {
using efgy::test::function;

static function construction(testConstruction);
static function streamOutput(testStreamOutput);
}
