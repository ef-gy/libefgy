/* Test cases the geometry factory
 *
 * The test cases in this file test the functionality of the geometry factory.
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

#include <ef.gy/factory.h>
#include <ef.gy/test-case.h>

#include <iostream>

using namespace efgy::geometry;
using namespace efgy::math;

/* Geometry factory tests
 * @log Where to write log messages to.
 *
 * Uses some functions of the geometry factory to verify that it compiles and
 * works as intended.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testGeometryFactory(std::ostream &log) {
  std::cerr << "\n";
  model<float, functor::echo, cube, 7, 7, format::cartesian>::with(
      log, 5, 0, format::cartesian());

  std::set<const char *> models;
  models = with<float, functor::models, 7>(models, "*", 0, 0);

  bool haveCube = false;

  for (std::string m : models) {
    haveCube || (haveCube = (m == "cube"));
    std::cerr << m << "\n";
  }

  if (!haveCube) {
    log << "no cube model\n";
    return false;
  }

  return true;
}

namespace test {
using efgy::test::function;

static function geometryFactory(testGeometryFactory);
}  // namespace test
