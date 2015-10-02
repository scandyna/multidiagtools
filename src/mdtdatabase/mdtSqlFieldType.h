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
#ifndef MDT_SQL_FIELD_TYPE_H
#define MDT_SQL_FIELD_TYPE_H

#include "mdtSqlDriverType.h"
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QLatin1String>
#include <QList>
#include <QMap>
#include <utility>

/*! \brief SQL field type
 *
 * SQL field type is defined in Qt QSQL module with QVariant::Type.
 *
 * MultiDiagTools tries to provide some functionnalities
 *  for schema generation and edition.
 *  This part is currently very limited (partial support of Sqlite and MySql/MariaDB).
 *
 * mdtSqlFieldType provides a mapping between QVariant::Type and datbase engine's type and its name.
 */
class mdtSqlFieldType
{
 public:

  /*! \brief Field type
   */
  enum Type : uint8_t
  {
    UnknownType,  /*!< Unknown type (or no type) */
    Boolean,      /*!< Boolean type */
    Integer,      /*!< Integer type */
    Float,        /*!< Float type */
    Double,       /*!< Double type */
    Varchar,      /*!< Variable length string */
    Date,         /*!< Date type */
    Time          /*!< Time type */
  };

  /*! \brief Default constructor
   */
  mdtSqlFieldType()
   : pvType(UnknownType)
   {
   }

  /*! \brief Check if field type is null
   */
  bool isNull() const
  {
    return (pvType == UnknownType);
  }

  /*! \brief Set type
   */
  void setType(Type type, mdtSqlDriverType::Type driverType)
  {
    Q_ASSERT(driverType != mdtSqlDriverType::Unknown);

    pvName = nameFromType(type, driverType);
    pvType = type;
  }

  /*! \brief Set type
   */
//   bool setType(QVariant::Type type, mdtSqlDriverType::Type driverType)
//   {
//     Q_ASSERT(driverType != mdtSqlDriverType::Unknown);
// 
//     pvName = nameFromType(type, driverType);
//     if(pvName.isEmpty()){
//       pvType = QVariant::Invalid;
//       return false;
//     }
//     pvType = type;
//     return true;
//   }

  /*! \brief Clear
   */
  void clear()
  {
    pvType = UnknownType;
    pvName.clear();
  }

  /*! \brief Get field type
   */
  Type type() const
  {
    return pvType;
  }

  /*! \brief Get field type name
   */
  QString name() const
  {
    return pvName;
  }

  /*! \brief Get field type name for given type and driver
   *
   * \param type Field type
   * \param driverType Driver type
   * \return Name of the field type, or a empty string if no mapping could be defined.
   * \pre driverType must be defined (not Unknown)
   */
//   [[deprecated]]
//   static QString nameFromType(QVariant::Type type, mdtSqlDriverType::Type driverType)
//   {
//     Q_ASSERT(driverType != mdtSqlDriverType::Unknown);
// 
//     switch(driverType){
//       case mdtSqlDriverType::SQLite:
//         return nameFromTypeSqlite(type);
//       case mdtSqlDriverType::MariaDB:
//       case mdtSqlDriverType::MySQL:
//         return nameFromTypeMysql(type);
//       case mdtSqlDriverType::Unknown:
//         // Should never happen.
//         break;
//     }
// 
//     return QString();
//   }

//   /*! \brief Get field type from given field type name and driver
//    *
//    * \param name Field type name
//    * \param driverType Driver type
//    * \return Field type, or invalid type if no mapping could be defined.
//    * \pre driverType must be defined (not Unknown)
//    */
//   static QVariant::Type typeFromName(const QString & name, mdtSqlDriverType::Type driverType)
//   {
//     Q_ASSERT(!driverType != mdtSqlDriverType::Unknown);
// 
//     switch(driverType){
//       case mdtSqlDriverType::SQLite:
//         return typeFromNameSqlite(name);
//       case mdtSqlDriverType::MariaDB:
//       case mdtSqlDriverType::MySQL:
//         return typeFromNameMysql(name);
//       case mdtSqlDriverType::Unknown:
//         // Should never happen.
//         break;
//     }
// 
//     return QVariant::Invalid;
//   }

