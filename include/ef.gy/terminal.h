/**\file
 * \brief Terminal I/O helpers
 *
 * This header contains templates that should make it fairly easy to use
 * text-based terminals efficiently. The general idea is that this header
 * provides a class template, terminal::terminal, with two screen buffers, one
 * for the current state of the terminal and one for the target state of the
 * terminal. Terminal-specific classes can then derive from this base class and
 * implement efficient algorithms to transform the terminal state from the
 * current state to the target state.
 *
 * One such deriving class is terminal::vt100, see ef.gy/vt100.h for more
 * details on that. Also see src/matrix.cpp for some example code if you're
 * stuck with the documentation for some reason.
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

#if !defined(EF_GY_TERMINAL_H)
#define EF_GY_TERMINAL_H

#include <ef.gy/render.h>
#include <ef.gy/maybe.h>
#include <vector>
#include <array>
#include <iostream>
#if !defined(NO_IOCTL)
#include <sys/ioctl.h>
#endif
#if !defined(NO_TERMIOS)
#include <termios.h>
#endif

namespace efgy {
/**\brief Terminal I/O helpers
 *
 * Contains classes and templates that should make it easy - or at least
 * easier - to produce snappy TUI interfaces, or other terminal
 * applications. See src/matrix.cpp for an example.
 */
namespace terminal {
/**\brief Terminal buffer cell
 *
 * Contains all the attributes associated with an individual cell in a
 * terminal buffer: the contained character, colours and any applicable
 * attributes.
 *
 * \tparam T Data type for an individual cell's contents. The default
 *           is a "long", which should be enough to cover all possible
 *           unicode codepoints, ever. Hopefully, anyway.
 */
template <typename T = long> class cell {
public:
  /**\brief The character in the cell
   *
   * Contains the current character in the given cell. Expect
   * this to be a unicode codepoint.
   */
  T content;

  /**\brief Foreground colour
   *
   * This is the colour of the glyph itself. The values in here
   * do sort of depend on the terminal settings on the output
   * driver, but expect xterm-256 codes to work as intended.
   */
  int foregroundColour;

  /**\brief Background colour
   *
   * This is the colour of the glyph's background. The values in
   * here do sort of depend on the terminal settings on the
   * output driver, but expect xterm-256 codes to work as
   * intended.
   */
  int backgroundColour;

  /**\brief Text attributes
   *
   * \todo This really needs to be fleshed out properly.
   */
  int attributes;

  /**\brief Compare two cells
   *
   * The frontend driver will need some way to figure out if two
   * cells contain the same data, so as to figure out if a cell
   * has been modified and needs to be modified. This operator
   * provides that functionality.
   *
   * \param[in] b The cell to compare this one with.
   *
   * \returns 'true' if the two cells don't contain the same
   *          data.
   */
  constexpr bool operator!=(const cell &b) const {
    return (content != b.content) || (foregroundColour != b.foregroundColour) ||
           (backgroundColour != b.backgroundColour) ||
           (attributes != b.attributes);
  }
};

/**\brief Screen buffer
 *
 * Contains the data stored in a single screen buffer - i.e. the
 * current contents of a terminal or the target contents.
 *
 * This buffer is derived from a nested std::vector using the 'cell'
 * template for individual cells. You should not modify this vector
 * manually, unless you really know what you're doing. Note that the
 * indices into this nested std::vector are reversed with respect to
 * the coordinate and dimension tuples used in other places.
 *
 * While odd, this was done on purpose - because typically, if you
 * iterate over the whole screen, you'd expect things on the same line
 * to be 'in sequence'; or at least you would if you read from left to
 * right, then top to bottom. However, it is customary to specify
 * terminal sizes as WidthXHeight, or rather ColumnsXLines, which
 * creates a bit of ambiguity in this case.
 *
 * \tparam T Data type to hold a single character of data. The default
 *         is 'long', which is easily able to handle all of the unicode
 *         code points.
 */
template <typename T = long>
class screen : public std::vector<std::vector<cell<T> > > {
public:
  /**\brief Alias for the parent class
   *
   * This class extends a nested std::vector, so this typedef
   * provides a convenient handle to this base class.
   */
  typedef std::vector<std::vector<cell<T> > > parent;

