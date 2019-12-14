/*  Test cases for continued fractions
 *
 * The test cases in this file test conversions to and from continued fractions
 * as well as basic arithmetic with those continued fractions.
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

#include <ef.gy/continued-fractions.h>
#include <ef.gy/fractions.h>
#include <ef.gy/test-case.h>

#include <iostream>

using namespace efgy::math;
using std::string;

/* Continued fraction arithmetic tests
 * @log A stream for test cases to log messages to.
 *
 * Initialises a few regular fractions and a few continued fractions, then
 * performs arithmetic with both of them and compares the results.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testContinuedFractionArithmetic(std::ostream &log) {
  numeric::fractional<long long> af(6, 11), bf(4, 5), rf;
  numeric::continuedFractional<long long> a(af), b(bf), r;

  rf = af + bf;
  log << af << " + " << bf << " = " << rf << "\n";

  r = a + b;
  rf = r;
  log << a << " + " << b << " = " << r << " = " << rf << "\n";

  if (rf != r) {
    return false;
  }

  rf = af - bf;
  log << af << " - " << bf << " = " << rf << "\n";

  r = a - b;
  rf = r;
  log << a << " - " << b << " = " << r << " = " << rf << "\n";

  if (rf != r) {
    return false;
  }

  rf = af * bf;
  log << af << " * " << bf << " = " << rf << "\n";

  r = a * b;
  rf = r;
  log << a << " * " << b << " = " << r << " = " << rf << "\n";

  if (rf != r) {
    return false;
  }

  rf = af / bf;
  log << af << " / " << bf << " = " << rf << "\n";

  r = a / b;
  rf = r;
  log << a << " / " << b << " = " << r << " = " << rf << "\n";

  if (rf != r) {
    return false;
  }

  return true;
}

namespace test {
using efgy::test::function;

static function continuedFractionArithmetic(testContinuedFractionArithmetic);
}  // namespace test
