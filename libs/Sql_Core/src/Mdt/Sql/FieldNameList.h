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
#ifndef MDT_SQL_FIELD_NAME_LIST_H
#define MDT_SQL_FIELD_NAME_LIST_H

#include "FieldNameListBase.h"
#include "FieldName.h"

namespace Mdt{ namespace Sql{

  /*! \brief List of field names
   *
   * Typical usage:
   * \code
   * #include "FieldNameList.h"
   *
   * namespace Sql = Mdt::Sql;
   *
   * using Sql::FieldName;
   * using Sql::FieldNameList;
   *
   * FieldNameList list( FieldName("A") , FieldName("B") );
   * \endcode
   */
  class FieldNameList : public FieldNameListBase<FieldNameList>
  {
   public:

    using FieldNameListBase::FieldNameListBase;

//   private:
// 
//     void addField(const FieldName & field)
//     {
//       addFieldName(field.toString());
//     }

  };

}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_FIELD_NAME_LIST_H