  /*! \brief Get field type name for given type and driver
   *
   * \param type Field type
   * \param driverType Driver type (If unknown, a common name is returned)
   * \return Name of the field type, or a empty string if no mapping could be defined.
   */
  static QString nameFromType(Type type, mdtSqlDriverType::Type driverType)
  {
    switch(driverType){
      case mdtSqlDriverType::SQLite:
      case mdtSqlDriverType::MariaDB:
      case mdtSqlDriverType::MySQL:
      case mdtSqlDriverType::Unknown:
        return nameFromTypeCommon(type);
    }
    return QString();
  }

  /*! \brief Get a list of available field type
   */
  static QList<mdtSqlFieldType> availableFieldTypeList(mdtSqlDriverType::Type driverType)
  {
    switch(driverType){
      case mdtSqlDriverType::SQLite:
      case mdtSqlDriverType::MariaDB:
      case mdtSqlDriverType::MySQL:
      case mdtSqlDriverType::Unknown:
        return availableFieldTypeListCommon(driverType);
    }
    return QList<mdtSqlFieldType>();
  }

  /*! \brief Get field type from QVariant::Type and driver type
   */
  static Type fromQVariantType(QVariant::Type qvt, mdtSqlDriverType::Type driverType)
  {
    switch(driverType){
      case mdtSqlDriverType::SQLite:
        return fromQVariantType(qvt, mappingTableSqlite());
      case mdtSqlDriverType::MariaDB:
      case mdtSqlDriverType::MySQL:
      case mdtSqlDriverType::Unknown:
        break;
    }
    return UnknownType;
  }

  /*! \brief Get field type from QVariant::Type and mapping table
   */
  static Type fromQVariantType(QVariant::Type qvt, const QMap<QVariant::Type, mdtSqlFieldType::Type> & mt)
  {
    return mt.value(qvt, UnknownType);
  }

  /*! \brief Get field type mapping table for given driver type
   */
  static QMap<QVariant::Type, mdtSqlFieldType::Type> mappingTable(mdtSqlDriverType::Type driverType)
  {
    switch(driverType){
      case mdtSqlDriverType::SQLite:
        return mappingTableSqlite();
      case mdtSqlDriverType::MariaDB:
      case mdtSqlDriverType::MySQL:
      case mdtSqlDriverType::Unknown:
        break;
    }
    return QMap<QVariant::Type, mdtSqlFieldType::Type>();
  }

 private:

  /*! \brief Get field type mapping table for SQLite
   */
  static QMap<QVariant::Type, mdtSqlFieldType::Type> mappingTableSqlite()
  {
    return {
      std::pair<QVariant::Type, mdtSqlFieldType::Type>{QVariant::Invalid, mdtSqlFieldType::UnknownType},
      std::pair<QVariant::Type, mdtSqlFieldType::Type>{QVariant::Bool, mdtSqlFieldType::Boolean},
      std::pair<QVariant::Type, mdtSqlFieldType::Type>{QVariant::Date, mdtSqlFieldType::Date},
      std::pair<QVariant::Type, mdtSqlFieldType::Type>{QVariant::Double, mdtSqlFieldType::Double},
      std::pair<QVariant::Type, mdtSqlFieldType::Type>{QVariant::Int, mdtSqlFieldType::Integer},
      std::pair<QVariant::Type, mdtSqlFieldType::Type>{QVariant::String, mdtSqlFieldType::Varchar},
      std::pair<QVariant::Type, mdtSqlFieldType::Type>{QVariant::Time, mdtSqlFieldType::Time}
    };
  }

