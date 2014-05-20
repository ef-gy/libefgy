/**\file
 * \brief CSS stream tag
 *
 * Contains the CSS stream tag that is used to differentiate between output in
 * plain text and output in CSS.
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
 * \see Project Source Code: https://github.com/ef-gy/libefgy.git
 */

#if !defined(EF_GY_STREAM_CSS_H)
#define EF_GY_STREAM_CSS_H

#include <ef.gy/css.h>
#include <string>
#include <ostream>

namespace efgy
{
    namespace css
    {
        /**\brief std::ostream CSS tag
         *
         * Used to distinguish between a plain std::ostream, and one where the
         * output should be in CSS format.
         *
         * \tparam C Character type for the basic_ostream reference.
         */
        template <typename C>
        class ostream
        {
            public:
                /**\brief Construct with stream reference
                 *
                 * Initialises a new ostream CSS tag instance.
                 *
                 * \param[out] pStream The stream to write to.
                 */
                ostream (std::basic_ostream<C> &pStream)
                    : stream(pStream)
                    {}

                /**\brief Output stream reference
                 *
                 * This is the stream where the output is written to.
                 */
                std::basic_ostream<C> &stream;
        };

        /**\brief CSS tag
         *
         * Write this to an ostream to turn it into an ocssstream. Like this:
         *
         * \code{.cpp}
         * cout << css::tag();
         * \encode
         */
        class tag {};

        /**\brief Convert std::ostream to CSS
         *
         * Converts the given stream to a CSS stream so that write operations
         * after that will produce CSS instead of plain text.
         *
         * \tparam C Character type for the basic_ostream reference.
         *
         * \param[out] stream The stream to write to.
         */
        template <typename C>
        constexpr inline ostream<C> operator << (std::basic_ostream<C> &stream, const tag &)
        {
            return ostream<C>(stream);
        }
    };
};

#endif
