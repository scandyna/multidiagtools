/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtClUnitConnectorData.h"
#include "mdtClUnitConnectorSelectionDialog.h"
#include "mdtClArticleConnectionSelectionDialog.h"
#include "mdtClConnectorContactSelectionDialog.h"
#include "mdtClUnitConnection.h"
#include "mdtClConnectionTypeModel.h"
#include <QMessageBox>
#include <QList>
#include <QStringList>
#include <QLocale>

//#include <QDebug>

mdtClUnitConnectionDialog::mdtClUnitConnectionDialog(QWidget *parent, QSqlDatabase db, mode_t mode)
 : QDialog(parent)
{
  pvDatabase = db;
  pvMode = mode;
  setupUi(this);
  connect(pbCopyFunctionEN, SIGNAL(clicked()), this, SLOT(copyFunctionEN()));
  connect(pbCopyFunctionFR, SIGNAL(clicked()), this, SLOT(copyFunctionFR()));
  connect(pbCopyFunctionDE, SIGNAL(clicked()), this, SLOT(copyFunctionDE()));
  connect(pbCopyFunctionIT, SIGNAL(clicked()), this, SLOT(copyFunctionIT()));
  connect(pbCopyContactName, SIGNAL(clicked()), this, SLOT(copyContactName()));
  connect(pbCopyResistance, SIGNAL(clicked()), this, SLOT(copyResistance()));
  hideArticleConnectionWidgets();
  connect(pbSelectUnitConnector, SIGNAL(clicked()), this, SLOT(selectUnitConnector()));
  connect(pbSelectContact, SIGNAL(clicked()), this, SLOT(selectConnection()));
  connect(pbSetNoConnector, SIGNAL(clicked()), this, SLOT(setNoConnector()));
  // In edition mode, we cannot act on unit connector
  if(pvMode != Add){
    pbSetNoConnector->setVisible(false);
    pbSelectUnitConnector->setVisible(false);
  }
  // Currently, ranges functionnality is not implemented
  gbRanges->setVisible(false);
  // Connection type
  pvConnectionTypeModel = new mdtClConnectionTypeModel(this, pvDatabase);
  cbConnectionType->setModel(pvConnectionTypeModel);
  cbConnectionType->setModelColumn(1);
  cbConnectionType->setCurrentIndex(-1);
}

mdtClUnitConnectionDialog::~mdtClUnitConnectionDialog()
{
}

void mdtClUnitConnectionDialog::setData(const mdtClUnitConnectionData &data, const QVariant & baseArticleId)
{
  Q_ASSERT(!data.keyData().unitId().isNull());

  pvData = data;
  pvBaseArticleId = baseArticleId;
  updateDialog();
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
  mdtClUnitConnectorSelectionDialog dialog(this);
  mdtClUnitConnectorData unitConnectorData;

  if(!dialog.select(pvDatabase, pvData.keyData().unitId())){
    displayError(dialog.lastError());
    return;
  }
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  unitConnectorData = dialog.selectedUnitConnectorData();
  if(unitConnectorData.isNull()){
    return;
  }
  pvData.setUnitConnectorFk(unitConnectorData.keyData());
  lbUnitConnectorName->setText(unitConnectorData.name.toString());
  updateDialog();
}

void mdtClUnitConnectionDialog::setNoConnector()
{
  /*
   * If current unit connector is based on a article connector,
   * we must also "detach" unit connection from article connection
   */
  if(pvData.keyData().unitConnectorFk().isBasedOnArticleConnector()){
    pvData.clearArticleConnectionFk();
  }
  pvData.clearUnitConnectorFk();
  updateDialog();
}

void mdtClUnitConnectionDialog::selectConnection()
{
  Q_ASSERT(canSelectConnection());

  if( (!pvBaseArticleId.isNull()) || (pvData.keyData().unitConnectorFk().isBasedOnArticleConnector()) ){
    setConnectionFromArticleConnection();
  }else{
    setConnectionFromConnectorContact();
  }
  updateDialog();
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
  leUnitContactName->setText(lbContactName->text());
}

