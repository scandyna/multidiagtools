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
#include "mdtClLinkedUnitConnectorInfoDialog.h"
#include "mdtSqlTableWidget.h"
///#include "mdtSqlRelation.h"
#include "mdtSqlTableViewController.h"
#include "mdtSqlRelationInfo.h"
#include "mdtClPathGraph.h"
#include "mdtClDirectLink.h"
#include "mdtClUnit.h"
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QVBoxLayout>
#include <QSqlError>
#include <QMessageBox>
#include <QTableView>
#include <QString>

#include <QDebug>

mdtClLinkedUnitConnectorInfoDialog::mdtClLinkedUnitConnectorInfoDialog(QWidget* parent, QSqlDatabase db, mdtClPathGraph* graph)
 : QDialog(parent)
{
  Q_ASSERT(graph != 0);

  pvDatabase = db;
  pvGraph = graph;
  pvLinkedConnectorsWidget = 0;
  pvDirectLink = 0;
  pvDirectLinksWidget = 0;
  setupUi(this);
  setupLinkedConnectorsTable();
}

void mdtClLinkedUnitConnectorInfoDialog::setConnector(const QVariant& unitConnectorId)
{
  setupDirectLinkTable();
  displayConnector(unitConnectorId);
  displayLinkedConnectors(unitConnectorId);
}

