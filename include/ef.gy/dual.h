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

#if !defined(EF_GY_DUAL_H)
#define EF_GY_DUAL_H

namespace efgy
{
    namespace math
    {
        template <typename Q, typename R>
        class dual
        {
            public:
                enum type
                {
                    ctQ,
                    ctR
                } contentType;

                dual ()
                    : contentType(ctQ), dataQ(0)
                    {}
                dual (const Q &pQ)
                    : contentType(ctQ), dataQ(pQ)
                    {}
                dual (const R &pR)
                    : contentType(ctR), dataR(pR)
                    {}

                dual &operator = (const Q &pQ)
                {
                    contentType = ctQ;
                    dataQ = pQ;
                    return *this;
                }
                dual &operator = (const R &pR)
                {
                    contentType = ctR;
                    dataR = pR;
                    return *this;
                }
                dual operator - () const
                {
                    switch (contentType)
                    {
                        case ctQ:
                            return dual(-dataQ);
                        case ctR:
                            return dual(-dataR);
                    }
                }

                dual operator + (const Q &pQ) const
                {
                    switch (contentType)
                    {
                        case ctQ:
                            return dual(dataQ + pQ);
                        case ctR:
                            return dual(dataR + pQ);
                    }
                }
                dual operator - (const Q &pQ) const
                {
                    switch (contentType)
                    {
                        case ctQ:
                            return dual(dataQ - pQ);
                        case ctR:
                            return dual(dataR - pQ);
                    }
                }
                dual operator * (const Q &pQ) const
                {
                    switch (contentType)
                    {
                        case ctQ:
                            return dual(dataQ * pQ);
                        case ctR:
                            return dual(dataR * pQ);
                    }
                }
                dual operator + (const R &pR) const
                {
                    switch (contentType)
                    {
                        case ctQ:
                            return dual(dataQ + pR);
                        case ctR:
                            return dual(dataR + pR);
                    }
                }
                dual operator - (const R &pR) const
                {
                    switch (contentType)
                    {
                        case ctQ:
                            return dual(dataQ - pR);
                        case ctR:
                            return dual(dataR - pR);
                    }
                }
                dual operator * (const R &pR) const
                {
                    switch (contentType)
                    {
                        case ctQ:
                            return dual(dataQ * pR);
                        case ctR:
                            return dual(dataR * pR);
                    }
                }
                dual operator + (const dual &pD) const
                {
                    switch (pD.contentType)
                    {
                        case ctQ:
                            return *this + pD.dataQ;
                        case ctR:
                            return *this + pD.dataR;
                    }
                }
                dual operator - (const dual &pD) const
                {
                    switch (pD.contentType)
                    {
                        case ctQ:
                            return *this - pD.dataQ;
                        case ctR:
                            return *this - pD.dataR;
                    }
                }
                dual operator * (const dual &pD) const
                {
                    switch (pD.contentType)
                    {
                        case ctQ:
                            return *this * pD.dataQ;
                        case ctR:
                            return *this * pD.dataR;
                    }
                }

                dual &operator += (const Q &pQ)
                {
                    switch (contentType)
                    {
                        case ctQ:
                            dataQ += pQ;
                            break;
                        case ctR:
                            dataR += pQ;
                            break;
                    }
                    return *this;
                }
                dual &operator -= (const Q &pQ)
                {
                    switch (contentType)
                    {
                        case ctQ:
                            dataQ -= pQ;
                            break;
                        case ctR:
                            dataR -= pQ;
                            break;
                    }
                    return *this;
                }
                dual &operator *= (const Q &pQ)
                {
                    switch (contentType)
                    {
                        case ctQ:
                            dataQ *= pQ;
                            break;
                        case ctR:
                            dataR *= pQ;
                            break;
                    }
                    return *this;
                }
                dual &operator += (const R &pR)
                {
                    switch (contentType)
                    {
                        case ctQ:
                            dataQ += pR;
                            break;
                        case ctR:
                            dataR += pR;
                            break;
                    }
                    return *this;
                }
                dual &operator -= (const R &pR)
                {
                    switch (contentType)
                    {
                        case ctQ:
                            dataQ -= pR;
                            break;
                        case ctR:
                            dataR -= pR;
                            break;
                    }
                    return *this;
                }
                dual &operator *= (const R &pR)
                {
                    switch (contentType)
                    {
                        case ctQ:
                            dataQ *= pR;
                            break;
                        case ctR:
                            dataR *= pR;
                            break;
                    }
                    return *this;
                }
                dual &operator += (const dual &pD)
                {
                    switch (pD.contentType)
                    {
                        case ctQ:
                            return *this += pD.dataQ;
                        case ctR:
                            return *this += pD.dataR;
                    }
                }
                dual &operator -= (const dual &pD)
                {
                    switch (pD.contentType)
                    {
                        case ctQ:
                            return *this -= pD.dataQ;
                        case ctR:
                            return *this -= pD.dataR;
                    }
                }
                dual &operator *= (const dual &pD)
                {
                    switch (pD.contentType)
                    {
                        case ctQ:
                            return *this *= pD.dataQ;
                        case ctR:
                            return *this *= pD.dataR;
                    }
                }

                Q dataQ;
                R dataR;
        };
    };
};

#endif
