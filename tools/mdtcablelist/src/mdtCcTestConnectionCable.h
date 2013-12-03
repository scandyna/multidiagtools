/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_CC_TEST_CONNECTION_CABLE_H
#define MDT_CC_TEST_CONNECTION_CABLE_H

#include "mdtClBase.h"
#include <QString>
#include <QVariant>

/*! \brief Helper class for test connection cable edition
 */
class mdtCcTestConnectionCable : public mdtClBase
{
 public:

  /*! \brief Constructor
   */
  mdtCcTestConnectionCable(QSqlDatabase db);

  /*! \brief Get SQL statement for start unit connector selection
   *
   * \param dutUnitId ID of unit that will become the DUT (Device Under Test)
   */
  QString sqlForStartConnectorSelection(const QVariant & dutUnitId) const;

 private:

  Q_DISABLE_COPY(mdtCcTestConnectionCable);
};

#endif // #ifndef MDT_CC_TEST_CONNECTION_CABLE_H
