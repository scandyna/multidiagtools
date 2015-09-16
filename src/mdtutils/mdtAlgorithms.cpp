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
#include "mdtAlgorithms.h"
#include <QChar>
#include <QStringRef>
#include <limits>
#include <cmath>

//#include <QDebug>

mdtAlgorithms::NumericRangeDouble::NumericRangeDouble()
 : bottom(0.0),
   top(0.0),
   includeBottom(false),
   includeTop(false)
{
}

mdtAlgorithms::NumericRangeDouble::NumericRangeDouble(double min, double max, bool includeMin, bool includeMax)
 : bottom(min),
   top(max),
   includeBottom(includeMin),
   includeTop(includeMax)
{
}

bool mdtAlgorithms::isInRange(double x, double bottom, double top, bool includeBottom, bool includeTop)
{
  Q_ASSERT(std::isless(bottom, top));
  if(includeBottom && includeTop){
    return (std::isgreaterequal(x, bottom) && std::islessequal(x, top));
  }
  if(includeBottom && (!includeTop)){
    return (std::isgreaterequal(x, bottom) && std::isless(x, top));
  }
  if((!includeBottom) && includeTop){
    return (std::isgreater(x, bottom) && std::islessequal(x, top));
  }
  Q_ASSERT((!includeBottom)&&(!includeTop));
  return (std::isgreater(x, bottom) && std::isless(x, top));
}

QStringList mdtAlgorithms::naturalSort(QStringList &list, Qt::CaseSensitivity caseSensitivity)
{
  if(caseSensitivity == Qt::CaseSensitive){
    qSort(list.begin(), list.end(), naturalCompareLessThanCaseSensitive);
  }else{
    qSort(list.begin(), list.end(), naturalCompareLessThanCaseInsensitive);
  }

  return list;
}

bool mdtAlgorithms::naturalCompareLessThan(const QString &str1, const QString &str2, Qt::CaseSensitivity caseSensitivity)
{
  QString a;
  QString b;
  const QChar *currA;
  const QChar *currB;
  const QChar *begSeqA;
  const QChar *begSeqB;
  int cmp;
  bool isFirstRun;
  int weight;

  if(caseSensitivity == Qt::CaseSensitive){
    a = str1;
    b = str2;
  }else{
    a = str1.toLower();
    b = str2.toLower();
  }
  // Get iterators over a and b
  currA = a.unicode();
  currB = b.unicode();
  // If str1 (a) and str2 (b) are the same string, we have no difference
  if(currA == currB){
    return false;
  }

  while((!currA->isNull()) && (!currB->isNull())){
    // Remember beginning of sequences
    begSeqA = currA;
    begSeqB = currB;
    // Check about special characters
    if(currA->unicode() == QChar::ObjectReplacementCharacter){
      return false;
    }
    if(currB->unicode() == QChar::ObjectReplacementCharacter){
      return true;
    }
    if(currA->unicode() == QChar::ReplacementCharacter){
      return false;
    }
    if(currB->unicode() == QChar::ReplacementCharacter){
      return true;
    }
    // find sequence of characters ending at the first non-character
    while((!currA->isNull()) && (!currA->isDigit()) && (!currA->isPunct()) && (!currA->isSpace())){
      ++currA;
    }
    while((!currB->isNull()) && (!currB->isDigit()) && (!currB->isPunct()) && (!currB->isSpace())){
      ++currB;
    }
    // compare these sequences
    const QStringRef &subA(a.midRef(begSeqA - a.unicode(), currA - begSeqA));
    const QStringRef &subB(b.midRef(begSeqB - b.unicode(), currB - begSeqB));
    cmp = QStringRef::localeAwareCompare(subA, subB);
    if(cmp < 0){
      return true;
    }else if(cmp > 0){
      return false;
    }
    if(currA->isNull() || currB->isNull()){
      break;
    }
    // find sequence of characters ending at the first non-character
    while((currA->isPunct() || currA->isSpace()) && (currB->isPunct() || currB->isSpace())){
      if(*currA < *currB){
        return true;
      }else if(*currA > *currB){
        return false;
      }
      ++currA;
      ++currB;
      if(currA->isNull() || currB->isNull()){
        break;
      }
    }
    // now some digits follow...
    if((*currA == QChar('0')) || (*currB == QChar('0'))){
      // one digit-sequence starts with 0 -> assume we are in a fraction part
      // do left aligned comparison (numbers are considered left aligned)
      while(1){
        if((!currA->isDigit()) && (!currB->isDigit())){
          break;
        }else if(!currA->isDigit()){
          return false;
        }else if(!currB->isDigit()){
          return true;
        }else if(*currA < *currB){
          return true;
        }else if(*currA > *currB){
          return false;
        }
        ++currA;
        ++currB;
      }
    }else{
      // no digit-sequence starts with 0, assume we are looking at some integer
      // do right aligned comparison
      // The longest run of digits wins. That aside, the gratest value wins,
      // but we can't know that it will until we have scanned both numbers
      // to know that they have the same magnitude
      isFirstRun = true;
      weight = 0;
      while(1){
        if((!currA->isDigit()) && (!currB->isDigit())){
          if(weight < 0){
            return true;
          }else if(weight > 0){
            return false;
          }
          break;
        }else if(!currA->isDigit()){
          if(isFirstRun){
            if(*currA < *currB){
              return true;
            }else{
              return false;
            }
          }else{
            return true;
          }
        }else if(!currB->isDigit()){
          if(isFirstRun){
            if(*currA < *currB){
              return true;
            }else{
              return false;
            }
          }else{
            return false;
          }
        }else if((*currA < *currB) && (weight == 0)){
          weight = -1;
        }else if((*currA > *currB) && (weight == 0)){
          weight = +1;
        }
        ++currA;
        ++currB;
        isFirstRun = false;
      }
    }
  }
  if(currA->isNull() && currB->isNull()){
    return false;
  }
  if(currA->isNull()){
    return true;
  }
  return false;
}

