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
#ifndef MDT_CL_CONNECTOR_SELECTION_DIALOG_H
#define MDT_CL_CONNECTOR_SELECTION_DIALOG_H

#include "mdtSqlSelectionDialog.h"
#include "mdtClConnectorData.h"
#include <QSqlDatabase>
#include <QVariant>

/*! \brief Dialog for connector selection (in Connector_tbl)
 */
class mdtClConnectorSelectionDialog : public mdtSqlSelectionDialog
{
 Q_OBJECT

 public:

   /*! \brief Constructor
    */
  mdtClConnectorSelectionDialog(QWidget *parent);

  /*! \brief Set SQL query and select data
   */
  bool select(QSqlDatabase db);

  /*! \brief Get selected connector key
   *
   * If user rejected the dialog, a null key is returned.
   */
  mdtClConnectorKeyData selectedConnectorKey() const;

  /*! \brief Get selected connector ID
   *
   * If user rejected the dialog, a null value is returned.
   */
  QVariant selectedConnectorId() const;

 private:

  Q_DISABLE_COPY(mdtClConnectorSelectionDialog);
};

#endif // #ifndef MDT_CL_CONNECTOR_SELECTION_DIALOG_H
