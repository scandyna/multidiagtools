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
#ifndef MDT_FILTER_EXPRESSION_LIKE_EXPRESSION_REGEX_TRANSFORM_H
#define MDT_FILTER_EXPRESSION_LIKE_EXPRESSION_REGEX_TRANSFORM_H

#include <QString>
#include <QChar>

namespace Mdt{ namespace FilterExpression{

  class LikeExpression;

//   struct LikeExpressionRegexTransformWildcardMatch
//   {
//     int index = -1;
//     QChar wildcard;
//   };

  /*! \brief Transform a LikeExpression to a regular expression
   */
  class LikeExpressionRegexTransform
  {
   public:

    /*! \brief Get regular expression of expr
     */
    static QString getRegexPattern(const LikeExpression & expr);

    /*! \brief Escape regexp metacharacters
     */
    static void escapeRegexMetacharacters(QString & str);

   private:

    /*! \brief Only escapes '\' if not escaping a wildcard character
     */
    static void escapeBackslash(QString & str);

    /*! \brief Replace wildcards if not escaped
     */
    static void replaceWildcards(QString & str);

    /*! \brief Replace wildcard at position pos in str
     *
     * \return next index in str
     */
//     static int replaceWildcard(QString & str, int pos, const QChar & wildcard);

    /*! \brief Find a wildcard in str
     */
//     static LikeExpressionRegexTransformWildcardMatch findFirstWildcard(const QString & str, int from);

    static bool isTokenWildcard(const QChar & c);
  };

}} // namespace Mdt{ namespace FilterExpression{

#endif // #ifndef MDT_FILTER_EXPRESSION_LIKE_EXPRESSION_REGEX_TRANSFORM_H
