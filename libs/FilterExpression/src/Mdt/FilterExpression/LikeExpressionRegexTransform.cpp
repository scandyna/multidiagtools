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
#include "LikeExpressionRegexTransform.h"
#include "Mdt/Algorithm.h"
#include <QRegularExpression>
#include <QLatin1String>
#include <QLatin1Char>
#include <QChar>
#include <QStringBuilder>
#include <algorithm>

// #include <QDebug>

namespace Mdt{ namespace FilterExpression{

QString LikeExpressionRegexTransform::getRegexPattern(const QString & expr)
{
  QString pattern = expr;

  escapeRegexMetacharacters(pattern);
  replaceWildcards(pattern);
  pattern = '^' % pattern % '$';

  return pattern;
}

void LikeExpressionRegexTransform::escapeRegexMetacharacters(QString & str)
{
  escapeBackslash(str);
  str.replace('{', QLatin1String("\\{"));
  str.replace('}', QLatin1String("\\}"));
  str.replace('[', QLatin1String("\\["));
  str.replace(']', QLatin1String("\\]"));
  str.replace('(', QLatin1String("\\("));
  str.replace(')', QLatin1String("\\)"));
  str.replace('^', QLatin1String("\\^"));
  str.replace('$', QLatin1String("\\$"));
  str.replace('.', QLatin1String("\\."));
  str.replace('+', QLatin1String("\\+"));
  str.replace('|', QLatin1String("\\|"));
}

void LikeExpressionRegexTransform::escapeBackslash(QString & str)
{
  if(str.isEmpty()){
    return;
  }
  int i = str.indexOf('\\');
  while( (i > -1) && (i < str.size()) ){
    /*
     * If we matched '\' at last char, we never have a wildcard after -> escape.
     * Else, we have to check if following char is a wildcard
     */
    if( i == (str.size()-1) ){
      str.insert(i, '\\');
    }else{
      Q_ASSERT( i < (str.size()-1) );
      if( !isTokenWildcard(str.at(i+1)) ){
        str.insert(i, '\\');
      }
    }
    /*
     * Increment and find next match
     */
    i += 2;
    if(i < str.size()){
      i = str.indexOf('\\', i);
    }
  }
}

void LikeExpressionRegexTransform::replaceWildcards(QString & str)
{
  using Mdt::Algorithm::replaceNonEscapedTokens;

  str = replaceNonEscapedTokens(str, {{'?',"."},{'*',".*"}}, '\\');
}

bool LikeExpressionRegexTransform::isTokenWildcard(const QChar & c)
{
  return (c == '?' || c == '*');
}

}} // namespace Mdt{ namespace FilterExpression{
