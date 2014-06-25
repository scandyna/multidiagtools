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
#ifndef MDT_TT_TEST_MODEL_ITEM_DIALOG_H
#define MDT_TT_TEST_MODEL_ITEM_DIALOG_H

#include "ui_mdtTtTestModelItemDialog.h"
#include "mdtError.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QVariant>

class mdtSqlTableWidget;

/*! \brief Test item editor dialog
 */
class mdtTtTestModelItemDialog : public QDialog, Ui::mdtTtTestModelItemDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestModelItemDialog(QDialog *parent, QSqlDatabase db);

  /*! \brief Set test model item ID
   */
  bool setTestModelItemId(const QVariant & id);

  /*! \brief Get last error
   */
  inline mdtError lastError() { return pvLastError; }

 private:

  /*! \brief Setup test link table
   */
  bool setupTestLinkTable(const QVariant & testModelItemId);

  /*! \brief Setup test node unit setup table
   */
  bool setupTestNodeUnitSetupTable(const QVariant & testModelItemId);

  /*! \brief Display last error
   */
  void displayLastError();

  Q_DISABLE_COPY(mdtTtTestModelItemDialog);

  mdtSqlTableWidget *pvLinkTableWidget;
  mdtSqlTableWidget *pvSetupTableWidget;
  mdtError pvLastError;
};

#endif // #ifndef MDT_TT_TEST_MODEL_ITEM_DIALOG_H
