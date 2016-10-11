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
#include "JoinHelper.h"
#include "Table.h"
#include "ForeignKey.h"
#include "../TableName.h"
#include "ViewTable.h"
#include "MainTableField.h"
#include "TableToJoinField.h"
#include "View.h"
#include "ViewTable.h"
#include <QStringList>
#include <QString>
#include <QByteArray>

// #include <QDebug>

namespace Mdt{ namespace Sql{ namespace Schema{

JoinClause JoinHelper::joinClauseFromTables(const Table& mainTable, const ViewTable& mainViewTable,
                                            const Table& tableToJoin, const ViewTable& viewTableToJoin,
                                            JoinOperator::Operator joinOperator)
{
  JoinClause join;
  ForeignKey fk;

  /*
   * Build debug string in case a precondition is not stisfied.
   * Seems heavy, but can be very usefull when defining a schema.
   */
#ifndef QT_NO_DEBUG
  QString whatStr;
  whatStr = QString("mainTable (%1) and mainViewTable (%2) are not refering to the same table").arg(mainTable.tableName()).arg(mainViewTable.tableName());
  Q_ASSERT_X(mainTable.tableName() == mainViewTable.tableName(), "JoinHelper::joinClauseFromTables()", whatStr.toLocal8Bit().constData());
  whatStr = QString("tableToJoin (%1) and viewTableToJoin (%2) are not refering to the same table").arg(tableToJoin.tableName()).arg(viewTableToJoin.tableName());
  Q_ASSERT_X(tableToJoin.tableName() == viewTableToJoin.tableName(), "JoinHelper::joinClauseFromTables()", whatStr.toLocal8Bit().constData());
#endif // #ifndef QT_NO_DEBUG

  /*
   * Note: it can be confusing why the requested mainTable will allways remain the main table of the join clause
   *       in following code.
   * The reason is: depending on which table we get the foreign key from,
   *                its parent and child table can be crossed (case of FK mainTable -> tableToJoin)
   */
  fk = tableToJoin.foreignKeyReferencing(mainTable.tableName());
  if(fk.isNull()){
    fk = mainTable.foreignKeyReferencing(tableToJoin.tableName());
#ifndef QT_NO_DEBUG
    whatStr = QString("no foreign key was found between table '%1' and '%2'").arg(mainTable.tableName()).arg(tableToJoin.tableName());
    Q_ASSERT_X(!fk.isNull(), "JoinHelper::joinClauseFromTables()", whatStr.toLocal8Bit().constData());
#endif // #ifndef QT_NO_DEBUG
    fillJoinClause(join, fk, ChildIsMain, mainViewTable, viewTableToJoin, joinOperator);
  }else{
    fillJoinClause(join, fk, ParentIsMain, mainViewTable, viewTableToJoin, joinOperator);
  }
  Q_ASSERT(join.mainTable().tableName() == mainTable.tableName());
  Q_ASSERT(join.tableToJoin().tableName() == tableToJoin.tableName());

  return join;
}

JoinClause JoinHelper::joinClauseFromTables(const Table& mainTable, const View& view,
                                            const Table& tableToJoin, const ViewTable& viewTableToJoin,
                                            JoinOperator::Operator joinOperator)
{
  Q_ASSERT(view.tableName() == mainTable.tableName());

  return joinClauseFromTables(mainTable, view.table(), tableToJoin, viewTableToJoin, joinOperator);
}

void JoinHelper::fillJoinClause(JoinClause& join, const ForeignKey& fk, JoinHelper::MainTableRule rule,
                                const ViewTable& mainViewTable, const ViewTable& viewTableToJoin,
                                JoinOperator::Operator joinOperator)
{
  Q_ASSERT(!fk.parentTableName().isEmpty());
  Q_ASSERT(!fk.childTableName().isEmpty());

#ifndef QT_NO_DEBUG
  if(rule == ParentIsMain){
    Q_ASSERT(mainViewTable.tableName() == fk.parentTableName());
    Q_ASSERT(viewTableToJoin.tableName() == fk.childTableName());
  }else{
    Q_ASSERT(mainViewTable.tableName() == fk.childTableName());
    Q_ASSERT(viewTableToJoin.tableName() == fk.parentTableName());
  }
#endif // #ifndef QT_NO_DEBUG
  join.setMainTable(mainViewTable);
  join.setTableToJoin(viewTableToJoin);
  addKeysToJoinClause(join, fk, rule);
  join.setJoinOperator(joinOperator);
}

void JoinHelper::addKeysToJoinClause(JoinClause& join, const ForeignKey & fk, JoinHelper::MainTableRule rule)
{
  QStringList mainTableFieldNameList;
  QStringList tableToJoinFieldNameList;

  if(rule == ParentIsMain){
    mainTableFieldNameList = fk.parentTableFieldNameList();
    tableToJoinFieldNameList = fk.childTableFieldNameList();
  }else{
    mainTableFieldNameList = fk.childTableFieldNameList();
    tableToJoinFieldNameList = fk.parentTableFieldNameList();
  }
  Q_ASSERT(mainTableFieldNameList.size() == tableToJoinFieldNameList.size());
  Q_ASSERT(!mainTableFieldNameList.isEmpty());
  const int n = mainTableFieldNameList.size();
  for(int i = 0; i < n; ++i){
    join.addKey( MainTableField(mainTableFieldNameList.at(i)), TableToJoinField(tableToJoinFieldNameList.at(i)) );
  }
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
