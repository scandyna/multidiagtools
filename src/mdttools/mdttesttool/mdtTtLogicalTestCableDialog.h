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
#ifndef MDT_TT_LOGICAL_TEST_CABLE_DIALOG_H
#define MDT_TT_LOGICAL_TEST_CABLE_DIALOG_H

#include "ui_mdtTtLogicalTestCableDialog.h"
#include "mdtError.h"
#include <QDialog>
#include <QList>
#include <QVariant>
#include <QSqlDatabase>

class QVBoxLayout;
class mdtTtLogicalTestCableDutWidget;
///class mdtTtLogicalTestCableTcWidget;
class mdtTtLogicalTestCableTsWidget;

/*
 * Struct for affectation storage
 */
/**
struct mdtTtLogicalTestCableDialogAffectation
{
  mdtTtLogicalTestCableDutWidget *dutWidget;
  mdtTtLogicalTestCableTcWidget *testCableWidget;
  mdtTtLogicalTestCableTsWidget *testSystemWidget;
};
*/

/*! \brief Helper class for logical test cable generation
 */
class mdtTtLogicalTestCableDialog : public QDialog, Ui::mdtTtLogicalTestCableDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtLogicalTestCableDialog(QWidget * parent, QSqlDatabase db);

  /*! \brief Set (physical) test cable
   */
  void setTestCable(const QVariant & testCableId);

 private:

  /*! \brief Add affectaion widget to pvAffectationLayout and pvDutSideAffectations container
   */
  void addAffectation(mdtTtLogicalTestCableDutWidget *w);

  /*! \brief Add affectaion widget to pvAffectationLayout and pvTsSideAffectations container
   */
  void addAffectation(mdtTtLogicalTestCableTsWidget *w);

  ///void addAffectation(mdtTtLogicalTestCableDutWidget *dutWidget, mdtTtLogicalTestCableTcWidget *testCableWidget, mdtTtLogicalTestCableTsWidget *testSystemWidget);

  /*! \brief Display a error
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtTtLogicalTestCableDialog);

  QSqlDatabase pvDatabase;
  QVBoxLayout *pvAffectationLayout;
  ///QList<mdtTtLogicalTestCableDialogAffectation> pvAffectations;
  QList<mdtTtLogicalTestCableDutWidget*> pvDutSideAffectations;
  QList<mdtTtLogicalTestCableTsWidget*> pvTsSideAffectations;
};

#endif // #ifndef MDT_TT_LOGICAL_TEST_CABLE_DIALOG_H