bool mdtAlgorithms::naturalCompareLessThanCaseSensitive(const QString &str1, const QString &str2)
{
  return naturalCompareLessThan(str1, str2, Qt::CaseSensitive);
}

bool mdtAlgorithms::naturalCompareLessThanCaseInsensitive(const QString &str1, const QString &str2)
{
  return naturalCompareLessThan(str1, str2, Qt::CaseInsensitive);
}

QByteArray mdtAlgorithms::hexStringToByteArray(const QString &hexStr)
{
  QByteArray byteArray;
  QString hexString;
  QString hexNum;
  int i, len;
  bool ok;

  // Remove every spaces
  hexString = hexStr.simplified();
  hexString.remove(' ');
  Q_ASSERT((hexString.size() % 2) == 0);
  len = hexString.size();
  // Travel the hexString and convert each 2 bytes (52 H4 ...)
  for(i=0; i<len; i = i+2){
    hexNum.clear();
    hexNum.append(hexString.at(i));
    hexNum.append(hexString.at(i+1));
    byteArray.append((char)hexNum.toInt(&ok, 16));
    Q_ASSERT(ok == true);
  }

  return byteArray;
}

QString mdtAlgorithms::byteArrayToHexString(const QByteArray &byteArray)
{
  QString hexString;
  QString hexNum;
  int i, len;
  
  // Travel the byte array and convert each byte
  len = byteArray.size();
  for(i=0; i<len; i++){
    hexNum.setNum((unsigned char)byteArray.at(i), 16);
    // If we have only one char, we prepend a 0
    if(hexNum.size() < 2){
      hexNum.prepend("0");
    }
    hexString.append(hexNum);
    // Append a white space
    hexString.append(" ");
  }

  return hexString.trimmed();
}

