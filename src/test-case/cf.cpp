/**\file
 * \brief Test cases for continued fractions
 *
 * The test cases in this file test conversions to and from continued fractions
 * as well as basic arithmetic with those continued fractions.
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
#include <ef.gy/fractions.h>
#include <ef.gy/continued-fractions.h>

using namespace efgy::math;
using std::string;

/**\brief Continued fraction arithmetic tests
 * \test Initialises a few regular fractions and a few continued fractions, then
 *       performs arithmetic with both of them and compares the results.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testContinuedFractionArithmetic (std::ostream &log)
{
    numeric::fractional<long long> af(6,11), bf(4,5), rf;
    numeric::continuedFractional<long long> a(af), b(bf), r;

    rf = af + bf;
    log << af << " + " << bf << " = " << rf << "\n";

    r = a + b;
    rf = r;
    log << a  << " + " << b  << " = " << r  << " = " << rf << "\n";

    if (rf != r)
    {
        return 1;
    }

    rf = af - bf;
    log << af << " - " << bf << " = " << rf << "\n";

    r = a - b;
    rf = r;
    log << a  << " - " << b  << " = " << r  << " = " << rf << "\n";

    if (rf != r)
    {
        return 2;
    }

    rf = af * bf;
    log << af << " * " << bf << " = " << rf << "\n";

    r = a * b;
    rf = r;
    log << a  << " * " << b  << " = " << r  << " = " << rf << "\n";

    if (rf != r)
    {
        return 3;
    }

    rf = af / bf;
    log << af << " / " << bf << " = " << rf << "\n";

    r = a / b;
    rf = r;
    log << a  << " / " << b  << " = " << r  << " = " << rf << "\n";

    if (rf != r)
    {
        return 4;
    }

    return 0;
}

TEST_BATCH(testContinuedFractionArithmetic)
