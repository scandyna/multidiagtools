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
#include "mdtClLinkEditor.h"
#include "mdtSqlWindow.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include "mdtError.h"
#include <QSqlTableModel>
#include <QSqlError>
#include <QPushButton>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QWidget>
#include <QHBoxLayout>
#include <QDataWidgetMapper>

mdtClLinkEditor::mdtClLinkEditor(QObject *parent, QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  // Setup Link
  pvLinkModel = new QSqlTableModel(this, pvDatabase);
  pvLinkWidget = new mdtSqlFormWidget;
  pvLinkUiWidget = new Ui::mdtClLinkEditor;
  pvLinkUiWidget->setupUi(pvLinkWidget);
  // Setup Article
  pvArticleWidgetMapper = new QDataWidgetMapper(this);
  pvArticleModel = new QSqlTableModel(this, pvDatabase);
  pvArticleLinkRelation = new mdtSqlRelation;

}

mdtClLinkEditor::~mdtClLinkEditor()
{
  delete pvArticleLinkRelation;
}

bool mdtClLinkEditor::setupTables()
{
  if(!setupLinkTable()){
    return false;
  }
  if(!setupArticleTable()){
    return false;
  }

  return true;
}

void mdtClLinkEditor::setupUi(mdtSqlWindow *window)
{
  Q_ASSERT(window != 0);

  window->setSqlWidget(pvLinkWidget);
  

  window->enableNavigation();
  window->enableEdition();
  window->resize(900, 500);
}

bool mdtClLinkEditor::setupLinkTable()
{
  QSqlError sqlError;

  pvLinkModel->setTable("Link_tbl");
  if(!pvLinkModel->select()){
    sqlError = pvLinkModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'Link_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    return false;
  }
  pvLinkWidget->setModel(pvLinkModel);
  pvLinkWidget->mapFormWidgets();

  return true;
}

bool mdtClLinkEditor::setupArticleTable()
{
  QSqlError sqlError;

  pvArticleModel->setTable("Article_tbl");
  if(!pvArticleModel->select()){
    sqlError = pvArticleModel->lastError();
    mdtError e(MDT_DATABASE_ERROR, "Unable to select data in table 'Article_tbl'", mdtError::Error);
    e.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(e, "mdtClLinkEditor");
    e.commit();
    return false;
  }
  /*
   * Setup Link <-> Article relation
   * Here we make it reversed,
   *  because the parent table is the Article table,
   *  but we need to display the article on witch
   *  current Link is based
   */
  pvArticleLinkRelation->setParentModel(pvLinkModel);
  pvArticleLinkRelation->setChildModel(pvArticleModel);
  if(!pvArticleLinkRelation->addRelation("Article_Id_FK", "Id_PK")){
    return false;
  }
  // Setup widget mapper
  pvArticleWidgetMapper->setModel(pvArticleModel);
  pvArticleWidgetMapper->addMapping(pvLinkUiWidget->leArticleCode, pvArticleModel->fieldIndex("ArticleCode"));
  pvArticleWidgetMapper->addMapping(pvLinkUiWidget->leArticleDesignationEN, pvArticleModel->fieldIndex("DesignationEN"));
  // We must catch link index changed + article model select to update widget mapper
  connect(pvLinkWidget, SIGNAL(currentRowChanged(int)), pvArticleLinkRelation, SLOT(setParentCurrentIndex(int)));
  connect(pvLinkWidget, SIGNAL(modelSelected()), pvArticleWidgetMapper, SLOT(toFirst()));
  // Force a update of relation
  pvArticleLinkRelation->setParentCurrentIndex(pvLinkWidget->currentRow());

  return true;
}

bool mdtClLinkEditor::setupStartUnitTables()
{
}

bool mdtClLinkEditor::setupEndUnitTables()
{
}

bool mdtClLinkEditor::setupListTable()
{
}
