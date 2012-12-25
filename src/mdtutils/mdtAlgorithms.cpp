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

#include <QDebug>

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

/*
    QStringList list;
    int start = 0;
    int extra = 0;
    int end;
    while ((end = indexOf(sep, start + extra, cs)) != -1) {
        if (start != end || behavior == KeepEmptyParts)
            list.append(mid(start, end - start));
        start = end + sep.size();
        extra = (sep.size() == 0 ? 1 : 0);
    }
    if (start != size() || behavior == KeepEmptyParts)
        list.append(mid(start));
    return list;
    */

QStringList mdtAlgorithms::splitString(const QString &str, const QString &separator, const QString &dataProtection)
{
  bool parserEnabled;   // Used for data protection ( "data";"other ; data";0123 )
  QString tmpStr;
  int sepCursor = 0;    // Cursor in str string for separator
  int dpCursor = 0;     // Cursor in str string for dataProtection
  QString field;
  QStringList fields;

  qDebug() << "Input: str: " << str << " , separator: " << separator << " , DP: " << dataProtection;
  
  int start = 0;
  int extra;
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
    qDebug() << "str: " << str << " , start: " << start << " , end: " << end << " , str size: " << str.size() <<  " , sepCursor: " << sepCursor << " , dpCursor: " << dpCursor;
    if((sepCursor < 0)&&(dpCursor < 0)){
      // No keyword found, copy remaining string
      end = str.size();
      if(start != end){
        qDebug() << "A: Copy from " << start << " , n: " << end-start << "(" << str.mid(start, end-start) << ")";
        fields.append(str.mid(start, end-start));
      }
    }else if((sepCursor < 0)&&(dpCursor > -1)){
      // Only data protection was found, copy until next data protection (or until end if not found)
      start += dataProtection.size();
      if(!dataProtection.isEmpty()){
        dpCursor =  str.indexOf(dataProtection, start);
      }else{
        dpCursor = -1;
      }
      if(dpCursor < 0){
        end = str.size();
      }else{
        end = dpCursor;
      }
      qDebug() << "B: Copy from " << start << " , n: " << end-start << "(" << str.mid(start, end-start) << ")";
      fields.append(str.mid(start, end-start));
      end += dataProtection.size();
      start = end + dataProtection.size();
    }else if((sepCursor > -1)&&(dpCursor < 0)){
      // Only separator was found, copy left part
      end = sepCursor;
      qDebug() << "C: Copy from " << start << " , n: " << end-start << "(" << str.mid(start, end-start) << ")";
      fields.append(str.mid(start, end-start));
      start = end + separator.size();
    }else{
      // Separator and data protection found
      if(dpCursor < sepCursor){
        // Data protection was first found, copy until next data protection (or until end if not found)
        start += dataProtection.size();
        if(!dataProtection.isEmpty()){
          dpCursor =  str.indexOf(dataProtection, start);
        }else{
          dpCursor = -1;
        }
        if(dpCursor < 0){
          end = str.size();
        }else{
          end = dpCursor;
        }
        qDebug() << "D1: Copy from " << start << " , n: " << end-start << "(" << str.mid(start, end-start) << ")";
        fields.append(str.mid(start, end-start));
        end += dataProtection.size();
        start = end + dataProtection.size();
      }else{
        // Separator was first found, copy left part
        end = sepCursor;
        qDebug() << "D2: Copy from " << start << " , n: " << end-start << "(" << str.mid(start, end-start) << ")";
        fields.append(str.mid(start, end-start));
        start = end + separator.size();
      }
    }
  }
  // Append a field if str terminates with a separator
  if((!separator.isEmpty())&&(str.indexOf(separator, end-separator.size())>-1)){
    qDebug() << "Z: indexOf(" << separator << "," << end-1 << "): " << str.indexOf(separator, end-1);
    fields.append("");
  }
  return fields;
  
  
  if(separator.size() == 0){
    extra = 1;
  }else{
    extra = 0;
  }

  // If separator is empty, nothing is to parse, simply return a 1 item list
  if(separator.isEmpty()){
    if(!str.isEmpty()){
      fields << str;
    }
    return fields;
  }

  // As default, parser is enabled
  parserEnabled = true;

  while(end > -1){
    if(parserEnabled){
      // Find first occurence of data protection and separator
      if(!dataProtection.isEmpty()){
        dpCursor = str.indexOf(dataProtection, start + 0);
      }else{
        dpCursor = -1;
      }
      if(!separator.isEmpty()){
        sepCursor = str.indexOf(separator, start + 0);
      }else{
        sepCursor = -1;
      }
      // Calculate end cursor
      qDebug() << "Parser ON , dpCursor: " << dpCursor << " , sepCursor: " << sepCursor;
      if(dpCursor == sepCursor){
        end = dpCursor;
      }else if(dpCursor < 0){
        end = sepCursor;
      }else if(sepCursor < 0){
        end = dpCursor;
      }else if(dpCursor < sepCursor){
        // Fisrt occurence is a data protection, diseable parser
        end = dpCursor;
        parserEnabled = false;
      }else{
        end = sepCursor;
      }
      qDebug() << "Parser ON , end: " << end;
      // Copy data
      ///if(start < str.size()){
        qDebug() << "Parser ON , Copy data from " << start << " , size " << end-start << " (" << str.mid(start, end - start) << ")";
        fields.append(str.mid(start, end - start));
      ///}
    }else{
      // Find first occurence on data protection
      if(!dataProtection.isEmpty()){
        dpCursor = str.indexOf(dataProtection, start + 0);
      }else{
        dpCursor = -1;
      }
      end = dpCursor;
      // Copy data
      if(start < str.size()){
        qDebug() << "Parser OFF , Copy data from " << start << " , size " << end-start << " (" << str.mid(start, end - start) << ")";
        fields.append(str.mid(start, end - start));
      }
      // Re-enable parser if we are at end of data protection section
      if(dpCursor > -1){
        parserEnabled = true;
      }
    }

    // Calc the new start position
      if(parserEnabled){
        start = end + separator.size();
      }else{
        start = end + dataProtection.size();
      }
      qDebug() << "NEW start: " << start;
  }
  
  return fields;
  
  
  // If separator is empty, nothing is to parse, simply return a 1 item list
  if(separator.isEmpty()){
    if(!str.isEmpty()){
      fields << str;
    }
    return fields;
  }
  // As default, parser is enabled
  parserEnabled = true;

  ///fields.clear();
  tmpStr = str;
  while(tmpStr.size() > 0){
    qDebug() << "tmpStr: " << tmpStr;
    field.clear();
    // Find first occurence of dataProtection in line string
    if(dataProtection.size() > 0){
      dpCursor = tmpStr.indexOf(dataProtection);
    }else{
      dpCursor = -1;
    }
    // If parser is enabled, react to keys
    if(parserEnabled){
      // Find first occurence of separator in line string
      sepCursor = tmpStr.indexOf(separator);
      qDebug() << "sepCursor: " << sepCursor;
      // See if we have a data protection
      if(dpCursor >= 0){
        // See if we have a separator
        if(sepCursor >= 0){
          // Have both, see if data protection is first occurence
          if(dpCursor < sepCursor){
            // Begin of data protection - Diseable parser and remove data protection
            parserEnabled = false;
            tmpStr.remove(0, dataProtection.size());
          }else{
            // Copy into field
            field.append(tmpStr.left(sepCursor));
            ///fields << pvCodec->toUnicode(field);
            fields << field;
            // Remove separator
            tmpStr.remove(0, sepCursor+separator.size());
          }
        }else{
          // Have only data protection - remove protection and put data into field
          tmpStr.remove(0, dataProtection.size());
          Q_ASSERT((tmpStr.size()-dataProtection.size()) >= 0);
          field.append(tmpStr.left(tmpStr.size()-dataProtection.size()));
          ///fields << pvCodec->toUnicode(field);
          fields << field;
          break;
        }
      }else{
        // Data protection not found , see about separator
        if(sepCursor >= 0){
          // Copy into field
          field.append(tmpStr.left(sepCursor));
          ///fields << pvCodec->toUnicode(field);
          qDebug() << "field: " << field;
          fields << field;
          // Remove separator
          tmpStr.remove(0, sepCursor+separator.size());
        }else{
          // Nothing found - Just copy all data into field
          field.append(tmpStr);
          ///fields << pvCodec->toUnicode(field);
          fields << field;
          break;
        }
      }
    }else{
      // Parser diseabled, see if we must enable it
      if(dpCursor >= 0){
        parserEnabled = true;
        // Store data -> cursor
        field.append(tmpStr.left(dpCursor));
        ///fields << pvCodec->toUnicode(field);
        fields << field;
        tmpStr.remove(0, dpCursor+dataProtection.size()+separator.size());
      }else{
        // Simply store the data into field
        field.append(tmpStr);
        ///fields << pvCodec->toUnicode(field);
        fields << field;
        break;
      }
    }
  }

  return fields;
}