  /**\brief Cell type
   *
   * Helper typedef for a single cell in the screen buffer.
   */
  typedef cell<T> cell;

  using parent::resize;

  /**\brief Construct with nested vectors
   *
   * Constructs a new screen buffer by copying the contents of
   * an instance of the parent class.
   *
   * \param[in] p The instance of the parent class to copy.
   */
  screen(const parent &p) : parent(p) {}

  /**\brief Construct empty buffer
   *
   * Construct an empty buffer given the width and height of the
   * buffer to create.
   *
   * If you omit the width and height of the buffer, then it is
   * assumed that you meant to create a typical 80x24 buffer.
   *
   * \param[in] width  Width of the new buffer.
   * \param[in] height Height of the new buffer.
   */
  screen(const int &width = 80, const int &height = 24) {
    resize(width, height);
  }

  /**\brief Resize screen buffer
   *
   * Modifies the current screen buffer so that it now fits a
   * terminal with the given number of columns and lines.
   *
   * Resizing the buffer has the same semantics as std::resize:
   * obsolete cells will be destroyed and new ones will be
   * created with their default constructor.
   *
   * \param[in] columns Number of columns to resize the buffer to
   * \param[in] lines   Number of lines to resize the buffer to
   *
   * \returns 'true' on success, 'false' otherwise. This method
   *          currently has no way to fail, so it'll always
   *          return 'true'.
   */
  bool resize(const std::size_t &columns, const std::size_t &lines) {
    resize(lines);

    for (std::vector<cell> &column : *this) {
      column.resize(columns);
    }

    return true;
  }

  /**\brief Resize screen buffer
   *
   * Resizes the screen buffer with an array of length 2, in the
   * same way that calling resize(size[0], size[1]) would. This
   * overload is purely for convenience, as this is the return
   * type of the terminal<T>::getOSDimensions() and the size()
   * methods.
   *
   * \param[in] size The size to resize the buffer to.
   *
   * \returns Same value as resize(size[0], size[1]) would have.
   */
  bool resize(const std::array<std::size_t, 2> &size) {
    return resize(size[0], size[1]);
  }

