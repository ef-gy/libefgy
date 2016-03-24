/**\file
 * \brief Test cases for the 'optional' template
 *
 * This file contains test cases for the 'optional' template. std::optional
 * is coming up in the next iteration of the C++ standard (C++17) and is
 * currently available in clang when using std=c++14. Or, well, at least an
 * experimental version is.
 *
 * This should replace efgy::maybe, so I've copied the test cases for that and
 * changed them slightly so they work with std::optional.
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
#include <sstream>
#include <ef.gy/test-case.h>
#include <experimental/optional>

using namespace std;
using std::experimental::optional;
using efgy::test::next_integer;

/**\brief 'Optional' assignment and initialisation tests
 * \test Initialises several instances of the 'optional' template of different
 *       numeric types to test if they're working as expected.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testOptional(std::ostream &log) {
  optional<int> nothing;

  if (nothing) {
    log << "value should have been 'nothing'\n";
    return 1;
  }

  optional<int> something = 42;

  if (!something) {
    log << "value should have been something other than 'nothing'\n";
    return 2;
  }

  if (*something != 42) {
    log << "value should have been '43', but is '" << *something << "'\n";
    return 3;
  }

  something = nothing;

  if (something) {
    log << "'something' was changed to 'nothing', but still thinks "
           "otherwise.\n";
    return 4;
  }

  optional<double> somethingElse;

  if (somethingElse) {
    log << "value should have been 'nothing'\n";
    return 5;
  }

  somethingElse = optional<double>(42);

  if (!somethingElse) {
    log << "value should have been something other than 'nothing'\n";
    return 6;
  }

  if (*somethingElse != 42) {
    log << "value should have been '42', but is '" << *somethingElse
        << "'\n";
    return 7;
  }

  return 0;
}

/**\brief Nontrivial example class
 *
 * Used in testOptionalNontrivial to test whether the (constexpr) optional template
 * works as intended with a nontrivial class.
 */
class outputInDestructor {
public:
  /**\brief Initialise with output stream
   *
   * Initialises the class with an output stream, which is used in the
   * destructor.
   *
   * \param[in,out] pLog The stream to log to upon destruction of the
   *                     object
   */
  outputInDestructor(std::ostream &pLog) : log(pLog) {}

  /**\brief Destructor
   *
   * This destructor will log a message to the ostream passed to the
   * constructor in order to visualise when copies of an object are being
   * created an destroyed when using the optional template.
   */
  ~outputInDestructor(void) {
    log << "outputInDestructor::~outputInDestructor()\n";
  }

protected:
  /**\brief Output stream reference
   *
   * The output stream passed to the constructor; used in the destructor.
   */
  std::ostream &log;
};

/**\brief 'Optional' usage with nontrivial types test
 * \test Initialises a optional with a nontrivial type to see if it behaves as
 *       expected. This test case also demonstrates how and when the contained
 *       objects are copied or re-initialised.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testOptionalNontrivial(std::ostream &log) {
  // optional<outputInDestructor> nontrivial; // should not compile

  optional<outputInDestructor> nontrivial((outputInDestructor(log)));

  const outputInDestructor &dest = *nontrivial;

  return 0;
}

TEST_BATCH(testOptional, testOptionalNontrivial)
