/* Test cases for trigonometric functions.
 *
 * Contains test cases that test libefgy's trigonometric.h and the functions
 * therein.
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

#include <ef.gy/fractions.h>
#include <ef.gy/range.h>
#include <ef.gy/test-case.h>
#include <ef.gy/trigonometric.h>

using namespace efgy::math;
using efgy::range;

/* Tests sines
 * @log Where to write log messages to.
 *
 * Calculates sines using libefgy's sine() function and compares the
 * results with reference data.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testSine(std::ostream &log) {
  double s, c;

  for (double x : range<double>(0, 3.14, 30, true)) {
    s = sines(x, c);

    log << "sine(" << x << ") = " << s << ":" << c << "\n";
  }

  return true;
}

namespace test {
using efgy::test::function;

static function sine(testSine);
}
