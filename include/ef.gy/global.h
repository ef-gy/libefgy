/* Helper for creating and finding a single global object of a type.
 *
 * This is kind of "half" of a singleton - there's no reason to prevent multiple
 * instances of classes, but having a lazily initialised global instance solves
 * some problems.
 *
 * See also:
 * * Project Documentation: https://ef.gy/documentation/libefgy
 * * Project Source Code: https://github.com/ef-gy/libefgy
 * * Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 *
 * @copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 */

#if !defined(EF_GY_GLOBAL_H)
#define EF_GY_GLOBAL_H

namespace efgy {
/* Lazily-initialised global object.
 * @object The type of the object to initialise. Must be default-constructible.
 *
 * Sometimes it's neat to have a default instance of, say, a list of things that
 * your binary supports, and that multiple pieces of the code can add to.
 * A typical example might be servlets in a network server, or command line
 * options, or test cases scattered throughout a library.
 *
 * @return A global default version of the type you used.
 */
template <class type>
static type &global(void) {
  static type obj;
  return obj;
}
}

#endif
