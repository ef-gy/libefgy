/**\file
 * \brief RGB colour vectors
 *
 * Contains colour vectors in the (s)RGB colour space. Currently only 3D and 4D
 * vectors are supported directly, which refer to RGB and RGBA vectors,
 * respectively.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_COLOUR_SPACE_RGB_H)
#define EF_GY_COLOUR_SPACE_RGB_H

#include <ef.gy/vector.h>

namespace efgy {
namespace math {
namespace format {
/**\brief RGB colour format tag
 *
 * This tag indicates that a vector is of the RGB(A) colour space;
 * since all vectors are tagged with a vector format tag class, this
 * makes it possible for vectors of different colour spaces to be
 * converted properly.
 */
class RGB {
 public:
  /**\copydoc cartesian::id */
  static const char *id(void) { return "RGB"; }
};
}  // namespace format

template <typename Q>
class vector<Q, 3, format::RGB> : public std::array<Q, 3> {
 public:
  vector(const std::array<Q, 3> &v = {{}}, const format::RGB & = format::RGB())
      : std::array<Q, 3>(v),
        red((*this)[0]),
        green((*this)[1]),
        blue((*this)[2]) {}
  vector(const Q &pRed, const Q &pGreen, const Q &pBlue)
      : std::array<Q, 3>({pRed, pGreen, pBlue}),
        red((*this)[0]),
        green((*this)[1]),
        blue((*this)[2]) {}
  vector(const vector &v)
      : std::array<Q, 3>({v.red, v.green, v.blue}),
        red((*this)[0]),
        green((*this)[1]),
        blue((*this)[2]) {}

  vector &operator=(const vector &v) {
    red = v.red;
    green = v.green;
    blue = v.blue;
    return *this;
  }

  Q &red;
  Q &green;
  Q &blue;
};

template <typename Q>
class vector<Q, 4, format::RGB> : public std::array<Q, 4> {
 public:
  vector(const std::array<Q, 4> &v = {{}}, const format::RGB & = format::RGB())
      : std::array<Q, 4>(v),
        red((*this)[0]),
        green((*this)[1]),
        blue((*this)[2]),
        alpha((*this)[3]) {}
  vector(const Q &pRed, const Q &pGreen, const Q &pBlue, const Q &pAlpha = Q(1))
      : std::array<Q, 4>({pRed, pGreen, pBlue, pAlpha}),
        red((*this)[0]),
        green((*this)[1]),
        blue((*this)[2]),
        alpha((*this)[3]) {}
  vector(const vector &v)
      : std::array<Q, 4>({v.red, v.green, v.blue, v.alpha}),
        red((*this)[0]),
        green((*this)[1]),
        blue((*this)[2]),
        alpha((*this)[3]) {}

  vector &operator=(const vector &v) {
    red = v.red;
    green = v.green;
    blue = v.blue;
    alpha = v.alpha;
    return *this;
  }

  Q &red;
  Q &green;
  Q &blue;
  Q &alpha;
};
};  // namespace math
};  // namespace efgy

#endif
