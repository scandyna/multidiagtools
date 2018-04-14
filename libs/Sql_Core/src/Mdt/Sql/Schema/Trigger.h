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
#ifndef MDT_SQL_SCHEMA_TRIGGER_H
#define MDT_SQL_SCHEMA_TRIGGER_H

#include "Table.h"
#include "TableTemplate.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief SQL trigger definition
   *
   * Typical usage:
   * \code
   * using Mdt::Sql::Schema::Trigger;
   *
   * // Get instance of defined tables
   * Client_tbl client;
   *
   * Trigger trigger;
   *
   * trigger.setName("SomeTrigger");
   * trigger.setEvent(mdtSqlTriggerSchema::AfterInsert);
   * trigger.setTable(client);
   * trigger.setScript("UPDATE Client_tbl SET Name = 'Some name';");
   * \endcode
   */
  class Trigger
  {
   public:

    /*! \brief Trigger event
     */
    enum Event
    {
      Unknown,      /*!< Unknown event (this is the default event) */
      AfterInsert   /*!< AFTER INSERT */
    };

    /*! \brief Constructor
     */
    Trigger()
     : pvIsTemporary(false), pvEvent(Unknown) {}

    /*! \brief Set temporary flag
     */
    void setTemporary(bool temp)
    {
      pvIsTemporary = temp;
    }

    /*! \brief Check temporary flag
     */
    bool isTemporary() const
    {
      return pvIsTemporary;
    }

    /*! \brief Set trigger name
     */
    void setName(const QString & name)
    {
      pvName = name;
    }

    /*! \brief Get trigger name
     */
    QString name() const
    {
      return pvName;
    }

    /*! \brief Set event
     */
    void setEvent(Event e)
    {
      pvEvent = e;
    }

    /*! \brief Get event
     */
    Event event() const
    {
      return pvEvent;
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

    /*! \brief Set script
     *
     * Script is the part between BEGIN and END in trigger SQL.
     */
    void setScript(const QString & sql)
    {
      pvScript = sql;
    }

    /*! \brief Get script
     */
    QString script() const
    {
      return pvScript;
    }

    /*! \brief Clear
     */
    void clear()
    {
      pvIsTemporary = false;
      pvEvent = Unknown;
      pvName.clear();
      pvTableName.clear();
      pvScript.clear();
    }

   private:

    bool pvIsTemporary;
    Event pvEvent;
    QString pvName;
    QString pvTableName;
    QString pvScript;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_TRIGGER_H
