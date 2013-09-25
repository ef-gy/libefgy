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

#if !defined(EF_GY_MAYBE_H)
#define EF_GY_MAYBE_H

namespace efgy
{
    template <typename T>
    class maybe
    {
        public:
            maybe ()
                : nothing(true)
                {}

            maybe (const T &pJust)
                : nothing(false), just(pJust)
                {}

            maybe (const maybe &b)
                : nothing(b.nothing), just(b.just)
                {}

            template <typename U>
            maybe (const maybe<U> &b)
                : nothing(b.nothing), just(T(b.just))
                {}

            template <typename U>
            maybe &operator = (const maybe<U> &b)
                {
                    nothing = b.nothing;
                    just = T(b.just);
                    return *this;
                }

            operator bool (void) const
            {
                return !nothing;
            }

            operator T (void) const
            {
                return just;
            }

/*
            template<typename U>
            operator U (void) const
            {
                return U(just);
            }
*/

            T just;
            bool nothing;
    };
};

#endif