  /*! \brief Get field type name for given type (Common implementation)
   */
  static QString nameFromTypeCommon(Type type)
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
    }
    return QLatin1String("");
  }

  /*! \brief Get a list of available field type (common implementation)
   */
  static QList<mdtSqlFieldType> availableFieldTypeListCommon(mdtSqlDriverType::Type driverType)
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

    return fieldTypeList;
  }

  /*! \brief Get field type name for given type (Sqlite implementation)
   */
//   [[deprecated]]
//   static QString nameFromTypeSqlite(QVariant::Type type)
//   {
//     switch(type){
//       case QVariant::Int:
//         return QLatin1String("INTEGER");
//       case QVariant::UInt:
//         return QLatin1String("INTEGER");
//       case QVariant::LongLong:
//         return QLatin1String("BIGINT");
//       case QVariant::ULongLong:
//         return QLatin1String("BIGINT");
//       case QVariant::Double:
//         return QLatin1String("DOUBLE");
//       case QVariant::Bool:
//         return QLatin1String("BOOLEAN");
//       case QVariant::Date:
//         return QLatin1String("DATE");
//       case QVariant::Time:
//         return QLatin1String("DATETIME");
//       case QVariant::DateTime:
//         return QLatin1String("DATETIME");
//       case QVariant::String:
//         return QLatin1String("VARCHAR");
//       case QVariant::Bitmap:
//         return QLatin1String("BLOB");
//       case QVariant::Image:
//         return QLatin1String("BLOB");
//       default:
//         return QLatin1String("");
//     }
//   }

  /*! \brief Get field type name for given type (MariaDB/MySQL implementation)
   */
//   [[deprecated]]
//   static QString nameFromTypeMysql(QVariant::Type type)
//   {
//     switch(type){
//       case QVariant::Int:
//         return QLatin1String("INT");
//       case QVariant::UInt:
//         return QLatin1String("UNSIGNED INT");
//       case QVariant::LongLong:
//         return QLatin1String("BIGINT");
//       case QVariant::ULongLong:
//         return QLatin1String("UNSIGNED BIGINT");
//       case QVariant::Double:
//         return QLatin1String("DOUBLE");
//       case QVariant::Bool:
//         return QLatin1String("BOOLEAN");
//       case QVariant::Date:
//         return QLatin1String("DATE");
//       case QVariant::Time:
//         return QLatin1String("TIME");
//       case QVariant::DateTime:
//         return QLatin1String("DATETIME");
//       case QVariant::String:
//         return QLatin1String("VARCHAR");
//       case QVariant::Bitmap:
//         return QLatin1String("BLOB");
//       case QVariant::Image:
//         return QLatin1String("BLOB");
//       default:
//         return QLatin1String("");
//     }
//   }

//   /*! \brief Get field type from name (QSQlite implementation)
//    */
//   static QVariant::Type typeFromNameSqlite(const QString & name)
//   {
//     const QString _name = name.toUpper();
// 
//     if(_name == QLatin1String("INTEGER")){
//       return QVariant::Int;
//     }
//     if(_name == QLatin1String("INT")){
//       return QVariant::Int;
//     }
//     if(_name == QLatin1String("DOUBLE")){
//       return QVariant::Double;
//     }
//     if(_name == QLatin1String("FLOAT")){
//       return QVariant::Double;
//     }
//     if(_name == QLatin1String("REAL")){
//       return QVariant::Double;
//     }
//     if(_name.startsWith(QLatin1String("NUMERIC"))){
//       return QVariant::Double;
//     }
//     if(_name == QLatin1String("BLOB")){
//       return QVariant::ByteArray;
//     }
//     if(_name == QLatin1String("BOOLEAN")){
//       return QVariant::Bool;
//     }
//     if(_name == QLatin1String("BOOL")){
//       return QVariant::Bool;
//     }
// 
//     return QVariant::Invalid;
//   }

