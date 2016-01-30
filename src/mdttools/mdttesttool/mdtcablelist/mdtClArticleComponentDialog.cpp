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
#include "mdtClArticleComponentDialog.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtClArticle.h"
#include "mdtError.h"
#include "mdt/sql/error/Error.h"
#include <QWidget>
#include <QSqlQueryModel>
#include <QModelIndex>
#include <QSqlError>

//#include <QDebug>

mdtClArticleComponentDialog::mdtClArticleComponentDialog(QWidget *parent, QSqlDatabase db, QVariant articleId, QVariant componentId)
 : QDialog(parent)
{
  QSqlError sqlError;

  pvDatabase = db;
  pvArticleId = articleId;
  pvComponentId = componentId;
  // Setup UI
  setupUi(this);
  setWindowTitle(tr("Article component edition"));
  connect(pbSelect, SIGNAL(clicked()), this, SLOT(selectComponent()));
  // Setup article component model
  pvArticleComponentModel = new QSqlQueryModel(this);
  pvArticleComponentModel->setQuery("SELECT Article_Id_PK, Component_Id_PK, ArticleCode, DesignationEN, ComponentQty, ComponentQtyUnit"\
                                    " FROM ArticleComponent_view", pvDatabase);
  sqlError = pvArticleComponentModel->lastError();
  if(sqlError.isValid()){
    auto error = mdtErrorNewQ("Unable to get data from ArticleComponent_view", mdtError::Error, this);
    error.stackError(mdt::sql::error::fromQSqlError(sqlError));
    error.commit();
  }
  // Set initial data
  displayCurrentComponent();
}

mdtClArticleComponentDialog::~mdtClArticleComponentDialog()
{
}

const QVariant &mdtClArticleComponentDialog::articleId() const
{
  return pvArticleId;
}

const QVariant &mdtClArticleComponentDialog::componentId() const
{
  return pvComponentId;
}

const QVariant mdtClArticleComponentDialog::qty() const
{
  return sbQty->value();
}

const QVariant mdtClArticleComponentDialog::qtyUnit() const
{
  return lbUnit->text();
}

void mdtClArticleComponentDialog::selectComponent()
{
  mdtSqlSelectionDialog dialog(this);
  mdtClArticle art(this, pvDatabase);
  QVariant data;

  // Setup and show dialog
  dialog.setMessage(tr("Please select a article to use as component:"));
  dialog.setQuery(art.sqlForArticleComponentSelection(pvArticleId), pvDatabase, false);
  dialog.setColumnHidden("Id_PK", true);
  dialog.setHeaderData("ArticleCode", tr("Article code"));
  dialog.setHeaderData("DesignationEN", tr("Designation\n(English)"));
  dialog.setHeaderData("DesignationFR", tr("Designation\n(French)"));
  dialog.setHeaderData("DesignationDE", tr("Designation\n(German)"));
  dialog.setHeaderData("DesignationIT", tr("Designation\n(Italian)"));
  dialog.addColumnToSortOrder("ArticleCode", Qt::AscendingOrder);
  dialog.sort();
  dialog.addSelectionResultColumn("Id_PK");
  dialog.addSelectionResultColumn("ArticleCode");
  dialog.addSelectionResultColumn("DesignationEN");
  dialog.addSelectionResultColumn("Unit");
  dialog.resize(700, 400);
  dialog.setWindowTitle(tr("Article component selection"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store result
  Q_ASSERT(dialog.selectionResult().size() == 4);
  pvComponentId = dialog.selectionResult().at(0);
  // Article code
  data = dialog.selectionResult().at(1);
  lbArticleCode->setText(data.toString());
  // Article designation EN
  data = dialog.selectionResult().at(2);
  lbArticleDesignationEN->setText(data.toString());
  // Qty
  sbQty->setValue(1.0);
  // Qty unit
  data = dialog.selectionResult().at(3);
  lbUnit->setText(data.toString());
}

void mdtClArticleComponentDialog::displayCurrentComponent()
{
  QModelIndex index;
  QVariant data;
  int row;

  if(pvComponentId.isNull()){
    lbArticleCode->clear();
    lbArticleDesignationEN->clear();
    sbQty->setValue(0.0);
    lbUnit->clear();
    return;
  }
  // Search component that match given component ID and display its data
  for(row = 0; row < pvArticleComponentModel->rowCount(); ++row){
    index = pvArticleComponentModel->index(row, 1);
    data = pvArticleComponentModel->data(index);
    if(data == pvComponentId){
      // Display article code
      index = pvArticleComponentModel->index(row, 2);
      data = pvArticleComponentModel->data(index);
      lbArticleCode->setText(data.toString());
      // Display article designation EN
      index = pvArticleComponentModel->index(row, 3);
      data = pvArticleComponentModel->data(index);
      lbArticleDesignationEN->setText(data.toString());
      // Display Qty
      index = pvArticleComponentModel->index(row, 4);
      data = pvArticleComponentModel->data(index);
      sbQty->setValue(data.toDouble());
      // Display Qty unit
      index = pvArticleComponentModel->index(row, 5);
      data = pvArticleComponentModel->data(index);
      lbUnit->setText(data.toString());
      break;
    }
  }
}
