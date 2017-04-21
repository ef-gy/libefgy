/**\file
 * \brief CSS stream tag
 *
 * Contains the CSS stream tag that is used to differentiate between output in
 * plain text and output in CSS.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_STREAM_CSS_H)
#define EF_GY_STREAM_CSS_H

#include <ostream>
#include <string>

namespace efgy {
namespace css {
/**\brief std::ostream CSS tag
 *
 * Used to distinguish between a plain std::ostream, and one where the
 * output should be in CSS format.
 *
 * \tparam C Character type for the basic_ostream reference.
 */
template <typename C>
class ostream {
 public:
  /**\brief Construct with stream reference
   *
   * Initialises a new ostream CSS tag instance.
   *
   * \param[out] pStream The stream to write to.
   */
  ostream(std::basic_ostream<C> &pStream) : stream(pStream) {}

  /**\brief Output stream reference
   *
   * This is the stream where the output is written to.
   */
  std::basic_ostream<C> &stream;
};

/**\brief CSS tag
 *
 * Write this to an ostream to turn it into an ocssstream. Like this:
 *
 * \code{.cpp}
 * cout << css::tag();
 * \encode
 */
class tag {};

/**\brief Convert std::ostream to CSS
 *
 * Converts the given stream to a CSS stream so that write operations
 * after that will produce CSS instead of plain text.
 *
 * \tparam C Character type for the basic_ostream reference.
 *
 * \param[out] stream The stream to write to.
 */
template <typename C>
constexpr inline ostream<C> operator<<(std::basic_ostream<C> &stream,
                                       const tag &) {
  return ostream<C>(stream);
}
};
};

#endif
