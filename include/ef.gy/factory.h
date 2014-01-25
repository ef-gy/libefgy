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
#include <sstream>

namespace efgy
{
    namespace geometry
    {
        /**\brief Assorted functors
         *
         * Contains functors for use with other geometric templates. Currently
         * only used for functors that are passed to geometry::with().
         */
        namespace functor
        {
            /**\brief Geometry factory functor: echo qualified model names
             *
             * This is a sample functor that echoes all the matching types to a
             * standard output stream.
             *
             * \tparam Q Base data type for calculations
             * \tparam T Model template, e.g. efgy::geometry::cube
             * \tparam d Model depth, e.g. 4 for a tesseract
             * \tparam e Model render depth, e.g. >= 4 when rendering a tesseract
             */
            template<typename Q, template <class,unsigned int,class,unsigned int> class T, unsigned int d, unsigned int e>
            class echo
            {
                public:
                    /**\brief Argument type
                     *
                     * This functor takes the stream to output to as its
                     * argument.
                     */
                    typedef std::ostream &argument;

                    /**\brief Return type
                     *
                     * This functor returns a reference to the stream that was
                     * provided as an argument to the function.
                     */
                    typedef std::ostream &output;

                    /**\brief Echo model parameters to stream
                     *
                     * This method will echo the model, along with the template
                     * parameters, to the provided stream.
                     *
                     * \param[out] out The stream to write to.
                     *
                     * \returns out, after writing the model information to it.
                     */
                    static output apply (argument out)
                    {
                        return out << d << "-" << T<Q,d,render::null<Q,e>,e>::id() << "@" << e << "\n";
                    }

                    /**\brief Pass argument
                     *
                     * Simply returns the provided argument verbatim; no further
                     * processing is performed.
                     *
                     * \param[out] out Not used in this function.
                     *
                     * \returns out, without modifications.
                     */
                    static output pass (argument out)
                    {
                        return out;
                    }
            };

            /**\brief Geometry factory functor: add model names to set
             *
             * Another sample functor; this one adds all the model types that
             * are encountered to a set of strings.
             *
             * \tparam Q Base data type for calculations
             * \tparam T Model template, e.g. efgy::geometry::cube
             * \tparam d Model depth, e.g. 4 for a tesseract
             * \tparam e Model render depth, e.g. >= 4 when rendering a tesseract
             */
            template<typename Q, template <class,unsigned int,class,unsigned int> class T, unsigned int d, unsigned int e>
            class models
            {
                public:
                    /**\brief Argument type
                     *
                     * This functor takes a reference to a set of strings.
                     */
                    typedef std::set<const char *> &argument;

                    /**\brief Return type
                     *
                     * This functor returns a reference to the given set.
                     */
                    typedef std::set<const char *> &output;

                    /**\brief Add model ID to set
                     *
                     * Adds the current model's ID to the std::set that is
                     * provided as the argument to this function. Useful when
                     * calling geometry::with() with the model name set to "*",
                     * which would return all the available models.
                     *
                     * \param[out] out The set to modify.
                     *
                     * \returns out, after adding the new model ID.
                     */
                    static output apply (argument out)
                    {
                        out.insert(T<Q,d,render::null<Q,e>,e>::id());
                        return out;
                    }

                    /**\copydoc echo::pass */
                    static output pass (argument out)
                    {
                        return out;
                    }
            };

            /**\brief Geometry factory functor: add model names w/ depth to set
             * \copydetails models
             *
             * This variant of the 'models' functor also adds the valid depths
             * to the strings so that they're of the form 'depth-name'.
             */
            template<typename Q, template <class,unsigned int,class,unsigned int> class T, unsigned int d, unsigned int e>
            class modelsWithDepth
            {
                public:
                    /**\copydoc models::argument */
                    typedef std::set<std::string> &argument;

                    /**\copydoc models::output */
                    typedef std::set<std::string> &output;

                    /**\brief Add model ID to set
                     *
                     * Adds the current model's depth and ID to the std::set
                     * that is provided as the argument to this function. Useful
                     * when calling geometry::with() with the model name set to
                     * "*", which would return all the available models along
                     * any valid depths they can be used in.
                     *
                     * \param[out] out The set to modify.
                     *
                     * \returns out, after adding the new string.
                     */
                    static output apply (argument out)
                    {
                        std::ostringstream os;
                        os        << T<Q,d,render::null<Q,e>,e>::depth()
                           << "-" << T<Q,d,render::null<Q,e>,e>::id();
                        out.insert(os.str());
                        return out;
                    }

