
#include <iostream>
#include <sstream>

#include <ef.gy/test-case.h>
#include <ef.gy/string.h>

using std::string;
using namespace efgy;
using std::ostringstream;

int testString(std::ostream &log) {

    {
      ostringstream oss;
      print(oss, "test");
      string output_string = oss.str();

      if (output_string != "test")
      {
          log << "No placeholders and no variables.";
          return 0;
      }
    }

    {
      ostringstream oss;
      print(oss, "test %");
      string output_string = oss.str();

      if (output_string != "test %")
      {
          log << "One placeholders and no variables.";
          return 1;
      }
    }

    {
      ostringstream oss;
      print(oss, "test %", 42);
      string output_string = oss.str();

      if (output_string != "test 42")
      {
          log << "One placeholders and one variable.";
          return 2;
      }
    }

    {
      ostringstream oss;
      print(oss, "test % %", 42);
      string output_string = oss.str();

      if (output_string != "test 42 %")
      {
          log << "Two placeholders and one variable.";
          return 3;
      }
    }

    {
      ostringstream oss;
      print(oss, "% test %", 1, 2);
      string output_string = oss.str();

      if (output_string != "1 test 2")
      {
          log << "Two placeholders and two variables.";
          return 4;
      }
    }

    {
      ostringstream oss;
      print(oss, "test % % %", 1, 2, 3);
      string output_string = oss.str();

      if (output_string != "test 1 2 3")
      {
          log << "3 placeholders at the end did not get replaced.";
          return 5;
      }
    }

    return 0;
}

TEST_BATCH(testString)