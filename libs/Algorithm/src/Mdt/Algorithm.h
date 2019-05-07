/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_ALGORITHM_H
#define MDT_ALGORITHM_H

#include "MdtAlgorithmExport.h"
#include <QString>
#include <QStringList>
#include <QLatin1String>
#include <QChar>
#include <Qt>
#include <algorithm>
#include <utility>
#include <vector>
#include <iterator>
#include <initializer_list>
#include <type_traits>

#include <functional>

namespace Mdt{

/*! \brief Some helper that could be usefull
 */
namespace Algorithm{

  /*! \brief Get a string with first and last char removed in str if they match c
   */
  QString MDT_ALGORITHM_EXPORT removeFirstLastCharIf(const QString & str, QChar c);

  /*! \brief Find the first token that is escaped in str
   *
   * \param str String in which to find token
   * \param from Index in str from which to start
   * \param tokens List of elements that are tokens
   * \param escape Char that is used to escape
   * \pre from must be in a valid position in str: 0 <= from < str length .
   *      This implies also that str is not empty.
   * \pre tokens must not be a empty list
   * \todo Define what happens when escape is same as a token
   * \return The position of escape that matched a token, or -1 if no token was found.
   *
   * Example:
   * \code
   * QString str = "A?B\\?CD\\?E";
   * int i;
   *
   * i = indexOfFirstEscapedToken(str, 0, {'?','*'}, '\\');
   * // i == 3
   *
   * i = indexOfFirstEscapedToken(str, 4, {'?','*'}, '\\');
   * // i == 7
   * \endcode
   */
  int MDT_ALGORITHM_EXPORT indexOfFirstEscapedToken(const QString & str, int from, const std::vector<QChar>  & tokens, const QChar & escape);

  /*! \brief Get a string in which each escaped token is unescaped
   *
   * \param str String in which to find escaped tokens
   * \param tokens List of elements that are tokens
   * \param escape Char that is used to escape
   * \return str with tokens unescaped
   *
   * Example:
   * \code
   * QString str = "A*B?C\\?D";
   *
   * str = unescapeEscapedTokens(str, {'?',"*"}, '\\' );
   * // str == "A*B?C?D"
   * \endcode
   */
  QString MDT_ALGORITHM_EXPORT unescapeEscapedTokens(const QString & str, const std::vector<QChar> & tokens, const QChar & escape);

  /*! \brief Find the first token that is not escaped in str
   *
   * \param str String in which to find token
   * \param from Index in str from which to start
   * \param tokens List of elements that are tokens
   * \param escape Char that is used to escape
   * \pre from must be in a valid position in str: 0 <= from < str length .
   *      This implies also that str is not empty.
   * \pre tokens must not be a empty list
   * \todo Define what happens when escape is same as a token
   * \return The position that matched, or -1 if no token was found.
   *
   * Example:
   * \code
   * QString str = "A?B\\?C?";
   * int i;
   *
   * i = indexOfFirstNonEscapedToken(str, 0, {'?','*'}, '\\');
   * // i == 1
   *
   * i = indexOfFirstNonEscapedToken(str, 2, {'?','*'}, '\\');
   * // i == 6
   * \endcode
   */
  int MDT_ALGORITHM_EXPORT indexOfFirstNonEscapedToken(const QString & str, int from, const std::vector<QChar>  & tokens, const QChar & escape);

  /*! \brief Get a string in which each non escaped token is replaced
   *
   * \param str String in which to search tokens to replace
   * \param replaceList List of replacement par.
   *                    For each pair, first is the token to match and second is its replacement
   * \param escape Char that is used to escape
   * \return str with tokens replaced
   *
   * Example:
   * \code
   * QString str = "A*B?C\\?D";
   *
   * str = replaceNonEscapedTokens(str, {{'?',"."},{'*',".*"}}, '\\' );
   * // str == "A%B_C\\?D"
   * \endcode
   */
  QString MDT_ALGORITHM_EXPORT replaceNonEscapedTokens(const QString & str, const std::initializer_list< std::pair<QChar,QString> > & replaceList, const QChar & escape);

/** \todo Add moveIf() in Mdt::Algorithm:
 *
 * \note Probably not a good idea
 *
 *  - Internaly, use std::partition
 *  - Explain what SourceContainer requires: iterator, begin(), end()
 *  - Explain what DestinationContainer requires: iterator, compatible with std::back_inserter, erase(const_iterator first, const_iterator last)
 *                                                        erase: QVector has erase(iterator first, iterator last), check.
 *
 * template<typename ForwardIt, typename OutputIt, typename UnaryPredicate>
 * void moveIf(ForwardIt first, ForwardIt last, OutputIt d_first, UnaryPredicate p);
 *
 * template<typename SourceContainer, typename DestinationContainer, typename UnaryPredicate>
 * void moveIf(SourceContainer & sourceConatiner, DestinationContainer & destinationContainer, UnaryPredicate p);
 */

