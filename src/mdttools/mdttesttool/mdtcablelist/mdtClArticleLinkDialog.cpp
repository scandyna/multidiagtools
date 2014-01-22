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
#include "mdtClArticleLinkDialog.h"
#include "mdtSqlSelectionDialog.h"
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlField>
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
  // Setup data
  if(!pvLinkData.addAllFields("ArticleLink_tbl", pvDatabase)){
    /// \todo Check if this works
    QMessageBox msgBox;
    msgBox.setText(pvLinkData.lastError().text());
    msgBox.setDetailedText(pvLinkData.lastError().systemText());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(tr("Article link error"));
    msgBox.exec();
    reject();
    return;
  }
  // Setup UI
  setupUi(this);
  setWindowTitle(tr("Article link edition"));
  connect(pbStartConnection, SIGNAL(clicked()), this, SLOT(selectStartConnection()));
  connect(pbEndConnection, SIGNAL(clicked()), this, SLOT(selectEndConnection()));
  // Setup link type
  pvLinkTypeModel = new QSqlQueryModel(this);
  pvLinkTypeModel->setQuery("SELECT Code_PK, NameEN, ValueUnit FROM LinkType_tbl ORDER BY NameEN", pvDatabase);
  cbLinkType->setModel(pvLinkTypeModel);
  cbLinkType->setModelColumn(1);
  cbLinkType->setCurrentIndex(-1);
  connect(cbLinkType, SIGNAL(currentIndexChanged(int)), this, SLOT(onCbLinkTypeCurrentIndexChanged(int)));
  // Setup link direction
  pvLinkDirectionModel = new QSqlQueryModel(this);
  pvLinkDirectionModel->setQuery("SELECT Code_PK, NameEN FROM LinkDirection_tbl", pvDatabase);
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

void mdtClArticleLinkDialog::setLinkTypeCode(const QVariant & code)
{
  QModelIndex index;
  int row;
  QVariant data;

  ///pvLinkData.setLinkTypeCode(code);
  pvLinkData.setValue("LinkType_Code_FK", code);
  for(row = 0; row < pvLinkTypeModel->rowCount(); ++row){
    index = pvLinkTypeModel->index(row, 0);
    data = pvLinkTypeModel->data(index);
    if(data == code){
      cbLinkType->setCurrentIndex(row);
      return;
    }
  }
}

/**
QVariant mdtClArticleLinkDialog::linkTypeCode() const
{
  return pvLinkData.linkTypeCode();
}
*/

void mdtClArticleLinkDialog::setLinkDirectionCode(const QVariant & code)
{
  QModelIndex index;
  int row;
  QVariant data;

  ///pvLinkData.setLinkDirectionCode(code);
  pvLinkData.setValue("LinkDirection_Code_FK", code);
  for(row = 0; row < pvLinkDirectionModel->rowCount(); ++row){
    index = pvLinkDirectionModel->index(row, 0);
    data = pvLinkDirectionModel->data(index);
    if(data == code){
      cbLinkDirection->setCurrentIndex(row);
      return;
    }
  }
}

/**
QVariant mdtClArticleLinkDialog::linkDirectionCode() const
{
  return pvLinkData.linkDirectionCode();
}
*/


void mdtClArticleLinkDialog::setValue(const QVariant & value)
{
  ///pvLinkData.setValue(value);
  pvLinkData.setValue("Value", value);
  sbValue->setValue(value.toDouble());
}

/**
QVariant mdtClArticleLinkDialog::value() const
{
  return pvLinkData.value();
}
*/

void mdtClArticleLinkDialog::setStartConnectionId(const QVariant & id)
{
  ///pvLinkData.setArticleConnectionStartId(id);
  pvLinkData.setValue("ArticleConnectionStart_Id_FK", id);
  displayCurrentSelectedStartConnection();
}

/**
QVariant mdtClArticleLinkDialog::startConnectionId() const
{
  return pvLinkData.articleConnectionStartId();
}
*/

void mdtClArticleLinkDialog::setEndConnectionId(const QVariant & id)
{
  ///pvLinkData.setArticleConnectionEndId(id);
  pvLinkData.setValue("ArticleConnectionEnd_Id_FK", id);
  displayCurrentSelectedEndConnection();
}

