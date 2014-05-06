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
#ifndef MDT_CL_LINKED_UNIT_CONNECTOR_INFO_DIALOG_H
#define MDT_CL_LINKED_UNIT_CONNECTOR_INFO_DIALOG_H

#include "ui_mdtClLinkedUnitConnectorInfoDialog.h"
#include "mdtError.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QVariant>
#include <QList>

class mdtSqlTableWidget;
class QWidget;

/*! \brief Dialog that displays linked connectors to a given unit connector
 */
class mdtClLinkedUnitConnectorInfoDialog : public QDialog, private Ui::mdtClLinkedUnitConnectorInfoDialog
{
 Q_OBJECT

 public:

  /*! \brief
   */
  mdtClLinkedUnitConnectorInfoDialog(QWidget *parent, QSqlDatabase db);

  /*! \brief
   */
  void setConnectors(const QVariant & unitConnectorId, const QList<QVariant> & linkedConnectionIdList);

 private:

  /*! \brief
   */
  void displayUnit();

  /*! \brief
   */
  void displayConnector();

  /*! \brief
   */
  void displayLinkedConnections();

  /*! \brief
   */
  void setupConnectorTable();

  /*! \brief Setup linked connectors table
   *
   * Will also setup layout for gbLinkedConnectors group box
   */
  void setupLinkedConnectorsTable();

  /*! \brief
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtClLinkedUnitConnectorInfoDialog);

  mdtSqlTableWidget *pvLinkedConnectorsWidget;
  mdtSqlTableWidget *pvLinkedConnectionsWidget;
  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_CL_LINKED_UNIT_CONNECTOR_INFO_DIALOG_H
