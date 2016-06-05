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
#include "DriverImplementationInterface.h"
#include <QStringList>
#include <QStringBuilder>
#include <QObject>

namespace Mdt{ namespace Sql{ namespace Schema{

FieldTypeList DriverImplementationInterface::getAvailableFieldTypeList() const
{
  FieldTypeList list;

  list.append(FieldType::Boolean);
  list.append(FieldType::Integer);
  list.append(FieldType::Float);
  list.append(FieldType::Double);
  list.append(FieldType::Varchar);
  list.append(FieldType::Date);
  list.append(FieldType::Time);
  list.append(FieldType::DateTime);

  return list;
}

FieldType DriverImplementationInterface::fieldTypeFromQMetaType(QMetaType::Type qmt) const
{
  switch(qmt){
    case QMetaType::Bool:
      return FieldType::Boolean;
    case QMetaType::Int:
      return FieldType::Integer;
    case QMetaType::Float:
      return FieldType::Float;
    case QMetaType::Double:
      return FieldType::Double;
    case QMetaType::QString:
      return FieldType::Varchar;
    case QMetaType::QDate:
      return FieldType::Date;
    case QMetaType::QTime:
      return FieldType::Time;
    case QMetaType::QDateTime:
      return FieldType::DateTime;
    default:
      break;
  }
  return FieldType::UnknownType;
}

QMetaType::Type DriverImplementationInterface::fieldTypeToQMetaType(FieldType ft) const
{
  switch(ft){
    case FieldType::Boolean:
      return QMetaType::Bool;
    case FieldType::Integer:
      return QMetaType::Int;
    case FieldType::Float:
      return QMetaType::Float;
    case FieldType::Double:
      return QMetaType::Double;
    case FieldType::Varchar:
      return QMetaType::QString;
    case FieldType::Date:
      return QMetaType::QDate;
    case FieldType::Time:
      return QMetaType::QTime;
    case FieldType::DateTime:
      return QMetaType::QDateTime;
    case FieldType::UnknownType:
      return QMetaType::UnknownType;
  }
  return QMetaType::UnknownType;
}

QString DriverImplementationInterface::getPrimaryKeyDefinition(const PrimaryKey & pk) const
{
  QString sql;
  const auto rawFieldNameList = pk.fieldNameList();
  QStringList escapedFieldNameList;

  for(const auto & fieldName : rawFieldNameList){
    escapedFieldNameList.append( escapeFieldName(fieldName) );
  }
  sql = QStringLiteral("PRIMARY KEY (") % escapedFieldNameList.join(',') % QStringLiteral(")");

  return sql;
}

QString DriverImplementationInterface::getSqlToCreateIndex(const Index & index) const
{
  QString sql;


  return sql;
}

QString DriverImplementationInterface::getSqlToDropIndex(const Index & index) const
{
  QString sql;
  ///const QStringList fieldNameList = index.


  return sql;
}

QString DriverImplementationInterface::escapeFieldName(const QString & fieldName) const
{
  return qsqlDriver()->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QString DriverImplementationInterface::escapeTableName(const QString & tableName) const
{
  return qsqlDriver()->escapeIdentifier(tableName, QSqlDriver::TableName);
}

QString DriverImplementationInterface::tr(const char* sourceText)
{
  return QObject::tr(sourceText);
}


}}} // namespace Mdt{ namespace Sql{ namespace Schema{
