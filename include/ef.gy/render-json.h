/**\file
 * \brief JSON renderer
 *
 * The templates in this header file allow tagging output streams as JSON output
 * streams so they can be written to like any other C++ stream but producing
 * JSON instead of plain text.
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

#if !defined(EF_GY_RENDER_JSON_H)
#define EF_GY_RENDER_JSON_H

#include <ef.gy/render.h>
#include <string>
#include <ostream>

namespace efgy
{
    namespace render
    {
        /**\brief std::ostream JSON tag
         *
         * Used to distinguish between a plain std::ostream, and one where the
         * output should be in JSON format.
         *
         * \tparam C Character type for the basic_ostream reference.
         */
        template <typename C>
        class ojsonstream
        {
            public:
                /**\brief Construct with stream reference
                 *
                 * Initialises a new ostream JSON tag instance.
                 *
                 * \param[out] pStream The stream to write to.
                 */
                ojsonstream (std::basic_ostream<C> &pStream)
                    : stream(pStream)
                    {}

                /**\brief Output stream reference
                 *
                 * This is the stream where the output is written to.
                 */
                std::basic_ostream<C> &stream;
        };

        /**\brief JSON tag
         *
         * Write this to an ostream to turn it into an ojsonstream. Like this:
         *
         * \code{.cpp}
         * cout << JSON();
         * \encode
         */
        class JSON {};

        /**\brief Convert std::ostream to JSON
         *
         * Converts the given stream to a JSON stream so that write operations
         * after that will produce JSON instead of plain text.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The stream to write to.
         */
        template <typename C>
        constexpr inline ojsonstream<C> operator << (std::basic_ostream<C> &stream, const JSON &)
        {
            return ojsonstream<C>(stream);
        }
    };
};

#endif
