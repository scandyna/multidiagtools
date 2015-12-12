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
#include "mdtCsvFileGenerator.h"
#include "mdtCsvStringGenerator.h"
#include <QObject>
#include <QDir>
#include <QTextCodec>
#include <QTextEncoder>

mdtCsvFileGenerator::mdtCsvFileGenerator(const mdtCsvGeneratorSettings & csvSettings)
 : pvCsvSettings(csvSettings)
{
}

mdtCsvFileGenerator::~mdtCsvFileGenerator()
{
}

bool mdtCsvFileGenerator::openFile(const QFileInfo & fileInfo, const QByteArray & encoding)
{
  // Close possibly previously open file
  if(pvFile.isOpen()){
    if(!closeFile()){
      return false;
    }
  }
  Q_ASSERT(!pvEncoder);
  Q_ASSERT(!pvFile.isOpen());
  /*
   * Find a codec for requested encodeing
   * Note: we not have to manage codec lifetime,
   *       see QTextCodec::~QTextCodec()
   */
  auto *codec = QTextCodec::codecForName(encoding);
  if(codec == nullptr){
    QString msg = tr("Could not find a codec for requested encoding '") + encoding + tr("'");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtCsvFileGenerator");
    pvLastError.commit();
    return false;
  }
  // Open CSV file
  pvFile.setFileName(fileInfo.absoluteFilePath());
  if(!pvFile.open(QIODevice::WriteOnly)){
    QString msg = tr("Could not open file '") + fileInfo.fileName() + tr("'\n") \
                  + tr("Directory: '") + fileInfo.dir().absolutePath() + tr("'");
    pvLastError.setError(msg, mdtError::Error);
    pvLastError.setSystemError(0, pvFile.errorString());
    MDT_ERROR_SET_SRC(pvLastError, "mdtCsvFileGenerator");
    pvLastError.commit();
    return false;
  }
  // Get encoder
  Q_ASSERT(codec != nullptr);
  pvEncoder.reset(codec->makeEncoder());

  return true;
}

bool mdtCsvFileGenerator::closeFile()
{
  pvEncoder.reset();
  if(pvFile.isOpen()){
    if(!pvFile.flush()){
      QFileInfo fileInfo(pvFile.fileName());
      QString msg = tr("Could not save file '") + fileInfo.fileName() + tr("'\n") \
                    + tr("Directory: '") + fileInfo.dir().absolutePath() + tr("'");
      pvLastError.setError(msg, mdtError::Error);
      pvLastError.setSystemError(0, pvFile.errorString());
      MDT_ERROR_SET_SRC(pvLastError, "mdtCsvFileGenerator");
      pvLastError.commit();
      return false;
    }
    pvFile.close();
  }
  return true;
}

bool mdtCsvFileGenerator::writeLine(const mdtCsvRecord & record)
{
  Q_ASSERT(pvEncoder);

  mdtCsvStringGenerator generator(pvCsvSettings);
  const QString csv = generator.getCsvString(record);

  if(pvFile.write(pvEncoder->fromUnicode(csv)) < 0){
    QFileInfo fileInfo(pvFile.fileName());
    QString msg = tr("Could write to file '") + fileInfo.fileName() + tr("'\n") \
                  + tr("Directory: '") + fileInfo.dir().absolutePath() + tr("'");
    pvLastError.setError(msg, mdtError::Error);
    pvLastError.setSystemError(0, pvFile.errorString());
    MDT_ERROR_SET_SRC(pvLastError, "mdtCsvFileGenerator");
    pvLastError.commit();
    return false;
  }

  return true;
}

QString mdtCsvFileGenerator::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}
