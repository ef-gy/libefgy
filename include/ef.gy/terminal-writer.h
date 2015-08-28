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
   * Constructs a new terminal output helper and binds it to the given
   * terminal
   * instance. Also sets colours and position to sensible defaults (i.e.
   * top-left, writing white on black).
   *
   * \param[out] pOutput The output terminal to bind to.
   */
  writer(terminal<T> &pOutput)
      : output(pOutput), foreground(7), background(0), position({0, 0}) {}

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

  /**\brief Output a string
   *
   * Writes the given string to the current cursor position, wrapping lines as
   * needed, and using the current colour settings. The foreground colour can be
   * overriden using the colour parameter, but is then reset at the time the
   * function returns.
   *
   * \param[in] str    The string to write to the terminal.
   * \param[in] width  The maximum number of characters of the string to write.
   * \param[in] colour A colour to use as the foreground colour.
   *
   * \returns A reference to *this.
   */
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

  /**\brief Draw a bar
   *
   * Given a value and a maximum value, draw a bar that is proportionally filled
   * using ASCII-art. For instance, if you drew a bar with value 13 out of 15,
   * and made that 50 characters wide, the result would be this:
   *
   * \code
   * [###########################################     ]
   * \endcode
   *
   * \param[in] min   Value of the bar.
   * \param[in] max   Maximum value of the bar.
   * \param[in] width Width of the bar.
   * \param[in] full  Character to use for filled parts of the bar.
   * \param[in] left  Left border character.
   * \param[in] right Right border character.
   *
   * \returns A reference to *this.
   */
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

  /**\brief Draw two bars in one line using colours
   *
   * This function draws a bar at the given coordinates that is one line high,
   * by using coloured half-block characters. E.g. imagine the following with
   * the first bunch of characters in red/white and the remainder in black/white
   * with white being the bottom colour.
   *
   * \code
   * [▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
   * ]
   * \endcode
   *
   * \param[in] min1    Value of the first bar.
   * \param[in] max1    Maximum value of the first bar.
   * \param[in] min2    Value of the second bar.
   * \param[in] max2    Maximum value of the second bar.
   * \param[in] width   Width of the two bars.
   * \param[in] colour1 Colour of the first bar.
   * \param[in] colour2 Colour of the second bar.
   * \param[in] lhf     An UTF8 character that is a half-filled block.
   * \param[in] left    Left border character.
   * \param[in] right   Right border character.
   *
   * \returns A reference to *this.
   */
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

  /**\brief Set the cursor position
   *
   * Updates the cursor position to the given coordinates.
   *
   * \param[in] column The column to move the cursor to.
   * \param[in] line   The line to move the cursor to.
   *
   * \returns A reference to *this.
   */
  writer &to(const ssize_t &column, const ssize_t &line) {
    position = {column, line};

    return *this;
  }

  /**\brief Set the current column
   *
   * This function updates the current cursor position's column as given.
   *
   * \param[in] column The column to move the cursor to.
   *
   * \returns A reference to *this.
   */
  writer &x(const ssize_t &column) {
    position[0] = column;

    return *this;
  }

  /**\brief Set the current line
   *
   * This function updates the current cursor position's line as given.
   *
   * \param[in] line The line to move the cursor to.
   *
   * \returns A reference to *this.
   */
  writer &y(const ssize_t &line) {
    position[1] = line;

    return *this;
  }

  /**\brief Change the background and foreground colour of an area
   *
   * Like clear(), but only sets the colour and does not write any symbols.
   *
   * \param[in] columns Width of the area in columns.
   * \param[in] lines   Height of the area in lines.
   *
   * \returns A reference to *this.
   */
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

  /**\brief Draw an ASCII-art box.
   *
   * This function draws a box at the given, current coordinates with the given
   * dimensions. Like so:
   *
   * \code
   * +--------------+
   * |              |
   * |              |
   * +--------------+
   * \endcode
   *
   * \param[in] columns Width of the box in columns.
   * \param[in] lines   Height of the box in lines.
   *
   * \returns A reference to *this.
   */
  writer &box(const ssize_t columns, const ssize_t lines) {
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

  /**\brief Clear a given area
   *
   * Clears a box of the given size around the current cursor position, by
   * writing the given symbol to all the affected positions. The default is to
   * write a space to all columns and lines, effectively blanking out the
   * screen.
   *
   * The current foreground and background colour are applied and not cleared.
   *
   * \param[in] columns How many columns to clear; -1 means the whole line.
   * \param[in] lines   How many lines to clear; -1 means all of them.
   * \param[in] sym     The symbol to write.
   *
   * \returns A reference to *this.
   */
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
  /**\brief Output terminal reference
   *
   * Set in the constructor; All output is sent to this terminal's target
   * buffer. Output is not automatically sync'd/flush'd, you'll have to do that
   * yourself.
   */
  terminal<T> &output;
};
}
}

#endif
