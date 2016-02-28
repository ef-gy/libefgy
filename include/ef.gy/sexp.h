/**\file
 * \brief S-expression parser
 *
 * Defines a template to handle S-expression parsing.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_SEXP_H)
#define EF_GY_SEXP_H

#include "maybe.h"

namespace efgy {
/* \brief Abstract base class for s-expressions
*
*  Abstract base class for s-expressions
*  In this implementation, you should instantiate
*  the concrete derived  classes, efgy::cons and efgy::atom.
*/
class sexp {
public:
};

/** \brief A cons expression
    *
    * Represents a cons expression which consists of two
    * other S-expressions. A cons can either combine two
    * nontrivial S-expressions, or combine one S-expression
    * with the special S-expression NIL, which is represented
    * through the 'nothing' value of the 'maybe' class here.
        *
        * \tparam T1 type of first element
        * \tparam T2 type of second element
    *
    * \sa \ref maybe
    */

template <typename T1, typename T2> class cons : public sexp {
public:
  /*\brief Constructor for a cons with two elements

            Constructs a cons of the form (x.y) where x and y
                are both s-expressions not equal to NIL*/
  cons(T1 car_, T2 cdr_) : car(car_), cdr(efgy::maybe<T2>(cdr_)) {}

  /*\brief Constructor for a cons with one element

         Constructs a cons of the form (x.nil)*/
  cons(T1 car_) : car(car_), cdr(efgy::maybe<T2>()) {}

  /* \brief First element of the cons
      *
      * First element of the cons
      */
  const T1 car;

  /* \brief Second element of the cons
   *
   * Second element of the cons, represented as a maybe:
   * it can either contain a value of type T2,
   * or 'nothing' to represent the special S-expression
   * NIL (which is used to indicate the end of a list,
   * among other things)
   */
  const efgy::maybe<T2> cdr;
};

/* \brief Stream insertion operator
         *
         * Displays the cons expression in a std::ostream.
         * The expression is displayed as (car . cdr).
         */
template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &str, cons<T1, T2> &s) {
  str << "(" << s.car << " . " << s.cdr << ")";
  return str;
}

/** \brief An atomic S-expression
*
* Represents an atomic S-expression, which is a value
* of some type T. T ought to be a truly atomic type,
* not a cons type.
*
* \tparam T type of atomic expression
*/
template <typename T> class atom : public sexp {
public:
  /* \brief Constructs an atomic S-expression.
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

/* \brief Stream insertion operator
         *
         * Displays the atomic value, as defined in the
         * stream insertion operator of type T.*/
template <typename T> std::ostream &operator<<(std::ostream &str, atom<T> a) {
  str << a.data;
  return str;
}

class parser {
public:
  // does nothing for now
  friend std::istream &operator>>(std::istream &is, parser const &p) {
    return is;
  }

private:
  int openParenCount;
  int closeParenCount;
};
}
#endif
