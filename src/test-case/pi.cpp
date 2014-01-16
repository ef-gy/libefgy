/**\file
 * \brief Test cases for the 'pi' template
 *
 * Test cases for the 'pi' template to make sure instances of that class compile
 * properly and generate reasonable approximations of 'pi'.
 *
 * \copyright
 * Copyright (c) 2012-2014, ef.gy Project Members
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
using efgy::test::next_integer;
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
    pi<longDouble> piD1(1,1);
    pi<longDouble> piD2(1,2);
    pi<longDouble> piD3(1,3);
    pi<longDouble> piD4(1,4);

    log << "pi<longDouble,1> = " << (long double)longDouble(piD1) << "\n";
    log << "pi<longDouble,2> = " << (long double)longDouble(piD2) << "\n";
    log << "pi<longDouble,3> = " << (long double)longDouble(piD3) << "\n";
    log << "pi<longDouble,4> = " << (long double)longDouble(piD4) << "\n";

    if (longDouble(piD1) != pi<longDouble>::get(1))
    {
        log << "constexpr method returned unexpected result\n";
        return 1;
    }

    if (longDouble(piD2) != pi<longDouble>::get(2))
    {
        log << "constexpr method returned unexpected result\n";
        return 2;
    }

    if (longDouble(piD3) != pi<longDouble>::get(3))
    {
        log << "constexpr method returned unexpected result\n";
        return 3;
    }

    if (longDouble(piD4) != pi<longDouble>::get(4))
    {
        log << "constexpr method returned unexpected result\n";
        return 4;
    }

    pi<long double> piDL1(1,1);
    pi<long double> piDL2(1,2);
    pi<long double> piDL3(1,3);
    pi<long double> piDL4(1,4);

    if ((long double)(piDL1) != pi<long double>::get(1))
    {
        log << "constexpr method returned unexpected result\n";
        return 11;
    }

    if ((long double)(piDL2) != pi<long double>::get(2))
    {
        log << "constexpr method returned unexpected result\n";
        return 12;
    }

    if ((long double)(piDL3) != pi<long double>::get(3))
    {
        log << "constexpr method returned unexpected result\n";
        return 13;
    }

    if ((long double)(piDL4) != pi<long double>::get(4))
    {
        log << "constexpr method returned unexpected result\n";
        return 14;
    }

    log << "pi<long double,1> = " << (long double)piDL1 << "\n";
    log << "pi<long double,2> = " << (long double)piDL2 << "\n";
    log << "pi<long double,3> = " << (long double)piDL3 << "\n";
    log << "pi<long double,4> = " << (long double)piDL4 << "\n";

    pi<fraction> piQ1(fraction(1),1);
    pi<fraction> piQ2(fraction(1),2);
    pi<fraction> piQ3(fraction(1),3);
    pi<fraction> piQ4(fraction(1),4);

    log << "pi<fraction,1> = " << fraction(piQ1) << "\n";
    log << "pi<fraction,2> = " << fraction(piQ2) << "\n";
    log << "pi<fraction,3> = " << fraction(piQ3) << "\n";
    log << "pi<fraction,4> = " << fraction(piQ4) << "\n";

    if (fraction(piQ1) != pi<fraction>::get(1))
    {
        log << "constexpr method returned unexpected result\n";
        return 21;
    }

    if (fraction(piQ2) != pi<fraction>::get(2))
    {
        log << "constexpr method returned unexpected result\n";
        return 22;
    }

    if (fraction(piQ3) != pi<fraction>::get(3))
    {
        log << "constexpr method returned unexpected result\n";
        return 23;
    }

    if (fraction(piQ4) != pi<fraction>::get(4))
    {
        log << "constexpr method returned unexpected result\n";
        return 24;
    }

    return 0;
}

TEST_BATCH(testPi)
