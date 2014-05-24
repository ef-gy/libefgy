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
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#if !defined(EF_GY_FACTORY_H)
#define EF_GY_FACTORY_H

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
             * \tparam Q      Base data type for calculations
             * \tparam T      Model template, e.g. efgy::geometry::cube
             * \tparam d      Model depth, e.g. 4 for a tesseract
             * \tparam e      Model render depth, e.g. >= 4 when rendering a
             *                tesseract
             * \tparam format Vector coordinate format to work in.
             */
            template <typename Q,
                      template <class,unsigned int,typename> class T,
                      unsigned int d, unsigned int e,
                      typename format>
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
                    static output apply (argument out, const format &)
                    {
                        return out << d << "-" << T<Q,d,format>::id() << "@" << e << "\n";
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
             * \tparam Q      Base data type for calculations
             * \tparam T      Model template, e.g. efgy::geometry::cube
             * \tparam d      Model depth, e.g. 4 for a tesseract
             * \tparam e      Model render depth, e.g. >= 4 when rendering a
             *                tesseract
             * \tparam format Vector coordinate format to work in.
             */
            template <typename Q,
                      template <class,unsigned int,typename> class T,
                      unsigned int d, unsigned int e,
                      typename format>
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
                    static output apply (argument out, const format &)
                    {
                        out.insert(T<Q,d,format>::id());
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
            template <typename Q,
                      template <class,unsigned int,typename> class T,
                      unsigned int d, unsigned int e,
                      typename format>
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
                    static output apply (argument out, const format &)
                    {
                        std::ostringstream os;
                        os        << T<Q,d,format>::depth
                           << "-" << T<Q,d,format>::id();
                        out.insert(os.str());
                        return out;
                    }

                    /**\copydoc models::pass */
                    static output pass (argument out)
                    {
                        return out;
                    }
            };

            /**\brief Geometry factory functor: add coordinate format to set
             *
             * Another sample functor; this one adds all the coordinate formats
             * that are encountered to a set of strings. This is very similar to
             * the 'models' functor, but using the format IDs instead of the
             * model IDs.
             *
             * \tparam Q      Base data type for calculations
             * \tparam T      Model template, e.g. efgy::geometry::cube
             * \tparam d      Model depth, e.g. 4 for a tesseract
             * \tparam e      Model render depth, e.g. >= 4 when rendering a
             *                tesseract
             * \tparam format Vector coordinate format to work in.
             */
            template <typename Q,
                      template <class,unsigned int,typename> class T,
                      unsigned int d, unsigned int e,
                      typename format>
            class formats
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

                    /**\brief Add coordinate format to set
                     *
                     * Adds the instance's coordinate format to the set. This
                     * only adds the proper name, without any attributes.
                     *
                     * \param[out] out The set to modify.
                     * \param[out] tag The coordinate format to add.
                     *
                     * \returns out, after adding the new coordinate format.
                     */
                    static output apply (argument out, const format &tag)
                    {
                        out.insert(tag.id());
                        return out;
                    }
                    
                    /**\copydoc echo::pass */
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
         * \tparam Q      Base data type for calculations
         * \tparam func   The function to call.
         * \tparam T      Model template, e.g. efgy::geometry::cube
         * \tparam d      Model depth, e.g. 4 for a tesseract
         * \tparam e      Model render depth, e.g. >= 4 when rendering a
         *                tesseract
         * \tparam format Vector coordinate format to work in.
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,typename> class, unsigned int, unsigned int, typename> class func,
                 template <class,unsigned int,typename> class T,
                 unsigned int d, unsigned int e,
                 typename format>
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
                 * \param[in]  tag   The vector format tag instance to use.
                 *
                 * \returns The return value of either func::pass or
                 *          func::apply, depending on whether the call
                 *          'succeeded'.
                 */
                constexpr static typename func<Q,T,d,e,format>::output with
                    (typename func<Q,T,d,e,format>::argument arg,
                     const unsigned int &dims,
                     const unsigned int &rdims,
                     const format &tag)
                {
                    return d < T<Q,d,format>::dimensions::modelDimensionMinimum ? func<Q,T,d,e,format>::pass(arg)
                         : (T<Q,d,format>::dimensions::modelDimensionMaximum > 0) && (d > T<Q,d,format>::dimensions::modelDimensionMaximum) ? model<Q,func,T,d-1,e,format>::with (arg, dims, rdims, tag)
//                         : e < T<Q,d,format>::renderDepth ? func<Q,T,d,e,format>::pass(arg)
                         : 0 == rdims
                            ? (   0 == dims ? func<Q,T,d,e,format>::apply(arg,tag), model<Q,func,T,d,e-1,format>::with (arg, dims, rdims, tag), model<Q,func,T,d-1,e,format>::with (arg, dims, rdims, tag)
                                : d == dims ? func<Q,T,d,e,format>::apply(arg, tag), model<Q,func,T,d,e-1,format>::with (arg, dims, rdims, tag)
                                : d  < dims ? func<Q,T,d,e,format>::pass(arg)
                                : model<Q,func,T,d-1,e,format>::with (arg, dims, rdims, tag) )
                         : e == rdims
                            ? (   0 == dims ? func<Q,T,d,e,format>::apply(arg,tag), model<Q,func,T,d-1,e,format>::with (arg, dims, rdims, tag)
                                : d == dims ? func<Q,T,d,e,format>::apply(arg,tag), model<Q,func,T,d,e-1,format>::with (arg, dims, rdims, tag)
                                : d  < dims ? func<Q,T,d,e,format>::pass(arg)
                                : model<Q,func,T,d-1,e,format>::with (arg, dims, rdims, tag) )
                         : e < rdims ? func<Q,T,d,e,format>::pass(arg)
                         : model<Q,func,T,d,e-1,format>::with (arg, dims, rdims, tag);
                }
        };

        /**\brief Model factory helper; e=2 fix point
         *
         * The model factory helper, geometry::model, works by calling itself
         * recursively with different template parameters. This is one of these
         * parameters' fix points, which prevents an infinite template
         * recursion.
         *
         * \tparam Q      Base data type for calculations
         * \tparam func   The function to call.
         * \tparam T      Model template, e.g. efgy::geometry::cube
         * \tparam d      Model depth, e.g. 4 for a tesseract
         * \tparam format Vector coordinate format to work in.
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,typename> class, unsigned int, unsigned int, typename> class func,
                 template <class,unsigned int,typename> class T,
                 unsigned int d,
                 typename format>
        class model<Q,func,T,d,2,format>
        {
            public:
                /**\brief Call func with parameters; e=2 fix point
                 *
                 * This is a fix point stub of the model::with() method; all
                 * arguments but the first are ignored and this fix point will
                 * simply return the result of applying func's pass method to
                 * the first argument.
                 *
                 * \param[in] arg The argument that is passed to the functor.
                 *
                 * \returns func::pass(arg).
                 */
                constexpr static typename func<Q,T,d,2,format>::output with
                    (typename func<Q,T,d,2,format>::argument arg,
                     const unsigned int &,
                     const unsigned int &,
                     const format &)
                {
                    return func<Q,T,d,2,format>::pass(arg);
                }
        };

        /**\brief Model factory helper; d=1 fix point
         *
         * The model factory helper, geometry::model, works by calling itself
         * recursively with different template parameters. This is one of these
         * parameters' fix points, which prevents an infinite template
         * recursion.
         *
         * \tparam Q      Base data type for calculations
         * \tparam func   The function to call.
         * \tparam T      Model template, e.g. efgy::geometry::cube
         * \tparam e      Model render depth, e.g. >= 4 when rendering a
         *                tesseract
         * \tparam format Vector coordinate format to work in.
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,typename> class, unsigned int, unsigned int, typename> class func,
                 template <class,unsigned int,typename> class T,
                 unsigned int e,
                 typename format>
        class model<Q,func,T,1,e,format>
        {
            public:
                /**\brief Call func with parameters; d=1 fix point
                 *
                 * This is a fix point stub of the model::with() method; all
                 * arguments but the first are ignored and this fix point will
                 * simply return the result of applying func's pass method to
                 * the first argument.
                 *
                 * \param[in] arg The argument that is passed to the functor.
                 *
                 * \returns func::pass(arg).
                 */
                constexpr static typename func<Q,T,1,e,format>::output with
                    (typename func<Q,T,1,e,format>::argument arg,
                     const unsigned int &,
                     const unsigned int &,
                     const format &)
                {
                    return func<Q,T,1,e,format>::pass(arg);
                }
        };

        /**\brief Call template function with class type
         *
         * Part of the geometric model type factory; called by the
         * geometry::with function given a type to check if the type is
         * compatible with the given parameters.
         *
         * \tparam Q      Base datatype for model geometry.
         * \tparam func   The function to call.
         * \tparam d      Maximum number of model dimensions.
         * \tparam e      Maximum number of render dimensions.
         * \tparam T      Model template, e.g. efgy::geometry::cube
         * \tparam format Vector coordinate format to work in.
         *
         * \param[out] arg   The argument to func::...().
         * \param[in]  type  The name to check, or "*" which always matches.
         * \param[in]  dims  The target number of model dimensions.
         * \param[in]  rdims The target number of render dimensions.
         * \param[in]  tag   The vector format tag instance to use.
         *
         * \returns Whatever func::pass or func::with returns.
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,typename> class, unsigned int, unsigned int, typename> class func,
                 unsigned int d,
                 unsigned int e,
                 template <class,unsigned int,typename> class T,
                 typename format>
        static inline typename func<Q,T,d,e,format>::output with
            (typename func<Q,T,d,e,format>::argument arg,
             const std::string &type,
             const unsigned int &dims,
             const unsigned int &rdims,
             const format &tag)
        {
            return ((type == "*") || (type == T<Q,d,format>::id()))
                 ? model<Q,func,T,d,e,format>::with(arg, dims, rdims, tag)
                 : func<Q,T,d,e,format>::pass(arg);
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
         * \tparam Q      Base datatype for model geometry.
         * \tparam func   The function to call.
         * \tparam d      Maximum number of model dimensions.
         * \tparam e      Maximum number of render dimensions.
         * \tparam T      Parametric formula template, e.g. formula::plane
         * \tparam format Vector coordinate format to work in.
         */
        template <typename Q,
                  template<typename, template <class,unsigned int,typename> class, unsigned int, unsigned int, typename> class func,
                  unsigned int d,
                  unsigned int e,
                  template <class,unsigned int,typename> class T,
                  typename format>
        class parametricFactory
        {
            public:
                template <typename Qm, unsigned int odm, typename formatm>
                using parametricModel = parametric<Qm,odm,T,formatm>;

                typedef typename func<Q,parametricModel,d,e,format>::output output;
                typedef typename func<Q,parametricModel,d,e,format>::argument argument;

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
                 * \param[in]  tag   The vector format tag instance to use.
                 *
                 * \returns Whatever func::pass returns.
                 */
                static inline output with
                    (argument arg,
                     const std::string &type,
                     const unsigned int &dims,
                     const unsigned int &rdims,
                     const format &tag)
                {
                    return geometry::with<Q,func,d,e,parametricModel,format>(arg,type,dims,rdims,tag);
                }
        };

        /**\brief Call template function with parametric formula
         *
         * This is a wrapper for parametricFactory<T>::with for sheer
         * convenience.
         *
         * \tparam Q      Base datatype for model geometry.
         * \tparam func   The function to call.
         * \tparam d      Maximum number of model dimensions.
         * \tparam e      Maximum number of render dimensions.
         * \tparam T      Parametric formula template, e.g. formula::plane
         * \tparam format Vector coordinate format to work in.
         *
         * \param[out] arg   The argument to func::...().
         * \param[in]  type  The name to check, or "*" which always matches.
         * \param[in]  dims  The target number of model dimensions.
         * \param[in]  rdims The target number of render dimensions.
         * \param[in]  tag   The vector format tag instance to use.
         *
         * \returns Whatever func::pass or func::with returns.
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,typename> class, unsigned int, unsigned int, typename> class func,
                 unsigned int d,
                 unsigned int e,
                 template <class,unsigned int,typename> class T,
                 typename format>
        static inline typename parametricFactory<Q,func,d,e,T,format>::output withF
            (typename parametricFactory<Q,func,d,e,T,format>::argument arg,
             const std::string &type,
             const unsigned int &dims,
             const unsigned int &rdims,
             const format &tag)
        {
            return parametricFactory<Q,func,d,e,T,format>::with(arg,type,dims,rdims,tag);
        }

        /**\brief Call template function with geometric type(s)
         *
         * This is the main entry function for the geometric type factory. This
         * function allows you to specify the type of geometric primitive that
         * you're interested in at run time, as well as the dimensional
         * parameters for those primitives.
         *
         * \tparam Q      Base datatype for model geometry.
         * \tparam func   The function to call.
         * \tparam d      Maximum number of model dimensions.
         * \tparam e      Maximum number of render dimensions.
         * \tparam format Vector coordinate format to work in.
         *
         * \param[out] arg   The argument to func::...().
         * \param[in]  type  The model to pass to func, or "*" for "all models".
         * \param[in]  dims  The target number of model dimensions.
         * \param[in]  rdims The target number of render dimensions.
         * \param[in]  tag   The vector format tag instance to use.
         *
         * \returns Whatever func::pass returns.
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,typename> class, unsigned int, unsigned int, typename> class func,
                 unsigned int d,
                 unsigned int e = d,
                 typename format = math::format::cartesian>
        static inline typename func<Q,cube,d,e,format>::output with
            (typename func<Q,cube,d,e,format>::argument arg,
             const std::string &type,
             const unsigned int &dims,
             const unsigned int &rdims,
             const format &tag = format())
        {
            with<Q,func,d,e,simplex,format>(arg,type,dims,rdims,tag);
            with<Q,func,d,e,plane,format>(arg,type,dims,rdims,tag);
            with<Q,func,d,e,cube,format>(arg,type,dims,rdims,tag);
            withF<Q,func,d,e,formula::sphere,format>(arg,type,dims,rdims,tag);
            withF<Q,func,d,e,formula::torus,format>(arg,type,dims,rdims,tag);
            withF<Q,func,d,e,formula::moebiusStrip,format>(arg,type,dims,rdims,tag);
            withF<Q,func,d,e,formula::kleinBagel,format>(arg,type,dims,rdims,tag);
            withF<Q,func,d,e,formula::kleinBottle,format>(arg,type,dims,rdims,tag);
            with<Q,func,d,e,sierpinski::gasket,format>(arg,type,dims,rdims,tag);
            with<Q,func,d,e,sierpinski::carpet,format>(arg,type,dims,rdims,tag);
            with<Q,func,d,e,randomAffineIFS,format>(arg,type,dims,rdims,tag);
            with<Q,func,d,e,flame::random,format>(arg,type,dims,rdims,tag);

            return func<Q,cube,d,e,format>::pass(arg);
        }

        /**\brief Call template function with geometric type(s)
         *
         * An alternative to the main entry to the model factory functionality,
         * this variant also allows specifying the vector format to use at run
         * time.
         *
         * \tparam Q      Base datatype for model geometry.
         * \tparam func   The function to call.
         * \tparam d      Maximum number of model dimensions.
         * \tparam e      Maximum number of render dimensions.
         *
         * \param[out] arg    The argument to func::...().
         * \param[in]  format Vector format to pass to func, e.g. "cartesian"
         *                    for the default or "*" for "all formats".
         * \param[in]  type   The model to pass to func, or "*" for "all
         *                    models".
         * \param[in]  dims   The target number of model dimensions.
         * \param[in]  rdims  The target number of render dimensions.
         *
         * \returns Whatever func::pass returns.
         */
        template<typename Q,
                 template<typename, template <class,unsigned int,typename> class, unsigned int, unsigned int, typename> class func,
                 unsigned int d,
                 unsigned int e = d>
        constexpr static inline typename func<Q,cube,d,e,math::format::cartesian>::output with
            (typename func<Q,cube,d,e,math::format::cartesian>::argument arg,
             const std::string &format,
             const std::string &type,
             const unsigned int &dims,
             const unsigned int &rdims)
        {
            return
                (format == "*" || format == "cartesian")
                    ? with<Q,func,d,e,math::format::cartesian>(arg,type,dims,rdims,math::format::cartesian())
                    : func<Q,cube,d,e,math::format::cartesian>::pass(arg),
                (format == "*" || format == "polar")
                    ? with<Q,func,d,e,math::format::polar>(arg,type,dims,rdims,math::format::polar())
                    : func<Q,cube,d,e,math::format::polar>::pass(arg);
        }
    };
};

#endif
