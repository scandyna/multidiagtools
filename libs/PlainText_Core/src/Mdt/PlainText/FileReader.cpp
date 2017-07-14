/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "FileReader.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextCodec>
#include <QTextStream>
#include <QCoreApplication>

#define tr(sourceText) QCoreApplication::translate("FileReader", sourceText)

namespace Mdt{ namespace PlainText{

bool FileReader::readFile(const QString& filePath, const QByteArray& encoding, qint64 maxSize, FileReader::EolMode eolMode)
{
  QFile file(filePath);
  QFile::OpenMode openMode;

  if(eolMode == TextMode){
    openMode = QFile::ReadOnly | QFile::Text;
  }else{
    openMode = QFile::ReadOnly;
  }
  // Open file
  if(!file.open(openMode)){
    QFileInfo fileInfo(file);
    const QString msg =
      tr("Could not open file '%1'\nDirectory: '%2'")
      .arg(fileInfo.fileName(), fileInfo.dir().absolutePath());
    mLastError = mdtErrorNew(msg, Mdt::Error::Critical, "FileReader");
    mLastError.stackError(mdtErrorFromQFile(file, "FileReader"));
    if(mAutoCommitError){
      mLastError.commit();
    }
    return false;
  }
  // Check file size
  if(file.size() > maxSize){
    QFileInfo fileInfo(file);
    const QString msg =
      tr(
        "File '%1' exceeds allowed size.\n"
        "File size: '%3' , allowed size: '%4'\n"
        "Directory: '%2'"
      )
      .arg(fileInfo.fileName(),fileInfo.dir().absolutePath())
      .arg(file.size())
      .arg(maxSize);
    mLastError = mdtErrorNew(msg, Mdt::Error::Critical, "FileReader");
    if(mAutoCommitError){
      mLastError.commit();
    }
    return false;
  }
  // Get codec
  auto *codec = QTextCodec::codecForName(encoding);
  if(codec == nullptr){
    const QString msg =
      tr("Could not find a codec for '%1' encoding.")
      .arg(QString::fromLatin1(encoding));
    mLastError = mdtErrorNew(msg, Mdt::Error::Critical, "FileReader");
    if(mAutoCommitError){
      mLastError.commit();
    }
    return false;
  }
  // Read
  QTextStream stream(&file);
  stream.setCodec(codec);
  mStringData = stream.readAll();

  file.close();

  return true;
}

void FileReader::setAutoCommitError(bool autoCommit)
{
  mAutoCommitError = autoCommit;
}

}} // namespace Mdt{ namespace PlainText{
