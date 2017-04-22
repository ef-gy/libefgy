/* Test cases for the numeric tracer
 *
 * Test cases in this file verify that the code in the tracer.h header works
 * properly.
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
#include <string>

#include <ef.gy/matrix.h>
#include <ef.gy/test-case.h>
#include <ef.gy/tracer.h>
#include <algorithm>

using namespace efgy::math::tracer;
using efgy::math::matrix;

/* Test case for the numeric tracer
 * @log Where to write log messages to.
 *
 * Uses the numeric tracer to create a parse tree, then compares that to
 * sample strings.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testTracer(std::ostream &log) {
  std::shared_ptr<tracer<int, void, 0>> frobrt(tracer<int, void, 0>(1));
  runtime frobrt2(new tracer<int, void, 0>(1));
  runtime frobrt3(frobrt);

  auto frobd = frobrt + 2;
  runtime frobde = frobrt + 3;

  frobde /= frobd;

  log << frobd << "\n";
  log << frobde << "\n";

  return true;
}

/* Test case for the numeric tracer with matrix operations
 * @log Where to write log messages to.
 *
 * Uses the numeric tracer to create a parse tree, then compare that to
 * sample strings, much like testTracer(). Uses matrix manipulations to
 * see if the calculations work with slightly more complex functions.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testTracerMatrix(std::ostream &log) {
  matrix<runtime, 3, 3> m1;
  matrix<runtime, 3, 3> m2;

  for (unsigned int i = 0; i < 3; i++) {
    for (unsigned int j = 0; j < 3; j++) {
      std::stringstream s("");
      s << "_" << i << "_" << j;

      m1[i][j] = std::shared_ptr<tracer<void, void, 0>>(
          new tracer<void, void, 0>("a" + s.str()));
      m2[i][j] = std::shared_ptr<tracer<void, void, 0>>(
          new tracer<void, void, 0>("b" + s.str()));
    }
  }

  matrix<runtime, 3, 3> m3 = m1 * m2;

  for (unsigned int i = 0; i < 3; i++) {
    for (unsigned int j = 0; j < 3; j++) {
      log << "result"
          << "_" << i << "_" << j << " = " << m3[i][j] << "\n";
    }
  }

  return true;
}

namespace test {
using efgy::test::function;

static function tracer(testTracer);
static function tracerMatrix(testTracerMatrix);
}