void mdtClLinkedUnitConnectorInfoDialog::displayConnector(const QVariant& connectorId) 
{
  mdtClUnit unit(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  bool ok;
  if(connectorId.isNull()){
    lbUnitConnectorName->clear();
    return;
  }
  // Get unit connector data
  sql = "SELECT Unit_Id_FK, Name FROM UnitConnector_tbl WHERE Id_PK = " + connectorId.toString();
  dataList = unit.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    lbUnitConnectorName->setText("<Error!>");
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  lbUnitConnectorName->setText(data.value("Name").toString());
  // Display unit informations
  displayUnit(data.value("Unit_Id_FK"));
}

void mdtClLinkedUnitConnectorInfoDialog::displayUnit(const QVariant& unitId) 
{
  mdtClUnit unit(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  bool ok;
  if(unitId.isNull()){
    lbSchemaPosition->clear();
    lbAlias->clear();
    ///lbCabinet->clear();
    ///lbCoordinate->clear();
    return;
  }
  // Get unit data
  sql = "SELECT SchemaPosition, Alias, Cabinet, Coordinate FROM Unit_tbl WHERE Id_PK = " + unitId.toString();
  dataList = unit.getDataList<QSqlRecord>(sql, ok);
  if(!ok){
    lbSchemaPosition->setText("<Error!>");
    lbAlias->setText("<Error!>");
    ///lbCabinet->setText("<Error!>");
    ///lbCoordinate->setText("<Error!>");
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  lbSchemaPosition->setText(data.value("SchemaPosition").toString());
  lbAlias->setText(data.value("Alias").toString());
  ///lbCabinet->setText(data.value("Cabinet").toString());
  ///lbCoordinate->setText(data.value("Coordinate").toString());
}

void mdtClLinkedUnitConnectorInfoDialog::displayLinkedConnectors(const QVariant& fromConnectorId)
{
  Q_ASSERT(pvGraph != 0);
  Q_ASSERT(pvLinkedConnectorsWidget != 0);

  QList<QVariant> linkedConnectorIdList;
  ///QString filter;
  bool ok;
  ///int i;

  // Get linked connector ID list
  linkedConnectorIdList = pvGraph->getLinkedConnectorIdList(fromConnectorId, &ok);
  if(!ok){
    displayError(pvGraph->lastError());
    return;
  }
  // Build SQL filter
  if(linkedConnectorIdList.isEmpty()){
    return;
  }
  /**
  Q_ASSERT(linkedConnectorIdList.size() > 0);
  filter = "Id_PK = " + linkedConnectorIdList.at(0).toString();
  for(i = 1; i < linkedConnectorIdList.size(); ++i){
    filter += " OR Id_PK = " + linkedConnectorIdList.at(i).toString();
  }
  */
  // Apply filter
  ///Q_ASSERT(pvLinkedConnectorsWidget->model() != 0);
  ///pvLinkedConnectorsWidget->model()->setFilter(filter);
  pvLinkedConnectorsWidget->setFilter("Id_PK", linkedConnectorIdList);
  pvLinkedConnectorsWidget->sort();
  // Populate direct links table
  populateDirectLinkTable(fromConnectorId, linkedConnectorIdList);
}

void mdtClLinkedUnitConnectorInfoDialog::displayLinkedConnections() 
{
}

void mdtClLinkedUnitConnectorInfoDialog::setupConnectorTable() 
{
}

void mdtClLinkedUnitConnectorInfoDialog::setupLinkedConnectorsTable() 
{
  ///QSqlTableModel *model;
  QVBoxLayout *layout;

  // Setup model
  /**
  model = new QSqlTableModel(this, pvDatabase);
  model->setTable("UnitConnector_view");
  if(!model->select()){
    QSqlError sqlError = model->lastError();
    mdtError e(tr("Unable to select data in table 'UnitConnector_view'"), mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkedUnitConnectorInfoDialog");
    e.commit();
    displayError(e);
  }
  model->setFilter("Id_PK = -1");
  */
  // Setup widget
  pvLinkedConnectorsWidget = new mdtSqlTableWidget;
  pvLinkedConnectorsWidget->setTableName("UnitConnector_view", pvDatabase, tr("Unit connectors"));
  pvLinkedConnectorsWidget->start();
  if(!pvLinkedConnectorsWidget->select()){
    displayError(pvLinkedConnectorsWidget->lastError());
    return;
  }
  ///pvLinkedConnectorsWidget->setModel(model);
  pvLinkedConnectorsWidget->setColumnHidden("Id_PK", true);
  pvLinkedConnectorsWidget->setColumnHidden("Unit_Id_FK", true);
  pvLinkedConnectorsWidget->setColumnHidden("Connector_Id_FK", true);
  pvLinkedConnectorsWidget->setColumnHidden("ArticleConnector_Id_FK", true);
  pvLinkedConnectorsWidget->addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  ///widget->sort();
  pvLinkedConnectorsWidget->resizeViewToContents();
  /**
  Q_ASSERT(pvLinkedConnectorsWidget->tableView() != 0);
  pvLinkedConnectorsWidget->tableView()->resizeColumnsToContents();
  */
  // Setup direct link widget
  pvDirectLinksWidget = new mdtSqlTableWidget;
  // Layout in gbLinkedConnectors
  layout = new QVBoxLayout;
  layout->addWidget(pvLinkedConnectorsWidget);
  layout->addWidget(pvDirectLinksWidget);
  gbLinkedConnectors->setLayout(layout);
}

void mdtClLinkedUnitConnectorInfoDialog::setupDirectLinkTable()
{
  Q_ASSERT(pvLinkedConnectorsWidget != 0);
  ///Q_ASSERT(pvLinkedConnectorsWidget->model() != 0);
  Q_ASSERT(pvDirectLinksWidget != 0);

  ///QSqlTableModel *model;
  ///mdtSqlRelation *relation;
  mdtSqlRelationInfo relationInfo;

  // Setup direct link
  pvDirectLink = new mdtClDirectLink(this, pvDatabase);
  
  // Setup model
  /**
  model = new QSqlTableModel(this, pvDatabase);
  model->setTable("DirectLink_tbl");
  */
  /**
  if(!model->select()){
    QSqlError sqlError = model->lastError();
    mdtError e(tr("Unable to select data in table 'DirectLink_tbl'"), mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkedUnitConnectorInfoDialog");
    e.commit();
    displayError(e);
  }
  */
  // Setup relation and child widget
  relationInfo.setChildTableName("DirectLink_tbl");
  relationInfo.addRelation("Id_PK", "UnitConnectorEnd_Id_FK", false);
  if(!pvDirectLinksWidget->addChildTable(relationInfo, tr("Direct links"))){
    displayError(pvDirectLinksWidget->lastError());
    return;
  }
  /**
  relation = new mdtSqlRelation;
  relation->setParentModel(pvLinkedConnectorsWidget->model());
  relation->setChildModel(model);
  if(!relation->addRelation("Id_PK", "UnitConnectorEnd_Id_FK", false)){
    mdtError e(tr("Unable add relation between 'UnitConnector_view' and 'DirectLink_tbl'"), mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtClLinkedUnitConnectorInfoDialog");
    e.commit();
    displayError(e);
  }
  */

  // Setup widget
  ///pvDirectLinksWidget->setModel(model);
  // Hide relation fields and PK
  pvDirectLinksWidget->setColumnHidden("StartVehicleType_Id_PK", true);
  pvDirectLinksWidget->setColumnHidden("StartVehicleType", true);
  pvDirectLinksWidget->setColumnHidden("StartVehicleSubType", true);
  pvDirectLinksWidget->setColumnHidden("StartVehicleSerie", true);
  pvDirectLinksWidget->setColumnHidden("StartUnit_Id_PK", true);
  pvDirectLinksWidget->setColumnHidden("StartCabinet", true);
  pvDirectLinksWidget->setColumnHidden("StartCoordinate", true);
  pvDirectLinksWidget->setColumnHidden("UnitConnectionStart_Id_FK", true);
  pvDirectLinksWidget->setColumnHidden("UnitConnectorStart_Id_FK", true);
  pvDirectLinksWidget->setColumnHidden("StartConnectionType_Code_FK", true);
  pvDirectLinksWidget->setColumnHidden("StartFunctionEN", true);
  pvDirectLinksWidget->setColumnHidden("StartFunctionFR", true);
  pvDirectLinksWidget->setColumnHidden("StartFunctionDE", true);
  pvDirectLinksWidget->setColumnHidden("StartFunctionIT", true);
  pvDirectLinksWidget->setColumnHidden("EndVehicleType_Id_PK", true);
  pvDirectLinksWidget->setColumnHidden("EndVehicleType", true);
  pvDirectLinksWidget->setColumnHidden("EndVehicleSubType", true);
  pvDirectLinksWidget->setColumnHidden("EndVehicleSerie", true);
  pvDirectLinksWidget->setColumnHidden("EndUnit_Id_PK", true);
  pvDirectLinksWidget->setColumnHidden("EndCabinet", true);
  pvDirectLinksWidget->setColumnHidden("EndCoordinate", true);
  pvDirectLinksWidget->setColumnHidden("UnitConnectorEnd_Id_FK", true);
  pvDirectLinksWidget->setColumnHidden("EndConnectionType_Code_FK", true);
  pvDirectLinksWidget->setColumnHidden("SinceVersion", true);
  pvDirectLinksWidget->setColumnHidden("Modification", true);
  pvDirectLinksWidget->setColumnHidden("Identification", true);
  pvDirectLinksWidget->setColumnHidden("UnitConnectionEnd_Id_FK", true);
  ///pvDirectLinksWidget->setColumnHidden("ArticleLink_Id_FK", true);
  pvDirectLinksWidget->setColumnHidden("LinkType_Code_FK", true);
  pvDirectLinksWidget->setColumnHidden("LinkDirection_Code_FK", true);
  pvDirectLinksWidget->setColumnHidden("ArticleConnectionStart_Id_FK", true);
  pvDirectLinksWidget->setColumnHidden("ArticleConnectionEnd_Id_FK", true);
  // Give fields a user friendly name
  pvDirectLinksWidget->setHeaderData("StartVehicleType", tr("Start\nvehicle\ntype"));
  pvDirectLinksWidget->setHeaderData("StartVehicleSubType", tr("Start\nvehicle\nsub type"));
  pvDirectLinksWidget->setHeaderData("StartVehicleSerie", tr("Start\nvehicle\nserie"));
  pvDirectLinksWidget->setHeaderData("StartSchemaPosition", tr("Start\nschema pos."));
  pvDirectLinksWidget->setHeaderData("StartAlias", tr("Start\nalias"));
  pvDirectLinksWidget->setHeaderData("StartCabinet", tr("Start\ncabinet"));
  pvDirectLinksWidget->setHeaderData("StartCoordinate", tr("Start\ncoordinate"));
  pvDirectLinksWidget->setHeaderData("StartUnitConnectorName", tr("Start\nconnector"));
  pvDirectLinksWidget->setHeaderData("StartUnitContactName", tr("Start\ncontact"));
  pvDirectLinksWidget->setHeaderData("EndSchemaPosition", tr("End\nschema pos."));
  pvDirectLinksWidget->setHeaderData("EndAlias", tr("End\nalias"));
  pvDirectLinksWidget->setHeaderData("EndUnitConnectorName", tr("End\nconnector"));
  pvDirectLinksWidget->setHeaderData("EndUnitContactName", tr("End\ncontact"));
  ///pvDirectLinksWidget->setHeaderData("SinceVersion", tr("Since\nversion"));
  ///pvDirectLinksWidget->setHeaderData("LinkTypeNameEN", tr("Link type"));
  pvDirectLinksWidget->setHeaderData("ValueUnit", tr("Unit"));
  pvDirectLinksWidget->setHeaderData("LinkDirectionPictureAscii", tr("Direction"));

  pvDirectLinksWidget->setHeaderData("EndVehicleType", tr("End\nvehicle\ntype"));
  pvDirectLinksWidget->setHeaderData("EndVehicleSubType", tr("End\nvehicle\nsub type"));
  pvDirectLinksWidget->setHeaderData("EndVehicleSerie", tr("End\nvehicle\nserie"));

  pvDirectLinksWidget->setHeaderData("StartSchemaPage", tr("Start\nschema\npage"));
  pvDirectLinksWidget->setHeaderData("EndSchemaPage", tr("End\nschema\npage"));
  pvDirectLinksWidget->setHeaderData("EndFunctionEN", tr("End\nfunction (English)"));
  pvDirectLinksWidget->setHeaderData("EndFunctionFR", tr("End\nfunction (French)"));
  pvDirectLinksWidget->setHeaderData("EndFunctionDE", tr("End\nfunction (German)"));
  pvDirectLinksWidget->setHeaderData("EndFunctionIT", tr("End\nfunction (Italian)"));
  pvDirectLinksWidget->setHeaderData("StartSignalName", tr("Start\nsignal"));
  pvDirectLinksWidget->setHeaderData("EndSignalName", tr("End\nsignal"));
  pvDirectLinksWidget->setHeaderData("StartSwAddress", tr("Start\nSW address"));
  pvDirectLinksWidget->setHeaderData("EndSwAddress", tr("End\nSW address"));
  pvDirectLinksWidget->resizeViewToContents();
  /**
  Q_ASSERT(pvDirectLinksWidget->tableView() != 0);
  pvDirectLinksWidget->tableView()->resizeColumnsToContents();
  */

  ///pvLinkedConnectorsWidget->addChildWidget(pvDirectLinksWidget, relation);
  // Force a update a first time
  ///relation->setParentCurrentIndex(pvLinkedConnectorsWidget->currentRow());
  
  ///qDebug() << "DL filter: " << pvDirectLinksWidget->model()->filter();
}

void mdtClLinkedUnitConnectorInfoDialog::populateDirectLinkTable(const QVariant & fromConnectorId, const QList<QVariant> & linkedConnectorsIdList)
{
  Q_ASSERT(pvDirectLink != 0);
  Q_ASSERT(pvGraph != 0);
  Q_ASSERT(pvDirectLinksWidget != 0);

  int i;
  ///QSqlTableModel *model;

  /**
  model = pvDirectLinksWidget->model();
  Q_ASSERT(model != 0);
  */
  for(i = 0; i < linkedConnectorsIdList.size(); ++i){
    if(!pvDirectLink->addLinksByUnitConnector(fromConnectorId, linkedConnectorsIdList.at(i), pvGraph)){
      displayError(pvDirectLink->lastError());
      return;
    }
  }
  if(!pvDirectLinksWidget->select()){
    displayError(pvDirectLinksWidget->lastError());
  }
  /**
  if(!model->select()){
    QSqlError sqlError = model->lastError();
    mdtError e(tr("Unable to select data in table 'DirectLink_tbl'"), mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkedUnitConnectorInfoDialog");
    e.commit();
    displayError(e);
  }
  */
}

void mdtClLinkedUnitConnectorInfoDialog::displayError(const mdtError & error) 
{
  /// \todo Used mdtErrorDialog
  QMessageBox msgBox;

  msgBox.setText(error.text());
  msgBox.setDetailedText(error.systemText());
  ///msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