QString mdtAlgorithms::unprotectedString(const QString &str, const QString &dataProtection, const QChar &escapeChar, int *strEndOffset, int strStartOffset)
{
  QString result;
  int dpCursor = -1;
  int dpCursorMem = -1;
  int remainingDp = 2;
  int start;
  int end = 0;
  bool escapeEqualProtection = QString(escapeChar) == dataProtection;

  if(strStartOffset > -1){
    start = strStartOffset;
  }else{
    start = 0;
  }

  while((end < str.size())&&(remainingDp > 0)){
    // Get index of data protection
    if(!dataProtection.isEmpty()){
      dpCursor = str.indexOf(dataProtection, start);
    }else{
      dpCursor = -1;
    }
    if(dpCursor < 0){
      // Finish parsing
      dpCursor = dpCursorMem;
      break;
    }else{
      if(escapeEqualProtection){
        if(dpCursor < (str.size()-1)){
          // check about escaped sequence
          if(str.at(dpCursor+1) == escapeChar){
            // Escaped sequence
            end = dpCursor + 1;
            result.append(str.mid(start, end-start));
            start = dpCursor + dataProtection.size() + 1;
          }else{
            --remainingDp;
            end = dpCursor;
            result.append(str.mid(start, end-start));
            start = dpCursor + dataProtection.size();
          }
        }else{
          end = dpCursor;
          result.append(str.mid(start, end-start));
          break;
        }
      }else{
        if(dpCursor == 0){
          // Just move start cursor after protection
          start = dpCursor + dataProtection.size();
          --remainingDp;
        }else{
          if(str.at(dpCursor-1) == escapeChar){
            // Escaped sequence
            end = dpCursor-1;
            result.append(str.mid(start, end-start));
            // Add escape sequence
            result.append(str.mid(dpCursor, dataProtection.size()));
            start = dpCursor + dataProtection.size();
          }else{
            --remainingDp;
            end = dpCursor;
            result.append(str.mid(start, end-start));
            start = dpCursor + dataProtection.size();
          }
        }
      }
    }
    dpCursorMem = dpCursor;
    end += dataProtection.size();
  }
  if(strEndOffset != 0){
    *strEndOffset = dpCursor;
  }

  return result;
}

QStringList mdtAlgorithms::splitString(const QString &str, const QString &separator, const QString &dataProtection, const QChar &escapeChar)
{
  int sepCursor = 0;    // Cursor in str string for separator
  int dpCursor = 0;     // Cursor in str string for dataProtection
  QStringList fields;
  int start = 0;
  int end = 0;

  while(end < str.size()){
    // Get separator and data protection indexes
    if(!separator.isEmpty()){
      sepCursor = str.indexOf(separator, start);
    }else{
      sepCursor = -1;
    }
    if(!dataProtection.isEmpty()){
      dpCursor =  str.indexOf(dataProtection, start);
    }else{
      dpCursor = -1;
    }
    if((sepCursor < 0)&&(dpCursor < 0)){
      // No keyword found, copy remaining string
      end = str.size();
      if(start != end){
        fields.append(str.mid(start, end-start));
      }
    }else if((sepCursor < 0)&&(dpCursor > -1)){
      // Only data protection was found, copy until next data protection (or until end if not found)
      fields.append(unprotectedString(str, dataProtection, escapeChar, &end, start));
      // Check about end of string
      if(end < 0){
        break;
      }
      end += dataProtection.size();
      start = end;
    }else if((sepCursor > -1)&&(dpCursor < 0)){
      // Only separator was found, copy left part
      end = sepCursor;
      fields.append(str.mid(start, end-start));
      start = end + separator.size();
    }else{
      // Separator and data protection found
      if(dpCursor < sepCursor){
        // Data protection was first found, copy until next data protection (or until end if not found)
        fields.append(unprotectedString(str, dataProtection, escapeChar, &end, start));
        // Check about end of string
        if(end < 0){
          break;
        }
        end += dataProtection.size();
        start = end + separator.size();
      }else{
        // Separator was first found, copy left part
        end = sepCursor;
        fields.append(str.mid(start, end-start));
        start = end + separator.size();
      }
    }
  }
  // Append a field if str terminates with a separator
  if((!separator.isEmpty())&&(str.indexOf(separator, end-separator.size())>-1)){
    fields.append("");
  }
  return fields;
}

QString mdtAlgorithms::longestLineInString(const QString & str, const QString & sep)
{
  int len;
  int i;
  int maxLen = 0;
  int maxLenIndex = -1;
  QStringList strList;

  if(str.isEmpty()){
    return QString();
  }
  strList = str.split(sep);
  for(i = 0; i < strList.size(); ++i){
    len = strList.at(i).size();
    if(len > maxLen){
      maxLen = len;
      maxLenIndex = i;
    }
  }
  if(maxLenIndex < 0){
    return QString();
  }
  Q_ASSERT(maxLenIndex < strList.size());

  return strList.at(maxLenIndex);
}

QString mdtAlgorithms::generateString(int length)
{
  QString str(length, QChar());
  static const char table[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

  for(int i = 0; i < length; ++i){
    str[i] = table[qrand() % (sizeof(table)-1)];
  }

  return str;
}

QString mdtAlgorithms::generateString(int length, const QStringList excludeList)
{
  QString str;

  do{
    str = generateString(length);
  }while(excludeList.contains(str));

  return str;
}
