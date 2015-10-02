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
      case Time:
        return QLatin1String("TIME");
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
    fieldType.setType(Time, driverType);
    fieldTypeList.append(fieldType);

    return fieldTypeList;
  }

  Type pvType;
  QString pvName;
};

#endif // #ifndef MDT_SQL_FIELD_TYPE_H
