/* S-expression handling.
 *
 * Defines a template to handle S-expression parsing.
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

#if !defined(EF_GY_SEXP_H)
#define EF_GY_SEXP_H

#include <optional>
#include <ostream>
#include <string>

namespace efgy {
/* A cons expression
 * @T1 type of first element
 * @T2 type of second element
 *
 * Represents a cons expression which consists of two other S-expressions. A
 * cons can either combine two nontrivial S-expressions, or combine one
 * S-expression with the special S-expression NIL, which is represented through
 * the 'nothing' value of the 'optional' class here.
 */
template <typename T1, typename T2>
class cons {
 public:
  /* Constructor for a cons with two elements
   *
   * Constructs a cons of the form (x.y) where x and y are both s-expressions
   * not equal to NIL.
   */
  cons(T1 car_, T2 cdr_) : car(car_), cdr(std::optional<T2>(cdr_)) {}

  /* Constructor for a cons with one element.
   *
   * Constructs a cons of the form (x.nil).
   */
  cons(T1 car_) : car(car_), cdr(std::optional<T2>()) {}

  /* First element of the cons. */
  const T1 car;

  /* Second element of the cons
   *
   * Second element of the cons, represented as a std::optional: it can either
   * contain a value of type T2, or 'nothing' to represent the special
   * S-expression NIL (which is used to indicate the end of a list, among other
   * things)
   */
  const std::optional<T2> cdr;
};

/* Stream insertion operator
 *
 * Displays the cons expression in a std::ostream. The expression is displayed
 * as (car . cdr).
 */
template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &str, cons<T1, T2> &s) {
  if (s.cdr) {
    str << "(" << s.car << " . " << *s.cdr << ")";
  } else {
    str << "(" << s.car << " . NIL)";
  }
  return str;
}

/* An atomic S-expression
 * @T type of atomic expression
 *
 * Represents an atomic S-expression, which is a value of some type T. T ought
 * to be a truly atomic type, not a cons type.
 *
 */
template <typename T>
class atom {
 public:
  /* Constructs an atomic S-expression.
   *
   * Constructs an atomic S-expression.
   */
  atom(T data_) : data(data_) {}

  /* \brief The atomic value
   *
   * The atomic value of the S-expression.
   */
  const T data;
};

/* Stream insertion operator
 *
 * Displays the atomic value, as defined in the stream insertion operator of
 * type T.
 */
template <typename T>
std::ostream &operator<<(std::ostream &str, atom<T> a) {
  str << a.data;
  return str;
}
}
#endif
