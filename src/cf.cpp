/*
 * This file is part of the colouri.se project.
 * See the appropriate repository at http://colouri.se/.git for exact file
 * modification records.
*/

/*
 * Copyright (c) 2012, ef.gy Project Members
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

#include <ef.gy/fractions.h>
#include <ef.gy/continued-fractions.h>

using namespace efgy::math;
using namespace std;

int main (int argc, char* argv[])
{
    try
    {
        numeric::continuedFractional<number> cfa, cfb, cfc;
        numeric::fractional<number> af, bf(54,92), cf(76,131);
        cfb = bf;
        cfc = cf;
        cfa = cfc - cfb;
        af = cfa;
        std::cerr << string(cfa) << " = " << string(af) << "\n"
                  << string(cfb) << " = " << string(bf) << "\n"
                  << string(cfc) << " = " << string(cf) << "\n";
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unknown Exception\n";
    }

    return 0;
}
