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
#include "Algorithm.h"
#include <algorithm>
#include <iterator>

#include <array>

#include <QDebug>

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

int indexOfFirstNonEscapedToken(const QString & str, int from, const std::vector<QChar> & tokens, const QChar & escape)
{
  Q_ASSERT(!str.isEmpty());
  Q_ASSERT(from >= 0);
  Q_ASSERT(from < str.size());
  Q_ASSERT(tokens.size() > 0);

  auto it = str.constBegin() + from;
  const auto last = str.constEnd();

  while(it != last){
    it = std::find_if(it, last, [tokens](const QChar & c){
                                  return ( std::find(tokens.begin(), tokens.end(), c) != tokens.end());
                                });
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


}} // namespace Mdt{ namespace Algorithm{