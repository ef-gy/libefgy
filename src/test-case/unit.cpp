/**\file
 * \brief Test cases for the unit conversion templates
 *
 * These test cases are used to ensure that the templates in the file units.h
 * work as intended.
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
#include <string>

#include <ef.gy/test-case.h>
#include <ef.gy/fractions.h>
#include <ef.gy/units.h>

using namespace efgy::unit;
using efgy::math::fraction;

/**\brief Test case for metric unit conversions
 * \test Convert between different bases of the metre template and test if the
 *       results are plausible.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testUnitMetric (std::ostream &log)
{
    metre<fraction>::unit one(fraction(1));

    if (one != fraction(1))
    {
        log << "variable did not have the expected value after initialisation\n";
        return -1;
    }

    metre<fraction>::milli onem = one;

    if (onem == one)
    {
        log << "value in ::milli should not have been the same as the unit value\n";
        return -2;
    }

    if (onem < one)
    {
        log << "value in ::milli should be larger than the unit value\n";
        return -3;
    }

    metre<fraction>::unit oneu = onem;

    if (oneu != one)
    {
        log << "value before and after converting should have been the same\n";
        return -4;
    }

    metre<fraction>::micro onemi = onem;

    if (onemi == onem)
    {
        log << "value in ::micro should not have been the same as the ::milli value\n";
        return -5;
    }

    if (onemi < onem)
    {
        log << "value in ::micro should be larger than the ::milli value\n";
        return -6;
    }

    if (metre<fraction>::milli(onemi) != onem)
    {
        log << "value in ::micro should be the same in ::milli after conversion\n";
        return -7;
    }

    if (onem != metre<fraction>::milli(fraction(1000)))
    {
        log << "value should have been precisely 1000\n";
        return -8;
    }

    if (onemi != metre<fraction>::micro(fraction(1000000)))
    {
        log << "value should have been precisely 1000000\n";
        return -9;
    }

    if (metre<fraction>::milli(onemi) != metre<fraction>::milli(fraction(1000)))
    {
        log << "value should have been precisely 1000\n";
        return -10;
    }

    metre<fraction>::mega oneM = one;

    if (oneM == one)
    {
        log << "value in ::mega should not have been the same as the unit value\n";
        return -11;
    }

    if (oneM > one)
    {
        log << "value in ::mega should be smaller than the unit value\n";
        return -12;
    }

    if (oneM != metre<fraction>::mega(fraction(1,1000000)))
    {
        log << "value should have been precisely 1/1000000\n";
        return -13;
    }

    return 0;
}

/**\brief Test metric multiplier helper class
 * \test Calculate some metric conversion factors to see if the helper
 *       providing those is working as expected.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testMetricMultipliers (std::ostream &log)
{
    if (metricMultiplier<fraction,1,1>::get() != fraction(10))
    {
        log << "metric multiplier <1,1> is " << (std::string)metricMultiplier<fraction,1,1>::get() << " but should have been 10/1\n";
        return -1;
    }

    if (metricMultiplier<fraction,1,2>::get() != fraction(100))
    {
        log << "metric multiplier <1,2> is " << (std::string)metricMultiplier<fraction,1,2>::get() << " but should have been 100/1\n";
        return -2;
    }

    if (metricMultiplier<fraction,0,2>::get() != fraction(1))
    {
        log << "metric multiplier <0,2> is " << (std::string)metricMultiplier<fraction,0,2>::get() << " but should have been 1/1\n";
        return -3;
    }

    if (metricMultiplier<fraction,3,2>::get() != fraction(1000000))
    {
        log << "metric multiplier <3,2> is " << (std::string)metricMultiplier<fraction,3,2>::get() << " but should have been 1000000/1\n";
        return -4;
    }

    if (metricMultiplier<fraction,3,3>::get() != fraction(1000000000))
    {
        log << "metric multiplier <3,3> is " << (std::string)metricMultiplier<fraction,3,3>::get() << " but should have been 1000000000/1\n";
        return -5;
    }

    return 0;
}

/**\brief Test case for exponential metric unit conversions
 * \test Convert between squared metric units and between inverse squared
 *       metric units.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testUnitEMetric (std::ostream &log)
{
    second<fraction,2>::unit s2_5_7 (fraction(5,7));

    if (fraction(5,7) != fraction(s2_5_7))
    {
        log << "variable was not initialised to the expected value\n";
        return -1;
    }

    second<fraction,2>::milli ms2_5_7 = s2_5_7;

    if (fraction(5000000,7) != ms2_5_7)
    {
        log << "variable was not converted to the expected value\n";
        return -2;
    }

    second<fraction,-2>::unit si2_5_7 (fraction(5,7));

    if (fraction(5,7) != fraction(si2_5_7))
    {
        log << "variable was not initialised to the expected value\n";
        return -3;
    }

    second<fraction,-2>::milli msi2_5_7 = si2_5_7;

    if (fraction(5,7000000) != msi2_5_7)
    {
        log << "variable was not converted to the expected value\n";
        return -4;
    }

    return 0;
}

TEST_BATCH(testUnitMetric, testMetricMultipliers, testUnitEMetric)
