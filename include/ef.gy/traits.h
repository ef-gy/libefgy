/**\file
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

#if !defined(EF_GY_TRAITS_H)
#define EF_GY_TRAITS_H

#include <array>

namespace efgy
{
    namespace math
    {
        template<typename Q, typename I>
        class primitive;

        namespace numeric
        {
            template<typename T>
            class traits
            {
                public:
                    typedef T integral;
                    typedef T rational;
                    typedef T self;
                    typedef primitive<T> derivable;

                    static const bool stable = false;
            };

            template<>
            class traits<double>
            {
                public:
                    typedef long integral;
                    typedef double rational;
                    typedef double self;
                    typedef primitive<double> derivable;

                    static const bool stable = false;
            };

            template<>
            class traits<long double>
            {
                public:
                    typedef long long integral;
                    typedef long double rational;
                    typedef long double self;
                    typedef primitive<long double> derivable;

                    static const bool stable = false;
            };

            template<typename Q, typename I>
            class traits<primitive<Q,I>>
            {
                public:
                    typedef I integral;
                    typedef Q rational;
                    typedef primitive<Q,I> self;
                    typedef primitive<Q,I> derivable;

                    static const bool stable = false;
            };
        };
    };
};

#endif
