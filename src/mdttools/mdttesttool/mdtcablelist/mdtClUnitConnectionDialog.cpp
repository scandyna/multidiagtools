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
#include "mdtClUnitConnectionDialog.h"
///#include "mdtClConnectorData.h"
///#include "mdtClArticleConnectorData.h"
#include "mdtClArticleConnectionData.h"
#include "mdtSqlSelectionDialog.h"
#include <QSqlQueryModel>
#include <QSqlError>
#include <QMessageBox>

#include <QDebug>

mdtClUnitConnectionDialog::mdtClUnitConnectionDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  pvDatabase = db;
  setupUi(this);
  connect(pbCopyFunctionEN, SIGNAL(clicked()), this, SLOT(copyFunctionEN()));
  connect(pbCopyFunctionFR, SIGNAL(clicked()), this, SLOT(copyFunctionFR()));
  connect(pbCopyFunctionDE, SIGNAL(clicked()), this, SLOT(copyFunctionDE()));
  connect(pbCopyFunctionIT, SIGNAL(clicked()), this, SLOT(copyFunctionIT()));
  connect(pbCopyConnectorName, SIGNAL(clicked()), this, SLOT(copyConnectorName()));
  connect(pbCopyContactName, SIGNAL(clicked()), this, SLOT(copyContactName()));
  hideArticleConnectionWidgets();
  /// \todo Currently, changing unit connector is not implemented
  connect(pbSelectUnitConnector, SIGNAL(clicked()), this, SLOT(selectUnitConnector()));
  ///pbSelectUnitConnector->setEnabled(false);
}

mdtClUnitConnectionDialog::~mdtClUnitConnectionDialog()
{
}

/**
void mdtClUnitConnectionDialog::setUnitId(const QVariant & id)
{
  pvData.setValue("Id_PK", id);
}

void mdtClUnitConnectionDialog::setArticleConnectionId(const QVariant & id)
{
  pvData.articleConnectionData().setValue("Id_PK", id);
}
*/

void mdtClUnitConnectionDialog::setData(const mdtClUnitConnectionData &data)
{
  Q_ASSERT(!data.value("Unit_Id_FK").isNull());

  pvData = data;
  updateDialog();
}

const mdtClUnitConnectionData &mdtClUnitConnectionDialog::data() const
{
  return pvData;
}

