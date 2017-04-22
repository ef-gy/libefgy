/* Test cases for the JSON render output
 *
 * The test cases in this file test the functionality of JSON renderer.
 *
 * See also:
 * * Project Documentation: https://ef.gy/documentation/libefgy
 * * Project Source Code: https://github.com/ef-gy/libefgy
 * * Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 *
 * @copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 */

#include <iostream>

#include <ef.gy/stream-json.h>
#include <ef.gy/test-case.h>
#include <sstream>

using namespace efgy;

/* JSON output tests
 * @log Where to write log messages to.
 *
 * Writes some JSON to a stringstream to see if it compiles and works as
 * intended.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testJSONOutput(std::ostream &log) {
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
      return false;
    }
  }

  for (const auto &tt : testsInt) {
    std::ostringstream s("");
    s << json::tag() << tt.in;

    if (s.str() != tt.out) {
      log << "bad JSON output: got '" << s.str() << "', expected '" << tt.out
          << "'\n";
      return false;
    }
  }

  for (const auto &tt : testsDouble) {
    std::ostringstream s("");
    s << json::tag() << tt.in;

    if (s.str() != tt.out) {
      log << "bad JSON output: got '" << s.str() << "', expected '" << tt.out
          << "'\n";
      return false;
    }
  }

  for (const auto &tt : testsString) {
    std::ostringstream s("");
    s << json::tag() << tt.in;

    if (s.str() != tt.out) {
      log << "bad JSON output: got '" << s.str() << "', expected '" << tt.out
          << "'\n";
      return false;
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
    return false;
  }

  s.str("");
  s << json::tag() << m;

  if (s.str() != "{\"a\":42,\"b\":23,\"c\":1}") {
    log << "unexpected JSON output: " << s.str();
    return false;
  }

  s.str("");
  s << json::tag() << mx;

  if (s.str() != "{\"d\":{\"a\":42,\"b\":23,\"c\":1}}") {
    log << "unexpected JSON output: " << s.str();
    return false;
  }

  return true;
}

/* JSON input tests
 * @log Where to write log messages to.
 *
 * Tries to parse some JSON with the JSON string parser.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testJSONInput(std::ostream &log) {
  json::json v;

  v.type = json::yes;

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

  return true;
}

/* Test json::json::size().
 * @log Where to write log messages to.
 *
 * json::size() returns the size of a JSON value; this exercises the function to
 * make sure it works correctly.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testSize(std::ostream &log) {
  struct sampleData {
    json::json in;
    std::size_t res;
  };

  std::vector<sampleData> tests{
      {2.l, 1},
      {"foo", 1},
      {json::json::objectType{{"foo", "bar"}, {"baz", 2.l}}, 2},
      {json::json::arrayType{"foo", "bar", "baz"}, 3},
  };

  for (const auto &tt : tests) {
    const auto s = tt.in.size();
    if (s != tt.res) {
      log << "size()=" << s << ", but expected " << tt.res << "\n";
      return false;
    }
  }
  return true;
}

namespace test {
using efgy::test::function;

static function JSONOutput(testJSONOutput);
static function JSONInput(testJSONInput);
static function size(testSize);
}
