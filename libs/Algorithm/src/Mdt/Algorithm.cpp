/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "Algorithm.h"
#include <algorithm>
#include <iterator>

// #include <QDebug>

namespace Mdt{ namespace Algorithm{

QString removeFirstLastCharIf(const QString & str, QChar c)
{
  QString retStr = str;

  if(retStr.endsWith(c)){
    retStr.remove( retStr.length()-1, 1 );
  }
  if(retStr.startsWith(c)){
    retStr.remove(0, 1);
  }

  return retStr;
}

int indexOfFirstEscapedToken(const QString & str, int from, const std::vector<QChar> & tokens, const QChar & escape)
{
  Q_ASSERT(!str.isEmpty());
  Q_ASSERT(from >= 0);
  Q_ASSERT(from < str.size());
  Q_ASSERT(tokens.size() > 0);

  auto it = str.constBegin() + from;
  it = std::adjacent_find(it, str.constEnd(),
    [escape, tokens](const QChar & a, const QChar & b)
    {
      if(a != escape){
        return false;
      }
      return ( std::find(tokens.cbegin(), tokens.cend(), b) != tokens.cend() );
    }
  );
  if(it == str.constEnd()){
    return -1;
  }
  return it - str.constBegin();
}

QString unescapeEscapedTokens(const QString & str, const std::vector<QChar> & tokens, const QChar & escape)
{
  QString result = str;

  if(result.isEmpty()){
    return result;
  }
  int i = indexOfFirstEscapedToken(result, 0, tokens , escape);
  while( (i > -1) && (i < result.size()) ){
    // Remove current matching escape
    result.remove(i, 1);
    ++i;
    // Find next escaped token
    if(i < result.size()){
      i = indexOfFirstEscapedToken(result, i, tokens , escape);
    }
  }

  return result;
}

int indexOfFirstNonEscapedToken(const QString & str, int from, const std::vector<QChar> & tokens, const QChar & escape)
{
  Q_ASSERT(!str.isEmpty());
  Q_ASSERT(from >= 0);
  Q_ASSERT(from < str.size());
  Q_ASSERT(tokens.size() > 0);

  auto it = str.constBegin() + from;
  const auto last = str.constEnd();

  while(it != last){
    it = std::find_first_of(it, last, tokens.cbegin(), tokens.cend());
    // If we found a token, see if it is not escaped
    if(it != last){
      /*
       * If token is found at first position in str, it cannot be escaped.
       * Else, we must check.
       */
      if(it == str.constBegin()){
        return 0;
      }else{
        if(*(it-1) != escape){
          return it - str.constBegin();
        }
      }
      ++it;
    }
  }

  return -1;
}

QString replaceNonEscapedTokens(const QString & str, const std::initializer_list< std::pair<QChar, QString> > & replaceList, const QChar & escape)
{
  QString result = str;

  if(result.isEmpty()){
    return result;
  }
  /*
   * Create list of tokens to match
   */
  std::vector<QChar> tokens;
  tokens.reserve(replaceList.size());
  std::transform(replaceList.begin(), replaceList.end(), std::back_inserter(tokens), [](const auto & p){return p.first;}  );
  /*
   * Process
   */
  int i = indexOfFirstNonEscapedToken(result, 0, tokens , escape);
  QChar token;
  while( (i > -1) && (i < result.size()) ){
    const QChar c = result.at(i);
    // Find token/replacement pair that match in current position in result
    const auto it = std::find_if(replaceList.begin(), replaceList.end(), [c](const auto & p){return p.first == c;} );
    Q_ASSERT(it != replaceList.end());
    const auto replacement = *it;
    // Replace
    result.replace(i, 1, replacement.second);
    // Go to position after replacement
    i += replacement.second.size();
    // Find next token to replace
    if(i < result.size()){
      i = indexOfFirstNonEscapedToken(result, i, tokens , escape);
    }
  }

  return result;
}

int count(const QStringList & stringList, const QString & string, Qt::CaseSensitivity caseSensitivity)
{
  Q_ASSERT(!string.isEmpty());

  const auto pred = [string, caseSensitivity](const QString & currentString){
    return (QString::compare(currentString, string, caseSensitivity) == 0);
  };

  return std::count_if(stringList.cbegin(), stringList.cend(), pred);
}

bool hasDuplicates(const QStringList & stringList, Qt::CaseSensitivity caseSensitivity)
{
  for(const auto & str : stringList){
    if( count(stringList, str, caseSensitivity) > 1 ){
      return true;
    }
  }
  return false;
}

}} // namespace Mdt{ namespace Algorithm{