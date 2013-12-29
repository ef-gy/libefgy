/**\file
 * \brief Test cases for the 'pi' template
 *
 * Test cases for the 'pi' template to make sure instances of that class compile
 * properly and generate reasonable approximations of 'pi'.
 *
 * \copyright
 * Copyright (c) 2012-2013, ef.gy Project Members
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
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#include <iostream>

#include <ef.gy/test-case.h>
#include <ef.gy/primitive.h>
#include <ef.gy/fractions.h>
#include <ef.gy/pi.h>

using namespace efgy::math;
using std::string;

typedef primitive<long double, unsigned long long> longDouble;

/**\brief 'Pi' initialisation and type cast tests
 * \test Initialises several instances of the 'pi' template with different base
 *       types and precisions. The instances are then cast to different types
 *       and written to the log to verify that the template compiles cleanly.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testPi (std::ostream &log)
{
    pi<longDouble,1> piD1;
    pi<longDouble,2> piD2;
    pi<longDouble,3> piD3;
    pi<longDouble,4> piD4;

    log << "pi<longDouble,1> = " << (long double)longDouble(piD1) << "\n";
    log << "pi<longDouble,2> = " << (long double)longDouble(piD2) << "\n";
    log << "pi<longDouble,3> = " << (long double)longDouble(piD3) << "\n";
    log << "pi<longDouble,4> = " << (long double)longDouble(piD4) << "\n";

    pi<long double,1> piDL1;
    pi<long double,2> piDL2;
    pi<long double,3> piDL3;
    pi<long double,4> piDL4;

    log << "pi<long double,1> = " << (long double)piDL1 << "\n";
    log << "pi<long double,2> = " << (long double)piDL2 << "\n";
    log << "pi<long double,3> = " << (long double)piDL3 << "\n";
    log << "pi<long double,4> = " << (long double)piDL4 << "\n";

    pi<fraction,1> piQ1;
    pi<fraction,2> piQ2;
    pi<fraction,3> piQ3;
    pi<fraction,4> piQ4;

    log << "pi<fraction,1> = " << (string)fraction(piQ1) << "\n";
    log << "pi<fraction,2> = " << (string)fraction(piQ2) << "\n";
    log << "pi<fraction,3> = " << (string)fraction(piQ3) << "\n";
    log << "pi<fraction,4> = " << (string)fraction(piQ4) << "\n";

    return 0;
}

TEST_BATCH(testPi)
