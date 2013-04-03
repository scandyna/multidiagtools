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

//#include <QDebug>

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
  QByteArray line;

  if(readLine(line, dataProtection, comment, escapeChar, eol)){
    return line;
  }

  return "";
}

bool mdtCsvFile::readLine(QByteArray &line, const QString &dataProtection, const QString &comment, const QChar &escapeChar, QByteArray eol)
{
  QByteArray buffer;
  bool eolFound = false;
  int eolCursor;
  int dpCursor;

  buffer = pvReadLineBuffer;
  // Read until EOL found or EOF
  while(!eolFound){
    eolCursor = buffer.indexOf(eol);
    if(eolCursor < 0){
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
      line.append(buffer.left(eolCursor));
      buffer.remove(0, eolCursor);
      if(dataProtectionSectionBegins(line, dataProtection, escapeChar)){
        // Search until we find a closing data protrection to confirm that we found a opening one
        readUntilDataProtection(buffer, dpCursor, dataProtection, escapeChar);
        if(dpCursor < 0){
          mdtError e(MDT_FILE_IO_ERROR, "In file " + fileName() + " : format error (missing a closing " + dataProtection + ")", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtCsvFile");
          e.commit();
          return false;
        }else{
          line.append(buffer.left(dpCursor+dataProtection.size()));
          buffer.remove(0, dpCursor+dataProtection.size());
        }
      }else{
        // Here we found a EOL in a unprotected section.
        buffer.remove(0, eol.size());
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

  return true;
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

  QString line;
  QByteArray lineBa;

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
    lineBa.clear();
    if(readLine(lineBa, dataProtection, comment, escapeChar, eol.toAscii())){
      line = pvCodec->toUnicode(lineBa);
      if(line.size() > 0){
        // Parse the line
        pvLines.append(mdtAlgorithms::splitString(line, separator, dataProtection, escapeChar));
      }
    }
  }

  return true;
}

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
  QByteArray escapedDataProtection;
  int dpCount;
  int esDpCount;

  // If no data protection is given, we are never in a protected section
  if(dataProtection.isEmpty()){
    return false;
  }
  // Build the escaped data protection version
  escapedDataProtection.append(escapeChar.toAscii());
  escapedDataProtection += dataProtection.toAscii();
  // Count data protection occurences
  dpCount = line.count(dataProtection.toAscii());
  // Count escaped data protection occurences
  if(escapeEqualProtection){
    esDpCount = 0;
  }else{
    esDpCount = line.count(escapedDataProtection);
  }

  return ((dpCount-esDpCount)%2);
}

void mdtCsvFile::readUntilDataProtection(QByteArray &buffer, int &dpIndex, const QString &dataProtection, const QChar &escapeChar)
{
  bool dpFound = false;
  bool escapeEqualProtection = QString(escapeChar) == dataProtection;
  char es = escapeChar.toAscii();
  int start = 0;

  while((!dpFound)/*&&(!atEnd())*/){
    // Search data protection
    dpIndex = buffer.indexOf(dataProtection, start);
    if(escapeEqualProtection){
      if(dpIndex > -1){
        // We need enougth elements to check about escape
        if(buffer.size() > (dpIndex+1)){
          // Check that data protection is not escaped
          if(buffer.at(dpIndex+1) == es){
            // Escaped data protection
            start = dpIndex+2;
            dpIndex = -1;
          }
        }else{
          // Not enougth data, read more
          dpIndex = -1;
        }
      }
    }
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
