/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_CL_UNIT_CONNECTOR_SELECTION_DIALOG_H
#define MDT_CL_UNIT_CONNECTOR_SELECTION_DIALOG_H

#include "mdtSqlSelectionDialog.h"
#include "mdtClUnitConnectorKeyData.h"
#include "mdtClUnitConnectorData.h"
#include <QSqlDatabase>
#include <QVariant>

/*! \brief Dialog for unit connector selection (in UnitConnector_tbl)
 */
class mdtClUnitConnectorSelectionDialog : public mdtSqlSelectionDialog
{
 public:

   /*! \brief Constructor
    */
  mdtClUnitConnectorSelectionDialog(QWidget *parent);

  /*! \brief Set SQL query and select data
   *
   * Will select unit connectors that belongs to given unit
   *
   * \pre unitId must not be null
   */
  bool select(QSqlDatabase db, const QVariant & unitId);

  /*! \brief Get selected unit connector key
   *
   * If user rejected the dialog, a null key is returned.
   */
  mdtClUnitConnectorKeyData selectedUnitConnectorKey() const;

  /*! \brief Get selected unit connector data
   *
   * If user rejected the dialog, null data is returned.
   */
  mdtClUnitConnectorData selectedUnitConnectorData() const;

 private:

  Q_DISABLE_COPY(mdtClUnitConnectorSelectionDialog);
};

#endif // #ifndef MDT_CL_UNIT_CONNECTOR_SELECTION_DIALOG_H