//   /*! \brief Get field type from name (MariaDB/MySql implementation)
//    */
//   static QVariant::Type typeFromNameMysql(const QString & name)
//   {
//     QString _name;
//     bool isUnsigned;
//     // Separate UNSIGNED and FIELDNAME (on case of UNSIGNED type)
//     QStringList strList = name.toUpper().split(' ', QString::SkipEmptyParts);
//     if(strList.isEmpty()){
//       return QVariant::Invalid;
//     }
//     if(strList.size() == 1){
//       _name = strList.at(0);
//       isUnsigned = false;
//     }else if(strList.size() == 2){
//       _name = strList.at(1).trimmed();
//       isUnsigned = strList.at(0).trimmed() == QLatin1String("UNSIGNED");
//     }else{
//       if(!strList.at(2).trimmed().startsWith(QLatin1String("("))){
//         return QVariant::Invalid;
//       }
//       _name = strList.at(1).trimmed();
//       isUnsigned = strList.at(0).trimmed() == QLatin1String("UNSIGNED");
//     }
//     // Map field name to type
//     if(_name.startsWith(QLatin1String("TINYINT"))){
//       if(isUnsigned){
//         return QVariant::UInt;
//       }else{
//         return QVariant::Int;
//       }
//     }
//     if(_name.startsWith(QLatin1String("SMALLINT"))){
//       if(isUnsigned){
//         return QVariant::UInt;
//       }else{
//         return QVariant::Int;
//       }
//     }
//     if(_name.startsWith(QLatin1String("MEDIUMINT"))){
//       if(isUnsigned){
//         return QVariant::UInt;
//       }else{
//         return QVariant::Int;
//       }
//     }
//     if(_name.startsWith(QLatin1String("INT"))){
//       if(isUnsigned){
//         return QVariant::UInt;
//       }else{
//         return QVariant::Int;
//       }
//     }
//     if(_name.startsWith(QLatin1String("BIGINT"))){
//       if(isUnsigned){
//         return QVariant::ULongLong;
//       }else{
//         return QVariant::LongLong;
//       }
//     }
//     if(_name.startsWith(QLatin1String("FLOAT"))){
//       return QVariant::Double;
//     }
//     if(_name.startsWith(QLatin1String("DOUBLE"))){
//       return QVariant::Double;
//     }
//     if(_name == QLatin1String("BLOB")){
//       return QVariant::ByteArray;
//     }
//     if(_name == QLatin1String("BOOLEAN")){
//       return QVariant::Bool;
//     }
//     if(_name == QLatin1String("BOOL")){
//       return QVariant::Bool;
//     }
// 
//     return QVariant::String;
//   }

//   /*! \brief Get a list of available field type (SQLite implementation)
//    */
//   static QList<mdtSqlFieldType> availableFieldTypeListSqlite()
//   {
//     QList<mdtSqlFieldType> fieldTypeList;
//     mdtSqlFieldType fieldType;
// 
//     fieldType.setType(QVariant::Int, mdtSqlDriverType::SQLite);
//     fieldTypeList.append(fieldType);
//     fieldType.setType(QVariant::String, mdtSqlDriverType::SQLite);
//     fieldTypeList.append(fieldType);
//     fieldType.setType(QVariant::Double, mdtSqlDriverType::SQLite);
//     fieldTypeList.append(fieldType);
//     fieldType.setType(QVariant::Bool, mdtSqlDriverType::SQLite);
//     fieldTypeList.append(fieldType);
//     fieldType.setType(QVariant::Date, mdtSqlDriverType::SQLite);
//     fieldTypeList.append(fieldType);
//     fieldType.setType(QVariant::DateTime, mdtSqlDriverType::SQLite);
//     fieldTypeList.append(fieldType);
// 
//     return fieldTypeList;
//   }

  /*! \brief Get a list of available field type (MariaDB/MySQL implementation)
   */
//   static QList<mdtSqlFieldType> availableFieldTypeListMysql()
//   {
//     QList<mdtSqlFieldType> fieldTypeList;
// 
//     
// 
//     return fieldTypeList;
//   }

  Type pvType;
  QString pvName;
};

#endif // #ifndef MDT_SQL_FIELD_TYPE_H
