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

#if !defined(EF_GY_PRIMITIVE_H)
#define EF_GY_PRIMITIVE_H

#include <cmath>

namespace efgy
{
    namespace math
    {
        template <typename Q, typename Tinteger = unsigned long>
        class primitive
        {
            public:
                typedef Tinteger integer;

                primitive ()
                    : data(Q(0))
                    {}
                primitive (const Q &pQ)
                    : data(pQ)
                    {}
                primitive (const primitive &pQ)
                    : data(pQ.data)
                    {}

                operator Q () const
                {
                    return data;
                }

                primitive &operator = (const Q &pQ)
                {
                    data = pQ;
                    return *this;
                }
                primitive &operator = (const primitive &pQ)
                {
                    data = pQ.data;
                    return *this;
                }

                primitive operator - () const
                {
                    return primitive(-data);
                }

                primitive operator + (const Q &pQ) const
                {
                    return primitive(data + pQ);
                }
                primitive operator - (const Q &pQ) const
                {
                    return primitive(data - pQ);
                }
                primitive operator * (const Q &pQ) const
                {
                    return primitive(data * pQ);
                }
                primitive operator / (const Q &pQ) const
                {
                    return primitive(data / pQ);
                }
                primitive operator % (const Q &pQ) const
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

                bool operator == (const Q &pQ) const
                {
                    return data == pQ;
                }
                bool operator != (const Q &pQ) const
                {
                    return data != pQ;
                }
                bool operator < (const Q &pQ) const
                {
                    return data < pQ;
                }
                bool operator > (const Q &pQ) const
                {
                    return data > pQ;
                }
                bool operator <= (const Q &pQ) const
                {
                    return data <= pQ;
                }
                bool operator >= (const Q &pQ) const
                {
                    return data >= pQ;
                }

                primitive operator + (const primitive &pQ) const
                {
                    return primitive(data + pQ.data);
                }
                primitive operator - (const primitive &pQ) const
                {
                    return primitive(data - pQ.data);
                }
                primitive operator * (const primitive &pQ) const
                {
                    return primitive(data * pQ.data);
                }
                primitive operator / (const primitive &pQ) const
                {
                    return primitive(data / pQ.data);
                }
                primitive operator % (const primitive &pQ) const
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

                bool operator == (const primitive &pQ) const
                {
                    return data == pQ.data;
                }
                bool operator != (const primitive &pQ) const
                {
                    return data != pQ.data;
                }
                bool operator < (const primitive &pQ) const
                {
                    return data < pQ.data;
                }
                bool operator > (const primitive &pQ) const
                {
                    return data > pQ.data;
                }
                bool operator <= (const primitive &pQ) const
                {
                    return data <= pQ.data;
                }
                bool operator >= (const primitive &pQ) const
                {
                    return data >= pQ.data;
                }

                primitive operator ^ (const Q &pQ)
                {
                    return primitive(pow(data, pQ));
                }
                primitive operator ^ (const primitive &pQ)
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

                Q data;
        };

        template <typename Q, typename u>
        primitive<Q,u> sqrt (const primitive<Q,u> &vA)
        {
            return primitive<Q,u>(std::sqrt(vA.data));
        }

        template <typename Q, typename u>
        primitive<Q,u> sin (const primitive<Q,u> &vA)
        {
            return primitive<Q,u>(std::sin(vA.data));
        }

        template <typename Q, typename u>
        primitive<Q,u> cos (const primitive<Q,u> &vA)
        {
            return primitive<Q,u>(std::cos(vA.data));
        }

        template <typename Q, typename u>
        primitive<Q,u> tan (const primitive<Q,u> &vA)
        {
            return primitive<Q,u>(std::tan(vA.data));
        }

        template <typename Q, typename u>
        primitive<Q,u> atan (const primitive<Q,u> &vA)
        {
            return primitive<Q,u>(std::atan(vA.data));
        }

        template <typename Q, typename u>
        primitive<Q,u> atan2 (const primitive<Q,u> &vA, const primitive<Q,u> &vB)
        {
            return primitive<Q,u>(std::atan2(vA.data, vB.data));
        }
    };
};

#endif
