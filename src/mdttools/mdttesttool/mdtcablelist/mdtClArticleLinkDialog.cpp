/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "mdtClArticleLinkDialog.h"
#include "mdtClArticleConnectionSelectionDialog.h"
#include "Mdt/CableList/LinkTypeModel.h"  /// \todo update once migrated
#include "Mdt/CableList/LinkDirectionModel.h"  /// \todo update once migrated
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QModelIndex>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMessageBox>

#include <QDebug>

using Mdt::CableList::LinkTypeModel;      /// \todo Remove once migrated
using Mdt::CableList::LinkDirectionModel; /// \todo Remove once migrated

mdtClArticleLinkDialog::mdtClArticleLinkDialog(QWidget *parent, QSqlDatabase db, QVariant articleId)
 : QDialog(parent)
{
  pvDatabase = db;
  pvArticleId = articleId;
  // Setup UI
  setupUi(this);
  setWindowTitle(tr("Article link edition"));
  connect(pbStartConnection, SIGNAL(clicked()), this, SLOT(selectStartConnection()));
  connect(pbEndConnection, SIGNAL(clicked()), this, SLOT(selectEndConnection()));
  // Setup link type
  pvLinkTypeModel = new LinkTypeModel(this, db);
  cbLinkType->setModel(pvLinkTypeModel);
  cbLinkType->setModelColumn(1);
  cbLinkType->setCurrentIndex(-1);
  connect(cbLinkType, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbLinkTypeCurrentIndexChanged(int)));
  // Setup link direction
  pvLinkDirectionModel = new LinkDirectionModel(this, db);
  cbLinkDirection->setModel(pvLinkDirectionModel);
  cbLinkDirection->setModelColumn(1);
  cbLinkDirection->setCurrentIndex(-1);
  connect(cbLinkDirection, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbLinkDirectionCurrentIndexChanged(int)));
  // Setup connections
  pvArticleConnectionModel = new QSqlQueryModel(this);
  pvArticleConnectionModel->setQuery("SELECT Id_PK, ArticleConnectorName, ArticleContactName, IoType, FunctionEN, FunctionFR, FunctionDE, FunctionIT \
                                     FROM ArticleConnection_view WHERE Article_Id_FK = " + articleId.toString(), pvDatabase);
  lbStartConnectorName->clear();
  lbStartContactName->clear();
  lbEndConnectorName->clear();
  lbEndContactName->clear();
  lbUnit->clear();
}

mdtClArticleLinkDialog::~mdtClArticleLinkDialog()
{
}

void mdtClArticleLinkDialog::setConnectionEditionLocked(bool lock)
{
  pbStartConnection->setEnabled(!lock);
  pbEndConnection->setEnabled(!lock);
}

void mdtClArticleLinkDialog::setLinkType(LinkType type)
{
  int row;

  row = pvLinkTypeModel->row(type);
  cbLinkType->setCurrentIndex(row);
}

void mdtClArticleLinkDialog::setLinkDirection(LinkDirectionType d)
{
  int row;

  row = pvLinkDirectionModel->row(d);
  cbLinkDirection->setCurrentIndex(row);
}

void mdtClArticleLinkDialog::setValue(const QVariant & value)
{
  pvLinkData.resistance = value;
  sbValue->setValue(value.toDouble());
}

void mdtClArticleLinkDialog::setLinkData(const mdtClArticleLinkData & data)
{
  pvLinkData = data;
  // Display start and end connections
  displayCurrentSelectedStartConnection();
  displayCurrentSelectedEndConnection();
  // Display link type
  setLinkType(data.linkType());
  // Display link direction
  setLinkDirection(data.linkDirection());
  // Display values
  sbValue->setValue(data.resistance.toDouble());
}

// void mdtClArticleLinkDialog::setStartConnection(const mdtClArticleConnectionKeyData &fk)
// {
//   auto key = pvLinkData.keyData();
//   key.connectionStartFk = fk;
//   pvLinkData.setKeyData(key);
//   displayCurrentSelectedStartConnection();
// }
// 
// void mdtClArticleLinkDialog::setEndConnection(const mdtClArticleConnectionKeyData &fk)
// {
//   auto key = pvLinkData.keyData();
//   key.connectionEndFk = fk;
//   pvLinkData.setKeyData(key);
//   displayCurrentSelectedEndConnection();
// }

void mdtClArticleLinkDialog::onCbLinkTypeCurrentIndexChanged(int row)
{
  if(row < 0){
    lbUnit->setText("");
    return;
  }
  // We must update available directions regarding link type
  auto pk = pvLinkTypeModel->primaryKey(row);
  pvLinkDirectionModel->setLinkType(pk.type());
  if(pvLinkDirectionModel->rowCount() > 1){
    cbLinkDirection->setEnabled(true);
  }else{
    cbLinkDirection->setEnabled(false);
  }
  // Update displayed unit (V, Ohm, ...)
  lbUnit->setText("[" + pvLinkTypeModel->unit(row) + "]");
  // Update link data
  pvLinkData.setLinkType(pk.type());
}

