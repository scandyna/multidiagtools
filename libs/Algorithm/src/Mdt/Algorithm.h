/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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

#include <QString>
#include <QLatin1String>
#include <QChar>
#include <utility>
// #include <string>

#include <vector>

#include <initializer_list>

namespace Mdt{

/*! \brief Some helper that could be usefull
 */
namespace Algorithm{

  /*! \brief Get a string with first and last char removed in str if they match c
   */
  QString removeFirstLastCharIf(const QString & str, QChar c);

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
  int indexOfFirstNonEscapedToken(const QString & str, int from, const std::vector<QChar>  & tokens, const QChar & escape);
//   int indexOfFirstNonEscapedToken(const QString & str, int from, const std::initializer_list<QChar>  & tokens, const QChar & escape);

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
   * 
   */
  QString replaceNonEscapedTokens(const QString & str, const std::initializer_list< std::pair<QChar,QString> > & replaceList, const QChar & escape);

}} // namespace Mdt{ namespace Algorithm{

#endif // #ifndef MDT_ALGORITHM_H
