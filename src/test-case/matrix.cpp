/*  Test cases for matrices
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

#include <ef.gy/matrix.h>
#include <ef.gy/range.h>
#include <ef.gy/test-case.h>

#include <algorithm>
#include <iostream>
#include <sstream>

using namespace efgy::math;
using efgy::range;

/* Tests matrix construction.
 * @log A stream to copy log messages to.
 *
 * Constructs a 3x4 matrix from scratch, then copies it to another matrix
 * object.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testConstruction(std::ostream &log) {
  matrix<int, 3, 4> m;
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 4; k++) {
      m[i][k] = i * k;
    }
  }

  matrix<int, 3, 4> m2(m);

  bool allCorrect = true;
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 4; k++) {
      allCorrect &= (m2[i][k] == (i * k));
    }
  }

  if (!allCorrect) {
    log << "Actual and expected value in result of copy constructor are "
           "different";
    return false;
  }
  return true;
}

/* Test matrix assignment.
 * @log A stream to copy log messages to.
 *
 * Constructs a 3x4 matrix and assigns it to another matrix object, then checks
 * if values were copied correctly.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testAssignment(std::ostream &log) {
  matrix<int, 3, 4> m;
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 4; k++) {
      m[i][k] = i * k;
    }
  }

  matrix<int, 3, 4> m2 = m;

  bool allCorrect = true;
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 3; k++) {
      allCorrect &= (m2[i][k] == m[i][k]);
    }
  }

  if (!allCorrect) {
    log << "Values were not copied correctly during assignment.";
    return false;
  }

  return true;
}

/* Test matrix addition.
 * @log A stream to copy log messages to.
 *
 * Constructs a matrix from scratch; checks whether adding the zero matrix
 * from the left and the right yields the expected result, then adds two
 * nontrivial matrices and checks the result.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testAddition(std::ostream &log) {
  matrix<int, 3, 4> m;
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 4; k++) {
      m[i][k] = i * k;
    }
  }

  matrix<int, 3, 4> n;

  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 4; k++) {
      n[i][k] = 0;
    }
  }

  // results of left and right addition of
  // neutral element to m
  matrix<int, 3, 4> leftNeutral = n + m;
  matrix<int, 3, 4> rightNeutral = m + n;

  // test addition of neutral element;
  bool allCorrectLeft = true;
  bool allCorrectRight = true;
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 4; k++) {
      allCorrectLeft &= (leftNeutral[i][k] == (i * k));
      allCorrectRight &= (rightNeutral[i][k] == (i * k));
    }
  }
  if (!allCorrectLeft) {
    log << "n + m, where n is the neutral element: ";
    for (int i = 0; i < 3; i++) {
      for (int k = 0; k < 4; k++) {
        log << leftNeutral[i][k];
      }
      log << "\n";
    }
    log << "\n";
    log << "m (original matrix):";
    for (int i = 0; i < 3; i++) {
      for (int k = 0; k < 4; k++) {
        log << m[i][k];
      }
      log << "\n";
    }

    log << "Unexpected result of left addition of neutral element to matrix.";
    return false;
  }
  if (!allCorrectRight) {
    log << "m + n, where n is the neutral element: ";
    for (int i = 0; i < 3; i++) {
      for (int k = 0; k < 4; k++) {
        log << rightNeutral[i][k];
      }
      log << "\n";
    }

    log << "\n";
    log << "m (original matrix):";
    for (int i = 0; i < 3; i++) {
      for (int k = 0; k < 4; k++) {
        log << m[i][k];
      }
      log << "\n";
    }
    log << "Unexpected result of right addition of neutral element to matrix.";
    return false;
  }

  // another test for addition of two non-neutral elements
  matrix<int, 3, 4> m2;

  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 4; k++) {
      m2[i][k] = i + k;
    }
  }

  matrix<int, 3, 4> sum = m + m2;

  bool allCorrectSum = true;
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 3; k++) {
      allCorrectSum &= (sum[i][k] == (i * k) + (i + k));
    }
  }

  if (!allCorrectSum) {
    log << "Unexpected result adding two matrices.";
    return false;
  }

  return true;
}

/* Tests stream output.
 * @log A stream to copy log messages to.
 *
 * Constructs a matrix and inserts it into a std::ostringstream.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testStream(std::ostream &log) {
  std::ostringstream stream;

  matrix<int, 3, 4> m;
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 4; k++) {
      m[i][k] = i * k;
    }
  }

  stream << m;

  std::stringstream expected;

  expected << "0\t0\t0\t0\t\n";
  expected << "0\t1\t2\t3\t\n";
  expected << "0\t2\t4\t6\t\n";

  if (stream.str() != expected.str()) {
    log << "Unexpected characters in printable representation of matrix.\n";
    log << "Matrix: \n";
    log << m;
    log << "\nExpected:\n";
    log << expected.str();

    return false;
  }
  return true;
}

/* Test matrix iterator.
 * @log A stream to copy log messages to.
 *
 * Constructs a matrix and then uses the matrix::iterator to compare the
 * contents with a range that should be the same.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testIterator(std::ostream &log) {
  matrix<int, 3, 4> m;
  int j = 0;
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 4; k++) {
      m[i][k] = j++;
    }
  }

  auto r = range<int>(0, 3 * 4, false);

  if (!std::equal(m.begin(), m.end(), r.begin())) {
    log << "matrix iterator and range iterator did not produce the same "
           "sequence:\n";
    for (auto i : m) {
      log << "\t" << i;
    }
    log << "\nvs. expected:\n";
    for (auto i : r) {
      log << "\t" << i;
    }
    log << "\n";
    return false;
  }

  return true;
}

namespace test {
using efgy::test::function;

static function construction(testConstruction);
static function assignment(testAssignment);
static function addition(testAddition);
static function stream(testStream);
static function iterator(testIterator);
}  // namespace test