  /*! \brief Move a range of elements to a new location
   *
   * Move the elements, for which the predicate \a p returns true,
   *  from range [\a first, \a last) to range \a d_first .
   *
   * \param first The beginnig of the source range
   * \param last The end of the source range
   * \param d_first 
   * \param p The unary predicate which returns true if a element must be moved.
   *          The signature of the predicate function should be equivalent to the following:
   *          \code
   *          bool pred(const Type & value);
   *          \endcode
   * \return A iterator past the end of the new source range.
   * \pre \a first and \a last must meet the requirement of
   *       [ValueSwappable](http://en.cppreference.com/w/cpp/concept/ValueSwappable)
   *       and [ForwardIterator](http://en.cppreference.com/w/cpp/concept/ForwardIterator)
   * \pre \a d_first must meet the requirement of
   *        [OutputIterator](http://en.cppreference.com/w/cpp/concept/OutputIterator)
   *
   * Example:
   * \code
   * QStringList sourceList{"A","B","C"};
   * QStringList destinationList;
   *
   * const auto predicate = [](const QString & value){
   *   return value == QLatin1String("B");
   * };
   * auto it = Mdt::Algorithm::moveIf(sourceList.cbegin(), sourceList.cend(), std::back_inserter(destinationList), predicate);
   * sourceList.erase(it, sourceList.cend());
   * \endcode
   * sourceList will contain A,C abd destinationList will contain B .
   *
   * \todo Check, think that cbegin()/cend() will not work
   *
   * \sa void moveIf(SourceContainer &, DestinationContainer &, UnaryPredicate)
   */
  template<typename ForwardIt, typename OutputIt, typename UnaryPredicate>
  ForwardIt moveIf(ForwardIt first, ForwardIt last, OutputIt d_first, UnaryPredicate p)
  {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type;
    std::function<bool(const value_type &)> f = p;
    auto itp = std::partition(first, last, std::not1(f)); // std::not1() is deprecated in C++17
    std::move(itp, last, d_first);
    return itp;
  }

  /*! \brief Move a range of elements to a new location
   *
   * Move the elements, for which the predicate \a p returns true,
   *  from \a sourceConatiner to \a destinationContainer .
   *
   * \param p The unary predicate which returns true if a element must be moved.
   *          The signature of the predicate function should be equivalent to the following:
   *          \code
   *          bool pred(const Type & value);
   *          \endcode
   * \pre \a SourceContainer requires STL compatible begin(), end() and erase() methods.
   * \pre \a DestinationContainer requires a push_back() method that can be used by std::back_inserter .
   *
   * Example:
   * \code
   * QStringList sourceList{"A","B","C"};
   * QStringList destinationList;
   *
   * const auto predicate = [](const QString & value){
   *   return value == QLatin1String("B");
   * };
   * Mdt::Algorithm::moveIf(sourceList, destinationList, predicate);
   * \endcode
   * sourceList will contain A,C abd destinationList will contain B .
   *
   * \sa ForwardIt moveIf(ForwardIt, ForwardIt, OutputIt, UnaryPredicate)
   */
  template<typename SourceContainer, typename DestinationContainer, typename UnaryPredicate>
  void moveIf(SourceContainer & sourceConatiner, DestinationContainer & destinationContainer, UnaryPredicate p)
  {
    auto it = moveIf(sourceConatiner.begin(), sourceConatiner.end(), std::back_inserter(destinationContainer), p);
    sourceConatiner.erase(it, sourceConatiner.end());
  }

  /*! \brief Get the number of occurences of \a string in \a stringList
   *
   * \pre \a string must not be empty
   */
  int MDT_ALGORITHM_EXPORT count(const QStringList & stringList, const QString & string, Qt::CaseSensitivity caseSensitivity);

