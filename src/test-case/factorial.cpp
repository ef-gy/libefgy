/* Test cases for the factorial class template
 *
 * This file contains test cases for the math::factorial template, which is
 * used to calculate the factorial of a number.
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

#include <ef.gy/factorial.h>
#include <ef.gy/test-case.h>

#include <iostream>

using namespace efgy;
using namespace efgy::math;
using namespace std;

/* Factorial tests
 * @log Where to write log messages to.
 *
 * Calculates the factorial of a few numbers and compares those to the
 * values that the template ought to return.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testFactorial(ostream &log) {
  if (factorial<int>(0) != 1) {
    log << "Factorial of 0 should be 1.";
    return false;
  }

  if (factorial<int>(1) != 1) {
    log << "Factorial of 1 should be 1.";
    return false;
  }

  if (factorial<int>(5) != 120) {
    log << "Factorial of 5 should be 120.";
    return false;
  }

  return true;
}

namespace test {
using efgy::test::function;

static function factorial(testFactorial);
}  // namespace test
