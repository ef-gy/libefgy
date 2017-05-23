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

#include <ef.gy/json.h>
#include <ef.gy/test-case.h>
#include <sstream>

using namespace efgy;

/* JSON input tests
 * @log Where to write log messages to.
 *
 * Tries to parse some JSON with the JSON string parser.
 *
 * @return 'true' on success, 'false' otherwise.
 */
bool testJSONInput(std::ostream &log) {
  struct sampleData {
    std::string input, remainder, output;
  };

  std::vector<sampleData> tests{
      {"true", "", "true"},
      {"  42.23", "", "42.23"},
      {"42.00", "", "42"},
      {"[false, true, null]", "", "[false,true,null]"},
      {"", "", "null"},
      {"{ \"a\" : true , \"b\" : false }", "", "{\"a\":true,\"b\":false}"},
      {"{\"a\":true,\"b\":false,\"q\":[false, true, false],\"c\":null}", "",
       "{\"a\":true,\"b\":false,\"c\":null,\"q\":[false,true,false]}"},
      {"{\"a\":true,\"b\":false,\"q\":[1,2 ,3 , 4],\"c\":null}", "",
       "{\"a\":true,\"b\":false,\"c\":null,\"q\":[1,2,3,4]}"},
  };

  for (const auto &tt : tests) {
    json::json va;
    const auto r = tt.input >> va;
    if (r != tt.remainder) {
      log << "JSON parsing (" << tt.input << ") had bad remainder: " << r
          << "; expected: " << tt.remainder << "\n";
      return false;
    }
    std::string so;
    so << va;
    if (so != tt.output) {
      log << "JSON parsing (" << tt.input << ") had bad string output: " << so
          << "; expected: " << tt.output << "\n";
      return false;
    }
  }

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

static function JSONInput(testJSONInput);
static function size(testSize);
}
