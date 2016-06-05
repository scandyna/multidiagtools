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
#ifndef MDT_SQL_SCHEMA_INDEX_H
#define MDT_SQL_SCHEMA_INDEX_H

#include "Field.h"
#include "Table.h"
#include <QString>
#include <QStringList>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief SQL schema index
   */
  class Index
  {
   public:

    /*! \brief Default constructor
     */
    Index()
     : pvIsUnique(false)
    {
    }

    /*! \brief Set index name
     */
    void setName(const QString & name)
    {
      pvName = name;
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
      Q_ASSERT(!pvFieldNameList.isEmpty());

      QString name = pvTableName + QStringLiteral("_");
      for(const auto & fieldName : pvFieldNameList){
        name += fieldName + QStringLiteral("_");
      }
      name += "index";
      setName(name);
    }

    /*! \brief Get index name
     */
    QString name() const
    {
      return pvName;
    }

    /*! \brief Set table
     */
    void setTable(const Table & table)
    {
      pvTableName = table.tableName();
    }

//     /*! \brief Set table name
//      */
//     void setTableName(const QString & name)
//     {
//       pvTableName = name;
//     }

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return pvTableName;
    }

    /*! \brief Add a field to this index
     */
    void addField(const Field & field)
    {
      pvFieldNameList.append(field.name());
    }

    /*! \brief Get field count
     */
    int fieldCount() const
    {
      return pvFieldNameList.size();
    }

    /*! \brief Get field name at given index
     *
     * \pre index must be in valid range
     */
    QString fieldName(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvFieldNameList.size());
      return pvFieldNameList.at(index);
    }

    /*! \brief Get list of field names
     */
    QStringList fieldNameList() const
    {
      return pvFieldNameList;
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

    /*! \brief Check if index is null
     *
     * Index is null is if name is empty,
     *  table name is empty or if no field was added.
     */
    bool isNull() const
    {
      return (pvName.isEmpty() || pvTableName.isEmpty() || pvFieldNameList.isEmpty());
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvIsUnique = false;
      pvName.clear();
      pvTableName.clear();
      pvFieldNameList.clear();
    }

   private:

    bool pvIsUnique;
    QString pvName;
    QString pvTableName;
    QStringList pvFieldNameList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef
