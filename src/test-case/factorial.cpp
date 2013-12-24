/**\file
 * \brief Test cases for the factorial class template
 *
 * This file contains test cases for the math::factorial template, which is
 * used to calculate the factorial of a number.
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
#include <ef.gy/factorial.h>

using namespace efgy;
using namespace efgy::math;
using namespace std;

/**\brief Factorial tests
 *
 * \test Calculates the factorial of a few numbers and compares those to the
 *       values that the template ought to return.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testFactorial(ostream &log)
{

    if(factorial<int>(0) != 1)
    {
        log << "Factorial of 0 should be 1.";
        return 1;
    }

    
    if(factorial<int>(1) != 1)
    {
        log << "Factorial of 1 should be 1.";
        return 2;
    }


    if(factorial<int>(5) != 120)
    {
        log << "Factorial of 5 should be 120.";
        return 3;
    }

    return 0;
}

TEST_BATCH(testFactorial)
