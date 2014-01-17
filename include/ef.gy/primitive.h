/**\file
 * \brief Primitive type wrapper implementation
 *
 * Contains a wrapper for primitive types - floats, doubles, etc. - which wraps
 * them in a class so they can be derived from properly.
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

#if !defined(EF_GY_PRIMITIVE_H)
#define EF_GY_PRIMITIVE_H

#include <cmath>

namespace efgy
{
    namespace math
    {
        /**\brief Primitive type wrapper
         *
         * This template wraps the specified primitive type in an object, so
         * that other can can use them as such and properly use these types as
         * base classes.
         *
         * \tparam Q        The primitive type to wrap; should be float, double
         *                  or similar.
         * \tparam Tinteger Integer type to use with the base type.
         */
        template <typename Q, typename Tinteger = unsigned long>
        class primitive
        {
            public:
                /**\brief Integer type
                 *
                 * This is a typedef to the integer base type that was passed
                 * to the template.
                 */
                typedef Tinteger integer;

                /**\brief Default constructor
                 *
                 * Initialises the object with the Q-equivalent of a zero.
                 */
                constexpr primitive (void) : data(Q(0)) {}

                /**\brief Value constructor
                 *
                 * Initialises the object with a copy of the provided value.
                 *
                 * \param[in] pQ The value to initialise the object to.
                 */
                constexpr primitive (const Q &pQ) : data(pQ) {}

                /**\brief Copy constructor
                 *
                 * Copies the value of another primitive to initialise a new
                 * instance o fthis class.
                 *
                 * \param[in] pQ The value to copy.
                 */
                constexpr primitive (const primitive &pQ) : data(pQ.data) {}

                /**\brief Cast to base type
                 *
                 * This operator returns a reference to the base type, so that
                 * this object can be used interchangeably with instances of the
                 * original primitive type.
                 *
                 * \return Const reference to the base value.
                 */
                constexpr operator const Q& (void) const { return data; }

                /**\brief Assign base value
                 *
                 * Assigns the value of the given base value to this object,
                 * then returns a reference to this object.
                 *
                 * \param[in] pQ The value to copy.
                 *
                 * \return Reference to the object.
                 */
                primitive &operator = (const Q &pQ)
                {
                    data = pQ;
                    return *this;
                }

                /**\brief Assign value of other instance
                 *
                 * Assigns the value of the given object to this object, then
                 * returns a reference to this object.
                 *
                 * \param[in] pQ The value to copy.
                 *
                 * \return Reference to the object.
                 */
                primitive &operator = (const primitive &pQ)
                {
                    data = pQ.data;
                    return *this;
                }

                /**\brief Swap sign of object
                 *
                 * Creates a new primitive object with a reversed sign. The sign
                 * is reversed with the help of the native unary minus operator.
                 *
                 * \return New object with the modified value.
                 */
                constexpr primitive operator - (void) const
                {
                    return primitive(-data);
                }

                /**\brief Add base type value
                 *
                 * Creates a new primitive object with the given parameter added
                 * to it.
                 *
                 * \param[in] pQ The value to add to this object.
                 *
                 * \return New object with the modified value.
                 */
                constexpr primitive operator + (const Q &pQ) const
                {
                    return primitive(data + pQ);
                }

                /**\brief Subtract base type value
                 *
                 * Creates a new primitive object with the given parameter
                 * subtracted from it.
                 *
                 * \param[in] pQ The value to subtract from this object.
                 *
                 * \return New object with the modified value.
                 */
                constexpr primitive operator - (const Q &pQ) const
                {
                    return primitive(data - pQ);
                }

                /**\brief Multiply with base type value
                 *
                 * Creates a new primitive object with the given parameter
                 * multiplied to it.
                 *
                 * \param[in] pQ The factor to multiply with this object.
                 *
                 * \return New object with the modified value.
                 */
                constexpr primitive operator * (const Q &pQ) const
                {
                    return primitive(data * pQ);
                }

                /**\brief Divide by base type value
                 *
                 * Creates a new primitive object by dividing the current value
                 * by the given divisor.
                 *
                 * \param[in] pQ The divisor to divide this object by.
                 *
                 * \return New object with the modified value.
                 */
                constexpr primitive operator / (const Q &pQ) const
                {
                    return primitive(data / pQ);
                }

                /**\brief Calculate remainder of division by base type value
                 *
                 * Creates a new primitive object by dividing the current value
                 * by the given divisor and then taking the remainder of that
                 * division.
                 *
                 * \param[in] pQ The divisor to divide this object by.
                 *
                 * \return New object with the modified value.
                 */
                constexpr primitive operator % (const Q &pQ) const
                {
                    return primitive(data % pQ);
                }

                primitive &operator += (const Q &pQ)
                {
                    data += pQ;
                    return *this;
                }
                primitive &operator -= (const Q &pQ)
                {
                    data -= pQ;
                    return *this;
                }
                primitive &operator *= (const Q &pQ)
                {
                    data *= pQ;
                    return *this;
                }
                primitive &operator /= (const Q &pQ)
                {
                    data /= pQ;
                    return *this;
                }
                primitive &operator %= (const Q &pQ)
                {
                    data %= pQ;
                    return *this;
                }

                constexpr bool operator == (const Q &pQ) const
                {
                    return data == pQ;
                }
                constexpr bool operator != (const Q &pQ) const
                {
                    return data != pQ;
                }
                constexpr bool operator < (const Q &pQ) const
                {
                    return data < pQ;
                }
                constexpr bool operator > (const Q &pQ) const
                {
                    return data > pQ;
                }
                constexpr bool operator <= (const Q &pQ) const
                {
                    return data <= pQ;
                }
                constexpr bool operator >= (const Q &pQ) const
                {
                    return data >= pQ;
                }

                constexpr primitive operator + (const primitive &pQ) const
                {
                    return primitive(data + pQ.data);
                }
                constexpr primitive operator - (const primitive &pQ) const
                {
                    return primitive(data - pQ.data);
                }
                constexpr primitive operator * (const primitive &pQ) const
                {
                    return primitive(data * pQ.data);
                }
                constexpr primitive operator / (const primitive &pQ) const
                {
                    return primitive(data / pQ.data);
                }
                constexpr primitive operator % (const primitive &pQ) const
                {
                    return primitive(data % pQ.data);
                }

                primitive &operator += (const primitive &pQ)
                {
                    data += pQ.data;
                    return *this;
                }
                primitive &operator -= (const primitive &pQ)
                {
                    data -= pQ.data;
                    return *this;
                }
                primitive &operator *= (const primitive &pQ)
                {
                    data *= pQ.data;
                    return *this;
                }
                primitive &operator /= (const primitive &pQ)
                {
                    data /= pQ.data;
                    return *this;
                }
                primitive &operator %= (const primitive &pQ)
                {
                    data %= pQ.data;
                    return *this;
                }

                constexpr bool operator == (const primitive &pQ) const
                {
                    return data == pQ.data;
                }
                constexpr bool operator != (const primitive &pQ) const
                {
                    return data != pQ.data;
                }
                constexpr bool operator < (const primitive &pQ) const
                {
                    return data < pQ.data;
                }
                constexpr bool operator > (const primitive &pQ) const
                {
                    return data > pQ.data;
                }
                constexpr bool operator <= (const primitive &pQ) const
                {
                    return data <= pQ.data;
                }
                constexpr bool operator >= (const primitive &pQ) const
                {
                    return data >= pQ.data;
                }

                constexpr primitive operator ^ (const Q &pQ) const
                {
                    return primitive(pow(data, pQ));
                }
                constexpr primitive operator ^ (const primitive &pQ) const
                {
                    return primitive(pow(data, pQ.data));
                }
                primitive operator ^= (const Q &pQ)
                {
                    data = pow(data, pQ);
                    return *this;
                }
                primitive operator ^= (const primitive &pQ)
                {
                    data = pow(data, pQ.data);
                    return *this;
                }

                /**\brief Actual contents
                 *
                 * This variable contains the actual contents of any variable
                 * using this class, in the specified base type.
                 */
                Q data;
        };

        template <typename Q, typename u>
        constexpr primitive<Q,u> sqrt (const primitive<Q,u> &vA)
        {
            return primitive<Q,u>(std::sqrt(vA.data));
        }

        template <typename Q, typename u>
        constexpr primitive<Q,u> sin (const primitive<Q,u> &vA)
        {
            return primitive<Q,u>(std::sin(vA.data));
        }

        template <typename Q, typename u>
        constexpr primitive<Q,u> cos (const primitive<Q,u> &vA)
        {
            return primitive<Q,u>(std::cos(vA.data));
        }

        template <typename Q, typename u>
        constexpr primitive<Q,u> tan (const primitive<Q,u> &vA)
        {
            return primitive<Q,u>(std::tan(vA.data));
        }

        template <typename Q, typename u>
        constexpr primitive<Q,u> atan (const primitive<Q,u> &vA)
        {
            return primitive<Q,u>(std::atan(vA.data));
        }

        template <typename Q, typename u>
        constexpr primitive<Q,u> atan2 (const primitive<Q,u> &vA, const primitive<Q,u> &vB)
        {
            return primitive<Q,u>(std::atan2(vA.data, vB.data));
        }
    };
};

#endif
