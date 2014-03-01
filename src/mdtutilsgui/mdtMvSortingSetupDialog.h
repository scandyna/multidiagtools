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
#ifndef MDT_MV_SORTING_SETUP_DIALOG_H
#define MDT_MV_SORTING_SETUP_DIALOG_H

#include "mdtItemsSelectorDialog.h"

class QWidget;
class mdtSortFilterProxyModel;
class QAbstractTableModel;

/*! \brief Let the user manage columns sorting
 */
class mdtMvSortingSetupDialog : public mdtItemsSelectorDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   *
   * \pre model and proxyModel must be valid.
   */
  mdtMvSortingSetupDialog(QWidget *parent, QAbstractTableModel *model, mdtSortFilterProxyModel *proxyModel);

  /*! \brief Apply new setup and close dialog
   */
  void accept();

 private:

  /*! \brief Display not sorted columns
   */
  void displayAvailableColumns();

  /*! \brief Display sorted columns
   */
  void displaySortedColumns();

  /*! \brief Apply setup to selected columns
   */
  void applySelectedColumns();

  /*! \brief Apply setup to not selected columns
   */
  void applyNotSelectedColumns();

  Q_DISABLE_COPY(mdtMvSortingSetupDialog);

  QAbstractTableModel * pvModel;
  mdtSortFilterProxyModel * pvProxyModel;
};

#endif // #ifndef MDT_MV_SORTING_SETUP_DIALOG_H
