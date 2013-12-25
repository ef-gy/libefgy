/**\file
 * \brief Stub renderer
 *
 * Implements the null render template, which discards any drawing commands.
 * This renderer is used in some template magic.
 *
 * \copyright
 * Copyright (c) 2012-2013, ef.gy Project Members
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

#if !defined(EF_GY_RENDER_NULL_H)
#define EF_GY_RENDER_NULL_H

#include <ef.gy/euclidian.h>
#include <ef.gy/projection.h>

namespace efgy
{
    namespace render
    {
        /**\brief Null renderer
         *
         * Stub renderer that discards any input.
         *
         * \tparam Q The base numeric type you intend to use.
         * \tparam d The number of dimensions for vectors.
         */
        template<typename Q, unsigned int d>
        class null
        {
            public:
                /**\brief Construct with matrices and lower renderer
                 *
                 * All of the parameters to this constructor are ignored. The
                 * constructor is only provided for compatibility with other
                 * renderers.
                 */
                null
                    (const geometry::transformation::affine<Q,d> &,
                     const geometry::projection<Q,d> &,
                     const null<Q,d-1> &)
                    {}

                /**\brief Begin drawing a new frame
                 *
                 * Reset the renderer's state and start drawing a new image.
                 */
                void frameStart (void) const {};

                /**\brief End drawing current frame
                 *
                 * Finalise the renderer's state.
                 */
                void frameEnd (void) const {};

                /**\brief Draw polygon
                 *
                 * Draw a polygon with q vertices. The Polygon should be
                 * convex; if it isn't then you'll get rather strange results.
                 *
                 * \tparam q The number of vertices that define the polygon.
                 */
                template<std::size_t q>
                void drawFace
                    (const std::array<typename geometry::euclidian::space<Q,d>::vector,q> &, const Q & = 0.5) const {}
        };

        /**\brief Null renderer (2D fix point)
         *
         * Stub renderer that discards any input.
         *
         * \tparam Q The base numeric type you intend to use.
         */
        template<typename Q>
        class null<Q,2>
        {
            public:
                /**\brief Construct with affine transformation
                 *
                 * Provided for compatibility with other renderers; the actual
                 * parameter is ignored.
                 */
                null
                    (const typename geometry::transformation::affine<Q,2> &)
                    {}

                /**\brief Begin drawing a new frame
                 *
                 * Reset the renderer's state and start drawing a new image.
                 */
                void frameStart (void) const {};

                /**\brief End drawing current frame
                 *
                 * Finalise the renderer's state.
                 */
                void frameEnd (void) const {};

                /**\brief Draw polygon
                 *
                 * Draw a polygon with q vertices. The Polygon should be
                 * convex; if it isn't then you'll get rather strange results.
                 *
                 * \tparam q The number of vertices that define the polygon.
                 */
                template<std::size_t q>
                void drawFace
                    (const std::array<typename geometry::euclidian::space<Q,2>::vector,q> &, const Q & = 0.5) const {}
        };
    };
};

#endif
