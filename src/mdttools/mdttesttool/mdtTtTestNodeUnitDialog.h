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
#ifndef MDT_TT_TEST_NODE_UNIT_DIALOG_H
#define MDT_TT_TEST_NODE_UNIT_DIALOG_H

#include "mdtTtTestNodeUnitData.h"
#include "ui_mdtTtTestNodeUnitDialog.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQueryModel>

class QWidget;

/*! \brief Dialog for test node unit edition
 */
class mdtTtTestNodeUnitDialog : public QDialog, Ui::mdtTtTestNodeUnitDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeUnitDialog(QWidget *parent, QSqlDatabase db);

  void setData(const mdtTtTestNodeUnitData & data);

  mdtTtTestNodeUnitData data();


  private slots:

  void selectBaseUnit();

  void selectTestConnection();

  void accept();

  void reject();

 private:

  /*! \brief Update dialog regarding internal data
   */
  void updateDialog();

  /*! \brief Display test connector and connection part regarding internal data
   */
  void displayTestConnection();

  /*! \brief Update BUS combo box regarding internal data
   */
  void displayBus();

  /*! \brief Store selected BUS to internal data
   */
  void storeBus();

  /*! \brief Update node unit type combo box regarding internal data
   */
  void displayType();

  /*! \brief Store selected type to internal data
   */
  void storeType();

  /*! \brief Setup and populate BUS combo box
   */
  void setupBusComboBox();

  /*! \brief Setup and populate node unit type combo box
   */
  void setupTypeComboBox();

  Q_DISABLE_COPY(mdtTtTestNodeUnitDialog);

  mdtTtTestNodeUnitData pvData;
  QSqlQueryModel pvTypeModel;
  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_TT_TEST_NODE_UNIT_DIALOG_H
