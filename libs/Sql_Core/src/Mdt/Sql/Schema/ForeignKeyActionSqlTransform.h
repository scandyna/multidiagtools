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
#ifndef MDT_SQL_SCHEMA_FOREIGN_KEY_ACTION_SQL_TRANSFORM_H
#define MDT_SQL_SCHEMA_FOREIGN_KEY_ACTION_SQL_TRANSFORM_H

#include "ForeignKeyAction.h"
#include <QString>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Transform a ForeignKeyAction to its SQL representation
   */
  class ForeignKeyActionSqlTransform
  {
   public:

    /*! \brief Get SQL representation of action
     */
    static QString getSql(ForeignKeyAction action);

    /*! \brief Get action from string representation
     */
    static ForeignKeyAction fromActionString(const QString & actionStr);
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FOREIGN_KEY_ACTION_SQL_TRANSFORM_H
