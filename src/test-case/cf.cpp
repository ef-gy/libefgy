/**\file
 * \brief Test cases for continued fractions
 *
 * The test cases in this file test conversions to and from continued fractions
 * as well as basic arithmetic with those continued fractions.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#include <iostream>

#include <ef.gy/continued-fractions.h>
#include <ef.gy/fractions.h>
#include <ef.gy/test-case.h>

using namespace efgy::math;
using std::string;

/**\brief Continued fraction arithmetic tests
 * \test Initialises a few regular fractions and a few continued fractions, then
 *       performs arithmetic with both of them and compares the results.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testContinuedFractionArithmetic(std::ostream &log) {
  numeric::fractional<long long> af(6, 11), bf(4, 5), rf;
  numeric::continuedFractional<long long> a(af), b(bf), r;

  rf = af + bf;
  log << af << " + " << bf << " = " << rf << "\n";

  r = a + b;
  rf = r;
  log << a << " + " << b << " = " << r << " = " << rf << "\n";

  if (rf != r) {
    return 1;
  }

  rf = af - bf;
  log << af << " - " << bf << " = " << rf << "\n";

  r = a - b;
  rf = r;
  log << a << " - " << b << " = " << r << " = " << rf << "\n";

  if (rf != r) {
    return 2;
  }

  rf = af * bf;
  log << af << " * " << bf << " = " << rf << "\n";

  r = a * b;
  rf = r;
  log << a << " * " << b << " = " << r << " = " << rf << "\n";

  if (rf != r) {
    return 3;
  }

  rf = af / bf;
  log << af << " / " << bf << " = " << rf << "\n";

  r = a / b;
  rf = r;
  log << a << " / " << b << " = " << r << " = " << rf << "\n";

  if (rf != r) {
    return 4;
  }

  return 0;
}

TEST_BATCH(testContinuedFractionArithmetic)
