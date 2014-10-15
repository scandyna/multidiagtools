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
#ifndef MDT_TT_TEST_LINK_DIALOG_H
#define MDT_TT_TEST_LINK_DIALOG_H

#include "mdtTtTestLinkData.h"
#include "ui_mdtTtTestLinkDialog.h"
#include "mdtClLink.h"
#include <QSqlDatabase>
#include <QDialog>
#include <QVariant>

class QWidget;

/*! \brief Dialog that provide test link edition
 */
class mdtTtTestLinkDialog : public QDialog, public Ui::mdtTtTestLinkDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestLinkDialog(QWidget *parent, QSqlDatabase db);

  /*! \brief
   */
  ~mdtTtTestLinkDialog();

  /*! \brief Set physical test cable link
   */
  void setTestCableLink(const QVariant & unitConnectionStartId, const QVariant & unitConnectionEndId);

  /*! \brief Set test node unit
   */
  void setTestNodeUnit(const QVariant & nodeUnitId);

  /*! \brief Limit test node unit selection to given list
   */
  void setTestNodeUnitSelectionList(const QList<QVariant> & idList);

  /*! \brief Set DUT unit
   */
  void setDutUnit(const QVariant & unitId);

  /*! \brief Limit DUT unit selection to given list
   */
  void setDutUnitSelectionList(const QList<QVariant> & idList);

  /*! \brief Set test connection
   */
  void setTestConnection(const QVariant & unitConnectionId);

  /*! \brief Set DUT connection
   */
  void setDutConnection(const QVariant & unitConnectionId);

  /*! \brief Set link data
   */
  void setLinkData(const mdtTtTestLinkData & data);

  /*! \brief Get link data
   */
  inline mdtTtTestLinkData linkData() const { return pvTestLinkData; };

 private slots:

  /*! \brief Select test cable link
   */
  void selectTestCableLink();

  /*! \brief Select test unit
   */
  void selectTestNodeUnit();

  /*! \brief Select DUT unit
   */
  void selectDutUnit();

  /*! \brief Select test connection
   */
  void selectTestConnection();

  /*! \brief Select DUT connection
   */
  void selectDutConnection();

  /*! \brief Show DUT linked connections informations
   */
  void viewDutLinkedConnections();

  /*! \brief
   */
  void accept();

  /*! \brief Clear link data and reject dialog
   */
  void reject();

 private:

  /*! \brief Display test cable
   *
   * Will display data from physical and logical test cable
   */
  void displayTestCable();

  /*! \brief Display test cable link
   */
  void displayTestCableLink();

  /*! \brief Display test node unit informations
   */
  void displayTestNodeUnit();

  /*! \brief Display DUT unit informations
   */
  void displayDutUnit();

  /*! \brief
   */
  void displayTestConnection();

  /*! \brief
   */
  void displayDutConnection();

  Q_DISABLE_COPY(mdtTtTestLinkDialog);

  QSqlDatabase pvDatabase;
  mdtTtTestLinkData pvTestLinkData;
  ///mdtClLinkData pvTestCableLinkData;
  QVariant pvTestNodeUnitId;
  QList<QVariant> pvTestNodeUnitSelectionIdList;
  QVariant pvDutUnitId;
  QList<QVariant> pvDutUnitSelectionIdList;
};

#endif // #ifndef MDT_TT_TEST_LINK_DIALOG_H
