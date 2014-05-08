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
class mdtClPathGraph;
class mdtClDirectLink;

/*! \brief Dialog that displays linked connectors to a given unit connector
 */
class mdtClLinkedUnitConnectorInfoDialog : public QDialog, private Ui::mdtClLinkedUnitConnectorInfoDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClLinkedUnitConnectorInfoDialog(QWidget *parent, QSqlDatabase db, mdtClPathGraph *graph);

  /*! \brief Set connector from witch linked ones must be searched
   */
  void setConnector(const QVariant & unitConnectorId);

 private:

  /*! \brief Display current connector information part
   */
  void displayConnector(const QVariant & connectorId);

  /*! \brief Display unit information part
   */
  void displayUnit(const QVariant & unitId);

  /*! \brief Display linked connectors
   */
  void displayLinkedConnectors(const QVariant & fromConnectorId);

  /*! \brief
   */
  void displayLinkedConnections();

  /*! \brief
   */
  void setupConnectorTable();

  /*! \brief Setup linked connectors table
   *
   * Will also create direct link widget and setup layout for gbLinkedConnectors group box
   */
  void setupLinkedConnectorsTable();

  /*! \brief Setup direct links table
   */
  void setupDirectLinkTable();

  /*! \brief Populate direct link table
   */
  void populateDirectLinkTable(const QVariant & fromConnectorId, const QList<QVariant> & linkedConnectorsIdList);

  /*! \brief
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtClLinkedUnitConnectorInfoDialog);

  mdtSqlTableWidget *pvLinkedConnectorsWidget;
  mdtSqlTableWidget *pvDirectLinksWidget;
  QSqlDatabase pvDatabase;
  mdtClPathGraph *pvGraph;
  mdtClDirectLink *pvDirectLink;
};

#endif // #ifndef MDT_CL_LINKED_UNIT_CONNECTOR_INFO_DIALOG_H
