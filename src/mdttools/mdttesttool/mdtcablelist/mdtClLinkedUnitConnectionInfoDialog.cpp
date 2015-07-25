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
#include "mdtClLinkedUnitConnectionInfoDialog.h"
#include "mdtSortFilterProxyModel.h"
#include "mdtClUnit.h"
#include <QSqlQueryModel>
#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QMessageBox>
#include <QItemSelectionModel>
#include <QDialogButtonBox>

#include <QDebug>

mdtClLinkedUnitConnectionInfoDialog::mdtClLinkedUnitConnectionInfoDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  // Setup databse part
  pvDatabase = db;
  pvModel = new QSqlQueryModel(this);
  pvProxyModel = new mdtSortFilterProxyModel(this);
  pvProxyModel->setSourceModel(pvModel);
  // Setup UI part
  setupUi(this);
  twLinkedConnections->setModel(pvProxyModel);
  twLinkedConnections->setSelectionMode(QAbstractItemView::SingleSelection);
  lbSchemaPosition->clear();
  lbAlias->clear();
  lbCabinet->clear();
  lbCoordinate->clear();
  lbConnector->clear();
  lbContact->clear();
}

void mdtClLinkedUnitConnectionInfoDialog::setSelectionModeEnabled(bool enable)
{
  if(enable){
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  }else{
    buttonBox->setStandardButtons(QDialogButtonBox::Close);
  }
}

void mdtClLinkedUnitConnectionInfoDialog::setConnections(const QVariant & unitConnectionId, const QList<QVariant> & linkedConnectionIdList) 
{
  displayConnection(unitConnectionId);
  displayLinkedConnections(linkedConnectionIdList);
}

QVariant mdtClLinkedUnitConnectionInfoDialog::functionStringEN() const
{
  return pvFunctionStringEN;
}

QVariant mdtClLinkedUnitConnectionInfoDialog::functionStringFR() const
{
  return pvFunctionStringFR;
}

QVariant mdtClLinkedUnitConnectionInfoDialog::functionStringDE() const
{
  return pvFunctionStringDE;
}

QVariant mdtClLinkedUnitConnectionInfoDialog::functionStringIT() const
{
  return pvFunctionStringIT;
}

void mdtClLinkedUnitConnectionInfoDialog::accept()
{
  Q_ASSERT(pvModel != 0);
  Q_ASSERT(pvProxyModel != 0);
  Q_ASSERT(twLinkedConnections->selectionModel() != 0);

  QModelIndexList selectedIndexes;
  QModelIndex index;

  // Get selected row to build function strings
  selectedIndexes = selectedIndexes = twLinkedConnections->selectionModel()->selectedIndexes();
  if(selectedIndexes.isEmpty()){
    return;
  }
  Q_ASSERT(selectedIndexes.size() == 1);
  index = pvProxyModel->mapToSource(selectedIndexes.at(0));
  if(!index.isValid()){
    return;
  }
  // Build function strings
  builFunctionStringEN(index);
  builFunctionStringFR(index);
  builFunctionStringDE(index);
  builFunctionStringIT(index);

  QDialog::accept();
}

