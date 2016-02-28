/**\file
 * \brief JSON renderer
 *
 * The templates in this header file allow tagging output streams as JSON output
 * streams so they can be written to like any other C++ stream but producing
 * JSON instead of plain text.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_RENDER_JSON_H)
#define EF_GY_RENDER_JSON_H

#include <ef.gy/render.h>
#include <ef.gy/stream-json.h>

namespace efgy {
namespace render {};
};

#endif
