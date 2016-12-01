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
#include "LikeExpression.h"
#include <QRegularExpression>
#include <QLatin1String>
#include <QLatin1Char>
#include <QChar>
#include <QStringBuilder>

#include <algorithm>
#include <iterator>

#include <QDebug>

namespace Mdt{ namespace ItemEditor{

QStringList LikeExpressionRegexTransform::mRegexReserved = {"(",")","[","]","{","}"};

QString LikeExpressionRegexTransform::getRegexPattern(const LikeExpression & expr)
{
  QString resultPattern;
  QString pattern = expr.expression();
  QRegularExpression regex;

  // Replace regular expression metacharacters and abbreviations
  pattern.replace(QLatin1String("{"), QLatin1String("\\{"));
  pattern.replace(QLatin1String("}"), QLatin1String("\\}"));
  pattern.replace(QLatin1String("["), QLatin1String("\\["));
  pattern.replace(QLatin1String("]"), QLatin1String("\\]"));
  pattern.replace(QLatin1String("("), QLatin1String("\\("));
  pattern.replace(QLatin1String(")"), QLatin1String("\\)"));
  pattern.replace(QLatin1String("+"), QLatin1String("\\+"));
  pattern.replace(QLatin1String("|"), QLatin1String("\\|"));
  pattern.replace(QLatin1String("^"), QLatin1String("\\^"));
  pattern.replace(QLatin1String("$"), QLatin1String("\\$"));
  pattern.replace(QLatin1String("."), QLatin1String("\\."));
  pattern.replace(QLatin1String("\\d"), QLatin1String("\\\\d"));
  pattern.replace(QLatin1String("\\D"), QLatin1String("\\\\D"));
  pattern.replace(QLatin1String("\\s"), QLatin1String("\\\\s"));
  pattern.replace(QLatin1String("\\S"), QLatin1String("\\\\S"));
  pattern.replace(QLatin1String("\\w"), QLatin1String("\\\\w"));
  pattern.replace(QLatin1String("\\W"), QLatin1String("\\\\W"));
  pattern.replace(QLatin1String("\\N"), QLatin1String("\\\\N"));

  /// \todo Comment
  //regex.setPattern("[^\\\\]\\?");
  regex.setPattern("\\?");
//   qDebug() << "Error: " << regex.errorString();
  Q_ASSERT(regex.isValid());
  pattern.replace(regex, QLatin1String("."));

  /// \todo Comment
  regex.setPattern("\\_");
  Q_ASSERT(regex.isValid());
  pattern.replace(regex, QLatin1String("."));

  /// \todo Comment
  regex.setPattern("\\*");
  Q_ASSERT(regex.isValid());
  pattern.replace(regex, QLatin1String(".*"));

  /// \todo Comment
  regex.setPattern("\\%");
  Q_ASSERT(regex.isValid());
  pattern.replace(regex, QLatin1String(".*"));

  resultPattern = QLatin1Char('^') % pattern % QLatin1Char('$');

  return resultPattern;
}

bool testFunt(const QChar & c1, const QChar & c2)
{
  qDebug() << "c1: " << c1 << " , c2: " << c2;
  return true;
}

/// \todo Make private
void LikeExpressionRegexTransform::escapeBackslash(QString & str)
{
  const QString & cpy = str;
  QString result;

  if(str.isEmpty()){
    return;
  }
  if(str.size() == 1){
    if(str.at(0) == '\\'){
      str.append('\\');
    }
    return;
  }
  
  int i = str.indexOf('\\');
  while( (i > -1) && (i < str.size()) ){
    qDebug() << "str: " << str << " , i: " << i;
    /*
     * If we matched '\' at last char, we never have a wildcard after -> escape.
     * Else, we have to check if following char is a wildcard
     */
    if( i == (str.size()-1) ){
      qDebug() << "At last char: " << str.at(i);
      str.insert(i, '\\');
    }else{
      Q_ASSERT( i < (str.size()-1) );
      qDebug() << "Before last char , i: " << i << " ch: " << str.at(i) << " , next: " << str.at(i+1);
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
  
//   Q_ASSERT(str.size() > 1);
//   const int lastMatch = str.size() - 1;
//   
//   while( (i >= 0) && (i < lastMatch) ){
//     i = str.indexOf('\\', i);
//     qDebug() << "i: " << i << " , lastMatch: " << lastMatch;
//     if(i < lastMatch && i >= 0){
//       qDebug() << "i: " << i << " , c1: " << str.at(i) << " , c2: " << str.at(i+1);
//     }
//     if( (i >= 0) && (i < lastMatch) && (!isTokenWildcard(str.at(i+1))) ){
//       str.insert(i, '\\');
//       ++i;
//     }
//   }
  
//   auto it1 = cpy.constBegin();
//   auto it2 = it1+1;
//   const auto last = cpy.constEnd();
//   while(it2 != last){
//     qDebug() << "it1: " << *it1 << ", it2: " << *it2;
//     if(*it1 == '\\'){
//       
//     }
//     ++it1;
//     ++it2;
//   }
  
  /// \todo precondition, etc..
//   std::transform(cpy.constBegin(), cpy.constEnd(), cpy.constBegin()+1, std::back_inserter(result), testFunt);
  
//   qDebug() << "result: " << result;
//   str.replace('\\', QLatin1String("\\\\"));
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
}

/// \todo obselete
void LikeExpressionRegexTransform::escape(QString & str, const QString & token, const QString & escapeStr)
{
  Q_ASSERT(!token.isEmpty());

  QString es = escapeStr;
  
  qDebug() << mRegexReserved;
  
  QRegularExpression regex;
  if(es == "\\"){
    es = "\\\\";
  }
  regex.setPattern(es + "{0,1}" + token);
  qDebug() << "regex: " << regex.pattern() << " , matches " << str << " : " << regex.match(str).hasMatch();
  Q_ASSERT(regex.isValid());
  str.replace(regex, escapeStr+token);
}

bool LikeExpressionRegexTransform::isTokenWildcard(const QChar & c)
{
  return (c == '?' || c == '*' || c == '%' || c == '_');
}

// bool LikeExpressionRegexTransform::isTokenWildcard(const QString & token)
// {
//   if(token.size() != 1){
//     return false;
//   }
//   const char c = token.at(0).toLatin1();
// 
//   return (c == '?' || c == '*' || c == '%' || c == '_');
// }

// void LikeExpressionRegexTransform::escapeWildcard(QString& str, const QChar & token)
// {
// 
// }

}} // namespace Mdt{ namespace ItemEditor{
