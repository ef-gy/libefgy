/**\file
 * \brief Test cases for the JSON render output
 *
 * The test cases in this file test the functionality of JSON renderer.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#include <iostream>

#include <ef.gy/test-case.h>
#include <ef.gy/render-json.h>
#include <sstream>

using namespace efgy::render;
using namespace efgy;

/**\brief JSON output tests
 * \test Writes some JSON to a stringstream to see if it compiles and works as
 *       intended.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testJSONOutput(std::ostream &log) {
  std::map<const char *, int> m;

  m["a"] = 42;
  m["b"] = 23;
  m["c"] = 1;

  std::map<const char *, std::map<const char *, int>> mx;

  mx["d"] = m;

  std::ostringstream s("");

  s << json::tag() << false << 1 << "foo" << 2 << "bar \"baz\"" << 42.23
    << std::array<double, 2>({23, 42}) << m << mx << true;

  if (s.str() != "false1\"foo\"2\"bar \\\"baz\\\"\"42.23[23,42]"
                 "{\"a\":42,\"b\":23,\"c\":1}"
                 "{\"d\":{\"a\":42,\"b\":23,\"c\":1}}true") {
    log << "unexpected JSON output: " << s.str();
    return 1;
  }

  json::value<> v;

  s.str("");

  s << json::tag() << v;

  log << s.str();

  return 0;
}

/**\brief JSON input tests
 * \test Tries to parse some JSON with the JSON string parser.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testJSONInput(std::ostream &log) {
  json::value<> v;
  std::string pr = "{ true }";

  //    pr >> v;

  v.type = json::value<>::yes;

  log << json::tag() << v;

  std::string tmp = ("true" >> v);

  log << json::tag() << tmp << v;

  tmp = ("42.23" >> v);

  log << json::tag() << tmp << v;

  tmp = ("[true,false,null]" >> v);

  log << json::tag() << tmp << v;

  tmp = ("{\"a\":true,\"b\":false}" >> v);

  log << json::tag() << tmp << v;

  tmp = ("{\"a\":true,\"b\":false,\"q\":[false,true,false],\"c\":null}" >> v);

  log << json::tag() << tmp << v;

  tmp = ("{\"a\":true,\"b\":false,\"q\":[1,2,3,4],\"c\":null}" >> v);

  log << json::tag() << tmp << v;

  return 0;
}

TEST_BATCH(testJSONOutput, testJSONInput)
