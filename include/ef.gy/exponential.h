/*
 * This file is part of the ef.gy project.
 * See the appropriate repository at http://ef.gy/.git for exact file
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

#if !defined(EF_GY_EXPONENTIAL_H)
#define EF_GY_EXPONENTIAL_H

namespace efgy
{
    namespace math
    {
        template <typename Q>
        class exponential
        {
            public:
                exponential()
                    : base(Q(0)), factor(Q(1)), exponent(Q(0))
                    {}

                exponential(const Q &pBase)
                    : base(pBase), factor(Q(1)), exponent(Q(0))
                    {}

                exponential(const Q &pBase, const Q &pExponent)
                    : base(pBase), factor(Q(1)), exponent(pExponent)
                    {}

                exponential(const Q &pBase, const Q &pExponent, const Q &pFactor)
                    : base(pBase), factor(pFactor), exponent(pExponent)
                    {}

                exponential operator + (const exponential &b) const;
                exponential operator - (const exponential &b) const;
                exponential operator * (const exponential &b) const;
                exponential operator / (const exponential &b) const;

                operator Q (void) const;

                Q base;
                Q factor;
                Q exponent;
        };
    };
};

#endif
