/* Test cases for the polytope iterators
 *
 * Test cases in this file test polytope iterator properties for basic and IFS
 * polytopes.
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

#include <ef.gy/ifs.h>
#include <ef.gy/parametric.h>
#include <ef.gy/polytope.h>
#include <ef.gy/test-case.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

using namespace efgy;

/* Test case for the polytope iterators.
 * @C The geometric primitive.
 * @lim Maximum number of iterations.
 * @min Minimum number of iterations.
 * @log A stream for test cases to log messages to.
 *
 * This is a plain compile-and-iterate test, to make sure it doesn't
 * explode and that there's no infinite loops.
 *
 * @return 'true' on success, 'false' otherwise.
 */
template <class C, long lim = 10000, long min = 1>
bool testPolytopeIteratorNotInfinite(std::ostream &log) {
  auto params = geometry::parameters<float>();
  std::ostringstream os("");

  auto p = C(params, typename C::format());
  long c = 0;

  log << "iterating through '" << p.id() << "': ";

  for (auto &f : p) {
    auto t = f;
    if (c > lim) {
      log << "suspect infinite loop while iterating through a '" << p.id()
          << "': more than '" << lim << "' iterations.\n";
      return false;
    }
    os << "vector: [";
    for (auto &n : t) {
      os << " " << n;
    }
    os << " ]\n";
    c++;
  }

  if (c < min) {
    log << "not enough iterations while going through a '" << p.id()
        << "': want at least '" << min << "', but did: '" << c << "'\n";
    return false;
  }

  if (c != p.size()) {
    log << "wrong size() result for object type '" << p.id()
        << "'; object said it would have " << p.size()
        << " elements, but iterator gave us " << c << " elements.\n";
    return false;
  }

  return true;
}

namespace test {
using efgy::test::function;

static function t1(
    testPolytopeIteratorNotInfinite<geometry::cube<float, 2>, 1, 1>);
static function t2(
    testPolytopeIteratorNotInfinite<geometry::cube<float, 3>, 6, 6>);
static function t3(
    testPolytopeIteratorNotInfinite<geometry::cube<float, 4>, 24, 24>);
static function t4(
    testPolytopeIteratorNotInfinite<geometry::cube<float, 5>, 80, 80>);
static function t5(
    testPolytopeIteratorNotInfinite<geometry::cube<float, 6>, 240, 240>);
static function t6(
    testPolytopeIteratorNotInfinite<geometry::cube<float, 7>, 672, 672>);
static function t7(
    testPolytopeIteratorNotInfinite<geometry::cube<float, 8>, 1792, 1792>);
static function t8(
    testPolytopeIteratorNotInfinite<geometry::cube<float, 9>, 4608, 4608>);

static function t9(testPolytopeIteratorNotInfinite<geometry::plane<float, 2>>);
static function t10(testPolytopeIteratorNotInfinite<geometry::plane<float, 3>>);
static function t11(testPolytopeIteratorNotInfinite<geometry::plane<float, 4>>);

static function t12(
    testPolytopeIteratorNotInfinite<geometry::sierpinski::gasket<float, 2>>);

static function t13(
    testPolytopeIteratorNotInfinite<geometry::sierpinski::gasket<float, 3>>);

static function t14(testPolytopeIteratorNotInfinite<
                    geometry::sierpinski::gasket<float, 4>, 1000000>);

static function t15(
    testPolytopeIteratorNotInfinite<
        geometry::adapt<float, 3, geometry::sierpinski::gasket<float, 2>,
                        math::format::cartesian>>);

static function t16(
    testPolytopeIteratorNotInfinite<
        geometry::adapt<float, 5, geometry::sierpinski::gasket<float, 3>,
                        math::format::cartesian>>);
}  // namespace test
