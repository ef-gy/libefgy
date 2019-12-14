/* Test cases for the 'exponentiate' namespace
 *
 * Test whether things in the efgy::math::exponentiate namespace work as
 * intended.
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

#include <ef.gy/exponential.h>
#include <ef.gy/fractions.h>
#include <ef.gy/test-case.h>

#include <iostream>

using namespace efgy::math;

/* Integer exponents
 * @log Where to write log messages to.
 *
 * Raises some numbers to integer powers and compares the results with reference
 * data to see if the exponentiate::integral template works correctly.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testIntegralExponents(std::ostream &log) {
  if (exponentiate::integral<fraction, 0>::raise(fraction(21, 5)) !=
      fraction(1, 1)) {
    log << "(21/5)^0 should be (1/1) but is "
        << exponentiate::integral<fraction, 0>::raise(fraction(21, 5)) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction, 1>::raise(fraction(21, 5)) !=
      fraction(21, 5)) {
    log << "(21/5)^1 should be (21/5) but is "
        << exponentiate::integral<fraction, 1>::raise(fraction(21, 5)) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction, 2>::raise(fraction(21, 5)) !=
      fraction(441, 25)) {
    log << "(21/5)^2 should be (441/25) but is "
        << exponentiate::integral<fraction, 2>::raise(fraction(21, 5)) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction, 3>::raise(fraction(21, 5)) !=
      fraction(9261, 125)) {
    log << "(21/5)^3 should be (9261/125) but is "
        << exponentiate::integral<fraction, 3>::raise(fraction(21, 5)) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction, 4>::raise(fraction(21, 5)) !=
      fraction(194481, 625)) {
    log << "(21/5)^4 should be (194481/625) but is "
        << exponentiate::integral<fraction, 4>::raise(fraction(21, 5)) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction, 5>::raise(fraction(21, 5)) !=
      fraction(4084101l, 3125)) {
    log << "(21/5)^5 should be (4084101/3125) but is "
        << exponentiate::integral<fraction, 5>::raise(fraction(21, 5)) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction, -1>::raise(fraction(21, 5)) !=
      fraction(5, 21)) {
    log << "(21/5)^(-1) should be (5/21) but is "
        << exponentiate::integral<fraction, -1>::raise(fraction(21, 5)) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction, -2>::raise(fraction(21, 5)) !=
      fraction(25, 441)) {
    log << "(21/5)^(-2) should be (25/441) but is "
        << exponentiate::integral<fraction, -2>::raise(fraction(21, 5)) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction, -3>::raise(fraction(21, 5)) !=
      fraction(125, 9261)) {
    log << "(21/5)^(-3) should be (125/9261) but is "
        << exponentiate::integral<fraction, -3>::raise(fraction(21, 5)) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction, -4>::raise(fraction(21, 5)) !=
      fraction(625, 194481)) {
    log << "(21/5)^(-4) should be (625/194481) but is "
        << exponentiate::integral<fraction, -4>::raise(fraction(21, 5)) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction, -5>::raise(fraction(21, 5)) !=
      fraction(3125, 4084101l)) {
    log << "(21/5)^(-5) should be (3125/4084101) but is "
        << exponentiate::integral<fraction, -5>::raise(fraction(21, 5)) << "\n";
    return false;
  }

  return true;
}

/* Integer exponents, function parameter base
 * @log Where to write log messages to.
 *
 * Same test procedure as testIntegralExponents, but this test cases uses the
 * two-parameter variant of the exponentiate::integral instead of specifying the
 * exponent as a template parameter.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testFunctionalIntegralExponents(std::ostream &log) {
  if (exponentiate::integral<fraction>::raise(fraction(21, 5), 0) !=
      fraction(1, 1)) {
    log << "(21/5)^0 should be (1/1) but is "
        << exponentiate::integral<fraction>::raise(fraction(21, 5), 0) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction>::raise(fraction(21, 5), 1) !=
      fraction(21, 5)) {
    log << "(21/5)^1 should be (21/5) but is "
        << exponentiate::integral<fraction>::raise(fraction(21, 5), 1) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction>::raise(fraction(21, 5), 2) !=
      fraction(441, 25)) {
    log << "(21/5)^2 should be (441/25) but is "
        << exponentiate::integral<fraction>::raise(fraction(21, 5), 2) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction>::raise(fraction(21, 5), 3) !=
      fraction(9261, 125)) {
    log << "(21/5)^3 should be (9261/125) but is "
        << exponentiate::integral<fraction>::raise(fraction(21, 5), 3) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction>::raise(fraction(21, 5), 4) !=
      fraction(194481, 625)) {
    log << "(21/5)^4 should be (194481/625) but is "
        << exponentiate::integral<fraction>::raise(fraction(21, 5), 4) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction>::raise(fraction(21, 5), 5) !=
      fraction(4084101l, 3125)) {
    log << "(21/5)^5 should be (4084101/3125) but is "
        << exponentiate::integral<fraction>::raise(fraction(21, 5), 5) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction>::raise(fraction(21, 5), -1) !=
      fraction(5, 21)) {
    log << "(21/5)^(-1) should be (5/21) but is "
        << exponentiate::integral<fraction>::raise(fraction(21, 5), -1) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction>::raise(fraction(21, 5), -2) !=
      fraction(25, 441)) {
    log << "(21/5)^(-2) should be (25/441) but is "
        << exponentiate::integral<fraction>::raise(fraction(21, 5), -2) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction>::raise(fraction(21, 5), -3) !=
      fraction(125, 9261)) {
    log << "(21/5)^(-3) should be (125/9261) but is "
        << exponentiate::integral<fraction>::raise(fraction(21, 5), -3) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction>::raise(fraction(21, 5), -4) !=
      fraction(625, 194481)) {
    log << "(21/5)^(-4) should be (625/194481) but is "
        << exponentiate::integral<fraction>::raise(fraction(21, 5), -4) << "\n";
    return false;
  }

  if (exponentiate::integral<fraction>::raise(fraction(21, 5), -5) !=
      fraction(3125, 4084101l)) {
    log << "(21/5)^(-5) should be (3125/4084101) but is "
        << exponentiate::integral<fraction>::raise(fraction(21, 5), -5) << "\n";
    return false;
  }

  return true;
}

namespace test {
using efgy::test::function;

static function integralExponents(testIntegralExponents);
static function functionalIntegralExponents(testFunctionalIntegralExponents);
}  // namespace test
