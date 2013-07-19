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
#ifndef MDT_CL_LINK_EDITOR_H
#define MDT_CL_LINK_EDITOR_H

#include "ui_mdtClLinkEditor.h"
#include <QSqlDatabase>
#include <QObject>
#include <QVariant>

class mdtSqlWindow;
class mdtSqlFormWidget;
class mdtSqlTableWidget;
class mdtSqlRelation;
class QSqlTableModel;
class QPushButton;
class QDataWidgetMapper;
class mdtClLinkEditor;

/*! \brief Cable list's link editor
 */
class mdtClLinkEditor : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClLinkEditor(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

  /*! \brief Destructor
   */
  ~mdtClLinkEditor();

  /*! \brief Setup tables
   */
  bool setupTables();

  /*! \brief Setup GUI
   *
   * Will dispose internal widgets into window
   *
   * \pre window must be a valid pointer
   */
  void setupUi(mdtSqlWindow *window);

 private slots:

  /*! \brief Connect current link between 2 unit connections
   */
  ///void connectLink();

  /*! \brief Select the Start connection
   */
  void selectStartConnection();

  /*! \brief Select the End connection
   */
  void selectEndConnection();

  /*! \brief Update widgets that display Start and End unit and connections
   */
  void updateUnitConnectionCurrent(int linkRow);

  /*! \brief Update List view
   */
  void updateListView();

  /*! \brief Update current row (Link widget) regarding selected row in List widget
   */
  void setCurrentRowByList(int listRow);

 signals:

  /*! \brief Used to tell Link widget that we have edited some data
   */
  void linkEdited();

 private:

  /*! \brief Setup Link table
   */
  bool setupLinkTable();

  /*! \brief Setup Article table
   */
  bool setupArticleTable();

  /*! \brief Setup Unit and UnitConnection tables for start point
   */
  bool setupStartUnitTables();

  /*! \brief Setup Unit and UnitConnection tables for end point
   */
  bool setupEndUnitTables();

  /*! \brief Setup list table
   */
  bool setupListTable();

  /*! \brief Get model index row for given value that must match in given field
   */
  int modelIndexRowForValue(QSqlTableModel *model, const QString &fieldName, const QVariant &value);

  /*! \brief Clear content of mapped widget
   */
  void clearWidgetContents(QDataWidgetMapper *mapper);

  Q_DISABLE_COPY(mdtClLinkEditor);

  QSqlDatabase pvDatabase;
  // Link members
  mdtSqlFormWidget *pvLinkWidget;
  Ui::mdtClLinkEditor *pvLinkUiWidget;
  QSqlTableModel *pvLinkModel;
  // Article members
  QDataWidgetMapper *pvArticleWidgetMapper;
  QSqlTableModel *pvArticleModel;
  mdtSqlRelation *pvArticleLinkRelation;
  // Start Unit members
  QSqlTableModel *pvStartUnitModel;
  QDataWidgetMapper *pvStartUnitWidgetMapper;
  QSqlTableModel *pvStartUnitConnectionModel;
  QDataWidgetMapper *pvStartUnitConnectionWidgetMapper;
  // End Unit members
  QSqlTableModel *pvEndUnitModel;
  QDataWidgetMapper *pvEndUnitWidgetMapper;
  QSqlTableModel *pvEndUnitConnectionModel;
  QDataWidgetMapper *pvEndUnitConnectionWidgetMapper;
  // List members
  mdtSqlTableWidget *pvListWidget;
  QSqlTableModel *pvListModel;
};

#endif // #ifndef MDT_CL_LINK_EDITOR_H
