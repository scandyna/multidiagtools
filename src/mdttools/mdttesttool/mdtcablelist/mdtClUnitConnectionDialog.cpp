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
#include <QMessageBox>

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
  pbSelectUnitConnector->setEnabled(false);
}

mdtClUnitConnectionDialog::~mdtClUnitConnectionDialog()
{
}

void mdtClUnitConnectionDialog::setUnitId(const QVariant & id)
{
  pvData.setUnitId(id);
}

void mdtClUnitConnectionDialog::setArticleConnectionId(const QVariant & id)
{
  pvData.articleConnectionData().setConnectionId(id);
}

void mdtClUnitConnectionDialog::setData(const mdtClUnitConnectionData &data)
{
  // Update GUI - Article connection data part
  /** \todo Currently, changing unit connector is not implemented
  if(data.articleConnectionData().connectorName().isNull()){
    lbArticleConnectorName->setVisible(false);
    pbCopyConnectorName->setVisible(false);
  }else{
    lbArticleConnectorName->setVisible(true);
    pbCopyConnectorName->setVisible(true);
    lbArticleConnectorName->setText(data.articleConnectionData().connectorName().toString());
  }
  */
  if(data.articleConnectionData().contactName().isNull()){
    lbArticleContactName->setVisible(false);
    pbCopyContactName->setVisible(false);
  }else{
    lbArticleContactName->setVisible(true);
    pbCopyContactName->setVisible(true);
    lbArticleContactName->setText(data.articleConnectionData().contactName().toString());
  }
  if(data.articleConnectionData().ioType().isNull()){
    lbArticleIoType->setVisible(false);
    lbArticleIoTypeLabel->setVisible(false);
  }else{
    lbArticleIoType->setVisible(true);
    lbArticleIoTypeLabel->setVisible(true);
    lbArticleIoType->setText(data.articleConnectionData().ioType().toString());
  }
  if(data.articleConnectionData().functionEN().isNull()){
    lbArticleFunctionEN->setVisible(false);
    pbCopyFunctionEN->setVisible(false);
  }else{
    lbArticleFunctionEN->setVisible(true);
    pbCopyFunctionEN->setVisible(true);
    lbArticleFunctionEN->setText(data.articleConnectionData().functionEN().toString());
  }
  if(data.articleConnectionData().functionFR().isNull()){
    lbArticleFunctionFR->setVisible(false);
    pbCopyFunctionFR->setVisible(false);
  }else{
    lbArticleFunctionFR->setVisible(true);
    pbCopyFunctionFR->setVisible(true);
    lbArticleFunctionFR->setText(data.articleConnectionData().functionFR().toString());
  }
  if(data.articleConnectionData().functionDE().isNull()){
    lbArticleFunctionDE->setVisible(false);
    pbCopyFunctionDE->setVisible(false);
  }else{
    lbArticleFunctionDE->setVisible(true);
    pbCopyFunctionDE->setVisible(true);
    lbArticleFunctionDE->setText(data.articleConnectionData().functionDE().toString());
  }
  if(data.articleConnectionData().functionIT().isNull()){
    lbArticleFunctionIT->setVisible(false);
    pbCopyFunctionIT->setVisible(false);
  }else{
    lbArticleFunctionIT->setVisible(true);
    pbCopyFunctionIT->setVisible(true);
    lbArticleFunctionIT->setText(data.articleConnectionData().functionIT().toString());
  }
  // Update GUI - Unit connection data part
  leSchemaPage->setText(data.schemaPage().toString());
  leFunctionEN->setText(data.functionEN().toString());
  leFunctionFR->setText(data.functionFR().toString());
  leFunctionDE->setText(data.functionDE().toString());
  leFunctionIT->setText(data.functionIT().toString());
  leSignalName->setText(data.signalName().toString());
  sbSwAddress->setValue(data.swAddress().toInt());
  lbUnitConnectorName->setText(data.connectorName().toString());
  leUnitContactName->setText(data.contactName().toString());
  // Store data
  pvData = data;
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
  // Store data
  if(!leSchemaPage->text().isEmpty()){
    pvData.setSchemaPage(leSchemaPage->text());
  }
  if(!leFunctionEN->text().isEmpty()){
    pvData.setFunctionEN(leFunctionEN->text());
  }
  if(!leFunctionFR->text().isEmpty()){
    pvData.setFunctionFR(leFunctionFR->text());
  }
  if(!leFunctionDE->text().isEmpty()){
    pvData.setFunctionDE(leFunctionDE->text());
  }
  if(!leFunctionIT->text().isEmpty()){
    pvData.setFunctionIT(leFunctionIT->text());
  }
  if(!leSignalName->text().isEmpty()){
    pvData.setSignalName(leSignalName->text());
  }
  if(sbSwAddress->value() > 0){
    pvData.setSwAddress(sbSwAddress->value());
  }
  /**
  if(!leUnitConnectorName->text().isEmpty()){
    pvData.setConnectorName(leUnitConnectorName->text());
  }
  */
  if(!leUnitContactName->text().isEmpty()){
    pvData.setContactName(leUnitContactName->text());
  }
  QDialog::accept();
}

void mdtClUnitConnectionDialog::reject()
{
  pvData.clear();
  QDialog::reject();
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
