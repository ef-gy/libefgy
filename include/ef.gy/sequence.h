/**\file
 * \brief Sequences
 *
 * Contains supporting types for infinite sequences.
 *
 * \copyright
 * This file is part of the libefgy project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: https://ef.gy/documentation/libefgy
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 * \see Licence Terms: https://github.com/ef-gy/libefgy/blob/master/COPYING
 */

#if !defined(EF_GY_SEQUENCES_H)
#define EF_GY_SEQUENCES_H

namespace efgy {
namespace math {
/**\brief Infinite sequence
 *
 * Represents an infinite sequence. You need to provide an algrithm to
 * calculate arbitrary members of these sequences.
 *
 * \tparam Q         Base type for calculations.
 * \tparam algorithm The algorithm to calculate the sequence members.
 * \tparam N         Base integral type; used for indices into the
 *                   sequence.
 */
template <typename Q, template <typename, typename> class algorithm,
          typename N = unsigned long long>
class sequence : public algorithm<Q, N> {
public:
  /**\brief Get n'th sequence member
   *
   * Uses the algorithm template to calculate the n'th member of
   * the sequence.
   *
   * \param[in] n The sequence member to return.
   *
   * \returns The n'th sequence member.
   */
  constexpr static Q at(const N &n) { return sequenceAlgorithm::at(n); }

protected:
  /**\brief Sequence algorithm
   *
   * This is a convenient typedef that allows easy access to the
   * sequence algorithm.
   */
  typedef algorithm<Q, N> sequenceAlgorithm;
};
}
}

#endif
