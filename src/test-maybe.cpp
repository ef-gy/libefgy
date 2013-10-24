/*
 * This file is part of the libefgy project.
*/

/*
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
*/

#include <iostream>

#include <ef.gy/maybe.h>

using namespace efgy;
using namespace std;

int main (int argc, char **argv)
{
    maybe<int> nothing;

    if (nothing)
    {
        cerr << "value should have been 'nothing'\n";
        return 1;
    }

    maybe<int> something(42);

    if (!something)
    {
        cerr << "value should have been something other than 'nothing'\n";
        return 2;
    }

    if (int(something) != 42)
    {
        cerr << "value should have been '42', but is '" << int(something) << "'\n";
        return 3;
    }

    something = nothing;

    if (something)
    {
        cerr << "'something' was changed to 'nothing', but still thinks otherwise.\n";
        return 4;
    }

    maybe<double> somethingElse (something);

    if (somethingElse)
    {
        cerr << "value should have been 'nothing'\n";
        return 5;
    }

    somethingElse = maybe<int>(42);

    if (!somethingElse)
    {
        cerr << "value should have been something other than 'nothing'\n";
        return 6;
    }

    if (double(somethingElse) != 42)
    {
        cerr << "value should have been '42', but is '" << double(somethingElse) << "'\n";
        return 7;
    }

    return 0;
}
