/**\file
 * \brief Terminal I/O Helpers
 *
 * The templates in this file extend the functionality of the terminal.h base
 * templates with useful 'standard' constructs, like drawing text and boxes at
 * given offsets on the screen.
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

#if !defined(EF_GY_TERMINAL_WRITER_H)
#define EF_GY_TERMINAL_WRITER_H

#include <ef.gy/terminal.h>
#include <string>
#include <algorithm>

namespace efgy {
namespace terminal {

/**\brief Terminal Helper Extension
 *
 * Objects of this template wrap around a "normal" terminal class to allow for
 * easier drawing of text, boxes, etc.
 *
 * \note The methods in this class are implemented such that you can chain
 *       commands easily, by always returning a reference to the objct they were
 *       called on.
 *
 * \note All of the methods here only update the target buffer of the terminal,
 *       meaning you still have to sync/flush the changes in order for them to
 *       actually be sent out.
 *
 * \tparam T The character type of the terminal to handle, e.g. long for UTF8.
 */
template <typename T = long> class writer {
public:
  /**\brief Construct with terminal
   *
   * Constructs a new terminal output helper and binds it to the given terminal
   * instance. Also sets colours and position to sensible defaults (i.e.
   * top-left, writing white on black).
   *
   * \param[out] pOutput The output terminal to bind to.
   */
  writer(terminal<T> &pOutput)
      : output(pOutput), foreground(7), background(0), position({
    0, 0
  }) {}

  /**\brief Current background colour.
   *
   * New writes use this colour, except for some very specialised functions that
   * override the foreground colour, e.g. coloured multi-bars.
   */
  std::size_t foreground;

  /**\brief Current background colour.
   *
   * New writes use this colour, except for some very specialised functions that
   * override the background colour, e.g. coloured multi-bars.
   */
  std::size_t background;

  /**\brief Current cursor position
   *
   * This is the position on-screen that new writes go to.
   */
  std::array<ssize_t, 2> position;

  /**\brief Resolve cursor position
   *
   * This function implements line wrapping and negative positions (used to
   * indicate positions from the right/bottom instead of the left/top). Output
   * functions use this after updating the cursor position, so that the cursor
   * is moved to somewhere it can actually be seen.
   *
   * \returns A reference to *this.
   */
  writer &solve(void) {
    const auto dim = output.size();

    position[0] = (position[0] >= 0) ? position[0] : (dim[0] + position[0]);
    position[1] = (position[1] >= 0) ? position[1] : (dim[1] + position[1]);

    position[0] = (position[0] >= 0) ? position[0] : 0;
    position[1] = (position[1] >= 0) ? position[1] : 0;

    position[1] += (position[0] >= dim[0]) ? 1 : 0;
    position[0] = (position[0] >= dim[0]) ? 0 : position[0];
    position[1] = (position[1] >= dim[1]) ? 0 : position[1];

    return *this;
  }

  /**\brief Output a single character
   *
   * Given a single character of the target terminal's type, this will write
   * that character to the terminal and update the screen position - with line
   * wrapping, if needed. The current foreground and background colours are also
   * applied.
   *
   * \param[in] ch The character to write.
   *
   * \returns A reference to *this.
   */
  writer &write(const T ch) {
    solve();

    auto &cell = output.target[position[1]][position[0]];
    if (ch > 0) {
      cell.content = ch;
    }
    cell.foregroundColour = foreground;
    cell.backgroundColour = background;

    position[0]++;
    return *this;
  }

  template <typename C>
  writer &write(const std::basic_string<C> &str, const std::size_t width,
                const ssize_t colour = -1) {
    const auto fg = foreground;

    if (colour != -1) {
      foreground = colour;
    }

    for (std::size_t i = 0; i < width; i++) {
      write(i < str.size() ? str[i] : ' ');
    }

    if (colour != -1) {
      foreground = fg;
    }

    return *this;
  }

  writer &bar(const ssize_t min, const ssize_t max, const std::size_t width,
              const T full = '#', const T left = '[', const T right = ']') {
    const double perc =
        max > 0 ? double(std::max(min, ssize_t(0))) / double(max) : 0.;
    const std::size_t fullchars = perc > 0 ? (width - 2) * perc : 0;

    write(left);
    for (std::size_t i = 0; i < (width - 2); i++) {
      write(i < fullchars ? full : ' ');
    }
    write(right);

    return *this;
  }

  writer &bar2(const ssize_t min1, const ssize_t max1, const ssize_t min2,
               const ssize_t max2, const std::size_t width,
               const T full = 0x2588, const T uhf = 0x2580,
               const T lhf = 0x2584, const T left = '[', const T right = ']') {
    const double perc1 =
        max1 > 0 ? double(std::max(min1, ssize_t(0))) / double(max1) : 0.;
    const double perc2 =
        max2 > 0 ? double(std::max(min2, ssize_t(0))) / double(max2) : 0.;
    const double minw = perc1 < perc2 ? perc1 : perc2;
    const std::size_t barwidth = width - 2;
    const std::size_t fullchars = minw * barwidth;
    const std::size_t uchars = perc1 * barwidth;
    const std::size_t lchars = perc2 * barwidth;

    write(left);
    for (std::size_t i = 0; i < barwidth; i++) {
      if (i < fullchars) {
        write(full);
      } else if (i < uchars) {
        write(uhf);
      } else if (i < lchars) {
        write(lhf);
      } else {
        write(' ');
      }
    }
    write(right);

    return *this;
  }

  writer &bar2c(const ssize_t min1, const ssize_t max1, const ssize_t min2,
                const ssize_t max2, const std::size_t width,
                const std::size_t colour1, const std::size_t colour2,
                const T lhf = 0x2584, const T left = '[', const T right = ']') {
    const double perc1 =
        max1 > 0 ? double(std::max(min1, ssize_t(0))) / double(max1) : 0.;
    const double perc2 =
        max2 > 0 ? double(std::max(min2, ssize_t(0))) / double(max2) : 0.;
    const double minw = perc1 < perc2 ? perc1 : perc2;
    const std::size_t barwidth = width - 2;
    const std::size_t fullchars = minw * barwidth;
    const std::size_t uchars = perc1 * barwidth;
    const std::size_t lchars = perc2 * barwidth;

    const std::size_t fg = foreground;
    const std::size_t bg = background;

    write(left);
    for (std::size_t i = 0; i < barwidth; i++) {
      if (i < fullchars) {
        foreground = colour2;
        background = colour1;
        write(lhf);
      } else if (i < uchars) {
        foreground = bg;
        background = colour1;
        write(lhf);
      } else if (i < lchars) {
        foreground = colour2;
        background = bg;
        write(lhf);
      } else {
        foreground = fg;
        background = bg;
        write(' ');
      }
    }

    foreground = fg;
    background = bg;

    write(right);

    return *this;
  }

  writer &to(const ssize_t &column, const ssize_t &line) {
    position = { column, line };

    return *this;
  }

  writer &x(const ssize_t &column) {
    position[0] = column;

    return *this;
  }

  writer &y(const ssize_t &line) {
    position[1] = line;

    return *this;
  }

  writer &colour(const ssize_t columns = -1, const ssize_t lines = -1) {
    const auto dim = output.size();
    const auto pos = position;

    const std::size_t cs =
        columns < 0 ? dim[0] - pos[0] + columns + 1 : columns;
    const std::size_t ls = lines < 0 ? dim[1] - pos[1] + lines + 1 : lines;

    for (std::size_t l = 0; l < ls; l++) {
      position[0] = pos[0];
      position[1] = pos[1] + l;
      for (std::size_t c = 0; c < cs; c++) {
        write(0);
      }
    }

    return *this;
  }

  writer &box(const ssize_t columns = -1, const ssize_t lines = -1) {
    const auto dim = output.size();
    const auto pos = position;

    const std::size_t cs =
        columns < 0 ? dim[0] - pos[0] + columns + 1 : columns;
    const std::size_t ls = lines < 0 ? dim[1] - pos[1] + lines + 1 : lines;

    for (std::size_t l = 0; l < ls; l++) {
      position[0] = pos[0];
      position[1] = pos[1] + l;
      for (std::size_t c = 0; c < cs; c++) {
        if ((l == 0) || (l == (ls - 1))) {
          if ((c == 0) || (c == (cs - 1))) {
            write('+');
          } else {
            write('-');
          }
        } else if ((c == 0) || (c == (cs - 1))) {
          write('|');
        } else {
          write(0);
        }
      }
    }

    return *this;
  }

  writer &clear(const ssize_t columns = -1, const ssize_t lines = -1,
                const T sym = ' ') {
    const auto dim = output.size();
    const auto pos = position;

    const std::size_t cs =
        columns < 0 ? dim[0] - pos[0] + columns + 1 : columns;
    const std::size_t ls = lines < 0 ? dim[1] - pos[1] + lines + 1 : lines;

    for (std::size_t l = 0; l < ls; l++) {
      position[0] = pos[0];
      position[1] = pos[1] + l;
      for (std::size_t c = 0; c < cs; c++) {
        write(sym);
      }
    }

    return *this;
  }

protected:
  terminal<T> &output;
};
}
}

#endif
