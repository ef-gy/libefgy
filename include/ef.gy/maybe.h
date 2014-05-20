/**\file
 * \brief Haskell-like "maybe" template
 *
 * This header implements a 'maybe' template, much like the one found in the
 * programming language Haskell. The basic idea is that you have a class that
 * either contains a value, or a special sentinel value that indicates a value
 * of 'nothing'.
 *
 * This concept comes up surprisingly often - mostly for errors and the like,
 * where you typically do get a proper value, but sometimes you just don't for
 * one reason or another.
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

#if !defined(EF_GY_MAYBE_H)
#define EF_GY_MAYBE_H

#include <iostream>

namespace efgy
{
    /**\brief Haskell-like "maybe" template
     *
     * A 'maybe' is a special container that contains either a value or
     * 'nothing'. This is useful when writing functions that may or may not
     * actually return a value; for example when inverting a matrix, you
     * might either get a proper matrix as the result of that function, or you
     * might get nothing - because maybe you're trying to invert a matrix that
     * doesn't have an inverse, or some error happened.
     *
     * This is kind of like extending a type with a NULL like in SQL. Oddly
     * enough the use of the word 'maybe' to describe this kind of data
     * structure seems to have become popular most recently with the Haskell
     * community.
     *
     * \note It'd probably cause a lot of confusion if you tried to use a
     *       maybe<bool>, so try to avoid those.
     *
     * \tparam T The type of a proper value.
     */
    template <typename T>
    class maybe
    {
        public:
            /**\brief Default constructor
             *
             * Construct a 'maybe' that contains 'nothing'. This constructor
             * can only be used if your type supports a default constructor; if
             * it does not, then you probably won't be able to use the template
             * all too effectively.
             */
            constexpr maybe (void)
                : nothing(true), just()
                {}

            /**\brief Construct with value
             *
             * Construct a 'maybe' that contains the value passed as the first
             * parameter. A copy of this value is created in the process.
             *
             * \param[in] pJust The value to use.
             */
            constexpr maybe (const T &pJust)
                : nothing(false), just(pJust)
                {}

            /**\brief Copy constructor
             *
             * Copies the contents of another 'maybe'. The value is copied
             * unconditionally, even if the maybe actually contains 'nothing'.
             *
             * \param[in] b The 'maybe' to copy.
             */
            constexpr maybe (const maybe &b)
                : nothing(b.nothing), just(b.just)
                {}

            /**\brief Copy constructor template
             *
             * Copies the contents of another 'maybe'. The value is copied
             * unconditionally, even if the maybe actually contains 'nothing'.
             *
             * The source 'maybe' may contain a value of a different type than
             * the created 'maybe' - in this case, the source value is cast to
             * the type of the destination value. Obviously this requires T to
             * have a constructor that takes a value of type U, or that U can
             * be cast to T.
             *
             * \tparam    U The value type of the 'maybe' to copy.
             *
             * \param[in] b The 'maybe' to copy.
             */
            template <typename U>
            constexpr maybe (const maybe<U> &b)
                : nothing(b.nothing), just(T(b.just))
                {}

            /**\brief Assignment operator template
             *
             * Assigns the contents of the given 'maybe' to this instance.
             * Similarly to the copy constructors, the value is copied
             * unconditionally; even if the 'maybe' contains 'nothing'.
             *
             * In the process, the value of the given 'maybe' is cast to T,
             * meaning that U must be convertible to T in some way. This
             * should be a no-op if the two are of the same type.
             *
             * \tparam    U The value type of the 'maybe' to assign.
             *
             * \param[in] b The 'maybe' to copy.
             *
             * \return    A reference to the 'maybe' that was assigned to.
             */
            template <typename U>
            maybe &operator = (const maybe<U> &b)
                {
                    nothing = b.nothing;
                    just = T(b.just);
                    return *this;
                }

            /**\brief Find out if a 'maybe' contains a value
             *
             * A 'maybe' contains a value if it does not contain 'nothing',
             * so this cast just returns the inverse of the 'nothing' member
             * value.
             *
             * This type conversion operator may seem strange at first, but it
             * allows for very legible code like the following:
             *
             * \code
             * maybe<Q> m = calculateSomething();
             * if (m)
             * {
             *     const Q value = Q(m);
             *     doSomethingWith(value);
             * }
             * \endcode
             *
             * \return 'true' when this 'maybe' does contain a proper value,
             *         'false' if it contains 'nothing'.
             */
            constexpr operator bool (void) const
            {
                return !nothing;
            }

            /**\brief Obtain the value of a 'maybe'
             *
             * This cast operator simply returns the contained value of a
             * 'maybe'. Note that this value is returned unconditionally, that
             * is it's returned even if the 'maybe' contains 'nothing'.
             *
             * \return The value contained in this 'maybe'.
             */
            constexpr operator const T& (void) const
            {
                return just;
            }

            /**\brief Less-than comparison operator
             *
             * You should cast a 'maybe' to the appropriate type directly if
             * you intend to work with it instead of relying on any overloads;
             * this operator in particular was implemented to make maybes work
             * with sorted STL containers.
             */
            constexpr bool operator < (const maybe &b) const
            {
                return (!nothing
                            ? (!b.nothing
                                   ? (just < b.just)
                                   : true)
                            : false);
            }

            /**\brief The value contained in this 'maybe'
             *
             * This contains the actual value stored in this 'maybe'. If the
             * maybe actually contains 'nothing', then the value stored here
             * is pretty much unspecified - it might be a default value for
             * the type T, or it might be some old value the 'maybe' used to
             * have.
             */
            T just;

            /**\brief Does this 'maybe' contain 'nothing'?
             *
             * This boolean is 'true' whenever the 'maybe' contains 'nothing',
             * and 'false' if it contains a proper value.
             */
            bool nothing;

            /** \brief Stream output for maybes
             *
             * Overloads the stream insertion operator for maybes.
             * If the maybe contains 'nothing', the output to the stream will 
             * be the string literal "nothing"; 
             * if it contains a proper value, the value will be inserted
             * into the stream (so its type ought to have a stream insertion 
             * operator as well).
             */
            friend std::ostream& operator << (std::ostream& str, const maybe &b)
            {
                if(b.nothing) 
                {
                    str << "nothing";
                }
                else
                {
                    str << b.just;
                }
                return str;
            }
    };
};

#endif
