/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_MATH_H
#define MDT_MATH_H

#include <numeric>
#include <cmath>
#include <iterator>
#include <boost/graph/graph_concepts.hpp>

//#include <QDebug>

/*! \brief Some mathematical stuff
 */
namespace mdt{
  inline namespace math
  {
    /*! \brief Helper function to advance iterator it by n elements
     *
     * Will increment given iterator by given distance n.
     * If iterator reaches end, it will not be incremented anymore.
     *
     * \param it Iterator to advance
     * \param itEnd Iterator that represents the end to never exceed.
     * \param n number of element to advance (i.e. distance)
     * \return Number of element that iterator was advanced (distance).
     */
    template<typename IteratorType>
    int advanceIterator(IteratorType & it, const IteratorType & itEnd, int n)
    {
      int i;
      for(i = 0; i < n; ++i){
        if(it == itEnd){
          return i;
        }
        ++it;
      }
      return i;
    }

    /*! \brief Process the product
     *
     * \f$
     *  y = x_0 \cdot x_1 \cdot ... \cdot x_{N-1}
     * \f$
     *
     * In above expression, N is the distance between itBegin and itEnd.
     */
    template<typename T = double, typename IteratorType>
    T product(const IteratorType & itBegin, const IteratorType & itEnd)
    {
      T y = 1.0;
      auto it = itBegin;
      while(it != itEnd){
        y *= (*it);
        ++it;
      }
      return y;
    }

    /*! \brief Process the arithmetic mean
    *
    * \f$
    *  \overline{x} = {1 \over N } \displaystyle\sum_{i=0}^{N-1} x_i
    * \f$
    *
    * In above expression, N is the size of the container c.
    *
    * \pre c must be a non empty container
    */
    template<typename T = double, typename ContainerType>
    T arithmeticMean(const ContainerType & c)
    {
      Q_ASSERT(std::distance(std::begin(c), std::end(c)) > 0);
      return std::accumulate(std::begin(c), std::end(c), 0.0) / static_cast<T>(c.size());
    }

    /*! \brief Process the geometric mean
     *
     * \f$
     *  \overline{x} = \sqrt[N]{x_0 \cdot x_1 \cdot ... \cdot x_{N-1}}
     * \f$
     *
    * In above expression, N is the size of the container c.
    *
    * Note: process is done by block of data.
    *  For each block, the product is computed,
    *  then the N-root applyed.
    *  Regarding given values, blockSize should be adapted.
    *
    * \pre c must be a non empty container
    * \pre blockSize must be <= container size
     */
    template<typename T = double, typename ContainerType>
    T geometricMean(const ContainerType & c, int blockSize)
    {
      Q_ASSERT(std::distance(std::begin(c), std::end(c)) > 0);
      Q_ASSERT( blockSize <= std::distance(std::begin(c), std::end(c)) );

      T mean = 1.0;
      T N = std::distance(std::begin(c), std::end(c));
      auto itBegin = std::begin(c);
      auto itEnd = itBegin;

      advanceIterator(itEnd, std::end(c), blockSize);
      while(itBegin != itEnd){
        mean *= std::pow( product(itBegin, itEnd), static_cast<T>(1)/N );
        advanceIterator(itBegin, std::end(c), blockSize);
        advanceIterator(itEnd, std::end(c), blockSize);
      }

      return mean;
    }

    /*! \brief Calculate the sum of squared differences
     *
     * \f$
     *  sqd = \displaystyle\sum_{i=0}^{N-1} \left(x_i - y \right)^2
     * \f$
     *
     * In above expression, N is the size of the container c.
     */
    template<typename T = double, typename ContainerType>
    T sumOfSquaredDifferences(const ContainerType & c, T y)
    {
      double sum = 0.0;
      for(const auto & x : c){
        sum += std::pow(x - y, 2);
      }
      return sum;
    }

    /*! \brief Calculate the variance
     *
     * \f$
     *  \sigma^2 = { { \displaystyle\sum_{i=0}^{N-1} \left(x_i - \overline{x} \right)^2 } \over N }
     * \f$
     *
     * In above expression, N is the size of the container c.
     *
     * \pre c must be a non empty container
     */
    template<typename T = double, typename ContainerType>
    T variance(const ContainerType & c, T mean)
    {
      Q_ASSERT(std::distance(std::begin(c), std::end(c)) > 0);
      return sumOfSquaredDifferences<T>(c, mean) / static_cast<T>(std::distance(std::begin(c), std::end(c)));
    }

    /*! \brief Calculate the uncorrected standard deviation
     *
     * \f$
     *  \sigma_e = \sqrt{ { \displaystyle\sum_{i=0}^{N-1} \left(x_i - \overline{x} \right)^2 } \over N }
     * \f$
     *
     * In above expression, N is the size of the container c.
     *
     * \pre c must be a non empty container
     */
    template<typename T = double, typename ContainerType>
    T uncorrectedStandardDeviation(const ContainerType & c, T mean)
    {
      Q_ASSERT(std::distance(std::begin(c), std::end(c)) > 0);
      return std::sqrt(variance<T>(c, mean));
    }

    /*! \brief Calculate the corrected standard deviation
     *
     * \f$
     *  \sigma_c = \sqrt{ { \displaystyle\sum_{i=0}^{N-1} \left(x_i - \overline{x} \right)^2 } \over {N - 1} }
     * \f$
     *
     * In above expression, N is the size of the container c.
     *
     * \pre c must contain at least 2 elements
     */
    template<typename T = double, typename ContainerType>
    T correctedStandardDeviation(const ContainerType & c, T mean)
    {
      Q_ASSERT(std::distance(std::begin(c), std::end(c)) > 1);
      return std::sqrt( sumOfSquaredDifferences<T>(c, mean) / (static_cast<T>(std::distance(std::begin(c), std::end(c)))-1.0) );
    }

  }
}
#endif  // #ifndef MDT_MATH_H
