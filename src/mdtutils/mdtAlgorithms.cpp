/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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

//#include <QDebug>

QStringList mdtAlgorithms::sortStringListWithNumericEnd(QStringList &list)
{
  qSort(list.begin(), list.end(), stringWithNumericEndLessThan);

  return list;
}

QString mdtAlgorithms::extractNumericPartAtEnd(const QString &str)
{
  QString num;

  if(str.size() < 1){
    return num;
  }
  for(int i=str.size()-1; i>=0; --i){
    if(str.at(i).isDigit()){
      num.prepend(str.at(i));
    }else{
      if(str.at(i) != ' '){
        break;
      }
    }
  }

  return num;
}

bool mdtAlgorithms::stringWithNumericEndLessThan(QString str1, QString str2)
{
  // Empty string
  if(str1.isEmpty()&&(!str2.isEmpty())){
    return true;
  }
  if((!str1.isEmpty()) && str2.isEmpty()){
    return false;
  }
  if((str1.isEmpty())&&(str2.isEmpty())){
    return false;
  }

  // Extract end numerical part
  QString strNum1, strNum2;
  strNum1 = extractNumericPartAtEnd(str1);
  strNum2 = extractNumericPartAtEnd(str2);

  // Extract begin part
  int len;
  len = str1.size() - strNum1.size();
  if(len >= 0){
    str1 = str1.mid(0, len);
  }
  len = str2.size() - strNum2.size();
  if(len >= 0){
    str2 = str2.mid(0, len);
  }
  // Compare the begin part
  if(str1 < str2){
    return true;
  }
  if(str2 < str1){
    return false;
  }

  // Begin part of str1 and str2 are the same, check on numerical end part
  int num1, num2;
  bool ok = false;

  // Error or empty string on numeric part
  if(strNum1.isEmpty()&&(!strNum2.isEmpty())){
    return true;
  }
  if((!strNum1.isEmpty()) && strNum2.isEmpty()){
    return false;
  }
  if((strNum1.isEmpty())&&(strNum2.isEmpty())){
    return false;
  }
  num1 = strNum1.toInt(&ok);
  if(!ok){
    return false;
  }
  ok = false;
  num2 = strNum2.toInt(&ok);
  if(!ok){
    return false;
  }
  // Result
  return (num1 < num2);
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
