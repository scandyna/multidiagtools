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
#ifndef MDT_SQL_TRIGGER_SCHEMA_H
#define MDT_SQL_TRIGGER_SCHEMA_H

#include <QString>
#include <QSqlDatabase>

/*! \brief SQL trigger definition
 *
 * Typical usage:
 * \code
 * mdtSqlTriggerSchema trigger;
 *
 * trigger.setName("SomeTrigger");
 * trigger.setEvent(mdtSqlTriggerSchema::AfterInsert);
 * trigger.setTableName("Client_tbl");
 * trigger.setScript("UPDATE Client_tbl SET Name = 'Some name';");
 * \endcode
 */
class mdtSqlTriggerSchema
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
  mdtSqlTriggerSchema();

  /*! \brief Set temporary flag
   */
  void setTemporary(bool temp);

  /*! \brief Check temporary flag
   */
  bool isTemporary() const
  {
    return pvIsTemporary;
  }

  /*! \brief Set trigger name
   */
  void setName(const QString & name);

  /*! \brief Get trigger name
   */
  QString name() const
  {
    return pvName;
  }

  /*! \brief Set event
   */
  void setEvent(Event e);

  /*! \brief Get event
   */
  Event event() const
  {
    return pvEvent;
  }

  /*! \brief Set table name
   */
  void setTableName(const QString & name);

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
  void setScript(const QString & sql);

  /*! \brief Get script
   */
  QString script() const
  {
    return pvScript;
  }

  /*! \brief Clear
   */
  void clear();

  /*! \brief Get SQL statements for table creation
   *
   * \pre db's driver must be loaded
   */
  QString getSqlForCreateTrigger(const QSqlDatabase & db) const;

  /*! \brief Get SQL statements for table deletion
   *
   * \pre db's driver must be loaded
   */
  QString getSqlForDropTrigger(const QSqlDatabase & db) const;

 private:

  /*! \brief Get temporary key word
   */
  QString temporaryKeyWord() const;

  /*! \brief Get event statemenet
   */
  QString eventStatement() const;

  bool pvIsTemporary;
  Event pvEvent;
  QString pvName;
  QString pvTableName;
  QString pvScript;
};

#endif // #ifndef MDT_SQL_TRIGGER_SCHEMA_H
