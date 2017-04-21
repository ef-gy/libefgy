/**\file
 * \brief Test cases to analyse cube mask properties
 *
 * mask::cube<> produces a bit mask for hypercubes. This programme examines some
 * of these, in the hopes of identifying a better algorithm to create them, as
 * almost all other meshes depend on hypercubes.
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

#include <ef.gy/polytope.h>
#include <ef.gy/test-case.h>
#include <algorithm>

using namespace efgy;

/**\brief Analyse cube mask properties.
 * \test While technically not a test case proper, this does exercise the cube
 *     mask generator and will sanity-check some of its' output.
 *
 * \tparam C The cube mask, with parameters applied.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
template <class C>
int analyseCubeMaskProperties(std::ostream &log) {
  auto faces = C::faces();

  long c = 0;

  for (auto &f : faces) {
    std::cerr << "\n[";
    for (auto v : f) {
      log << "  (";
      for (auto w : v) {
        log << " " << w;
      }
      log << " )";
    }
    log << " ]";

    auto t = f;
    c++;
  }

  if (C::size() != faces.size()) {
    log << "wrong size() result; object said it would have " << C::size()
        << " elements, but iterator gave us " << c << " elements.\n";
    return -1;
  }

  return 0;
}

namespace test {
using efgy::test::function;

static function cube1(
    analyseCubeMaskProperties<geometry::generators::mask::cube<1>>);
static function cube2(
    analyseCubeMaskProperties<geometry::generators::mask::cube<2>>);
static function cube3(
    analyseCubeMaskProperties<geometry::generators::mask::cube<3>>);
static function cube4(
    analyseCubeMaskProperties<geometry::generators::mask::cube<4>>);
static function cube5(
    analyseCubeMaskProperties<geometry::generators::mask::cube<5>>);
}
