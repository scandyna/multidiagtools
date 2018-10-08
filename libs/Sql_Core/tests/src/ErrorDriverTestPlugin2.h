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
#ifndef MDT_SQL_ERROR_DRIVER_TEST_PLUGIN_2_H
#define MDT_SQL_ERROR_DRIVER_TEST_PLUGIN_2_H

#include "Mdt/Sql/AbstractErrorDriver.h"
#include "Mdt/Sql/AbstractErrorDriverPlugin.h"
#include <QSqlDatabase>
#include <QObject>
#include <QtPlugin>

namespace Mdt{ namespace Sql{

  class ErrorDriverTestDriver2 : public AbstractErrorDriver
  {
   Q_OBJECT

   public:

    Mdt::ErrorCode errorCode(const QSqlError& sqlError) const override;
    Mdt::Error::Level errorLevel(const QSqlError& sqlError) const override;
  };

  class ErrorDriverTestDriverPlugin2 : public QObject, public AbstractErrorDriverPlugin
  {
   Q_OBJECT
   Q_PLUGIN_METADATA(IID MdtSqlAbstractErrorDriver_iid FILE "ErrorDriverTestPlugin2.json")
   Q_INTERFACES(Mdt::Sql::AbstractErrorDriverPlugin)

   public:

    AbstractErrorDriver *create() override;
  };
}} // namespace Mdt{ namespace Sql{

#endif // #ifndef MDT_SQL_ERROR_DRIVER_TEST_PLUGIN_2_H
