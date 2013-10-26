/*
 * This file is part of the libefgy project.
*/

/*
 * Copyright (c) 2012-2013, ef.gy Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include <iostream>

#include <ef.gy/test-case.h>
#include <ef.gy/fractions.h>
#include <ef.gy/continued-fractions.h>

using namespace efgy::math;
using std::string;

int testContinuedFractionArithmetic (std::ostream &log)
{
    numeric::fractional<number> af(6,11), bf(4,5), rf;
    numeric::continuedFractional<number> a(af), b(bf), r;

    rf = af + bf;
    log << string(af) << " + " << string(bf) << " = " << string(rf) << "\n";

    r = a + b;
    rf = r;
    log << string(a)  << " + " << string(b)  << " = " << string(r)  << " = " << string(rf) << "\n";

    if (rf != r)
    {
        return 1;
    }

    rf = af - bf;
    log << string(af) << " - " << string(bf) << " = " << string(rf) << "\n";

    r = a - b;
    rf = r;
    log << string(a)  << " - " << string(b)  << " = " << string(r)  << " = " << string(rf) << "\n";

    if (rf != r)
    {
        return 2;
    }

    rf = af * bf;
    log << string(af) << " * " << string(bf) << " = " << string(rf) << "\n";

    r = a * b;
    rf = r;
    log << string(a)  << " * " << string(b)  << " = " << string(r)  << " = " << string(rf) << "\n";

    if (rf != r)
    {
        return 3;
    }

    rf = af / bf;
    log << string(af) << " / " << string(bf) << " = " << string(rf) << "\n";

    r = a / b;
    rf = r;
    log << string(a)  << " / " << string(b)  << " = " << string(r)  << " = " << string(rf) << "\n";

    if (rf != r)
    {
        return 4;
    }

    return 0;
}

TEST_BATCH(testContinuedFractionArithmetic)
