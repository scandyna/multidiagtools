/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "mdtCsvSourceInfo.h"
#include <QLatin1String>

void mdtCsvSourceInfo::setFormat(const mdtCsvRecordFormat& format)
{
  Q_ASSERT(format.fieldCount() == pvHeader.count());
  pvRecordFormat = format;
}

void mdtCsvSourceInfo::setSourceName(const QString & name)
{
  pvSourceName = name;
}

void mdtCsvSourceInfo::setCsvSettings(const mdtCsvParserSettings &settings)
{
  pvCsvSettings = settings;
}

void mdtCsvSourceInfo::setHeader(const mdtCsvRecord & hdr)
{
  pvHeader = hdr;
}

int mdtCsvSourceInfo::fieldIndex(const QString & name) const
{
  return pvHeader.columnDataList.indexOf(name);
}

QString mdtCsvSourceInfo::fieldTypeName(int index) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvRecordFormat.fieldCount());

  auto type = pvRecordFormat.fieldType(index);
  switch(type){
    case QMetaType::QString:
      return QLatin1String("String");
    case QMetaType::Bool:
      return QLatin1String("Boolean");
    case QMetaType::Int:
      return QLatin1String("Integer");
    default:
      return QMetaType::typeName(type);
  }
}

QStringList mdtCsvSourceInfo::getFieldNameList() const
{
  QStringList fieldNames;

  for(const auto & var : pvHeader.columnDataList){
    fieldNames.append(var.toString());
  }

  return fieldNames;
}
