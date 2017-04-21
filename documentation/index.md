#<cldoc:index>
# libefgy-8

## Introduction

libefgy is a header-only C++ template library with lots of templates dealing
with maths in all its glory. And a few random other things not directly
related to maths that still come in handy. Unlike other libraries, all of
the classes take the datatype to work on as a template argument and
oftentimes the size of things like vectors and matrices are set at compile
time.

Using sizes that are fixed at compile time may seem inconvenient, but in
theory it should allow the compiler to produce code that is better
optimised. Also, since everything is done in templates, the compiler might
even be able to remove a lot of code it would otherwise have to leave in.

Most of the headers require C++11 compiler features. Also, it is recommended to
use 'clang++' with these headers, if possible, as tha is the compiler the
headers were designed for. 'clang++' also happens to provide much more useful
error messages when things don't work out like they should with templates.

## Obtaining libefgy

The primary location for this library is the git repository at:
https://github.com/ef-gy/libefgy; use this link to browse the source code
online, or to download tarballs of development versions or releases. To check
out a working copy with git, run the following in a shell:

    $ git clone https://github.com/ef-gy/libefgy.git

The primary location for the library's documentation is:
https://ef.gy/documentation/libefgy; please note that the some pages in all
HTML versions of the documentation may include YouTube videos - if you're uneasy
about this for privacy reasons, then feel free to browse the headers directly in
the git repository.

Also feel free to browse the blog at https://ef.gy/ if you happen to be in the
area :).

## Installation

Due to its header-only nature, installing libefgy is both easy and, technically,
optional. It's perfectly fine to just copy the headers to your programme's
include path, or to check out the upstream version with git and to create
symlinks to that.

To install the headers system-wide, libefgy has a makefile that will copy all of
the headers to the default location on unix-like systems. If you'd like to do
so, issue a command like the following on the command line: 

    $ sudo make PREFIX=/usr install

Note that this will overwrite any currently installed version of libefgy. The
headers are all contained in their own directory, so no other libraries should
be affected, unless they too would be using the ef.gy/ directory in your
system-wide include directory. Nobody should be doing this, obviously.

## Test Cases

libefgy comes with a handful of test cases that you may wish to run to verify
that things are working as expected.

To run the test cases, use the 'test' makefile target, like this:

    $ make test

This will bail on any errors it encounters - while compiling the test cases or
while running them. If this ever happens then something's definitely wrong and
it'd be great if you could send the developers an email describing the problem.

## Licence

libefgy is distributed under the terms of the standard MIT/X licence, which
reads:

#<cldoc:include(../COPYING)>

### Further details

See the file AUTHORS and the commit history on the official git repository for
more details on the 'ef.gy Project Members'.

If you'd like to use libefgy in your project, but for some reason are unable to
comply with the terms of this MIT/X licence then feel free to contact
magnus@ef.gy - I'm sure we can work something out.
