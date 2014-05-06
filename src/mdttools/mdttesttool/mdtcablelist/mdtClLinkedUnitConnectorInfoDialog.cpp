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
#include "mdtSqlRelation.h"
#include <QSqlTableModel>
#include <QVBoxLayout>
#include <QSqlError>
#include <QMessageBox>
#include <QTableView>

mdtClLinkedUnitConnectorInfoDialog::mdtClLinkedUnitConnectorInfoDialog(QWidget *parent, QSqlDatabase db)
{
  pvDatabase = db;
  setupUi(this);
  setupLinkedConnectorsTable();
}

void mdtClLinkedUnitConnectorInfoDialog::setConnectors(const QVariant& unitConnectorId, const QList<QVariant> & linkedConnectionIdList) 
{
}

void mdtClLinkedUnitConnectorInfoDialog::displayUnit() 
{
}

void mdtClLinkedUnitConnectorInfoDialog::displayConnector() 
{
}

void mdtClLinkedUnitConnectorInfoDialog::displayLinkedConnections() 
{
}

void mdtClLinkedUnitConnectorInfoDialog::setupConnectorTable() 
{
}

void mdtClLinkedUnitConnectorInfoDialog::setupLinkedConnectorsTable() 
{
  QSqlTableModel *model;
  mdtSqlTableWidget *widget;
  QVBoxLayout *layout;

  // Setup model
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
  // Setup widget
  widget = new mdtSqlTableWidget;
  widget->setModel(model);
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("Connector_Id_FK", true);
  widget->setColumnHidden("ArticleConnector_Id_FK", true);
  widget->addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  ///widget->sort();
  Q_ASSERT(widget->tableView() != 0);
  widget->tableView()->resizeColumnsToContents();
  // Layout in gbLinkedConnectors
  layout = new QVBoxLayout;
  layout->addWidget(widget);
  gbLinkedConnectors->setLayout(layout);
}

void mdtClLinkedUnitConnectorInfoDialog::displayError(const mdtError & error) 
{
  QMessageBox msgBox;

  msgBox.setText(error.text());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
