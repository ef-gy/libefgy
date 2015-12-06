/**\file
 * \brief VT100 terminal frontend implementation
 *
 * This file contains code to drive a vt100-like terminal using the data in a
 * terminal::terminal instance.
 *
 * \copyright
 * Copyright (c) 2012-2015, ef.gy Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#if !defined(EF_GY_VT100_H)
#define EF_GY_VT100_H

#include <ef.gy/terminal.h>
#include <ef.gy/maybe.h>
#include <sstream>
#include <functional>

namespace efgy {
namespace terminal {
/**\brief VT100 terminal frontend
 *
 * A frontend for the terminal::terminal template, this class
 * implements a very simple optimiser to output the screen buffer to a
 * VT100, or any of the many newer terminals that support the basic
 * escape codes the VT100 supported - which is pretty much all of them.
 *
 * \tparam T Data type for an individual cell's contents. The default
 *           is a "long", which should be enough to cover all possible
 *           unicode codepoints, ever. Hopefully, anyway.
 */
template <typename T = long> class vt100 : public terminal<T> {
public:
  /**\brief Base terminal type
   *
   * A convnenient typedef to the base type for this terminal.
   */
  typedef terminal<T> base;

  /**\brief Construct with streams
   *
   * Calls the parent constructor, which will set up the streams
   * connecting the terminal; this constructor will also set the
   * current foreground and background text colours to invalid
   * values, so they're reset the first time they're used.
   *
   * \param[out] pInput  Input stream for the terminal, e.g.
   *                     std::cin
   * \param[out] pOutput Output stream for the terminal, e.g.
   *                     std::cout
   * \param[in]  doSetup Whether to disable echo and canonical
   *                     mode on the terminal connected to stdin
   */
  vt100(std::istream &pInput = std::cin, std::ostream &pOutput = std::cout,
        const bool &doSetup = true)
      : base(pInput, pOutput, doSetup), currentForegroundColour(-1),
        currentBackgroundColour(-1), didSetup(doSetup) {}

  /*\brief Destructor
   *
   * Resets the terminal's colour and text attributes and starts
   * a new line, but only if the "doSetup" was true when the
   * constructor ran.
   */
  ~vt100(void) {
    if (didSetup) {
      std::cout << "\e[39;49;0m\n";
    }
  }

  using base::current;
  using base::target;
  using base::input;
  using base::output;
  using base::queue;
  using base::resize;
  using base::size;
  using base::read;
  using base::cursor;

  class command {
  public:
    command(const T &pCode,
            const std::vector<long> &pParameter = std::vector<long>())
        : code(pCode), parameter(pParameter) {}

    T code;
    std::vector<long> parameter;
  };

  std::vector<command> commandQueue;

  /**\brief Flush current buffer contents
   *
   * Updates the current state of the terminal by writing vt100
   * escape codes and updated character data to the terminal. The
   * function will only write up to targetOps many operations to
   * the terminal - this is to prevent slow terminals from
   * freezing an interactive programme, such as a game. To make
   * sure everything has been updated, call this method in a loop
   * until the return value is 0.
   *
   * The first argument, postProcess, should be a function that
   * returns the actual value to use for each cell. The arguments
   * to this function are the terminal, with associated metadata,
   * the line and the column in question. If you pass in 0 then
   * the function is not used, which is the same as using a
   * lambda like this one:
   *
   * \code{.cpp}
   * [] (const terminal<T> &t,
   *     const std::size_t &l,
   *     const std::size_t &c)
   * {
   *     return t.target[l][c];
   * }
   * \endcode
   *
   * The purpose of this parameter is, as the name suggests, to
   * allow for full-screen special effects, much like pixel
   * shaders do for 3D graphics. For example, the matrix demo
   * uses this to make sure all cells have the right colour, and
   * if you dig around in the code a bit there's a second
   * post-processing function that you can activate in the code
   * to make the matrix streams run along curvy lines.
   *
   * \param[in] postProcess Post-processing function.
   * \param[in] targetOps   Target number of operations; for
   *                        vt100 terminals, each operation tends
   *                        to be longer than one byte. The code
   *                        may also emit slightly more
   *                        operations than specified here.
   *
   * \returns The actual number of operations performed. If this
   *          is more than 0 you might want to call the function
   *          again to make sure everything is updated.
   */
  std::size_t
  flush(std::function<typename base::cell(const base &, const std::size_t &,
                                          const std::size_t &)> postProcess = 0,
        std::size_t targetOps = 1024) {
    std::size_t ops = 0;

    for (unsigned int l = 0; l < target.screen<T>::parent::size(); l++) {
      const std::size_t width = target[l].size();

      for (unsigned int c = 0; c < width; c++) {
        const cell<T> &ccell = current[l][c];
        const cell<T> tcell =
            postProcess ? postProcess(*this, l, c) : target[l][c];

        if (ccell != tcell) {
          if ((c == 0) && (cursor[1] == l)) {
            output << "\r";
            ops++;
            cursor[0] = 0;
          } else if ((c == 0) && (cursor[1] == (l - 1))) {
            output << "\n";
            ops++;
            cursor[0] = 0;
            cursor[1] = l;
          } else if ((cursor[0] != c) &&
                     ((cursor[1] != l) || (cursor[0] >= width))) {
            std::size_t vtl = l + 1;
            std::size_t vtc = c + 1;
            if (vtl == 1) {
              if (vtc == 1) {
                output << "\e[H";
                ops++;
              } else {
                output << "\e["
                       << ";" << vtc << "H";
                ops++;
              }
            } else if (vtc == 1) {
              output << "\e[" << vtl << "H";
              ops++;
            } else {
              output << "\e[" << vtl << ";" << vtc << "H";
              ops++;
            }
            cursor[0] = c;
            cursor[1] = l;
          } else if (cursor[1] != l) {
            signed long sp = (signed long)cursor[1] - (signed long)l;
            if (sp > 0) {
              if (sp == 1) {
                output << "\e[A";
              } else {
                output << "\e[" << sp << "A";
              }
              ops++;
            } else {
              if (sp == -1) {
                output << "\e[B";
              } else {
                output << "\e[" << -sp << "B";
              }
              ops++;
            }
            cursor[1] = l;
          } else if (cursor[0] != c) {
            signed long sp = (signed long)cursor[0] - (signed long)c;
            if (sp > 0) {
              if (sp == 1) {
                output << "\e[D";
              } else {
                output << "\e[" << sp << "D";
              }
              ops++;
            } else {
              if (sp == -1) {
                output << "\e[C";
              } else {
                output << "\e[" << -sp << "C";
              }
              ops++;
            }
            cursor[0] = c;
          }

          if (tcell.foregroundColour != currentForegroundColour) {
            if (tcell.foregroundColour < 8) {
              output << "\e[3" << tcell.foregroundColour << "m";
              ops++;
            } else {
              output << "\e[38;5;" << tcell.foregroundColour << "m";
              ops++;
            }
            currentForegroundColour = tcell.foregroundColour;
          }
          if (tcell.backgroundColour != currentBackgroundColour) {
            if (tcell.backgroundColour < 8) {
              output << "\e[4" << tcell.backgroundColour << "m";
              ops++;
            } else {
              output << "\e[48;5;" << tcell.backgroundColour << "m";
              ops++;
            }
            currentBackgroundColour = tcell.backgroundColour;
          }

          if ((tcell.content < 0x20) || (tcell.content == 0x7f)) {
            output << ".";
            ops++;
            /* don't print control characters */
          } else if (tcell.content < 0x80) {
            output << char(tcell.content);
            ops++;
          } else if (tcell.content < 0x800) {
            output << char(((tcell.content >> 6) & 0x1f) | 0xc0)
                   << char((tcell.content & 0x3f) | 0x80);
            ops++;
          } else if (tcell.content < 0x10000) {
            output << char(((tcell.content >> 12) & 0x0f) | 0xe0)
                   << char(((tcell.content >> 6) & 0x3f) | 0x80)
                   << char((tcell.content & 0x3f) | 0x80);
            ops++;
          } else if (tcell.content < 0x200000) {
            output << char(((tcell.content >> 18) & 0x07) | 0xf0)
                   << char(((tcell.content >> 12) & 0x3f) | 0x80)
                   << char(((tcell.content >> 6) & 0x3f) | 0x80)
                   << char((tcell.content & 0x3f) | 0x80);
            ops++;
          } else if (tcell.content < 0x4000000) {
            output << char(((tcell.content >> 24) & 0x03) | 0xf8)
                   << char(((tcell.content >> 18) & 0x3f) | 0x80)
                   << char(((tcell.content >> 12) & 0x3f) | 0x80)
                   << char(((tcell.content >> 6) & 0x3f) | 0x80)
                   << char((tcell.content & 0x3f) | 0x80);
            ops++;
          } else /* if (tcell.content < 0x80000000) */
          {
            output << char(((tcell.content >> 30) & 0x01) | 0xfc)
                   << char(((tcell.content >> 24) & 0x3f) | 0x80)
                   << char(((tcell.content >> 18) & 0x3f) | 0x80)
                   << char(((tcell.content >> 12) & 0x3f) | 0x80)
                   << char(((tcell.content >> 6) & 0x3f) | 0x80)
                   << char((tcell.content & 0x3f) | 0x80);
            ops++;
          }

          current[l][c] = tcell;
          cursor[0]++;
          if (ops >= targetOps) {
            output.flush();
            return ops;
          }
        }
      }
    }

    output.flush();
    return ops;
  }

  static std::vector<command>
  decode(std::vector<T> &queue,
         std::function<bool(const command &)> emitCommand = 0,
         std::function<bool(const T &)> emitLiteral = 0) {
    std::vector<command> r;

    enum parserState state = text;
    std::vector<T> result;

    maybe<long> vtparam;
    std::vector<long> vtparams;

    for (T v : queue) {
      switch (state) {
      case text:
        switch (v) {
        case '\e':
          state = escape1;
          break;
        default:
          if (!emitLiteral || emitLiteral(v)) {
            result.push_back(v);
          }
        }
        break;
      case escape1:
        switch (v) {
        case '[':
          state = escape2;
          break;
        default:
          state = text;
          if (!emitLiteral || emitLiteral(T('\e'))) {
            result.push_back(T('\e'));
          }
          if (!emitLiteral || emitLiteral(v)) {
            result.push_back(v);
          }
        }
        break;
      case escape2:
        switch (v) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          vtparam = (long)vtparam * 10 + (v - '0');
          break;
        case ';':
          if ((bool)vtparam) {
            vtparams.push_back((long)vtparam);
            vtparam = maybe<long>();
          }
          break;
        default:
          if ((bool)vtparam) {
            vtparams.push_back((long)vtparam);
            vtparam = maybe<long>();
          }
          command c(v, vtparams);
          if (!emitCommand || emitCommand(c)) {
            r.push_back(c);
          }
          state = text;
          vtparams.clear();
          break;
        }
        break;
      }
    }

    /* incomplete commands: don't emit but feed back into the
       buffer */
    if (state == escape1) {
      result.push_back(T('\e'));
    } else if (state == escape2) {
      result.push_back(T('\e'));
      result.push_back('[');

      bool first = true;
      if ((bool)vtparam) {
        vtparams.push_back((long)vtparam);
        vtparam = maybe<long>();
      }
      for (auto p : vtparams) {
        if (first) {
          first = false;
        } else {
          result.push_back(';');
        }

        std::vector<T> pv;

        while (p > 0) {
          pv.push_back('0' + p % 10);
          p /= 10;
        }

        result.insert(result.end(), pv.rbegin(), pv.rend());
      }
    }

    queue = result;

    return r;
  }

  bool read(std::function<bool(const command &)> emitCommand = 0,
            std::function<bool(const T &)> emitLiteral = 0) {
    do {
      base::read();
    } while (input.rdbuf()->in_avail() > 0);

    auto q = decode(queue,
                    [emitCommand, this](const command &c) -> bool {
                      bool keep = true;
                      if (c.code == 'R') {
                        switch (c.parameter.size()) {
                        case 0:
                          cursor[1] = 0;
                          cursor[0] = 0;
                          break;
                        case 1:
                          cursor[1] = c.parameter[0] - 1;
                          cursor[0] = 0;
                          break;
                        default:
                          cursor[1] = c.parameter[0] - 1;
                          cursor[0] = c.parameter[1] - 1;
                          break;
                        }
                        keep = false;
                      }
                      return (bool)emitCommand ? emitCommand(c) : keep;
                    },
                    [emitLiteral](const T &l) -> bool {
                      return (bool)emitLiteral ? emitLiteral(l) : true;
                    });

    commandQueue.insert(commandQueue.end(), q.begin(), q.end());

    return true;
  }

  /**\brief Query current cursor position from terminal
   *
   * This function will ask the terminal where the cursor
   * currently resides, then try to read() from the input stream
   * at least once iff doRead was not set to false.. This may or
   * may not update the input position depending on whether the
   * terminal replies immediately.
   *
   * \param[in] doRead Whether to try reading from the terminal
   *                   at least once.
   *
   * \returns 'true' if the query was sent was updated, 'false'
   *          otherwise. The code will always emit the query, so
   *          'false' should not come up.
   */
  bool updatePosition(bool doRead = true) {
    output << "\e[6n";

    if (doRead) {
      read();
    }

    return true;
  }

private:
  /**\brief Has the constructor set up the terminal?
   *
   * Set to 'true' if the constructor has modified terminal
   * settings which it should reverse in the destructor.
   */
  const bool didSetup;

  /**\brief Terminal's current foreground colour
   *
   * Set as part of running flush; VT100s have 'current' colours
   * and other text attributes, which are used when printing a
   * character at the current position. The flush() function
   * needs to know what this colour is so it can issue commands
   * to change that colour, if necessary.
   */
  std::size_t currentForegroundColour;

  /**\brief Terminal's current background colour
   *
   * Set as part of running flush; VT100s have 'current' colours
   * and other text attributes, which are used when printing a
   * character at the current position. The flush() function
   * needs to know what this colour is so it can issue commands
   * to change that colour, if necessary.
   */
  std::size_t currentBackgroundColour;

  enum parserState { text, escape1, escape2 };
};
};
};

#endif
