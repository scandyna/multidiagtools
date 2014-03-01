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
#ifndef MDT_MV_COLUMN_SELECTION_DIALOG_H
#define MDT_MV_COLUMN_SELECTION_DIALOG_H

#include "mdtItemsSelectorDialog.h"

class QWidget;
class QAbstractTableModel;
class QTableView;

/*! \brief Let the user manage fields to display in table view
 */
class mdtMvColumnSelectionDialog : public mdtItemsSelectorDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   *
   * \pre model and view must be valid.
   */
  mdtMvColumnSelectionDialog(QWidget *parent, QAbstractTableModel *model, QTableView *view);

  /*! \brief Apply new setup and close dialog
   */
  void accept();

 private:

  /*! \brief Get existing columns, that are not visible, from model
   */
  void displayAvailableColumns();

  /*! \brief Get existing columns, that are visible, from model
   */
  void displaySelectedColumns();

  /*! \brief Apply selected columns to model and view
   */
  void applySelectedColumns();

  /*! \brief Apply not selected columns to model and view
   */
  void applyNotSelectedColumns();

  Q_DISABLE_COPY(mdtMvColumnSelectionDialog);

  QAbstractTableModel * pvModel;
  QTableView * pvView;
};

#endif // #ifndef MDT_MV_COLUMN_SELECTION_DIALOG_H
