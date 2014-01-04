/**\file
 * \brief Sequences
 *
 * Contains supporting types for infinite sequences.
 *
 * \copyright
 * Copyright (c) 2012-2014, ef.gy Project Members
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

#if !defined(EF_GY_SEQUENCES_H)
#define EF_GY_SEQUENCES_H

namespace efgy
{
    namespace math
    {
        /**\brief Infinite sequence
         *
         * Represents an infinite sequence. You need to provide an algrithm to
         * calculate arbitrary members of these sequences.
         *
         * \tparam Q         Base type for calculations.
         * \tparam algorithm The algorithm to calculate the sequence members.
         * \tparam N         Base integral type.
         */
        template<typename Q, template<typename Q, typename N> class algorithm, typename N = unsigned long long>
        class sequence : public algorithm<Q, N>
        {
            public:
                constexpr static get (const N &n)
                {
                    return at(n);
                }
        };
    };
};

#endif