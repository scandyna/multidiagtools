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
#include <cctype>
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

QByteArray mdtCsvFile::readLine(const QString &dataProtection, const QString &comment, const QChar &escapeChar, QByteArray eol)
{
  QByteArray buffer;
  QByteArray line;
  QString dpBuffer;
  bool eolFound = false;
  int eolCursor;
  int dpCursor;

  buffer = pvReadLineBuffer;
  
  /// Search if a part of buffer is start of beginning commented line
  /// If true, read from comment until EOL (without worry about dataProtection
  /// Keep left part of buffer and process
  
  // Read until EOL found or EOF
  while(!eolFound){
    qDebug() << "line (0): " << line << " , buffer (0): " << buffer;
    eolCursor = buffer.indexOf(eol);
    qDebug() << "EOL cursor: " << eolCursor;
    if(eolCursor < 0){
      qDebug() << "buffer (1): " << buffer;
      if(atEnd()){
        line.append(buffer);
        buffer.clear();
        // Now check that line is not commented
        if(indexOfCommentedLineBeginning(line, comment) > -1){
          line.clear();
        }
        break;
      }
      buffer.append(read(pvReadLineBufferSize));
    }else{
      // A EOL was found, check if it is in a data protection or not
      qDebug() << "EOL found ? (0) , line: " << line << " , buffer: " << buffer;
      line.append(buffer.left(eolCursor));
      buffer.remove(0, eolCursor);
      qDebug() << "EOL found ? (1) , line: " << line << " , buffer: " << buffer;
      if(dataProtectionSectionBegins(line, dataProtection, escapeChar)){
        // Search until we find a closing data protrection to confirm that we found a opening one
        qDebug() << "Read until DP ...";
        readUntilDataProtection(buffer, dpCursor, dataProtection, escapeChar);
        if(dpCursor < 0){
          qDebug() << "Parse error.. , buffer: " << buffer;
          return "<PARSE ERROR: missing closing " + dataProtection.toAscii() + ">";
        }else{
          qDebug() << "-> DONE, line (A): " << line << " , buffer: " << buffer;
          line.append(buffer.left(dpCursor+dataProtection.size()));
          buffer.remove(0, dpCursor+dataProtection.size());
          qDebug() << "-> DONE, line (B): " << line << " , buffer: " << buffer;
        }
      }else{
        // Here we found a EOL in a unprotected section.
        qDebug() << "buffer: " << buffer << " , removing " << eol.size() << " items";
        buffer.remove(0, eol.size());
        qDebug() << "-> buffer: " << buffer;
        // Now check that line is not commented
        if(indexOfCommentedLineBeginning(line, comment) > -1){
          line.clear();
        }else{
          eolFound = true;
        }
      }
    }
  }
  pvReadLineBuffer = buffer;
  qDebug() << "EOL found (END) , line: " << line << " , pvReadLineBuffer: " << pvReadLineBuffer;

  return line;
}

bool mdtCsvFile::hasMoreLines() const
{
  if((pvReadLineBuffer.size() > 0)||(!atEnd())){
    return true;
  }
  return false;
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
  // Read file and parse each line
  while(hasMoreLines()){
    line = pvCodec->toUnicode(readLine(dataProtection, comment, escapeChar, eol.toAscii()));
    // Check if we have a commented line
    if(line.size() > 0){
      // Parse the line
      pvLines.append(mdtAlgorithms::splitString(line, separator, dataProtection, escapeChar));
    }
  }

  return true;

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

  while((!dpFound)/*&&(!atEnd())*/){
    dpIndex = buffer.indexOf(dataProtection);
    if(dpIndex > -1){
      dpFound = true;
    }else{
      if(atEnd()){
        buffer.append(pvReadLineBuffer);
        pvReadLineBuffer.clear();
        dpIndex = buffer.indexOf(dataProtection);
        break;
      }else{
        buffer.append(read(pvReadLineBufferSize));
      }
    }
  }
}

int mdtCsvFile::indexOfCommentedLineBeginning(const QByteArray &line, const QString &comment)
{
  int i;

  if(comment.isEmpty()){
    return -1;
  }
  for(i=0; i<line.size(); i++){
    if(!isspace(line.at(i))){
      // We are at first char position
      if(line.indexOf(comment, i) == i){
        return i;
      }else{
        return -1;
      }
    }
  }

  return -1;
}
