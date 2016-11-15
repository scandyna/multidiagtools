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
#ifndef MDT_SQL_SCHEMA_TABLE_POPULATION_H
#define MDT_SQL_SCHEMA_TABLE_POPULATION_H

#include "Table.h"
#include "TableTemplate.h"
#include "Field.h"
#include "AutoIncrementPrimaryKey.h"
// #include "SingleFieldPrimaryKey.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantList>
#include <QVector>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Simple container that holds data to populate on a specific table
   *
   * Typical usage:
   * \code
   * using Mdt::Sql::Schema::TablePopulation;
   *
   * // Use instance of allready defined table
   * Client_tbl client;
   *
   * TablePopulation tp;
   *
   * // Name and table name
   * tp.setName("Client_tbl data");
   * tp.setTable(client);
   * // Add fields
   * tp.addField(client.Id_PK());
   * tp.addFieldName(client.Name());
   * // Add data
   * tp << 1 << "Name 1";
   * tp.commitCurrentRow();
   * tp << 2 << "Name 2";
   * tp.commitCurrentRow();
   * \endcode
   */
  class TablePopulation
  {
   public:

    /*! \brief Set identification name
     *
     * This name will not interact with the database,
     *  but is used when database schema is presented to the user.
     */
    void setName(const QString & name)
    {
      pvName = name;
    }

    /*! \brief Get identification name
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

    /*! \brief Set table
     */
    template<typename T>
    void setTable(const TableTemplate<T> & table)
    {
      pvTableName = table.tableName();
    }

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return pvTableName;
    }

    /*! \brief Add a field
     *
     * \pre No data must exist in this table population when adding a field
     */
    void addField(const Field & field)
    {
      Q_ASSERT(pvCurrentDataRow.isEmpty());
      Q_ASSERT(pvDataRowList.isEmpty());
      pvFieldNameList.append( field.name() );
    }

    /*! \brief Add a field
     *
     * \pre No data must exist in this table population when adding a field
     */
    void addField(const AutoIncrementPrimaryKey & pk)
    {
      Q_ASSERT(pvCurrentDataRow.isEmpty());
      Q_ASSERT(pvDataRowList.isEmpty());
      pvFieldNameList.append( pk.fieldName() );
    }

    /*! \brief Add a field
     *
     * \pre No data must exist in this table population when adding a field
     */
//     void addField(const SingleFieldPrimaryKey & pk)
//     {
//       Q_ASSERT(pvCurrentDataRow.isEmpty());
//       Q_ASSERT(pvDataRowList.isEmpty());
//       pvFieldNameList.append( pk.fieldName() );
//     }

    /*! \brief Get field names count
     */
    int fieldcount() const
    {
      return pvFieldNameList.size();
    }

    /*! \brief Get field name at fieldIndex
     *
     * \pre fieldIndex must be in valid range
     */
    QString fieldName(int fieldIndex) const
    {
      Q_ASSERT(fieldIndex >= 0);
      Q_ASSERT(fieldIndex < pvFieldNameList.size());
      return pvFieldNameList.at(fieldIndex);
    }

    /*! \brief Get list of field names
     */
    QStringList fieldNameList() const
    {
      return pvFieldNameList;
    }

    /*! \brief Stream data into current row
     */
    QVariantList & operator<<(const QVariant & value)
    {
      pvCurrentDataRow << value;
      Q_ASSERT(pvCurrentDataRow.size() <= pvFieldNameList.size());
      return pvCurrentDataRow;
    }

    /*! \brief Commit current row of data
     *
     * \pre Current row of data must contain the same amount of elements than fieldNameList
     */
    void commitCurrentRow()
    {
      Q_ASSERT(pvCurrentDataRow.size() == pvFieldNameList.size());
      pvDataRowList << pvCurrentDataRow;
      pvCurrentDataRow.clear();
    }

    /*! \brief Get row count
     */
    int rowCount() const
    {
      return pvDataRowList.size();
    }

    /*! \brief Get data at given row and fieldIndex
     *
     * \pre row and fieldIndex must be in valid ranges
     */
    QVariant data(int row, int fieldIndex) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < pvDataRowList.size());
      Q_ASSERT(fieldIndex >= 0);
      Q_ASSERT(fieldIndex < pvFieldNameList.size());
      return pvDataRowList.at(row).at(fieldIndex);
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvName.clear();
      pvTableName.clear();
      pvFieldNameList.clear();
      pvCurrentDataRow.clear();
      pvDataRowList.clear();
    }

   private:

    QString pvName;
    QString pvTableName;
    QStringList pvFieldNameList;
    QVariantList pvCurrentDataRow;
    QVector<QVariantList> pvDataRowList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_POPULATION_H
