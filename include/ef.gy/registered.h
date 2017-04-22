/* Object registration list with global default instance.
 *
 * Used for registering global things, like command line argument specs, test
 * cases or network server servlets.
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

#if !defined(EF_GY_REGISTERED_H)
#define EF_GY_REGISTERED_H

#include <set>

namespace efgy {
/* Object pointer list with default instance.
 * @item Item class.
 *
 * This is a wrapper for std::set<item*>. The only reason to use this over the
 * original set type, is that this version provides a global "default" instance.
 *
 * This is only useful for items that primarily have some global meaning, and
 * where the individual components don't know where exactly the registry is.
 * Examples of this would be command line argument specifications (see cli.h),
 * test cases (see test-case.h), or, say, network server servlets.
 */
template <class item>
class registered {
 public:
  /* Return global default instance for this item list.
   *
   * Thanks to how template instantiation and static function members work, this
   * provides one globally unique set per item type.
   *
   * @returns Default instance for the class's item type.
   */
  static registered &common(void) {
    static registered reg;
    return reg;
  }

  /* Register item.
   * @it The item to add.
   *
   * Adds 'it' to the set of items this object keeps track of. Only the item's
   * pointer is added, therefore if it should go out of scope you must first
   * unregister it with the 'remove' method.
   *
   * Adding 'it' multiple times has no effect, and would still only require
   * removing it once.
   */
  void add(item &it) { items.insert(&it); }

  /* Unregister item.
   * @it The item to remove.
   *
   * The opposite of 'add'. Like with the 'add' function, only the item's
   * pointer is used.
   */
  void remove(item &it) { items.erase(&it); }

  /* Get number of items in the set.
   *
   * Returns the number of items this class is keeping track of. This is a
   * wrapper around items.size(), as that is deliberately inaccessible.
   *
   * @return The number if items in the set. Can't be negative.
   */
  std::size_t size(void) const { return items.size(); }

  /* Get iterator to beginning of set.
   *
   * This, along with the 'end' function, allows C++11-style for loops to work,
   * without exposing 'items'.
   *
   * @return The iterator to the beginning of 'items'.
   */
  typename std::set<item *>::iterator begin(void) { return items.begin(); }

  /* Get iterator to end of set.
   *
   * This, along with the 'begin' function, allows C++11-style for loops to
   * work, without exposing 'items'.
   *
   * @return The iterator to the end of 'items'.
   */
  typename std::set<item *>::iterator end(void) { return items.end(); }

 protected:
  /* The list of items this object keeps track of.
   *
   * This is a set of pointers, because we can't guarantee that the items have
   * less-than comparators defined for them. And we don't actually want to
   * create our own copies.
   */
  std::set<item *> items;
};
}

#endif