  /*! \brief Check if \a stringList has duplicate elements
   *
   * \code
   * QStringList list1{"a","b","c"};
   * hasDuplicates(list1, Qt::CaseInsensitive); // Returns false
   * hasDuplicates(list1, Qt::CaseSensitive);   // Returns false
   *
   * QStringList list2{"a","b","a"};
   * hasDuplicates(list2, Qt::CaseInsensitive); // Returns true
   * hasDuplicates(list2, Qt::CaseSensitive);   // Returns true
   *
   * QStringList list3{"a","b","A"};
   * hasDuplicates(list3, Qt::CaseInsensitive); // Returns true
   * hasDuplicates(list3, Qt::CaseSensitive);   // Returns false
   * \endcode
   */
  bool MDT_ALGORITHM_EXPORT hasDuplicates(const QStringList & stringList, Qt::CaseSensitivity caseSensitivity);

  /*! \brief Applies the given function object \a f to the result of dereferencing every pair of adjacent iterators in the range [\a first, \a last)
   *
   * This function template is similar to [std::for_each()](http://en.cppreference.com/w/cpp/algorithm/for_each),
   *  but it calls \a f for each adjacent iterators \a first and \a first + 1.
   *
   * The iterator types are mutable, f may modify the elements of the range trough the dereferenced iterator.
   *  If f returns a result, the result is ignored.
   *
   * \tparam ForwardIt Type of a iterator that meets the requirement of [ForwardIterator](https://en.cppreference.com/w/cpp/named_req/ForwardIterator).
   * \tparam BinaryFunction A function object with a signature equivalent to:
   *          \code
   *          void f(const Type & a, const Type & b);
   *          \endcode
   *
   * Example:
   * \code
   * std::vector<int> inVector{1,2,3,4,5};
   * std::vector<int> outVector;
   *
   * const auto f = [&outVector](int a, int b){
   *   outVector.push_back(a+b);
   * };
   *
   * Mdt::Algorithm::forEachAdjacentPair(inVector.cbegin(), inVector.cend(), f);
   *
   * // outVector: {3,5,7,9}
   * \endcode
   */
  template<typename ForwardIt, typename BinaryFunction>
  constexpr void forEachAdjacentPair(ForwardIt first, ForwardIt last, BinaryFunction f)
  {
    if(first != last){
      auto trailer = first;
      ++first;
      while(first != last){
        f(*trailer, *first);
        ++first;
        ++trailer;
      }
    }
  }

  /*! \brief Returns the last element that is consecutive in the range [\a first, \a last)
   *
   * \tparam ForwardIt Type of a iterator that meets the requirement of [ForwardIterator](https://en.cppreference.com/w/cpp/named_req/ForwardIterator).
   *
   * \pre The value type of \a ForwardIt must be a integral type
   * \pre The range [\a first, \a last) must be sorted
   *
   * Example:
   * \code
   * std::vector<int> v0{};
   * std::vector<int> v1{1};
   * std::vector<int> v2{1,2};
   * std::vector<int> v3{1,2,3,5};
   * std::vector<int> v4{1,3,4,5};
   *
   * const auto it0 = Mdt::Algorithm::findLastConsecutiveIntegralValue(v0.cbegin(), v0.cend());
   * // it0 == v0.cend()
   *
   * const auto it1 = Mdt::Algorithm::findLastConsecutiveIntegralValue(v1.cbegin(), v1.cend());
   * // *it1 == 1
   *
   * const auto it2 = Mdt::Algorithm::findLastConsecutiveIntegralValue(v2.cbegin(), v2.cend());
   * // *it2 == 2
   *
   * const auto it3 = Mdt::Algorithm::findLastConsecutiveIntegralValue(v3.cbegin(), v3.cend());
   * // *it3 == 3
   *
   * const auto it4 = Mdt::Algorithm::findLastConsecutiveIntegralValue(v4.cbegin(), v4.cend());
   * // *it4 == 1
   * \endcode
   */
  template<typename ForwardIt>
  ForwardIt findLastConsecutiveIntegralValue(ForwardIt first, ForwardIt last)
  {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type;

    static_assert( std::is_integral<value_type>::value,
                   "The values in the range must be of integral type" );
    Q_ASSERT(std::is_sorted(first, last));

    const auto isConsecutive = [](value_type a, value_type b){
      return (b-a) == 1;
    };

    return std::mismatch(first, last, first+1, last, isConsecutive).first;
  }

}} // namespace Mdt{ namespace Algorithm{

#endif // #ifndef MDT_ALGORITHM_H
