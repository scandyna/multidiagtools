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
#ifndef MDT_SQL_JOIN_CONSTRAINT_FIELD_H
#define MDT_SQL_JOIN_CONSTRAINT_FIELD_H

#include "Expression/JoinConstraint/TableFieldTerminal.h"

namespace Mdt{ namespace Sql{

  struct JoinConstraintFieldTag
  {
  };

  /*! \brief Field terminal used in a JoinConstraintExpression
   *
   * Typical usage:
   * \code
   * #include <Mdt/Sql/JoinConstraintField.h>
   *
   * using Mdt::Sql::JoinConstraintField;
   * using Mdt::Sql::TableName;
   * using Mdt::Sql::FieldName;
   *
   * JoinConstraintField cliendId(TableName("Client_tbl"), FieldName("Id_PK") );
   * // cliendId is a terminal that can be used in a JoinConstraintExpression
   *
   * \endcode
   */
  using JoinConstraintField = const Expression::JoinConstraint::TableFieldTerminal< JoinConstraintFieldTag >;


}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_JOIN_CONSTRAINT_FIELD_H
