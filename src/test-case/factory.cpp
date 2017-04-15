/**\file
 * \brief Test cases the geometry factory
 *
 * The test cases in this file test the functionality of the geometry factory.
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

#include <ef.gy/factory.h>
#include <ef.gy/test-case.h>

using namespace efgy::geometry;
using namespace efgy::math;

/**\brief Geometry factory tests
 * \test Uses some functions of the geometry factory to verify that it compiles
 *       and works as intended.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testGeometryFactory(std::ostream &log) {
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
    return 1;
  }

  return 0;
}

TEST_BATCH(testGeometryFactory)
