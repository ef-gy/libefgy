/**\file
 * \brief efgy::render namespace
 *
 * This file defines the efgy::render namespace for classes and functions that
 * are used to visualise other objects.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_RENDER_H)
#define EF_GY_RENDER_H

namespace efgy {
/**\brief Data visualisation
 *
 * Functions and classes in this namespace are used to present data
 * visually in various formats. This includes things such as turning tuples
 * of vectors into polygons in an SVG, turning calculated colour values
 * into plain XML to post-process in other programmes or even rendering
 * complex higher dimensional models in OpenGL or WebGL.
 */
namespace render {};
};  // namespace efgy

#endif
