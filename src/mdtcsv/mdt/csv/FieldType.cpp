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
#include "FieldType.h"

namespace mdt{ namespace csv{

FieldType::FieldType()
 : pvType(QMetaType::UnknownType)
{
}

FieldType::FieldType(QMetaType::Type type)
 : pvType(type)
{
  pvName = nameFromType(type);
}

QString FieldType::nameFromType(QMetaType::Type type)
{
  switch(type){
    case QMetaType::Bool:
      return QStringLiteral("Boolean");
    case QMetaType::QDate:
      return QStringLiteral("Date");
    case QMetaType::QTime:
      return QStringLiteral("Time");
    case QMetaType::QDateTime:
      return QStringLiteral("DateTime");
    case QMetaType::Double:
      return QStringLiteral("Double");
    case QMetaType::Int:
      return QStringLiteral("Integer");
    case QMetaType::LongLong:
      return QStringLiteral("LongInteger");
    case QMetaType::UInt:
      return QStringLiteral("UnsignedInteger");
    case QMetaType::ULongLong:
      return QStringLiteral("UnsignedLongInteger");
    case QMetaType::QString:
      return QStringLiteral("String");
    default:
      return QStringLiteral("");
  }
}

std::vector<FieldType> FieldType::getAvailableFieldTypeList()
{
  std::vector<FieldType> ftList;

  ftList.emplace_back(QMetaType::Bool);
  ftList.emplace_back(QMetaType::QDate);
  ftList.emplace_back(QMetaType::QTime);
  ftList.emplace_back(QMetaType::QDateTime);
  ftList.emplace_back(QMetaType::Double);
  ftList.emplace_back(QMetaType::Int);
  ftList.emplace_back(QMetaType::LongLong);
  ftList.emplace_back(QMetaType::UInt);
  ftList.emplace_back(QMetaType::ULongLong);
  ftList.emplace_back(QMetaType::QString);

  return ftList;
}

}} // namespace mdt{ namespace csv{
