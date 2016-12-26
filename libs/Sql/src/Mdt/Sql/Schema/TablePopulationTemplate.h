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
#ifndef MDT_SQL_SCHEMA_TABLE_POPULATION_TEMPLATE_H
#define MDT_SQL_SCHEMA_TABLE_POPULATION_TEMPLATE_H

#include "TablePopulation.h"

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Template class to define SQL schema table population
   *
   * TablePopulationTemplate is a wraper of TablePopulation that permit to define
   *  a schema table population.
   *
   * Example of ClientPopulation.h:
   * \include libs/Sql/tests/src/Schema/ClientPopulation.h
   *
   * ClientPopulation.cpp would look like this:
   * \include libs/Sql/tests/src/Schema/ClientPopulation.cpp
   */
  template<typename Derived>
  class TablePopulationTemplate
  {
   public:

    /*! \brief Get table population instance
     */
    TablePopulation toTablePopulation() const
    {
      return mPopulation;
    }

    /*! \brief Get identification name
     */
    QString name() const
    {
      return mPopulation.name();
    }

    /*! \brief Get table name
     */
    QString tableName() const
    {
      return mPopulation.tableName();
    }

    /*! \brief Get field names count
     */
    int fieldcount() const
    {
      return mPopulation.fieldcount();
    }

    /*! \brief Get field name at fieldIndex
     *
     * \pre fieldIndex must be in valid range
     */
    QString fieldName(int fieldIndex) const
    {
      Q_ASSERT(fieldIndex >= 0);
      Q_ASSERT(fieldIndex < fieldcount());
      return mPopulation.fieldName(fieldIndex);
    }

    /*! \brief Get list of field names
     */
    QStringList fieldNameList() const
    {
      return mPopulation.fieldNameList();
    }

    /*! \brief Get row count
     */
    int rowCount() const
    {
      return mPopulation.rowCount();
    }

    /*! \brief Get data at given row and fieldIndex
     *
     * \pre row and fieldIndex must be in valid ranges
     */
    QVariant data(int row, int fieldIndex) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      Q_ASSERT(fieldIndex >= 0);
      Q_ASSERT(fieldIndex < fieldcount());
      return mPopulation.data(row, fieldIndex);
    }

   protected:

    /*! \brief Set identification name
     *
     * This name will not interact with the database,
     *  but is used when database schema is presented to the user.
     */
    void setName(const QString & name)
    {
      mPopulation.setName(name);
    }

    /*! \brief Set table
     */
    void setTable(const Table & table)
    {
      mPopulation.setTable(table);
    }

    /*! \brief Set table
     */
    template<typename T>
    void setTable(const TableTemplate<T> & table)
    {
      mPopulation.setTable(table);
    }

    /*! \brief Set table name
     */
    void setTableName(const QString & name)
    {
      mPopulation.setTableName(name);
    }

    /*! \brief Add a field
     *
     * \pre No data must exist in this table population when adding a field
     */
    void addField(const Field & field)
    {
      mPopulation.addField(field);
    }

    /*! \brief Add a field
     *
     * \pre No data must exist in this table population when adding a field
     */
    void addField(const AutoIncrementPrimaryKey & pk)
    {
      mPopulation.addField(pk);
    }

    /*! \brief Add a field name
     *
     * \pre No data must exist in this table population when adding a field
     */
    void addFieldName(const QString & name)
    {
      mPopulation.addFieldName(name);
    }

    /*! \brief Add a row of data
     *
     * \pre The count of arguments in dataList must match fieldcount()
     */
    template<typename...Ts>
    void addRow(const Ts & ...dataList)
    {
      mPopulation.addRow(dataList...);
    }

   private:

    TablePopulation mPopulation;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TABLE_POPULATION_TEMPLATE_H
