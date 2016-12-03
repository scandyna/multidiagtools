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
   * tp.addField(client.Name());
   * // Add data
   * tp.addRow(1, "Name 1");
   * tp.addRow(2, "Name 2");
   * \endcode
   *
   * To populate a database, you can use a Driver instance.
   *
   * \sa Driver
   * \sa Schema
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
      mName = name;
    }

    /*! \brief Get identification name
     */
    QString name() const
    {
      return mName;
    }

    /*! \brief Set table
     */
    void setTable(const Table & table)
    {
      mTableName = table.tableName();
    }

    /*! \brief Set table
     */
    template<typename T>
    void setTable(const TableTemplate<T> & table)
    {
      mTableName = table.tableName();
    }

    /*! \brief Set table name
     */
    void setTableName(const QString & name)
    {
      mTableName = name;
    }

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return mTableName;
    }

    /*! \brief Add a field
     *
     * \pre No data must exist in this table population when adding a field
     */
    void addField(const Field & field)
    {
      Q_ASSERT(mDataRowList.isEmpty());
      mFieldNameList.append( field.name() );
    }

    /*! \brief Add a field
     *
     * \pre No data must exist in this table population when adding a field
     */
    void addField(const AutoIncrementPrimaryKey & pk)
    {
      Q_ASSERT(mDataRowList.isEmpty());
      mFieldNameList.append( pk.fieldName() );
    }

    /*! \brief Add a field name
     *
     * \pre No data must exist in this table population when adding a field
     */
    void addFieldName(const QString & name)
    {
      Q_ASSERT(mDataRowList.isEmpty());
      mFieldNameList.append(name);
    }

    /*! \brief Get field names count
     */
    int fieldcount() const
    {
      return mFieldNameList.size();
    }

    /*! \brief Get field name at fieldIndex
     *
     * \pre fieldIndex must be in valid range
     */
    QString fieldName(int fieldIndex) const
    {
      Q_ASSERT(fieldIndex >= 0);
      Q_ASSERT(fieldIndex < mFieldNameList.size());
      return mFieldNameList.at(fieldIndex);
    }

    /*! \brief Get list of field names
     */
    QStringList fieldNameList() const
    {
      return mFieldNameList;
    }

    /*! \brief Add a row of data
     *
     * \pre The count of arguments in dataList must match fieldcount()
     */
    template<typename...Ts>
    void addRow(const Ts & ...dataList)
    {
      Q_ASSERT(sizeof...(dataList) == mFieldNameList.size());
      mDataRowList.append(QVariantList());
      mDataRowList[mDataRowList.size()-1].reserve(sizeof...(dataList));
      addRowImpl(dataList...);
    }

    /*! \brief Get row count
     */
    int rowCount() const
    {
      return mDataRowList.size();
    }

    /*! \brief Get data at given row and fieldIndex
     *
     * \pre row and fieldIndex must be in valid ranges
     */
    QVariant data(int row, int fieldIndex) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < mDataRowList.size());
      Q_ASSERT(fieldIndex >= 0);
      Q_ASSERT(fieldIndex < mFieldNameList.size());
      return mDataRowList.at(row).at(fieldIndex);
    }

    /*! \brief Clear
     */
    void clear()
    {
      mName.clear();
      mTableName.clear();
      mFieldNameList.clear();
      mDataRowList.clear();
    }

   private:

    /*! \brief Terminal condition for addRow()
     */
    void addRowImpl()
    {
    }

    /*! \brief Part of addRow(...)
     */
    template<typename T, typename...Ts>
    void addRowImpl(const T & data, const Ts & ...dataList)
    {
      mDataRowList[mDataRowList.size()-1].append(data);
      addRowImpl(dataList...);
    }

    QString mName;
    QString mTableName;
    QStringList mFieldNameList;
    QVector<QVariantList> mDataRowList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_POPULATION_H
