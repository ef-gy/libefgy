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
                    typedef std::ostream &output;

                    output operator () (argument out)
                    {
                        return out << d << "-" << T<Q,d,render::null<Q,e>,e>::id() << "@" << e << "\n";
                    }
            };
        };

        template<typename Q, template <class,unsigned int,class,unsigned int> class T,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 unsigned int d, unsigned int e = d>
        class model
        {
            public:
                constexpr static typename func<Q,T,d,e>::output with
                    (typename func<Q,T,d,e>::argument &arg,
                     const unsigned int &dims = 0,
                     const unsigned int &rdims = 0)
                {
                    return d < T<Q,d,render::null<Q,e>,e>::modelDimensionMinimum ? arg
                         : (T<Q,d,render::null<Q,e>,e>::modelDimensionMaximum > 0) && (d > T<Q,d,render::null<Q,e>,e>::modelDimensionMaximum) ? model<Q,T,func,d-1,e>::with (arg, dims, rdims)
                         : e < T<Q,d,render::null<Q,e>,e>::renderDimensionMinimum ? arg
                         : (T<Q,d,render::null<Q,e>,e>::renderDimensionMaximum > 0) && (e > T<Q,d,render::null<Q,e>,e>::renderDimensionMaximum) ? model<Q,T,func,d,e-1>::with (arg, dims, rdims)
                         : 0 == rdims
                            ? (   0 == dims ? func<Q,T,d,e>()(arg), model<Q,T,func,d,e-1>::with (arg, dims, rdims), model<Q,T,func,d-1,e>::with (arg, dims, rdims)
                                : d == dims ? func<Q,T,d,e>()(arg), model<Q,T,func,d,e-1>::with (arg, dims, rdims)
                                : d  < dims ? arg
                                : model<Q,T,func,d-1,e>::with (arg, dims, rdims) )
                         : e == rdims
                            ? (   0 == dims ? func<Q,T,d,e>()(arg), model<Q,T,func,d-1,e>::with (arg, dims, rdims)
                                : d == dims ? func<Q,T,d,e>()(arg), model<Q,T,func,d,e-1>::with (arg, dims, rdims)
                                : d  < dims ? arg
                                : model<Q,T,func,d-1,e>::with (arg, dims, rdims) )
                         : e < rdims ? arg
                         : model<Q,T,func,d,e-1>::with (arg, dims, rdims);
                }
        };

        template<typename Q, template <class,unsigned int,class,unsigned int> class T,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 unsigned int d>
        class model<Q,T,func,d,2>
        {
            public:
                constexpr static typename func<Q,T,d,2>::output with
                    (typename func<Q,T,d,2>::argument &arg,
                     const unsigned int &,
                     const unsigned int &)
                {
                    return arg;
                }
        };

        template<typename Q, template <class,unsigned int,class,unsigned int> class T,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 unsigned int e>
        class model<Q,T,func,1,e>
        {
            public:
                constexpr static typename func<Q,T,1,e>::output with
                    (typename func<Q,T,1,e>::argument &arg,
                     const unsigned int &,
                     const unsigned int &)
                {
                    return arg;
                }
        };
    };
};

#endif
