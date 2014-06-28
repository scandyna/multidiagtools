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
#ifndef MDT_TT_TEST_NODE_UNIT_SETUP_DIALOG_H
#define MDT_TT_TEST_NODE_UNIT_SETUP_DIALOG_H

#include "ui_mdtTtTestNodeUnitSetupDialog.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include <QDialog>
#include <QSqlDatabase>

class QWidget;

class mdtTtTestNodeUnitSetupDialog : public QDialog, Ui::mdtTtTestNodeUnitSetupDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeUnitSetupDialog(QWidget *parent, QSqlDatabase db);

  /*! \brief Set setup data
   */
  void setData(const mdtTtTestNodeUnitSetupData & data);

  /*! \brief Get setup data
   */
  inline mdtTtTestNodeUnitSetupData data() const { return pvData; }

 private slots:

  /*! \brief Update data and eccept dialog
   */
  void accept();

  /*! \brief Update State check box text
   */
  void updateStateText(int state);

 private:

  /*! \brief Display data in dialog
   */
  void displayData();

  /*! \brief Update data from dialog widgets
   */
  void updateData();

  Q_DISABLE_COPY(mdtTtTestNodeUnitSetupDialog);

  mdtTtTestNodeUnitSetupData pvData;
};

#endif // #ifndef MDT_TT_TEST_NODE_UNIT_SETUP_DIALOG_H
