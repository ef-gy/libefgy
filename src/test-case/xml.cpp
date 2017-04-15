/**\file
 * \brief Test cases for the XML render output
 *
 * The test cases in this file test the functionality of XML renderer.
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

#include <ef.gy/render-xml.h>
#include <ef.gy/test-case.h>

using namespace efgy::render;
using namespace efgy;

/**\brief XML output tests
 * \test Uses some functions of the geometry factory to verify that it compiles
 *       and works as intended.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testXMLoutput(std::ostream &log) {
  log << xml::tag() << xml::precision(16);

  return 0;
}

TEST_BATCH(testXMLoutput)