void mdtClUnitConnectionDialog::copyResistance()
{
  if(!pvData.resistance.isNull()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Contact resistance was allready set."));
    msgBox.setInformativeText(tr("Do you want to overwrite it ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  pvData.resistance = pvArticleConnectionResistance;
  if(pvData.resistance.isNull()){
    sbConnectionResistance->setValue(sbConnectionResistance->minimum());
  }else{
    sbConnectionResistance->setValue(pvData.resistance.toDouble());
  }
}

void mdtClUnitConnectionDialog::accept()
{
  // Store data and accept
  updateData();
  if(!checkData()){
    return;
  }
  QDialog::accept();
}

void mdtClUnitConnectionDialog::reject()
{
  pvData.clear();
  QDialog::reject();
}

void mdtClUnitConnectionDialog::setCurrentConnectionType(mdtClConnectionType_t t)
{
  int row = pvConnectionTypeModel->row(t);
  cbConnectionType->setCurrentIndex(row);
}

void mdtClUnitConnectionDialog::setConnectionFromArticleConnection()
{
  Q_ASSERT(!pvBaseArticleId.isNull());

  mdtClArticleConnectionSelectionDialog dialog(this);
  mdtClArticleConnectorKeyData articleConnectorFk;
  mdtClArticleConnectionData articleConnectionData;

  articleConnectorFk = pvData.keyData().unitConnectorFk().articleConnectorFk();
  // Setup and show selection dialog
  if(articleConnectorFk.isNull()){
    if(!dialog.select(pvDatabase , pvBaseArticleId, pvData.keyData().unitId(), mdtClArticleConnectionSelectionDialog::ArticleConnectorMembership_t::NotPartOfArticleConnector, false)){
      displayError(dialog.lastError());
      return;
    }
  }else{
    if(!dialog.select(pvDatabase, articleConnectorFk, pvData.keyData().unitId(), false)){
      displayError(dialog.lastError());
      return;
    }
  }
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store result
  articleConnectionData = dialog.selectedArticleConnectionData();
  if(articleConnectionData.isNull()){
    return;
  }
  pvData.setArticleConnectionFk(articleConnectionData.keyData());
  pvData.name = articleConnectionData.name;
  // Dialog update is done by selectConnection()
}

void mdtClUnitConnectionDialog::setConnectionFromConnectorContact()
{
  Q_ASSERT(!pvData.keyData().unitConnectorFk().connectorFk().isNull());
  Q_ASSERT(!pvData.isBasedOnArticleConnection());

  mdtClConnectorContactSelectionDialog dialog(this);
  mdtClConnectorContactData contactData;

  // Setup and show selection dialog
  if(!dialog.select(pvDatabase, pvData.keyData().unitConnectorFk().connectorFk(), false)){
    displayError(dialog.lastError());
    return;
  }
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  contactData = dialog.selectedContactData();
  if(contactData.isNull()){
    return;
  }
  // Update data
  pvData.setConnectionType(contactData.connectionType());
  pvData.name = contactData.name;
  // Dialog update is done by selectConnection()
}

void mdtClUnitConnectionDialog::updateConnectorData()
{
  mdtClUnitConnection ucnx(pvDatabase);
  mdtClUnitConnectorData unitConnectorData;
  mdtClUnitConnectorKeyData unitConnectorFk;
  bool ok;

  // Get unit connector data
  if(pvData.isPartOfUnitConnector()){
    unitConnectorData = ucnx.getUnitConnectorData(pvData.keyData().unitConnectorFk().pk, false, ok);
    if(!ok){
      displayError(ucnx.lastError());
    }
  }
  // Update unit connector name
  if(unitConnectorData.isNull()){
    lbUnitConnectorName->setText(tr("<None>"));
  }else{
    lbUnitConnectorName->setText(unitConnectorData.name.toString());
  }
  // Check on what unit connector (for witch unit connection is part) is based
  unitConnectorFk = unitConnectorData.keyData();
  if(unitConnectorFk.isBasedOnArticleConnector()){
    lbArticleConnectorNameLabel->setVisible(true);
    lbArticleConnectorName->setVisible(true);
  }else{
    lbArticleConnectorNameLabel->setVisible(false);
    lbArticleConnectorName->setVisible(false);
  }
  // Update article connector name
  if(unitConnectorFk.isBasedOnArticleConnector()){
    mdtClArticleConnectorData articeConnectorData;
    articeConnectorData = ucnx.getArticleConnectorData(unitConnectorFk.articleConnectorFk(), false, ok);
    if(!ok){
      displayError(ucnx.lastError());
      lbArticleConnectorName->clear();
      return;
    }
    lbArticleConnectorName->setText(articeConnectorData.name.toString());
  }else{
    lbArticleConnectorName->clear();
  }
}

void mdtClUnitConnectionDialog::updateDialog()
{
  // Connector part
  updateConnectorData();
  // Article connection data part
  if(pvData.isBasedOnArticleConnection()){
    mdtClUnitConnection ucnx(pvDatabase);
    mdtClArticleConnectionData data;
    bool ok;
    data = ucnx.getArticleConnectionData(pvData.keyData().articleConnectionFk(), ok);
    if(!ok){
      displayError(ucnx.lastError());
    }
    if(data.name.isNull()){
      lbContactName->setVisible(false);
      pbCopyContactName->setVisible(false);
    }else{
      lbContactName->setVisible(true);
      pbCopyContactName->setVisible(true);
      lbContactName->setText(data.name.toString());
      if(pvData.name.isNull()){
        pvData.name = data.name;
      }
    }
    pvArticleConnectionResistance = data.resistance;
    if(pvArticleConnectionResistance.isNull()){
      lbArticleConnectionResistance->setVisible(false);
      pbCopyResistance->setVisible(false);
    }else{
      lbArticleConnectionResistance->setVisible(true);
      pbCopyResistance->setVisible(true);
      lbArticleConnectionResistance->setText(pvArticleConnectionResistance.toString() + " [Ohm]");
      if(pvData.resistance.isNull()){
        pvData.resistance = pvArticleConnectionResistance;
      }
    }
    if(data.ioType.isNull()){
      lbArticleIoType->setVisible(false);
      lbArticleIoTypeLabel->setVisible(false);
    }else{
      lbArticleIoType->setVisible(true);
      lbArticleIoTypeLabel->setVisible(true);
      lbArticleIoType->setText(data.ioType.toString());
    }
    if(data.functionEN.isNull()){
      lbArticleFunctionEN->setVisible(false);
      pbCopyFunctionEN->setVisible(false);
    }else{
      lbArticleFunctionEN->setVisible(true);
      pbCopyFunctionEN->setVisible(true);
      lbArticleFunctionEN->setText(data.functionEN.toString());
    }
    if(data.functionFR.isNull()){
      lbArticleFunctionFR->setVisible(false);
      pbCopyFunctionFR->setVisible(false);
    }else{
      lbArticleFunctionFR->setVisible(true);
      pbCopyFunctionFR->setVisible(true);
      lbArticleFunctionFR->setText(data.functionFR.toString());
    }
    if(data.functionDE.isNull()){
      lbArticleFunctionDE->setVisible(false);
      pbCopyFunctionDE->setVisible(false);
    }else{
      lbArticleFunctionDE->setVisible(true);
      pbCopyFunctionDE->setVisible(true);
      lbArticleFunctionDE->setText(data.functionDE.toString());
    }
    if(data.functionIT.isNull()){
      lbArticleFunctionIT->setVisible(false);
      pbCopyFunctionIT->setVisible(false);
    }else{
      lbArticleFunctionIT->setVisible(true);
      pbCopyFunctionIT->setVisible(true);
      lbArticleFunctionIT->setText(data.functionIT.toString());
    }
  }else{
    pvArticleConnectionResistance.clear();
    hideArticleConnectionWidgets();
  }
  // Unit connection pvData part
  pbSelectContact->setVisible(canSelectConnection());
  setCurrentConnectionType(pvData.connectionType());
  cbConnectionType->setEnabled(canSelectConnectionType());
  leUnitContactName->setText(pvData.name.toString());
  leSchemaPage->setText(pvData.schemaPage.toString());
  leFunctionEN->setText(pvData.functionEN.toString());
  leFunctionFR->setText(pvData.functionFR.toString());
  leFunctionDE->setText(pvData.functionDE.toString());
  leFunctionIT->setText(pvData.functionIT.toString());
  leSignalName->setText(pvData.signalName.toString());
  sbSwAddress->setValue(pvData.swAddress.toInt());
  if(pvData.resistance.isNull()){
    sbConnectionResistance->setValue(sbConnectionResistance->minimum());
  }else{
    sbConnectionResistance->setValue(pvData.resistance.toDouble());
  }
}

void mdtClUnitConnectionDialog::updateData()
{
  // Set connection type
  if(!pvData.isBasedOnArticleConnection()){
    pvData.setConnectionType(pvConnectionTypeModel->keyData(cbConnectionType->currentIndex()).type());
  }
  // Update data part
  if(leUnitContactName->text().isEmpty()){
    pvData.name.clear();
  }else{
    pvData.name = leUnitContactName->text();
  }
  if(leSchemaPage->text().isEmpty()){
    pvData.schemaPage.clear();
  }else{
    pvData.schemaPage = leSchemaPage->text();
  }
  if(leFunctionEN->text().isEmpty()){
    pvData.functionEN.clear();
  }else{
    pvData.functionEN = leFunctionEN->text();
  }
  if(leFunctionFR->text().isEmpty()){
    pvData.functionFR.clear();
  }else{
    pvData.functionFR = leFunctionFR->text();
  }
  if(leFunctionDE->text().isEmpty()){
    pvData.functionDE.clear();
  }else{
    pvData.functionDE = leFunctionDE->text();
  }
  if(leFunctionIT->text().isEmpty()){
    pvData.functionIT.clear();
  }else{
    pvData.functionIT = leFunctionIT->text();
  }
  if(leSignalName->text().isEmpty()){
    pvData.signalName.clear();
  }else{
    pvData.signalName = leSignalName->text();
  }
  if(sbSwAddress->value() == 0){
    pvData.swAddress.clear();
  }else{
    pvData.swAddress = sbSwAddress->value();
  }
  if(sbConnectionResistance->value() < 0.0){
    pvData.resistance.clear();
  }else{
    pvData.resistance = sbConnectionResistance->value();
  }
}

bool mdtClUnitConnectionDialog::checkData()
{
  QStringList missingItems;

  // Check connection type
  if(pvData.connectionType() == mdtClConnectionType_t::Undefined){
    missingItems << tr("Connection type is not defined");
  }
  // Check connection name
  if(pvData.name.toString().trimmed().isEmpty()){
    missingItems << tr("Contact (name) is not set");
  }
  // If some data are missing, display a message
  if(!missingItems.isEmpty()){
    QMessageBox msgBox(this);
    QString text;
    msgBox.setText(tr("Some data are not correctly set."));
    text = tr("Problems are:\n");
    for(const auto & item : missingItems){
      text += " - " + item + "\n";
    }
    msgBox.setInformativeText(text);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return false;
  }

  return true;
}

void mdtClUnitConnectionDialog::hideArticleConnectionWidgets()
{
  lbContactName->setVisible(false);
  lbArticleConnectionResistance->setVisible(false);
  pbCopyResistance->setVisible(false);
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
  lbArticleConnectorNameLabel->setVisible(false);
  pbCopyContactName->setVisible(false);
}

void mdtClUnitConnectionDialog::showArticleConnectionWidgets()
{
  lbContactName->setVisible(true);
  lbArticleConnectionResistance->setVisible(true);
  pbCopyResistance->setVisible(true);
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
  lbArticleConnectorNameLabel->setVisible(true);
  pbCopyContactName->setVisible(true);
}

void mdtClUnitConnectionDialog::displayError(const mdtError &error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  ///msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}

bool mdtClUnitConnectionDialog::canSelectConnection() const
{
  /*
   * Selecting a connection has only sense when:
   *  - Unit connection is part on a unit connector witch,
   *   -> is based on a connector or a article connector
   *  - Unit is based on a article
   */
  if(pvData.isPartOfUnitConnector()){
    mdtClUnitConnectorKeyData unitConnectorFk = pvData.keyData().unitConnectorFk();
    if(unitConnectorFk.isBasedOnArticleConnector() || unitConnectorFk.isBasedOnConnector()){
      return true;
    }
  }
  if(!pvBaseArticleId.isNull()){
    return true;
  }
  return false;
}

bool mdtClUnitConnectionDialog::canSelectConnectionType() const
{
  if(pvData.isPartOfUnitConnector()){
    mdtClUnitConnectorKeyData unitConnectorFk = pvData.keyData().unitConnectorFk();
    if(unitConnectorFk.isBasedOnConnector()){
      return false;
    }
  }
  if(pvData.isBasedOnArticleConnection()){
    return false;
  }
  return true;
}
