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
#ifndef MDT_ITEM_EDITOR_LIKE_EXPRESSION_REGEX_TRANSFORM_H
#define MDT_ITEM_EDITOR_LIKE_EXPRESSION_REGEX_TRANSFORM_H

#include <QString>
#include <QStringList>

class QChar;

namespace Mdt{ namespace ItemEditor{

  class LikeExpression;

  /*! \brief Transform a LikeExpression to a regular expression
   */
  class LikeExpressionRegexTransform
  {
   public:

    /*! \brief Get regular expression of expr
     */
    static QString getRegexPattern(const LikeExpression & expr);

    /*! \brief Only escapes '\' if not escaping a wildcard character
     */
    static void escapeBackslash(QString & str);

    /*! \brief Escape regexp metacharacters
     */
    static void escapeRegexMetacharacters(QString & str);

    /*! \brief Escape each occurence of token with escape in str
     */
    static void escape(QString & str, const QString & token, const QString & escapeStr);

   private:


    static bool isTokenWildcard(const QChar & token);
//     static bool isTokenWildcard(const QString & token);
    static void escapeWildcard(QString & str, const QChar & token);

    static QStringList mRegexReserved;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_LIKE_EXPRESSION_REGEX_TRANSFORM_H
