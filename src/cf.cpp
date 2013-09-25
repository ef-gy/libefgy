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
#include <cstdlib>

using namespace efgy::math;

numeric::continuedFractional<number> apply
    (const numeric::continuedFractional<number> &cf,
     const char &op,
     const number &n)
{
    switch (op)
    {
        case 0:
            return n;
        case '+':
            return cf+n;
        case '-':
            return cf-n;
        case '*':
            return cf*n;
        case '/':
            return cf/n;
        case ',':
            return (cf,n);
        default:
            throw std::exception();
    }
}

int main (int argc, char* argv[])
{
    try
    {
        for (int i = 1; i < argc; i++)
        {
            numeric::continuedFractional<number> res;
            std::string arg;
            number n;
            char op = 0;

            for (int j = 0; argv[i][j]; j++)
            {
                switch (argv[i][j])
                {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        arg += std::string(1, argv[i][j]);
                        break;
                    case '-':
                    case '+':
                        if (arg.size() == 0)
                        {
                            arg += std::string(1, argv[i][j]);
                            break;
                        }
                    case '*':
                    case '/':
                    case ',':
                        op = argv[i][j];
                        n = std::atoi(arg.c_str());
                        arg = std::string();
                        res = apply (res, op, n);
                        break;
                }
            }

            if (arg.size() > 0)
            {
                n = std::atoi(arg.c_str());
                arg = std::string();
                res = apply (res, op, n);
            }

            std::cerr << std::string(res) << " = " << std::string(numeric::fractional<number>(res)) << "\n";
        }
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
