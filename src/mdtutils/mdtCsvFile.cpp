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
  pvReadLineBufferSize = 8192;
}

mdtCsvFile::~mdtCsvFile()
{
  clear();
}

void mdtCsvFile::close()
{
  pvReadLineBuffer.clear();
  QFile::close();
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

bool mdtCsvFile::writeLine(const QStringList &line, const QString &separator, const QString &dataProtection, const QChar &escapeChar, QString eol)
{
  QString str, item;
  int i;

  // Check if file was open
  if(!isOpen()){
    mdtError e(MDT_FILE_IO_ERROR, "File " + fileName() + " is not open", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtCsvFile");
    e.commit();
    return false;
  }
  // If file was open with Text flag, EOL is allways converted
  if(isTextModeEnabled()){
    eol = "\n";
  }
  // Get items, escape and write
  for(i=0; i<line.size(); i++){
    str += dataProtection;
    if(!escapeChar.isNull()){
      item = line.at(i);
      item.replace(dataProtection, escapeChar + dataProtection);
      str += item;
    }else{
      str += line.at(i);
    }
    str += dataProtection;
    if(i < (line.size()-1)){
      str += separator;
    }
  }
  str += eol;
  if(write(pvCodec->fromUnicode(str)) < 0){
    mdtError e(MDT_FILE_IO_ERROR, "Write error occured on file " + fileName(), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtCsvFile");
    e.commit();
    return false;
  }

  return true;
}

QByteArray mdtCsvFile::readLine(const QString &dataProtection, const QChar &escapeChar, QByteArray eol)
{
  QByteArray buffer;
  QByteArray line;
  QString dpBuffer;
  bool eolFound = false;
  int eolCursor;
  int dpCursor;

  /**
  // Append previously readen part after EOL
  line.append(pvReadLineBuffer);
  pvReadLineBuffer.clear();
  // Check about EOL in previous readen data
  eolCursor = line.indexOf(eol);
  if(eolCursor > -1){
    if(eolCursor == 0){
      eolFound = true;
    }else{
      // Check if eol is in a protected section
      if(!dataProtectionSectionBegins(line.left(line.indexOf(eol)), dataProtection, escapeChar)){
        eolFound = true;
      }
    }
  }
  */
  // Read until EOL found or EOF
  while((!eolFound)&&(!atEnd())){
    ///buffer = read(pvReadLineBufferSize);
    // Check about eol
    eolCursor = buffer.indexOf(eol);
    qDebug() << "eolCursor: " << eolCursor << " , buffer: " << buffer;
    if(eolCursor > -1){
      if(dataProtectionSectionBegins(line, dataProtection, escapeChar)){
        // Search until we find a closing data protrection to confirm that we found a opening one
        readUntilDataProtection(buffer, dpCursor, dataProtection, escapeChar);
        if(dpCursor < 0){
          line.append(buffer);
        }else{
          qDebug() << "End of DP, dpCursor: " << dpCursor << " , buffer: " << buffer;
          line.append(buffer.left(dpCursor+1));
          buffer.remove(0, dpCursor);
        }
        // First in current buffer
        /**
        for(i=0; i<buffer.size(); i++){
          dpBuffer += buffer.at(i);
          if(dpBuffer.right(dataProtection.size()) == dataProtection){
            break;
          }
        }
        */
        // Read more if needed
        ///while((dpBuffer != dataProtection)&&(!atEnd())){
        /**
        if(dpBuffer.right(dataProtection.size()) != dataProtection){
          while(!atEnd()){
            dpBuffer = read(dataProtection.size());
            if(dpBuffer != dataProtection){
              buffer.append(dpBuffer.toAscii());
            }else{
              break;
            }
          }
        }
        */
        ///line.append(buffer);
      }else{
        ///line.append(buffer.left(eolCursor+1));
        eolFound = true;
      }
    }else{
      ///line.append(buffer);
      buffer = read(pvReadLineBufferSize);
    }
    /**
    if( (eolCursor > -1) && (!dataProtectionSectionBegins(line, dataProtection, escapeChar)) ){
      eolFound = true;
    }else{
      line.append(buffer);
    }
    */
  }
  if(eolFound){
    ///qDebug() << "eolFound, buffer: " << buffer;
    line.append(buffer.left(eolCursor));
    pvReadLineBuffer = buffer.right(buffer.size()-eolCursor-1);
  }
  /**
  if(eolFound){
    // Split left and right part of EOL
    eolCursor = line.indexOf(eol);
    if(line.size() > 1){
      pvReadLineBuffer = line.right(line.size()-eolCursor-1);
    }else{
      pvReadLineBuffer.clear();
    }
    line.remove(eolCursor, line.size()-eolCursor);
  }
  */
  qDebug() << "line: " << line << " , pvReadLineBuffer: " << pvReadLineBuffer;

  return line;
}

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

void mdtCsvFile::setReadLineBufferSize(int size)
{
  pvReadLineBufferSize = size;
}

bool mdtCsvFile::dataProtectionSectionBegins(const QByteArray &line, const QString &dataProtection, const QChar &escapeChar)
{
  bool escapeEqualProtection = QString(escapeChar) == dataProtection;
  int dpCursor = -1;
  int dpCount = 0;
  int start = 0;
  int end = 0;

  // If no data protection is given, we are never in a protected section
  if(dataProtection.isEmpty()){
    return false;
  }
  // Search..
  while(end < line.size()){
    // Get index of data protection
    dpCursor = line.indexOf(dataProtection, start);
    ///qDebug() << "dpCursor: " << dpCursor << " , line: " << line;
    if(dpCursor < 0){
      // Finish parsing
      break;
    }else{
      if(escapeEqualProtection){
        if(dpCursor < (line.size()-1)){
          // check about escaped sequence
          if(line.at(dpCursor+1) == escapeChar){
            // Escaped sequence
            end = dpCursor + 1;
            start = dpCursor + dataProtection.size() + 1;
          }else{
            // No escaped
            end = dpCursor;
            start = dpCursor + dataProtection.size();
            dpCount++;
          }
        }else{
          end = dpCursor;
          break;
        }
      }else{
        if(dpCursor == 0){
          // Just move start cursor after protection
          start = dpCursor + dataProtection.size();
          dpCount++;
        }else{
          if(line.at(dpCursor-1) == escapeChar){
            // Escaped sequence
            end = dpCursor-1;
            start = dpCursor + dataProtection.size();
          }else{
            end = dpCursor;
            start = dpCursor + dataProtection.size();
            dpCount++;
          }
        }
      }
    }
  }
  // Result
  qDebug() << "DPCHECK, dpCount: " << dpCount;
  qDebug() << "In protected section ? " << (dpCount%2) << " , line: " << line;
  return (dpCount%2);

  /**
  if(dpCount == 0){
    qDebug() << "NOT in protected section, line: " << line;
    return false;
  }else{
    qDebug() << "In protected section ? " << (dpCount%2) << " , line: " << line;
    return (dpCount%2);
  }
  */
}

void mdtCsvFile::readUntilDataProtection(QByteArray &buffer, int &dpIndex, const QString &dataProtection, const QChar &escapeChar)
{
  bool dpFound = false;

  while((!dpFound)&&(!atEnd())){
    dpIndex = buffer.indexOf(dataProtection);
    if(dpIndex > -1){
      dpFound = true;
    }else{
      buffer.append(read(pvReadLineBufferSize));
    }
  }
}
