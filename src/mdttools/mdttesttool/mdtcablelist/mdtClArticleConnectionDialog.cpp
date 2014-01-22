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
#include "mdtClArticleConnectionDialog.h"
#include "mdtSqlSelectionDialog.h"
#include <QWidget>
#include <QSqlQueryModel>

mdtClArticleConnectionDialog::mdtClArticleConnectionDialog(QWidget *parent)
 : QDialog(parent)
{
  pvArticleConnectorModel = 0;
  setupUi(this);
  pbSelectArticleConnector->setEnabled(false);
  connect(pbSelectArticleConnector, SIGNAL(clicked()), this, SLOT(selectArticleConnector()));
}

mdtClArticleConnectionDialog::~mdtClArticleConnectionDialog() 
{
}

void mdtClArticleConnectionDialog::setArticleConnectorModel(QSqlQueryModel *model)
{
  Q_ASSERT(model != 0);

  pvArticleConnectorModel = model;
  pbSelectArticleConnector->setEnabled(true);
}

/**
void mdtClArticleConnectionDialog::setData(const mdtClArticleConnectionData & data) 
{
  pvData = data;
}
*/

void mdtClArticleConnectionDialog::setData(const QSqlRecord & data)
{
  pvData = data;

  Q_ASSERT(pvData.contains("ArticleContactName"));
  Q_ASSERT(pvData.contains("IoType"));
  Q_ASSERT(pvData.contains("FunctionEN"));
  Q_ASSERT(pvData.contains("FunctionDE"));
  Q_ASSERT(pvData.contains("FunctionFR"));
  Q_ASSERT(pvData.contains("FunctionIT"));
}

/**
mdtClArticleConnectionData mdtClArticleConnectionDialog::data() const
{
  return pvData;
}
*/

mdtSqlRecord mdtClArticleConnectionDialog::data() const
{
  return pvData;
}

void mdtClArticleConnectionDialog::selectArticleConnector()
{
  Q_ASSERT(pvArticleConnectorModel != 0);

  mdtSqlSelectionDialog dialog(this);
  QString connectorName;

  // Setup and show dialog
  dialog.setModel(pvArticleConnectorModel, false);
  dialog.setColumnHidden("Id_PK", true);
  dialog.addSelectionResultColumn("Id_PK");
  dialog.addSelectionResultColumn("Name");
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store selected article connector ID
  Q_ASSERT(dialog.selectionResult().size() == 2);
  ///pvData.setArticleConnectorId(dialog.selectionResult().at(0));
  pvData.setValue("ArticleConnector_Id_FK", dialog.selectionResult().at(0));
  // Display connector name
  connectorName = dialog.selectionResult().at(1).toString().trimmed();
  if(connectorName.isEmpty()){
    connectorName = tr("<None>");
  }
  lbArticleConnectorName->setText(connectorName);
}

void mdtClArticleConnectionDialog::accept()
{
  // Store data
  pvData.setValue("ArticleContactName", fld_ArticleContactName->text());
  pvData.setValue("IoType", fld_IoType->text());
  pvData.setValue("FunctionEN", fld_FunctionEN->text());
  pvData.setValue("FunctionFR", fld_FunctionFR->text());
  pvData.setValue("FunctionDE", fld_FunctionDE->text());
  pvData.setValue("FunctionIT", fld_FunctionIT->text());
  /**
  pvData.setContactName(fld_ArticleContactName->text());
  pvData.setIoType(fld_IoType->text());
  pvData.setFunctionEN(fld_FunctionEN->text());
  pvData.setFunctionFR(fld_FunctionFR->text());
  pvData.setFunctionDE(fld_FunctionDE->text());
  pvData.setFunctionIT(fld_FunctionIT->text());
  */
  QDialog::accept();
}
