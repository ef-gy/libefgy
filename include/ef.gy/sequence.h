/* Sequences
 *
 * Contains supporting types for infinite sequences.
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

#if !defined(EF_GY_SEQUENCES_H)
#define EF_GY_SEQUENCES_H

namespace efgy {
namespace math {
/* Infinite sequence
 * @Q Base type for calculations.
 * @algorithm The algorithm to calculate the sequence members.
 * @N Base integral type; used for indices into the sequence.
 *
 * Represents an infinite sequence. You need to provide an algrithm to
 * calculate arbitrary members of these sequences.
 */
template <typename Q, template <typename, typename> class algorithm,
          typename N = unsigned long long>
class sequence : public algorithm<Q, N> {
 public:
  /* Get n'th sequence member
   *
   * Uses the algorithm template to calculate the n'th member of
   * the sequence.
   *
   * @n The sequence member to return.
   *
   * @return The n'th sequence member.
   */
  constexpr static Q at(const N &n) { return sequenceAlgorithm::at(n); }

 protected:
  /* Sequence algorithm
   *
   * This is a convenient typedef that allows easy access to the
   * sequence algorithm.
   */
  using sequenceAlgorithm = algorithm<Q, N>;
};
}  // namespace math
}  // namespace efgy

#endif
