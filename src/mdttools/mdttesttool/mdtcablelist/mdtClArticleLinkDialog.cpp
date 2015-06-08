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
#include "mdtClArticleLinkDialog.h"
#include "mdtClArticleConnectionSelectionDialog.h"

#include "mdtSqlSelectionDialog.h"

#include "mdtClLinkTypeModel.h"
#include "mdtClLinkDirectionModel.h"
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

//#include <QDebug>

mdtClArticleLinkDialog::mdtClArticleLinkDialog(QWidget *parent, QSqlDatabase db, QVariant articleId)
 : QDialog(parent)
{
  pvDatabase = db;
  pvArticleId = articleId;
  // Setup data
//   if(!pvLinkData.addAllFields("ArticleLink_tbl", pvDatabase)){
//     /// \todo Check if this works
//     QMessageBox msgBox;
//     msgBox.setText(pvLinkData.lastError().text());
//     msgBox.setDetailedText(pvLinkData.lastError().systemText());
//     msgBox.setIcon(QMessageBox::Critical);
//     msgBox.setWindowTitle(tr("Article link error"));
//     msgBox.exec();
//     reject();
//     return;
//   }
  // Setup UI
  setupUi(this);
  setWindowTitle(tr("Article link edition"));
  connect(pbStartConnection, SIGNAL(clicked()), this, SLOT(selectStartConnection()));
  connect(pbEndConnection, SIGNAL(clicked()), this, SLOT(selectEndConnection()));
  // Setup link type
  pvLinkTypeModel = new mdtClLinkTypeModel(this, db);
  cbLinkType->setModel(pvLinkTypeModel);
  cbLinkType->setModelColumn(1);
  cbLinkType->setCurrentIndex(-1);
  connect(cbLinkType, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbLinkTypeCurrentIndexChanged(int)));
  // Setup link direction
  pvLinkDirectionModel = new mdtClLinkDirectionModel(this, db);
  cbLinkDirection->setModel(pvLinkDirectionModel);
  cbLinkDirection->setModelColumn(1);
  connect(cbLinkDirection, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbLinkDirectionCurrentIndexChanged(int)));
  cbLinkDirection->setCurrentIndex(-1);
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

void mdtClArticleLinkDialog::setLinkType(mdtClLinkType_t t)
{
  auto key = pvLinkData.keyData();
  int row;

  key.linkTypeFk.setType(t);
  row = pvLinkTypeModel->row(key.linkTypeFk);
  cbLinkType->setCurrentIndex(row);
  pvLinkData.setKeyData(key);
}

void mdtClArticleLinkDialog::setLinkDirection(mdtClLinkDirection_t d)
{
  auto key = pvLinkData.keyData();
  int row;

  key.linkDirectionFk.setDirection(d);
  row = pvLinkDirectionModel->row(key.linkDirectionFk);
  cbLinkDirection->setCurrentIndex(row);
  pvLinkData.setKeyData(key);
}

// void mdtClArticleLinkDialog::setLinkTypeCode(const QVariant & code)
// {
//   QModelIndex index;
//   int row;
//   QVariant data;
// 
//   pvLinkData.setValue("LinkType_Code_FK", code);
//   for(row = 0; row < pvLinkTypeModel->rowCount(); ++row){
//     index = pvLinkTypeModel->index(row, 0);
//     data = pvLinkTypeModel->data(index);
//     if(data == code){
//       cbLinkType->setCurrentIndex(row);
//       return;
//     }
//   }
// }

// void mdtClArticleLinkDialog::setLinkDirectionCode(const QVariant & code)
// {
//   QModelIndex index;
//   int row;
//   QVariant data;
// 
//   pvLinkData.setValue("LinkDirection_Code_FK", code);
//   for(row = 0; row < pvLinkDirectionModel->rowCount(); ++row){
//     index = pvLinkDirectionModel->index(row, 0);
//     data = pvLinkDirectionModel->data(index);
//     if(data == code){
//       cbLinkDirection->setCurrentIndex(row);
//       return;
//     }
//   }
// }

void mdtClArticleLinkDialog::setValue(const QVariant & value)
{
//   pvLinkData.setValue("Resistance", value);
  pvLinkData.resistance = value;
  sbValue->setValue(value.toDouble());
}

void mdtClArticleLinkDialog::setStartConnection(const mdtClArticleConnectionKeyData &fk)
{
  auto key = pvLinkData.keyData();
  key.connectionStartFk = fk;
  pvLinkData.setKeyData(key);
  displayCurrentSelectedStartConnection();
}

