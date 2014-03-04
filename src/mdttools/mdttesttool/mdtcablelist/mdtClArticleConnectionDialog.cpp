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
#include "mdtClArticleConnectorData.h"
#include "mdtClArticle.h"
#include <QWidget>
#include <QSqlQueryModel>

mdtClArticleConnectionDialog::mdtClArticleConnectionDialog(QWidget *parent, const QSqlDatabase & db)
 : QDialog(parent)
{
  pvDatabase = db;
  setupUi(this);
  ///pbSelectArticleConnector->setEnabled(false);
  connect(pbSelectArticleConnector, SIGNAL(clicked()), this, SLOT(selectArticleConnector()));
}

mdtClArticleConnectionDialog::~mdtClArticleConnectionDialog() 
{
}

void mdtClArticleConnectionDialog::setData(const mdtClArticleConnectionData & data)
{
  pvData = data;
  Q_ASSERT(pvData.contains("Id_PK"));
  Q_ASSERT(pvData.contains("Article_Id_FK"));
  Q_ASSERT(pvData.contains("ArticleConnector_Id_FK"));
  Q_ASSERT(pvData.contains("ArticleContactName"));
  Q_ASSERT(pvData.contains("IoType"));
  Q_ASSERT(pvData.contains("FunctionEN"));
  Q_ASSERT(pvData.contains("FunctionDE"));
  Q_ASSERT(pvData.contains("FunctionFR"));
  Q_ASSERT(pvData.contains("FunctionIT"));
  updateDialog();
}

mdtClArticleConnectionData mdtClArticleConnectionDialog::data() const
{
  return pvData;
}

void mdtClArticleConnectionDialog::selectArticleConnector()
{
  mdtSqlSelectionDialog dialog(this);
  QString sql;

  // Setup and show dialog
  sql = "SELECT Id_PK, Name FROM ArticleConnector_tbl WHERE Article_Id_FK = " + pvData.value("Article_Id_FK").toString();
  dialog.setQuery(sql, pvDatabase, false);
  dialog.setMessage(tr("Select article connector to use:"));
  dialog.setColumnHidden("Id_PK", true);
  dialog.addColumnToSortOrder("Name", Qt::AscendingOrder);
  dialog.sort();
  dialog.addSelectionResultColumn("Id_PK");
  dialog.setWindowTitle(tr("Article connector selection"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store selected article connector ID
  Q_ASSERT(dialog.selectionResult().size() == 1);
  pvData.setValue("ArticleConnector_Id_FK", dialog.selectionResult().at(0));
  updateDialog();
}

void mdtClArticleConnectionDialog::accept()
{
  updateData();
  QDialog::accept();
}

void mdtClArticleConnectionDialog::updateDialog()
{
  mdtClArticle art(0, pvDatabase);
  mdtClArticleConnectorData connectorData;
  bool ok;

  // Update article connections data
  fld_ArticleContactName->setText(pvData.value("ArticleContactName").toString());
  fld_IoType->setText(pvData.value("IoType").toString());
  fld_FunctionEN->setText(pvData.value("FunctionEN").toString());
  fld_FunctionFR->setText(pvData.value("FunctionFR").toString());
  fld_FunctionDE->setText(pvData.value("FunctionDE").toString());
  fld_FunctionIT->setText(pvData.value("FunctionIT").toString());
  // Update article connector data if one is set
  if(!pvData.value("ArticleConnector_Id_FK").isNull()){
    connectorData = art.getConnectorData(pvData.value("ArticleConnector_Id_FK"), &ok, false, false);
    if(ok){
      lbArticleConnectorName->setText(connectorData.value("Name").toString());
    }else{
      lbArticleConnectorName->setText("<Error!>");
    }
  }
}

void mdtClArticleConnectionDialog::updateData()
{
  pvData.setValue("ArticleContactName", fld_ArticleContactName->text());
  pvData.setValue("IoType", fld_IoType->text());
  pvData.setValue("FunctionEN", fld_FunctionEN->text());
  pvData.setValue("FunctionFR", fld_FunctionFR->text());
  pvData.setValue("FunctionDE", fld_FunctionDE->text());
  pvData.setValue("FunctionIT", fld_FunctionIT->text());
}