                    /**\copydoc models::pass */
                    static output pass (argument out)
                    {
                        return out;
                    }
            };
        };

        /**\brief Model factory helper
         *
         * A helper for geometry::with, which provides a ::with method that has
         * the type already specified as a template argument.
         *
         * \tparam Q    Base data type for calculations
         * \tparam func The function to call.
         * \tparam T    Model template, e.g. efgy::geometry::cube
         * \tparam d    Model depth, e.g. 4 for a tesseract
         * \tparam e    Model render depth, e.g. >= 4 when rendering a tesseract
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 template <class,unsigned int,class,unsigned int> class T,
                 unsigned int d, unsigned int e = d>
        class model
        {
            public:
                /**\brief Call func with parameters
                 *
                 * Calls func::pass(arg) or func::apply(arg), depending on
                 * whether the function can resolve the conundrum posed by the
                 * dimensionality parameters.
                 *
                 * \param[out] arg   The argument to func::...().
                 * \param[in]  dims  The target number of model dimensions.
                 * \param[in]  rdims The target number of render dimensions.
                 *
                 * \returns The return value of either func::pass or
                 *          func::apply, depending on whether the call
                 *          'succeeded'.
                 */
                constexpr static typename func<Q,T,d,e>::output with
                    (typename func<Q,T,d,e>::argument arg,
                     const unsigned int &dims = 0,
                     const unsigned int &rdims = 0)
                {
                    return d < T<Q,d,render::null<Q,e>,e>::dimensions::modelDimensionMinimum ? func<Q,T,d,e>::pass(arg)
                         : (T<Q,d,render::null<Q,e>,e>::dimensions::modelDimensionMaximum > 0) && (d > T<Q,d,render::null<Q,e>,e>::dimensions::modelDimensionMaximum) ? model<Q,func,T,d-1,e>::with (arg, dims, rdims)
                         : e < T<Q,d,render::null<Q,e>,e>::dimensions::renderDimensionMinimum ? func<Q,T,d,e>::pass(arg)
                         : (T<Q,d,render::null<Q,e>,e>::dimensions::renderDimensionMaximum > 0) && (e > T<Q,d,render::null<Q,e>,e>::dimensions::renderDimensionMaximum) ? model<Q,func,T,d,e-1>::with (arg, dims, rdims)
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
         * \tparam Q    Base data type for calculations
         * \tparam func The function to call.
         * \tparam T    Model template, e.g. efgy::geometry::cube
         * \tparam d    Model depth, e.g. 4 for a tesseract
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
                 * arguments but the first are ignored and this fix point will
                 * simply return the result of applying func's pass method to
                 * the first argument.
                 *
                 * \returns func::pass(arg).
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
         * \tparam Q    Base data type for calculations
         * \tparam func The function to call.
         * \tparam T    Model template, e.g. efgy::geometry::cube
         * \tparam e    Model render depth, e.g. >= 4 when rendering a tesseract
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

        /**\brief Call template function with class type
         *
         * Part of the geometric model type factory; called by the
         * geometry::with function given a type to check if the type is
         * compatible with the given parameters.
         *
         * \tparam Q    Base datatype for model geometry.
         * \tparam func The function to call.
         * \tparam d    Maximum number of model dimensions.
         * \tparam e    Maximum number of render dimensions.
         * \tparam T    Model template, e.g. efgy::geometry::cube
         *
         * \param[out] arg   The argument to func::...().
         * \param[in]  type  The name to check, or "*" which always matches.
         * \param[in]  dims  The target number of model dimensions.
         * \param[in]  rdims The target number of render dimensions.
         *
         * \returns Whatever func::pass or func::with returns.
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 unsigned int d,
                 unsigned int e,
                 template <class,unsigned int,class,unsigned int> class T>
        static inline typename func<Q,T,d,e>::output with
            (typename func<Q,T,d,e>::argument arg,
             const std::string &type,
             const unsigned int &dims,
             const unsigned int &rdims)
        {
            return ((type == "*") || (type == T<Q,d,render::null<Q,e>,e>::id()))
                 ? model<Q,func,T,d,e>::with(arg, dims, rdims)
                 : func<Q,T,d,e>::pass(arg);
        }

        /**\brief Model factory helper for parametric formulae
         *
         * Provides a function that, given a parametric formula, provides a
         * with() method that checks if the given parameters apply to a
         * parametric model with that formula and dispatches as usual. We sadly
         * need to provide a class template for this, because we need to
         * synthesise a type on the fly and we can't do that in an ordinary
         * function.
         *
         * \tparam Q    Base datatype for model geometry.
         * \tparam func The function to call.
         * \tparam d    Maximum number of model dimensions.
         * \tparam e    Maximum number of render dimensions.
         * \tparam T    Parametric formula template, e.g. formula::plane
         */
        template <typename Q,
                  template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                  unsigned int d,
                  unsigned int e,
                  template <class,unsigned int,unsigned int> class T>
        class parametricFactory
        {
            public:
                template <typename Qm, unsigned int odm, typename renderm, unsigned int dm>
                using parametricModel = parametric<Qm,odm,dm,T,renderm>;

                typedef typename func<Q,parametricModel,d,e>::output output;
                typedef typename func<Q,parametricModel,d,e>::argument argument;

                /**\brief Call template function with class type
                 *
                 * Part of the geometric model type factory; called by the
                 * geometry::with function given a type to check if the type is
                 * compatible with the given parameters.
                 *
                 * \param[out] arg   The argument to func::...().
                 * \param[in]  type  The name to check, or "*" which always
                 *                   matches.
                 * \param[in]  dims  The target number of model dimensions.
                 * \param[in]  rdims The target number of render dimensions.
                 *
                 * \returns Whatever func::pass returns.
                 */
                static inline output with
                    (argument arg,
                     const std::string &type,
                     const unsigned int &dims,
                     const unsigned int &rdims)
                {
                    return geometry::with<Q,func,d,e,parametricModel>(arg,type,dims,rdims);
                }
        };

        /**\brief Call template function with parametric formula
         *
         * This is a wrapper for parametricFactory<T>::with for sheer
         * convenience.
         *
         * \tparam Q    Base datatype for model geometry.
         * \tparam func The function to call.
         * \tparam d    Maximum number of model dimensions.
         * \tparam e    Maximum number of render dimensions.
         * \tparam T    Parametric formula template, e.g. formula::plane
         *
         * \param[out] arg   The argument to func::...().
         * \param[in]  type  The name to check, or "*" which always matches.
         * \param[in]  dims  The target number of model dimensions.
         * \param[in]  rdims The target number of render dimensions.
         *
         * \returns Whatever func::pass or func::with returns.
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,class,unsigned int> class, unsigned int, unsigned int> class func,
                 unsigned int d,
                 unsigned int e,
                 template <class,unsigned int,unsigned int> class T>
        static inline typename parametricFactory<Q,func,d,e,T>::output with
            (typename parametricFactory<Q,func,d,e,T>::argument arg,
             const std::string &type,
             const unsigned int &dims,
             const unsigned int &rdims)
        {
            return parametricFactory<Q,func,d,e,T>::with(arg,type,dims,rdims);
        }

        /**\brief Call template function with geometric type(s)
         *
         * This is the main entry function for the geometric type factory. This
         * function allows you to specify the type of geometric primitive that
         * you're interested in at run time, as well as the dimensional
         * parameters for those primitives.
         *
         * \tparam Q    Base datatype for model geometry.
         * \tparam func The function to call.
         * \tparam d    Maximum number of model dimensions.
         * \tparam e    Maximum number of render dimensions.
         *
         * \param[out] arg   The argument to func::...().
         * \param[in]  type  The model to pass to func, or "*" for "all models".
         * \param[in]  dims  The target number of model dimensions.
         * \param[in]  rdims The target number of render dimensions.
         *
         * \returns Whatever func::pass returns.
         */
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
            with<Q,func,d,e,simplex>(arg,type,dims,rdims);
            with<Q,func,d,e,plane>(arg,type,dims,rdims);
            with<Q,func,d,e,cube>(arg,type,dims,rdims);
            with<Q,func,d,e,formula::sphere>(arg,type,dims,rdims);
            with<Q,func,d,e,formula::torus>(arg,type,dims,rdims);
            with<Q,func,d,e,formula::moebiusStrip>(arg,type,dims,rdims);
            with<Q,func,d,e,formula::kleinBagel>(arg,type,dims,rdims);
            with<Q,func,d,e,formula::kleinBottle>(arg,type,dims,rdims);
            with<Q,func,d,e,sierpinski::gasket>(arg,type,dims,rdims);
            with<Q,func,d,e,sierpinski::carpet>(arg,type,dims,rdims);
            with<Q,func,d,e,randomAffineIFS>(arg,type,dims,rdims);
            with<Q,func,d,e,flame::random>(arg,type,dims,rdims);

            return func<Q,cube,d,e>::pass(arg);
        }
    };
};

#endif
