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
#include "mdtSqlTriggerSchema.h"
#include <QSqlDriver>

mdtSqlTriggerSchema::mdtSqlTriggerSchema()
 : pvIsTemporary(false),
   pvEvent(Unknown)
{

}

void mdtSqlTriggerSchema::setTemporary(bool temp)
{
  pvIsTemporary = temp;
}

void mdtSqlTriggerSchema::setName(const QString& name)
{
  pvName = name;
}

void mdtSqlTriggerSchema::setEvent(mdtSqlTriggerSchema::Event e)
{
  pvEvent = e;
}

void mdtSqlTriggerSchema::setTableName(const QString& name)
{
  pvTableName = name;
}

void mdtSqlTriggerSchema::setScript(const QString & sql)
{
  pvScript = sql;
}

void mdtSqlTriggerSchema::clear()
{
  pvIsTemporary = false;
  pvName.clear();
  pvEvent = Unknown;
  pvTableName.clear();
  pvScript.clear();
}

QString mdtSqlTriggerSchema::getSqlForCreateTrigger(const QSqlDatabase & db) const
{
  Q_ASSERT(db.isValid());

  QString sql;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);

  sql = "CREATE " + temporaryKeyWord() + "TRIGGER " + driver->escapeIdentifier(pvName, QSqlDriver::TableName) \
      + " " + eventStatement() + " ON " + driver->escapeIdentifier(pvTableName, QSqlDriver::TableName) \
      + "\nFOR EACH ROW\nBEGIN\n" \
      + pvScript \
      + "\nEND";

  return sql;
}

QString mdtSqlTriggerSchema::getSqlForDropTrigger(const QSqlDatabase & db) const
{
  Q_ASSERT(db.isValid());

  QString sql;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);

  sql = "DROP TRIGGER IF EXISTS " + driver->escapeIdentifier(pvName, QSqlDriver::TableName);

  return sql;
}

QString mdtSqlTriggerSchema::temporaryKeyWord() const
{
  if(pvIsTemporary){
    return "TEMPORARY ";
  }
  return "";
}

QString mdtSqlTriggerSchema::eventStatement() const
{
  switch(pvEvent){
    case Unknown:
      return "";
    case AfterInsert:
      return "AFTER INSERT";
  }
  return "";
}