  /**\brief Query buffer size
   *
   * Creates a std::array with the size of the buffer, as
   * obtained by querying the size of the parent vector.
   *
   * \returns An array of the form {{ columns, lines }}.
   */
  constexpr std::array<std::size_t, 2> size(void) const {
    return { { this->parent::size() > 0 ? (*this)[0].size() : 0,
               this->parent::size() } };
  }
};

/**\brief Terminal base class
 *
 * This class template encapsulates the current state of a terminal as
 * well as the intended target state of a terminal. This information
 * can then be used by the individual terminal frontents to produce an
 * efficient set of commands to get the terminal's current state to the
 * intended target state.
 *
 * Additionally, this template class provides some basic,
 * terminal-independent, OS-level queries and setup functionality. This
 * includes querying the size of virtual terminals from the OS and
 * encapsulating the input and output streams that provide a connection
 * to the terminal for commands to be sent.
 *
 * \tparam T Data type to hold a single character of data. The default
 *         is 'long', which is easily able to handle all of the unicode
 *         code points.
 */
template <typename T = long> class terminal {
public:
    /**\brief Construct with I/O streams and set raw mode
     *
     * Constructs an instance of the terminal class, optionally
     * with input and output streams that connect to the terminal.
     * It is assumed that terminals are controlled through two
     * character streams.
     *
     * The optional third parameter, set to 'true' by default, will
     * set the terminal connected to stdin to non-canonical and
     * non-echo mode. Non-canonical mode means that input is not
     * processed line-by-line, but rather character-by-character,
     * which in turn means that the application can read from stdin
     * without the user having to press "enter" to release the
     * input. Non-echo mode means that the input is not echoed back
     * to the terminal.
     *
     * \param[out] pInput  Input stream for the terminal, e.g.
     *                     std::cin
     * \param[out] pOutput Output stream for the terminal, e.g.
     *                     std::cout
     * \param[in]  doSetup Whether to disable echo and canonical
     *                     mode on the terminal connected to stdin
     *
     * \note If you intend to pass in your own streams, do note
     *       the streams are stored in the class as references (the
     *       input and output members), meaning that you must make
     *       sure these two streams are not destroyed before this
     *       object is.
     */
  terminal(std::istream &pInput = std::cin, std::ostream &pOutput = std::cout,
           const bool &doSetup = true)
      :
#if defined(TCSANOW)
        didSetup(doSetup),
#endif
        input(pInput), output(pOutput), cursor({
    (std::size_t) - 1, (std::size_t) - 1
  }) {
#if defined(TCSANOW)
    if (doSetup) {
      std::ios_base::sync_with_stdio(false);

      termios terminalFlags;
      tcgetattr(0, &originalTerminalFlags);
      terminalFlags = originalTerminalFlags;
      terminalFlags.c_lflag &= ~(ICANON | ECHO);
      tcsetattr(0, TCSANOW, &terminalFlags);
      didSetup = true;
    }
#endif
  }

#if defined(TCSANOW)
  /**\brief Destructor
   *
   * Only defined when the global macro TCSANOW is defined, i.e.
   * if it would have been possible for the constructor to grab
   * the current terminal settings and then modify them.
   *
   * When called, this destructor will reset the current terminal
   * attributes to what they were before the constructor ran, but
   * only if the constructor had the doSetup parameter set.
   */
  ~terminal(void) {
    if (didSetup) {
      tcsetattr(0, TCSANOW, &originalTerminalFlags);
    }
  }
#endif

  /**\brief Cell type
   *
   * Helper typedef for an individual cell in the screen object.
   */
  typedef typename screen<T>::cell cell;

  /**\brief Input queue
   *
   * Data coming in on the input stream is buffered here, so that
   * things like control sequences can be filtered out by the
   * frontend driver.
   */
  std::vector<T> queue;

  /**\brief Current state of the terminal
   *
   * Contains the current state of the terminal, or a lot of
   * zeroes if the object has just been created. This buffer is
   * kept around so that terminal frontend classes can decide
   * which parts of the screen need to be updated.
   */
  screen<T> current;

  /**\brief Target state of the terminal
   *
   * Contains a screen buffer describing what the programmer
   * wants the terminal to look like. To update the screen
   * contents, you'd modify this buffer in one way or another,
   * then tell your frontend class to update the screen contents.
   */
  screen<T> target;

  /**\brief Terminal input stream
   *
   * This stream is used to read data from the terminal, such as
   * key presses or terminal control codes.
   */
  std::istream &input;

  /**\brief Terminal output stream
   *
   * Terminal output commands are written to this stream; these
   * commands range from plain old characters to escape codes
   * that modify the appearance of the characters following them.
   */
  std::ostream &output;

  /**\brief Resize screen buffers
   *
   * Calls the screen::resize() method of both the current and
   * the target screen buffer so that both of them share the same
   * new size afterwards.
   *
   * Resizing the buffer has the same semantics as std::resize:
   * obsolete cells will be destroyed and new ones will be
   * created with their default constructor.
   *
   * \param[in] columns Number of columns to resize the buffers to
   * \param[in] lines   Number of lines to resize the buffers to
   *
   * \returns 'true' if both buffers were successfully resized,
   *          'false' otherwise.
   */
  bool resize(std::size_t columns, std::size_t lines) {
    return current.resize(columns, lines) && target.resize(columns, lines);
  }

  /**\brief Resize screen buffers
   *
   * Calls the screen::resize() method of both the current and
   * the target screen buffer so that both of them share the same
   * new size afterwards.
   *
   * Resizing the buffer has the same semantics as std::resize:
   * obsolete cells will be destroyed and new ones will be
   * created with their default constructor.
   *
   * Unlike the two-parameter resize() method, this one takes the
   * same type of argument that getOSDimensions() produces as its
   * return value: a maybe<> of the typical 2-element size array.
   * If the maybe contains nothing, then the size is not changed,
   * otherwise the array is taken as a (columns, lines) vector.
   *
   * \param[in] size Target size of the buffers; buffers aren't
   *                 resized if this maybe contains nothing.
   *
   * \returns 'true' if both buffers were successfully resized,
   *          'false' otherwise.
   */
  bool resize(const maybe<std::array<std::size_t, 2> > &size) {
    return size && current.resize(std::array<std::size_t, 2>(size)) &&
           target.resize(std::array<std::size_t, 2>(size));
  }

  /**\brief Query buffer size
   *
   * Creates a std::array with the size of the current buffer, as
   * obtained by usnig screen::size().
   *
   * \returns An array of the form {{ columns, lines }}.
   */
  constexpr std::array<std::size_t, 2> size(void) const {
    return current.size();
  }

  /**\brief Query OS screen dimensions
   *
   * Uses an ioctl to query the size of the terminal connected to
   * stdin, then returns that size as an array. If the size could
   * not be determined then this method returns 'nothing'.
   *
   * Querying the size of the terminal requires the TIOCGWINSZ
   * ioctl to be available.
   *
   * \returns The size of the screen or 'nothing'.
   */
  static maybe<std::array<std::size_t, 2> > getOSDimensions(void) {
#if defined(TIOCGWINSZ)
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return maybe<std::array<std::size_t, 2> >(std::array<std::size_t, 2>({
      { w.ws_col, w.ws_row }
    }));
#else
    return maybe<std::array<std::size_t, 2> >();
#endif
  }

  /**\brief Read from terminal
   *
   * Tries to read a single character from the terminal input
   * stream. If that stream happens to be in an invalid state
   * then nothing will be read from the terminal.
   *
   * If a character was indeed read in then this character is
   * stored in the queue variable.
   *
   * \returns The character that was read or 'nothing'.
   */
  maybe<T> read(void) {
    if (!input) {
      return maybe<T>();
    } else {
      char n;
      if (input.get(n)) {
        queue.push_back(T(n));
        return maybe<T>(T(n));
      } else {
        return maybe<T>();
      }
    }
  }

  /**\brief Write string to target buffer
   *
   * Will write the given text at the coordinates specified by
   * (column, line) with the specified colours to the target
   * buffer.
   *
   * If you omit either the column or the line then this method
   * will write to the internal cursor position. If you omit the
   * colours then this method will not modify the colours at the
   * specified coordinates.
   *
   * This function will break to the next line at the end of a
   * line. If that line happens to be the last one on the screen,
   * then the method will wrap around to (0,0).
   *
   * The internal cursor position is NOT updated after the write
   * operation.
   *
   * \tparam S Character type of the string you pass to this
   *           function; separate from T because you might well
   *           try to write a regular string to a terminal<long>.
   *
   * \param[in] text   The string to write.
   * \param[in] column Column coordinate of the target position.
   * \param[in] line   Line coordinate of the target position.
   * \param[in] foregroundColour The foreground colour to use.
   * \param[in] backgroundColour The background colour to use.
   *
   * \returns 'true' if the operation succeeded, 'false'
   *          otherwise. Since there is no way for this function
   *          to fail right now, it'll always return 'true'.
   */
  template <typename S>
  bool write(const std::basic_string<S> &text,
             const maybe<std::size_t> &column = maybe<std::size_t>(),
             const maybe<std::size_t> &line = maybe<std::size_t>(),
             const maybe<int> &foregroundColour = maybe<int>(),
             const maybe<int> &backgroundColour = maybe<int>()) {
    std::size_t c = column ? std::size_t(column) : cursor[0];
    std::size_t l = line ? std::size_t(line) : cursor[1];
    std::array<std::size_t, 2> si = size();

    for (const S &character : text) {
      if (c >= si[0]) {
        c = 0;
        l++;
      }
      if (l >= si[1]) {
        l = 0;
      }
      target[l][c].content = T(character);
      if (foregroundColour) {
        target[l][c].foregroundColour = foregroundColour;
      }
      if (backgroundColour) {
        target[l][c].backgroundColour = backgroundColour;
      }
      c++;
    }

    return true;
  }

protected:
  /**\brief Current cursor position
   *
   * This is the current device cursor position. Should be kept
   * up to date by any terminal frontends that make use of this
   * information - e.g. by the vt100 class.
   */
  std::array<std::size_t, 2> cursor;

private:
#if defined(TCSANOW)
  /**\brief Has the constructor set up the terminal?
   *
   * Set to 'true' if the constructor has modified terminal
   * settings which it should reverse in the destructor.
   */
  bool didSetup;

  /**\brief Original terminal settings
   *
   * Contains the original terminal flags before the class was
   * instantiated. These settings are restored in the destructor
   * iff 'didSetup' is set to 'true'.
   */
  termios originalTerminalFlags;
#endif

};
};
};

#endif
