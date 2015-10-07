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
#ifndef MDT_SQL_INDEX_H
#define MDT_SQL_INDEX_H

#include "mdtSqlIndexBase.h"
#include <QString>

class QSqlDatabase;

/*! \brief Container for SQL indexes
 */
class mdtSqlIndex : public mdtSqlIndexBase
{
 public:

  /*! \brief Default constructor
   */
  mdtSqlIndex()
   : pvIsUnique(false)
  {
  }

  /*! \brief Set unique constraint
   */
  void setUnique(bool u)
  {
    pvIsUnique = u;
  }

  /*! \brief Check if unique constraint is set
   */
  bool isUnique() const
  {
    return pvIsUnique;
  }

  /*! \brief Set table name
   */
  void setTableName(const QString & name)
  {
    pvTableName = name;
  }

  /*! \brief Get table name
   */
  QString tableName() const
  {
    return pvTableName;
  }

  /*! \brief Generate index name
   *
   * The generated name will be of form:
   *  tableName_fieldName1_fieldName2_..._fieldNameN_index
   *
   * \pre tableName must be set, and at least 1 field must exist
   */
  void generateName()
  {
    Q_ASSERT(!pvTableName.isEmpty());
    Q_ASSERT(!pvFields.isEmpty());

    QString name = pvTableName + "_";
    for(const auto & fieldName : pvFields){
      name += fieldName + "_";
    }
    name += "index";
    setName(name);
  }

  /*! \brief Clear
   */
  void clear();

  /*! \brief Get SQL statement to drop the index
   *
   * \pre db's driver must be loaded
   */
  QString getSqlForDrop(const QSqlDatabase & db, const QString & databaseName) const;

  /*! \brief Get SQL statement to create the index
   *
   * \pre db's driver must be loaded
   */
  QString getSqlForCreate(const QSqlDatabase & db, const QString & databaseName) const;

 private:

  uint pvIsUnique : 1;
  QString pvTableName;
};

#endif // #ifndef MDT_SQL_INDEX_H
