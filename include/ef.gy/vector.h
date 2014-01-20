/**\file
 * \brief Generic vectors
 *
 * Contains the basic template for generic vectors.
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

#if !defined(EF_GY_VECTOR_H)
#define EF_GY_VECTOR_H

#include <ef.gy/traits.h>
#include <array>
#include <ostream>

namespace efgy
{
    namespace math
    {
        /**\brief Coordinate space tags
         *
         * Contains tags for different coordinate spaces. These are used in the
         * math::vector template to distinguish between different kinds of
         * vectors.
         */
        namespace space
        {
            /**\brief Real coordinate space tag
             *
             * This is the default coordinate space tag, which indicates that
             * the coordinate space is the typical real coordinate space.
             */
            class real {};

            /**\brief Stream output operator for the real space tag
             *
             * Writes the contents of a real space tag to an output stream;
             * since these tags do not contain any actual data, this operator
             * really just writes a string to the stream.
             *
             * \tparam C The character type of the stream.
             *
             * \param[out] stream The stream to write to.
             *
             + \returns The parameter 'stream' after writing to it.
             */
            template <typename C>
            constexpr inline std::basic_ostream<C> &operator << (std::basic_ostream<C> &stream, const real &)
            {
                return stream << "[real]";
            }
        };

        /**\brief Generic vector
         *
         * Implements a generic vector type over a field, which is tagged with a
         * coordinate space that describes what kind of vector the class
         * represents.
         *
         * \tparam F     Base type for the vector; should have field-like
         *               properties.
         * \tparam n     Number of vector elements.
         * \tparam space Coordinate space tag, defaults to space::real.
         */
        template <typename F, unsigned int n, typename space = space::real>
        class vector : public std::array<F,n>
        {
            public:
                /**\brief Construct with array
                 *
                 * Construct an instance of the vector with the elements
                 * specified as a C++-style array.
                 *
                 * \param[in] t The array to copy.
                 * \param[in] s An instance of the space tag; typically not
                 *              used, but might be used to specify additional
                 *              parameters.
                 */
                constexpr vector
                    (const std::array<F,n> &t = {{}},
                     const space &s = space())
                    : std::array<F, n>(t) {}

                constexpr const space tag (void) const { return space(); }
        };

        /**\brief Scalar multiplication
         *
         * Implements the scalar multiplication of a vector with a scalar; the
         * result in the most common vector spaces is that the vector is scaled
         * by the scalar and retains its orientation.
         *
         * \tparam F     Base type for the vector; should have field-like
         *               properties.
         * \tparam n     Number of vector elements.
         * \tparam space Coordinate space tag, defaults to space::real.
         *
         * \param[in] a The vector to scale.
         * \param[in] s The scale to apply to the vector.
         *
         * \returns The modified, scaled vector.
         */
        template <typename F, unsigned int n, typename space>
        vector<F,n,space> operator * (vector<F,n,space> a, const F &s)
        {
            for (unsigned int i = 0; i < n; i++)
            {
                a[i] *= s;
            }
            return a;
        }

        /**\brief Dot product
         *
         * Implements the generic dot product, which is the sum of all the
         * products of corresponding vector elements of both vectors.
         *
         * \tparam F     Base type for the vector; should have field-like
         *               properties.
         * \tparam n     Number of vector elements.
         * \tparam space Coordinate space tag, defaults to space::real.
         *
         * \param[in] a First input vector.
         * \param[in] b Second input vector.
         *
         * \returns The dot product of a and b.
         */
        template <typename F, unsigned int n, typename space>
        F operator * (const vector<F,n,space> &a, const vector<F,n,space> &b)
        {
            F s = F(0);
            for (unsigned int i = 0; i < n; i++)
            {
                s += a[i] * b[i];
            }
            return s;
        }

        /**\brief Scalar multiplication with reciprocal
         *
         * Analogous to the scalar multiplication, this is equivalent to
         * multiplying a vector with the reciprocal of the given vector, except
         * that you don't have to calculate this reciprocal separately. It
         * seemed like a logical extension.
         *
         * \tparam F     Base type for the vector; should have field-like
         *               properties.
         * \tparam n     Number of vector elements.
         * \tparam space Coordinate space tag, defaults to space::real.
         *
         * \param[in] a The vector to scale.
         * \param[in] s The reciprocal of this is multiplied with the vector.
         *
         * \returns The modified, scaled vector.
         */
        template <typename F, unsigned int n, typename space>
        vector<F,n,space> operator / (vector<F,n,space> a, const F &s)
        {
            for (unsigned int i = 0; i < n; i++)
            {
                a[i] /= s;
            }
            return a;
        }

        /**\brief Dot product with reciprocals of second vector
         *
         * This is an extension of the dot product where the right hand side
         * elements serve as the divisor instead of as a factor.
         *
         * \tparam F     Base type for the vector; should have field-like
         *               properties.
         * \tparam n     Number of vector elements.
         * \tparam space Coordinate space tag, defaults to space::real.
         *
         * \param[in] a First input vector.
         * \param[in] b Second input vector.
         *
         * \returns The "dot quotient" of a and b.
         */
        template <typename F, unsigned int n, typename space>
        F operator / (const vector<F,n,space> &a, const vector<F,n,space> &b)
        {
            F s = F(0);
            for (unsigned int i = 0; i < n; i++)
            {
                s += a[i] / b[i];
            }
            return s;
        }

        template <typename F, unsigned int n, typename space>
        vector<F,n,space> operator + (vector<F,n,space> a, const vector<F,n,space> &b)
        {
            for (unsigned int i = 0; i < n; i++)
            {
                a[i] += b[i];
            }
            return a;
        }

        template <typename F, unsigned int n, typename space>
        vector<F,n,space> &operator += (vector<F,n,space> &a, const vector<F,n,space> &b)
        {
            for (unsigned int i = 0; i < n; i++)
            {
                a[i] += b[i];
            }
            return a;
        }

        template <typename F, unsigned int n, typename space>
        vector<F,n,space> operator - (vector<F,n,space> a, const vector<F,n,space> &b)
        {
            for (unsigned int i = 0; i < n; i++)
            {
                a[i] -= b[i];
            }
            return a;
        }

        template <typename F, unsigned int n, typename space>
        vector<F,n,space> &operator -= (vector<F,n,space> &a, const vector<F,n,space> &b)
        {
            for (unsigned int i = 0; i < n; i++)
            {
                a[i] -= b[i];
            }
            return a;
        }

        template <typename C, typename F, unsigned int n, typename space>
        std::basic_ostream<C> &operator << (std::basic_ostream<C> &stream, const vector<F,n,space> &v)
        {
            stream << "(" << v.tag();
            for (unsigned int i = 0; i < n; i++)
            {
                stream << (i > 0 ? ", " : " ") << v[i];
            }
            return stream << ")";
        }
    };
};

#endif