void mdtClArticleLinkDialog::setEndConnection(const mdtClArticleConnectionKeyData &fk)
{
  auto key = pvLinkData.keyData();
  key.connectionEndFk = fk;
  pvLinkData.setKeyData(key);
  displayCurrentSelectedEndConnection();
}

// void mdtClArticleLinkDialog::setStartConnectionId(const QVariant & id)
// {
//   pvLinkData.setValue("ArticleConnectionStart_Id_FK", id);
//   displayCurrentSelectedStartConnection();
// }

// void mdtClArticleLinkDialog::setEndConnectionId(const QVariant & id)
// {
//   pvLinkData.setValue("ArticleConnectionEnd_Id_FK", id);
//   displayCurrentSelectedEndConnection();
// }

// mdtSqlRecord mdtClArticleLinkDialog::linkData() const
// {
//   return pvLinkData;
// }


void mdtClArticleLinkDialog::onCbLinkTypeCurrentIndexChanged(int row)
{
  auto key = pvLinkData.keyData();

  if(row < 0){
    lbUnit->setText("");
    return;
  }
  // We must update available directions regarding link type
  key.linkTypeFk = pvLinkTypeModel->keyData(row);
  pvLinkDirectionModel->setLinkType(key.linkTypeFk);
  if(pvLinkDirectionModel->rowCount() > 1){
    cbLinkDirection->setEnabled(true);
  }else{
    cbLinkDirection->setEnabled(false);
  }
  // Update displayed unit (V, Ohm, ...)
  lbUnit->setText("[" + pvLinkTypeModel->unit(row) + "]");
  // Update link data
  pvLinkData.setKeyData(key);
}

