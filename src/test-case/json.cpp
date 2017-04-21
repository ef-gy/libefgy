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

#include <ef.gy/stream-json.h>
#include <ef.gy/test-case.h>
#include <sstream>

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
  struct sampleDataBool {
    bool in;
    std::string out;
  };

  struct sampleDataInt {
    int in;
    std::string out;
  };

  struct sampleDataDouble {
    double in;
    std::string out;
  };

  struct sampleDataString {
    const char *in;
    std::string out;
  };

  std::vector<sampleDataBool> testsBool{
      {false, "false"}, {true, "true"},
  };

  std::vector<sampleDataInt> testsInt{
      {1, "1"}, {2, "2"},
  };

  std::vector<sampleDataDouble> testsDouble{
      {1.23, "1.23"}, {42.23, "42.23"},
  };

  std::vector<sampleDataString> testsString{
      {"foo", "\"foo\""}, {"bar \"baz\"", "\"bar \\\"baz\\\"\""},
  };

  for (const auto &tt : testsBool) {
    std::ostringstream s("");
    s << json::tag() << tt.in;

    if (s.str() != tt.out) {
      log << "bad JSON output: got '" << s.str() << "', expected '" << tt.out
          << "'\n";
      return 1;
    }
  }

  for (const auto &tt : testsInt) {
    std::ostringstream s("");
    s << json::tag() << tt.in;

    if (s.str() != tt.out) {
      log << "bad JSON output: got '" << s.str() << "', expected '" << tt.out
          << "'\n";
      return 2;
    }
  }

  for (const auto &tt : testsDouble) {
    std::ostringstream s("");
    s << json::tag() << tt.in;

    if (s.str() != tt.out) {
      log << "bad JSON output: got '" << s.str() << "', expected '" << tt.out
          << "'\n";
      return 3;
    }
  }

  for (const auto &tt : testsString) {
    std::ostringstream s("");
    s << json::tag() << tt.in;

    if (s.str() != tt.out) {
      log << "bad JSON output: got '" << s.str() << "', expected '" << tt.out
          << "'\n";
      return 4;
    }
  }

  std::map<std::string, int> m{
      {"a", 42}, {"b", 23}, {"c", 1},
  };

  std::map<std::string, std::map<std::string, int>> mx{
      {"d", m},
  };

  std::ostringstream s("");
  s << json::tag() << std::array<double, 2>({23, 42});

  if (s.str() != "[23,42]") {
    log << "unexpected JSON output: " << s.str();
    return 5;
  }

  s.str("");
  s << json::tag() << m;

  if (s.str() != "{\"a\":42,\"b\":23,\"c\":1}") {
    log << "unexpected JSON output: " << s.str();
    return 6;
  }

  s.str("");
  s << json::tag() << mx;

  if (s.str() != "{\"d\":{\"a\":42,\"b\":23,\"c\":1}}") {
    log << "unexpected JSON output: " << s.str();
    return 7;
  }

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

namespace test {
using efgy::test::function;

static function JSONOutput(testJSONOutput);
static function JSONInput(testJSONInput);
}
