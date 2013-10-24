#if !defined(EF_GY_VERSION_H)
#define EF_GY_VERSION_H

/**\file
 * \brief Version information
 *
 * This header contains the library's version number and some additional
 * documentation that didn't seem to fit in anywhere else.
 */

/**\if LIBEFGYMAIN
 * \mainpage libefgy-3
 * \else
 * \page libefgy libefgy-3
 * \endif
 *
 * \section libefgy-introduction Introduction
 * libefgy is a header-only C++ template library with lots of templates dealing
 * with maths in all its glory. And a few random other things not directly
 * related to maths that still come in handy. Unlike other libraries, all of
 * the classes take the datatype to work on as a template argument and
 * oftentimes the size of things like vectors and matrices are set at compile
 * time.
 *
 * Using sizes that are fixed at compile time may seem inconvenient, but in
 * theory it should allow the compiler to produce code that is better
 * optimised. Also, since everything is done in templates, the compiler might
 * even be able to remove a lot of code it would otherwise have to leave in.
 *
 * \note Some of the headers may require C++0x compiler features. Also, it is
 *       recommended to use 'clang++' with these headers, if possible, as that
 *       is the compiler the headers were designed for. 'clang++' also happens
 *       to provide much more useful error messages when things don't work out
 *       like they should with templates.
 *
 * \section libefgy-installation Installation
 * Due to its header-only nature, installing libefgy is both easy and,
 * technically, optional. It's perfectly fine to just copy the headers to your
 * programme's include path, or to check out the upstream version with git and
 * to create symlinks to that.
 *
 * To install the headers system-wide, libefgy has a makefile that will copy
 * all of the headers to the default location on *nix. If you'd like to do so,
 * issue a command like the following on the command line:
 *
 * \code
 * $ sudo make PREFIX=/usr install
 * \endcode
 *
 * Note that this will overwrite any currently installed version of libefgy.
 * The headers are all contained in their own directory, so no other libraries
 * should be affected, unless they too would be using the ef.gy/ directory in
 * your system-wide include directory. Nobody should be doing this, obviously.
 *
 * \section libefgy-tests Test Cases
 * libefgy comes with a handful of test cases that you may wish to run to
 * verify that things are working as expected.
 *
 * To run the test cases, use the 'test' makefile target, like this:
 *
 * \code
 * $ make test
 * \endcode
 *
 * This will bail on any errors it encounters - while compiling the test cases
 * or while running them. If this ever happens then something's definitely
 * wrong and it'd be great if you could send the developers an email describing
 * the problem.
 *
 * \section libefgy-licence Licence
 * libefgy is distributed under the terms of the standard MIT/X licence, which
 * reads:
 *
 * Copyright (c) 2012-2013, ef.gy Project Members
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
 *
 * \note See the file AUTHORS and the commit history on the official git
 *       repository for more details on the 'ef.gy Project Members'.
 *
 * \note If you'd like to use libefgy in your project, but for some reason are
 *       unable to comply with the terms of this MIT/X licence then feel free
 *       to contact magnus@ef.gy - I'm sure we can work something out.
 */

/**\brief Base namespace
 *
 * This is the base namespace for all the headers contained within libefgy.
 * This namespace is used to prevent namespace clashes with your own code - if
 * you don't want to use fully qualified names in your code, use the 'using'
 * directive after including the headers, like so:
 *
 * \code
 * using namespace efgy;
 * \endcode
 */
namespace efgy
{
    /**\brief Library version
     *
     * This is the version of the header files you're including. You could test
     * this if you expect trouble with certain versions, or you know that your
     * code requires a very specific version of these headers.
     *
     * \note libefgy uses a very simple integer versioning scheme. There are no
     *       'major' and 'minor' versions and releases - every release gets the
     *       next available number, and that's it. This seems uncommon these
     *       days, but when you think about it, it makes a lot of sense:
     *       there's no such things as 'minor' changes, either you changed
     *       something or you haven't. Any change, no matter how minute, will
     *       ruin someone's day, so why not just stick with a plain ol' number
     *       that you can test for much more easily than a convoluted version
     *       string? ;)
     */
    static const unsigned int version = 3;
};

#endif
