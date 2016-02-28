/**\file
 * \ingroup example-programmes
 * \brief Matrix-esque terminal animation
 *
 * This is a terminal programme that uses libefgy's vt100 code to render a text
 * version of the matrix 'scrolling streams of text' animation. It's really
 * fairly simple but also kinda nice to see how the vt100 output is performing.
 *
 * \image html matrix.png "Screenshot of the programme running in Terminal.app"
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

#include <ef.gy/vt100.h>
#include <chrono>
#include <csignal>
#include <cmath>
#include <sched.h>
#include <random>

using namespace efgy;
using namespace std::chrono;

/**\brief Data and functions related to the matrix demo
 *
 * Contains global variables and classes used by the 'matrix' demo animation.
 * This is a separate namespace to keep things all neat, clean and tidy.
 */
namespace thematrix {
/**\brief VT100 output buffer
 *
 * Encapsulates stdio to automatically generate the terminal escape
 * sequences to write things at specific positions with specific colours.
 */
terminal::vt100<> output;

/**\brief Random number generator
 *
 * A seeded instance of the mersenne twister RNG; used to position the
 * streams and to add and remove glyphs randomly.
 */
std::mt19937 rng(1337);

/**\brief Current time
 *
 * The point in time when processing of the current streams started; used
 * to determine when streams should be updated.
 */
system_clock::time_point now;

/**\brief A stream of data
 *
 * A 'stream' is what I dubbed the individual columns of text in the
 * animation. These are generated randomly and they mutate randomly as
 * well.
 */
class stream {
public:
  /**\brief A single glyph in a stream
   *
   * To create the animation, each of the glyphs in the output needs
   * to be tagges with the time it was created, in order to know
   * which colour to draw the glyph in.
   */
  class cell {
  public:
    /**\brief Construct with glyph
     *
     * Constructs an instance of the class using the given
     * character and the current system time.
     *
     * \param[in] pCharacter A unicode code point.
     */
    cell(const unsigned long &pCharacter)
        : character(pCharacter), created(now) {}

    /**\brief The glyph
     *
     * This is the glyph to output; must be a valid unicode
     * glyph as the vt100 code will expect one to send to the
     * terminal.
     */
    unsigned long character;

    /**\brief When was this object created?
     *
     * Contains the time point of this object's creation; used
     * to determine the colour of the glyph in the output;
     */
    system_clock::time_point created;
  };

  /**\brief Construct with position
   *
   * Initialises an instance of the class given the line and column
   * of where the stream should appear.
   *
   * \param[in] pLine   The line at which to create the stream; must
   *                    be less than output.size()[1].
   * \param[in] pColumn The column at which to create the stream;
   *                    must be less than output.size()[0].
   */
  stream(const std::size_t &pLine, const std::size_t &pColumn)
      : line(pLine), column(pColumn), last(now), doDelete(false) {}

  /**\brief Line component of stream position
   *
   * This is the line at which the stream is rendered to the
   * terminal.
   */
  std::size_t line;

  /**\brief Column component of stream position
   *
   * This is the column at which the stream is rendered to the
   * terminal.
   */
  std::size_t column;

  /**\brief The stream contents to render
   *
   * A vector of glyphs and the time at which they were inserted into
   * the stream; this vector is placed vertically at the coordinates
   * given by line and column. Recent glyphs are rendered in white,
   * older ones in green.
   */
  std::vector<cell> data;

  /**\brief Time of last update
   *
   * Records the last time a glyph was inserted into or removed from
   * the stream; used to determine when to add new glyphs.
   */
  system_clock::time_point last;

  /**\brief Should this stream be deleted?
   *
   * Set to true when the upper part of the stream reaches the bottom
   * of the screen; the stream should then be deleted in the main
   * loop because it won't produce any output on the screen when this
   * happens.
   */
  bool doDelete;

