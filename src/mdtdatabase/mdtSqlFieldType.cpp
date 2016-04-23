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
#include "mdtSqlFieldType.h"
#include <QStringList>
#include <QLatin1String>
#include <utility>

//#include <QDebug>

QMetaType::Type mdtSqlFieldType::toQMetaType(mdtSqlFieldType::Type sqlType)
{
  switch(sqlType){
    case mdtSqlFieldType::UnknownType:
      return QMetaType::UnknownType;
    case mdtSqlFieldType::Boolean:
      return QMetaType::Bool;
    case mdtSqlFieldType::Integer:
      return QMetaType::Int;
    case mdtSqlFieldType::Float:
      return QMetaType::Float;
    case mdtSqlFieldType::Double:
      return QMetaType::Double;
    case mdtSqlFieldType::Varchar:
      return QMetaType::QString;
    case mdtSqlFieldType::Date:
      return QMetaType::QDate;
    case mdtSqlFieldType::Time:
      return QMetaType::QTime;
    case mdtSqlFieldType::DateTime:
      return QMetaType::QDateTime;
  }
  return QMetaType::UnknownType;
}

QMap<QMetaType::Type, mdtSqlFieldType::Type> mdtSqlFieldType::mappingTableSqlite()
{
  return {
    std::pair<QMetaType::Type, mdtSqlFieldType::Type>{QMetaType::UnknownType, mdtSqlFieldType::UnknownType},
    std::pair<QMetaType::Type, mdtSqlFieldType::Type>{QMetaType::Bool, mdtSqlFieldType::Boolean},
    std::pair<QMetaType::Type, mdtSqlFieldType::Type>{QMetaType::QDate, mdtSqlFieldType::Date},
    std::pair<QMetaType::Type, mdtSqlFieldType::Type>{QMetaType::Double, mdtSqlFieldType::Double},
    std::pair<QMetaType::Type, mdtSqlFieldType::Type>{QMetaType::Int, mdtSqlFieldType::Integer},
    std::pair<QMetaType::Type, mdtSqlFieldType::Type>{QMetaType::QString, mdtSqlFieldType::Varchar},
    std::pair<QMetaType::Type, mdtSqlFieldType::Type>{QMetaType::QTime, mdtSqlFieldType::Time}
  };

}

QString mdtSqlFieldType::nameFromTypeCommon(mdtSqlFieldType::Type type)
{
  switch(type){
    case UnknownType:
      return QLatin1String("");
    case Boolean:
      return QLatin1String("BOOLEAN");
    case Integer:
      return QLatin1String("INTEGER");
    case Float:
      return QLatin1String("FLOAT");
    case Double:
      return QLatin1String("DOUBLE");
    case Varchar:
      return QLatin1String("VARCHAR");
    case Date:
      return QLatin1String("DATE");
    case Time:
      return QLatin1String("TIME");
    case DateTime:
      return QLatin1String("DATETIME");
  }
  return QLatin1String("");
}

mdtSqlFieldType mdtSqlFieldType::typeFromNameCommon(const QString & typeName)
{
  mdtSqlFieldType ft;
  QStringList tnl = typeName.split("(", QString::SkipEmptyParts);
  Q_ASSERT(!tnl.isEmpty());

  ft.pvName = tnl.at(0).trimmed();
  // Set type regarding type name
  const QString tn = ft.name().toUpper();
  if(tn == QLatin1String("BOOL") || tn == QLatin1String("BOOLEAN")){
    ft.pvType = Boolean;
  }else if(tn == QLatin1String("INT") || tn == QLatin1String("INTEGER")){
    ft.pvType = Integer;
  }else if(tn == QLatin1String("FLOAT")){
    ft.pvType = Float;
  }else if(tn == QLatin1String("DOUBLE")){
    ft.pvType = Double;
  }else if(tn == QLatin1String("VARCHAR")){
    ft.pvType = Varchar;
  }else if(tn == QLatin1String("DATE")){
    ft.pvType = Date;
  }else if(tn == QLatin1String("TIME")){
    ft.pvType = Time;
  }else if(tn == QLatin1String("DATETIME")){
    ft.pvType = DateTime;
  }else{
    ft.pvType = UnknownType;
  }
  // Set length
  if(tnl.size() > 1){
    const QString sizeStr = tnl.at(1).trimmed().remove(")");
    bool ok;
    ft.pvLength = sizeStr.toInt(&ok);
    if(!ok){
      ft.pvLength = -1;
    }
  }

  return ft;
}

QList< mdtSqlFieldType > mdtSqlFieldType::availableFieldTypeListCommon(mdtSqlDriverType::Type driverType)
{
  QList<mdtSqlFieldType> fieldTypeList;
  mdtSqlFieldType fieldType;

  fieldType.setType(Boolean, driverType);
  fieldTypeList.append(fieldType);
  fieldType.setType(Integer, driverType);
  fieldTypeList.append(fieldType);
  fieldType.setType(Float, driverType);
  fieldTypeList.append(fieldType);
  fieldType.setType(Double, driverType);
  fieldTypeList.append(fieldType);
  fieldType.setType(Varchar, driverType);
  fieldTypeList.append(fieldType);
  fieldType.setType(Date, driverType);
  fieldTypeList.append(fieldType);
  fieldType.setType(Time, driverType);
  fieldTypeList.append(fieldType);

  return fieldTypeList;
}

bool operator==(const mdtSqlFieldType & a, const mdtSqlFieldType & b)
{
  /*
   * If type is unknown for a or for b, a != b
   */
  if( (a.type() == mdtSqlFieldType::UnknownType) || (b.type() == mdtSqlFieldType::UnknownType) ){
    return false;
  }
  // Trivial type check
  if(a.type() != b.type()){
    return false;
  }
  /*
   * If one field type has its length set, but not the other, a != b
   */
  if( (a.length() < 0) && (b.length() >= 0) ){
    return false;
  }
  if( (b.length() < 0) && (a.length() >= 0) ){
    return false;
  }
  /*
   * If both a and b has not thier length set, a == b
   */
  if( (a.length() < 0) && (b.length() < 0) ){
    return true;
  }
  // Trivial length check
  Q_ASSERT(a.length() > 0);
  Q_ASSERT(b.length() > 0);
  if(a.length() != b.length()){
    return false;
  }

  return true;
}
