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

#if !defined(EF_GY_UNITS_H)
#define EF_GY_UNITS_H

namespace efgy
{
    namespace unit
    {
        template<typename Q>
        class unitValue : public Q
        {
            public:
                typedef Q base;

                unitValue()
                    : base(0)
                    {}

                unitValue(const base &pV)
                    : base(pV)
                    {}
        };

        template<typename U, typename I, I factor, I divisor>
        class scaledUnitValue : public U::base
        {
            public:
                typedef typename U::base base;

                scaledUnitValue()
                    : base(0)
                    {}

                scaledUnitValue(const base &pV)
                    : base(pV)
                    {}

                scaledUnitValue(const U &pV)
                    : base(base(pV) * base(factor) / base(divisor))
                    {}

                operator U () const
                {
                    return U(base(*this) * base(divisor) / base(factor));
                }
        };

        template<typename Q>
        class metre : public unitValue<Q>
        {
            public:
                typedef typename unitValue<Q>::base base;

                metre() {}
                metre(const base &pV)
                    : unitValue<Q>(pV)
                    {}
        };

        template<typename Q>
        class milli : public scaledUnitValue<Q, unsigned long long, 1, 1000>
        {
            public:
                typedef typename Q::base base;
                milli() {}
                milli(const base &pV)
                    : scaledUnitValue<Q,unsigned long long, 1, 1000>(pV)
                    {}
                milli(const Q &pV)
                    : scaledUnitValue<Q,unsigned long long, 1, 1000>(pV)
                    {}
        };
    };
};

#endif
