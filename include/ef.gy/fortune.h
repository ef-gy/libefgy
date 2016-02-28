/**\file
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_FORTUNE_H)
#define EF_GY_FORTUNE_H

#include <ef.gy/httpd.h>

#include <cstdlib>
#include <dirent.h>
#include <iostream>
#include <fstream>

namespace efgy {

class fortune {
public:
  fortune(void) : cookies(), data() {}

  static fortune &common(void) {
    static fortune f;
    return f;
  }

protected:
  class cookie {
  public:
    const std::string file;

    cookie(bool pROT13, const std::string &pData, const std::string &pFile)
        : rot13(pROT13), data(pData), file(pFile) {}

    operator std::string(void) const {
      std::string r = data;

      if (rot13) {
        for (size_t i = 0; i < r.size(); i++) {
          char c = r[i];

          switch (c) {
          case 'a':
          case 'b':
          case 'c':
          case 'd':
          case 'e':
          case 'f':
          case 'g':
          case 'h':
          case 'i':
          case 'j':
          case 'k':
          case 'l':
          case 'm':
          case 'A':
          case 'B':
          case 'C':
          case 'D':
          case 'E':
          case 'F':
          case 'G':
          case 'H':
          case 'I':
          case 'J':
          case 'K':
          case 'L':
          case 'M':
            r[i] = c + 13;
            break;
          case 'n':
          case 'o':
          case 'p':
          case 'q':
          case 'r':
          case 's':
          case 't':
          case 'u':
          case 'v':
          case 'w':
          case 'x':
          case 'y':
          case 'z':
          case 'N':
          case 'O':
          case 'P':
          case 'Q':
          case 'R':
          case 'S':
          case 'T':
          case 'U':
          case 'V':
          case 'W':
          case 'X':
          case 'Y':
          case 'Z':
            r[i] = c - 13;
            break;
          default:
            break;
          }
        }
      }

      return r;
    }

  protected:
    bool rot13;
    const std::string data;
  };

  std::vector<cookie> cookies;
  std::map<std::string, std::string> data;

public:
  bool prepare(const std::string &dir, const bool doROT13 = false) {
    static const std::regex dataFile(".*/[a-zA-Z-]+");
    std::smatch matches;
    DIR *d = opendir(dir.c_str());

    if (!d) {
      return false;
    }

    struct dirent *en;

    while ((en = readdir(d)) != 0) {
      std::string e = dir + en->d_name;

      if (regex_match(e, matches, dataFile)) {
        std::ifstream t(e);
        std::stringstream buffer;
        buffer << t.rdbuf();

        data[e] = buffer.str();

        const std::string &p = data[e];
        const char *data = p.c_str();
        size_t start = 0;
        enum { stN, stNL, stNLP } state = stN;

        for (size_t c = 0; c < p.size(); c++) {
          switch (data[c]) {
          case '\n':
            switch (state) {
            case stN:
              state = stNL;
              break;
            case stNLP:
              cookies.push_back(cookie(
                  doROT13, std::string(p.data() + start, c - start - 1), e));
              start = c + 1;
            default:
              state = stN;
              break;
            }
            break;

          case '%':
            switch (state) {
            case stNL:
              state = stNLP;
              break;
            default:
              state = stN;
              break;
            }
            break;

          case '\r':
            break;

          default:
            state = stN;
            break;
          }
        }
      }
    }
    closedir(d);

    return true;
  }

  const std::size_t size(void) const { return cookies.size(); }

  const cookie &get(std::size_t i) const {
    if (i < cookies.size()) {
      return cookies[i];
    }

    return get();
  }

  const cookie &get(void) const { return get((rand() % size())); }
};
}

#endif
