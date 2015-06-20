/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_CL_UNIT_CONNECTION_SELECTION_DIALOG_H
#define MDT_CL_UNIT_CONNECTION_SELECTION_DIALOG_H

#include "mdtSqlSelectionDialog.h"
#include "mdtClUnitConnectionKeyData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorKeyData.h"
#include <QSqlDatabase>
#include <QVariant>
#include <QList>

/*! \brief Dialog for unit connection selection (in UnitConnection_tbl)
 */
class mdtClUnitConnectionSelectionDialog : public mdtSqlSelectionDialog
{
 public:

 private:

  Q_DISABLE_COPY(mdtClUnitConnectionSelectionDialog);
};

#endif // #ifndef MDT_CL_UNIT_CONNECTION_SELECTION_DIALOG_H