/**
QVariant mdtClArticleLinkDialog::endConnectionId() const
{
  return pvLinkData.articleConnectionEndId();
}
*/

/**
mdtClLinkData mdtClArticleLinkDialog::linkData()
{
  return pvLinkData;
}
*/

mdtSqlRecord mdtClArticleLinkDialog::linkData() const
{
  return pvLinkData;
}


void mdtClArticleLinkDialog::onCbLinkTypeCurrentIndexChanged(int row)
{
  QModelIndex index;
  QVariant data;

  if(row < 0){
    lbUnit->setText("");
    return;
  }
  // We must update available directions regarding link type
  index = pvLinkTypeModel->index(row, 0);
  data = pvLinkTypeModel->data(index);
  if(data == QVariant("DIODE")){
    pvLinkDirectionModel->setQuery("SELECT Code_PK, NameEN, PictureAscii FROM LinkDirection_tbl WHERE Code_PK <> 'BID'", pvDatabase);
    cbLinkDirection->setEnabled(true);
    sbValue->setValue(0.7);
  }else{
    pvLinkDirectionModel->setQuery("SELECT Code_PK, NameEN, PictureAscii FROM LinkDirection_tbl WHERE Code_PK = 'BID'", pvDatabase);
    cbLinkDirection->setEnabled(false);
    sbValue->setValue(0.0);
  }
  // Update displayed unit (V, Ohm, ...)
  index = pvLinkTypeModel->index(row, 2);
  data = pvLinkTypeModel->data(index);
  lbUnit->setText("[" + data.toString() + "]");
}

void mdtClArticleLinkDialog::onCbLinkDirectionCurrentIndexChanged(int row)
{
  QModelIndex index;
  QVariant data;

  if(row < 0){
    lbLinkDirectionAsciiPicture->setText("");
    return;
  }
  // Update the ASCII picture
  index = pvLinkDirectionModel->index(row, 2);
  data = pvLinkDirectionModel->data(index);
  lbLinkDirectionAsciiPicture->setText(data.toString());
}

void mdtClArticleLinkDialog::selectStartConnection()
{
  mdtSqlSelectionDialog dialog(this);

  // Setup and show dialog
  dialog.setMessage(tr("Please select the start connection"));
  dialog.setModel(pvArticleConnectionModel, false);
  dialog.setColumnHidden("Id_PK", true);
  dialog.setHeaderData("ArticleConnectorName", "Connector");
  dialog.setHeaderData("ArticleContactName", "Contact");
  dialog.setHeaderData("IoType", "I/O type");
  dialog.setHeaderData("FunctionEN", "Function\n(English)");
  dialog.setHeaderData("FunctionFR", "Function\n(French)");
  dialog.setHeaderData("FunctionDE", "Function\n(German)");
  dialog.setHeaderData("FunctionIT", "Function\n(Italian)");
  dialog.addSelectionResultColumn("Id_PK");
  dialog.resize(800, 400);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store result
  Q_ASSERT(dialog.selectionResult().size() == 1);
  ///pvLinkData.setArticleConnectionStartId(dialog.selectionResult().at(0));
  pvLinkData.setValue("ArticleConnectionStart_Id_FK", dialog.selectionResult().at(0));
  // Update GUI
  displayCurrentSelectedStartConnection();
}

void mdtClArticleLinkDialog::selectEndConnection()
{
  mdtSqlSelectionDialog dialog(this);

  // Setup and show dialog
  dialog.setMessage(tr("Please select the end connection"));
  dialog.setModel(pvArticleConnectionModel, false);
  dialog.setColumnHidden("Id_PK", true);
  dialog.setHeaderData("ArticleConnectorName", "Connector");
  dialog.setHeaderData("ArticleContactName", "Contact");
  dialog.setHeaderData("IoType", "I/O type");
  dialog.setHeaderData("FunctionEN", "Function\n(English)");
  dialog.setHeaderData("FunctionFR", "Function\n(French)");
  dialog.setHeaderData("FunctionDE", "Function\n(German)");
  dialog.setHeaderData("FunctionIT", "Function\n(Italian)");
  dialog.addSelectionResultColumn("Id_PK");
  dialog.resize(800, 400);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store result
  Q_ASSERT(dialog.selectionResult().size() == 1);
  ///pvLinkData.setArticleConnectionEndId(dialog.selectionResult().at(0));
  pvLinkData.setValue("ArticleConnectionEnd_Id_FK", dialog.selectionResult().at(0));
  // Update GUI
  displayCurrentSelectedEndConnection();
}

