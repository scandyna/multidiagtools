/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtClArticleEditor.h"
#include "ui_mdtClArticleEditor.h"
#include "mdtSqlWindow.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtAbstractSqlWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtError.h"
#include "mdtClArticleLinkDialog.h"
#include "mdtClArticle.h"
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QStringList>
#include <QString>
#include <QList>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QTableView>

#include <QDebug>

mdtClArticleEditor::mdtClArticleEditor(QObject *parent, const QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  pvForm = new mdtSqlFormWindow;
}

mdtClArticleEditor::~mdtClArticleEditor()
{
  delete pvForm;
}

bool mdtClArticleEditor::setupTables(bool includeConnections)
{
  QSqlError sqlError;

  // Setup Unit table
  if(!setupArticleTable()){
    return false;
  }
  // Setup connection table
  if(!setupArticleConnectionTable()){
    return false;
  }
  // Setup link table
  if(!setupArticleLinkTable()){
    return false;
  }

  return true;
}

mdtSqlFormWindow *mdtClArticleEditor::form()
{
  Q_ASSERT(pvForm != 0);

  return pvForm;
}

void mdtClArticleEditor::addLink()
{
  mdtClArticleLinkDialog dialog(0, pvDatabase, currentArticleId());
  mdtClArticle art(pvDatabase);

  // Check if some connection exists
  if(pvForm->rowCount("ArticleConnection_tbl") < 1){
    QMessageBox msgBox;
    msgBox.setText(tr("There is no connection available for current article"));
    msgBox.setInformativeText(tr("You must add connections to be able to link them"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }
  // Setup and show dialog
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Add link
  if(!art.addLink(dialog.startConnectionId(), dialog.endConnectionId(), dialog.value().toDouble(), dialog.linkDirectionCode(), dialog.linkTypeCode())){
    QMessageBox msgBox;
    msgBox.setText(tr("Link insertion failed"));
    msgBox.setInformativeText(tr("Please see details for more informations"));
    msgBox.setDetailedText(art.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update link table
  pvForm->select("ArticleLink_view");
}

void mdtClArticleEditor::editLink()
{
  mdtClArticleLinkDialog dialog(0, pvDatabase, currentArticleId());
  int row;
  mdtAbstractSqlWidget *widget;
  mdtClArticle art(pvDatabase);

  widget = pvForm->sqlWidget("ArticleLink_view");
  Q_ASSERT(widget != 0);
  // Check that a link is selected
  row = widget->rowCount();
  if(row < 0){
    return;
  }
  // Setup and show dialog
  dialog.setLinkTypeCode(widget->currentData("LinkType_Code_FK"));
  dialog.setLinkDirectionCode(widget->currentData("LinkDirection_Code_FK"));
  dialog.setValue(widget->currentData("Value"));
  dialog.setStartConnectionId(widget->currentData("ArticleConnectionStart_Id_FK"));
  dialog.setEndConnectionId(widget->currentData("ArticleConnectionEnd_Id_FK"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Edit link
  if(!art.editLink(widget->currentData("Id_PK") ,dialog.startConnectionId(), dialog.endConnectionId(), dialog.value().toDouble(), dialog.linkDirectionCode(), dialog.linkTypeCode())){
    QMessageBox msgBox;
    msgBox.setText(tr("Link insertion failed"));
    msgBox.setInformativeText(tr("Please see details for more informations"));
    msgBox.setDetailedText(art.lastError().text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update link table
  pvForm->select("ArticleLink_view");
}

void mdtClArticleEditor::removeLinks()
{
  mdtSqlTableWidget *widget;
  mdtClArticle art(pvDatabase);
  QMessageBox msgBox;
  QModelIndexList indexes;
  QSqlError sqlError;
  int ret;

  widget = pvForm->sqlTableWidget("ArticleLink_view");
  Q_ASSERT(widget != 0);
  // Get selected rows
  indexes = widget->indexListOfSelectedRows("Id_PK");
  if(indexes.size() < 1){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove links for current article."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  if(ret != QMessageBox::Yes){
    return;
  }
  // Delete seleced rows
  if(!art.removeLinks(indexes)){
    sqlError = art.lastError();
    QMessageBox msgBox;
    msgBox.setText(tr("Links removing failed."));
    ///msgBox.setInformativeText(tr("Please check if connect"));
    msgBox.setDetailedText(sqlError.text());
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return;
  }
  // Update link table
  pvForm->select("ArticleLink_view");
}

QVariant mdtClArticleEditor::currentArticleId()
{
  return pvForm->currentData("Article_tbl", "Id_PK");
}

bool mdtClArticleEditor::setupArticleTable()
{
  Ui::mdtClArticleEditor ae;

  // Setup main form widget
  ae.setupUi(pvForm->mainSqlWidget());
  ///connect(this, SIGNAL(unitEdited()), pvForm->mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!pvForm->setTable("Article_tbl", "Article", pvDatabase)){
    return false;
  }
  pvForm->sqlWindow()->enableNavigation();
  pvForm->sqlWindow()->enableEdition();
  pvForm->sqlWindow()->resize(800, 500);
  pvForm->sqlWindow()->setWindowTitle(tr("Article edition"));

  return true;
}

bool mdtClArticleEditor::setupArticleConnectionTable()
{
  mdtSqlTableWidget *widget;

  if(!pvForm->addChildTable("ArticleConnection_tbl", tr("Connections"), pvDatabase)){
    return false;
  }
  if(!pvForm->addRelation("Id_PK", "ArticleConnection_tbl", "Article_Id_FK")){
    return false;
  }
  widget = pvForm->sqlTableWidget("ArticleConnection_tbl");
  Q_ASSERT(widget != 0);
  widget->enableLocalEdition();
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Article_Id_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("ArticleConnectorName", tr("Connector"));
  widget->setHeaderData("ArticleContactName", tr("Contact"));
  widget->setHeaderData("IoType", tr("I/O type"));
  widget->setHeaderData("FunctionEN", tr("Function EN"));

  return true;
}

bool mdtClArticleEditor::setupArticleLinkTable()
{
  mdtSqlTableWidget *widget;
  QPushButton *pbAddLink;
  QPushButton *pbEditLink;
  QPushButton *pbRemoveLinks;

  if(!pvForm->addChildTable("ArticleLink_view", tr("Links"), pvDatabase)){
    return false;
  }
  if(!pvForm->addRelation("Id_PK", "ArticleLink_view", "StartArticle_Id_FK")){
    return false;
  }
  if(!pvForm->addRelation("Id_PK", "ArticleLink_view", "EndArticle_Id_FK")){
    return false;
  }
  widget = pvForm->sqlTableWidget("ArticleLink_view");
  Q_ASSERT(widget != 0);
  Q_ASSERT(widget->tableView() != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("StartArticle_Id_FK", true);
  widget->setColumnHidden("EndArticle_Id_FK", true);
  widget->setColumnHidden("LinkType_Code_FK", true);
  widget->setColumnHidden("LinkDirection_Code_FK", true);
  widget->setColumnHidden("ArticleConnectionStart_Id_FK", true);
  widget->setColumnHidden("ArticleConnectionEnd_Id_FK", true);
  widget->setColumnHidden("UnitConnectionStart_Id_FK", true);
  widget->setColumnHidden("UnitConnectionEnd_Id_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("SinceVersion", tr("Since\nversion"));
  widget->setHeaderData("LinkTypeNameEN", tr("Link type"));
  widget->setHeaderData("ValueUnit", tr("Unit"));
  widget->setHeaderData("StartArticleConnectorName", tr("Start\nconnector"));
  widget->setHeaderData("StartArticleContactName", tr("Start\ncontact"));
  widget->setHeaderData("StartIoType", tr("Start\nI/O type"));
  widget->setHeaderData("StartFunctionEN", tr("Start\nFunction"));
  widget->setHeaderData("LinkDirectionPictureAscii", tr("Direction"));
  widget->setHeaderData("EndArticleConnectorName", tr("End\nconnector"));
  widget->setHeaderData("EndArticleContactName", tr("End\ncontact"));
  widget->setHeaderData("EndIoType", tr("End\nI/O type"));
  widget->setHeaderData("EndFunctionEN", tr("End\nFunction"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add edition buttons
  pbAddLink = new QPushButton(tr("Add link ..."));
  connect(pbAddLink, SIGNAL(clicked()), this, SLOT(addLink()));
  widget->addWidgetToLocalBar(pbAddLink);
  pbEditLink = new QPushButton(tr("Edit link ..."));
  connect(pbEditLink, SIGNAL(clicked()), this, SLOT(editLink()));
  widget->addWidgetToLocalBar(pbEditLink);
  pbRemoveLinks = new QPushButton(tr("Remove links"));
  connect(pbRemoveLinks, SIGNAL(clicked()), this, SLOT(removeLinks()));
  widget->addWidgetToLocalBar(pbRemoveLinks);
  widget->addStretchToLocalBar();

  return true;
}
