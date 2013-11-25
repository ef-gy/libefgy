/**\file
 * \brief Ranged sequences
 *
 * Contains class and iterator templates for ranged sequences with arbitrary
 * types. These sequences are intended to replace the more common 3-clause for
 * loops with ranged for loops, thus hopefully increasing the legibility of
 * certain types of code.
 *
 * \copyright
 * Copyright (c) 2013, ef.gy Project Members
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

#if !defined(EF_GY_RANGE_H)
#define EF_GY_RANGE_H

#include <array>
#include <iterator>

namespace efgy
{
    /**\brief Range iterator template
     *
     * This template specifies a random access iterator for arbitrary, ranged
     * sequences. You should use the range class template to get instances of
     * this class.
     *
     * Unlike some other iterators, this iterator in particular is rather hard
     * to invalidate. That means you probably won't have to worry about most
     * operations on these iterators in tight loops.
     *
     * \tparam T Numeric base type for the sequences. Basic types such as int
     *           or double should work fine with this template, as should
     *           classes that imitate these types.
     */
    template<typename T>
    class rangeIterator : public std::iterator<std::random_access_iterator_tag, T>
    {
        public:
            constexpr rangeIterator (const T &pStart, const T &pStep, std::size_t pPosition)
                : start(pStart), step(pStep), position(pPosition) {}

            constexpr bool operator == (const rangeIterator &b) const
            {
                return (start == b.start)
                    && (step  == b.step)
                    && (position == b.position);
            }

            constexpr bool operator != (const rangeIterator &b) const
            {
                return (start != b.start)
                    || (step  != b.step)
                    || (position != b.position);
            }

            constexpr T operator * (void) const
            {
                return start + step * position;
            }

            rangeIterator &operator ++ (void)
            {
                ++position;
                return *this;
            }

            rangeIterator &operator -- (void)
            {
                --position;
                return *this;
            }

            rangeIterator &operator += (const std::ptrdiff_t &b)
            {
                position += b;
                return *this;
            }

            constexpr rangeIterator operator + (const std::ptrdiff_t &b) const
            {
                return rangeIterator (start, step, position + b);
            }

            rangeIterator &operator -= (const std::ptrdiff_t &b)
            {
                position -= b;
                return *this;
            }

            constexpr rangeIterator operator - (const std::ptrdiff_t &b) const
            {
                return rangeIterator (start, step, position - b);
            }

            constexpr std::ptrdiff_t operator - (const rangeIterator &b) const
            {
                return std::ptrdiff_t (position) - std::ptrdiff_t (b.position);
            }

            constexpr T &operator [] (const std::ptrdiff_t &b) const
            {
                return *((*this) + b);
            }

            constexpr bool operator < (const rangeIterator &b) const
            {
                return position < b.position;
            }

            constexpr bool operator <= (const rangeIterator &b) const
            {
                return position <= b.position;
            }

            constexpr bool operator > (const rangeIterator &b) const
            {
                return position > b.position;
            }

            constexpr bool operator >= (const rangeIterator &b) const
            {
                return position >= b.position;
            }

        protected:
            const T start;
            const T step;
            std::size_t position;
    };

    template<typename T>
    constexpr rangeIterator<T> operator + (const std::ptrdiff_t &a, const rangeIterator<T> &b)
    {
        return b + a;
    }

    template<typename T, std::size_t n, std::size_t c = n>
    class range
    {
        public:
            constexpr range (const T &pStart)
                : start(pStart), step(T(1)) {}

            constexpr range (const T &pStart, const T &pEnd, const bool inclusive = true)
                : start(pStart), step((pEnd - pStart)/T(n-(inclusive ? 1 : 0))) {}

            typedef rangeIterator<T> iterator;

            constexpr iterator begin (void)
            {
                return iterator(start, step, 0);
            }

            constexpr iterator end (void)
            {
                return iterator(start, step, n);
            }

            constexpr static std::array<T,n> get (T start = 0, T step = 1, std::array<T,n> p = {{}})
            {
                return (p[c] = (start+(step * c))), range<T,n,c-1>::get(start, step, p);
            }

        protected:
            const T start;
            const T step;
    };

    template<typename T, std::size_t n>
    class range<T,n,0>
    {
        public:
            constexpr static std::array<T,n> get (T start = 0, T step = 1, std::array<T,n> p = {{}})
            {
                return (p[0] = start), p;
            }
    };

    template<typename T, T start, T end>
    constexpr static inline std::array<T,1+((end-start) < 0 ? (start-end) : (end-start))> sequence (void)
    {
        return range<T,1+((end-start) < 0 ? (start-end) : (end-start))>::get (start, (end-start) < 0 ? -1 : 1);
    }
};

#endif
