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
#include "mdtSqlDatabaseSchema.h"
#include "mdtSqlDatabaseSchemaThread.h"
#ifndef QT_NO_DEBUG
#include <QByteArray>
#endif // #ifndef QT_NO_DEBUG

void mdtSqlDatabaseSchema::addTable(const mdtSqlSchemaTable & st)
{
  pvTableList.append(st);
}

mdtSqlSchemaTable mdtSqlDatabaseSchema::table(const QString & tableName) const
{
  for(const auto & ts : pvTableList){
    if(ts.tableName() == tableName){
      return ts;
    }
  }
  return mdtSqlSchemaTable();
}

mdtSqlViewSchema::JoinClause mdtSqlDatabaseSchema::joinClause(const mdtSqlViewSchema::Table & mainTable, const mdtSqlViewSchema::Table & tableToJoin) const
{
  using namespace mdtSqlViewSchema;

  JoinClause join;
  mdtSqlSchemaTable mainTableSchema;
  mdtSqlSchemaTable tableToJoinSchema;
  mdtSqlForeignKey fk;
  bool revserse = false;  // true if main table is child table in relation
#ifndef QT_NO_DEBUG
  QString whatStr;
#endif // #ifndef QT_NO_DEBUG

  /*
   * Get requested tables
   */
  mainTableSchema = table(mainTable.tableName());
#ifndef QT_NO_DEBUG
  whatStr = "main table '" + mainTable.tableName() + "' not exists in database schema";
  Q_ASSERT_X(!mainTableSchema.tableName().isEmpty(), "get join clause", whatStr.toLocal8Bit().constData());
#endif // #ifndef QT_NO_DEBUG
  tableToJoinSchema = table(tableToJoin.tableName());
#ifndef QT_NO_DEBUG
  whatStr = "table to join '" + tableToJoin.tableName() + "' not exists in DB schema";
  Q_ASSERT_X(!tableToJoinSchema.tableName().isEmpty(), "get join clause", whatStr.toLocal8Bit().constData());
#endif // #ifndef QT_NO_DEBUG
  /*
   * Check witch table is child of the other, and get its foreign key.
   * Note: it is required that we can JOIN parent table TO a child table,
   *       in witch case child table is main table and parent table the table to join
   */
  // Check if table to join is the child table in relation (FK constraint)
  fk = tableToJoinSchema.foreignKeyReferencing(mainTableSchema.tableName());
  if(fk.parentTableName() != mainTable.tableName()){
    // Main table should be the child table in relation (FK constraint)
    fk = mainTableSchema.foreignKeyReferencing(tableToJoinSchema.tableName());
    revserse = true;
  }
  // Here, FK must be defined
#ifndef QT_NO_DEBUG
  whatStr = "could not find a relation between '" + mainTable.tableName() + "' and '" + tableToJoin.tableName() + "'";
  Q_ASSERT_X(!fk.parentTableName().isEmpty(), "get join clause", whatStr.toLocal8Bit().constData());
  Q_ASSERT_X(!fk.childTableName().isEmpty(), "get join clause", whatStr.toLocal8Bit().constData());
#endif // #ifndef QT_NO_DEBUG
  // Get table to join's FK
  Q_ASSERT(fk.parentTableFields().size() == fk.childTableFields().size());
  Q_ASSERT(fk.parentTableFields().size() > 0);
  // Setup join clause
  join.setMainTable(mainTable);
  join.setTableToJoin(tableToJoin);
  if(revserse){
    JoinKey key;
    key.setMainTableField(fk.childTableFields().at(0));
    key.setTableToJoinField(fk.parentTableFields().at(0));
    join.addKey(key);
    for(int i = 1; i < fk.parentTableFields().size(); ++i){
      JoinKey key;
      key.setConstraintOperator(JoinKey::And);
      key.setMainTableField(fk.childTableFields().at(i));
      key.setTableToJoinField(fk.parentTableFields().at(i));
      join.addKey(key);
    }
  }else{
    JoinKey key;
    key.setMainTableField(fk.parentTableFields().at(0));
    key.setTableToJoinField(fk.childTableFields().at(0));
    join.addKey(key);
    for(int i = 1; i < fk.parentTableFields().size(); ++i){
      JoinKey key;
      key.setConstraintOperator(JoinKey::And);
      key.setMainTableField(fk.parentTableFields().at(i));
      key.setTableToJoinField(fk.childTableFields().at(i));
      join.addKey(key);
    }
  }

  return join;
}

void mdtSqlDatabaseSchema::addView(const mdtSqlViewSchema::Schema & vs)
{
  pvViewList.append(vs);
}

void mdtSqlDatabaseSchema::addTablePopulation(const mdtSqlTablePopulationSchema& tps)
{
  pvTablePopulationSchemaList.append(tps);
}

void mdtSqlDatabaseSchema::addTrigger(const mdtSqlTriggerSchema & trigger)
{
  pvTriggerList.append(trigger);
}

bool mdtSqlDatabaseSchema::createSchema(const QSqlDatabase& db)
{
  mdtSqlDatabaseSchemaThread thread;

  return thread.createSchemaBlocking(*this, db);
}