  /**\brief Update and render stream
   *
   * Render the current contents of the stream to the screen; if it
   * so happens that enough time has passed since the last time this
   * method was called then this function will also randomly add or
   * remove glyphs, or move the stream further down.
   *
   * \returns 'true' if the update went smoothly; 'false' otherwise.
   *          There is currently no way this function can fail, so
   *          at the moment it will always return 'true'.
   */
  bool update(void) {
    if ((now - last) > std::chrono::milliseconds(10)) {
      last = now;
      switch (rng() % 3) {
      case 0:
        if (data.size() > 0) {
          data.erase(data.begin() + (rng() % data.size()));
          break;
        }
      default:
        data.push_back(cell(rng() % (1 << 7)));
      }
    }
    std::array<std::size_t, 2> s = output.size();
    if (data.size() > (s[1] / 2)) {
      if (line < s[1]) {
        if (rng() % 5 == 0) {
          output.target[line][column].content = ' ';
          line++;
        }
      } else {
        doDelete = true;
      }
      data.erase(data.begin());
    }
    int i = line;
    for (cell &d : data) {
      if (i >= s[1]) {
        break;
      }
      output.target[i][column].content = d.character;
      output.target[i][column].foregroundColour =
          (now - d.created) > std::chrono::milliseconds(120) ? 2 : 7;
      output.target[i][column].backgroundColour = 0;
      i++;
    }
    return true;
  }
};

/**\brief SIGINT handler
 *
 * Signal handler that calls exit() properly; the SIGINT handler is
 * replaced by this function. While SIGINT would ordinarily terminate the
 * programme anyway, it doesn't do so with a proper call to exit(), meaning
 * that destructors will not be called, further meaning that the terminal
 * can't be reset to a proper state by the terminal handling code, because
 * not using exit() properly means destructors won't run.
 */
void handle_interrupt(int signal) { exit(0); }

/**\brief Green-tinted post processing function
 *
 * A simple post-processing function for the vt100 code, which makes sure
 * that any output cell is always either white or green. This makes it
 * easier for the optimiser as it won't have to switch colours quite as
 * often as it otherwise might.
 */
terminal::cell<long> postProcess(const terminal::terminal<long> &t,
                                 const std::size_t &l, const std::size_t &c) {
  terminal::cell<long> rv = t.target[l][c];
  rv.content = rv.content == 0 ? ' ' : rv.content;
  rv.foregroundColour = rv.foregroundColour == 7 ? 7 : 2;
  rv.backgroundColour = 0;
  return rv;
}

/**\brief Alternative post processing function
 *
 * Adds some curviness to the output by modifying the input coordinates a
 * bit before creating the output. Kinda looks funky, but not really
 * matrix-y. You'll have to modify the main() function yourself if you want
 * to see this in action.
 */
terminal::cell<long> postProcessPolar(const terminal::terminal<long> &t,
                                      const std::size_t &pl,
                                      const std::size_t &pc) {
  double l = pl;
  double c = pc;
  std::array<std::size_t, 2> s = t.size();
  double hl = s[1] / 2;
  double hc = s[0] / 2;
  double loff = l - hl;
  double coff = c - hc;
  double r2 = loff * loff + coff * coff;
  double r = std::sqrt(r2);
  l = hl + loff + std::sin(r) * 1.0;
  c = hc + coff + std::cos(r) * 1.0;
  const std::size_t tl = (std::size_t)l < s[1] ? (std::size_t)l : (s[1] - 1);
  const std::size_t tc = (std::size_t)c < s[0] ? (std::size_t)c : (s[0] - 1);
  terminal::cell<long> rv = t.target[tl][tc];
  terminal::cell<long> cv = t.current[tl][tc];
  rv.content =
      rv.content == 0 ? (cv.content == 0 ? ' ' : cv.content) : rv.content;
  rv.foregroundColour = rv.foregroundColour == 7 ? 7 : 2;
  rv.backgroundColour = 0;
  return rv;
}
};

using namespace thematrix;

/**\brief Matrix demo main function
 *
 * Entry point for the matrix programme; resizes the output buffer to encompass
 * the whole terminal, sets up a SIGINT handler and then maintains a vector of
 * thematrix::stream objects at random positions.
 *
 * Use CTRL+C to terminate the programme.
 *
 * \note Command line arguments and the programme environment are ignored.
 *
 * \returns 0 for 'success', but this should never be reached as the SIGINT
 *          handler calls exit(), which is the only way to break out of the main
 *          loop.
 */
int main(int, char **) {
  output.resize(output.getOSDimensions());
  std::array<std::size_t, 2> s = output.size();

  std::size_t i;

  std::vector<stream> streams;

  std::signal(SIGINT, handle_interrupt);

  while (1) {
    i++;
    now = system_clock::now();

    if ((streams.size() <= 100) && (i % 50 == 0)) {
      std::size_t l = rng() % (s[1] / 3);
      std::size_t c = rng() % s[0];
      streams.push_back(stream(l, c));
    }

    for (stream &s : streams) {
      s.update();
    }

    for (unsigned int i = 0; i < streams.size(); i++) {
      if (streams[i].doDelete) {
        streams.erase(streams.begin() + i);
      }
    }

    if (output.flush(postProcess) == 0) {
      sched_yield();
    }
  }

  while ((i = output.flush(postProcess)) > 0)
    ;

  return 0;
}
