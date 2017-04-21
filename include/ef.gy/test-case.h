/**\file
 * \brief Common code for test cases
 *
 * Most of the test cases follow a very simple pattern, so this file implements
 * certain basic pieces of code that are shared by all test cases. To write
 * test cases, all that needs to be done is to include this header, define
 * appropriate test functions and to use the TEST_BATCH() macro to point out
 * which of the functions in the file are proper test functions.
 *
 * This latter macro will set everything up so that a default main() function
 * can be used that actually runs those tests. You then simply place this code
 * into a file named src/test-case/my-test-case.cpp, and then it gets picked up
 * by 'make test' automagically.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

/**\dir src/test-case
 * \brief Library test cases
 *
 * Library test cases all go in the src/test-case/ directory. The makefile will
 * automatically build them, and you can run them in a batch by using the
 * make target 'test', like so:
 *
 * \code
 * $ make test
 * \endcode
 *
 * If any of the test cases fail to run then the make command will fail with an
 * error. You could use this to run the test cases automatically after a
 * checkout, for example.
 */

#include <iostream>
#include <vector>

namespace efgy {
/**\brief Functions and types related to test cases
 *
 * This namespace contains functions and types that make defining automatic
 * unit tests easier.
 */
namespace test {
/**\brief Test case function pointer
 *
 * All test cases in a batch are added to a vector to run, so it's
 * important to get their function prototypes right. This typedef
 * reflects the expected function prototype: return an int and take
 * an ostream.
 */
typedef int (*testCase)(std::ostream &log);

/**\brief Run test case batch
 *
 * Runs all test cases specified in the testCases vector. This produces
 * output on stderr about the progress of running these test cases, and
 * the individual test cases are instructed to output any log messages
 * to stderr as well.
 *
 * The test cases are run in the sequence specified by the testCases
 * vector until either all of the test cases ran successfully, or until
 * one of them fails, in which case execution stops and the function
 * returns whatever that failed test case returned.
 *
 * \param[in] testCases The test cases to run.
 *
 * \return 0 if all test cases ran successfully, the return value of
 *         the first test that failed otherwise.
 */
int run(int, char **, const std::vector<testCase> &testCases) {
  try {
    for (std::vector<testCase>::const_iterator it = testCases.begin();
         it != testCases.end(); it++) {
      std::cerr << "running test case "
                << (std::distance(testCases.begin(), it) + 1) << " in batch of "
                << testCases.size() << ": ";
      int res = (*it)(std::cerr);
      if (res != 0) {
        std::cerr << "failed; code: " << res << "\n";
        return res;
      }
      std::cerr << "OK\n";
    }
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
    return -1;
  } catch (...) {
    std::cerr << "Unknown Exception\n";
    return -1;
  }

  return 0;
}

/** \brief Get next return value
 *
 * This function returns a non-zero integer to return after a test case
 * was unsuccessful. This is useful if one wants to add test cases to an
 * existing test later and avoid returning any one value more than once.
 */
static inline int next_integer(void) {
  static int counter = 1;
  ++counter;
  if (counter == 0) {
    return ++counter;
  } else {
    return counter;
  }
}

#if defined(RUN_TEST_CASES)
/**\brief List of test cases in this file
 *
 * The list of test cases defined in this file; this is set by the TEST_BATCH()
 * macro to the actual list of test cases that you want to run.
 */
extern const std::vector<testCase> cases;

/**\brief Test case runner main stub
 *
 * This is a very simple main function that will simply run the test cases
 * defined in the programme including this header. Needless to say you should
 * not define your own main function in test case files.
 */
extern "C" int main(int argc, char **argv) {
  return efgy::test::run(argc, argv, cases);
}
}
}
/**\brief Define test batch
 *
 * This macro defines a set of test cases to run. You should use this in your
 * test case file after defining all the test cases you'd like to run. The
 * macro will set things up so that the test cases can be run by the stub main
 * function defined by this header.
 *
 * You should use this macro like this:
 *
 * \code
 * TEST_BATCH(myTestFunction1, MyTestFunction2)
 * \endcode
 *
 * The macro takes an arbitrary number of functions. Make sure they all match
 * the efgy::test::testCase prototype and run the macro at the top level of
 * your source code, outside of any functions,
 *
 * \note Expansion of this macro depends on whether the RUN_TEST_CASES macro
 *       is defined or not. If it is, then this will set a global variable
 *       as described; if not then this macro is simply discarded.
 */
#define TEST_BATCH(...)                                          \
  namespace efgy {                                               \
  namespace test {                                               \
  static const testCase testCasesArray[] = {__VA_ARGS__};        \
  const std::vector<testCase> cases(testCasesArray,              \
                                    testCasesArray +             \
                                        sizeof(testCasesArray) / \
                                            sizeof(testCase));   \
  }                                                              \
  }
#else
}
}
/**\brief Define test batch
 *
 * This macro defines a set of test cases to run. You should use this in your
 * test case file after defining all the test cases you'd like to run. The
 * macro will set things up so that the test cases can be run by the stub main
 * function defined by this header.
 *
 * You should use this macro like this:
 *
 * \code
 * TEST_BATCH(myTestFunction1, MyTestFunction2)
 * \endcode
 *
 * The macro takes an arbitrary number of functions. Make sure they all match
 * the efgy::test::testCase prototype and run the macro at the top level of
 * your source code, outside of any functions,
 *
 * \note Expansion of this macro depends on whether the RUN_TEST_CASES macro
 *       is defined or not. If it is, then this will set a global variable
 *       as described; if not then this macro is simply discarded.
 */
#define TEST_BATCH(...)
#endif