void mdtClArticleLinkDialog::onCbLinkDirectionCurrentIndexChanged(int row)
{
  auto key = pvLinkData.keyData();
//   QModelIndex index;
//   QVariant data;

  if(row < 0){
    lbLinkDirectionAsciiPicture->setText("");
    return;
  }
  key.linkDirectionFk = pvLinkDirectionModel->keyData(row);
  // Update the ASCII picture
//   index = pvLinkDirectionModel->index(row, 2);
//   data = pvLinkDirectionModel->data(index);
//   lbLinkDirectionAsciiPicture->setText(data.toString());
  lbLinkDirectionAsciiPicture->setText(pvLinkDirectionModel->pictureAscii(row));
  // Update link data
  pvLinkData.setKeyData(key);
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
  
  return;
  
  mdtSqlSelectionDialog dialog(this);

  // Setup and show dialog
  dialog.setMessage(tr("Please select start connection:"));
  dialog.setQuery(pvArticleConnectionModel->query().lastQuery(), pvDatabase, false);
  dialog.setColumnHidden("Id_PK", true);
  dialog.setHeaderData("ArticleConnectorName", "Connector");
  dialog.setHeaderData("ArticleContactName", "Contact");
  dialog.setHeaderData("IoType", "I/O type");
  dialog.setHeaderData("FunctionEN", "Function\n(English)");
  dialog.setHeaderData("FunctionFR", "Function\n(French)");
  dialog.setHeaderData("FunctionDE", "Function\n(German)");
  dialog.setHeaderData("FunctionIT", "Function\n(Italian)");
  dialog.addColumnToSortOrder("ArticleConnectorName", Qt::AscendingOrder);
  dialog.addColumnToSortOrder("ArticleContactName", Qt::AscendingOrder);
  dialog.sort();
  dialog.addSelectionResultColumn("Id_PK");
  dialog.resize(700, 400);
  dialog.setWindowTitle(tr("Article start connection selection"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store result
  Q_ASSERT(dialog.selectionResult().size() == 1);
//   pvLinkData.setValue("ArticleConnectionStart_Id_FK", dialog.selectionResult().at(0));
  // Update GUI
  displayCurrentSelectedStartConnection();
}

void mdtClArticleLinkDialog::selectEndConnection()
{
  mdtSqlSelectionDialog dialog(this);

  // Setup and show dialog
  dialog.setMessage(tr("Please select end connection:"));
  dialog.setQuery(pvArticleConnectionModel->query().lastQuery(), pvDatabase, false);
  dialog.setColumnHidden("Id_PK", true);
  dialog.setHeaderData("ArticleConnectorName", "Connector");
  dialog.setHeaderData("ArticleContactName", "Contact");
  dialog.setHeaderData("IoType", "I/O type");
  dialog.setHeaderData("FunctionEN", "Function\n(English)");
  dialog.setHeaderData("FunctionFR", "Function\n(French)");
  dialog.setHeaderData("FunctionDE", "Function\n(German)");
  dialog.setHeaderData("FunctionIT", "Function\n(Italian)");
  dialog.addColumnToSortOrder("ArticleConnectorName", Qt::AscendingOrder);
  dialog.addColumnToSortOrder("ArticleContactName", Qt::AscendingOrder);
  dialog.sort();
  dialog.addSelectionResultColumn("Id_PK");
  dialog.resize(700, 400);
  dialog.setWindowTitle(tr("Article end connection selection"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store result
  Q_ASSERT(dialog.selectionResult().size() == 1);
//   pvLinkData.setValue("ArticleConnectionEnd_Id_FK", dialog.selectionResult().at(0));
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
  if(key.linkTypeFk.isNull()){
    errorList << tr("Link type is not set");
  }
  // Link direction was allready set by onCbLinkDirectionCurrentIndexChanged() - check it
  if(key.linkDirectionFk.isNull()){
    errorList << tr("Link type is not set");
  }
  // Start and end connections are allready set afetr selection by user
  if(key.connectionStartFk.isNull()){
    errorList << tr("Start connection is not set");
  }
  if(key.connectionEndFk.isNull()){
    errorList << tr("End connection is not set");
  }
  // Store resistance
  pvLinkData.resistance = sbValue->value();
  
//   // Store and check link type
//   storeCurrentSelectedLinkType();
//   if(pvLinkData.value("LinkType_Code_FK").isNull()){
//     errorList << tr("Link type is not set");
//   }
//   // Store and check link direction
//   storeCurrentSelectedLinkDirection();
//   if(pvLinkData.value("LinkDirection_Code_FK").isNull()){
//     errorList << tr("Direction is not set");
//   }
//   // Store value
//   pvLinkData.setValue("Resistance", sbValue->value());
//   // Start and end connections are allready set afetr selection by user
//   if(pvLinkData.value("ArticleConnectionStart_Id_FK").isNull()){
//     errorList << tr("Start connection is not set");
//   }
//   if(pvLinkData.value("ArticleConnectionEnd_Id_FK").isNull()){
//     errorList << tr("End connection is not set");
//   }
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

// void mdtClArticleLinkDialog::storeCurrentSelectedLinkType()
// {
//   QModelIndex index;
//   int row;
// 
//   row = cbLinkType->currentIndex();
//   if(row < 0){
//     pvLinkData.setValue("LinkType_Code_FK", QVariant());
//     return;
//   }
//   index = pvLinkTypeModel->index(row, 0);
//   pvLinkData.setValue("LinkType_Code_FK", pvLinkTypeModel->data(index));
// }

// void mdtClArticleLinkDialog::storeCurrentSelectedLinkDirection()
// {
//   QModelIndex index;
//   int row;
// 
//   row = cbLinkDirection->currentIndex();
//   if(row < 0){
//     pvLinkData.setValue("LinkDirection_Code_FK", QVariant());
//     return;
//   }
//   index = pvLinkDirectionModel->index(row, 0);
//   pvLinkData.setValue("LinkDirection_Code_FK", pvLinkDirectionModel->data(index));
// }

void mdtClArticleLinkDialog::displayCurrentSelectedStartConnection()
{
  auto fk = pvLinkData.keyData().connectionStartFk;
  QModelIndex index;
  QVariant data;
  int row;

  if(fk.isNull()){
//   if(pvLinkData.value("ArticleConnectionStart_Id_FK").isNull()){
    lbStartConnectorName->clear();
    lbStartContactName->clear();
    return;
  }
  for(row = 0; row < pvArticleConnectionModel->rowCount(); ++row){
    index = pvArticleConnectionModel->index(row, 0);
    data = pvArticleConnectionModel->data(index);
    if(data == fk.id){
//     if(data == pvLinkData.value("ArticleConnectionStart_Id_FK")){
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
  auto fk = pvLinkData.keyData().connectionEndFk;
  QModelIndex index;
  QVariant data;
  int row;

  if(fk.isNull()){
//   if(pvLinkData.value("ArticleConnectionEnd_Id_FK").isNull()){
    lbEndConnectorName->clear();
    lbEndContactName->clear();
    return;
  }
  for(row = 0; row < pvArticleConnectionModel->rowCount(); ++row){
    index = pvArticleConnectionModel->index(row, 0);
    data = pvArticleConnectionModel->data(index);
    if(data == fk.id){
//     if(data == pvLinkData.value("ArticleConnectionEnd_Id_FK")){
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
