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
#include "JoinConstraintFieldPairList.h"
#include "SelectTable.h"
#include "Schema/ForeignKey.h"
#include <QStringList>
#include <QString>
#include <QByteArray>

namespace Mdt{ namespace Sql{

JoinConstraintFieldPairList JoinConstraintFieldPairList::fromTables(const SelectTable & left, const SelectTable & right)
{
  JoinConstraintFieldPairList fpList(left.aliasOrTableName(), right.aliasOrTableName());
  QStringList leftFieldList;
  QStringList rightFieldList;
  Schema::ForeignKey fk;

  // Get the foreign key that links left and right
  fk = left.foreignKeyReferencing(right);
  if(!fk.isNull()){
    leftFieldList = fk.childTableFieldNameList();
    rightFieldList = fk.parentTableFieldNameList();
  }else{
    fk = right.foreignKeyReferencing(left);
    if(!fk.isNull()){
      leftFieldList = fk.parentTableFieldNameList();
      rightFieldList = fk.childTableFieldNameList();
    }
  }
  /*
   * If a precondition is not satisfied, generate a message.
   * This seems heavy, but can be very usefull for the user.
   */
#ifndef QT_NO_DEBUG
  QString whatStr;
  // Check that a foreign key was found.
  whatStr = QString("Deducing relation between '%1' and '%2' is not possible because no foreign key is linking them together.").arg(left.aliasOrTableName(), right.aliasOrTableName());
  Q_ASSERT_X(!fk.isNull(), "JoinConstraintFieldPairList::fromTables()", whatStr.toLocal8Bit().constData());
  // Check that parent and child field count is the same
  whatStr = QString("Deducing relation between '%1' and '%2' is not possible because foreign key that links them together has not the same field count in parent (refering) and child.").arg(left.aliasOrTableName(), right.aliasOrTableName());
  Q_ASSERT_X(leftFieldList.size() == rightFieldList.size(), "JoinConstraintFieldPairList::fromTables()", whatStr.toLocal8Bit().constData());
#endif // #ifndef QT_NO_DEBUG
  // Build list of field pairs
  for(int i = 0; i < leftFieldList.size(); ++i){
    fpList.addFieldPair(leftFieldList.at(i), rightFieldList.at(i));
  }

  return fpList;
}

}} // namespace Mdt{ namespace Sql{
