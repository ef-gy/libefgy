/**\file
 * \brief Geometry type factory
 *
 * New geometric primitives are added ever so often and adding new types always
 * seems to require changing minor details in the frontend. The templates in
 * this class are an attempt at providing a simple interface to the primitives
 * that does not require application code, such as Topologic, to know which ids
 * belong to which primitive templates.
 *
 * Adding new types will still require recompiling an application, but it
 * should no longer require code changes other than UI cosmetics.
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

#if !defined(EF_GY_FACTORY_H)
#define EF_GY_FACTORY_H

#include <ef.gy/render-null.h>
#include <ef.gy/parametric.h>
#include <ef.gy/flame.h>

namespace efgy
{
    namespace geometry
    {
        namespace functor
        {
            template<typename Q, template <class,unsigned int,class,unsigned int> class T, unsigned int d, unsigned int e>
            class echo
            {
                public:
                    typedef std::ostream &argument;

                    bool operator () (argument &out)
                    {
                        out << d << "-" << T<Q,d,render::null<Q,e>,e>::id() << "@" << e << "\n";
                        return true;
                    }
            };
        };

        template<typename Q, template <class,unsigned int,class,unsigned int> class T,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 unsigned int d, unsigned int e = d>
        class model
        {
            public:
                static bool with (typename func<Q,T,d,e>::argument &arg, const unsigned int &dims = 0, const unsigned int &rdims = 0)
                {
                    if (d < T<Q,d,render::null<Q,e>,e>::modelDimensionMinimum)
                    {
                        return (dims == 0);
                    }

                    if (   (T<Q,d,render::null<Q,e>,e>::modelDimensionMaximum > 0)
                        && (d > T<Q,d,render::null<Q,e>,e>::modelDimensionMaximum))
                    {
                        return model<Q,T,func,d-1,e>::with (arg, dims, rdims);
                    }

                    if (e < T<Q,d,render::null<Q,e>,e>::renderDimensionMinimum)
                    {
                        return (rdims == 0);
                    }

                    if (   (T<Q,d,render::null<Q,e>,e>::renderDimensionMaximum > 0)
                        && (e > T<Q,d,render::null<Q,e>,e>::renderDimensionMaximum))
                    {
                        return model<Q,T,func,d,e-1>::with (arg, dims, rdims);
                    }

                    if (rdims == 0)
                    {
                        if (dims == 0)
                        {
                            func<Q,T,d,e>()(arg);
                            (void) model<Q,T,func,d,e-1>::with (arg, dims, rdims);
                            return model<Q,T,func,d-1,e>::with (arg, dims, rdims);
                        }
                        else if (d == dims)
                        {
                            func<Q,T,d,e>()(arg);
                            return model<Q,T,func,d,e-1>::with (arg, dims, rdims);
                        }
                        else if (d < dims)
                        {
                            return (dims == 0);
                        }
                        else
                        {
                            return model<Q,T,func,d-1,e>::with (arg, dims, rdims);
                        }
                    }
                    else if (e == rdims)
                    {
                        if (dims == 0)
                        {
                            func<Q,T,d,e>()(arg);
                            return model<Q,T,func,d-1,e>::with (arg, dims, rdims);
                        }
                        else if (d == dims)
                        {
                            func<Q,T,d,e>()(arg);
                            return model<Q,T,func,d,e-1>::with (arg, dims, rdims);
                        }
                        else if (d < dims)
                        {
                            return (dims == 0);
                        }
                        else
                        {
                            return model<Q,T,func,d-1,e>::with (arg, dims, rdims);
                        }
                    }
                    else if (e < rdims)
                    {
                        return (rdims == 0);
                    }
                    else
                    {
                        return model<Q,T,func,d,e-1>::with (arg, dims, rdims);
                    }
                }
        };

        template<typename Q, template <class,unsigned int,class,unsigned int> class T,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 unsigned int d>
        class model<Q,T,func,d,2>
        {
            public:
                static bool with (typename func<Q,T,d,2>::argument &, const unsigned int &, const unsigned int &rdims)
                {
                    return (rdims == 0);
                }
        };

        template<typename Q, template <class,unsigned int,class,unsigned int> class T,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 unsigned int e>
        class model<Q,T,func,1,e>
        {
            public:
                static bool with (typename func<Q,T,1,e>::argument &, const unsigned int &dims, const unsigned int &)
                {
                    return (dims == 0);
                }
        };
    };
};

#endif
