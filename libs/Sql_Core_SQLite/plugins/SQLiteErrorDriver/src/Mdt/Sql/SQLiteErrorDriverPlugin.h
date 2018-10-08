/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_SQL_SQLITE_ERROR_DRIVER_PLUGIN_H
#define MDT_SQL_SQLITE_ERROR_DRIVER_PLUGIN_H

#include "Mdt/Sql/AbstractErrorDriverPlugin.h"
#include <QObject>
#include <QtPlugin>

namespace Mdt{ namespace Sql{

  /*! \brief SQLite plugin implementation of error driver
   */
  class SQLiteErrorDriverPlugin : public QObject, public AbstractErrorDriverPlugin
  {
   Q_OBJECT
   Q_PLUGIN_METADATA(IID MdtSqlAbstractErrorDriver_iid FILE "SQLiteErrorDriverPlugin.json")
   Q_INTERFACES(Mdt::Sql::AbstractErrorDriverPlugin)

   public:

    AbstractErrorDriver *create() override;
  };
}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_SQLITE_ERROR_DRIVER_PLUGIN_H
