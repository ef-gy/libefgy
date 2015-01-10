/**\file
 * \brief Test cases for the 'exponentiate' namespace
 *
 * Test whether things in the efgy::math::exponentiate namespace work as
 * intended.
 *
 * \copyright
 * Copyright (c) 2012-2015, ef.gy Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#include <iostream>

#include <ef.gy/test-case.h>
#include <ef.gy/exponential.h>
#include <ef.gy/fractions.h>

using namespace efgy::math;

/**\brief Integer exponents
 * \test Raises some numbers to integer powers and compares the results with
 *       reference data to see if the exponentiate::integral template works
 *       correctly.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testIntegralExponents (std::ostream &log)
{
    if (exponentiate::integral<fraction,0>::raise(fraction(21,5)) != fraction(1,1))
    {
        log << "(21/5)^0 should be (1/1) but is " << exponentiate::integral<fraction,0>::raise(fraction(21,5)) << "\n";
        return 1;
    }

    if (exponentiate::integral<fraction,1>::raise(fraction(21,5)) != fraction(21,5))
    {
        log << "(21/5)^1 should be (21/5) but is " << exponentiate::integral<fraction,1>::raise(fraction(21,5)) << "\n";
        return 2;
    }

    if (exponentiate::integral<fraction,2>::raise(fraction(21,5)) != fraction(441,25))
    {
        log << "(21/5)^2 should be (441/25) but is " << exponentiate::integral<fraction,2>::raise(fraction(21,5)) << "\n";
        return 3;
    }

    if (exponentiate::integral<fraction,3>::raise(fraction(21,5)) != fraction(9261,125))
    {
        log << "(21/5)^3 should be (9261/125) but is " << exponentiate::integral<fraction,3>::raise(fraction(21,5)) << "\n";
        return 4;
    }

    if (exponentiate::integral<fraction,4>::raise(fraction(21,5)) != fraction(194481,625))
    {
        log << "(21/5)^4 should be (194481/625) but is " << exponentiate::integral<fraction,4>::raise(fraction(21,5)) << "\n";
        return 5;
    }

    if (exponentiate::integral<fraction,5>::raise(fraction(21,5)) != fraction(4084101l,3125))
    {
        log << "(21/5)^5 should be (4084101/3125) but is " << exponentiate::integral<fraction,5>::raise(fraction(21,5)) << "\n";
        return 6;
    }

    if (exponentiate::integral<fraction,-1>::raise(fraction(21,5)) != fraction(5,21))
    {
        log << "(21/5)^(-1) should be (5/21) but is " << exponentiate::integral<fraction,-1>::raise(fraction(21,5)) << "\n";
        return 7;
    }

    if (exponentiate::integral<fraction,-2>::raise(fraction(21,5)) != fraction(25,441))
    {
        log << "(21/5)^(-2) should be (25/441) but is " << exponentiate::integral<fraction,-2>::raise(fraction(21,5)) << "\n";
        return 8;
    }

    if (exponentiate::integral<fraction,-3>::raise(fraction(21,5)) != fraction(125,9261))
    {
        log << "(21/5)^(-3) should be (125/9261) but is " << exponentiate::integral<fraction,-3>::raise(fraction(21,5)) << "\n";
        return 9;
    }

    if (exponentiate::integral<fraction,-4>::raise(fraction(21,5)) != fraction(625,194481))
    {
        log << "(21/5)^(-4) should be (625/194481) but is " << exponentiate::integral<fraction,-4>::raise(fraction(21,5)) << "\n";
        return 10;
    }

    if (exponentiate::integral<fraction,-5>::raise(fraction(21,5)) != fraction(3125,4084101l))
    {
        log << "(21/5)^(-5) should be (3125/4084101) but is " << exponentiate::integral<fraction,-5>::raise(fraction(21,5)) << "\n";
        return 11;
    }

    return 0;
}

/**\brief Integer exponents, function parameter base
 * \test Same test procedure as testIntegralExponents, but this test cases uses
 *       the two-parameter variant of the exponentiate::integral instead of
 *       specifying the exponent as a template parameter.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testFunctionalIntegralExponents (std::ostream &log)
{
    if (exponentiate::integral<fraction>::raise(fraction(21,5),0) != fraction(1,1))
    {
        log << "(21/5)^0 should be (1/1) but is " << exponentiate::integral<fraction>::raise(fraction(21,5),0) << "\n";
        return 1;
    }

    if (exponentiate::integral<fraction>::raise(fraction(21,5),1) != fraction(21,5))
    {
        log << "(21/5)^1 should be (21/5) but is " << exponentiate::integral<fraction>::raise(fraction(21,5),1) << "\n";
        return 2;
    }

    if (exponentiate::integral<fraction>::raise(fraction(21,5),2) != fraction(441,25))
    {
        log << "(21/5)^2 should be (441/25) but is " << exponentiate::integral<fraction>::raise(fraction(21,5),2) << "\n";
        return 3;
    }

    if (exponentiate::integral<fraction>::raise(fraction(21,5),3) != fraction(9261,125))
    {
        log << "(21/5)^3 should be (9261/125) but is " << exponentiate::integral<fraction>::raise(fraction(21,5),3) << "\n";
        return 4;
    }

    if (exponentiate::integral<fraction>::raise(fraction(21,5),4) != fraction(194481,625))
    {
        log << "(21/5)^4 should be (194481/625) but is " << exponentiate::integral<fraction>::raise(fraction(21,5),4) << "\n";
        return 5;
    }

    if (exponentiate::integral<fraction>::raise(fraction(21,5),5) != fraction(4084101l,3125))
    {
        log << "(21/5)^5 should be (4084101/3125) but is " << exponentiate::integral<fraction>::raise(fraction(21,5),5) << "\n";
        return 6;
    }

    if (exponentiate::integral<fraction>::raise(fraction(21,5),-1) != fraction(5,21))
    {
        log << "(21/5)^(-1) should be (5/21) but is " << exponentiate::integral<fraction>::raise(fraction(21,5),-1) << "\n";
        return 7;
    }

    if (exponentiate::integral<fraction>::raise(fraction(21,5),-2) != fraction(25,441))
    {
        log << "(21/5)^(-2) should be (25/441) but is " << exponentiate::integral<fraction>::raise(fraction(21,5),-2) << "\n";
        return 8;
    }

    if (exponentiate::integral<fraction>::raise(fraction(21,5),-3) != fraction(125,9261))
    {
        log << "(21/5)^(-3) should be (125/9261) but is " << exponentiate::integral<fraction>::raise(fraction(21,5),-3) << "\n";
        return 9;
    }

    if (exponentiate::integral<fraction>::raise(fraction(21,5),-4) != fraction(625,194481))
    {
        log << "(21/5)^(-4) should be (625/194481) but is " << exponentiate::integral<fraction>::raise(fraction(21,5),-4) << "\n";
        return 10;
    }

    if (exponentiate::integral<fraction>::raise(fraction(21,5),-5) != fraction(3125,4084101l))
    {
        log << "(21/5)^(-5) should be (3125/4084101) but is " << exponentiate::integral<fraction>::raise(fraction(21,5),-5) << "\n";
        return 11;
    }

    return 0;
}
TEST_BATCH(testIntegralExponents, testFunctionalIntegralExponents)
