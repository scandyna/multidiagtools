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
#ifndef MDT_SQL_FIELD_TYPE_H
#define MDT_SQL_FIELD_TYPE_H

#include "mdtSqlDriverType.h"
#include <QVariant>
#include <QMetaType>
#include <QString>
#include <QList>
#include <QMap>

/*! \brief SQL field type
 *
 * SQL field type is defined in Qt QSQL module with QVariant::Type.
 *
 * MultiDiagTools tries to provide some functionnalities
 *  for schema generation and edition.
 *  This part is currently very limited (partial support of Sqlite and MySql/MariaDB).
 *
 * mdtSqlFieldType provides a mapping between QMetaType::Type and datbase engine's type and its name.
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
    Time,         /*!< Time type */
    DateTime      /*!< DateTime type */
  };

  /*! \brief Default constructor
   */
  mdtSqlFieldType()
   : pvType(UnknownType),
     pvLength(-1)
   {
   }

  /*! \brief Check if field type is null
   */
  bool isNull() const
  {
    return (pvType == UnknownType);
  }

  /*! \brief Set type
   *
   * This version will set name
   */
  void setType(Type type, mdtSqlDriverType::Type driverType)
  {
    Q_ASSERT(driverType != mdtSqlDriverType::Unknown);

    pvName = nameFromType(type, driverType);
    pvType = type;
  }

  /*! \brief Set type
   *
   * This version will not set name
   */
  void setType(Type type)
  {
    pvType = type;
    pvName.clear();
  }

  /*! \brief Set length
   */
  void setLength(int length)
  {
    pvLength = length;
  }

  /*! \brief Clear
   */
  void clear()
  {
    pvType = UnknownType;
    pvLength = -1;
    pvName.clear();
  }

  /*! \brief Get field type
   */
  Type type() const
  {
    return pvType;
  }

  /*! \brief Get field type name
   *
   * \note returns only the name of field type, length is not included
   */
  QString name() const
  {
    return pvName;
  }

  /*! \brief Get field type length
   *
   * If length was not defined, this function returns -1
   */
  int length() const
  {
    return pvLength;
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

  /*! \brief Get field type from type name
   */
  static mdtSqlFieldType typeFromName(const QString & typeName, mdtSqlDriverType::Type driverType)
  {
    switch(driverType){
      case mdtSqlDriverType::SQLite:
      case mdtSqlDriverType::MariaDB:
      case mdtSqlDriverType::MySQL:
      case mdtSqlDriverType::Unknown:
        return typeFromNameCommon(typeName);
    }
    return mdtSqlFieldType();
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

  /*! \brief Get SQL field type from QMetaType::Type for driver type
   */
  static Type fromQMetaType(QMetaType::Type qmt, mdtSqlDriverType::Type driverType)
  {
    switch(driverType){
      case mdtSqlDriverType::SQLite:
        return fromQMetaType(qmt, mappingTableSqlite());
      case mdtSqlDriverType::MariaDB:
      case mdtSqlDriverType::MySQL:
      case mdtSqlDriverType::Unknown:
        break;
    }
    return UnknownType;
  }

  /*! \brief Get SQL field type from QMetaType::Type for mapping table
   */
  static Type fromQMetaType(QMetaType::Type qmt, const QMap<QMetaType::Type, mdtSqlFieldType::Type> & mappingTable)
  {
    return mappingTable.value(qmt, UnknownType);
  }

  /*! \brief Get field type from QVariant::Type and driver type
   *
   * Note that QVariant::Type is deprecated in Qt library
   */
  static Type fromQVariantType(QVariant::Type qvt, mdtSqlDriverType::Type driverType)
  {
    return fromQMetaType(static_cast<QMetaType::Type>(qvt), driverType);
  }

  /*! \brief Get field type from QVariant::Type and mapping table
   *
   * Note that QVariant::Type is deprecated in Qt library
   */
  static Type fromQVariantType(QVariant::Type qvt, const QMap<QMetaType::Type, mdtSqlFieldType::Type> & mappingTable)
  {
    return fromQMetaType(static_cast<QMetaType::Type>(qvt), mappingTable);
  }

  /*! \brief Get QMetaType::Type from SQL field type
   */
  static QMetaType::Type toQMetaType(mdtSqlFieldType::Type sqlType);

  /*! \brief Get QVariant::Type from SQL field type
   *
   * Note that QVariant::Type is deprecated in Qt library
   */
  static QVariant::Type toQVariantType(Type sqlType)
  {
    return static_cast<QVariant::Type>(toQMetaType(sqlType));
  }

  /*! \brief Get field type mapping table for given driver type
   */
  static QMap<QMetaType::Type, mdtSqlFieldType::Type> mappingTable(mdtSqlDriverType::Type driverType)
  {
    switch(driverType){
      case mdtSqlDriverType::SQLite:
        return mappingTableSqlite();
      case mdtSqlDriverType::MariaDB:
      case mdtSqlDriverType::MySQL:
      case mdtSqlDriverType::Unknown:
        break;
    }
    return QMap<QMetaType::Type, mdtSqlFieldType::Type>();
  }

 private:

  /*! \brief Get field type mapping table for SQLite
   */
  static QMap<QMetaType::Type, mdtSqlFieldType::Type> mappingTableSqlite();

  /*! \brief Get field type name for given type (Common implementation)
   */
  static QString nameFromTypeCommon(Type type);

  /*! \brief Get field type from type name (Common implementation)
   */
  static mdtSqlFieldType typeFromNameCommon(const QString & typeName);

  /*! \brief Get a list of available field type (common implementation)
   */
  static QList<mdtSqlFieldType> availableFieldTypeListCommon(mdtSqlDriverType::Type driverType);

  Type pvType;
  int pvLength;
  QString pvName;
};

/*! \brief Check if field type a and b are equal
  *
  * Checking equality for field types requiers some rules
  *  for cases of unset type or length:
  *  - type is unknown for a or for b -> a != b
  *  - one of the field types has a length set, but the other is set -> a != b
  */
bool operator==(const mdtSqlFieldType & a, const mdtSqlFieldType & b);

/*! \brief Check if field type a and b are different
 *
 * \sa operator==()
 */
inline bool operator!=(const mdtSqlFieldType & a, const mdtSqlFieldType & b)
{
  return !(a == b);
}

#endif // #ifndef MDT_SQL_FIELD_TYPE_H
