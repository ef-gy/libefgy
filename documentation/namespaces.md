#<cldoc:test>

Library test cases.

Library test cases all go in the src/test-case/ directory. The makefile will
automatically build them, and you can run them in a batch by using the make
target 'test', like so:

    $ make test

If any of the test cases fail to run then the make command will fail with an
error. You could use this to run the test cases automatically after a checkout,
for example.

#<cldoc:efgy>

Base namespace

This is the base namespace for all the headers contained within libefgy. This
namespace is used to prevent namespace clashes with your own code - if you don't
want to use fully qualified names in your code, use the 'using' directive after
including the headers, like so:

    using namespace efgy;

#<cldoc:efgy::cli>

Command line argument handling.

libefgy provides facilities for command line parsing using regular expressions.

#<cldoc:efgy::json>

JSON values and ways to work with them.

As it turns out, a lot of things these days rely on processing JSON data, so
this namespace provides an implementation of JSON values, a parser for them and
a serialiser to generate your own data in JSON.

See http://www.json.org/ for more details on the format.

#<cldoc:efgy::test>

Functions and types related to test cases.

This namespace contains functions and types that make defining automatic unit
tests easier.

