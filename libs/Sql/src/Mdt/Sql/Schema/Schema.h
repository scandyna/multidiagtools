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
#ifndef MDT_SQL_SCHEMA_SCHEMA_H
#define MDT_SQL_SCHEMA_SCHEMA_H

#include "Table.h"
#include "TableList.h"
#include "View.h"
#include "ViewList.h"
#include "TablePopulation.h"
#include "TablePopulationList.h"
#include "Trigger.h"
#include "TriggerList.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  template<typename T>
  class TableTemplate;

  template<typename T>
  class ViewTemplate;

  /*! \brief Container for database schema
   */
  class Schema
  {
   public:

    /*! \brief Add a table
     */
    void addTable(const Table & table);

    /*! \brief Add a table
     */
    template<typename T>
    void addTable(const TableTemplate<T> & table)
    {
      addTable(table.toTable());
    }

    /*! \brief Get tables count
     */
    int tableCount() const
    {
      return pvTableList.size();
    }

    /*! \brief Get table name for given index
     *
     * \pre index must be in a valid range
     */
    QString tableName(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvTableList.size());
      return pvTableList.at(index).tableName();
    }

    /*! \brief Get all tables
     */
    TableList tableList() const
    {
      return pvTableList;
    }

    /*! \brief Add a view
     */
    void addView(const View & view);

    /*! \brief Add a view
     */
    template<typename T>
    void addView(const ViewTemplate<T> & view)
    {
      addView(view.toView());
    }

    /*! \brief Get view schema count
     */
    int viewCount() const
    {
      return pvViewList.size();
    }

    /*! \brief Get view name for given index
     *
     * \pre index must be in a valid range
     */
    QString viewName(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvViewList.size());
      return pvViewList.at(index).name();
    }

    /*! \brief Get all views
     */
    ViewList viewList() const
    {
      return pvViewList;
    }

    /*! \brief Add a table population
     */
    void addTablePopulation(const TablePopulation & tp);

    /*! \brief Get count of table population
     */
    int tablePopulationCount() const
    {
      return pvTablePopulationList.size();
    }

    /*! \brief Get table population name
     *
     * \pre index must be in a valid range
     */
    QString tablePopulationName(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvTablePopulationList.size());
      return pvTablePopulationList.at(index).name();
    }

    /*! \brief Get all table populations
     */
    TablePopulationList tablePopulationList() const
    {
      return pvTablePopulationList;
    }

    /*! \brief Add a trigger
     */
    void addTrigger(const Trigger & trigger);

    /*! \brief Get trigger count
     */
    int triggerCount() const
    {
      return pvTriggerList.size();
    }

    /*! \brief Get trigger name
     *
     * \pre index must be in a valid range
     */
    QString triggerName(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < pvTriggerList.size());
      return pvTriggerList.at(index).name();
    }

    /*! \brief Get all triggers
     */
    TriggerList triggerList() const
    {
      return pvTriggerList;
    }

    /*! \brief Clear schema
     */
    void clear();

   private:

    TableList pvTableList;
    ViewList pvViewList;
    TablePopulationList pvTablePopulationList;
    TriggerList pvTriggerList;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_SCHEMA_H
