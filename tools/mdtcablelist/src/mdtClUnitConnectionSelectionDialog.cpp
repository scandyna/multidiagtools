/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtClUnitConnectionSelectionDialog.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtError.h"
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QPushButton>
#include <QString>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QDialogButtonBox>

#include <QDebug>

mdtClUnitConnectionSelectionDialog::mdtClUnitConnectionSelectionDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  QSqlError sqlError;
  QVBoxLayout *vLayout;
  QLabel *lb;
  QPushButton *pb;

  pvDatabase = db;
  vLayout = new QVBoxLayout;
  // Setup Unit model
  pvUnitModel = new QSqlTableModel(this, pvDatabase);
  pvUnitModel->setTable("VehicleType_Unit_view");
  if(!pvUnitModel->select()){
    sqlError = pvUnitModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'VehicleType_Unit_view'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
  }
  // Setup Unit widget
  lb = new QLabel(tr("Units"));
  vLayout->addWidget(lb);
  pvUnitWidget = new mdtSqlTableWidget;
  pvUnitWidget->setModel(pvUnitModel);
  pvUnitWidget->setEditionEnabled(false);
  Q_ASSERT(pvUnitWidget->tableView() != 0);
  pvUnitWidget->tableView()->setSelectionBehavior(QAbstractItemView::SelectRows);
  pvUnitWidget->tableView()->setSelectionMode(QAbstractItemView::SingleSelection);
  pvUnitWidget->setColumnHidden("VehicleType_Id_FK", true);
  pvUnitWidget->setColumnHidden("Unit_Id_FK", true);
  pvUnitWidget->setDefaultColumnToSelect("SchemaPosition");
  vLayout->addWidget(pvUnitWidget);
  // Setup Unit Connection model
  pvUnitConnectionModel = new QSqlTableModel(this, pvDatabase);
  pvUnitConnectionModel->setTable("UnitConnection_tbl");
  if(!pvUnitConnectionModel->select()){
    sqlError = pvUnitConnectionModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'UnitConnection_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
  }
  // Setup Unit Connection widget
  lb = new QLabel(tr("Connections"));
  vLayout->addWidget(lb);
  pvUnitConnectionWidget = new mdtSqlTableWidget;
  pvUnitConnectionWidget->setModel(pvUnitConnectionModel);
  pvUnitConnectionWidget->setEditionEnabled(false);
  Q_ASSERT(pvUnitConnectionWidget->tableView() != 0);
  pvUnitConnectionWidget->tableView()->setSelectionBehavior(QAbstractItemView::SelectRows);
  pvUnitConnectionWidget->tableView()->setSelectionMode(QAbstractItemView::SingleSelection);
  pvUnitConnectionWidget->setColumnHidden("Id_PK", true);
  pvUnitConnectionWidget->setColumnHidden("Unit_Id_FK", true);
  pvUnitConnectionWidget->setColumnHidden("ArticleConnection_Id_FK", true);
  pvUnitConnectionWidget->setDefaultColumnToSelect("ContactName");
  vLayout->addWidget(pvUnitConnectionWidget);
  // Setup Unit <-> Connection relation
  pvUnitConnectionRelation = new mdtSqlRelation;
  pvUnitConnectionRelation->setParentModel(pvUnitModel);
  pvUnitConnectionRelation->setChildModel(pvUnitConnectionModel);
  pvUnitConnectionRelation->addRelation("Unit_Id_FK", "Unit_Id_FK");
  pvUnitWidget->addChildWidget(pvUnitConnectionWidget, pvUnitConnectionRelation);
  // Setup buttons
  pvButtons = new QDialogButtonBox(this);
  pb = pvButtons->addButton(QDialogButtonBox::Ok);
  connect(pb, SIGNAL(clicked()), this, SLOT(accept()));
  pb = pvButtons->addButton(QDialogButtonBox::Cancel);
  connect(pb, SIGNAL(clicked()), this, SLOT(reject()));

  vLayout->addWidget(pvButtons);
  resize(850, 500);
  setWindowTitle(tr("Unit connection selection"));
  setLayout(vLayout);
}

mdtClUnitConnectionSelectionDialog::~mdtClUnitConnectionSelectionDialog()
{
  delete pvUnitConnectionRelation;
}

QVariant mdtClUnitConnectionSelectionDialog::selectedConnectionId() const
{
  return pvSelectedConnectionId;
}

QVariant mdtClUnitConnectionSelectionDialog::selectedConnectorName() const
{
  return pvSelectedConnectorName;
}

QVariant mdtClUnitConnectionSelectionDialog::selectedContactName() const
{
  return pvSelectedContactName;
}

QVariant mdtClUnitConnectionSelectionDialog::selectedSchemaPosition() const
{
  return pvSelectedShemaPosition;
}

void mdtClUnitConnectionSelectionDialog::accept()
{
  Q_ASSERT(pvUnitConnectionWidget->selectionModel() != 0);

  QModelIndexList selectedIndexes;
  QModelIndex index;
  QSqlError sqlError;

  selectedIndexes = pvUnitConnectionWidget->selectionModel()->selectedRows();
  if(selectedIndexes.size() != 1){
    return;
  }
  // Store Connection ID
  index = pvUnitConnectionModel->index(selectedIndexes.at(0).row(), pvUnitConnectionModel->fieldIndex("Id_PK"));
  pvSelectedConnectionId = pvUnitConnectionModel->data(index);
  // Store connector name
  index = pvUnitConnectionModel->index(selectedIndexes.at(0).row(), pvUnitConnectionModel->fieldIndex("ConnectorName"));
  pvSelectedConnectorName = pvUnitConnectionModel->data(index);
  // Store contact name
  index = pvUnitConnectionModel->index(selectedIndexes.at(0).row(), pvUnitConnectionModel->fieldIndex("ContactName"));
  pvSelectedContactName = pvUnitConnectionModel->data(index);
  // Store schema position
  selectedIndexes = pvUnitWidget->selectionModel()->selectedRows();
  if(selectedIndexes.size() == 1){
    index = pvUnitModel->index(selectedIndexes.at(0).row(), pvUnitModel->fieldIndex("SchemaPosition"));
    pvSelectedShemaPosition = pvUnitModel->data(index);
  }

  QDialog::accept();
}

void mdtClUnitConnectionSelectionDialog::reject()
{
  pvSelectedConnectionId.clear();
  QDialog::reject();
}
