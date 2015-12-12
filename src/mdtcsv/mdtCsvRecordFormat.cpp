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
#include "mdtCsvRecordFormat.h"
#include <QObject>

bool mdtCsvRecordFormat::formatRecord(mdtCsvRecord& record)
{
  Q_ASSERT(record.count() == fieldCount());

  for(int i = 0; i < record.count(); ++i){
    if(hasType(i)){
      if(!convert(record.columnDataList[i], pvFieldTypes.at(i))){
        return false;
      }
    }
  }

  return true;
}


void mdtCsvRecordFormat::clearFormats()
{
  for(auto & type : pvFieldTypes){
    type = QMetaType::UnknownType;
  }
}

bool mdtCsvRecordFormat::convert(QVariant & data, QMetaType::Type type)
{
  const QString str = data.toString();

  /*
   * If data is empty, we want to convert to a null QVariant of requested type.
   * The best way to do this is using QVariant::convert() and ignore return value.
   * See Qt's QVariant documentation about this.
   */
  if(str.isEmpty()){
    data.convert(type);
    return true;
  }
  // Convert
  if(!data.convert(type)){
    QString msg = tr("Convertion of data '") + str.left(50) \
                  + tr("' to type '") + QMetaType::typeName(type) \
                  + tr("' failed.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtCsvRecordFormat");
    pvLastError.commit();
    return false;
  }

  return true;
}

QString mdtCsvRecordFormat::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}
