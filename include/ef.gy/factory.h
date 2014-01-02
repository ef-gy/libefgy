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

#include <set>
#include <string>
#include <iostream>

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

                    static output apply (argument out)
                    {
                        return out << d << "-" << T<Q,d,render::null<Q,e>,e>::id() << "@" << e << "\n";
                    }

                    static output pass (argument out)
                    {
                        return out;
                    }
            };

            template<typename Q, template <class,unsigned int,class,unsigned int> class T, unsigned int d, unsigned int e>
            class models
            {
                public:
                    typedef std::set<const char *> &argument;
                    typedef std::set<const char *> &output;

                    static output apply (argument out)
                    {
                        out.insert(T<Q,d,render::null<Q,e>,e>::id());
                        return out;
                    }

                    static output pass (argument out)
                    {
                        return out;
                    }
            };
        };

        /**
         * \tparam Q Base data type for calculations
         * \tparam T Model template, e.g. efgy::geometry::cube
         * \tparam d Model depth, e.g. 4 for a tesseract
         * \tparam e Model render depth, e.g. >= 4 when rendering a tesseract
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 template <class,unsigned int,class,unsigned int> class T,
                 unsigned int d, unsigned int e = d>
        class model
        {
            public:
                constexpr static typename func<Q,T,d,e>::output with
                    (typename func<Q,T,d,e>::argument arg,
                     const unsigned int &dims = 0,
                     const unsigned int &rdims = 0)
                {
                    return d < T<Q,d,render::null<Q,e>,e>::modelDimensionMinimum ? func<Q,T,d,e>::pass(arg)
                         : (T<Q,d,render::null<Q,e>,e>::modelDimensionMaximum > 0) && (d > T<Q,d,render::null<Q,e>,e>::modelDimensionMaximum) ? model<Q,func,T,d-1,e>::with (arg, dims, rdims)
                         : e < T<Q,d,render::null<Q,e>,e>::renderDimensionMinimum ? func<Q,T,d,e>::pass(arg)
                         : (T<Q,d,render::null<Q,e>,e>::renderDimensionMaximum > 0) && (e > T<Q,d,render::null<Q,e>,e>::renderDimensionMaximum) ? model<Q,func,T,d,e-1>::with (arg, dims, rdims)
                         : 0 == rdims
                            ? (   0 == dims ? func<Q,T,d,e>::apply(arg), model<Q,func,T,d,e-1>::with (arg, dims, rdims), model<Q,func,T,d-1,e>::with (arg, dims, rdims)
                                : d == dims ? func<Q,T,d,e>::apply(arg), model<Q,func,T,d,e-1>::with (arg, dims, rdims)
                                : d  < dims ? func<Q,T,d,e>::pass(arg)
                                : model<Q,func,T,d-1,e>::with (arg, dims, rdims) )
                         : e == rdims
                            ? (   0 == dims ? func<Q,T,d,e>::apply(arg), model<Q,func,T,d-1,e>::with (arg, dims, rdims)
                                : d == dims ? func<Q,T,d,e>::apply(arg), model<Q,func,T,d,e-1>::with (arg, dims, rdims)
                                : d  < dims ? func<Q,T,d,e>::pass(arg)
                                : model<Q,func,T,d-1,e>::with (arg, dims, rdims) )
                         : e < rdims ? func<Q,T,d,e>::pass(arg)
                         : model<Q,func,T,d,e-1>::with (arg, dims, rdims);
                }
        };

        /**\brief Model factory helper; e=2 fix point
         *
         * The model factory helper, geometry::model, works by calling itself
         * recursively with different template parameters. This is one of these
         * parameters' fix points, which prevents an infinite template
         * recursion.
         *
         * \tparam Q Base data type for calculations
         * \tparam T Model template, e.g. efgy::geometry::cube
         * \tparam d Model depth, e.g. 4 for a tesseract
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 template <class,unsigned int,class,unsigned int> class T,
                 unsigned int d>
        class model<Q,func,T,d,2>
        {
            public:
                /**\brief Call func with parameters; e=2 fix point
                 *
                 * This is a fix point stub of the model::with() method; all
                 * arguments are ignored and this fix point will simply return
                 * the first argument. The func template parameter is not
                 * called.
                 *
                 * \returns Always the first argument.
                 */
                constexpr static typename func<Q,T,d,2>::output with
                    (typename func<Q,T,d,2>::argument arg,
                     const unsigned int &,
                     const unsigned int &)
                {
                    return func<Q,T,d,2>::pass(arg);
                }
        };

        /**\brief Model factory helper; d=1 fix point
         *
         * The model factory helper, geometry::model, works by calling itself
         * recursively with different template parameters. This is one of these
         * parameters' fix points, which prevents an infinite template
         * recursion.
         *
         * \tparam Q Base data type for calculations
         * \tparam T Model template, e.g. efgy::geometry::cube
         * \tparam e Model render depth, e.g. >= 4 when rendering a tesseract
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 template <class,unsigned int,class,unsigned int> class T,
                 unsigned int e>
        class model<Q,func,T,1,e>
        {
            public:
                /**\brief Call func with parameters; d=1 fix point
                 *
                 * This is a fix point stub of the model::with() method; all
                 * arguments but the first are ignored and this fix point will
                 * simply return the result of applying func's pass method to
                 * the first argument.
                 *
                 * \returns func::pass(arg).
                 */
                constexpr static typename func<Q,T,1,e>::output with
                    (typename func<Q,T,1,e>::argument arg,
                     const unsigned int &,
                     const unsigned int &)
                {
                    return func<Q,T,1,e>::pass(arg);
                }
        };

        template<typename Q,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 unsigned int d,
                 unsigned int e = d>
        static inline typename func<Q,cube,d,e>::output with
            (typename func<Q,cube,d,e>::argument arg,
             const std::string &type,
             const unsigned int &dims,
             const unsigned int &rdims)
        {
            if ((type == "*") || (type == "simplex"))
                model<Q,func,simplex,d,e>::with(arg, dims, rdims);
            if ((type == "*") || (type == "plane"))
                model<Q,func,plane,d,e>::with(arg, dims, rdims);
            if ((type == "*") || (type == "cube"))
                model<Q,func,cube,d,e>::with(arg, dims, rdims);
            if ((type == "*") || (type == "sphere"))
                model<Q,func,sphere,d,e>::with(arg, dims, rdims);
            if ((type == "*") || (type == "moebius-strip"))
                model<Q,func,moebiusStrip,d,e>::with(arg, dims, rdims);
            if ((type == "*") || (type == "klein-bagel"))
                model<Q,func,kleinBagel,d,e>::with(arg, dims, rdims);
            if ((type == "*") || (type == "sierpinski-gasket"))
                model<Q,func,sierpinski::gasket,d,e>::with(arg, dims, rdims);
            if ((type == "*") || (type == "sierpinski-carpet"))
                model<Q,func,sierpinski::carpet,d,e>::with(arg, dims, rdims);
            if ((type == "*") || (type == "random-affine-ifs"))
                model<Q,func,randomAffineIFS,d,e>::with(arg, dims, rdims);
            if ((type == "*") || (type == "random-flame"))
                model<Q,func,flame::random,d,e>::with(arg, dims, rdims);

            return func<Q,cube,d,e>::pass(arg);
        }
    };
};

#endif
