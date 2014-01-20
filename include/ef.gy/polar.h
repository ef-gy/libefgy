/**\file
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

#if !defined(EF_GY_POLAR_H)
#define EF_GY_POLAR_H

#include <ef.gy/euclidian.h>

namespace efgy
{
    namespace math
    {
        namespace space
        {
            /**\brief Polar coordinate space tag
             *
             * This coordinate space tag identifies vectors as containing polar
             * coordinates as opposed to regular cartesian coordinates.
             */
            class polar
            {
                public:
                    polar(unsigned long long pPrecision = 10)
                        : precision(pPrecision) {}

                    unsigned long long precision;
            };

            /**\brief Stream output operator for the polar space tag
             *
             * Writes the contents of a polar space tag to an output stream;
             * Polar space tags contain a precision parameter, which is also
             * written to the provided stream.
             *
             * \tparam C The character type of the stream.
             *
             * \param[out] stream The stream to write to.
             * \param[in]  space  The space tag to write to the stream.
             *
             + \returns The parameter 'stream' after writing to it.
             */
            template <typename C>
            constexpr inline std::basic_ostream<C> &operator << (std::basic_ostream<C> &stream, const polar &s)
            {
                return stream << "[polar:" << s.precision << "]";
            }
        };

        template <typename F, unsigned int n>
        class vector<F,n,space::polar> : public std::array<F,n>
        {
            public:
                /**\brief Construct with array
                 *
                 * Construct an instance of the vector with the elements
                 * specified as a C++-style array.
                 *
                 * \param[in] t The array to copy.
                 * \param[in] s An instance of the space tag; may be used to
                 *              specify a precision for conversion operations.
                 */
                constexpr vector
                    (const std::array<F,n> &t = {{}},
                     const space::polar &s = space::polar())
                    : std::array<F, n>(t),
                      spaceTag(s)
                    {}

                operator vector<F,n,space::real> () const
                {
                    vector<F,n,space::real> v;

                    for (unsigned int i = 0; i < n; i++)
                    {
                        v[i] = (*this)[0];
                    }

                    for (unsigned int i = 0; i < (n-1); i++)
                    {
                        const unsigned int p = i + 1;

                        v[i] *= cos((*this)[p]);

                        for (unsigned int j = p; j < n; j++)
                        {
                            v[j] *= sin((*this)[p]);
                        }
                    }

                    return v;
                }

                constexpr const space::polar &tag (void) const { return spaceTag; }

            protected:
                space::polar spaceTag;
        };
    }

    namespace geometry
    {
        namespace polar
        {
            template <typename F, unsigned int n>
            class space
            {
                public:
                    typedef F base;
                    typedef F scalar;
                    typedef math::vector<scalar, n, math::space::polar> vector;
            };
        };
    };
};

#endif