void mdtClLinkedUnitConnectionInfoDialog::displayConnection(const QVariant & connectionId) 
{
  mdtClUnit unit(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  bool ok;
  if(connectionId.isNull()){
    lbConnector->clear();
    lbContact->clear();
    return;
  }
  // Get unit connection data
  sql = "SELECT Unit_Id_FK, UnitConnectorName, UnitContactName FROM UnitConnection_view WHERE UnitConnection_Id_PK = " + connectionId.toString();
  dataList = unit.getData(sql, &ok);
  if(!ok){
    lbConnector->setText("<Error!>");
    lbContact->setText("<Error!>");
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  lbConnector->setText(data.value("UnitConnectorName").toString());
  lbContact->setText(data.value("UnitContactName").toString());
  // Display unit informations
  displayUnit(data.value("Unit_Id_FK"));
}

void mdtClLinkedUnitConnectionInfoDialog::displayUnit(const QVariant & unitId) 
{
  mdtClUnit unit(0, pvDatabase);
  QString sql;
  QList<QSqlRecord> dataList;
  QSqlRecord data;
  bool ok;
  if(unitId.isNull()){
    lbSchemaPosition->clear();
    lbAlias->clear();
    lbCabinet->clear();
    lbCoordinate->clear();
    return;
  }
  // Get unit data
  sql = "SELECT SchemaPosition, Alias, Cabinet, Coordinate FROM Unit_tbl WHERE Id_PK = " + unitId.toString();
  dataList = unit.getData(sql, &ok);
  if(!ok){
    lbSchemaPosition->setText("<Error!>");
    lbAlias->setText("<Error!>");
    lbCabinet->setText("<Error!>");
    lbCoordinate->setText("<Error!>");
    return;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  lbSchemaPosition->setText(data.value("SchemaPosition").toString());
  lbAlias->setText(data.value("Alias").toString());
  lbCabinet->setText(data.value("Cabinet").toString());
  lbCoordinate->setText(data.value("Coordinate").toString());
}

void mdtClLinkedUnitConnectionInfoDialog::displayLinkedConnections(const QList<QVariant> & connectionIdList) 
{
  QString sql;
  QSqlError sqlError;
  mdtError error;

  sql = sqlForLinkedConnections(connectionIdList);
  pvModel->setQuery(sql, pvDatabase);
  sqlError = pvModel->lastError();
  if(sqlError.isValid()){
    error.setError(tr("Cannot show linked connections. Please see details for more information."), mdtError::Error);
    error.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(error, "mdtClLinkedUnitConnectionInfoDialog");
    error.commit();
    displayError(error);
    return;
  }
  // Set user friendly headers
  pvModel->setHeaderData(0, Qt::Horizontal, tr("Cabinet"));
  pvModel->setHeaderData(1, Qt::Horizontal, tr("Coordinate"));
  pvModel->setHeaderData(2, Qt::Horizontal, tr("Schema\nposition"));
  pvModel->setHeaderData(3, Qt::Horizontal, tr("Alias"));
  pvModel->setHeaderData(4, Qt::Horizontal, tr("Unit\nconnector"));
  pvModel->setHeaderData(5, Qt::Horizontal, tr("Unit\ncontact"));
  pvModel->setHeaderData(6, Qt::Horizontal, tr("Schema\npage"));
  pvModel->setHeaderData(7, Qt::Horizontal, tr("Unit\nFunction\n(English)"));
  pvModel->setHeaderData(8, Qt::Horizontal, tr("Unit\nFunction\n(Frensh)"));
  pvModel->setHeaderData(9, Qt::Horizontal, tr("Unit\nFunction\n(German)"));
  pvModel->setHeaderData(10, Qt::Horizontal, tr("Unit\nFunction\n(Italian)"));
  pvModel->setHeaderData(11, Qt::Horizontal, tr("Signal name"));
  pvModel->setHeaderData(12, Qt::Horizontal, tr("SW address"));
  pvModel->setHeaderData(13, Qt::Horizontal, tr("Article\nconnector\nname"));
  pvModel->setHeaderData(14, Qt::Horizontal, tr("Article\ncontact\nname"));
  pvModel->setHeaderData(15, Qt::Horizontal, tr("I/O type"));
  pvModel->setHeaderData(16, Qt::Horizontal, tr("Article\nFunction\n(English)"));
  pvModel->setHeaderData(17, Qt::Horizontal, tr("Article\nFunction\n(Frensh)"));
  pvModel->setHeaderData(18, Qt::Horizontal, tr("Article\nFunction\n(German)"));
  pvModel->setHeaderData(19, Qt::Horizontal, tr("Article\nFunction\n(Italian)"));
  // Setup sorting
  pvProxyModel->addColumnToSortOrder(pvModel, "SchemaPosition", Qt::AscendingOrder);
  pvProxyModel->addColumnToSortOrder(pvModel, "UnitConnectorName", Qt::AscendingOrder);
  pvProxyModel->addColumnToSortOrder(pvModel, "UnitContactName", Qt::AscendingOrder);
  // If model is empty, canFetchMore will allways return true, prevent this
  if(pvModel->rowCount() < 1){
    return;
  }
  while(pvModel->canFetchMore()){
    pvModel->fetchMore();
  }
  twLinkedConnections->sortByColumn(0);
  // Resize
  twLinkedConnections->resizeColumnsToContents();
  twLinkedConnections->resizeRowsToContents();
}

QString mdtClLinkedUnitConnectionInfoDialog::sqlForLinkedConnections(const QList<QVariant> & connectionIdList) const
{
  QLocale locale;
  QString unitFunctionField;
  QString articleFunctionField;
  QString sql;
  int i;

  if(connectionIdList.isEmpty()){
    return sql;
  }
  // Setup base of query
  sql = "SELECT\n"\
        " Unit_tbl.Cabinet,\n"\
        " Unit_tbl.Coordinate,\n"\
        " Unit_tbl.SchemaPosition,\n"\
        " Unit_tbl.Alias,\n"\
        " UnitConnector_tbl.Name AS UnitConnectorName ,\n"\
        " UnitConnection_tbl.UnitContactName ,\n"\
        " UnitConnection_tbl.SchemaPage ,\n"\
        " UnitConnection_tbl.FunctionEN AS UnitFunctionEN,\n"\
        " UnitConnection_tbl.FunctionFR AS UnitFunctionFR,\n"\
        " UnitConnection_tbl.FunctionDE AS UnitFunctionDE,\n"\
        " UnitConnection_tbl.FunctionIT AS UnitFunctionIT,\n"\
        " UnitConnection_tbl.SignalName ,\n"\
        " UnitConnection_tbl.SwAddress ,\n"\
        " ArticleConnector_tbl.Name AS ArticleConnectorName ,\n"\
        " ArticleConnection_tbl.ArticleContactName ,\n"\
        " ArticleConnection_tbl.IoType ,\n"\
        " ArticleConnection_tbl.FunctionEN AS ArticleFunctionEN,\n"\
        " ArticleConnection_tbl.FunctionFR AS ArticleFunctionFR,\n"\
        " ArticleConnection_tbl.FunctionDE AS ArticleFunctionDE,\n"\
        " ArticleConnection_tbl.FunctionIT AS ArticleFunctionIT\n"\
        "FROM UnitConnection_tbl\n"\
        " LEFT JOIN UnitConnector_tbl\n"\
        "  ON UnitConnector_tbl.Id_PK = UnitConnection_tbl.UnitConnector_Id_FK\n"\
        " LEFT JOIN ArticleConnector_tbl\n"\
        "  ON UnitConnector_tbl.ArticleConnector_Id_FK = ArticleConnector_tbl.Id_PK\n"\
        " LEFT JOIN ArticleConnection_tbl\n"\
        "  ON UnitConnection_tbl.ArticleConnection_Id_FK = ArticleConnection_tbl.Id_PK\n"\
        " JOIN Unit_tbl\n"\
        "  ON Unit_tbl.Id_PK = UnitConnection_tbl.Unit_Id_FK";
  // Add filter on requested connection IDs
  Q_ASSERT(connectionIdList.size() > 0);
  sql += " WHERE UnitConnection_tbl.Id_PK = " + connectionIdList.at(0).toString();
  for(i = 1; i < connectionIdList.size(); ++i){
    sql += " OR UnitConnection_tbl.Id_PK = " + connectionIdList.at(i).toString();
  }

  return sql;
}

void mdtClLinkedUnitConnectionInfoDialog::displayError(const mdtError & error)
{
  QMessageBox msgBox;

  msgBox.setText(error.text());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}

void mdtClLinkedUnitConnectionInfoDialog::builFunctionStringEN(const QModelIndex& index)
{
  Q_ASSERT(pvModel != 0);

  QModelIndex dataIndex;

  pvFunctionStringEN.clear();
  if(!index.isValid()){
    return;
  }
  // Set schema position
  pvFunctionStringEN = "-> Pos. ";
  dataIndex = pvModel->index(index.row(), 2);
  pvFunctionStringEN += pvModel->data(dataIndex).toString();
  // Set Alias
  dataIndex = pvModel->index(index.row(), 3);
  if(!pvModel->data(dataIndex).toString().isEmpty()){
    pvFunctionStringEN += " (" + pvModel->data(dataIndex).toString() + "): ";
  }
  // Set function EN
  dataIndex = pvModel->index(index.row(), 7);
  pvFunctionStringEN += pvModel->data(dataIndex).toString();
}

void mdtClLinkedUnitConnectionInfoDialog::builFunctionStringFR(const QModelIndex& index)
{
  Q_ASSERT(pvModel != 0);

  QModelIndex dataIndex;

  pvFunctionStringFR.clear();
  if(!index.isValid()){
    return;
  }
  // Set schema position
  pvFunctionStringFR = "-> Pos. ";
  dataIndex = pvModel->index(index.row(), 2);
  pvFunctionStringFR += pvModel->data(dataIndex).toString();
  // Set Alias
  dataIndex = pvModel->index(index.row(), 3);
  if(!pvModel->data(dataIndex).toString().isEmpty()){
    pvFunctionStringFR += " (" + pvModel->data(dataIndex).toString() + "): ";
  }
  // Set function EN
  dataIndex = pvModel->index(index.row(), 8);
  pvFunctionStringFR += pvModel->data(dataIndex).toString();
}

void mdtClLinkedUnitConnectionInfoDialog::builFunctionStringDE(const QModelIndex& index)
{
  Q_ASSERT(pvModel != 0);

  QModelIndex dataIndex;

  pvFunctionStringDE.clear();
  if(!index.isValid()){
    return;
  }
  // Set schema position
  pvFunctionStringDE = "-> Pos. ";
  dataIndex = pvModel->index(index.row(), 2);
  pvFunctionStringDE += pvModel->data(dataIndex).toString();
  // Set Alias
  dataIndex = pvModel->index(index.row(), 3);
  if(!pvModel->data(dataIndex).toString().isEmpty()){
    pvFunctionStringDE += " (" + pvModel->data(dataIndex).toString() + "): ";
  }
  // Set function EN
  dataIndex = pvModel->index(index.row(), 9);
  pvFunctionStringDE += pvModel->data(dataIndex).toString();
}

void mdtClLinkedUnitConnectionInfoDialog::builFunctionStringIT(const QModelIndex& index)
{
  Q_ASSERT(pvModel != 0);

  QModelIndex dataIndex;

  pvFunctionStringIT.clear();
  if(!index.isValid()){
    return;
  }
  // Set schema position
  pvFunctionStringIT = "-> Pos. ";
  dataIndex = pvModel->index(index.row(), 2);
  pvFunctionStringIT += pvModel->data(dataIndex).toString();
  // Set Alias
  dataIndex = pvModel->index(index.row(), 3);
  if(!pvModel->data(dataIndex).toString().isEmpty()){
    pvFunctionStringIT += " (" + pvModel->data(dataIndex).toString() + "): ";
  }
  // Set function EN
  dataIndex = pvModel->index(index.row(), 10);
  pvFunctionStringIT += pvModel->data(dataIndex).toString();
}