void mdtClUnitConnectionDialog::copyFunctionEN()
{
  if(!leFunctionEN->text().isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Function ENG has allready data."));
    msgBox.setInformativeText(tr("Do you want to overwrite ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  leFunctionEN->setText(lbArticleFunctionEN->text());
}

void mdtClUnitConnectionDialog::copyFunctionFR()
{
  if(!leFunctionFR->text().isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Function FRA has allready data."));
    msgBox.setInformativeText(tr("Do you want to overwrite ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  leFunctionFR->setText(lbArticleFunctionFR->text());
}

void mdtClUnitConnectionDialog::copyFunctionDE()
{
  if(!leFunctionDE->text().isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Function DEU has allready data."));
    msgBox.setInformativeText(tr("Do you want to overwrite ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  leFunctionDE->setText(lbArticleFunctionDE->text());
}

void mdtClUnitConnectionDialog::copyFunctionIT()
{
  if(!leFunctionIT->text().isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Function ITA has allready data."));
    msgBox.setInformativeText(tr("Do you want to overwrite ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  leFunctionIT->setText(lbArticleFunctionIT->text());
}

void mdtClUnitConnectionDialog::selectUnitConnector()
{
  mdtSqlSelectionDialog dialog(this);
  QSqlQueryModel model;
  QString sql;

  // Setup and show dialog
  sql = "SELECT Id_PK, Name FROM UnitConnector_tbl WHERE Unit_Id_FK = " + pvData.value("Unit_Id_FK").toString();
  model.setQuery(sql, pvDatabase);
  dialog.setModel(&model, false);
  dialog.setColumnHidden("Id_PK", true);
  dialog.addSelectionResultColumn("Id_PK");
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store selected article connector ID
  Q_ASSERT(dialog.selectionResult().size() == 1);
  pvData.setValue("UnitConnector_Id_FK", dialog.selectionResult().at(0));
  updateDialog();
}

void mdtClUnitConnectionDialog::copyConnectorName()
{
  /**
  if(!leUnitConnectorName->text().isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Connector name has allready data."));
    msgBox.setInformativeText(tr("Do you want to overwrite ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  leUnitConnectorName->setText(lbArticleConnectorName->text());
  */
}

void mdtClUnitConnectionDialog::copyContactName()
{
  if(!leUnitContactName->text().isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Contact name has allready data."));
    msgBox.setInformativeText(tr("Do you want to overwrite ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  leUnitContactName->setText(lbArticleContactName->text());
}

void mdtClUnitConnectionDialog::accept()
{
  // Checks
  /**
  if((leUnitConnectorName->text().isEmpty())&&(leUnitContactName->text().isEmpty())){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Connector name and contact name are both empty."));
    msgBox.setInformativeText(tr("Do you want to continue ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  */
  // Store data and accept
  updateData();
  QDialog::accept();
}

void mdtClUnitConnectionDialog::reject()
{
  pvData.clear();
  QDialog::reject();
}

void mdtClUnitConnectionDialog::updateDialog()
{
  // Article connection data part
  /**
  if(pvData.articleConnectionData().connectorName().isNull()){
    lbArticleConnectorName->setVisible(false);
    pbCopyConnectorName->setVisible(false);
  }else{
    lbArticleConnectorName->setVisible(true);
    pbCopyConnectorName->setVisible(true);
    lbArticleConnectorName->setText(pvData.articleConnectionData().connectorName().toString());
  }
  */
  if(pvData.articleConnectionData().value("ArticleContactName").isNull()){
    lbArticleContactName->setVisible(false);
    pbCopyContactName->setVisible(false);
  }else{
    lbArticleContactName->setVisible(true);
    pbCopyContactName->setVisible(true);
    lbArticleContactName->setText(pvData.articleConnectionData().value("ArticleContactName").toString());
  }
  if(pvData.articleConnectionData().value("IoType").isNull()){
    lbArticleIoType->setVisible(false);
    lbArticleIoTypeLabel->setVisible(false);
  }else{
    lbArticleIoType->setVisible(true);
    lbArticleIoTypeLabel->setVisible(true);
    lbArticleIoType->setText(pvData.articleConnectionData().value("IoType").toString());
  }
  if(pvData.articleConnectionData().value("FunctionEN").isNull()){
    lbArticleFunctionEN->setVisible(false);
    pbCopyFunctionEN->setVisible(false);
  }else{
    lbArticleFunctionEN->setVisible(true);
    pbCopyFunctionEN->setVisible(true);
    lbArticleFunctionEN->setText(pvData.articleConnectionData().value("FunctionEN").toString());
  }
  if(pvData.articleConnectionData().value("FunctionFR").isNull()){
    lbArticleFunctionFR->setVisible(false);
    pbCopyFunctionFR->setVisible(false);
  }else{
    lbArticleFunctionFR->setVisible(true);
    pbCopyFunctionFR->setVisible(true);
    lbArticleFunctionFR->setText(pvData.articleConnectionData().value("FunctionFR").toString());
  }
  if(pvData.articleConnectionData().value("FunctionDE").isNull()){
    lbArticleFunctionDE->setVisible(false);
    pbCopyFunctionDE->setVisible(false);
  }else{
    lbArticleFunctionDE->setVisible(true);
    pbCopyFunctionDE->setVisible(true);
    lbArticleFunctionDE->setText(pvData.articleConnectionData().value("FunctionDE").toString());
  }
  if(pvData.articleConnectionData().value("FunctionIT").isNull()){
    lbArticleFunctionIT->setVisible(false);
    pbCopyFunctionIT->setVisible(false);
  }else{
    lbArticleFunctionIT->setVisible(true);
    pbCopyFunctionIT->setVisible(true);
    lbArticleFunctionIT->setText(pvData.articleConnectionData().value("FunctionIT").toString());
  }
  // Update GUI - Unit connection pvData part
  leSchemaPage->setText(pvData.value("SchemaPage").toString());
  leFunctionEN->setText(pvData.value("FunctionEN").toString());
  leFunctionFR->setText(pvData.value("FunctionFR").toString());
  leFunctionDE->setText(pvData.value("FunctionDE").toString());
  leFunctionIT->setText(pvData.value("FunctionIT").toString());
  leSignalName->setText(pvData.value("SignalName").toString());
  sbSwAddress->setValue(pvData.value("SwAddress").toInt());
  lbUnitConnectorName->setText(pvData.value("???").toString());
  leUnitContactName->setText(pvData.value("UnitContactName").toString());
}

void mdtClUnitConnectionDialog::updateData()
{
  if(!leSchemaPage->text().isEmpty()){
    pvData.setValue("SchemaPage", leSchemaPage->text());
  }
  if(!leFunctionEN->text().isEmpty()){
    pvData.setValue("FunctionEN", leFunctionEN->text());
  }
  if(!leFunctionFR->text().isEmpty()){
    pvData.setValue("FunctionFR", leFunctionFR->text());
  }
  if(!leFunctionDE->text().isEmpty()){
    pvData.setValue("FunctionDE", leFunctionDE->text());
  }
  if(!leFunctionIT->text().isEmpty()){
    pvData.setValue("FunctionIT", leFunctionIT->text());
  }
  if(!leSignalName->text().isEmpty()){
    pvData.setValue("SignalName", leSignalName->text());
  }
  if(sbSwAddress->value() > 0){
    pvData.setValue("SwAddress", sbSwAddress->value());
  }
  /**
  if(!leUnitConnectorName->text().isEmpty()){
    pvData.setConnectorName(leUnitConnectorName->text());
  }
  */
  if(!leUnitContactName->text().isEmpty()){
    pvData.setValue("UnitContactName", leUnitContactName->text());
  }
}


void mdtClUnitConnectionDialog::hideArticleConnectionWidgets()
{
  lbArticleIoTypeLabel->setVisible(false);
  lbArticleIoType->setVisible(false);
  lbArticleFunctionEN->setVisible(false);
  pbCopyFunctionEN->setVisible(false);
  lbArticleFunctionFR->setVisible(false);
  pbCopyFunctionFR->setVisible(false);
  lbArticleFunctionDE->setVisible(false);
  pbCopyFunctionDE->setVisible(false);
  lbArticleFunctionIT->setVisible(false);
  pbCopyFunctionIT->setVisible(false);
  lbArticleConnectorName->setVisible(false);
  pbCopyConnectorName->setVisible(false);
  lbArticleContactName->setVisible(false);
  pbCopyContactName->setVisible(false);
}

void mdtClUnitConnectionDialog::showArticleConnectionWidgets()
{
  lbArticleIoTypeLabel->setVisible(true);
  lbArticleIoType->setVisible(true);
  lbArticleFunctionEN->setVisible(true);
  pbCopyFunctionEN->setVisible(true);
  lbArticleFunctionFR->setVisible(true);
  pbCopyFunctionFR->setVisible(true);
  lbArticleFunctionDE->setVisible(true);
  pbCopyFunctionDE->setVisible(true);
  lbArticleFunctionIT->setVisible(true);
  pbCopyFunctionIT->setVisible(true);
  lbArticleConnectorName->setVisible(true);
  pbCopyConnectorName->setVisible(true);
  lbArticleContactName->setVisible(true);
  pbCopyContactName->setVisible(true);
}