void mdtClArticleLinkDialog::onCbLinkDirectionCurrentIndexChanged(int row)
{
  if(row < 0){
    lbLinkDirectionAsciiPicture->setText("");
    return;
  }
  auto key = pvLinkDirectionModel->directionPk(row);
  // Update the ASCII picture
  lbLinkDirectionAsciiPicture->setText(pvLinkDirectionModel->pictureAscii(row));
  // Update link data
  pvLinkData.setLinkDirection(key.direction());
}

void mdtClArticleLinkDialog::selectStartConnection()
{
  mdtClArticleConnectionSelectionDialog acsDialog(this);

  if(!acsDialog.select(pvDatabase, pvArticleId, mdtClArticleConnectionSelectionDialog::ArticleConnectorMembership_t::All, mdtClArticleConnectionSelectionDialog::ArticleLinkUsage_t::NotUsedAsConnectionStart, false)){
    /// \todo We should display a message
    return;
  }
  if(acsDialog.exec() != QDialog::Accepted){
    return;
  }
  // Store result
  auto key = acsDialog.selectedArticleConnectionKey();
  pvLinkData.setConnectionStartFk(key);
  // Update GUI
  displayCurrentSelectedStartConnection();
}

void mdtClArticleLinkDialog::selectEndConnection()
{
  mdtClArticleConnectionSelectionDialog acsDialog(this);

  if(!acsDialog.select(pvDatabase, pvArticleId, mdtClArticleConnectionSelectionDialog::ArticleConnectorMembership_t::All, mdtClArticleConnectionSelectionDialog::ArticleLinkUsage_t::NotUsedAsConnectionEnd, false)){
    /// \todo We should display a message
    return;
  }
  if(acsDialog.exec() != QDialog::Accepted){
    return;
  }
  // Store result
  auto key = acsDialog.selectedArticleConnectionKey();
  pvLinkData.setConnectionEndFk(key);
  // Update GUI
  displayCurrentSelectedEndConnection();
}

void mdtClArticleLinkDialog::accept()
{
  QStringList errorList;
  QString infoText;
  int i;
  auto key = pvLinkData.keyData();

  // Link type was allready set by onCbLinkTypeCurrentIndexChanged() - check it
  if(key.linkTypeFk().isNull()){
    errorList << tr("Link type is not set");
  }
  // Link direction was allready set by onCbLinkDirectionCurrentIndexChanged() - check it
  if(key.linkDirectionFk().isNull()){
    errorList << tr("Link direction is not set");
  }
  // Start and end connections are allready set afetr selection by user
  if(key.pk.connectionStartId.isNull()){
    errorList << tr("Start connection is not set");
  }
  if(key.pk.connectionEndId.isNull()){
    errorList << tr("End connection is not set");
  }
  // Store resistance
  pvLinkData.resistance = sbValue->value();
  // If something is missing, display a message to the user
  if(!errorList.isEmpty()){
    infoText = tr("Please check following points and try again:\n");
    for(i = 0; i < errorList.size(); ++i){
      infoText += "- " + errorList.at(i) + "\n";
    }
    QMessageBox msgBox;
    msgBox.setText(tr("Some informations are missing"));
    msgBox.setInformativeText(infoText);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Article link warning"));
    msgBox.exec();
    return;
  }
  // Here, all is ok, accept dialog
  QDialog::accept();
}

void mdtClArticleLinkDialog::reject()
{
  pvLinkData.clear();
  QDialog::reject();
}

void mdtClArticleLinkDialog::displayCurrentSelectedStartConnection()
{
  auto id = pvLinkData.keyData().pk.connectionStartId;
  QModelIndex index;
  QVariant data;
  int row;

  if(id.isNull()){
    lbStartConnectorName->clear();
    lbStartContactName->clear();
    return;
  }
  for(row = 0; row < pvArticleConnectionModel->rowCount(); ++row){
    index = pvArticleConnectionModel->index(row, 0);
    data = pvArticleConnectionModel->data(index);
    if(data == id){
      // Set connector name
      index = pvArticleConnectionModel->index(row, 1);
      data = pvArticleConnectionModel->data(index);
      lbStartConnectorName->setText(data.toString());
      // Set contact name
      index = pvArticleConnectionModel->index(row, 2);
      data = pvArticleConnectionModel->data(index);
      lbStartContactName->setText(data.toString());
      break;
    }
  }
}

void mdtClArticleLinkDialog::displayCurrentSelectedEndConnection()
{
  auto id = pvLinkData.keyData().pk.connectionEndId;
  QModelIndex index;
  QVariant data;
  int row;

  if(id.isNull()){
    lbEndConnectorName->clear();
    lbEndContactName->clear();
    return;
  }
  for(row = 0; row < pvArticleConnectionModel->rowCount(); ++row){
    index = pvArticleConnectionModel->index(row, 0);
    data = pvArticleConnectionModel->data(index);
    if(data == id){
      // Set connector name
      index = pvArticleConnectionModel->index(row, 1);
      data = pvArticleConnectionModel->data(index);
      lbEndConnectorName->setText(data.toString());
      // Set contact name
      index = pvArticleConnectionModel->index(row, 2);
      data = pvArticleConnectionModel->data(index);
      lbEndContactName->setText(data.toString());
      break;
    }
  }
}
