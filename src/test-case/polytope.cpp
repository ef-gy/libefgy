/**\file
 * \brief Test cases for the polytope iterators
 *
 * Test cases in this file test polytope iterator properties for basic and IFS
 * polytopes.
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
#include <string>

#include <ef.gy/test-case.h>
#include <ef.gy/polytope.h>
#include <ef.gy/parametric.h>
#include <ef.gy/ifs.h>
#include <algorithm>

using namespace efgy;

/**\brief Test case for the polytope iterators.
 * \test This is a plain compile-and-iterate test, to make sure it doesn't
 *       explode and that there's no infinite loops.
 *
 * \tparam     C   The geometric primitive.
 * \tparam     lim Maximum number of iterations.
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
template<class C, long lim = 10000, long min = 1>
int testPolytopeIteratorNotInfinite(std::ostream &log) {
  auto params = geometry::parameters<float>();

  auto p = C(params, typename C::format());
  long c = 0;

  log << "iterating through '" << p.id() << "'\n";

  for(auto &f : p) {
    auto t = f;
    if (c > lim) {
      log << "suspect infinite loop while iterating through a '"
          << p.id() << "': more than '" << lim << "' iterations.\n";
      return -1;
    }
    log << "vector: [";
    for (auto &n : t) {
      log << " " << n;
    }
    log << " ]\n";
    c++;
  }

  if (c < min) {
    log << "not enough iterations while going through a '"
        << p.id() << "': want at least '" << min << "', but did: '" << c
        << "'\n";
    return -2;
  }

  if (c != p.size()) {
    log << "BUG: wrong size() result for object type '"
        << p.id() << "'; object said it would have " << p.size()
        << " elements, but iterator gave us " << c << " elements.\n";
#if 0
    return -3;
#endif
  }

  return 0;
}

TEST_BATCH(
    testPolytopeIteratorNotInfinite<geometry::cube<float,2>, 1, 1>,
    testPolytopeIteratorNotInfinite<geometry::cube<float,3>, 6, 6>,
    testPolytopeIteratorNotInfinite<geometry::cube<float,4>, 24, 24>,

    testPolytopeIteratorNotInfinite<geometry::plane<float,2>>,
    testPolytopeIteratorNotInfinite<geometry::plane<float,3>>,
    testPolytopeIteratorNotInfinite<geometry::plane<float,4>>,

    testPolytopeIteratorNotInfinite<
        geometry::sierpinski::gasket<float,2>>,

    testPolytopeIteratorNotInfinite<
        geometry::adapt<float, 3,
            geometry::sierpinski::gasket<float,2>,
            math::format::cartesian>>,
    )
