/**\file
 * \brief Test cases for transformation
 *
 * Contains test cases that test libefgy's transformation.h and the functions
 * therein.
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

#include <ef.gy/test-case.h>
#include <ef.gy/transformation.h>

using namespace efgy::geometry::transformation;
using namespace efgy::test;

/*\brief Tests correct evaluation of identity map.
 *
 * \test Initialises an identity map using the identity class,
 * and a non-trivial linear map.
 * Computes both products of the identity and the non-trivial map;
 * if the identity transformation behaves correctly, the result should
 * be equal to the original non-trivial map in both cases.
 *
 * \param log A stream for message output.
 *
 * \returns Zero if the test was successful, a nonzero integer otherwise.
 */
int testIdentity(std::ostream &log) {
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

    return next_integer();
  }
  if (!rightProductOK) {
    log << "Incorrect product of non-trivial transformation with identity "
           "transformation (phi * id).\n";
    log << "\n Expected: \n";
    log << phi.matrix;
    log << "\n Actual: \n";
    log << rightProduct.matrix;

    return next_integer();
  }

  return 0;
}

#if 0
/*\brief Tests whether the identity class enforces an identity matrix.
 *
 * \test Initialises an object of the identity class, then attempts to
 * assign a non-identity matrix to them. This should not work, and
 * ideally, it should not even compile.
 *
 * \param log Stream for output messages.
 *
 * \returns A nonzero integer in any case, since if this test compiles,
 * there is a bug.
 *
 * \note This has been resolved so it is no longer compiled in. The function is
 *       left here in case someone wants to verify that this doesn't compile at
 *       some point in the future.
 */
int testIdentityAssignment(std::ostream &log) {
  identity<double, 3> id;

  for (int i = 0; i < 3; i++) {
    for (int k = 0; k < 3; k++) {
      id.matrix[i][k] = 2;
    }
  }

  log << "Assignment to the transformation matrix of an identity object ";
  log << "is possible; this is a bug and should not compile.\n";
  return next_integer();
}
#endif

/* \brief Tests construction of affine transformations.
 *
 * \param log Stream for output messages
 *
 * \test Constructs an affine transformation from a linear map.
 *
 * \returns Zero if the test is successful, a nonzero integer otherwise.
 */
int testAffineConstruction(std::ostream &log) {
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

    return next_integer();
  } else {
    return 0;
  }
}

namespace test {
using efgy::test::function;

static function identity(testIdentity);
static function affineConstruction(testAffineConstruction);
}
