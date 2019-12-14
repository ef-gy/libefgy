/* Test cases for transformation
 *
 * Contains test cases that test libefgy's transformation.h and the functions
 * therein.
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

#include <ef.gy/test-case.h>
#include <ef.gy/transformation.h>

#include <iostream>

using namespace efgy::geometry::transformation;
using namespace efgy::test;

/* Tests correct evaluation of identity map.
 * @log Where to write log messages to.
 *
 * Initialises an identity map using the identity class, and a non-trivial
 * linear map. Computes both products of the identity and the non-trivial map;
 * if the identity transformation behaves correctly, the result should
 * be equal to the original non-trivial map in both cases.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testIdentity(std::ostream &log) {
  identity<double, 3> id;

  linear<double, 3> phi;

  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 3; k++) {
      phi.matrix[i][k] = i * k;
    }
  }

  linear<double, 3> leftProduct = id * phi;
  linear<double, 3> rightProduct = phi * id;

  bool leftProductOK = true;
  bool rightProductOK = true;

  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 3; k++) {
      leftProductOK &= (leftProduct.matrix[i][k] == i * k);
      rightProductOK &= (rightProduct.matrix[i][k] == i * k);
    }
  }

  if (!leftProductOK) {
    log << "Incorrect product of identity transformation with non-trivial "
           "transformation (id * phi).\n";
    log << "\n Expected: \n";
    log << phi.matrix;
    log << "\n Actual: \n";
    log << leftProduct.matrix;

    return false;
  }
  if (!rightProductOK) {
    log << "Incorrect product of non-trivial transformation with identity "
           "transformation (phi * id).\n";
    log << "\n Expected: \n";
    log << phi.matrix;
    log << "\n Actual: \n";
    log << rightProduct.matrix;

    return false;
  }

  return true;
}

/* Tests construction of affine transformations.
 * @log Where to write log messages to.
 *
 * Constructs an affine transformation from a linear map.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testAffineConstruction(std::ostream &log) {
  linear<double, 3> phi;

  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 3; k++) {
      phi.matrix[i][k] = i * k;
    }
  }

  affine<double, 3> psi(phi);

  bool allCorrect = true;

  // check if matrix entries of linear map are in place
  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 3; k++) {
      allCorrect &= (psi.matrix[i][k] == i * k);
    }
  }

  // check for zeroes in the last row and column
  for (int i = 0; i < 3; i++) {
    allCorrect &= (psi.matrix[i][3] == 0);
    allCorrect &= (psi.matrix[3][i] == 0);
  }

  allCorrect &= (psi.matrix[3][3] == 1);

  if (!allCorrect) {
    log << "An error occurred in constructing an affine transformation";
    log << " from a linear map.\n";

    return false;
  } else {
    return true;
  }
}

namespace test {
using efgy::test::function;

static function identity(testIdentity);
static function affineConstruction(testAffineConstruction);
}  // namespace test
