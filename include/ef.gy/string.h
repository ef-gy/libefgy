/**\file
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

#if !defined(EF_GY_STRING_H)
#define EF_GY_STRING_H

#include <string>
#include <ef.gy/numeric.h>
#include <string.h>
#include <math.h>

using efgy::math::numeric::zero;
using std::cout;

namespace efgy
{
    /**
     * Template specialization for a print without arguments.
     *
     * \see print
     */
    template<typename stream_type, typename... values>
    void print(stream_type &stream, const std::string &unformatted_string) {
        stream << unformatted_string;
    }

    /**
     * Template to print with % as placeholders to a stream.
     *
     * It just outputs a single placeholder variable, but does a recursive
     * (template) function call for each character in the string.
     *
     * A usage example:
     * \code
     * print("Time: % Name: % Counter: %", time, name, counter);
     * \endcode
     */
    template<typename stream_type, typename T, typename... values>
    void print(stream_type &stream, const std::string &unformatted_string, T first, values... rest) {
        size_t pos_placeholder = unformatted_string.find_first_of("%");
        if (pos_placeholder == std::string::npos) {
            print(stream, unformatted_string);
            return;
        }
        else {
            stream << unformatted_string.substr(0, pos_placeholder);
            stream << first;
            print(stream, unformatted_string.substr(pos_placeholder + 1), rest...);
            return;
        }
    }


};

#endif
