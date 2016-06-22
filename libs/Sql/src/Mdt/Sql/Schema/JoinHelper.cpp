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
#include "TableName.h"
#include "ViewTable.h"
#include "MainTableField.h"
#include "TableToJoinField.h"
#include <QStringList>

// #include <QDebug>

namespace Mdt{ namespace Sql{ namespace Schema{

JoinClause JoinHelper::joinClauseFromForeignKey(const ForeignKey & fk, JoinHelper::MainTableRule rule,
                                                const QString & mainTableAlias , const QString & tableToJoinAlias)
{
  JoinClause join;
  ViewTable mainTable;
  ViewTable tableToJoin;

  if(rule == ParentIsMain){
    mainTable.setTableName(fk.parentTableName());
    tableToJoin.setTableName(fk.childTableName());
  }else{
    mainTable.setTableName(fk.childTableName());
    tableToJoin.setTableName(fk.parentTableName());
  }
  mainTable.setAlias(mainTableAlias);
  tableToJoin.setAlias(tableToJoinAlias);
  join.setMainTable(mainTable);
  join.setTableToJoin(tableToJoin);
  addKeysToJoinClause(join, fk, rule);

  return join;
}

JoinClause JoinHelper::joinClauseFromTables(const Table & mainTable, const Table & tableToJoin,
                                            const QString & mainTableAlias, const QString & tableToJoinAlias)
{
  JoinClause join;
  ForeignKey fk;

  /*
   * Note: it can be confusing why the requested mainTable will allways remain the main table of the join clause
   *       in following code.
   * The reason is: depending on which table we get the foreign key from,
   *                its parent and child table can be crossed (case of FK mainTable -> tableToJoin)
   */
  fk = tableToJoin.foreignKeyReferencing(mainTable.tableName());
  if(fk.isNull()){
    fk = mainTable.foreignKeyReferencing(tableToJoin.tableName());
    if(fk.isNull()){
      return join;
    }
    join = joinClauseFromForeignKey(fk, ChildIsMain, mainTableAlias, tableToJoinAlias);
  }else{
    join = joinClauseFromForeignKey(fk, ParentIsMain, mainTableAlias, tableToJoinAlias);
  }
  Q_ASSERT(QString::compare(join.mainTable().tableName(), mainTable.tableName(), Qt::CaseInsensitive) == 0);
  Q_ASSERT(QString::compare(join.tableToJoin().tableName(), tableToJoin.tableName(), Qt::CaseInsensitive) == 0);

  return join;
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
  const int n = mainTableFieldNameList.size();
  for(int i = 0; i < n; ++i){
    join.addKey( MainTableField(mainTableFieldNameList.at(i)), TableToJoinField(tableToJoinFieldNameList.at(i)) );
  }
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
