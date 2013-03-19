/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtCsvFile.h"
#include "mdtError.h"
#include "mdtAlgorithms.h"

#include <QDebug>

mdtCsvFile::mdtCsvFile(QObject *parent, const QByteArray &fileEncoding)
 : QFile(parent)
{
  pvCodec = QTextCodec::codecForName(fileEncoding);
  Q_ASSERT(pvCodec != 0);
}

mdtCsvFile::~mdtCsvFile()
{
  clear();
}

/**
bool mdtCsvFile::readLines(QByteArray separator, QByteArray dataProtection, QByteArray comment)
{
  Q_ASSERT(separator != dataProtection);
  Q_ASSERT(separator != comment);
  Q_ASSERT(dataProtection != comment);

  QByteArray line;

  // Check if file was open
  if(!isOpen()){
    setErrorString(tr("mdtCsvFile::readLines(): file is not open"));
    return false;
  }
  // Clear previous results
  clear();
  // Travel the file
  while(!atEnd()){
    // Read a line
    line = readLine();
    // Remove begin and end whit space, end of line, etc...
    line = line.trimmed();
    if(line.size() > 0){
      // Check about comments
      if((comment.size() > 0)&&(line.startsWith(comment))){
        continue;
      }
      // Parse the block
      pvLines << parseLine(line, separator, dataProtection);
    }
  }

  return true;
}
*/

///splitString(const QString &str, const QString &separator, const QString &dataProtection, const QChar &escapeChar = QChar());

bool mdtCsvFile::readLines(const QString &separator, const QString &dataProtection, const QString &comment, const QChar &escapeChar, QString eol)
{
  Q_ASSERT(separator != dataProtection);
  Q_ASSERT(separator != comment);
  Q_ASSERT(dataProtection != comment);

  QStringList lines;
  QString line;
  QStringList items;
  int i;

  // Check if file was open
  if(!isOpen()){
    mdtError e(MDT_FILE_IO_ERROR, "File " + fileName() + " is not open", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtCsvFile");
    e.commit();
    return false;
  }
  // Clear previous results
  clear();
  // If file was open with Text flag, EOL is allready converted
  if(isTextModeEnabled()){
    eol = "\n";
  }
  // Read the file and separate lines (not very efficient...)
  ///lines = mdtAlgorithms::splitString(pvCodec->toUnicode(readAll()), eol, dataProtection, escapeChar);
  lines = mdtAlgorithms::splitString(pvCodec->toUnicode(readAll()), eol, "", escapeChar);
  for(i=0; i<lines.size(); i++){
    line = lines.at(i).trimmed();
    // Check if we have a commented line
    if(line.size() > 0){
      if((!comment.isEmpty())&&(line.startsWith(comment))){
        continue;
      }
      // Parse the line
      pvLines.append(mdtAlgorithms::splitString(line, separator, dataProtection, escapeChar));
    }
  }

  return true;
}

/**
QStringList &mdtCsvFile::parseLine(const QByteArray &line, const QByteArray &separator, const QByteArray &dataProtection)
{
  bool parserEnabled;   // Used for data protection ( "data";"other ; data";0123 )
  QByteArray tmpStr;
  int sepCursor = 0;    // Cursor in line string for separator
  int dpCursor = 0;     // Cursor in line string for dataProtection
  QByteArray field;

  // As default, parser is enabled
  parserEnabled = true;

  pvFields.clear();
  tmpStr = line;
  while(tmpStr.size() > 0){
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
            pvFields << pvCodec->toUnicode(field);
            // Remove separator
            tmpStr.remove(0, sepCursor+separator.size());
          }
        }else{
          // Have only data protection - remove protection and put data into field
          tmpStr.remove(0, dataProtection.size());
          Q_ASSERT((tmpStr.size()-dataProtection.size()) >= 0);
          field.append(tmpStr.left(tmpStr.size()-dataProtection.size()));
          pvFields << pvCodec->toUnicode(field);
          break;
        }
      }else{
        // Data protection not found , see about separator
        if(sepCursor >= 0){
          // Copy into field
          field.append(tmpStr.left(sepCursor));
          pvFields << pvCodec->toUnicode(field);
          // Remove separator
          tmpStr.remove(0, sepCursor+separator.size());
        }else{
          // Nothing found - Just copy all data into field
          field.append(tmpStr);
          pvFields << pvCodec->toUnicode(field);
          break;
        }
      }
    }else{
      // Parser diseabled, see if we must enable it
      if(dpCursor >= 0){
        parserEnabled = true;
        // Store data -> cursor
        field.append(tmpStr.left(dpCursor));
        pvFields << pvCodec->toUnicode(field);
        tmpStr.remove(0, dpCursor+dataProtection.size()+separator.size());
      }else{
        // Simply store the data into field
        field.append(tmpStr);
        pvFields << pvCodec->toUnicode(field);
        break;
      }
    }
  }

  return pvFields;
}
*/

void mdtCsvFile::clear()
{
  pvLines.clear();
}

QString mdtCsvFile::valueAt(int line, int column)
{
  if((line < 0)||(column < 0)){
    return "";
  }
  if(pvLines.size() < 1){
    return "";
  }
  if(line >= pvLines.size()){
    return "";
  }
  if(column >= pvLines.at(line).size()){
    return "";
  }
  return pvLines.at(line).at(column);
}

QList<QStringList> &mdtCsvFile::lines()
{
  return pvLines;
}

