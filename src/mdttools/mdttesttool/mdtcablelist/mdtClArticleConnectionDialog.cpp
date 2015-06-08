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
#include "mdtClArticleConnectionDialog.h"
#include "mdtClArticleConnection.h"
#include "mdtClArticleConnectorData.h"
#include "mdtClArticleConnectorSelectionDialog.h"
#include "mdtClConnectionTypeData.h"
#include "mdtClConnectionTypeModel.h"
#include <QWidget>
#include <QSqlQueryModel>
#include <QList>
#include <QSqlRecord>
#include <QComboBox>
#include <QLocale>
#include <QMessageBox>
#include <QStringList>

//#include <QDebug>

mdtClArticleConnectionDialog::mdtClArticleConnectionDialog(QWidget *parent, const QSqlDatabase & db)
 : QDialog(parent)
{
  pvDatabase = db;
  setupUi(this);
  ///pbSelectArticleConnector->setEnabled(false);
  connect(pbSelectArticleConnector, SIGNAL(clicked()), this, SLOT(selectArticleConnector()));
  // Setup connection type selection combobox
  cbConnectionType->setModel(new mdtClConnectionTypeModel(db));
  cbConnectionType->setModelColumn(1);
}

mdtClArticleConnectionDialog::~mdtClArticleConnectionDialog() 
{
  /// \todo Check if we have to delete our model or not (this not crashes, because QComboBox uses destroyed() signal..)
  delete cbConnectionType->model();
}

void mdtClArticleConnectionDialog::setData(const mdtClArticleConnectionData & data)
{
  Q_ASSERT(!data.keyData().articleId.isNull());

  pvData = data;
  updateDialog();
}

mdtClArticleConnectionData mdtClArticleConnectionDialog::data() const
{
  return pvData;
}

void mdtClArticleConnectionDialog::selectArticleConnector()
{
  Q_ASSERT(!pvData.keyData().articleId.isNull());

  mdtClArticleConnectorSelectionDialog dialog(this);

  if(!dialog.select(pvDatabase, pvData.keyData().articleId)){
    displayError(dialog.lastError());
    return;
  }
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  pvData.setArticleConnectorFk(dialog.selectedArticleConnectorKey());
  updateDialog();
}

void mdtClArticleConnectionDialog::accept()
{
  updateData();
  if(!checkData()){
    return;
  }
  QDialog::accept();
}

void mdtClArticleConnectionDialog::setCurrentConnectionType(const QString& type)
{
  int idx;

  for(idx = 0; idx < cbConnectionType->count(); ++idx){
    if(cbConnectionType->itemText(idx) == type){
      cbConnectionType->setCurrentIndex(idx);
      return;
    }
  }
  // type not found
  cbConnectionType->setCurrentIndex(-1);
}

void mdtClArticleConnectionDialog::setCurrentConnectionType(const mdtClConnectionTypeKeyData & type)
{
  Q_ASSERT(dynamic_cast<mdtClConnectionTypeModel*>(cbConnectionType->model()) != nullptr);

  auto *model = static_cast<mdtClConnectionTypeModel*>(cbConnectionType->model());
  int idx;

  idx = model->row(type);
  cbConnectionType->setCurrentIndex(idx);
}

void mdtClArticleConnectionDialog::updateDialog()
{
  // Update article connections data
  fld_ArticleContactName->setText(pvData.name.toString());
  if(pvData.resistance.isNull()){
    sbConnectionResistance->setValue(sbConnectionResistance->minimum());
  }else{
    sbConnectionResistance->setValue(pvData.resistance.toDouble());
  }
  fld_IoType->setText(pvData.ioType.toString());
  fld_FunctionEN->setText(pvData.functionEN.toString());
  fld_FunctionFR->setText(pvData.functionFR.toString());
  fld_FunctionDE->setText(pvData.functionDE.toString());
  fld_FunctionIT->setText(pvData.functionIT.toString());
  setCurrentConnectionType(pvData.keyData().connectionTypeFk);
  // Update article connector data if one is set
  if(pvData.isPartOfArticleConnector()){
    mdtClArticleConnection acnx(pvDatabase);
    mdtClArticleConnectorData connectorData;
    bool ok;
    connectorData = acnx.getArticleConnectorData(pvData.keyData().articleConnectorFk, false, ok);
    if(ok){
      lbArticleConnectorName->setText(connectorData.name.toString());
    }else{
      lbArticleConnectorName->setText("<Error!>");
    }
  }
}

void mdtClArticleConnectionDialog::updateData()
{
  Q_ASSERT(dynamic_cast<mdtClConnectionTypeModel*>(cbConnectionType->model()) != nullptr);
  auto *model = static_cast<mdtClConnectionTypeModel*>(cbConnectionType->model());
  Q_ASSERT(model != nullptr);

  // Article connector FK was allready set by selectArticleConnector()
  pvData.setConnectionType( model->keyData(cbConnectionType->currentIndex()) );
  if(sbConnectionResistance->value() < 0.0){
    pvData.resistance.clear();
  }else{
    pvData.resistance = sbConnectionResistance->value();
  }
  pvData.name = fld_ArticleContactName->text();
  pvData.ioType = fld_IoType->text();
  pvData.functionEN = fld_FunctionEN->text();
  pvData.functionFR = fld_FunctionFR->text();
  pvData.functionDE = fld_FunctionDE->text();
  pvData.functionIT = fld_FunctionIT->text();
}

bool mdtClArticleConnectionDialog::checkData()
{
  QStringList missingItems;

  // Check connection type
  if(pvData.keyData().connectionTypeFk.type() == mdtClConnectionType_t::Undefined){
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

void mdtClArticleConnectionDialog::displayError(const mdtError & error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}
