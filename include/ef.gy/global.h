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

#include <set>

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

/* Object beacon.
 * @type The type of the object to remember.
 *
 * This class allows registering an object for as long as it exists, and to then
 * automatically remove the reference when it no longer does.
 */
template <class type>
class beacon {
 public:
  /* Set type.
   *
   * This type is here to help you know what you need to pass into the
   * constructor.
   */
  using set = std::set<type *>;

  /* Construct with reference.
   * @pReference What to keep track of.
   * @pRoot Where to keep track of that; optional, defaults to a global set.
   *
   * Allows constructing a beacon by using a reference to whatever needs to be
   * tracked. By forcing construction with a reference, the object we do keep
   * track of must already be initialised by the time the beacon is constructed,
   * which helps in avoiding exposing uninitialised objets.
   *
   * Note: multiple objects with the same address and of the same type will not
   * work as expected. Or, they will, until the destructor comes into play. so
   * you'll be fine for global, static things, but you won't be fine for local
   * objects in the sense that they might get removed from the set earlier than
   * they should.
   */
  beacon(type &pReference, set &pRoot = global<set>())
      : reference(pReference), root(pRoot) {
    root.insert(&reference);
  }

  /* Unregister the reference.
   *
   * This destructor removes the reference from the set of things to keep track
   * of. The destructor is the perfect place for this, since we want this to be
   * bound to the lifecycle of the beacon.
   *
   * Also, the order of destructors is defined to be the reverse of the
   * constructors, so if the reference and the beacon were in the same scope
   * then the beacon would have to be destroyed first, since we require a
   * reference to construct the beacon.
   */
  ~beacon(void) { root.erase(&reference); }

 protected:
  /* What we keep track of.
   *
   * This is a reference to the thing we keep track of, as outlined in the
   * constructor and destructor docs.
   */
  type &reference;

  /* Where we keep track of the reference.
   *
   * A reference to the set of things that are being kept track of by other
   * beacons.
   */
  set &root;
};

/* Type alias for registered sets.
 * @type What was registered.
 *
 * This is a minor convenience alias to get the set type right when dealing with
 * things that use a beacon.
 */
template <typename type>
using beacons = typename beacon<type>::set;
}

#endif
