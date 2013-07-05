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
#include "mdtClArticleEditor.h"
#include "ui_mdtClArticleEditor.h"
#include "mdtSqlWindow.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtError.h"
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QStringList>
#include <QString>
#include <QList>
#include <QVariant>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>

#include <QDebug>

mdtClArticleEditor::mdtClArticleEditor(QObject *parent, const QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  // Setup unit
  pvArticleWidget = new mdtSqlFormWidget;
  Ui::mdtClArticleEditor *e = new Ui::mdtClArticleEditor;
  e->setupUi(pvArticleWidget);
  pvArticleModel = new QSqlTableModel(this, pvDatabase);
  // Setup connection
  pvArticleConnectionWidget = new mdtSqlTableWidget;
  pvArticleConnectionModel = new QSqlTableModel(this, pvDatabase);

  pvArticleConnectionRelation = new mdtSqlRelation;
}

mdtClArticleEditor::~mdtClArticleEditor()
{
  ///delete pvArticleWidget;
  ///delete pvArticleConnectionWidget;
  delete pvArticleConnectionRelation;
  ///delete pvArticleModel;
  ///delete pvArticleConnectionModel;
  ///delete pvVehicleTypeUnitModel;
}

bool mdtClArticleEditor::setupTables(bool includeConnections)
{
  QSqlError sqlError;

  // Setup Unit table
  if(!setupArticleTable()){
    return false;
  }
  // Setup connection table
  if(!setupArticleConnectionTable()){
    return false;
  }

  return true;
}

void mdtClArticleEditor::setupUi(mdtSqlWindow *window)
{
  Q_ASSERT(window != 0);

  window->setSqlWidget(pvArticleWidget);
  window->addChildWidget(pvArticleConnectionWidget, "Connections");
  window->enableNavigation();
  window->enableEdition();
  window->resize(700, 500);
}

int mdtClArticleEditor::currentArticleId()
{
  int articleIdColumn;
  QModelIndex index;

  if(pvArticleWidget->currentRow() < 0){
    return -1;
  }
  articleIdColumn = pvArticleModel->record().indexOf("Id_PK");
  if(articleIdColumn < 0){
    return -1;
  }
  index = pvArticleModel->index(pvArticleWidget->currentRow(), articleIdColumn);
  if(!index.isValid()){
    return -1;
  }

  return pvArticleModel->data(index).toInt();
}

bool mdtClArticleEditor::setupArticleTable()
{
  QSqlError sqlError;

  pvArticleModel->setTable("Article_tbl");
  if(!pvArticleModel->select()){
    sqlError = pvArticleModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'Article_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClArticleEditor");
    e.commit();
    return false;
  }
  pvArticleWidget->setModel(pvArticleModel);
  pvArticleWidget->mapFormWidgets();

  return true;
}

bool mdtClArticleEditor::setupArticleConnectionTable()
{
  QSqlError sqlError;

  pvArticleConnectionModel->setTable("ArticleConnection_tbl");
  if(!pvArticleConnectionModel->select()){
    sqlError = pvArticleConnectionModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'ArticleConnection_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClArticleEditor");
    e.commit();
    return false;
  }
  pvArticleConnectionWidget->setModel(pvArticleConnectionModel);
  pvArticleConnectionWidget->enableLocalEdition();
  // Hide relation fields and PK
  pvArticleConnectionWidget->setColumnHidden("Id_PK", true);
  pvArticleConnectionWidget->setColumnHidden("Article_Id_FK", true);
  // Give fields a user friendly name
  pvArticleConnectionWidget->setHeaderData("ConnectorName", tr("Connector"));
  pvArticleConnectionWidget->setHeaderData("ContactName", tr("Contact"));
  pvArticleConnectionWidget->setHeaderData("IoType", tr("I/O type"));
  // Setup Article <-> Connection relation
  pvArticleConnectionRelation->setParentModel(pvArticleModel);
  pvArticleConnectionRelation->setChildModel(pvArticleConnectionModel);
  if(!pvArticleConnectionRelation->addRelation("Id_PK", "Article_Id_FK")){
    return false;
  }
  pvArticleWidget->addChildWidget(pvArticleConnectionWidget, pvArticleConnectionRelation);

  return true;
}
