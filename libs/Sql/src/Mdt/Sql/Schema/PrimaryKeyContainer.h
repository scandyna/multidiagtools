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
#ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_CONTAINER_H
#define MDT_SQL_SCHEMA_PRIMARY_KEY_CONTAINER_H

#include "AutoIncrementPrimaryKey.h"
#include "SingleFieldPrimaryKey.h"
#include "PrimaryKey.h"
#include <boost/variant.hpp>

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief Conatin one type of primary key
   *
   * To help defining SQL schema in most possible easy way,
   *  several primary key types exsits.
   *
   * To store this varaints of primary key in a unified way,
   *  for exaple in a Table, this container class helps.
   */
  class PrimaryKeyContainer
  {
   public:

    

   private:

    boost::variant<PrimaryKey, AutoIncrementPrimaryKey, SingleFieldPrimaryKey> pvPrimaryKey;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_PRIMARY_KEY_CONTAINER_H