void mdtClArticleLinkDialog::accept()
{
  QStringList errorList;
  QString infoText;
  int i;

  // Store and check link type
  storeCurrentSelectedLinkType();
  ///if(pvLinkData.linkTypeCode().isNull()){
  if(pvLinkData.value("LinkType_Code_FK").isNull()){
    errorList << tr("Link type is not set");
  }
  // Store and check link direction
  storeCurrentSelectedLinkDirection();
  ///if(pvLinkData.linkDirectionCode().isNull()){
  if(pvLinkData.value("LinkDirection_Code_FK").isNull()){
    errorList << tr("Direction is not set");
  }
  // Store value
  ///pvLinkData.setValue(sbValue->value());
  pvLinkData.setValue("Value", sbValue->value());
  // Start and end connections are allready set afetr selection by user
  ///if(pvLinkData.articleConnectionStartId().isNull()){
  if(pvLinkData.value("ArticleConnectionStart_Id_FK").isNull()){
    errorList << tr("Start connection is not set");
  }
  ///if(pvLinkData.articleConnectionEndId().isNull()){
  if(pvLinkData.value("ArticleConnectionEnd_Id_FK").isNull()){
    errorList << tr("End connection is not set");
  }
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

void mdtClArticleLinkDialog::storeCurrentSelectedLinkType()
{
  QModelIndex index;
  int row;

  row = cbLinkType->currentIndex();
  if(row < 0){
    ///pvLinkData.setLinkTypeCode(QVariant());
    pvLinkData.setValue("LinkType_Code_FK", QVariant());
    return;
  }
  index = pvLinkTypeModel->index(row, 0);
  ///pvLinkData.setLinkTypeCode(pvLinkTypeModel->data(index));
  pvLinkData.setValue("LinkType_Code_FK", pvLinkTypeModel->data(index));
}

void mdtClArticleLinkDialog::storeCurrentSelectedLinkDirection()
{
  QModelIndex index;
  int row;

  row = cbLinkDirection->currentIndex();
  if(row < 0){
    ///pvLinkData.setLinkDirectionCode(QVariant());
    pvLinkData.setValue("LinkDirection_Code_FK", QVariant());
    return;
  }
  index = pvLinkDirectionModel->index(row, 0);
  ///pvLinkData.setLinkDirectionCode(pvLinkDirectionModel->data(index));
  pvLinkData.setValue("LinkDirection_Code_FK", pvLinkDirectionModel->data(index));
}

void mdtClArticleLinkDialog::displayCurrentSelectedStartConnection()
{
  QModelIndex index;
  QVariant data;
  int row;

  ///if(pvLinkData.articleConnectionStartId().isNull()){
  if(pvLinkData.value("ArticleConnectionStart_Id_FK").isNull()){
    lbStartConnectorName->clear();
    lbStartContactName->clear();
    return;
  }
  for(row = 0; row < pvArticleConnectionModel->rowCount(); ++row){
    index = pvArticleConnectionModel->index(row, 0);
    data = pvArticleConnectionModel->data(index);
    if(data == pvLinkData.value("ArticleConnectionStart_Id_FK")){
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
  QModelIndex index;
  QVariant data;
  int row;

  ///if(pvLinkData.articleConnectionEndId().isNull()){
  if(pvLinkData.value("ArticleConnectionEnd_Id_FK").isNull()){
    lbEndConnectorName->clear();
    lbEndContactName->clear();
    return;
  }
  for(row = 0; row < pvArticleConnectionModel->rowCount(); ++row){
    index = pvArticleConnectionModel->index(row, 0);
    data = pvArticleConnectionModel->data(index);
    if(data == pvLinkData.value("ArticleConnectionEnd_Id_FK")){
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
