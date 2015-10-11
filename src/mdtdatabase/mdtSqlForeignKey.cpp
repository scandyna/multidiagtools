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
#include "mdtSqlForeignKey.h"
#include "mdtSqlIndex.h"
#include "mdtSqlField.h"
#include <QSqlDatabase>
#include <QSqlDriver>

void mdtSqlForeignKey::addKeyFields(const QString & parentTableFieldName, const mdtSqlField & childTableField)
{
  Q_ASSERT(!childTableField.name().isEmpty());
  addKeyFields(parentTableFieldName, childTableField.name());
}

void mdtSqlForeignKey::clear()
{
  pvCreateParentIndex = false;
  pvCreateChildIndex = false;
  pvOnDeleteAction = NoAction;
  pvOnUpdateAction = NoAction;
  pvParentTableName.clear();
  pvChildTableName.clear();
  pvParentTableFields.clear();
  pvChildTableFields.clear();
}

QString mdtSqlForeignKey::getSqlForForeignKey(const QSqlDatabase & db) const
{
  QString sql;
  int lastFieldIndex;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  Q_ASSERT(pvParentTableFields.size() == pvChildTableFields.size());

  if(pvParentTableFields.isEmpty() || pvChildTableFields.isEmpty()){
    return sql;
  }
  sql = "  FOREIGN KEY (";
  // Add fields from table
  lastFieldIndex = pvChildTableFields.size() - 1;
  Q_ASSERT(lastFieldIndex >= 0);
  for(int i = 0; i < lastFieldIndex; ++i){
    sql += driver->escapeIdentifier(pvChildTableFields.at(i), QSqlDriver::FieldName) + ",";
  }
  sql += driver->escapeIdentifier(pvChildTableFields.at(lastFieldIndex), QSqlDriver::FieldName) + ")\n";
  sql += "   REFERENCES " + driver->escapeIdentifier(pvParentTableName, QSqlDriver::TableName) + " (";
  // Add fields from parent (referenced) table
  lastFieldIndex = pvParentTableFields.size() - 1;
  Q_ASSERT(lastFieldIndex >= 0);
  for(int i = 0; i < lastFieldIndex; ++i){
    sql += driver->escapeIdentifier(pvParentTableFields.at(i), QSqlDriver::FieldName) + ",";
  }
  sql += driver->escapeIdentifier(pvParentTableFields.at(lastFieldIndex), QSqlDriver::FieldName) + ")\n";
  // Add actions
  sql += "   ON DELETE " + actionStr(pvOnDeleteAction) + "\n";
  sql += "   ON UPDATE " + actionStr(pvOnUpdateAction);

  return sql;
}

QString mdtSqlForeignKey::getSqlForDropParentTableIndex(const QSqlDatabase & db, const QString & databaseName) const
{
  Q_ASSERT(db.isValid());

  mdtSqlIndex index;

  index.setTableName(pvParentTableName);
  for(const auto & fn : pvParentTableFields){
    index.addField(fn);
  }
  index.generateName();

  return index.getSqlForDrop(db, databaseName);
}

QString mdtSqlForeignKey::getSqlForCreateParentTableIndex(const QSqlDatabase & db, const QString & databaseName) const
{
  Q_ASSERT(db.isValid());

  mdtSqlIndex index;

  index.setTableName(pvParentTableName);
  for(const auto & fn : pvParentTableFields){
    index.addField(fn);
  }
  index.generateName();

  return index.getSqlForCreate(db, databaseName);
}

QString mdtSqlForeignKey::getSqlForDropChildTableIndex(const QSqlDatabase & db, const QString & databaseName) const
{
  Q_ASSERT(db.isValid());

  mdtSqlIndex index;

  index.setTableName(pvChildTableName);
  for(const auto & fn : pvChildTableFields){
    index.addField(fn);
  }
  index.generateName();

  return index.getSqlForDrop(db, databaseName);
}

QString mdtSqlForeignKey::getSqlForCreateChildTableIndex(const QSqlDatabase & db, const QString & databaseName) const
{
  Q_ASSERT(db.isValid());

  mdtSqlIndex index;

  index.setTableName(pvChildTableName);
  for(const auto & fn : pvChildTableFields){
    index.addField(fn);
  }
  index.generateName();

  return index.getSqlForCreate(db, databaseName);
}

QString mdtSqlForeignKey::actionStr(mdtSqlForeignKey::Action action) const
{
  switch(action){
    case NoAction:
      return "NO ACTION";
    case Restrict:
      return "RESTRICT";
    case SetNull:
      return "SET NULL";
    case SetDefault:
      return "SET DEFAULT";
    case Cascade:
      return "CASCADE";
  }
  return QString();
}
