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

#include <ef.gy/primitive.h>
#include <ef.gy/fractions.h>
#include <ef.gy/pi.h>

using namespace efgy::math;
using namespace std;

typedef primitive<long double, unsigned long long> longDouble;

int main (int argc, char **argv)
{
    pi<longDouble,1> piD1;
    pi<longDouble,2> piD2;
    pi<longDouble,3> piD3;
    pi<longDouble,4> piD4;

    std::cerr << "pi<1> = " << (long double)longDouble(piD1) << "\n";
    std::cerr << "pi<2> = " << (long double)longDouble(piD2) << "\n";
    std::cerr << "pi<3> = " << (long double)longDouble(piD3) << "\n";
    std::cerr << "pi<4> = " << (long double)longDouble(piD4) << "\n";

    pi<fraction,1> piQ1;
    pi<fraction,2> piQ2;
    pi<fraction,3> piQ3;
    pi<fraction,4> piQ4;

    std::cerr << "pi<fraction,1> = " << (string)fraction(piQ1) << "\n";
    std::cerr << "pi<fraction,2> = " << (string)fraction(piQ2) << "\n";
    std::cerr << "pi<fraction,3> = " << (string)fraction(piQ3) << "\n";
    std::cerr << "pi<fraction,4> = " << (string)fraction(piQ4) << "\n";

    return 0;
}
