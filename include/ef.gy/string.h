/*
 * This file is part of the ef.gy project.
 * See the appropriate repository at http://ef.gy/.git for exact file
 * modification records.
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

#if !defined(EF_GY_STRING_H)
#define EF_GY_STRING_H

#include <string>

namespace efgy
{
    namespace data
    {
        template<typename T>
        std::string intToString (T pNumber, const int pBase = 10)
        {
            bool negative = pNumber < math::numeric::zero();
            std::string rv = "";

            if (negative)
            {
                pNumber = -pNumber;
            }

            while (pNumber > math::numeric::zero())
            {
                const char t[2] = { "0123456789abcdefghijklmnopqrstuvwxyz"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ#,."[(pNumber % T(pBase))],
                                    0 };
                std::string tq(t);

                rv = tq + rv;
                pNumber /= pBase;
            }

            if (rv == "")
            {
                rv = "0";
            }

            if (negative)
            {
                std::string m("-");
                rv = m + rv;
            }

            return rv;
        }
    };
};

#endif
