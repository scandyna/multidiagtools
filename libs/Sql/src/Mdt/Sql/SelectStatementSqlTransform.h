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
#ifndef MDT_SQL_SELECT_STATEMENT_SQL_TRANSFORM_H
#define MDT_SQL_SELECT_STATEMENT_SQL_TRANSFORM_H

#include "SelectOperator.h"
#include <QString>

class QSqlDatabase;

namespace Mdt{ namespace Sql{

  class SelectStatement;

  /*! \brief Transform a SelectStatement to its SQL representation
   */
  class SelectStatementSqlTransform
  {
   public:

    /*! \brief Get SQL string of stm
     *
     * \pre stm must have a non empty list of select fields
     * \pre db must be valid (must have a driver loaded)
     */
    static QString getSql(const SelectStatement & stm, const QSqlDatabase & db);

   private:

    static QString getSelectOperatorString(SelectOperator op);
  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SELECT_STATEMENT_SQL_TRANSFORM_H
