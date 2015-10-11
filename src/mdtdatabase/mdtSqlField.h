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
#ifndef MDT_SQL_FIELD_H
#define MDT_SQL_FIELD_H

#include "mdtError.h"
#include "mdtSqlFieldType.h"
#include "mdtSqlCollation.h"
#include <QString>
#include <QVariant>
#include <QSqlField>

class QSqlDatabase;

/*! \brief Represents a field in a SQL database table
 *
 * This is a simple data storage class.
 *  Because of multiple possible usage,
 *  no coherence check is made.
 */
class mdtSqlField final
{
 public:

  /*! \brief Default constructor
   */
  mdtSqlField()
   : pvType(mdtSqlFieldType::UnknownType),
     pvIsAutoValue(false),
     pvIsRequired(false),
     pvLength(-1)
  {
  }

  /*! \brief Check if field is null
   *
   * Field is null at least if one of the mandatory attribute is not set.
   *  Mandatory attributes are:
   *  - type \sa setType() , type()
   *  - name \sa setName() , name()
   */
  bool isNull() const
  {
    return ( (pvType == mdtSqlFieldType::UnknownType) || pvName.isEmpty());
  }

  /*! \brief Clear
   */
  void clear();

  /*! \brief Set field type
   */
  void setType(mdtSqlFieldType::Type t)
  {
    pvType = t;
  }

  /*! \brief Get field type
   */
  mdtSqlFieldType::Type type() const
  {
    return pvType;
  }

  /*! \brief Set field name
   */
  void setName(const QString & name)
  {
    pvName = name;
  }

  /*! \brief Get field name
   */
  QString name() const
  {
    return pvName;
  }

  /*! \brief Set auto value
   */
  void setAutoValue(bool autoValue)
  {
    pvIsAutoValue = autoValue;
  }

  /*! \brief Check if field is auto value
   */
  bool isAutoValue() const
  {
    return pvIsAutoValue;
  }

  /*! \brief Set field required
   */
  void setRequired(bool r)
  {
    pvIsRequired = r;
  }

  /*! \brief Check if field is required
   */
  bool isRequired() const
  {
    return pvIsRequired;
  }

  /*! \brief Set default value
   */
  void setDefaultValue(const QVariant & v)
  {
    pvDefaultValue = v;
  }

  /*! \brief Get default value
   */
  QVariant defaultValue() const
  {
    return pvDefaultValue;
  }

  /*! \brief Set length
   *
   * \sa length()
   */
  void setLength(int length)
  {
    pvLength = length;
  }

  /*! \brief Get length
   *
   * A value < 0 means that length is not set.
   */
  int length() const
  {
    return pvLength;
  }

  /*! \brief Set collation
   */
  void setCollation(const mdtSqlCollation & collation)
  {
    pvCollation = collation;
  }

  /*! \brief Set case sensitivity
   *
   * Case sensitivity will update collation
   */
  void setCaseSensitive(bool cs)
  {
    pvCollation.setCaseSensitive(cs);
  }

  /*! \brief Get collation
   */
  mdtSqlCollation collation() const
  {
    return pvCollation;
  }

  /*! \brief Setup from QSqlField
   */
  void setupFromQSqlField(const QSqlField & qtField, mdtSqlDriverType::Type driverType);

  /*! \brief Get SQL for field definition
   *
   * \param db Will be used to fetch driver type, and to escape field name
   * \param pk If true, PRIMARY KEY will be included in definition.
   *            This has only sense if this field is the only one in the primary key of the table.
   * \pre db's driver must be loaded
   */
  QString getSql(const QSqlDatabase & db, bool pk) const;

 private:

  /*! \brief Complete definition - SQLite implementation
   */
  QString getSqliteDefinition(bool pk) const;

  /*! \brief Complete definition - MySQL implementation
   */
  QString getMysqlDefinition(bool pk) const;

  mdtSqlFieldType::Type pvType;
  uint pvIsAutoValue : 1;
  uint pvIsRequired : 1;
  int pvLength;
  QString pvName;
  QVariant pvDefaultValue;
  mdtSqlCollation pvCollation;
  mdtError pvLastError;
};

#endif // #ifndef MDT_SQL_FIELD_H
