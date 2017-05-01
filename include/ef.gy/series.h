/* Series
 *
 * Contains supporting types for (potentially infinite) series.
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

#if !defined(EF_GY_SERIES_H)
#define EF_GY_SERIES_H

#include <ef.gy/exponential.h>
#include <ef.gy/sequence.h>

namespace efgy {
namespace math {
/* Mathematical series templates
 *
 * This namespace contains templates for the different types of series
 * that aries in mathematics, starting with the series::series template
 * that describes the most basic form of series.
 */
namespace series {
/* Basic series
 * @Q         Base type for calculations.
 * @algorithm The algorithm to calculate the sequence members.
 * @N         Base integral type; used for indices into the sequence.
 *
 * Represents a (potentially infinite) series. Infinite series will
 * be truncated in the process of casting this to the base type.
 */
template <typename Q, template <typename, typename> class algorithm,
          typename N = unsigned long long>
class series : public math::sequence<Q, algorithm, N> {
 protected:
  /* Base sequence
   *
   * The sequence that is used as the basis for the series.
   */
  using sequence = math::sequence<Q, algorithm, N>;

 public:
  using typename sequence::sequenceAlgorithm;

  /* Construct with factor and iterations
   * @pFactor     The factor to apply to the sequence members. Defaults to '1'.
   * @pIterations The default number of iterations to use when approximating the
   * series.
   *
   * Initialises a new series instance with the given factor
   * and number of iterations.
   */
  series(const Q pFactor = Q(1),
         const N &pIterations = sequenceAlgorithm::defaultSeriesIterations)
      : factor(pFactor), iterations(pIterations) {}

  /* Get sum of first n+1 items
   * @n Up to which sequence member to accumulate.
   * @f Factor to multiply the sequence members with.
   *
   * Used to sum up the first n+1 sequence members when it
   * becomes necessary to provide an approximation of the
   * sequence.
   *
   * @return The sum of the 0th to the nth sequence member.
   */
  constexpr static Q get(
      const N &n = sequenceAlgorithm::defaultSeriesIterations,
      const Q &f = Q(1)) {
    return sumTo(n, f, Q(0));
  }

  /* Calculate approximation
   *
   * Cast an actual instance of the series to its base type to
   * get an approximation with the parameters stored in the
   * type.
   *
   * @return The approximation of the series with the parameters in the
   * instance.
   */
  constexpr operator Q(void) const { return get(iterations, factor); }

 protected:
  /* Constant summation function
   * @n Up to which sequence member to accumulate.
   * @f Factor to multiply the sequence members with.
   * @acc The initial (or current) value; used for tail recursion.
   *
   * Used to sum up the first n+1 members of the sequence.
   * This method is tail-recursive so it shouldn't make your
   * stack explode. It is also static and constexpr, meaning
   * it should be evaluated at compile time.
   *
   * @return The sum of the 0th to the nth sequence member.
   */
  constexpr static Q sumTo(const N &n, const Q &f, const Q &acc) {
    return n == 0 ? acc + (sequence::at(0) * f)
                  : sumTo(n - 1, f, acc + (sequence::at(n) * f));
  }

  /* Number of iterations
   *
   * When approximating the sequence, this determines up to
   * which element the series is summed up to.
   */
  const N iterations;

  /* Series factor
   *
   * This factor is applied to each sequence member when
   * approximating an instance of the series.
   */
  const Q factor;
};

/* Power series
 * @Q Base type for calculations.
 * @algorithm The algorithm to calculate the sequence members.
 * @N Base integral type; used for indices into the sequence.
 *
 * Based on the regular series, this represents a power series,
 * which is basically like a regular series but with two additional
 * parameters: a power factor and a centre.
 */
template <typename Q, template <typename, typename> class algorithm,
          typename N = unsigned long long>
class power : public series<Q, algorithm, N> {
  using typename series<Q, algorithm, N>::sequence;
  using typename series<Q, algorithm, N>::sequenceAlgorithm;
  using series<Q, algorithm, N>::iterations;
  using series<Q, algorithm, N>::factor;

 public:
  /* Construct with factors and iterations
   * @pFactor The factor to apply to the sequence members.
   * @pPowerFactor The factor that is raised to the n'th power for each of the
   * sequence members.
   * @pCentre The centre that is subtracted from the power factor.
   * @pIterations The default number of iterations to use when approximating the
   * series.
   *
   * Initialises a new series instance with the given factors
   * and number of iterations.
   */
  power(const Q pFactor = Q(1), const Q pPowerFactor = Q(1),
        const Q pCentre = Q(0),
        const N &pIterations = sequenceAlgorithm::defaultSeriesIterations)
      : series<Q, algorithm, N>(pFactor, pIterations),
        powerFactor(pPowerFactor),
        centre(pCentre) {}

  /**\copydoc series::get
   * @x The power factor.
   * @c The centre of the power series.
   *
   * The power series version of this function has two
   * additional parameters for the power factor and the centre
   * of the power series:
   */
  constexpr static Q get(
      const N &n = sequenceAlgorithm::defaultSeriesIterations,
      const Q &f = Q(1), const Q &x = Q(1), const Q &c = Q(0)) {
    return sumTo(n, f, x, c, Q(0));
  }

  /* Calculate approximation
   *
   * Cast an actual instance of the series to its base type to
   * get an approximation with the parameters stored in the
   * type.
   *
   * @return The approximation of the series with the parameters in the
   * instance.
   */
  constexpr operator Q(void) const {
    return get(iterations, factor, powerFactor, centre);
  }

 protected:
  /**\copydoc series::sumTo
   * @x The power factor.
   * @c The centre of the power series.
   *
   * This varaint of the summation function has two additional
   * parameters for the power factor and the centre of the
   * power series:
   */
  constexpr static Q sumTo(const N &n, const Q &f, const Q &x, const Q &c,
                           const Q &acc) {
    return n == 0 ? acc + (sequence::at(0) * f)
                  : sumTo(n - 1, f, x, c,
                          acc + (sequence::at(n) * f *
                                 exponentiate::integral<Q>::raise(x - c, n)));
  }

  /* Centre
   *
   * This value is subtracted from the power factor before it
   * is raised and subsequently multiplied.
   */
  const Q centre;

  /* Power factor
   *
   * This factor is applied to each sequence member when
   * approximating an instance of the series. Unlike the
   * other factor, this one is raised to the n'th power before
   * being multiplied with the sequence member.
   */
  const Q powerFactor;
};
}
}
}

#endif
