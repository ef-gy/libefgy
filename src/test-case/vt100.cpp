/**\file
 * \brief Test cases for the VT100 terminal frontend driver
 *
 * Test cases in this file verify the functionality of the vt100.h terminal
 * frontend.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#include <ef.gy/test-case.h>

#include <ef.gy/vt100.h>

using namespace efgy::terminal;

/**\brief Tests the vt100 command decoder
 * \test Tries to decode a set of vt100 commands to see if the result is as
 *       expected.
 *
 * \param[out] log A stream for test cases to log messages to.
 *
 * \return Zero when everything went as expected, nonzero otherwise.
 */
int testVT100Decode(std::ostream &log) {
  std::string sq = "\e[11;12q";

  {
    std::vector<long> queue(sq.begin(), sq.end());

    auto dec = vt100<long>::decode(queue);

    if (dec.size() != 1) {
      log << "vt100 decode returned an incorrect number of results; should be "
             "1, have " << dec.size() << ".\n";
      return 1;
    }

    const auto &e = dec.front();

    if (e.code != 'q') {
      log << "vt100 decode returned an incorrect code; should be 'q' but is '"
          << e.code << "'.\n";
      return 2;
    }

    if (e.parameter.size() != 2) {
      log << "vt100 decode came up with the wrong parameters; should have 2 "
             "but have " << e.parameter.size() << ".\n";
      log << "parameters are:\n";
      for (const auto &p : e.parameter) {
        log << p << "\n";
      }
      return 3;
    }

    if (e.parameter[0] != 11) {
      log << "vt100 decode came up with the wrong parameter; parameter 0 "
             "should be 11 but is " << e.parameter[0] << "\n";
      return 4;
    }

    if (e.parameter[1] != 12) {
      log << "vt100 decode came up with the wrong parameter; parameter 1 "
             "should be 12 but is " << e.parameter[1] << "\n";
      return 5;
    }
  }

  sq = "\e[A";

  {
    std::vector<long> queue(sq.begin(), sq.end());

    auto dec = vt100<long>::decode(queue);

    if (dec.size() != 1) {
      log << "vt100 decode returned an incorrect number of results; should be "
             "1, have " << dec.size() << ".\n";
      return 11;
    }

    const auto &e = dec.front();

    if (e.code != 'A') {
      log << "vt100 decode returned an incorrect code; should be 'A' but is '"
          << e.code << "'.\n";
      return 12;
    }

    if (e.parameter.size() != 0) {
      log << "vt100 decode came up with the wrong parameters; should have 0 "
             "but have " << e.parameter.size() << ".\n";
      log << "parameters are:\n";
      for (const auto &p : e.parameter) {
        log << p << "\n";
      }
      return 13;
    }
  }

  sq = "\e[A\e[B";

  {
    std::vector<long> queue(sq.begin(), sq.end());

    auto dec = vt100<long>::decode(queue);

    if (dec.size() != 2) {
      log << "vt100 decode returned an incorrect number of results; should be "
             "2, have " << dec.size() << ".\n";
      return 21;
    }

    {
      const auto &e = dec[0];

      if (e.code != 'A') {
        log << "vt100 decode returned an incorrect code; should be 'A' but is '"
            << e.code << "'.\n";
        return 22;
      }

      if (e.parameter.size() != 0) {
        log << "vt100 decode came up with the wrong parameters; should have 0 "
               "but have " << e.parameter.size() << ".\n";
        log << "parameters are:\n";
        for (const auto &p : e.parameter) {
          log << p << "\n";
        }
        return 23;
      }
    }

    {
      const auto &e = dec[1];

      if (e.code != 'B') {
        log << "vt100 decode returned an incorrect code; should be 'B' but is '"
            << e.code << "'.\n";
        return 24;
      }

      if (e.parameter.size() != 0) {
        log << "vt100 decode came up with the wrong parameters; should have 0 "
               "but have " << e.parameter.size() << ".\n";
        log << "parameters are:\n";
        for (const auto &p : e.parameter) {
          log << p << "\n";
        }
        return 25;
      }
    }
  }

  sq = "\e[A]\e[Bfoo";

  {
    std::vector<long> queue(sq.begin(), sq.end());

    auto dec = vt100<long>::decode(queue);

    if (dec.size() != 2) {
      log << "vt100 decode returned an incorrect number of results; should be "
             "2, have " << dec.size() << ".\n";
      return 31;
    }

    {
      const auto &e = dec[0];

      if (e.code != 'A') {
        log << "vt100 decode returned an incorrect code; should be 'A' but is '"
            << e.code << "'.\n";
        return 32;
      }

      if (e.parameter.size() != 0) {
        log << "vt100 decode came up with the wrong parameters; should have 0 "
               "but have " << e.parameter.size() << ".\n";
        log << "parameters are:\n";
        for (const auto &p : e.parameter) {
          log << p << "\n";
        }
        return 33;
      }
    }

    {
      const auto &e = dec[1];

      if (e.code != 'B') {
        log << "vt100 decode returned an incorrect code; should be 'B' but is '"
            << e.code << "'.\n";
        return 34;
      }

      if (e.parameter.size() != 0) {
        log << "vt100 decode came up with the wrong parameters; should have 0 "
               "but have " << e.parameter.size() << ".\n";
        log << "parameters are:\n";
        for (const auto &p : e.parameter) {
          log << p << "\n";
        }
        return 35;
      }
    }

    std::string s(queue.begin(), queue.end());

    if (s != "]foo") {
      log << "vt100 decode came up with the wrong remainder; should be ']foo' "
             "but is '" << s << "'.\n";
      return 36;
    }
  }

  sq = "\e[11;12";

  {
    std::vector<long> queue(sq.begin(), sq.end());

    auto dec = vt100<long>::decode(queue);

    if (dec.size() != 0) {
      log << "vt100 decode returned an incorrect number of results; should be "
             "0, have " << dec.size() << ".\n";
      return 41;
    }

    std::string s(queue.begin(), queue.end());

    if (s != sq) {
      log << "vt100 decode came up with the wrong remainder; should be '" << sq
          << "' but is '" << s << "'.\n";
      return 42;
    }
  }

  sq = "p\e[11r";

  {
    long ce = 0, le = 0;
    std::vector<long> queue(sq.begin(), sq.end());

    auto dec =
        vt100<long>::decode(queue,
                            [&ce](const vt100<long>::command &c) -> bool {
                              if (c.code == 'r' && c.parameter.size() == 1 &&
                                  c.parameter[0] == 11) {
                                ce++;
                              }
                              return false;
                            },
                            [&le](const long &l) -> bool {
                              if (l == 'p') {
                                le++;
                              }
                              return false;
                            });

    if (dec.size() != 0) {
      log << "vt100 decode returned an incorrect number of results; should be "
             "0, have " << dec.size() << ".\n";
      return 51;
    }

    if (ce != 1) {
      log << "vt100 decode emitted the wrong number of (correct) commands; "
             "should be 1, was " << ce << "\n";
      return 52;
    }

    if (le != 1) {
      log << "vt100 decode emitted the wrong number of (correct) literalss; "
             "should be 1, was " << le << "\n";
      return 53;
    }

    std::string s(queue.begin(), queue.end());

    if (s != "") {
      log << "vt100 decode came up with the wrong remainder; should be '' but "
             "is '" << s << "'.\n";
      return 54;
    }
  }

  return 0;
}

TEST_BATCH(testVT100Decode)
