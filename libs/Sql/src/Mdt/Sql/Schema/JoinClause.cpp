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
#include "JoinClause.h"
#include "JoinKey.h"

namespace Mdt{ namespace Sql{ namespace Schema{

void JoinClause::addKey(JoinOperator::ConstraintOperator constraintOperator, const MainTableField & mainTableField,
                        JoinOperator::RelativeFieldComparisonOperator fieldOperator, const TableToJoinField & tableToJoinField)
{
  JoinKey key;

  key.setConstraintOperator(constraintOperator);
  switch(fieldOperator){
    case JoinOperator::LessThan:
      key.setFieldComparisonOperator(JoinOperator::MtfLessThanTdjf);
      break;
    case JoinOperator::Equal:
      key.setFieldComparisonOperator(JoinOperator::MtfEqualTdjf);
      break;
    case JoinOperator::GreaterThan:
      key.setFieldComparisonOperator(JoinOperator::MtfGreaterThanTdjf);
      break; 
  }
  key.setMainTableFieldName(mainTableField.fieldName());
  key.setTableToJoinFieldName(tableToJoinField.fieldName());
  pvKeyList.append(key);
}

void JoinClause::addKey(const MainTableField & mainTableField, JoinOperator::RelativeFieldComparisonOperator fieldOperator,
                        const TableToJoinField & tableToJoinField)
{
  if(pvKeyList.isEmpty()){
    addKey(JoinOperator::On, mainTableField, fieldOperator, tableToJoinField);
  }else{
    addKey(JoinOperator::And, mainTableField, fieldOperator, tableToJoinField);
  }
}

void JoinClause::addKey(const MainTableField& mainTableField, const TableToJoinField& tableToJoinField)
{
  addKey(mainTableField, JoinOperator::Equal, tableToJoinField);
}

void JoinClause::clear()
{
  pvOperator = JoinOperator::Join;
  pvMainTable.clear();
  pvTableToJoin.clear();
  pvKeyList.clear();
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{