/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "FieldTypeMapImplementation.h"

namespace Mdt{ namespace Sql{ namespace Schema{

FieldType FieldTypeMapImplementation::fieldTypeFromQMetaType(QMetaType::Type qmetaType, FieldLength fieldLength) const
{
  switch(qmetaType){
    case QMetaType::Bool:
      return FieldType::Boolean;
    case QMetaType::Short:
    case QMetaType::UShort:
      return FieldType::Smallint;
    case QMetaType::Int:
    case QMetaType::UInt:
      return FieldType::Integer;
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
      return FieldType::Bigint;
    case QMetaType::Float:
      return FieldType::Float;
    case QMetaType::Double:
      return FieldType::Double;
    case QMetaType::QChar:
    case QMetaType::QString:
      return fieldTypeFlagsFromQMetaTypeString(qmetaType, fieldLength);
    case QMetaType::QByteArray:
      return FieldType::Blob;
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

FieldTypeFlags FieldTypeMapImplementation::fieldTypeFlagsFromQMetaType(QMetaType::Type qmetaType) const
{
  switch(qmetaType){
    case QMetaType::Bool:
      return FieldTypeFlag::NoFlag;
    case QMetaType::Short:
      return FieldTypeFlag::NoFlag;
    case QMetaType::UShort:
      return FieldTypeFlag::IsUnsigned;
    case QMetaType::Int:
      return FieldTypeFlag::NoFlag;
    case QMetaType::UInt:
      return FieldTypeFlag::IsUnsigned;
    case QMetaType::LongLong:
      return FieldTypeFlag::NoFlag;
    case QMetaType::ULongLong:
      return FieldTypeFlag::IsUnsigned;
    case QMetaType::Float:
      return FieldTypeFlag::NoFlag;
    case QMetaType::Double:
      return FieldTypeFlag::NoFlag;
    case QMetaType::QChar:
      return FieldTypeFlag::NoFlag;
    case QMetaType::QString:
      return FieldTypeFlag::NoFlag;
    case QMetaType::QByteArray:
      return FieldTypeFlag::NoFlag;
    case QMetaType::QDate:
      return FieldTypeFlag::NoFlag;
    case QMetaType::QTime:
      return FieldTypeFlag::NoFlag;
    case QMetaType::QDateTime:
      return FieldTypeFlag::NoFlag;
    default:
      break;
  }
  return FieldTypeFlag::NoFlag;
}

QMetaType::Type FieldTypeMapImplementation::qmetaTypeFromFieldType(FieldType fieldType, FieldLength fieldLength, FieldTypeFlags fieldTypeFlags) const
{
  switch(fieldType){
    case FieldType::UnknownType:
      return QMetaType::UnknownType;
    case FieldType::Boolean:
      return QMetaType::Bool;
    case FieldType::Smallint:
      if(fieldTypeFlags.testFlag(FieldTypeFlag::IsUnsigned)){
        return QMetaType::UShort;
      }else{
        return QMetaType::Short;
      }
    case FieldType::Integer:
      if(fieldTypeFlags.testFlag(FieldTypeFlag::IsUnsigned)){
        return QMetaType::UInt;
      }else{
        return QMetaType::Int;
      }
    case FieldType::Bigint:
      if(fieldTypeFlags.testFlag(FieldTypeFlag::IsUnsigned)){
        return QMetaType::ULongLong;
      }else{
        return QMetaType::LongLong;
      }
    case FieldType::Float:
      return QMetaType::Float;
    case FieldType::Double:
      return QMetaType::Double;
    case FieldType::Char:
    case FieldType::Varchar:
    case FieldType::Text:
      return qmetaTypeFromFieldTypeString(fieldType, fieldLength);
    case FieldType::Blob:
      return QMetaType::QByteArray;
    case FieldType::Date:
      return QMetaType::QDate;
    case FieldType::Time:
      return QMetaType::QTime;
    case FieldType::DateTime:
      return QMetaType::QDateTime;
  }
  return QMetaType::UnknownType;
}

FieldType FieldTypeMapImplementation::fieldTypeFlagsFromQMetaTypeString(QMetaType::Type qmetaType, FieldLength fieldLength)
{
  Q_ASSERT( (qmetaType == QMetaType::QChar) || (qmetaType == QMetaType::QString) );

  if(qmetaType == QMetaType::QChar){
    Q_ASSERT( fieldLength.isNull() || (fieldLength.value() == 1) );
    return FieldType::Char;
  }
  if(qmetaType == QMetaType::QString){
    if(fieldLength.isNull()){
      return FieldType::Text;
    }else{
      if(fieldLength.value() <= 2){
        return FieldType::Char;
      }else{
        return FieldType::Varchar;
      }
    }
  }

  return FieldType::UnknownType;
}

QMetaType::Type FieldTypeMapImplementation::qmetaTypeFromFieldTypeString(FieldType fieldType, FieldLength fieldLength)
{
  Q_ASSERT( (fieldType == FieldType::Char) || (fieldType == FieldType::Varchar) || (fieldType == FieldType::Text) );

  if(fieldType == FieldType::Char){
    if(fieldLength.isNull()){
      return QMetaType::QChar;
    }
    if(fieldLength.value() <= 1){
      return QMetaType::QChar;
    }
    return QMetaType::QString;
  }

  return QMetaType::QString;
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
